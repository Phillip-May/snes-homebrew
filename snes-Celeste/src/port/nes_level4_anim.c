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

// NMI-driven flush: build a neslib VRAM update buffer during CPU free time,
// then set_vram_update() hands it to the NMI handler for guaranteed vblank timing.
// Delta data in ROM has pre-computed [ppu_hi, ppu_lo, tile] triples — same as
// neslib's non-sequential update format. Just copy + append 0xFF EOF.
//
// IMPORTANT: clear set_vram_update(0) BEFORE modifying buffer to prevent
// NMI reading half-written data (race condition).
#define LA_WRITES_PER_NMI 8  // 8 × ~40 = ~320 cycles in NMI, leaves room for post-NMI BG tile writes
#define LA_VRAM_BUF_SIZE (LA_WRITES_PER_NMI * 3 + 68)  // tiles + attr horizontal run + EOF
static uint8_t s_laVramBuf[LA_VRAM_BUF_SIZE];

// Multi-frame flush state
static const unsigned char *s_laDeltaPtr;
static uint8_t s_laDeltaRemaining;
static const unsigned char *s_laDeltaAttrPtr;

#define LEVEL4_ANIM_BANK 4
#define LEVEL4_ANIM_SPEED 6  // 30fps / 6 = 5fps (8 writes/NMI needs ~11 NMIs to flush 77 changes)

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

// Prepare delta — store ROM pointer and count. First chunk is built immediately.
__attribute__((section(".prg_rom_4.text"), noinline))
static void la_prepare_delta(const unsigned char *delta) {
    uint16_t count = delta[0] | ((uint16_t)delta[1] << 8);
    s_laDeltaPtr = delta + 2;  // Points to first [ppu_hi, ppu_lo, tile] triple
    s_laDeltaRemaining = (uint8_t)count;
    s_laDeltaAttrPtr = delta + 2 + count * 3;  // Attribute data follows tile changes
    s_delta_active = 1;
}

// Build the next VRAM update buffer chunk from the current delta position.
// Called from bank 4 during advance, copies ROM data to RAM buffer for NMI.
__attribute__((section(".prg_rom_4.text"), noinline))
static void la_build_next_vram_chunk(void) {
    // CRITICAL: clear update pointer FIRST to prevent NMI reading half-written buffer
    set_vram_update(0);

    if (s_laDeltaRemaining > 0) {
        uint8_t n = (s_laDeltaRemaining > LA_WRITES_PER_NMI) ? LA_WRITES_PER_NMI : s_laDeltaRemaining;
        const unsigned char *p = s_laDeltaPtr;
        uint8_t *buf = s_laVramBuf;
        for (uint8_t i = 0; i < n; i++) {
            *buf++ = *p++;
            *buf++ = *p++;
            *buf++ = *p++;
        }
        *buf = 0xFF;  // NT_UPD_EOF
        s_laDeltaPtr = p;
        s_laDeltaRemaining -= n;
        // Buffer complete — now safe to register for NMI
        set_vram_update(s_laVramBuf);
    } else if (s_laDeltaAttrPtr) {
        uint8_t *buf = s_laVramBuf;
        *buf++ = 0x23 | 0x40;  // NT_UPD_HORZ
        *buf++ = 0xC0;
        *buf++ = 64;
        const unsigned char *a = s_laDeltaAttrPtr;
        for (uint8_t i = 0; i < 64; i++) *buf++ = a[i];
        *buf = 0xFF;
        s_laDeltaAttrPtr = NULL;
        set_vram_update(s_laVramBuf);
    } else {
        s_delta_active = 0;
    }
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
    extern const unsigned char game_chr_data[];

    s_levelAnimVariant = 0;
    s_levelAnimFrame = 0;
    s_levelAnimTimer = 0;
    s_laDeltaRemaining = 0;
    s_laDeltaAttrPtr = NULL;
    set_vram_update(0);  // Clear any pending NMI updates

    // Write animation BG tiles to $1000 (sprite pattern table area).
    // During level 4, PPU_CTRL bit 4 is set so BG reads from $1000.
    // Game sprites continue reading from $0000 (untouched).
    vram_adr(0x1000);
    for (uint16_t i = 0; i < sizeof(level4_anim_chr_tiles); i++) {
        vram_put(level4_anim_chr_tiles[i]);
    }

    // Collapse tile CHR patterns are copied by port_LoadRoomData (fixed bank)
    // before this function is called, since we can't switch PRG banks from bank 4.

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

    // If a delta is being flushed, build the next VRAM update chunk for the NMI
    if (s_delta_active) {
        la_build_next_vram_chunk();
        return;
    }

    s_levelAnimTimer++;
    if (s_levelAnimTimer >= LEVEL4_ANIM_SPEED) {
        s_levelAnimTimer = 0;
        if (s_levelAnimVariant == 0)
            la_prepare_delta(&level4_anim_delta_a[level4_anim_delta_offsets_a[s_levelAnimFrame]]);
        else
            la_prepare_delta(&level4_anim_delta_b[level4_anim_delta_offsets_b[s_levelAnimFrame]]);
        la_build_next_vram_chunk();  // Build first chunk immediately
        s_levelAnimFrame++;
        if (s_levelAnimFrame >= LEVEL4_ANIM_FRAME_COUNT) s_levelAnimFrame = 0;
    }
}

// port_levelAnimFlush is no longer needed — NMI handles VRAM updates via set_vram_update.
// Kept as a no-op stub for compatibility with port_vblank's call.
__attribute__((noinline))
void port_levelAnimFlush(void) {
    // NMI processes the VRAM update buffer automatically.
    // Nothing to do here.
}

// --- Fixed bank trampoline ---

__attribute__((noinline))
void port_levelAnimAdvance(void) {
    if (!s_levelAnimActive) return;
    set_prg_bank(LEVEL4_ANIM_BANK);
    la_advance_bank4();  // Builds next VRAM chunk when flushing, or starts new delta
    set_prg_bank(0);
}
