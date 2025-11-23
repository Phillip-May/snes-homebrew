#include "port.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>


#include "../../python/tilemap_level1_nes.h"

#ifndef _WIN32 //Fix linter
#include <neslib.h>
#endif

#define OAM_ADDR   (*(volatile uint8_t *)0x2003)
#define OAM_DATA   (*(volatile uint8_t *)0x2004)
#define OAM_DMA    (*(volatile uint8_t *)0x4014)

extern uint8_t OAM_BUF[];

static volatile uint8_t s_inputState = 0;
static uint8_t s_oamIndex = 0;

extern struct sActiveLevelData GLOBAL_ActiveLevel;
extern struct sPlayerData GLOBAL_PlayerData;
extern OBJ_DATA GLOBAL_OBJList[];

static void oam_upload(void) {
    OAM_ADDR = 0x00;
    OAM_DMA = (uint8_t)(((uintptr_t)OAM_BUF) >> 8);
}



void port_init(void) {
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;

    ppu_off();
    oam_clear();
    oam_size(0);
    
    // Enable both background and sprites
    // PPU control register will use nametable 0 (default)
    ppu_on_all();
    
    // Ensure palettes are set after PPU is on (in case they got cleared)
    ppu_wait_nmi();

}

void port_beginSpriteBuild(const struct sPlayerData *playerObj) {
    (void)playerObj;
    s_oamIndex = 0;
    oam_set(0);
}

void port_finishSpriteBuild(void) {

}

void port_updatePlayerSprite(const struct sPlayerData *playerObj) {
    if (playerObj == NULL) {
        return;
    }
    
    const struct sOBJ_DATA *playerData = &playerObj->objData;
    uint8_t baseX = (uint8_t)playerData->pos.x;
    uint8_t baseY = (uint8_t)playerData->pos.y;
    uint8_t baseTile = playerData->oamTile;
    uint8_t baseProps = playerData->oamProps;
    // Clear bit 5 (0x20) to make sprite appear in front of background
    // Bit 5 = 0: in front, Bit 5 = 1: behind
    baseProps &= ~0x20;
    
    // Sprite 0: Top-left
    OAM_BUF[0] = baseY;
    OAM_BUF[1] = baseTile;
    OAM_BUF[2] = baseProps;
    OAM_BUF[3] = baseX;
    
    // Sprite 1: Top-right
    OAM_BUF[4] = baseY;
    OAM_BUF[5] = baseTile + 1;
    OAM_BUF[6] = baseProps;
    OAM_BUF[7] = baseX + 8;
    
    // Sprite 2: Bottom-left
    OAM_BUF[8] = baseY + 8;
    OAM_BUF[9] = baseTile + 16;  // Assuming tiles are arranged in 16-tile rows
    OAM_BUF[10] = baseProps;
    OAM_BUF[11] = baseX;
    
    // Sprite 3: Bottom-right
    OAM_BUF[12] = baseY + 8;
    OAM_BUF[13] = baseTile + 17;  // Assuming tiles are arranged in 16-tile rows
    OAM_BUF[14] = baseProps;
    OAM_BUF[15] = baseX + 8;
}

void port_buildUnused(uint8_t index) {
}

void port_buildSmoke(uint8_t index) {
}

void port_buildBreakableWall(uint8_t index) {
}

void port_buildBalloon(uint8_t index) {
}

void port_buildMonument(uint8_t index) {
}

void port_buildChest(uint8_t index) {
}

void port_buildBigChest(uint8_t index) {
}

void port_buildKey(uint8_t index) {
}

void port_buildSpring(uint8_t index) {
}

void port_buildCollapseTile(uint8_t index) {
}

void port_buildStrawberry(uint8_t index) {
}

void port_buildPlatMov(uint8_t index) {
}

void port_buildFlyingBerry(uint8_t index) {
}

void port_buildDoubleDashOrb(uint8_t index) {
}

void port_buildStaticDecor(uint8_t index) {
}

void port_resetSprites(void) {
    oam_hide_rest();
    s_inputState = 0;
}

void port_drawText(const unsigned char *text, uint8_t x, uint8_t y) {
    (void)text;
    (void)x;
    (void)y;
}

// Function pointer to pad_poll - prevents compiler from optimizing away the call
static char (*volatile pad_poll_fn)(char) = pad_poll;

__attribute__((noinline)) void port_vblank(void) {
    ppu_wait_nmi();
    oam_upload();

    // Read controller - use volatile function pointer to prevent optimization
    volatile uint8_t raw_state = (uint8_t)pad_poll_fn(0);
    
    // Memory barrier to prevent optimization from reordering/eliminating pad read
    __asm__ __volatile__("" ::: "memory");

    uint8_t mapped_state = 0;
    if (raw_state & PAD_A)      { mapped_state |= PORT_INPUT_B_MASK; }       // NES A
    if (raw_state & PAD_B)      { mapped_state |= PORT_INPUT_Y_MASK; }       // NES B
    if (raw_state & PAD_SELECT) { mapped_state |= PORT_INPUT_SELECT_MASK; }
    if (raw_state & PAD_START)  { mapped_state |= PORT_INPUT_START_MASK; }
    if (raw_state & PAD_UP)     { mapped_state |= PORT_INPUT_UP_MASK; }
    if (raw_state & PAD_DOWN)   { mapped_state |= PORT_INPUT_DOWN_MASK; }
    if (raw_state & PAD_LEFT)   { mapped_state |= PORT_INPUT_LEFT_MASK; }
    if (raw_state & PAD_RIGHT)  { mapped_state |= PORT_INPUT_RIGHT_MASK; }

    // Force write to volatile variable to prevent optimization
    s_inputState = mapped_state;
}

uint8_t port_getInputs(void) {
    return s_inputState;
}

// Convert NES 6-bit color to neslib palette format
// NES 6-bit format: RRGGBB (2 bits per component)
// neslib expects color indices (0-63) into the NES color palette
static uint8_t nes_6bit_to_palette_index(uint8_t nes_6bit) {
    // NES 6-bit color is already in the correct format for direct use
    // The NES PPU uses this directly as a color index
    return nes_6bit;
}

// Load background palettes from tilemap data
static void load_background_palettes(void) {
    // NES has 4 background palettes, each with 4 colors
    // First color (index 0) is always the universal background color
    
    // Convert palette data to neslib format
    // neslib pal_bg expects an array of 16 bytes (4 palettes * 4 colors)
    // Format: [pal0_color0, pal0_color1, pal0_color2, pal0_color3, pal1_color0, ...]
    static uint8_t bg_palette[16];
    
    // Initialize with universal background color (0x0D = black, matching Python generation)
    // The first color of each palette (index 0) should be 0x0D for consistency
    for (uint8_t i = 0; i < 16; i++) {
        bg_palette[i] = 0x0D; // Universal background color (matches Python: 0x0D)
    }
    
    // Load background palettes from tilemap data (up to 4 palettes)
    uint8_t pal_count = PALETTE_BACKGROUND_LEVEL1_COUNT;
    if (pal_count > 4) pal_count = 4;
    
    for (uint8_t pal_idx = 0; pal_idx < pal_count; pal_idx++) {
        // Each palette has 4 colors
        for (uint8_t col_idx = 0; col_idx < 4; col_idx++) {
            uint8_t nes_6bit = palette_background_level1[pal_idx][col_idx];
            // Use the palette data directly - it's already in NES palette index format (0-63)
            bg_palette[pal_idx * 4 + col_idx] = nes_6bit_to_palette_index(nes_6bit);
        }
    }
    
    pal_bg(bg_palette);
}

// Load sprite palettes from tilemap data
static void load_sprite_palettes(void) {
    // NES has 4 sprite palettes, each with 4 colors
    // Note: Color 0 in sprite palettes is transparent, not black
    
    // Convert palette data to neslib format
    // neslib pal_spr expects an array of 16 bytes (4 palettes * 4 colors)
    // Format: [pal0_color0, pal0_color1, pal0_color2, pal0_color3, pal1_color0, ...]
    static uint8_t spr_palette[16];
    
    // Initialize with default colors (transparent for color 0)
    // Color 0 in sprite palettes is transparent, but we initialize to 0x0D for consistency
    for (uint8_t i = 0; i < 16; i++) {
        spr_palette[i] = 0x0D; // Default to black (matches Python: 0x0D)
    }
    
    // Load sprite palettes from tilemap data (up to 4 palettes)
    uint8_t pal_count = PALETTE_SPRITE_LEVEL1_COUNT;
    if (pal_count > 4) pal_count = 4;
    
    for (uint8_t pal_idx = 0; pal_idx < pal_count; pal_idx++) {
        // Each palette has 4 colors
        for (uint8_t col_idx = 0; col_idx < 4; col_idx++) {
            uint8_t nes_6bit = palette_sprite_level1[pal_idx][col_idx];
            spr_palette[pal_idx * 4 + col_idx] = nes_6bit_to_palette_index(nes_6bit);
        }
    }
    
    pal_spr(spr_palette);
}

// Write nametable from tilemap data
static void write_nametable(void) {
    // NES nametable is 32x30 tiles (256x240 pixels)
    // Our tilemap is 16x16 tiles, each tile is 16x16 pixels (2x2 NES tiles)
    // So we'll write a 32x32 NES tile area (though NES only supports 32x30)
    
    const uint8_t map_width = TILEMAP_LEVEL1_WIDTH;  // 16
    const uint8_t map_height = TILEMAP_LEVEL1_HEIGHT; // 16
    
    // First, write all tile indices to the nametable
    // Start at nametable 0, position (0, 0)
    vram_adr(NTADR_A(0, 0));
    
    // Write tile indices row by row
    // Each map tile becomes 2 rows of NES tiles
    for (uint8_t nes_y = 0; nes_y < map_height * 2; nes_y++) {
        uint8_t map_y = nes_y / 2;
        uint8_t tile_row = nes_y % 2; // 0 = top row, 1 = bottom row
        
        for (uint8_t map_x = 0; map_x < map_width; map_x++) {
            uint16_t tilemap_idx = map_y * map_width + map_x;
            const unsigned char *tile_entry = &tilemap_level1[tilemap_idx * 6];
            
            // Check if this is an empty tile (all zeros)
            // Empty tiles are written as tile 0, which should be transparent/empty
            if (tile_row == 0) {
                // Top row: write left and right tiles
                vram_put(tile_entry[0]); // TL
                vram_put(tile_entry[1]); // TR
            } else {
                // Bottom row: write left and right tiles
                vram_put(tile_entry[2]); // BL
                vram_put(tile_entry[3]); // BR
            }
        }
    }
    
    // Now write attributes to the attribute table
    // Attribute table is at 0x23C0 in nametable 0
    // Each attribute byte controls a 4x4 NES tile area (32x32 pixels)
    // NES attribute format: [BR][BL][TR][TL] (2 bits each for palette, 0-3)
    // Bits 0-1: palette for top-left 2x2 NES tiles
    // Bits 2-3: palette for top-right 2x2 NES tiles
    // Bits 4-5: palette for bottom-left 2x2 NES tiles
    // Bits 6-7: palette for bottom-right 2x2 NES tiles
    
    vram_adr(0x23C0); // Attribute table start
    
    // Write attributes - each attribute byte covers 4x4 NES tiles = 2x2 map tiles
    // So for a 16x16 map (32x32 NES tiles), we need 8x8 attribute bytes
    for (uint8_t attr_y = 0; attr_y < map_height / 2; attr_y++) {
        for (uint8_t attr_x = 0; attr_x < map_width / 2; attr_x++) {
            // Get the 4 map tiles that this attribute byte covers
            // Top-left
            uint16_t tl_idx = (attr_y * 2) * map_width + (attr_x * 2);
            uint8_t tl_palette = 0;
            if (tl_idx < TILEMAP_LEVEL1_COUNT) {
                uint8_t palette_idx = tilemap_level1[tl_idx * 6 + 4];
                // Extract palette index (bits 0-1) for both background (bit 7=1) and sprite (bit 7=0) palettes
                // Since NES attribute table only supports background palettes, sprite palette indices
                // are mapped to corresponding background palette indices (0-3)
                tl_palette = palette_idx & 0x03;
            }
            
            // Top-right
            uint16_t tr_idx = (attr_y * 2) * map_width + (attr_x * 2) + 1;
            uint8_t tr_palette = 0;
            if (tr_idx < TILEMAP_LEVEL1_COUNT) {
                uint8_t palette_idx = tilemap_level1[tr_idx * 6 + 4];
                tr_palette = palette_idx & 0x03;
            }
            
            // Bottom-left
            uint16_t bl_idx = ((attr_y * 2) + 1) * map_width + (attr_x * 2);
            uint8_t bl_palette = 0;
            if (bl_idx < TILEMAP_LEVEL1_COUNT) {
                uint8_t palette_idx = tilemap_level1[bl_idx * 6 + 4];
                bl_palette = palette_idx & 0x03;
            }
            
            // Bottom-right
            uint16_t br_idx = ((attr_y * 2) + 1) * map_width + (attr_x * 2) + 1;
            uint8_t br_palette = 0;
            if (br_idx < TILEMAP_LEVEL1_COUNT) {
                uint8_t palette_idx = tilemap_level1[br_idx * 6 + 4];
                br_palette = palette_idx & 0x03;
            }
            
            // Build attribute byte: [BR][BL][TR][TL]
            uint8_t attr_byte = tl_palette | 
                               (tr_palette << 2) |
                               (bl_palette << 4) |
                               (br_palette << 6);
            
            vram_put(attr_byte);
        }
    }
}

void port_LoadRoomData(uint16_t roomID) {
    if (roomID == 1) {
        // Turn off PPU during loading
        ppu_off();
        
        // Set room ID and size
        GLOBAL_ActiveLevel.currentRoomID = roomID;
        GLOBAL_ActiveLevel.roomSizeX = TILEMAP_LEVEL1_WIDTH;
        GLOBAL_ActiveLevel.roomSizeY = TILEMAP_LEVEL1_HEIGHT;
        
        // Copy collision data
        memcpy(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level1, COLLISION_LEVEL1_COUNT);
        memcpy(GLOBAL_ActiveLevel.collisionFlagsArr, collision_level1, COLLISION_LEVEL1_COUNT);
        
        // Set player spawn position
        GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL1;
        GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL1;
        
        // Set object count and copy object data
        GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL1_COUNT;
        memcpy(GLOBAL_ActiveLevel.objectData, object_level1, OBJECT_LEVEL1_COUNT * 3);
        
        // Set scroll point (from SNES implementation)
        GLOBAL_ActiveLevel.scrollPointY = 72;
        
        // Write nametable first (tiles and attributes)
        write_nametable();
        
        // Load background palettes (after nametable, so they're ready when PPU turns on)
        load_background_palettes();
        
        // Load sprite palettes
        load_sprite_palettes();
        
        // Mark level as loaded
        GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
        
        // Turn PPU back on
        ppu_on_all();
        ppu_wait_nmi();
    }
}

