// Define __NES__ before including headers to get NES-specific enum values
#define __NES__

#include "port.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <mapper.h>
// FamiStudio Sound Engine - raw entry points (6502 register calling convention)
extern void famistudio_init(void);       // X=data_lo, Y=data_hi
extern void famistudio_music_play(void); // A=song_index
extern void famistudio_music_stop(void);
extern void famistudio_update(void);

// C wrappers that set up 6502 registers for the engine's calling convention
static void fs_init(const unsigned char *data) {
    // famistudio_init wants X=lo, Y=hi of data pointer
    uint8_t lo = (uint8_t)((uint16_t)data);
    uint8_t hi = (uint8_t)((uint16_t)data >> 8);
    __asm__ volatile(
        "ldx %0\n"
        "ldy %1\n"
        "jsr famistudio_init\n"
        : : "r"(lo), "r"(hi) : "a", "x", "y"
    );
}

static void fs_play(uint8_t song) {
    // famistudio_music_play wants A=song index
    __asm__ volatile(
        "lda %0\n"
        "jsr famistudio_music_play\n"
        : : "r"(song) : "a", "x", "y"
    );
}

// sprite_animation_enums_nes.h is included by port.h when __NES__ is defined
#include "../../python/gid_to_tile_shared.h"
#include "../../python/gid_to_tile_collapse.h"
#include "../../python/gid_to_tile_breakable_wall.h"
#include "../../python/gid_to_tile_monument.h"
#include "../../python/gid_to_tile_big_chest.h"
#include "../../python/compression_dict_shared.h"
#include "../../python/object_sprite_dict_shared_nes.h"
#include "../../python/tilemap_level1_nes.h"
#include "../../python/tilemap_level2_nes.h"
#include "../../python/tilemap_level3_nes.h"
#include "../../python/tilemap_level4_nes.h"
#include "../../python/tilemap_level5_nes.h"
#include "../../python/tilemap_level6_nes.h"
#include "../../python/tilemap_level7_nes.h"
#include "../../python/tilemap_level8_nes.h"
#include "../../python/tilemap_level9_nes.h"
#include "../../python/tilemap_level10_nes.h"
#include "../../python/tilemap_level11_nes.h"
#include "../../python/tilemap_level12_nes.h"
#include "../../python/tilemap_level13_nes.h"
#include "../../python/tilemap_level14_nes.h"
#include "../../python/tilemap_level15_nes.h"
#include "../../python/tilemap_level16_nes.h"
#include "../../python/tilemap_level17_nes.h"
#include "../../python/tilemap_level18_nes.h"
#include "../../python/tilemap_level19_nes.h"
#include "../../python/tilemap_level20_nes.h"
#include "../../python/tilemap_level21_nes.h"
#include "../../python/tilemap_level22_nes.h"
#include "../../python/tilemap_level23_nes.h"
#include "../../python/tilemap_level24_nes.h"
#include "../../python/tilemap_level25_nes.h"
#include "../../python/tilemap_level26_nes.h"
#include "../../python/tilemap_level27_nes.h"
#include "../../python/tilemap_level28_nes.h"
#include "../../python/tilemap_level29_nes.h"
#include "../../python/tilemap_level30_nes.h"
#include "../../python/tilemap_level31_nes.h"
#include "../../python/title_screen_data.h"
// level4_anim_data.h is included only in nes_level4_anim.c (non-LTO TU)

#ifndef _WIN32 //Fix linter
#include <neslib.h>
#endif

#define PPU_CTRL   (*(volatile uint8_t *)0x2000)
#define PPU_MASK   (*(volatile uint8_t *)0x2001)
#define PPU_STATUS (*(volatile uint8_t *)0x2002)
#define OAM_ADDR   (*(volatile uint8_t *)0x2003)
#define OAM_DATA   (*(volatile uint8_t *)0x2004)
#define OAM_DMA    (*(volatile uint8_t *)0x4014)
#define PPU_SCROLL (*(volatile uint8_t *)0x2005)
#define PPU_ADDR   (*(volatile uint8_t *)0x2006)
#define PPU_DATA   (*(volatile uint8_t *)0x2007)

extern uint8_t OAM_BUF[];

volatile uint8_t s_inputState = 0;  // Non-static: shared with nes_level4_anim.c
static uint8_t s_scrollY = 0; // Vertical scroll position
static uint8_t s_frameCounter = 0; // For screenshake pattern

// Sprite flickering via alternating OAM write direction.
// Player sprites always occupy OAM slots 0-3 (bytes 0-15).
// Even frames: objects write forward from low OAM slots (first object = highest priority).
// Odd frames:  objects write backward from high OAM slots (first object = lowest priority).
// This flips OAM priority order every frame, giving even 50/50 flicker when
// the hardware 8-per-scanline limit forces sprite drops.
#define OAM_PLAYER_END 16   // Player uses bytes 0-15 (4 sprites)
#define OAM_LAST_SLOT  252  // Last valid OAM byte offset (slot 63)
static uint8_t s_oamCursor = OAM_PLAYER_END;
static int8_t  s_oamDirection = 4;  // +4 = forward, -4 = backward
static uint8_t s_rotateOffset = 0;

// Bitmask of pending background tile object updates (collapse tiles, breakable walls, etc.)
// One bit per object slot (supports all 29 objects in 4 bytes, saves RAM vs array)
static uint32_t s_pendingBgTileMask = 0;

// Pre-calculated VRAM write operations for background tile objects
// Only 1 processed per frame, static buffer replaces 390-byte stack allocation
#define MAX_BG_TILE_WRITES 2
typedef struct {
    uint16_t addr_top;
    uint16_t addr_bottom;
    uint16_t addr_row2;
    uint16_t addr_row3;
    const unsigned char *tile_data;
    uint8_t is_4x4;
} BgTileWrite;

extern struct sActiveLevelData GLOBAL_ActiveLevel;
extern struct sPlayerData GLOBAL_PlayerData;
extern OBJ_DATA GLOBAL_OBJList[];

// Palette data stored in RAM (for background and sprite palettes)
// Lower 16 bytes (0-15): 4 background palettes * 4 colors
// Upper 16 bytes (16-31): 4 sprite palettes * 4 colors
uint8_t palette_ram[32];  // Non-static: shared with nes_level4_anim.c

// Fixed level dimensions (all levels are 16x16 = 256 tiles)
#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 16
#define LEVEL_TILE_COUNT 256
#define LEVEL_BG_PALETTE_COUNT 4

typedef struct {
    const unsigned char *tilemap_compressed;
    const unsigned char *objects;
    uint16_t object_count;
    const unsigned char (*bg_palettes)[4];
    const unsigned char (*sprite_palettes)[4];
    uint8_t spawn_x;
    uint8_t spawn_y;
} LevelData;

__attribute__((section(".prg_rom_1")))
static const LevelData level_data[] = {
    // Level 1
    {
        tilemap_level1_compressed,
        object_level1, OBJECT_LEVEL1_COUNT,
        palette_background_level1,
        palette_sprite_level1,
        SPAWN_X_LEVEL1, SPAWN_Y_LEVEL1
    },
    // Level 2
    {
        tilemap_level2_compressed,
        object_level2, OBJECT_LEVEL2_COUNT,
        palette_background_level2,
        palette_sprite_level2,
        SPAWN_X_LEVEL2, SPAWN_Y_LEVEL2
    },
    // Level 3
    {
        tilemap_level3_compressed,
        object_level3, OBJECT_LEVEL3_COUNT,
        palette_background_level3,
        palette_sprite_level3,
        SPAWN_X_LEVEL3, SPAWN_Y_LEVEL3
    },
    // Level 4
    {
        tilemap_level4_compressed,
        object_level4, OBJECT_LEVEL4_COUNT,
        palette_background_level4,
        palette_sprite_level4,
        SPAWN_X_LEVEL4, SPAWN_Y_LEVEL4
    },
    // Level 5
    {
        tilemap_level5_compressed,
        object_level5, OBJECT_LEVEL5_COUNT,
        palette_background_level5,
        palette_sprite_level5,
        SPAWN_X_LEVEL5, SPAWN_Y_LEVEL5
    },
    // Level 6
    {
        tilemap_level6_compressed,
        object_level6, OBJECT_LEVEL6_COUNT,
        palette_background_level6,
        palette_sprite_level6,
        SPAWN_X_LEVEL6, SPAWN_Y_LEVEL6
    },
    // Level 7
    {
        tilemap_level7_compressed,
        object_level7, OBJECT_LEVEL7_COUNT,
        palette_background_level7,
        palette_sprite_level7,
        SPAWN_X_LEVEL7, SPAWN_Y_LEVEL7
    },
    // Level 8
    {
        tilemap_level8_compressed,
        object_level8, OBJECT_LEVEL8_COUNT,
        palette_background_level8,
        palette_sprite_level8,
        SPAWN_X_LEVEL8, SPAWN_Y_LEVEL8
    },
    // Level 9
    {
        tilemap_level9_compressed,
        object_level9, OBJECT_LEVEL9_COUNT,
        palette_background_level9,
        palette_sprite_level9,
        SPAWN_X_LEVEL9, SPAWN_Y_LEVEL9
    },
    // Level 10
    {
        tilemap_level10_compressed,
        object_level10, OBJECT_LEVEL10_COUNT,
        palette_background_level10,
        palette_sprite_level10,
        SPAWN_X_LEVEL10, SPAWN_Y_LEVEL10
    },
    // Level 11
    {
        tilemap_level11_compressed,
        object_level11, OBJECT_LEVEL11_COUNT,
        palette_background_level11,
        palette_sprite_level11,
        SPAWN_X_LEVEL11, SPAWN_Y_LEVEL11
    },
    // Level 12
    {
        tilemap_level12_compressed,
        object_level12, OBJECT_LEVEL12_COUNT,
        palette_background_level12,
        palette_sprite_level12,
        SPAWN_X_LEVEL12, SPAWN_Y_LEVEL12
    },
    // Level 13
    {
        tilemap_level13_compressed,
        object_level13, OBJECT_LEVEL13_COUNT,
        palette_background_level13,
        palette_sprite_level13,
        SPAWN_X_LEVEL13, SPAWN_Y_LEVEL13
    },
    // Level 14
    {
        tilemap_level14_compressed,
        object_level14, OBJECT_LEVEL14_COUNT,
        palette_background_level14,
        palette_sprite_level14,
        SPAWN_X_LEVEL14, SPAWN_Y_LEVEL14
    },
    // Level 15
    {
        tilemap_level15_compressed,
        object_level15, OBJECT_LEVEL15_COUNT,
        palette_background_level15,
        palette_sprite_level15,
        SPAWN_X_LEVEL15, SPAWN_Y_LEVEL15
    },
    // Level 16
    {
        tilemap_level16_compressed,
        object_level16, OBJECT_LEVEL16_COUNT,
        palette_background_level16,
        palette_sprite_level16,
        SPAWN_X_LEVEL16, SPAWN_Y_LEVEL16
    },
    // Level 17
    {
        tilemap_level17_compressed,
        object_level17, OBJECT_LEVEL17_COUNT,
        palette_background_level17,
        palette_sprite_level17,
        SPAWN_X_LEVEL17, SPAWN_Y_LEVEL17
    },
    // Level 18
    {
        tilemap_level18_compressed,
        object_level18, OBJECT_LEVEL18_COUNT,
        palette_background_level18,
        palette_sprite_level18,
        SPAWN_X_LEVEL18, SPAWN_Y_LEVEL18
    },
    // Level 19
    {
        tilemap_level19_compressed,
        object_level19, OBJECT_LEVEL19_COUNT,
        palette_background_level19,
        palette_sprite_level19,
        SPAWN_X_LEVEL19, SPAWN_Y_LEVEL19
    },
    // Level 20
    {
        tilemap_level20_compressed,
        object_level20, OBJECT_LEVEL20_COUNT,
        palette_background_level20,
        palette_sprite_level20,
        SPAWN_X_LEVEL20, SPAWN_Y_LEVEL20
    },
    // Level 21
    {
        tilemap_level21_compressed,
        object_level21, OBJECT_LEVEL21_COUNT,
        palette_background_level21,
        palette_sprite_level21,
        SPAWN_X_LEVEL21, SPAWN_Y_LEVEL21
    },
    // Level 22
    {
        tilemap_level22_compressed,
        object_level22, OBJECT_LEVEL22_COUNT,
        palette_background_level22,
        palette_sprite_level22,
        SPAWN_X_LEVEL22, SPAWN_Y_LEVEL22
    },
    // Level 23
    {
        tilemap_level23_compressed,
        object_level23, OBJECT_LEVEL23_COUNT,
        palette_background_level23,
        palette_sprite_level23,
        SPAWN_X_LEVEL23, SPAWN_Y_LEVEL23
    },
    // Level 24
    {
        tilemap_level24_compressed,
        object_level24, OBJECT_LEVEL24_COUNT,
        palette_background_level24,
        palette_sprite_level24,
        SPAWN_X_LEVEL24, SPAWN_Y_LEVEL24
    },
    // Level 25
    {
        tilemap_level25_compressed,
        object_level25, OBJECT_LEVEL25_COUNT,
        palette_background_level25,
        palette_sprite_level25,
        SPAWN_X_LEVEL25, SPAWN_Y_LEVEL25
    },
    // Level 26
    {
        tilemap_level26_compressed,
        object_level26, OBJECT_LEVEL26_COUNT,
        palette_background_level26,
        palette_sprite_level26,
        SPAWN_X_LEVEL26, SPAWN_Y_LEVEL26
    },
    // Level 27
    {
        tilemap_level27_compressed,
        object_level27, OBJECT_LEVEL27_COUNT,
        palette_background_level27,
        palette_sprite_level27,
        SPAWN_X_LEVEL27, SPAWN_Y_LEVEL27
    },
    // Level 28
    {
        tilemap_level28_compressed,
        object_level28, OBJECT_LEVEL28_COUNT,
        palette_background_level28,
        palette_sprite_level28,
        SPAWN_X_LEVEL28, SPAWN_Y_LEVEL28
    },
    // Level 29
    {
        tilemap_level29_compressed,
        object_level29, OBJECT_LEVEL29_COUNT,
        palette_background_level29,
        palette_sprite_level29,
        SPAWN_X_LEVEL29, SPAWN_Y_LEVEL29
    },
    // Level 30
    {
        tilemap_level30_compressed,
        object_level30, OBJECT_LEVEL30_COUNT,
        palette_background_level30,
        palette_sprite_level30,
        SPAWN_X_LEVEL30, SPAWN_Y_LEVEL30
    },
    // Level 31
    {
        tilemap_level31_compressed,
        object_level31, OBJECT_LEVEL31_COUNT,
        palette_background_level31,
        palette_sprite_level31,
        SPAWN_X_LEVEL31, SPAWN_Y_LEVEL31
    }
};

#define LEVEL_DATA_COUNT (sizeof(level_data) / sizeof(level_data[0]))

// Shared GID mapping (used by all levels)
#define GID_TO_TILE_MAP gid_to_tile_shared
#define GID_TO_TILE_MAP_COUNT GID_TO_TILE_SHARED_COUNT
// GID_TO_COLLISION_COUNT is defined in gid_to_tile_shared.h as GID_TO_TILE_SHARED_COUNT

// decompress_tilemap must be in fixed bank (no section attribute) so it can be called from any bank
// Caller must have level data bank active to read compressed_data
// Function switches to bank 5 when needed to read compression_dict_shared
static void decompress_tilemap(const unsigned char *compressed_data, uint8_t *output, uint8_t data_bank) {
    uint16_t comp_idx = 0, out_idx = 0;
    while (out_idx < LEVEL_TILE_COUNT) {
        uint8_t byte = compressed_data[comp_idx];
        if (byte < 0x80) {
            output[out_idx++] = byte;
            comp_idx++;
        } else if (byte < 0xC0) {
            uint8_t count = (byte & 0x3F) + 1;
            uint8_t gid = compressed_data[++comp_idx];
            comp_idx++;
            for (uint8_t i = 0; i < count && out_idx < LEVEL_TILE_COUNT; i++) {
                output[out_idx++] = gid;
            }
        } else {
            uint8_t dict_idx = byte & 0x3F;
            if (dict_idx < COMPRESSION_DICT_SHARED_COUNT) {
                // compression_dict_shared is in bank 5, switch to it to access
                set_prg_bank(5);
                output[out_idx++] = compression_dict_shared[dict_idx][0];
                if (out_idx < LEVEL_TILE_COUNT) output[out_idx++] = compression_dict_shared[dict_idx][1];
                // Switch back to level data bank to continue reading compressed_data
                set_prg_bank(data_bank);
            }
            comp_idx++;
        }
    }
    while (out_idx < LEVEL_TILE_COUNT) output[out_idx++] = 0;
}


static const unsigned char* get_object_sprite_data(uint8_t tile_index);
static void push_sprite(uint8_t y, uint8_t tile, uint8_t attr, uint8_t x);
static void render_16x16_sprite(const unsigned char *sprite_data, uint8_t baseX, uint8_t baseY, uint8_t oamProps);
static void render_16x8_sprite(const unsigned char *sprite_data, uint8_t baseX, uint8_t baseY, uint8_t oamProps);
static void render_object_sprite(OBJ_DATA *obj);

// get_object_sprite_data must stay in fixed bank - used by port_updatePlayerSprite (fixed bank)
// object_sprite_lookup_table and object_sprite_dict_compact are in bank 5
// Note: This function switches to bank 5 and does NOT switch back - caller must switch back after using the sprite data
static const unsigned char* get_object_sprite_data(uint8_t tile_index) {
    if (tile_index >= OBJECT_SPRITE_DICT_LOOKUP_TABLE_SIZE) return NULL;
    uint8_t compact_idx = object_sprite_lookup_table[tile_index];
    if (compact_idx == 0xFF || compact_idx >= OBJECT_SPRITE_DICT_COMPACT_COUNT) {
        set_prg_bank(0);  // Switch back to fixed bank on error
        return NULL;
    }
    return &object_sprite_dict_compact[compact_idx][0];
    // Note: Bank 5 remains active - caller must switch back to bank 0 after using the sprite data
}

// Write a sprite to OAM_BUF at the current cursor, advancing in the current direction.
// Forward: cursor goes 16→20→24→...→252 then wraps to 16
// Backward: cursor goes 252→248→244→...→16 then wraps to 252
__attribute__((section(".prg_rom_6")))
static void push_sprite(uint8_t y, uint8_t tile, uint8_t attr, uint8_t x) {
    OAM_BUF[s_oamCursor + 0] = y;
    OAM_BUF[s_oamCursor + 1] = tile;
    OAM_BUF[s_oamCursor + 2] = attr;
    OAM_BUF[s_oamCursor + 3] = x;
    s_oamCursor += s_oamDirection;
    // Wrap around within non-player region
    if (s_oamCursor < OAM_PLAYER_END || s_oamCursor > OAM_LAST_SLOT) {
        s_oamCursor = (s_oamDirection > 0) ? OAM_PLAYER_END : OAM_LAST_SLOT;
    }
}

__attribute__((section(".prg_rom_6")))
static void render_object_sprite(OBJ_DATA *obj) {
    if (obj->eType == OBJ_UNUSED) {
        return;
    }
    uint8_t baseX = (uint8_t)(obj->pos.x << 1);
    uint8_t spriteY = (uint8_t)(obj->pos.y << 1);
    uint8_t baseY = (spriteY < 240) ? (spriteY - s_scrollY) : spriteY;
    uint8_t tile_index = obj->oamTile;
    const unsigned char *sprite_data = get_object_sprite_data(tile_index);
    if (sprite_data == NULL) {
        return;
    }
    render_16x16_sprite(sprite_data, baseX, baseY, obj->oamProps);
}



// Music data (assembled into bank 7)
extern const unsigned char music_data_untitled[];

// --- Title Screen ---

#define TITLE_BANK_A 2
#define TITLE_BANK_B 3
#define TITLE_ANIM_SPEED 2  // 60Hz / 2 = 30fps (all tiles fit in 1 chunk + 1 attr = 2 vblanks)

// Palette 3, color 2 text fade: NES color values per animation frame
// Text "press select to change gfx" uses pixel value 14 = palette 3, color 2 ($3F0E)
// Frames 0-6: original, 7-9: step down, 10-14: black, 15-17: step up, 18+: original
static const uint8_t title_text_fade[TITLE_FRAME_COUNT] = {
    // 0-6: original color ($20 = white)
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    // 7-9: stepping down to black
    0x10, 0x00, 0x0F,
    // 10-14: black
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    // 15-17: stepping back up
    0x00, 0x10, 0x20,
    // 18-29: original
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
};

// Decode RLE nametable data and write to PPU nametable 0
// Must be called with PPU rendering off and correct PRG bank active
static void title_decode_rle_to_ppu(const unsigned char *rle_data) {
    vram_adr(NTADR_A(0, 0));
    uint16_t tiles_written = 0;
    uint16_t idx = 0;
    while (tiles_written < 960) {
        uint8_t count_minus_1 = rle_data[idx++];
        if (count_minus_1 == 0xFF) break;  // End marker (safety)
        uint8_t tile = rle_data[idx++];
        uint8_t count = count_minus_1 + 1;
        for (uint8_t i = 0; i < count && tiles_written < 960; i++) {
            vram_put(tile);
            tiles_written++;
        }
    }
}

// Write attribute table to PPU nametable 0
// Must be called with PPU rendering off and correct PRG bank active
static void title_write_attributes(const unsigned char *attr_data) {
    vram_adr(0x23C0);
    for (uint8_t i = 0; i < 64; i++) {
        vram_put(attr_data[i]);
    }
}

// --- Buffered PPU write system ---
// Decode delta from banked ROM during visible frame (CPU time is free).
// Blast buffered writes to PPU during vblank using 8-bit indexed arrays
// so the compiler emits efficient LDA array,X (4 cycles) instead of
// expensive 16-bit indirect addressing (~20 cycles).
//
// Per write: 3x LDA abs,X (12) + 3x STA abs (12) + INX (2) = ~26 cycles.
// After neslib NMI (~800 cycles), ~1400 cycles remain. 34 tiles proven safe.
// Attributes cost ~280 cycles. When the last tile batch is small (<=23 tiles,
// ~600 cycles), attributes are merged into the same vblank to save a frame.
// Shared animation state (used by title screen and level 4 animation)
uint8_t  s_ppu_write_count;
uint8_t  s_ppu_write_cursor;
uint8_t  s_attr_buf[64];
uint8_t  s_delta_active;
uint8_t  s_flush_rate;

// Title flush functions in nes_title_flush.c (non-LTO to keep switch tables out of fixed bank)
extern void title_flush_ppu_writes(uint8_t variant);
extern void title_prepare_delta(uint8_t anim_frame, uint8_t tile_count);

// Apply a full delta with PPU off (used for SELECT variant swap replay)
static void title_apply_delta_full(const unsigned char *delta) {
    uint16_t idx = 0;
    uint16_t count = delta[idx] | ((uint16_t)delta[idx + 1] << 8);
    idx += 2;
    for (uint16_t i = 0; i < count; i++) {
        uint16_t pos = delta[idx] | ((uint16_t)delta[idx + 1] << 8);
        uint8_t tile = delta[idx + 2];
        idx += 3;
        uint16_t addr = 0x2000 + pos;
        PPU_ADDR = (uint8_t)(addr >> 8);
        PPU_ADDR = (uint8_t)(addr);
        PPU_DATA = tile;
    }
    PPU_ADDR = 0x23;
    PPU_ADDR = 0xC0;
    for (uint8_t i = 0; i < 64; i++) {
        PPU_DATA = delta[idx++];
    }
}

// Load frame 0 with PPU off (full nametable + attributes)
// Sets correct PRG bank internally.
static void title_load_frame0(uint8_t variant) {
    if (variant == 0) {
        set_prg_bank(TITLE_BANK_A);
        title_decode_rle_to_ppu(title_nt_rle_a);
        title_write_attributes(title_attr0_a);
    } else {
        set_prg_bank(TITLE_BANK_B);
        title_decode_rle_to_ppu(title_nt_rle_b);
        title_write_attributes(title_attr0_b);
    }
    set_prg_bank(0);
}

// Forward declarations for CHR-RAM helpers (defined after title_screen_loop)
static void copy_to_ppu(uint16_t ppu_addr, const unsigned char *src, uint16_t count);
static void load_game_chr(void);

// Run the title screen loop. Returns when player presses START.
static void title_screen_loop(void) {
    // Load title CHR tiles to CHR-RAM (single bank, replaces game tiles)
    {
        extern const unsigned char title_chr_data[];
        ppu_off();
        set_prg_bank(3);
        copy_to_ppu(0x0000, title_chr_data, 8192);
        set_prg_bank(0);
    }

    // Load title palette
    for (uint8_t i = 0; i < 16; i++) {
        palette_ram[i] = title_bg_palette[i];
    }
    pal_bg(palette_ram);

    // Hide all sprites
    oam_clear();

    uint8_t variant = 0;  // 0 = nametableA (NES), 1 = nametableB (PICO-8)
    uint8_t anim_frame = 0;
    uint8_t frame_timer = 0;
    uint8_t prev_input = 0xFF;  // Prevent immediate trigger on first frame

    // Load initial frame 0 (full nametable, PPU off)
    ppu_off();
    title_load_frame0(variant);
    PPU_CTRL = 0x80;  // NMI enabled, BG pattern $0000, sprite pattern $0000
    s_delta_active = 0;
    ppu_on_all();

    for (;;) {
        ppu_wait_nmi();
        // --- Vblank-critical: only raw PPU register writes here ---
        // Data was already decoded into RAM buffers during the previous visible frame.

        // Flush tile/attr writes using inline constant code (banks 10/11)
        if (s_delta_active) {
            title_flush_ppu_writes(variant);
        }

        // Update text fade: write palette 3 color 2 ($3F0E) directly
        // Bypasses neslib's deferred pal_bg() which conflicts with raw PPU writes
        (void)PPU_STATUS;          // Reset address latch
        PPU_ADDR = 0x3F;
        PPU_ADDR = 0x0E;
        PPU_DATA = title_text_fade[anim_frame];
        palette_ram[14] = title_text_fade[anim_frame];  // Keep in sync for variant switch

        // Reset scroll. PPU_CTRL re-asserts nametable 0 in t[10:11] — the $3F0F
        // address write above corrupts those bits, and PPU_SCROLL can't clear them.
        PPU_CTRL = 0x80;           // NMI on, nametable 0 → fixes t[10:11]
        PPU_SCROLL = 0;
        PPU_SCROLL = 0;

        // --- End vblank-critical section ---
        // Everything below runs during visible frame (CPU time is free)

        // Update music engine
        set_prg_bank(BANK_MUSIC);
        famistudio_update();
        set_prg_bank(0);

        // Poll input
        volatile uint8_t raw = (uint8_t)pad_poll(0);
        __asm__ __volatile__("" ::: "memory");
        uint8_t pressed = raw & ~prev_input;
        prev_input = raw;

        // START exits title screen
        if (pressed & PAD_START) break;

        // SELECT toggles graphic variant: need full reload (PPU off)
        if (pressed & PAD_SELECT) {
            variant ^= 1;
            s_delta_active = 0;
            ppu_off();
            title_load_frame0(variant);
            // Replay deltas to reach current anim_frame (PPU is off, no time limit)
            for (uint8_t f = 0; f < anim_frame; f++) {
                if (variant == 0) {
                    set_prg_bank(TITLE_BANK_A);
                    title_apply_delta_full(&title_delta_a[title_delta_offsets_a[f]]);
                } else {
                    set_prg_bank(TITLE_BANK_B);
                    title_apply_delta_full(&title_delta_b[title_delta_offsets_b[f]]);
                }
            }
            set_prg_bank(0);
            ppu_on_all();
        }

        // Advance animation: decode next delta into RAM buffer (CPU work, not vblank)
        if (!s_delta_active) {
            frame_timer++;
            if (frame_timer >= TITLE_ANIM_SPEED) {
                frame_timer = 0;
                // Set up inline flush for this delta (no ROM access needed)
                if (variant == 0)
                    title_prepare_delta(anim_frame, title_delta_counts_a[anim_frame]);
                else
                    title_prepare_delta(anim_frame, title_delta_counts_b[anim_frame]);
                anim_frame++;
                if (anim_frame >= TITLE_FRAME_COUNT) {
                    anim_frame = 0;
                }

                // Inline flush handles chunking automatically via TITLE_WRITES_PER_CHUNK
            }
        }
    }

    // Transition to game: restore game CHR tiles to CHR-RAM and PPU settings
    ppu_off();
    load_game_chr();
    bank_bg(0);
    bank_spr(0);  // Sprites use $0000 (all game sprite tiles are in BG pattern table)
}

// --- Level 4 Animation (implemented in nes_level4_anim.c) ---
// Extern declarations for state shared with the animation module
extern uint8_t s_levelAnimActive;
extern void la_init_bank4(void);
extern void port_levelAnimFlush(void);

#define LEVEL4_ANIM_BANK 4

// Level animation functions are in nes_level4_anim.c (separate TU, no LTO)
// Remaining old code removed — everything below this line is unrelated to level animation.
// Copy data to PPU CHR-RAM at a given address (call with PPU off)
static void copy_to_ppu(uint16_t ppu_addr, const unsigned char *src, uint16_t count) {
    vram_adr(ppu_addr);
    for (uint16_t i = 0; i < count; i++) {
        vram_put(src[i]);
    }
}

// Load game CHR tiles to CHR-RAM (single 8KB bank, call with PPU off)
static void load_game_chr(void) {
    extern const unsigned char game_chr_data[];
    set_prg_bank(5);
    copy_to_ppu(0x0000, game_chr_data, 8192);
    set_prg_bank(0);
}

void port_init(void) {
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;

    ppu_off();
    oam_clear();
    oam_size(0);

    // Initialize CHR-RAM with game tiles (single 8KB bank, no bank switching)
    load_game_chr();

    bank_bg(0);
    bank_spr(0);  // Sprites use $0000 (all game sprite tiles are in BG pattern table)
    ppu_on_all();
    ppu_wait_nmi();  // First NMI sets NTSC_MODE — must happen before music init

    // Init FamiStudio engine (engine + data both in bank 7)
    set_prg_bank(BANK_MUSIC);
    fs_init(music_data_untitled);
    fs_play(0);
    set_prg_bank(0);

    // Show title screen (blocks until START is pressed)
    title_screen_loop();
}

void port_beginSpriteBuild(const struct sPlayerData *playerObj) {
    (void)playerObj;
    // Hide all non-player OAM entries; visible sprites overwrite them.
    memset(&OAM_BUF[OAM_PLAYER_END], 240, 256 - OAM_PLAYER_END);
    // Alternate write direction each frame:
    // Even frames: write forward from low OAM (object 1 gets highest priority)
    // Odd frames:  write backward from high OAM (object 1 gets lowest priority)
    if (s_rotateOffset & 1) {
        s_oamCursor = OAM_LAST_SLOT;
        s_oamDirection = -4;
    } else {
        s_oamCursor = OAM_PLAYER_END;
        s_oamDirection = 4;
    }
}

void port_finishSpriteBuild(void) {
    s_rotateOffset++;
}

void port_updatePlayerSprite(const struct sPlayerData *playerObj) {
    if (playerObj == NULL) {
        return;
    }

    const struct sOBJ_DATA *playerData = &playerObj->objData;
    uint8_t baseX = (uint8_t)(playerData->pos.x << 1);
    uint8_t spriteY = (uint8_t)(playerData->pos.y << 1);
    uint8_t baseY = (spriteY < 240) ? (spriteY - s_scrollY) : spriteY;
    const unsigned char *sd = get_object_sprite_data(playerData->oamTile);
    if (sd == NULL) {
        set_prg_bank(0);
        memset(OAM_BUF, 240, OAM_PLAYER_END);
        return;
    }
    // Write player directly to OAM slots 0-3 (fixed, no rotation)
    uint8_t props = (sd[0] & 0x03) & ~0x20;
    bool flipH = (playerData->oamProps & 0x40) != 0;
    if (flipH) props |= 0x40;
    uint8_t t0 = flipH ? sd[2] : sd[1], t1 = flipH ? sd[1] : sd[2];
    uint8_t t2 = flipH ? sd[4] : sd[3], t3 = flipH ? sd[3] : sd[4];
    OAM_BUF[0]=baseY;   OAM_BUF[1]=t0; OAM_BUF[2]=props;  OAM_BUF[3]=baseX;
    OAM_BUF[4]=baseY;   OAM_BUF[5]=t1; OAM_BUF[6]=props;  OAM_BUF[7]=baseX+8;
    OAM_BUF[8]=baseY+8; OAM_BUF[9]=t2; OAM_BUF[10]=props; OAM_BUF[11]=baseX;
    OAM_BUF[12]=baseY+8;OAM_BUF[13]=t3;OAM_BUF[14]=props; OAM_BUF[15]=baseX+8;
    set_prg_bank(0);
}

__attribute__((section(".prg_rom_6")))
void port_buildUnused(uint8_t index) {
    // Clear dirty flag - no sprites to push for unused objects
    GLOBAL_OBJList[index].flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

__attribute__((section(".prg_rom_6")))
void port_buildSmoke(uint8_t index) {
}

__attribute__((section(".prg_rom_6")))
void port_buildBreakableWall(uint8_t index) {
}

__attribute__((section(".prg_rom_6")))
void port_buildBalloon(uint8_t index) {
    OBJ_DATA *balloon = &GLOBAL_OBJList[index];

    // Check if balloon should be hidden (unused, popped, or hidden)
    // BALLOON_STATE_IDLE = 0, BALLOON_STATE_POPPED = 1
    if (balloon->eType == OBJ_UNUSED ||
        balloon->data.balloon.state == 1 ||
        balloon->data.balloon.hideFrameCount > 0) {
        return;
    }

    // Calculate balloon position with Y offset for bob animation
    uint8_t baseX = (uint8_t)(balloon->pos.x << 1);
    uint8_t spriteY = (uint8_t)(balloon->pos.y << 1);
    int8_t yOffset = balloon->data.balloon.spriteYOffset << 1;
    uint8_t balloonY = (spriteY < 240) ? ((uint8_t)((int16_t)spriteY - (int16_t)s_scrollY + (int16_t)yOffset)) : ((uint8_t)((int16_t)spriteY + (int16_t)yOffset));

    // Render balloon sprite (top 16x16 sprite)
    uint8_t tile_index = balloon->oamTile;  // BALLOON_SPRITE_1 = 22
    const unsigned char *balloon_sprite_data = get_object_sprite_data(tile_index);
    if (balloon_sprite_data == NULL) {
        return;
    }
    render_16x16_sprite(balloon_sprite_data, baseX, balloonY, balloon->oamProps);

    // Render string sprite (bottom 16x16 sprite, positioned below balloon)
    uint8_t stringTile = balloon->data.balloon.stringTile;  // BALLOON_STRING_1, BALLOON_STRING_2, or BALLOON_STRING_3
    const unsigned char *string_sprite_data = get_object_sprite_data(stringTile);
    if (string_sprite_data == NULL) {
        return;
    }
    // String sprite is positioned 14 pixels below the balloon (moved up 2 pixels)
    uint8_t stringY = balloonY + 14;
    // Use same palette as balloon
    render_16x16_sprite(string_sprite_data, baseX, stringY, balloon->oamProps);
}

__attribute__((section(".prg_rom_6")))
void port_buildMonument(uint8_t index) {
    // Monuments render as background tiles, not sprites
    // They are handled by the background tile update system (port_updateCollapseTileNametable)
    (void)index;
}

__attribute__((section(".prg_rom_6")))
void port_buildChest(uint8_t index) {
    OBJ_DATA *chest = &GLOBAL_OBJList[index];
    if (chest->eType == OBJ_UNUSED) {
        return;
    }
    // Hide chest when it's in the OPEN state (state == 2)
    // CHEST_STATE_OPEN = 2
    if (chest->data.chest.state == 2) {
        return;
    }
    render_object_sprite(chest);
    // render_object_sprite calls get_object_sprite_data which switches to bank 5
    // We need to switch back to bank 6 since this function is in bank 6
    set_prg_bank(6);
}

__attribute__((section(".prg_rom_6")))
void port_buildBigChest(uint8_t index) {
    // Big chest renders as background tiles, not sprites
    // It is handled by the background tile update system (port_updateCollapseTileNametable)
    (void)index;
}

__attribute__((section(".prg_rom_6")))
void port_buildKey(uint8_t index) {
    OBJ_DATA *key = &GLOBAL_OBJList[index];
    if (key->eType == OBJ_UNUSED) {
        return;
    }
    render_object_sprite(key);
    // render_object_sprite calls get_object_sprite_data which switches to bank 5
    // We need to switch back to bank 6 since this function is in bank 6
    set_prg_bank(6);
}

// Render a 16x16 object sprite into rotating OAM slots
__attribute__((section(".prg_rom_6")))
static void render_16x16_sprite(const unsigned char *sprite_data, uint8_t baseX, uint8_t baseY, uint8_t oamProps) {
    if (sprite_data == NULL) return;
    uint8_t palette_idx = sprite_data[0];
    uint8_t tl_tile = sprite_data[1], tr_tile = sprite_data[2];
    uint8_t bl_tile = sprite_data[3], br_tile = sprite_data[4];
    uint8_t baseProps = (palette_idx & 0x03) & ~0x20;
    bool flip_horizontal = (oamProps & 0x40) != 0;
    if (flip_horizontal) baseProps |= 0x40;

    if (flip_horizontal) {
        push_sprite(baseY,     tr_tile, baseProps, baseX);
        push_sprite(baseY,     tl_tile, baseProps, baseX + 8);
        push_sprite(baseY + 8, br_tile, baseProps, baseX);
        push_sprite(baseY + 8, bl_tile, baseProps, baseX + 8);
    } else {
        push_sprite(baseY,     tl_tile, baseProps, baseX);
        push_sprite(baseY,     tr_tile, baseProps, baseX + 8);
        push_sprite(baseY + 8, bl_tile, baseProps, baseX);
        push_sprite(baseY + 8, br_tile, baseProps, baseX + 8);
    }
}

// Render a 16x8 sprite (2 sprites: left and right, top row only)
__attribute__((section(".prg_rom_6")))
static void render_16x8_sprite(const unsigned char *sprite_data, uint8_t baseX, uint8_t baseY, uint8_t oamProps) {
    if (sprite_data == NULL) return;
    uint8_t palette_idx = sprite_data[0];
    uint8_t tl_tile = sprite_data[1], tr_tile = sprite_data[2];
    uint8_t baseProps = (palette_idx & 0x03) & ~0x20;
    bool flip_horizontal = (oamProps & 0x40) != 0;
    if (flip_horizontal) baseProps |= 0x40;

    if (flip_horizontal) {
        push_sprite(baseY, tr_tile, baseProps, baseX);
        push_sprite(baseY, tl_tile, baseProps, baseX + 8);
    } else {
        push_sprite(baseY, tl_tile, baseProps, baseX);
        push_sprite(baseY, tr_tile, baseProps, baseX + 8);
    }
}

__attribute__((section(".prg_rom_6")))
void port_buildSpring(uint8_t index) {
    OBJ_DATA *spring = &GLOBAL_OBJList[index];
    if (spring->eType == OBJ_UNUSED || spring->data.spring.isDisabled) {
        return;
    }
    render_object_sprite(spring);
}

// Queue a background tile object update (called from mainBankZero.c when state changes)
// Handles collapse tiles, breakable walls, and other background tile objects
void port_updateCollapseTileNametable(uint8_t index) {
    if (index < 29) {
        s_pendingBgTileMask |= (1UL << index);
    }
}

static void prepare_bg_tiles_nametable(BgTileWrite *writes, uint8_t *write_count) {
    *write_count = 0;
    if (s_pendingBgTileMask == 0) return;

    // Process 1 pending update per frame (find lowest set bit)
    uint8_t index;
    {
        uint32_t mask = s_pendingBgTileMask;
        for (index = 0; index < 29; index++) {
            if (mask & (1UL << index)) break;
        }
        if (index >= 29) { s_pendingBgTileMask = 0; return; }
    }
    {
        OBJ_DATA *bgTileObj = &GLOBAL_OBJList[index];
        
        uint8_t tileX = bgTileObj->pos.x / 8;
        uint8_t tileY = (bgTileObj->pos.y + 1) / 8;
        const unsigned char *tile_entry = NULL;
        
        // Handle collapse tiles
        if (bgTileObj->eType == OBJ_COLLAPSE_TILE) {
            if (bgTileObj->data.collapseTile.state != 2 && bgTileObj->oamTile >= COLLAPSE_TILE_SPRITE_1 && bgTileObj->oamTile <= COLLAPSE_TILE_SPRITE_3) {
                // gid_to_tile_collapse is in bank 5, switch to it before accessing
                set_prg_bank(5);
                tile_entry = gid_to_tile_collapse[bgTileObj->oamTile - COLLAPSE_TILE_SPRITE_1];
                // Switch back to bank 6
                set_prg_bank(6);
            }
            // If state == 2 (HIDDEN), tile_entry remains NULL (will clear the tile)
        }
        // Handle breakable walls
        else if (bgTileObj->eType == OBJ_BREAKABLE_WALL) {
            // Breakable walls: if object still exists (not destroyed), show the wall tile
            if (bgTileObj->oamTile == BREAKABLE_WALL_SPRITE_1) {
                // gid_to_tile_breakable_wall is in bank 5, switch to it before accessing
                set_prg_bank(5);
                tile_entry = gid_to_tile_breakable_wall[0];
                // Switch back to bank 6
                set_prg_bank(6);
            }
            // If oamTile doesn't match, tile_entry remains NULL (will clear the tile)
        }
        // Handle monuments
        else if (bgTileObj->eType == OBJ_MONUMENT) {
            // Monuments: oamTile contains MONUMENT_SPRITE_1 (70)
            // Monuments are 32x32 (4x4 tiles) composed of 4 sprites: 70, 71, 86, 87
            // All monuments use the same tile data (index 0) which contains all 16 tiles
            if (bgTileObj->oamTile == MONUMENT_SPRITE_1) {
                // gid_to_tile_monument is in bank 5, switch to it before accessing
                set_prg_bank(5);
                tile_entry = gid_to_tile_monument[0];  // All monuments use index 0 (contains 16 tiles)
                // Switch back to bank 6
                set_prg_bank(6);
            }
        }
        // Handle big chest
        else if (bgTileObj->eType == OBJ_BIG_CHEST) {
            // Big chest: oamTile contains BIG_CHEST_SPRITE_1 (96)
            // Big chest is 32x32 (4x4 tiles) composed of sprites: 96, 97 (top), 112, 113 (bottom)
            // All big chests use the same tile data (index 0) which contains all 16 tiles
            if (bgTileObj->oamTile == BIG_CHEST_SPRITE_1) {
                // gid_to_tile_big_chest is in bank 5, switch to it before accessing
                set_prg_bank(5);
                
                // If chest is open (state != 0 means OPEN_ANIM or OPENED), clear top 8 tiles (rows 1 and 2)
                // Keep bottom 8 tiles (rows 3 and 4, which are GIDs 112, 113) visible
                // BIG_CHEST_STATE_IDLE = 0, BIG_CHEST_STATE_OPEN_ANIM = 1, BIG_CHEST_STATE_OPENED = 2
                static unsigned char big_chest_open_tiles[18];  // Static buffer for modified tile data
                if (bgTileObj->data.bigChest.state != 0) {  // 0 = BIG_CHEST_STATE_IDLE
                    // Chest is open: clear top 8 tiles, keep bottom 8 tiles
                    const unsigned char *full_tiles = gid_to_tile_big_chest[0];
                    for (uint8_t i = 0; i < 8; i++) {
                        big_chest_open_tiles[i] = 0;  // Clear top rows (indices 0-7)
                    }
                    for (uint8_t i = 8; i < 18; i++) {
                        big_chest_open_tiles[i] = full_tiles[i];  // Keep bottom rows + palette + flip (indices 8-17)
                    }
                    tile_entry = big_chest_open_tiles;
                } else {
                    // Chest is closed: use full tile data
                    tile_entry = gid_to_tile_big_chest[0];
                }
                // Switch back to bank 6
                set_prg_bank(6);
            }
        }
        // If object is OBJ_UNUSED, tile_entry remains NULL (will clear the tile)
        // This handles the case where a breakable wall was queued before being destroyed
        
        BgTileWrite *write = &writes[(*write_count)++];
        
        // Check if this is a breakable wall (either active or destroyed)
        // Destroyed breakable walls become OBJ_UNUSED but may still have BREAKABLE_WALL_SPRITE_1 in oamTile
        bool is_breakable_wall = (bgTileObj->eType == OBJ_BREAKABLE_WALL) || 
                                  (bgTileObj->eType == OBJ_UNUSED && bgTileObj->oamTile == BREAKABLE_WALL_SPRITE_1);
        bool is_monument = (bgTileObj->eType == OBJ_MONUMENT);
        bool is_big_chest = (bgTileObj->eType == OBJ_BIG_CHEST);
        
        if (is_breakable_wall || is_monument || is_big_chest) {
            // 4x4 grid for breakable walls
            uint8_t nes_tile_x = tileX * 2;
            uint8_t nes_tile_y_row1 = tileY * 2;
            uint8_t nes_tile_y_row2 = nes_tile_y_row1 + 1;
            uint8_t nes_tile_y_row3 = nes_tile_y_row1 + 2;
            uint8_t nes_tile_y_row4 = nes_tile_y_row1 + 3;
            
            uint16_t nametable_base_row1 = (nes_tile_y_row1 < 30) ? 0x2000 : 0x2800;
            uint16_t nametable_base_row2 = (nes_tile_y_row2 < 30) ? 0x2000 : 0x2800;
            uint16_t nametable_base_row3 = (nes_tile_y_row3 < 30) ? 0x2000 : 0x2800;
            uint16_t nametable_base_row4 = (nes_tile_y_row4 < 30) ? 0x2000 : 0x2800;
            
            uint8_t nes_tile_y_adj_row1 = (nes_tile_y_row1 < 30) ? nes_tile_y_row1 : (nes_tile_y_row1 - 30);
            uint8_t nes_tile_y_adj_row2 = (nes_tile_y_row2 < 30) ? nes_tile_y_row2 : (nes_tile_y_row2 - 30);
            uint8_t nes_tile_y_adj_row3 = (nes_tile_y_row3 < 30) ? nes_tile_y_row3 : (nes_tile_y_row3 - 30);
            uint8_t nes_tile_y_adj_row4 = (nes_tile_y_row4 < 30) ? nes_tile_y_row4 : (nes_tile_y_row4 - 30);
            
            write->addr_top = nametable_base_row1 + ((uint16_t)nes_tile_y_adj_row1 * 32) + nes_tile_x;
            write->addr_bottom = nametable_base_row2 + ((uint16_t)nes_tile_y_adj_row2 * 32) + nes_tile_x;
            write->addr_row2 = nametable_base_row3 + ((uint16_t)nes_tile_y_adj_row3 * 32) + nes_tile_x;
            write->addr_row3 = nametable_base_row4 + ((uint16_t)nes_tile_y_adj_row4 * 32) + nes_tile_x;
            write->is_4x4 = 1;
        } else if (is_monument) {
            // 4x4 grid for monuments (same as breakable walls)
            uint8_t nes_tile_x = tileX * 2;
            uint8_t nes_tile_y_row1 = tileY * 2;
            uint8_t nes_tile_y_row2 = nes_tile_y_row1 + 1;
            uint8_t nes_tile_y_row3 = nes_tile_y_row1 + 2;
            uint8_t nes_tile_y_row4 = nes_tile_y_row1 + 3;
            
            uint16_t nametable_base_row1 = (nes_tile_y_row1 < 30) ? 0x2000 : 0x2800;
            uint16_t nametable_base_row2 = (nes_tile_y_row2 < 30) ? 0x2000 : 0x2800;
            uint16_t nametable_base_row3 = (nes_tile_y_row3 < 30) ? 0x2000 : 0x2800;
            uint16_t nametable_base_row4 = (nes_tile_y_row4 < 30) ? 0x2000 : 0x2800;
            
            uint8_t nes_tile_y_adj_row1 = (nes_tile_y_row1 < 30) ? nes_tile_y_row1 : (nes_tile_y_row1 - 30);
            uint8_t nes_tile_y_adj_row2 = (nes_tile_y_row2 < 30) ? nes_tile_y_row2 : (nes_tile_y_row2 - 30);
            uint8_t nes_tile_y_adj_row3 = (nes_tile_y_row3 < 30) ? nes_tile_y_row3 : (nes_tile_y_row3 - 30);
            uint8_t nes_tile_y_adj_row4 = (nes_tile_y_row4 < 30) ? nes_tile_y_row4 : (nes_tile_y_row4 - 30);
            
            write->addr_top = nametable_base_row1 + ((uint16_t)nes_tile_y_adj_row1 * 32) + nes_tile_x;
            write->addr_bottom = nametable_base_row2 + ((uint16_t)nes_tile_y_adj_row2 * 32) + nes_tile_x;
            write->addr_row2 = nametable_base_row3 + ((uint16_t)nes_tile_y_adj_row3 * 32) + nes_tile_x;
            write->addr_row3 = nametable_base_row4 + ((uint16_t)nes_tile_y_adj_row4 * 32) + nes_tile_x;
            write->is_4x4 = 1;
        } else {
            // 2x2 grid for collapse tiles
            uint8_t nes_tile_x = tileX * 2;
            uint8_t nes_tile_y_top = tileY * 2;
            uint8_t nes_tile_y_bottom = nes_tile_y_top + 1;
            uint16_t nametable_base_top = (nes_tile_y_top < 30) ? 0x2000 : 0x2800;
            uint16_t nametable_base_bottom = (nes_tile_y_bottom < 30) ? 0x2000 : 0x2800;
            uint8_t nes_tile_y_adj_top = (nes_tile_y_top < 30) ? nes_tile_y_top : (nes_tile_y_top - 30);
            uint8_t nes_tile_y_adj_bottom = (nes_tile_y_bottom < 30) ? nes_tile_y_bottom : (nes_tile_y_bottom - 30);
            write->addr_top = nametable_base_top + ((uint16_t)nes_tile_y_adj_top * 32) + nes_tile_x;
            write->addr_bottom = nametable_base_bottom + ((uint16_t)nes_tile_y_adj_bottom * 32) + nes_tile_x;
            write->addr_row2 = 0;
            write->addr_row3 = 0;
            write->is_4x4 = 0;
        }
        
        write->tile_data = tile_entry;
    }
    // Clear the processed bit
    s_pendingBgTileMask &= ~(1UL << index);
}

static void execute_bg_tiles_nametable_writes(const BgTileWrite *writes, uint8_t write_count) {
    // Check if any writes need bank 5 data
    bool needs_bank5 = false;
    for (uint8_t i = 0; i < write_count; i++) {
        if (writes[i].tile_data != NULL) {
            needs_bank5 = true;
            break;
        }
    }
    
    // Switch to bank 5 if needed (tile_data pointers point to data in bank 5)
    if (needs_bank5) {
        set_prg_bank(5);
    }
    
    for (uint8_t i = 0; i < write_count; i++) {
        const BgTileWrite *write = &writes[i];
        if (write->tile_data != NULL) {
            if (write->is_4x4) {
                // 4x4 grid for breakable walls and monuments (16 tiles in row-major order)
                vram_adr(write->addr_top);
                vram_put(write->tile_data[0]);
                vram_put(write->tile_data[1]);
                vram_put(write->tile_data[2]);
                vram_put(write->tile_data[3]);
                vram_adr(write->addr_bottom);
                vram_put(write->tile_data[4]);
                vram_put(write->tile_data[5]);
                vram_put(write->tile_data[6]);
                vram_put(write->tile_data[7]);
                vram_adr(write->addr_row2);
                vram_put(write->tile_data[8]);
                vram_put(write->tile_data[9]);
                vram_put(write->tile_data[10]);
                vram_put(write->tile_data[11]);
                vram_adr(write->addr_row3);
                vram_put(write->tile_data[12]);
                vram_put(write->tile_data[13]);
                vram_put(write->tile_data[14]);
                vram_put(write->tile_data[15]);
            } else {
                // 2x2 grid for collapse tiles
                vram_adr(write->addr_top);
                vram_put(write->tile_data[0]);
                vram_put(write->tile_data[1]);
                vram_adr(write->addr_bottom);
                vram_put(write->tile_data[2]);
                vram_put(write->tile_data[3]);
            }
        } else {
            if (write->is_4x4) {
                // Clear 4x4 grid
                vram_adr(write->addr_top);
                vram_put(0);
                vram_put(0);
                vram_put(0);
                vram_put(0);
                vram_adr(write->addr_bottom);
                vram_put(0);
                vram_put(0);
                vram_put(0);
                vram_put(0);
                vram_adr(write->addr_row2);
                vram_put(0);
                vram_put(0);
                vram_put(0);
                vram_put(0);
                vram_adr(write->addr_row3);
                vram_put(0);
                vram_put(0);
                vram_put(0);
                vram_put(0);
            } else {
                // Clear 2x2 grid
                vram_adr(write->addr_top);
                vram_put(0);
                vram_put(0);
                vram_adr(write->addr_bottom);
                vram_put(0);
                vram_put(0);
            }
        }
    }
    
    // Switch back to bank 6 (execute_bg_tiles_nametable_writes is called from bank 6 context)
    if (needs_bank5) {
        set_prg_bank(6);
    }
    if (write_count > 0) {
        (void)PPU_STATUS;
        PPU_ADDR = 0;
        PPU_ADDR = 0;
    }
}

void port_buildCollapseTile(uint8_t index) {
    (void)index;
}

__attribute__((section(".prg_rom_6")))
void port_buildStrawberry(uint8_t index) {
    OBJ_DATA *strawberry = &GLOBAL_OBJList[index];
    if (strawberry->eType == OBJ_UNUSED ||
        (strawberry->data.strawberry.isCollected && strawberry->data.strawberry.frameCount > 60)) {
        return;
    }
    render_object_sprite(strawberry);
}

__attribute__((section(".prg_rom_6")))
void port_buildPlatMov(uint8_t index) {
    OBJ_DATA *platMov = &GLOBAL_OBJList[index];
    if (platMov->eType == OBJ_UNUSED) {
        return;
    }

    uint8_t baseX = (uint8_t)(platMov->pos.x << 1);
    uint8_t spriteY = (uint8_t)(platMov->pos.y << 1);
    uint8_t baseY = (spriteY < 240) ? (spriteY - s_scrollY) : spriteY;
    uint8_t oamProps = platMov->oamProps;

    // Render left side (PLATMOV_SPRITE_1 = 11)
    const unsigned char *left_sprite_data = get_object_sprite_data(PLATMOV_SPRITE_1);
    if (left_sprite_data == NULL) {
        set_prg_bank(6);
        return;
    }
    render_16x8_sprite(left_sprite_data, baseX, baseY, oamProps);

    // Render right side (PLATMOV_SPRITE_2 = 12) at baseX + 16
    const unsigned char *right_sprite_data = get_object_sprite_data(PLATMOV_SPRITE_2);
    if (right_sprite_data == NULL) {
        set_prg_bank(6);
        return;
    }
    render_16x8_sprite(right_sprite_data, baseX + 16, baseY, oamProps);

    // get_object_sprite_data switches to bank 5, switch back to bank 6
    set_prg_bank(6);
}

__attribute__((section(".prg_rom_6")))
void port_buildFlyingBerry(uint8_t index) {
    OBJ_DATA *berry = &GLOBAL_OBJList[index];
    bool isHidden = (berry->eType == OBJ_UNUSED ||
                     (berry->data.strawberry.isCollected && berry->data.strawberry.frameCount > 60));

    if (isHidden) {
        return;
    }

    // Render main berry sprite
    render_object_sprite(berry);

    // Calculate wing position and animation
    uint8_t baseX = (uint8_t)(berry->pos.x << 1);
    uint8_t spriteY = (uint8_t)(berry->pos.y << 1);
    uint8_t baseY = (spriteY < 240) ? (spriteY - s_scrollY) : spriteY;

    // Determine wing tile based on vertical movement (comparing current Y to startY)
    int16_t deltaY = (int16_t)berry->pos.y - (int16_t)berry->data.strawberry.startY;
    uint8_t wingTile;
    if (deltaY < 0) {
        wingTile = FLYING_BERRY_WING_UP;  // Moving up
    } else if (deltaY > 0) {
        wingTile = FLYING_BERRY_WING_DOWN;  // Moving down
    } else {
        wingTile = FLYING_BERRY_WING_MID;  // At rest
    }

    // Get wing sprite data
    const unsigned char *wing_sprite_data = get_object_sprite_data(wingTile);
    if (wing_sprite_data == NULL) {
        set_prg_bank(6);
        return;
    }

    // Render left wing (at baseX - 14, baseY - 2)
    // Left wing uses flipped properties (0x74 = priority 3, palette 2, flip horizontal)
    uint8_t leftWingX = (baseX >= 14) ? (baseX - 14) : 0;
    uint8_t leftWingY = (baseY >= 2) ? (baseY - 2) : 0;
    render_16x16_sprite(wing_sprite_data, leftWingX, leftWingY, 0x74);

    // Render right wing (at baseX + 14, baseY - 2)
    // Right wing uses normal properties (0x34 = priority 3, palette 2)
    uint8_t rightWingX = baseX + 14;
    uint8_t rightWingY = (baseY >= 2) ? (baseY - 2) : 0;
    render_16x16_sprite(wing_sprite_data, rightWingX, rightWingY, 0x34);

    // get_object_sprite_data switches to bank 5, switch back to bank 6
    set_prg_bank(6);
}

__attribute__((section(".prg_rom_6")))
void port_buildDoubleDashOrb(uint8_t index) {
    OBJ_DATA *orb = &GLOBAL_OBJList[index];
    if (orb->eType == OBJ_UNUSED) {
        return;
    }
    render_object_sprite(orb);
    // render_object_sprite calls get_object_sprite_data which switches to bank 5
    // We need to switch back to bank 6 since this function is in bank 6
    set_prg_bank(6);
}

__attribute__((section(".prg_rom_6")))
void port_buildStaticDecor(uint8_t index) {
}

__attribute__((section(".prg_rom_6")))
void port_buildSpriteIfDirty(uint8_t index, enum eOBJType eType)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    // Player sprite is handled by port_updatePlayerSprite, skip here
    if (index == 0U) {
        obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
        return;
    }
    if (eType == OBJ_UNUSED) {
        obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
        return;
    }

    // BG-tile-only and no-op object types: only process when dirty.
    // These don't push OAM sprites, so rebuilding every frame just
    // wastes CPU cycles that are needed for vblank nametable updates.
    if (eType == OBJ_COLLAPSE_TILE || eType == OBJ_BREAKABLE_WALL ||
        eType == OBJ_MONUMENT || eType == OBJ_BIG_CHEST ||
        eType == OBJ_DECO_TREE || eType == OBJ_DECO_FLOWER ||
        eType == OBJ_SMOKE) {
        if ((obj->flags & OBJ_FLAG_DIRTY) == 0U) {
            return;
        }
        obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
        // Dispatch to their (mostly no-op) build functions
        if (eType == OBJ_COLLAPSE_TILE) {
            port_buildCollapseTile(index);
        } else if (eType == OBJ_BREAKABLE_WALL) {
            port_buildBreakableWall(index);
        } else if (eType == OBJ_MONUMENT) {
            port_buildMonument(index);
        } else if (eType == OBJ_BIG_CHEST) {
            port_buildBigChest(index);
        }
        // SMOKE, DECO_TREE, DECO_FLOWER have empty build functions
        return;
    }

    // Sprite-based objects: always rebuild every frame.
    // OAM slot assignments alternate direction each frame for flicker,
    // so every visible sprite must be re-pushed regardless of dirty flag.
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;

    if (eType == OBJ_DOUBLE_JUMP_ORB) {
        port_buildDoubleDashOrb(index);
    } else if (eType == OBJ_KEY) {
        port_buildKey(index);
    } else if (eType == OBJ_PLATMOV_R || eType == OBJ_PLATMOV_L) {
        port_buildPlatMov(index);
    } else if (eType == OBJ_SPRING) {
        port_buildSpring(index);
    } else if (eType == OBJ_CHEST) {
        port_buildChest(index);
    } else if (eType == OBJ_BALLOON) {
        port_buildBalloon(index);
    } else if (eType == OBJ_STRAWBERRY) {
        port_buildStrawberry(index);
    } else if (eType == OBJ_FLYING_BERRY) {
        port_buildFlyingBerry(index);
    }
}

void port_resetSprites(void) {
    s_oamCursor = OAM_PLAYER_END;
    // Hide all OAM entries
    for (uint16_t i = 0; i < 256; i += 4) {
        OAM_BUF[i] = 240;
    }
    s_inputState = 0;
}

__attribute__((section(".prg_rom_6")))
void port_drawText(const unsigned char *text, uint8_t x, uint8_t y) {
    (void)text;
    (void)x;
    (void)y;
}

static char (*volatile pad_poll_fn)(char) = pad_poll;
static void update_player_hair_color(void);
static void prepare_bg_tiles_nametable(BgTileWrite *writes, uint8_t *write_count);
static void execute_bg_tiles_nametable_writes(const BgTileWrite *writes, uint8_t write_count);

__attribute__((noinline)) void port_vblank(void) {
    int16_t playerY = (int16_t)(GLOBAL_PlayerData.objData.pos.y << 1);
    int16_t scrollCalc = playerY - 16 - (int16_t)GLOBAL_ActiveLevel.scrollPointY;
    s_scrollY = (uint8_t)CLAMP(scrollCalc, 0, 16);
    BgTileWrite bgTileWrites[MAX_BG_TILE_WRITES];
    uint8_t bgTileWriteCount = 0;
    set_prg_bank(6);
    prepare_bg_tiles_nametable(bgTileWrites, &bgTileWriteCount);
    ppu_wait_nmi();
    volatile uint8_t raw_state = (uint8_t)pad_poll_fn(0);
    __asm__ __volatile__("" ::: "memory");
    uint8_t mapped_state = 0;
    if (raw_state & PAD_A)      mapped_state |= PORT_INPUT_B_MASK;
    if (raw_state & PAD_B)      mapped_state |= PORT_INPUT_Y_MASK;
    if (raw_state & PAD_SELECT) mapped_state |= PORT_INPUT_SELECT_MASK;
    if (raw_state & PAD_START)  mapped_state |= PORT_INPUT_START_MASK;
    if (raw_state & PAD_UP)     mapped_state |= PORT_INPUT_UP_MASK;
    if (raw_state & PAD_DOWN)   mapped_state |= PORT_INPUT_DOWN_MASK;
    if (raw_state & PAD_LEFT)   mapped_state |= PORT_INPUT_LEFT_MASK;
    if (raw_state & PAD_RIGHT)  mapped_state |= PORT_INPUT_RIGHT_MASK;
    s_inputState = mapped_state;
    execute_bg_tiles_nametable_writes(bgTileWrites, bgTileWriteCount);
    // Animation tile writes are handled by NMI via set_vram_update — no manual flush needed.
    set_prg_bank(0);
    update_player_hair_color();

    // Use neslib's NMI-driven scroll and pattern table select.
    // These take effect at the START of the next vblank — guaranteed timing
    // even if game logic overruns the frame.
    int8_t shakeOffset = 0;
    if (GLOBAL_ActiveLevel.shakeFrames > 0) {
        shakeOffset = (s_frameCounter & 1) ? 2 : -2;
        s_frameCounter++;
    }
    // bank_bg: 0 = BG from $0000 (normal), 1 = BG from $1000 (level 4 animation)
    bank_bg(s_levelAnimActive ? 1 : 0);
    scroll(0, (unsigned)(uint8_t)CLAMP((int16_t)s_scrollY + shakeOffset, 0, 255));
}

uint8_t port_getInputs(void) {
    return s_inputState;
}

static uint8_t nes_6bit_to_palette_index(uint8_t nes_6bit) {
    return nes_6bit;
}

static inline uint8_t get_level_bank(uint16_t level_idx) {
    (void)level_idx;
    return 1; // All levels currently in bank 1
}

static void load_background_palettes(void) {
    for (uint8_t i = 0; i < 16; i++) palette_ram[i] = 0x0D;
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) level_idx = 0;
    set_prg_bank(1);
    const LevelData *level = &level_data[level_idx];
    for (uint8_t pal_idx = 0; pal_idx < 4; pal_idx++) {
        for (uint8_t col_idx = 0; col_idx < 4; col_idx++) {
            palette_ram[pal_idx * 4 + col_idx] = nes_6bit_to_palette_index(level->bg_palettes[pal_idx][col_idx]);
        }
    }
    pal_bg(palette_ram);
    set_prg_bank(6);
}

static void load_sprite_palettes(void) {
    for (uint8_t i = 0; i < 16; i++) palette_ram[16 + i] = 0x0D;
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) level_idx = 0;
    set_prg_bank(1);
    const LevelData *level = &level_data[level_idx];
    for (uint8_t pal_idx = 0; pal_idx < 4; pal_idx++) {
        for (uint8_t col_idx = 0; col_idx < 4; col_idx++) {
            palette_ram[16 + pal_idx * 4 + col_idx] = nes_6bit_to_palette_index(level->sprite_palettes[pal_idx][col_idx]);
        }
    }
    pal_spr(&palette_ram[16]);
    set_prg_bank(0);
}

static void update_player_hair_color(void) {
    uint8_t hair_color_index = 16 + 3 * 4 + 1;
    uint8_t dashes_left = GLOBAL_PlayerData.dashesLeft;
    uint8_t hair_color = (dashes_left == 0) ? 0x12 : ((dashes_left == 1) ? 0x15 : 0x1A);
    if (palette_ram[hair_color_index] != hair_color) {
        palette_ram[hair_color_index] = hair_color;
        pal_spr(&palette_ram[16]);
    }
}

static uint8_t get_palette_from_gid(uint8_t gid, const unsigned char (*gid_to_tile_map)[6], uint16_t gid_map_count) {
    if (gid >= gid_map_count || !gid_to_tile_map) return 0;
    return (gid_to_tile_map[gid][4] & 0x03);
}

static void write_nametable(void) {
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) level_idx = 0;
    set_prg_bank(1);  // Switch to bank 1 to access level_data array (all levels are in bank 1)
    const LevelData *level = &level_data[level_idx];
    uint8_t level_bank = get_level_bank(level_idx);  // Always returns 1
    // decompress_tilemap is in fixed bank, so it can be called from any bank
    // compressed_data pointer is in bank 1 (all levels are in bank 1)
    uint8_t *decompressed_tilemap = (uint8_t *)GLOBAL_OBJList;
    decompress_tilemap(level->tilemap_compressed, decompressed_tilemap, level_bank);
    // Switch to bank 5 to access gid_to_tile_shared (which is in bank 5)
    set_prg_bank(5);
    const unsigned char *tilemap_gids = decompressed_tilemap;
    const unsigned char (*gid_to_tile_map)[6] = GID_TO_TILE_MAP;
    uint16_t gid_map_count = GID_TO_TILE_MAP_COUNT;
    uint8_t total_nes_rows = LEVEL_HEIGHT * 2;
    
    // Write tiles to nametable 0 (rows 0-29)
    vram_adr(NTADR_A(0, 0));
    uint8_t nes_rows_nt0 = (total_nes_rows < 30) ? total_nes_rows : 30;
    for (uint8_t nes_y = 0; nes_y < nes_rows_nt0; nes_y++) {
        uint8_t map_y = nes_y / 2;
        uint8_t tile_row = nes_y % 2;
        for (uint8_t map_x = 0; map_x < LEVEL_WIDTH; map_x++) {
            uint16_t tilemap_idx = map_y * LEVEL_WIDTH + map_x;
            uint8_t gid = (tilemap_idx < LEVEL_TILE_COUNT) ? tilemap_gids[tilemap_idx] : 0;
            const unsigned char *tile_entry = (gid < gid_map_count && gid_to_tile_map) ? gid_to_tile_map[gid] : (gid_to_tile_map ? gid_to_tile_map[0] : NULL);
            if (tile_entry) {
                vram_put(tile_entry[tile_row * 2]);
                vram_put(tile_entry[tile_row * 2 + 1]);
            } else {
                vram_put(0); vram_put(0);
            }
        }
    }
    
    // Write tiles to nametable 2 (rows 30-31)
    if (total_nes_rows > 30) {
        vram_adr(0x2800);
        uint8_t nes_rows_nt2 = total_nes_rows - 30;
        for (uint8_t nes_y = 0; nes_y < nes_rows_nt2; nes_y++) {
            uint8_t map_y = (nes_y + 30) / 2;
            uint8_t tile_row = (nes_y + 30) % 2;
            for (uint8_t map_x = 0; map_x < LEVEL_WIDTH; map_x++) {
                uint16_t tilemap_idx = map_y * LEVEL_WIDTH + map_x;
                uint8_t gid = (tilemap_idx < LEVEL_TILE_COUNT) ? tilemap_gids[tilemap_idx] : 0;
                const unsigned char *tile_entry = (gid < gid_map_count && gid_to_tile_map) ? gid_to_tile_map[gid] : (gid_to_tile_map ? gid_to_tile_map[0] : NULL);
                if (tile_entry) {
                    vram_put(tile_entry[tile_row * 2]);
                    vram_put(tile_entry[tile_row * 2 + 1]);
                } else {
                    vram_put(0); vram_put(0);
                }
            }
        }
    }
    
    // Write attributes to nametable 0
    vram_adr(0x23C0);
    for (uint8_t attr_y = 0; attr_y < LEVEL_HEIGHT / 2; attr_y++) {
        for (uint8_t attr_x = 0; attr_x < LEVEL_WIDTH / 2; attr_x++) {
            uint16_t base_idx = (attr_y * 2) * LEVEL_WIDTH + (attr_x * 2);
            uint8_t tl_palette = get_palette_from_gid((base_idx < LEVEL_TILE_COUNT) ? tilemap_gids[base_idx] : 0, gid_to_tile_map, gid_map_count);
            uint8_t tr_palette = get_palette_from_gid((base_idx + 1 < LEVEL_TILE_COUNT) ? tilemap_gids[base_idx + 1] : 0, gid_to_tile_map, gid_map_count);
            uint8_t bl_palette = get_palette_from_gid((base_idx + LEVEL_WIDTH < LEVEL_TILE_COUNT) ? tilemap_gids[base_idx + LEVEL_WIDTH] : 0, gid_to_tile_map, gid_map_count);
            uint8_t br_palette = get_palette_from_gid((base_idx + LEVEL_WIDTH + 1 < LEVEL_TILE_COUNT) ? tilemap_gids[base_idx + LEVEL_WIDTH + 1] : 0, gid_to_tile_map, gid_map_count);
            vram_put(tl_palette | (tr_palette << 2) | (bl_palette << 4) | (br_palette << 6));
        }
    }
    
    // Write attributes to nametable 2
    // NT2 rows 0-1 display game row 15 (the last game tile row), so the
    // top quadrants (tl/tr) of NT2's first attribute row must use row 15's
    // palette. Bottom quadrants are beyond the level, left as palette 0.
    if (total_nes_rows > 30) {
        vram_adr(0x2BC0);
        uint8_t last_game_row = LEVEL_HEIGHT - 1;
        for (uint8_t attr_x = 0; attr_x < LEVEL_WIDTH / 2; attr_x++) {
            uint16_t base_idx = last_game_row * LEVEL_WIDTH + (attr_x * 2);
            uint8_t tl_palette = get_palette_from_gid((base_idx < LEVEL_TILE_COUNT) ? tilemap_gids[base_idx] : 0, gid_to_tile_map, gid_map_count);
            uint8_t tr_palette = get_palette_from_gid((base_idx + 1 < LEVEL_TILE_COUNT) ? tilemap_gids[base_idx + 1] : 0, gid_to_tile_map, gid_map_count);
            vram_put(tl_palette | (tr_palette << 2));
        }
    }
    set_prg_bank(0);  // Switch back to fixed bank before returning (write_nametable is in fixed bank)
}

static void fix_collapse_tile_palettes(const uint8_t *decompressed_tilemap) {
    for (uint8_t map_y = 0; map_y < LEVEL_HEIGHT; map_y++) {
        for (uint8_t map_x = 0; map_x < LEVEL_WIDTH; map_x++) {
            uint8_t gid = decompressed_tilemap[map_y * LEVEL_WIDTH + map_x];
            if (gid >= 24 && gid <= 26) {
                uint8_t attr_x = map_x / 2;
                uint8_t attr_y = map_y / 2;
                // In NT2, game row 15 is at the top (rows 0-1), so use top quadrants (0/1)
                uint8_t quadrant = (map_y == 15) ? (map_x & 1) : (((map_y & 1) << 1) | (map_x & 1));
                uint16_t attr_addr = (map_y == 15) ? (0x2BC0 + attr_x) : (0x23C0 + (attr_y * 8) + attr_x);
                vram_adr(attr_addr);
                (void)PPU_STATUS;
                (void)PPU_DATA;
                uint8_t attr_byte = PPU_DATA;
                uint8_t mask = 0x03 << (quadrant * 2);
                attr_byte = (attr_byte & ~mask) | (1 << (quadrant * 2));
                vram_adr(attr_addr);
                vram_put(attr_byte);
            }
        }
    }
}

// Must be in fixed bank — does internal bank switching to banks 1 and 5
void port_LoadRoomData(uint16_t roomID) {
    ppu_off(); // Turn off rendering immediately when reloading
    uint16_t level_idx = roomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) level_idx = 0;
    set_prg_bank(1);  // Switch to bank 1 to access level_data array (all levels are in bank 1)
    const LevelData *level = &level_data[level_idx];
    uint8_t level_bank = get_level_bank(level_idx);  // Always returns 1
    s_pendingBgTileMask = 0;
    GLOBAL_ActiveLevel.currentRoomID = roomID;
    GLOBAL_ActiveLevel.roomSizeX = LEVEL_WIDTH;
    GLOBAL_ActiveLevel.roomSizeY = LEVEL_HEIGHT;
    // decompress_tilemap is in fixed bank, so it can be called from any bank
    // compressed_data pointer is in bank 1 (all levels are in bank 1)
    uint8_t *decompressed_tilemap = (uint8_t *)GLOBAL_OBJList;
    decompress_tilemap(level->tilemap_compressed, decompressed_tilemap, level_bank);
    // Switch to bank 5 to access gid_to_collision (which is in bank 5)
    set_prg_bank(5);
    for (uint16_t i = 0; i < LEVEL_TILE_COUNT && i < 256; i++) {
        uint8_t gid = decompressed_tilemap[i];
        uint8_t collision_flag = (gid < GID_TO_COLLISION_COUNT) ? gid_to_collision[gid] : 0;
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = collision_flag;
    }
    // Switch back to bank 1 to access level_data array (all levels are in bank 1)
    set_prg_bank(1);
    const LevelData *level_for_struct = &level_data[level_idx];  // Re-get pointer while in bank 1
    GLOBAL_ActiveLevel.playerSpawnX = level_for_struct->spawn_x;
    GLOBAL_ActiveLevel.playerSpawnY = level_for_struct->spawn_y;
    GLOBAL_ActiveLevel.objectCount = level_for_struct->object_count;
    // level->objects is also in bank 1 (all levels are in bank 1)
    const LevelData *level_for_objects = &level_data[level_idx];  // Re-get pointer while in bank 1
    memcpy(GLOBAL_ActiveLevel.objectData, level_for_objects->objects, level_for_objects->object_count * 3);
    GLOBAL_ActiveLevel.scrollPointY = 72;
    // Deactivate level animation when leaving level 4
    // No CHR restore needed — animation tiles are at $1000, game tiles at $0000 are untouched.
    // PPU_CTRL will revert to default (BG from $0000) on next vblank.
    // Clear any NMI VRAM update buffer immediately (prevents stale writes during load)
    set_vram_update(0);
    if (s_levelAnimActive && roomID != 4) {
        s_levelAnimActive = 0;
        s_delta_active = 0;
    }
    if (roomID == 4) {
        // la_init_bank4 writes animation tiles to $1000.
        // port_vblank sets PPU_CTRL bit 4 so BG reads from $1000 during level 4.
        s_levelAnimActive = 1;
        s_delta_active = 0;
        set_prg_bank(LEVEL4_ANIM_BANK);
        la_init_bank4();
        // Copy collapse tile CHR patterns (indices 73-84) from game CHR to $1000
        // so they render correctly when the game updates nametable during animation.
        {
            extern const unsigned char game_chr_data[];
            set_prg_bank(5);  // game_chr_data is in bank 5
            for (uint8_t tile = 73; tile <= 84; tile++) {
                copy_to_ppu(0x1000 + (uint16_t)tile * 16, &game_chr_data[tile * 16], 16);
            }
        }
    } else {
        write_nametable();
        fix_collapse_tile_palettes(decompressed_tilemap);
        load_background_palettes();
    }
    load_sprite_palettes();
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    ppu_on_all();
    set_prg_bank(6); // Restore bank 6 for caller (LoadRoomData is in bank 6)
}

// Restore collision from ROM (death/respawn without full level reload)
// Must be in fixed bank — does internal bank switching
__attribute__((noinline)) void port_restoreCollisionFlags(void) {
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) level_idx = 0;
    set_prg_bank(1);
    const LevelData *level = &level_data[level_idx];
    uint8_t level_bank = get_level_bank(level_idx);
    uint8_t *buf = (uint8_t *)GLOBAL_OBJList;
    decompress_tilemap(level->tilemap_compressed, buf, level_bank);
    set_prg_bank(5);
    for (uint16_t i = 0; i < LEVEL_TILE_COUNT && i < 256; i++) {
        uint8_t gid = buf[i];
        uint8_t cf = (gid < GID_TO_COLLISION_COUNT) ? gid_to_collision[gid] : 0;
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = cf;
    }
    set_prg_bank(6); // Restore caller's bank (playerInit is in bank 6)
}
