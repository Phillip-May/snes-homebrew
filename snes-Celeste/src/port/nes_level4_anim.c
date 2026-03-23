// Level 4 animation — separate compilation unit (no LTO) to respect bank section attributes.
#define __NES__
#include "port.h"
#include <stdint.h>
#include <string.h>
#include <mapper.h>
#ifndef _WIN32
#include <neslib.h>
#endif

#include "../../python/level4_anim_data.h"

#define PPU_CTRL   (*(volatile uint8_t *)0x2000)
#define PPU_STATUS (*(volatile uint8_t *)0x2002)
#define PPU_ADDR   (*(volatile uint8_t *)0x2006)
#define PPU_DATA   (*(volatile uint8_t *)0x2007)

// Shared state with nes.c (defined there, extern here)
extern volatile uint8_t s_inputState;
extern uint8_t palette_ram[32];

// Title screen shared variables (reused for level animation)
extern uint8_t  s_ppu_write_count;
extern uint8_t  s_ppu_write_cursor;
extern uint8_t  s_attr_buf[64];
extern uint8_t  s_delta_active;
extern uint8_t  s_flush_rate;

// Level animation state
uint8_t s_levelAnimActive;
uint8_t s_levelAnimVariant;
uint8_t s_levelAnimFrame;
uint8_t s_levelAnimTimer;
uint8_t s_laSelectPrev;


extern struct sPlayerData GLOBAL_PlayerData;
extern OBJ_DATA GLOBAL_OBJList[];

// PPU delta write buffers in main RAM. Sized to fit within available RAM.
#define LA_MAX_BUF 40
static uint8_t s_laBufHi[LA_MAX_BUF];
static uint8_t s_laBufLo[LA_MAX_BUF];
static uint8_t s_laBufTile[LA_MAX_BUF];
#define LA_BUF_HI   s_laBufHi
#define LA_BUF_LO   s_laBufLo
#define LA_BUF_TILE  s_laBufTile

#define LEVEL4_ANIM_BANK 4
#define LEVEL4_ANIM_SPEED 5
#define LA_MAX_WRITES_PER_VBLANK 34
#define LA_ATTR_MERGE_THRESHOLD 20

// --- Bank 4 functions ---

__attribute__((section(".prg_rom_4.text"), noinline))
static void la_decode_rle(const unsigned char *rle, uint16_t addr, uint16_t max) {
    vram_adr(addr);
    uint16_t written = 0, idx = 0;
    while (written < max) {
        uint8_t cnt = rle[idx++];
        if (cnt == 0xFF) break;
        uint8_t tile = rle[idx++];
        for (uint8_t i = 0; i <= cnt && written < max; i++, written++)
            vram_put(tile);
    }
}

__attribute__((section(".prg_rom_4.text"), noinline))
static void la_write_attrs(uint16_t addr, const unsigned char *a) {
    vram_adr(addr);
    for (uint8_t i = 0; i < 64; i++) vram_put(a[i]);
}

__attribute__((section(".prg_rom_4.text"), noinline))
static void la_load_frame0(uint8_t v) {
    if (v == 0) {
        la_decode_rle(level4_anim_nt0_rle_a, NTADR_A(0,0), 960);
        la_write_attrs(0x23C0, level4_anim_attr0_nt0_a);
        la_decode_rle(level4_anim_nt2_rle_a, 0x2800, 960);
        la_write_attrs(0x2BC0, level4_anim_attr0_nt2_a);
    } else {
        la_decode_rle(level4_anim_nt0_rle_b, NTADR_A(0,0), 960);
        la_write_attrs(0x23C0, level4_anim_attr0_nt0_b);
        la_decode_rle(level4_anim_nt2_rle_b, 0x2800, 960);
        la_write_attrs(0x2BC0, level4_anim_attr0_nt2_b);
    }
}

// Prepare all tile changes from a delta into the PPU buffer (single shot, buffer is large enough)
__attribute__((section(".prg_rom_4.text"), noinline))
static void la_prepare_delta(const unsigned char *delta) {
    uint16_t idx = 0;
    uint16_t count = delta[idx] | ((uint16_t)delta[idx+1] << 8);
    idx += 2;
    uint8_t *bh = LA_BUF_HI, *bl = LA_BUF_LO, *bt = LA_BUF_TILE;
    uint8_t n = (count > LA_MAX_BUF) ? LA_MAX_BUF : (uint8_t)count;
    for (uint8_t i = 0; i < n; i++) {
        uint16_t pos = delta[idx] | ((uint16_t)delta[idx+1] << 8);
        uint8_t tile = delta[idx+2]; idx += 3;
        uint16_t a = (pos < 960) ? (0x2000 + pos) : (0x2800 + pos - 960);
        bh[i] = (uint8_t)(a >> 8); bl[i] = (uint8_t)a; bt[i] = tile;
    }
    if (count > n) idx += (count - n) * 3;
    s_ppu_write_count = n;
    s_ppu_write_cursor = 0;
    s_flush_rate = LA_MAX_WRITES_PER_VBLANK;
    for (uint8_t i = 0; i < 64; i++) s_attr_buf[i] = delta[idx++];
    s_delta_active = 1;
}

__attribute__((section(".prg_rom_4.text"), noinline))
static void la_apply_delta_full(const unsigned char *delta) {
    uint16_t idx = 0;
    uint16_t count = delta[idx] | ((uint16_t)delta[idx+1] << 8);
    idx += 2;
    for (uint16_t i = 0; i < count; i++) {
        uint16_t pos = delta[idx] | ((uint16_t)delta[idx+1] << 8);
        uint8_t tile = delta[idx+2]; idx += 3;
        uint16_t a = (pos < 960) ? (0x2000 + pos) : (0x2800 + pos - 960);
        PPU_ADDR = (uint8_t)(a >> 8); PPU_ADDR = (uint8_t)a; PPU_DATA = tile;
    }
    PPU_ADDR = 0x23; PPU_ADDR = 0xC0;
    for (uint8_t i = 0; i < 64; i++) PPU_DATA = delta[idx++];
    // Skip NT2 attributes (written once at init)
}

__attribute__((section(".prg_rom_4.text"), noinline))
void la_init_bank4(void) {
    s_levelAnimVariant = 0;
    s_levelAnimFrame = 0;
    s_levelAnimTimer = 0;
    // Write animation BG tiles to $1000 (sprite pattern table area).
    // During level 4, PPU_CTRL bit 4 is set so BG reads from $1000.
    // Game sprites continue reading from $0000 (untouched).
    vram_adr(0x1000);  // Write to second pattern table
    for (uint16_t i = 0; i < sizeof(level4_anim_chr_tiles); i++) {
        vram_put(level4_anim_chr_tiles[i]);
    }

    la_load_frame0(0);
    for (uint8_t i = 0; i < 16; i++) palette_ram[i] = level4_anim_bg_palette[i];
    pal_bg(palette_ram);
}

__attribute__((section(".prg_rom_4.text"), noinline))
void la_advance_bank4(void) {
    uint8_t sel = s_inputState & PORT_INPUT_SELECT_MASK;
    uint8_t prev = s_laSelectPrev & PORT_INPUT_SELECT_MASK;
    s_laSelectPrev = s_inputState;
    if (sel && !prev) {
        s_levelAnimVariant ^= 1;
        s_delta_active = 0;
        ppu_off();
        la_load_frame0(s_levelAnimVariant);
        for (uint8_t f = 0; f < s_levelAnimFrame; f++) {
            if (s_levelAnimVariant == 0)
                la_apply_delta_full(&level4_anim_delta_a[level4_anim_delta_offsets_a[f]]);
            else
                la_apply_delta_full(&level4_anim_delta_b[level4_anim_delta_offsets_b[f]]);
        }
        ppu_on_all();
    }

    if (s_delta_active) return;  // Wait for flush to complete

    s_levelAnimTimer++;
    if (s_levelAnimTimer >= LEVEL4_ANIM_SPEED) {
        s_levelAnimTimer = 0;
        if (s_levelAnimVariant == 0)
            la_prepare_delta(&level4_anim_delta_a[level4_anim_delta_offsets_a[s_levelAnimFrame]]);
        else
            la_prepare_delta(&level4_anim_delta_b[level4_anim_delta_offsets_b[s_levelAnimFrame]]);
        s_levelAnimFrame++;
        if (s_levelAnimFrame >= LEVEL4_ANIM_FRAME_COUNT) s_levelAnimFrame = 0;
        if (s_levelAnimFrame == 0) {
            s_flush_rate = (s_ppu_write_count + LEVEL4_ANIM_SPEED - 1) / LEVEL4_ANIM_SPEED;
            if (s_flush_rate < 1) s_flush_rate = 1;
        }
    }
}

// --- Bank 6 function (called from port_vblank when bank 6 is active) ---

__attribute__((section(".prg_rom_6.text"), noinline))
void port_levelAnimFlush(void) {
    if (!s_levelAnimActive || !s_delta_active) return;
    (void)PPU_STATUS;
    uint8_t write_attrs = 0;
    if (s_delta_active == 1) {
        const uint8_t *bh = LA_BUF_HI, *bl = LA_BUF_LO, *bt = LA_BUF_TILE;
        uint8_t cur = s_ppu_write_cursor, rem = s_ppu_write_count - cur;
        uint8_t rate = s_flush_rate;
        uint8_t batch = (rem > rate) ? rate : rem;
        uint8_t end = cur + batch;
        for (uint8_t i = cur; i < end; i++) {
            PPU_ADDR = bh[i]; PPU_ADDR = bl[i]; PPU_DATA = bt[i];
        }
        s_ppu_write_cursor = end;
        if (end >= s_ppu_write_count) {
            if (batch <= LA_ATTR_MERGE_THRESHOLD) write_attrs = 1;
            else { s_delta_active = 2; return; }
        }
    } else write_attrs = 1;
    if (write_attrs) {
        PPU_ADDR = 0x23; PPU_ADDR = 0xC0;
        for (uint8_t i = 0; i < 64; i++) PPU_DATA = s_attr_buf[i];
        // NT2 attributes written once at init, not updated per-delta
        s_delta_active = 0;
    }
}

// --- Fixed bank trampoline ---

__attribute__((noinline))
void port_levelAnimAdvance(void) {
    if (!s_levelAnimActive || s_delta_active) return;
    set_prg_bank(LEVEL4_ANIM_BANK);
    la_advance_bank4();
    set_prg_bank(0);
}
