#include "port.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>


#include "../../python/gid_to_tile_shared.h"
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

#ifndef _WIN32 //Fix linter
#include <neslib.h>
#endif

#define PPU_CTRL   (*(volatile uint8_t *)0x2000)
#define PPU_SCROLL (*(volatile uint8_t *)0x2005)
#define OAM_ADDR   (*(volatile uint8_t *)0x2003)
#define OAM_DATA   (*(volatile uint8_t *)0x2004)
#define OAM_DMA    (*(volatile uint8_t *)0x4014)

extern uint8_t OAM_BUF[];

static volatile uint8_t s_inputState = 0;
static uint8_t s_oamIndex = 0;
static uint8_t s_scrollY = 0; // Vertical scroll position

extern struct sActiveLevelData GLOBAL_ActiveLevel;
extern struct sPlayerData GLOBAL_PlayerData;
extern OBJ_DATA GLOBAL_OBJList[];

// Centralized level data structure
// Comment out levels here to test ROM size
typedef struct {
    uint8_t width;
    uint8_t height;
    const unsigned char *tilemap;
    uint16_t tilemap_count;
    // Collision data is now generated from tilemap GIDs, so this field is unused
    // Keeping for compatibility but it will be NULL
    const unsigned char *collision;
    uint16_t collision_count;
    const unsigned char *objects;
    uint16_t object_count;
    const unsigned char *object_sprites;
    uint16_t object_sprite_count;
    const unsigned char (*bg_palettes)[4];
    uint8_t bg_palette_count;
    const unsigned char (*sprite_palettes)[4];
    uint8_t sprite_palette_count;
    uint8_t spawn_x;
    uint8_t spawn_y;
} LevelData;

// Level data table - comment out levels here to exclude them from ROM
static const LevelData level_data[] = {
    // Level 1
    {
        TILEMAP_LEVEL1_WIDTH, TILEMAP_LEVEL1_HEIGHT,
        tilemap_level1, TILEMAP_LEVEL1_COUNT,
        NULL, TILEMAP_LEVEL1_COUNT, // Collision generated from tilemap
        object_level1, OBJECT_LEVEL1_COUNT,
        object_sprite_level1, OBJECT_SPRITE_LEVEL1_COUNT,
        palette_background_level1, PALETTE_BACKGROUND_LEVEL1_COUNT,
        palette_sprite_level1, PALETTE_SPRITE_LEVEL1_COUNT,
        SPAWN_X_LEVEL1, SPAWN_Y_LEVEL1
    },
    // Level 2
    {
        TILEMAP_LEVEL2_WIDTH, TILEMAP_LEVEL2_HEIGHT,
        tilemap_level2, TILEMAP_LEVEL2_COUNT,
        NULL, TILEMAP_LEVEL2_COUNT, // Collision generated from tilemap
        object_level2, OBJECT_LEVEL2_COUNT,
        object_sprite_level2, OBJECT_SPRITE_LEVEL2_COUNT,
        palette_background_level2, PALETTE_BACKGROUND_LEVEL2_COUNT,
        palette_sprite_level2, PALETTE_SPRITE_LEVEL2_COUNT,
        SPAWN_X_LEVEL2, SPAWN_Y_LEVEL2
    },
    // Level 3
    {
        TILEMAP_LEVEL3_WIDTH, TILEMAP_LEVEL3_HEIGHT,
        tilemap_level3, TILEMAP_LEVEL3_COUNT,
        NULL, TILEMAP_LEVEL3_COUNT, // Collision generated from tilemap
        object_level3, OBJECT_LEVEL3_COUNT,
        object_sprite_level3, OBJECT_SPRITE_LEVEL3_COUNT,
        palette_background_level3, PALETTE_BACKGROUND_LEVEL3_COUNT,
        palette_sprite_level3, PALETTE_SPRITE_LEVEL3_COUNT,
        SPAWN_X_LEVEL3, SPAWN_Y_LEVEL3
    },
    // Level 4
    {
        TILEMAP_LEVEL4_WIDTH, TILEMAP_LEVEL4_HEIGHT,
        tilemap_level4, TILEMAP_LEVEL4_COUNT,
        NULL, TILEMAP_LEVEL4_COUNT, // Collision generated from tilemap
        object_level4, OBJECT_LEVEL4_COUNT,
        object_sprite_level4, OBJECT_SPRITE_LEVEL4_COUNT,
        palette_background_level4, PALETTE_BACKGROUND_LEVEL4_COUNT,
        palette_sprite_level4, PALETTE_SPRITE_LEVEL4_COUNT,
        SPAWN_X_LEVEL4, SPAWN_Y_LEVEL4
    },
    // Level 5
    {
        TILEMAP_LEVEL5_WIDTH, TILEMAP_LEVEL5_HEIGHT,
        tilemap_level5, TILEMAP_LEVEL5_COUNT,
        NULL, TILEMAP_LEVEL5_COUNT, // Collision generated from tilemap
        object_level5, OBJECT_LEVEL5_COUNT,
        object_sprite_level5, OBJECT_SPRITE_LEVEL5_COUNT,
        palette_background_level5, PALETTE_BACKGROUND_LEVEL5_COUNT,
        palette_sprite_level5, PALETTE_SPRITE_LEVEL5_COUNT,
        SPAWN_X_LEVEL5, SPAWN_Y_LEVEL5
    },
    // Level 6
    {
        TILEMAP_LEVEL6_WIDTH, TILEMAP_LEVEL6_HEIGHT,
        tilemap_level6, TILEMAP_LEVEL6_COUNT,
        NULL, TILEMAP_LEVEL6_COUNT, // Collision generated from tilemap
        object_level6, OBJECT_LEVEL6_COUNT,
        object_sprite_level6, OBJECT_SPRITE_LEVEL6_COUNT,
        palette_background_level6, PALETTE_BACKGROUND_LEVEL6_COUNT,
        palette_sprite_level6, PALETTE_SPRITE_LEVEL6_COUNT,
        SPAWN_X_LEVEL6, SPAWN_Y_LEVEL6
    },
    // Level 7
    {
        TILEMAP_LEVEL7_WIDTH, TILEMAP_LEVEL7_HEIGHT,
        tilemap_level7, TILEMAP_LEVEL7_COUNT,
        NULL, TILEMAP_LEVEL7_COUNT, // Collision generated from tilemap
        object_level7, OBJECT_LEVEL7_COUNT,
        object_sprite_level7, OBJECT_SPRITE_LEVEL7_COUNT,
        palette_background_level7, PALETTE_BACKGROUND_LEVEL7_COUNT,
        palette_sprite_level7, PALETTE_SPRITE_LEVEL7_COUNT,
        SPAWN_X_LEVEL7, SPAWN_Y_LEVEL7
    },
    // Level 8
    {
        TILEMAP_LEVEL8_WIDTH, TILEMAP_LEVEL8_HEIGHT,
        tilemap_level8, TILEMAP_LEVEL8_COUNT,
        NULL, TILEMAP_LEVEL8_COUNT, // Collision generated from tilemap
        object_level8, OBJECT_LEVEL8_COUNT,
        object_sprite_level8, OBJECT_SPRITE_LEVEL8_COUNT,
        palette_background_level8, PALETTE_BACKGROUND_LEVEL8_COUNT,
        palette_sprite_level8, PALETTE_SPRITE_LEVEL8_COUNT,
        SPAWN_X_LEVEL8, SPAWN_Y_LEVEL8
    },
    // Level 9
    {
        TILEMAP_LEVEL9_WIDTH, TILEMAP_LEVEL9_HEIGHT,
        tilemap_level9, TILEMAP_LEVEL9_COUNT,
        NULL, TILEMAP_LEVEL9_COUNT, // Collision generated from tilemap
        object_level9, OBJECT_LEVEL9_COUNT,
        object_sprite_level9, OBJECT_SPRITE_LEVEL9_COUNT,
        palette_background_level9, PALETTE_BACKGROUND_LEVEL9_COUNT,
        palette_sprite_level9, PALETTE_SPRITE_LEVEL9_COUNT,
        SPAWN_X_LEVEL9, SPAWN_Y_LEVEL9
    },
    // Level 10
    {
        TILEMAP_LEVEL10_WIDTH, TILEMAP_LEVEL10_HEIGHT,
        tilemap_level10, TILEMAP_LEVEL10_COUNT,
        NULL, TILEMAP_LEVEL10_COUNT, // Collision generated from tilemap
        object_level10, OBJECT_LEVEL10_COUNT,
        object_sprite_level10, OBJECT_SPRITE_LEVEL10_COUNT,
        palette_background_level10, PALETTE_BACKGROUND_LEVEL10_COUNT,
        palette_sprite_level10, PALETTE_SPRITE_LEVEL10_COUNT,
        SPAWN_X_LEVEL10, SPAWN_Y_LEVEL10
    },
    // Level 11
    {
        TILEMAP_LEVEL11_WIDTH, TILEMAP_LEVEL11_HEIGHT,
        tilemap_level11, TILEMAP_LEVEL11_COUNT,
        NULL, TILEMAP_LEVEL11_COUNT, // Collision generated from tilemap
        object_level11, OBJECT_LEVEL11_COUNT,
        object_sprite_level11, OBJECT_SPRITE_LEVEL11_COUNT,
        palette_background_level11, PALETTE_BACKGROUND_LEVEL11_COUNT,
        palette_sprite_level11, PALETTE_SPRITE_LEVEL11_COUNT,
        SPAWN_X_LEVEL11, SPAWN_Y_LEVEL11
    },
    // Level 12
    {
        TILEMAP_LEVEL12_WIDTH, TILEMAP_LEVEL12_HEIGHT,
        tilemap_level12, TILEMAP_LEVEL12_COUNT,
        NULL, TILEMAP_LEVEL12_COUNT, // Collision generated from tilemap
        object_level12, OBJECT_LEVEL12_COUNT,
        object_sprite_level12, OBJECT_SPRITE_LEVEL12_COUNT,
        palette_background_level12, PALETTE_BACKGROUND_LEVEL12_COUNT,
        palette_sprite_level12, PALETTE_SPRITE_LEVEL12_COUNT,
        SPAWN_X_LEVEL12, SPAWN_Y_LEVEL12
    },
    // Level 13
    {
        TILEMAP_LEVEL13_WIDTH, TILEMAP_LEVEL13_HEIGHT,
        tilemap_level13, TILEMAP_LEVEL13_COUNT,
        NULL, TILEMAP_LEVEL13_COUNT, // Collision generated from tilemap
        object_level13, OBJECT_LEVEL13_COUNT,
        object_sprite_level13, OBJECT_SPRITE_LEVEL13_COUNT,
        palette_background_level13, PALETTE_BACKGROUND_LEVEL13_COUNT,
        palette_sprite_level13, PALETTE_SPRITE_LEVEL13_COUNT,
        SPAWN_X_LEVEL13, SPAWN_Y_LEVEL13
    },
    // Level 14
    {
        TILEMAP_LEVEL14_WIDTH, TILEMAP_LEVEL14_HEIGHT,
        tilemap_level14, TILEMAP_LEVEL14_COUNT,
        NULL, TILEMAP_LEVEL14_COUNT, // Collision generated from tilemap
        object_level14, OBJECT_LEVEL14_COUNT,
        object_sprite_level14, OBJECT_SPRITE_LEVEL14_COUNT,
        palette_background_level14, PALETTE_BACKGROUND_LEVEL14_COUNT,
        palette_sprite_level14, PALETTE_SPRITE_LEVEL14_COUNT,
        SPAWN_X_LEVEL14, SPAWN_Y_LEVEL14
    },
    /*
    // Level 15
    {
        TILEMAP_LEVEL15_WIDTH, TILEMAP_LEVEL15_HEIGHT,
        tilemap_level15, TILEMAP_LEVEL15_COUNT,
        NULL, TILEMAP_LEVEL15_COUNT, // Collision generated from tilemap
        object_level15, OBJECT_LEVEL15_COUNT,
        object_sprite_level15, OBJECT_SPRITE_LEVEL15_COUNT,
        palette_background_level15, PALETTE_BACKGROUND_LEVEL15_COUNT,
        palette_sprite_level15, PALETTE_SPRITE_LEVEL15_COUNT,
        SPAWN_X_LEVEL15, SPAWN_Y_LEVEL15
    },
    // Level 16
    {
        TILEMAP_LEVEL16_WIDTH, TILEMAP_LEVEL16_HEIGHT,
        tilemap_level16, TILEMAP_LEVEL16_COUNT,
        NULL, TILEMAP_LEVEL16_COUNT, // Collision generated from tilemap
        object_level16, OBJECT_LEVEL16_COUNT,
        object_sprite_level16, OBJECT_SPRITE_LEVEL16_COUNT,
        palette_background_level16, PALETTE_BACKGROUND_LEVEL16_COUNT,
        palette_sprite_level16, PALETTE_SPRITE_LEVEL16_COUNT,
        SPAWN_X_LEVEL16, SPAWN_Y_LEVEL16
    },
    // Level 17
    {
        TILEMAP_LEVEL17_WIDTH, TILEMAP_LEVEL17_HEIGHT,
        tilemap_level17, TILEMAP_LEVEL17_COUNT,
        NULL, TILEMAP_LEVEL17_COUNT, // Collision generated from tilemap
        object_level17, OBJECT_LEVEL17_COUNT,
        object_sprite_level17, OBJECT_SPRITE_LEVEL17_COUNT,
        palette_background_level17, PALETTE_BACKGROUND_LEVEL17_COUNT,
        palette_sprite_level17, PALETTE_SPRITE_LEVEL17_COUNT,
        SPAWN_X_LEVEL17, SPAWN_Y_LEVEL17
    },
    // Level 18
    {
        TILEMAP_LEVEL18_WIDTH, TILEMAP_LEVEL18_HEIGHT,
        tilemap_level18, TILEMAP_LEVEL18_COUNT,
        NULL, TILEMAP_LEVEL18_COUNT, // Collision generated from tilemap
        object_level18, OBJECT_LEVEL18_COUNT,
        object_sprite_level18, OBJECT_SPRITE_LEVEL18_COUNT,
        palette_background_level18, PALETTE_BACKGROUND_LEVEL18_COUNT,
        palette_sprite_level18, PALETTE_SPRITE_LEVEL18_COUNT,
        SPAWN_X_LEVEL18, SPAWN_Y_LEVEL18
    },
    // Level 19
    {
        TILEMAP_LEVEL19_WIDTH, TILEMAP_LEVEL19_HEIGHT,
        tilemap_level19, TILEMAP_LEVEL19_COUNT,
        NULL, TILEMAP_LEVEL19_COUNT, // Collision generated from tilemap
        object_level19, OBJECT_LEVEL19_COUNT,
        object_sprite_level19, OBJECT_SPRITE_LEVEL19_COUNT,
        palette_background_level19, PALETTE_BACKGROUND_LEVEL19_COUNT,
        palette_sprite_level19, PALETTE_SPRITE_LEVEL19_COUNT,
        SPAWN_X_LEVEL19, SPAWN_Y_LEVEL19
    },
    // Level 20
    {
        TILEMAP_LEVEL20_WIDTH, TILEMAP_LEVEL20_HEIGHT,
        tilemap_level20, TILEMAP_LEVEL20_COUNT,
        NULL, TILEMAP_LEVEL20_COUNT, // Collision generated from tilemap
        object_level20, OBJECT_LEVEL20_COUNT,
        object_sprite_level20, OBJECT_SPRITE_LEVEL20_COUNT,
        palette_background_level20, PALETTE_BACKGROUND_LEVEL20_COUNT,
        palette_sprite_level20, PALETTE_SPRITE_LEVEL20_COUNT,
        SPAWN_X_LEVEL20, SPAWN_Y_LEVEL20
    },
    */
};

#define LEVEL_DATA_COUNT (sizeof(level_data) / sizeof(level_data[0]))

// Shared GID mapping (used by all levels)
#define GID_TO_TILE_MAP gid_to_tile_shared
#define GID_TO_TILE_MAP_COUNT GID_TO_TILE_SHARED_COUNT
#define GID_TO_COLLISION_COUNT 72

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
    ppu_on_all();
    
    // Configure PPU for horizontal mirroring
    // This allows two unique vertical nametables ($2000 and $2800)
    // With horizontal mirroring: $2000 mirrors to $2400, $2800 mirrors to $2C00
    // PPU_CTRL bit 0 selects base nametable (0=$2000/$2400, 1=$2800/$2C00)
    // We start with nametable 0, but the system can use both with proper scrolling
    // Note: Actual mirroring is typically controlled by mapper hardware,
    // but setting PPU_CTRL bit 0 appropriately helps with nametable selection
    // Set after ppu_on_all() to ensure our setting isn't overwritten
    // PPU_CTRL is write-only, so we set it directly with NMI enabled and nametable $2000
    // Bit 7 = NMI enable, bit 0 = nametable base (0 = $2000 for horizontal mirroring)
    PPU_CTRL = 0x88; // NMI enabled (bit 7), sprite pattern $1000 (bit 3), nametable $2000 (bit 0 = 0)
    
    // Ensure palettes are set after PPU is on (in case they got cleared)
    ppu_wait_nmi();

}

void port_beginSpriteBuild(const struct sPlayerData *playerObj) {
    (void)playerObj;
    // Player uses slots 0-3 (4 sprites), so start other objects at slot 4
    s_oamIndex = 4;
    oam_set(0);
}

void port_finishSpriteBuild(void) {

}

// Map PLAYER_SPRITE_ enum values to sprite sheet tile indices
// PLAYER_SPRITE_ macros: IDLE=0, WALK_1=2, WALK_2=4, WALK_3=6, WALL=8, DOWN=10, UP=12
// Sprite sheet tile indices: 1, 2, 3, 4, 5, 6, 7
static uint8_t map_player_sprite_to_tile_index(uint8_t player_sprite_state) {
    // Map PLAYER_SPRITE_ enum to sprite sheet tile index (1-7)
    switch (player_sprite_state) {
        case 0:  // PLAYER_SPRITE_IDLE
            return 1;
        case 2:  // PLAYER_SPRITE_WALK_1
            return 2;
        case 4:  // PLAYER_SPRITE_WALK_2
            return 3;
        case 6:  // PLAYER_SPRITE_WALK_3
            return 4;
        case 8:  // PLAYER_SPRITE_WALL
            return 5;
        case 10: // PLAYER_SPRITE_DOWN
            return 6;
        case 12: // PLAYER_SPRITE_UP
            return 7;
        default:
            return 1; // Default to IDLE
    }
}

// Look up player sprite data from object_sprite array
// Player frames: IDLE=0, WALK_1=2, WALK_2=4, WALK_3=6, WALL=8, DOWN=10, UP=12
// Maps to sprite sheet tile indices: 1, 2, 3, 4, 5, 6, 7
static const unsigned char* find_player_sprite_data(uint8_t frame_index) {
    // Map PLAYER_SPRITE_ enum value to sprite sheet tile index
    uint8_t tile_index = map_player_sprite_to_tile_index(frame_index);
    
    // Search through object_sprite array for the mapped tile index based on current level
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    // Get level data (room ID is 1-indexed, array is 0-indexed)
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) {
        level_idx = 0; // Default to level 1 if out of range
    }
    const LevelData *level = &level_data[level_idx];
    
    for (uint8_t i = 0; i < level->object_sprite_count; i++) {
        const unsigned char *sprite_entry = &level->object_sprites[i * 6];
        if (sprite_entry[0] == tile_index) {  // Match the mapped tile index
            return sprite_entry;
        }
    }
    return NULL;  // Player sprite data not found
}

// Map SPRING_SPRITE_ OAM tile values to sprite sheet tile indices
// SPRING_SPRITE_1 = 0x2E (46), SPRING_SPRITE_2 = 0x40 (64)
// Maps to sprite sheet tile indices: 23, 24 (or similar, adjust based on actual data)
static uint8_t map_spring_sprite_to_tile_index(uint8_t spring_sprite_state) {
    switch (spring_sprite_state) {
        case 0x2E:  // SPRING_SPRITE_1
            return 23;  // Adjust based on actual sprite sheet tile index
        case 0x40:  // SPRING_SPRITE_2
            return 24;  // Adjust based on actual sprite sheet tile index
        default:
            return 23; // Default to SPRING_SPRITE_1
    }
}

// Map COLLAPSE_TILE_SPRITE_ OAM tile values to sprite sheet tile indices
// COLLAPSE_TILE_SPRITE_1 = 0x48 (72), SPRITE_2 = 0x4A (74), SPRITE_3 = 0x4C (76)
// Maps to sprite sheet tile indices: 30, 31, 32 (or similar, adjust based on actual data)
static uint8_t map_collapse_tile_sprite_to_tile_index(uint8_t collapse_tile_sprite_state) {
    switch (collapse_tile_sprite_state) {
        case 0x48:  // COLLAPSE_TILE_SPRITE_1
            return 30;  // Adjust based on actual sprite sheet tile index
        case 0x4A:  // COLLAPSE_TILE_SPRITE_2
            return 31;  // Adjust based on actual sprite sheet tile index
        case 0x4C:  // COLLAPSE_TILE_SPRITE_3
            return 32;  // Adjust based on actual sprite sheet tile index
        default:
            return 30; // Default to COLLAPSE_TILE_SPRITE_1
    }
}

// Look up sprite data from object_sprite array by tile index
static const unsigned char* find_sprite_data_by_tile_index(uint8_t tile_index) {
    // Get level data (room ID is 1-indexed, array is 0-indexed)
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) {
        level_idx = 0; // Default to level 1 if out of range
    }
    const LevelData *level = &level_data[level_idx];
    
    // Search through object_sprite array for the tile index
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    for (uint8_t i = 0; i < level->object_sprite_count; i++) {
        const unsigned char *sprite_entry = &level->object_sprites[i * 6];
        if (sprite_entry[0] == tile_index) {
            return sprite_entry;
        }
    }
    return NULL;  // Sprite data not found
}

void port_updatePlayerSprite(const struct sPlayerData *playerObj) {
    if (playerObj == NULL) {
        return;
    }
    
    const struct sOBJ_DATA *playerData = &playerObj->objData;
    uint8_t baseX = (uint8_t)playerData->pos.x;
    uint8_t baseY = (uint8_t)playerData->pos.y - s_scrollY; // Offset by scroll position
    
    // Get player frame index from oamTile (player sprite state)
    // Player sprite states: IDLE=0, WALK_1=2, WALK_2=4, WALK_3=6, WALL=8, DOWN=10, UP=12
    uint8_t frame_index = playerData->oamTile;
    
    // Look up player sprite data from object sprite array for this frame
    const unsigned char *player_sprite = find_player_sprite_data(frame_index);
    
    if (player_sprite != NULL) {
        // player_sprite format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
        uint8_t palette_idx = player_sprite[1];  // palette_index (should be 3 for player)
        uint8_t tl_tile = player_sprite[2];     // Top-left tile
        uint8_t tr_tile = player_sprite[3];     // Top-right tile
        uint8_t bl_tile = player_sprite[4];     // Bottom-left tile
        uint8_t br_tile = player_sprite[5];     // Bottom-right tile
        
        // Check if sprite should be flipped horizontally (for left/right facing)
        // isFliped is stored in oamProps bit 6 (0x40)
        bool flip_horizontal = (playerData->oamProps & 0x40) != 0;
        
        // Set palette in properties (bits 0-1 of byte 2 in OAM entry)
        // Player uses sprite palette 3 (bits 0-1 = 3)
        // NES OAM format: bits 0-1 = palette (0-3), bit 5 = priority, bit 6 = flip horizontal
        uint8_t baseProps = (palette_idx & 0x03);
        // Clear bit 5 (0x20) to make sprite appear in front of background
        baseProps &= ~0x20;
        // Set bit 6 (0x40) for horizontal flip if needed
        if (flip_horizontal) {
            baseProps |= 0x40;
        }
        
        if (flip_horizontal) {
            // Flipped horizontally: swap left/right tiles and use flip bit
            // Sprite 0: Top-right tile at left position (flipped)
            OAM_BUF[0] = baseY;
            OAM_BUF[1] = tr_tile;
            OAM_BUF[2] = baseProps;  // Already has flip bit set
            OAM_BUF[3] = baseX;
            
            // Sprite 1: Top-left tile at right position (flipped)
            OAM_BUF[4] = baseY;
            OAM_BUF[5] = tl_tile;
            OAM_BUF[6] = baseProps;  // Already has flip bit set
            OAM_BUF[7] = baseX + 8;
            
            // Sprite 2: Bottom-right tile at left position (flipped)
            OAM_BUF[8] = baseY + 8;
            OAM_BUF[9] = br_tile;
            OAM_BUF[10] = baseProps;  // Already has flip bit set
            OAM_BUF[11] = baseX;
            
            // Sprite 3: Bottom-left tile at right position (flipped)
            OAM_BUF[12] = baseY + 8;
            OAM_BUF[13] = bl_tile;
            OAM_BUF[14] = baseProps;  // Already has flip bit set
            OAM_BUF[15] = baseX + 8;
        } else {
            // Normal (not flipped)
            // Sprite 0: Top-left
            OAM_BUF[0] = baseY;
            OAM_BUF[1] = tl_tile;
            OAM_BUF[2] = baseProps;
            OAM_BUF[3] = baseX;
            
            // Sprite 1: Top-right
            OAM_BUF[4] = baseY;
            OAM_BUF[5] = tr_tile;
            OAM_BUF[6] = baseProps;
            OAM_BUF[7] = baseX + 8;
            
            // Sprite 2: Bottom-left
            OAM_BUF[8] = baseY + 8;
            OAM_BUF[9] = bl_tile;
            OAM_BUF[10] = baseProps;
            OAM_BUF[11] = baseX;
            
            // Sprite 3: Bottom-right
            OAM_BUF[12] = baseY + 8;
            OAM_BUF[13] = br_tile;
            OAM_BUF[14] = baseProps;
            OAM_BUF[15] = baseX + 8;
        }
    } else {
        // Fallback to old method if player sprite data not found
        uint8_t baseTile = playerData->oamTile;
        uint8_t baseProps = playerData->oamProps;
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
        OAM_BUF[9] = baseTile + 16;
        OAM_BUF[10] = baseProps;
        OAM_BUF[11] = baseX;
        
        // Sprite 3: Bottom-right
        OAM_BUF[12] = baseY + 8;
        OAM_BUF[13] = baseTile + 17;
        OAM_BUF[14] = baseProps;
        OAM_BUF[15] = baseX + 8;
    }
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

// Helper function to render a 16x16 sprite from sprite data
// sprite_data format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
// baseX, baseY: position of the sprite
// oamProps: properties (palette, priority, flip bits)
// oamOffset: offset into OAM_BUF (in bytes, 4 bytes per sprite)
static void render_16x16_sprite(const unsigned char *sprite_data, uint8_t baseX, uint8_t baseY, uint8_t oamProps, uint16_t oamOffset) {
    if (sprite_data == NULL) {
        return;
    }
    
    uint8_t palette_idx = sprite_data[1];  // palette_index
    uint8_t tl_tile = sprite_data[2];     // Top-left tile
    uint8_t tr_tile = sprite_data[3];     // Top-right tile
    uint8_t bl_tile = sprite_data[4];     // Bottom-left tile
    uint8_t br_tile = sprite_data[5];     // Bottom-right tile
    
    // Set palette in properties (bits 0-1 of byte 2 in OAM entry)
    // NES OAM format: bits 0-1 = palette (0-3), bit 5 = priority, bit 6 = flip horizontal
    uint8_t baseProps = (palette_idx & 0x03);
    // Clear bit 5 (0x20) to make sprite appear in front of background
    baseProps &= ~0x20;
    // Check if sprite should be flipped horizontally (bit 6 of oamProps)
    bool flip_horizontal = (oamProps & 0x40) != 0;
    // Set bit 6 (0x40) for horizontal flip if needed
    if (flip_horizontal) {
        baseProps |= 0x40;
    }
    
    if (flip_horizontal) {
        // Flipped horizontally: swap left/right tiles and use flip bit
        // Sprite 0: Top-right tile at left position (flipped)
        OAM_BUF[oamOffset + 0] = baseY;
        OAM_BUF[oamOffset + 1] = tr_tile;
        OAM_BUF[oamOffset + 2] = baseProps;  // Already has flip bit set
        OAM_BUF[oamOffset + 3] = baseX;
        
        // Sprite 1: Top-left tile at right position (flipped)
        OAM_BUF[oamOffset + 4] = baseY;
        OAM_BUF[oamOffset + 5] = tl_tile;
        OAM_BUF[oamOffset + 6] = baseProps;  // Already has flip bit set
        OAM_BUF[oamOffset + 7] = baseX + 8;
        
        // Sprite 2: Bottom-right tile at left position (flipped)
        OAM_BUF[oamOffset + 8] = baseY + 8;
        OAM_BUF[oamOffset + 9] = br_tile;
        OAM_BUF[oamOffset + 10] = baseProps;  // Already has flip bit set
        OAM_BUF[oamOffset + 11] = baseX;
        
        // Sprite 3: Bottom-left tile at right position (flipped)
        OAM_BUF[oamOffset + 12] = baseY + 8;
        OAM_BUF[oamOffset + 13] = bl_tile;
        OAM_BUF[oamOffset + 14] = baseProps;  // Already has flip bit set
        OAM_BUF[oamOffset + 15] = baseX + 8;
    } else {
        // Normal (not flipped)
        // Sprite 0: Top-left
        OAM_BUF[oamOffset + 0] = baseY;
        OAM_BUF[oamOffset + 1] = tl_tile;
        OAM_BUF[oamOffset + 2] = baseProps;
        OAM_BUF[oamOffset + 3] = baseX;
        
        // Sprite 1: Top-right
        OAM_BUF[oamOffset + 4] = baseY;
        OAM_BUF[oamOffset + 5] = tr_tile;
        OAM_BUF[oamOffset + 6] = baseProps;
        OAM_BUF[oamOffset + 7] = baseX + 8;
        
        // Sprite 2: Bottom-left
        OAM_BUF[oamOffset + 8] = baseY + 8;
        OAM_BUF[oamOffset + 9] = bl_tile;
        OAM_BUF[oamOffset + 10] = baseProps;
        OAM_BUF[oamOffset + 11] = baseX;
        
        // Sprite 3: Bottom-right
        OAM_BUF[oamOffset + 12] = baseY + 8;
        OAM_BUF[oamOffset + 13] = br_tile;
        OAM_BUF[oamOffset + 14] = baseProps;
        OAM_BUF[oamOffset + 15] = baseX + 8;
    }
}

void port_buildSpring(uint8_t index) {    
    OBJ_DATA *spring = &GLOBAL_OBJList[index];
    
    uint8_t baseX = (uint8_t)spring->pos.x;
    uint8_t baseY = (uint8_t)spring->pos.y - s_scrollY; // Offset by scroll position
    uint8_t frame_index = spring->oamTile;  // SPRING_SPRITE_1 or SPRING_SPRITE_2
    
    // Map OAM tile index to sprite sheet tile index
    uint8_t tile_index = map_spring_sprite_to_tile_index(frame_index);
    
    // Look up sprite data from object_sprite array
    const unsigned char *sprite_data = find_sprite_data_by_tile_index(tile_index);
    
    // Calculate OAM offset (4 sprites * 4 bytes = 16 bytes per 16x16 sprite)
    // Player uses slots 0-3 (16 bytes), so start at s_oamIndex
    uint16_t oamOffset = s_oamIndex * 4;  // Each sprite is 4 bytes
    
    if (sprite_data != NULL) {
        // Render the 16x16 sprite
        render_16x16_sprite(sprite_data, baseX, baseY, spring->oamProps, oamOffset);
    } else {
        // Fallback to old method if sprite data not found
        uint8_t baseTile = spring->oamTile;
        uint8_t baseProps = spring->oamProps;
        baseProps &= ~0x20;  // Clear priority bit to appear in front
        
        // Sprite 0: Top-left
        OAM_BUF[oamOffset + 0] = baseY;
        OAM_BUF[oamOffset + 1] = baseTile;
        OAM_BUF[oamOffset + 2] = baseProps;
        OAM_BUF[oamOffset + 3] = baseX;
        
        // Sprite 1: Top-right
        OAM_BUF[oamOffset + 4] = baseY;
        OAM_BUF[oamOffset + 5] = baseTile + 1;
        OAM_BUF[oamOffset + 6] = baseProps;
        OAM_BUF[oamOffset + 7] = baseX + 8;
        
        // Sprite 2: Bottom-left
        OAM_BUF[oamOffset + 8] = baseY + 8;
        OAM_BUF[oamOffset + 9] = baseTile + 16;
        OAM_BUF[oamOffset + 10] = baseProps;
        OAM_BUF[oamOffset + 11] = baseX;
        
        // Sprite 3: Bottom-right
        OAM_BUF[oamOffset + 12] = baseY + 8;
        OAM_BUF[oamOffset + 13] = baseTile + 17;
        OAM_BUF[oamOffset + 14] = baseProps;
        OAM_BUF[oamOffset + 15] = baseX + 8;
    }
    
    // Increment OAM index by 4 sprites (16x16 sprite = 4 8x8 sprites)
    s_oamIndex += 4;
}

void port_buildCollapseTile(uint8_t index) {    
    OBJ_DATA *collapseTile = &GLOBAL_OBJList[index];
    uint8_t baseX = (uint8_t)collapseTile->pos.x;
    uint8_t baseY = (uint8_t)collapseTile->pos.y - s_scrollY; // Offset by scroll position
    uint8_t frame_index = collapseTile->oamTile;  // COLLAPSE_TILE_SPRITE_1, _2, or _3
    
    // Map OAM tile index to sprite sheet tile index
    uint8_t tile_index = map_collapse_tile_sprite_to_tile_index(frame_index);
    
    // Look up sprite data from object_sprite array
    const unsigned char *sprite_data = find_sprite_data_by_tile_index(tile_index);
    
    // Calculate OAM offset (4 sprites * 4 bytes = 16 bytes per 16x16 sprite)
    // Player uses slots 0-3 (16 bytes), so start at s_oamIndex
    uint16_t oamOffset = s_oamIndex * 4;  // Each sprite is 4 bytes
    
    if (sprite_data != NULL) {
        // Render the 16x16 sprite
        render_16x16_sprite(sprite_data, baseX, baseY, collapseTile->oamProps, oamOffset);
    } else {
        // Fallback to old method if sprite data not found
        uint8_t baseTile = collapseTile->oamTile;
        uint8_t baseProps = collapseTile->oamProps;
        baseProps &= ~0x20;  // Clear priority bit to appear in front
        
        // Sprite 0: Top-left
        OAM_BUF[oamOffset + 0] = baseY;
        OAM_BUF[oamOffset + 1] = baseTile;
        OAM_BUF[oamOffset + 2] = baseProps;
        OAM_BUF[oamOffset + 3] = baseX;
        
        // Sprite 1: Top-right
        OAM_BUF[oamOffset + 4] = baseY;
        OAM_BUF[oamOffset + 5] = baseTile + 1;
        OAM_BUF[oamOffset + 6] = baseProps;
        OAM_BUF[oamOffset + 7] = baseX + 8;
        
        // Sprite 2: Bottom-left
        OAM_BUF[oamOffset + 8] = baseY + 8;
        OAM_BUF[oamOffset + 9] = baseTile + 16;
        OAM_BUF[oamOffset + 10] = baseProps;
        OAM_BUF[oamOffset + 11] = baseX;
        
        // Sprite 3: Bottom-right
        OAM_BUF[oamOffset + 12] = baseY + 8;
        OAM_BUF[oamOffset + 13] = baseTile + 17;
        OAM_BUF[oamOffset + 14] = baseProps;
        OAM_BUF[oamOffset + 15] = baseX + 8;
    }
    
    // Increment OAM index by 4 sprites (16x16 sprite = 4 8x8 sprites)
    s_oamIndex += 4;
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

// Forward declaration
static void update_player_hair_color(void);

__attribute__((noinline)) void port_vblank(void) {
    ppu_wait_nmi();
    oam_upload();

    // Update player hair color based on dash count
    update_player_hair_color();

    // Update scroll registers
    // PPU_SCROLL must be written twice: first X, then Y
    // Writing must happen during vblank, before rendering starts
    
    // Calculate scroll based on player position
    // Similar to SNES version: scroll starts at scrollPointY (72)
    // Scroll ranges from 0 to 16 (the two extremes)
    int16_t playerY = (int16_t)GLOBAL_PlayerData.objData.pos.y;
    int16_t scrollCalc = playerY - 16 - (int16_t)GLOBAL_ActiveLevel.scrollPointY;
    s_scrollY = (uint8_t)CLAMP(scrollCalc, 0, 16);
    
    PPU_SCROLL = 0; // X scroll (horizontal, set to 0 for now)
    PPU_SCROLL = s_scrollY; // Y scroll (vertical, based on player position)

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

// Palette data stored in RAM (for background and sprite palettes)
// Lower 16 bytes (0-15): 4 background palettes * 4 colors
// Upper 16 bytes (16-31): 4 sprite palettes * 4 colors
static uint8_t palette_ram[32];

// Load background palettes from tilemap data into RAM
static void load_background_palettes(void) {
    // NES has 4 background palettes, each with 4 colors
    // First color (index 0) is always the universal background color
    
    // Initialize with universal background color (0x0D = black, matching Python generation)
    // The first color of each palette (index 0) should be 0x0D for consistency
    for (uint8_t i = 0; i < 16; i++) {
        palette_ram[i] = 0x0D; // Universal background color (matches Python: 0x0D)
    }
    
    // Get level data (room ID is 1-indexed, array is 0-indexed)
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) {
        level_idx = 0; // Default to level 1 if out of range
    }
    const LevelData *level = &level_data[level_idx];
    
    // Load background palettes from tilemap data (up to 4 palettes)
    uint8_t pal_count = level->bg_palette_count;
    if (pal_count > 4) pal_count = 4;
    
    for (uint8_t pal_idx = 0; pal_idx < pal_count; pal_idx++) {
        // Each palette has 4 colors
        for (uint8_t col_idx = 0; col_idx < 4; col_idx++) {
            uint8_t nes_6bit = level->bg_palettes[pal_idx][col_idx];
            // Use the palette data directly - it's already in NES palette index format (0-63)
            palette_ram[pal_idx * 4 + col_idx] = nes_6bit_to_palette_index(nes_6bit);
        }
    }
    
    // Upload to PPU (lower 16 bytes for background)
    pal_bg(palette_ram);
}

// Load sprite palettes from tilemap data into RAM
static void load_sprite_palettes(void) {
    // NES has 4 sprite palettes, each with 4 colors
    // Note: Color 0 in sprite palettes is transparent, not black
    
    // Initialize with default colors (transparent for color 0)
    // Color 0 in sprite palettes is transparent, but we initialize to 0x0D for consistency
    for (uint8_t i = 0; i < 16; i++) {
        palette_ram[16 + i] = 0x0D; // Default to black (matches Python: 0x0D)
    }
    
    // Get level data (room ID is 1-indexed, array is 0-indexed)
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) {
        level_idx = 0; // Default to level 1 if out of range
    }
    const LevelData *level = &level_data[level_idx];
    
    // Load sprite palettes from tilemap data
    // The sprite_palettes array is always [4][4], so we always load all 4 palettes
    // sprite_palette_count tells us how many are actually used, but we need all 4
    // (especially palette 3 which is reserved for the player)
    for (uint8_t pal_idx = 0; pal_idx < 4; pal_idx++) {
        // Each palette has 4 colors
        for (uint8_t col_idx = 0; col_idx < 4; col_idx++) {
            uint8_t nes_6bit = level->sprite_palettes[pal_idx][col_idx];
            palette_ram[16 + pal_idx * 4 + col_idx] = nes_6bit_to_palette_index(nes_6bit);
        }
    }
    
    // Upload to PPU (upper 16 bytes for sprites)
    pal_spr(&palette_ram[16]);
}

// Update player hair color based on dash count
// 0 dashes = blue (0x12), 1 dash = default (0x37), 2 dashes = green (0x1A)
static void update_player_hair_color(void) {
    uint8_t dashes_left = GLOBAL_PlayerData.dashesLeft;
    uint8_t hair_color;
    
    // Sprite palette 3 is the player palette, color index 2 is the hair color
    // palette_ram[16 + 3 * 4 + 1] = palette_ram[16 + 14]
    uint8_t hair_color_index = 16 + 3 * 4 + 1; // Sprite palette 3, color 2 (offset by 16 for sprite palettes)
    
    if (dashes_left == 0) {
        hair_color = 0x12; // Blue (NES palette index)
    } else if (dashes_left == 1) {
        hair_color = 0x15; // Default hair color
    } else { // dashes_left >= 2
        hair_color = 0x1A; // Green (NES palette index)
    }
    
    // Only update if the color changed to avoid unnecessary PPU writes
    if (palette_ram[hair_color_index] != hair_color) {
        palette_ram[hair_color_index] = hair_color;
        // Upload updated palette to PPU (upper 16 bytes for sprites)
        pal_spr(&palette_ram[16]);
    }
}

// Write nametable from tilemap data
// With horizontal mirroring, we have two unique vertical nametables:
// - Nametable 0: $2000-$23FF (32x30 tiles)
// - Nametable 2: $2800-$2BFF (32x30 tiles)
// These are not contiguous in memory but can be smoothly scrolled between
static void write_nametable(void) {
    // NES nametable is 32x30 tiles (256x240 pixels)
    // Our tilemap is 16x16 tiles, each tile is 16x16 pixels (2x2 NES tiles)
    // So a 16x16 map = 32x32 NES tiles
    // We write rows 0-29 to nametable 0, rows 30-31 to nametable 2
    
    // Get level data (room ID is 1-indexed, array is 0-indexed)
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) {
        level_idx = 0; // Default to level 1 if out of range
    }
    const LevelData *level = &level_data[level_idx];
    
    uint8_t map_width = level->width;
    uint8_t map_height = level->height;
    const unsigned char *tilemap_gids = level->tilemap;  // Array of GIDs (one byte each)
    uint16_t tilemap_count = level->tilemap_count;
    
    // Use shared GID mapping (same for all levels)
    const unsigned char (*gid_to_tile_map)[6] = GID_TO_TILE_MAP;
    uint16_t gid_map_count = GID_TO_TILE_MAP_COUNT;
    
    // Calculate total NES tile rows (each map tile = 2 NES tile rows)
    uint8_t total_nes_rows = map_height * 2;
    
    // Write tiles to nametable 0 ($2000-$23FF) - rows 0-29
    vram_adr(NTADR_A(0, 0));
    uint8_t nes_rows_nt0 = (total_nes_rows < 30) ? total_nes_rows : 30;
    for (uint8_t nes_y = 0; nes_y < nes_rows_nt0; nes_y++) {
        uint8_t map_y = nes_y / 2;
        uint8_t tile_row = nes_y % 2; // 0 = top row, 1 = bottom row
        
        for (uint8_t map_x = 0; map_x < map_width; map_x++) {
            uint16_t tilemap_idx = map_y * map_width + map_x;
            
            // Get GID from tilemap
            uint8_t gid = 0;
            if (tilemap_idx < tilemap_count) {
                gid = tilemap_gids[tilemap_idx];
            }
            
            // Look up tile data from GID mapping
            const unsigned char *tile_entry = NULL;
            if (gid < gid_map_count && gid_to_tile_map != NULL) {
                tile_entry = gid_to_tile_map[gid];
            } else {
                if (gid_to_tile_map != NULL) {
                    tile_entry = gid_to_tile_map[0];
                }
            }
            
            if (tile_entry != NULL) {
                if (tile_row == 0) {
                    vram_put(tile_entry[0]); // TL
                    vram_put(tile_entry[1]); // TR
                } else {
                    vram_put(tile_entry[2]); // BL
                    vram_put(tile_entry[3]); // BR
                }
            } else {
                vram_put(0);
                vram_put(0);
            }
        }
    }
    
    // Write tiles to nametable 2 ($2800-$2BFF) - rows 30-31 (if any)
    if (total_nes_rows > 30) {
        vram_adr(0x2800); // Nametable 2 start address
        uint8_t nes_rows_nt2 = total_nes_rows - 30;
        for (uint8_t nes_y = 0; nes_y < nes_rows_nt2; nes_y++) {
            uint8_t map_y = (nes_y + 30) / 2;
            uint8_t tile_row = (nes_y + 30) % 2;
            
            for (uint8_t map_x = 0; map_x < map_width; map_x++) {
                uint16_t tilemap_idx = map_y * map_width + map_x;
                
                uint8_t gid = 0;
                if (tilemap_idx < tilemap_count) {
                    gid = tilemap_gids[tilemap_idx];
                }
                
                const unsigned char *tile_entry = NULL;
                if (gid < gid_map_count && gid_to_tile_map != NULL) {
                    tile_entry = gid_to_tile_map[gid];
                } else {
                    if (gid_to_tile_map != NULL) {
                        tile_entry = gid_to_tile_map[0];
                    }
                }
                
                if (tile_entry != NULL) {
                    if (tile_row == 0) {
                        vram_put(tile_entry[0]); // TL
                        vram_put(tile_entry[1]); // TR
                    } else {
                        vram_put(tile_entry[2]); // BL
                        vram_put(tile_entry[3]); // BR
                    }
                } else {
                    vram_put(0);
                    vram_put(0);
                }
            }
        }
    }
    
    // Write attributes to nametable 0 ($23C0)
    vram_adr(0x23C0);
    // Write all attribute rows (each covers 2 map tile rows = 4 NES tile rows)
    uint8_t max_attr_rows_nt0 = map_height / 2;
    for (uint8_t attr_y = 0; attr_y < max_attr_rows_nt0; attr_y++) {
        for (uint8_t attr_x = 0; attr_x < map_width / 2; attr_x++) {
            uint16_t tl_idx = (attr_y * 2) * map_width + (attr_x * 2);
            uint8_t tl_palette = 0;
            if (tl_idx < tilemap_count && gid_to_tile_map != NULL) {
                uint8_t gid = tilemap_gids[tl_idx];
                if (gid < gid_map_count) {
                    const unsigned char *tile_entry = gid_to_tile_map[gid];
                    tl_palette = tile_entry[4] & 0x03;
                }
            }
            
            uint16_t tr_idx = (attr_y * 2) * map_width + (attr_x * 2) + 1;
            uint8_t tr_palette = 0;
            if (tr_idx < tilemap_count && gid_to_tile_map != NULL) {
                uint8_t gid = tilemap_gids[tr_idx];
                if (gid < gid_map_count) {
                    const unsigned char *tile_entry = gid_to_tile_map[gid];
                    tr_palette = tile_entry[4] & 0x03;
                }
            }
            
            uint16_t bl_idx = ((attr_y * 2) + 1) * map_width + (attr_x * 2);
            uint8_t bl_palette = 0;
            if (bl_idx < tilemap_count && gid_to_tile_map != NULL) {
                uint8_t gid = tilemap_gids[bl_idx];
                if (gid < gid_map_count) {
                    const unsigned char *tile_entry = gid_to_tile_map[gid];
                    bl_palette = tile_entry[4] & 0x03;
                }
            }
            
            uint16_t br_idx = ((attr_y * 2) + 1) * map_width + (attr_x * 2) + 1;
            uint8_t br_palette = 0;
            if (br_idx < tilemap_count && gid_to_tile_map != NULL) {
                uint8_t gid = tilemap_gids[br_idx];
                if (gid < gid_map_count) {
                    const unsigned char *tile_entry = gid_to_tile_map[gid];
                    br_palette = tile_entry[4] & 0x03;
                }
            }
            
            uint8_t attr_byte = tl_palette | (tr_palette << 2) | (bl_palette << 4) | (br_palette << 6);
            vram_put(attr_byte);
        }
    }
    
    // Write attributes to nametable 2 ($2BC0) if needed
    // For rows 30-31 in nametable 2, we need attribute row 0 (covers NES tile rows 0-3 in NT2's coordinate space)
    // But in map coordinates, these are map tile row 15, which is attribute row 7
    if (total_nes_rows > 30) {
        vram_adr(0x2BC0); // Nametable 2 attribute table start
        // Map tile row 15 corresponds to NES tile rows 30-31
        // This is attribute row 7 in the full map (15 / 2 = 7, but we need the last attribute row)
        uint8_t map_attr_row = (map_height / 2) - 1; // Last attribute row (row 7 for 16-tile map)
        for (uint8_t attr_x = 0; attr_x < map_width / 2; attr_x++) {
            uint16_t tl_idx = (map_attr_row * 2) * map_width + (attr_x * 2);
            uint8_t tl_palette = 0;
            if (tl_idx < tilemap_count && gid_to_tile_map != NULL) {
                uint8_t gid = tilemap_gids[tl_idx];
                if (gid < gid_map_count) {
                    const unsigned char *tile_entry = gid_to_tile_map[gid];
                    tl_palette = tile_entry[4] & 0x03;
                }
            }
            
            uint16_t tr_idx = (map_attr_row * 2) * map_width + (attr_x * 2) + 1;
            uint8_t tr_palette = 0;
            if (tr_idx < tilemap_count && gid_to_tile_map != NULL) {
                uint8_t gid = tilemap_gids[tr_idx];
                if (gid < gid_map_count) {
                    const unsigned char *tile_entry = gid_to_tile_map[gid];
                    tr_palette = tile_entry[4] & 0x03;
                }
            }
            
            uint16_t bl_idx = ((map_attr_row * 2) + 1) * map_width + (attr_x * 2);
            uint8_t bl_palette = 0;
            if (bl_idx < tilemap_count && gid_to_tile_map != NULL) {
                uint8_t gid = tilemap_gids[bl_idx];
                if (gid < gid_map_count) {
                    const unsigned char *tile_entry = gid_to_tile_map[gid];
                    bl_palette = tile_entry[4] & 0x03;
                }
            }
            
            uint16_t br_idx = ((map_attr_row * 2) + 1) * map_width + (attr_x * 2) + 1;
            uint8_t br_palette = 0;
            if (br_idx < tilemap_count && gid_to_tile_map != NULL) {
                uint8_t gid = tilemap_gids[br_idx];
                if (gid < gid_map_count) {
                    const unsigned char *tile_entry = gid_to_tile_map[gid];
                    br_palette = tile_entry[4] & 0x03;
                }
            }
            
            uint8_t attr_byte = tl_palette | (tr_palette << 2) | (bl_palette << 4) | (br_palette << 6);
            vram_put(attr_byte);
        }
    }
}

void port_LoadRoomData(uint16_t roomID) {
    // Get level data (room ID is 1-indexed, array is 0-indexed)
    uint16_t level_idx = roomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) {
        level_idx = 0; // Default to level 1 if out of range
    }
    const LevelData *level = &level_data[level_idx];
    
    // Turn off PPU during loading
    ppu_off();
    
    // Set room ID and size
    GLOBAL_ActiveLevel.currentRoomID = roomID;
    GLOBAL_ActiveLevel.roomSizeX = level->width;
    GLOBAL_ActiveLevel.roomSizeY = level->height;
    
    // Generate collision data from tilemap GIDs
    // Use shared GID to collision mapping (defined in gid_to_tile_shared.h)
    for (uint16_t i = 0; i < level->tilemap_count && i < 256; i++) {
        uint8_t gid = level->tilemap[i];
        uint8_t collision_flag = 0;
        if (gid < GID_TO_COLLISION_COUNT) {
            collision_flag = gid_to_collision[gid];
        }
        GLOBAL_ActiveLevel.collisionFlagsReset[i] = collision_flag;
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = collision_flag;
    }
    
    // Set player spawn position
    GLOBAL_ActiveLevel.playerSpawnX = level->spawn_x;
    GLOBAL_ActiveLevel.playerSpawnY = level->spawn_y;
    
    // Set object count and copy object data
    GLOBAL_ActiveLevel.objectCount = level->object_count;
    memcpy(GLOBAL_ActiveLevel.objectData, level->objects, level->object_count * 3);
    
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

