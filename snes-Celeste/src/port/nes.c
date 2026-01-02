// Define __NES__ before including headers to get NES-specific enum values
#define __NES__

#include "port.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

// sprite_animation_enums_nes.h is included by port.h when __NES__ is defined
#include "../../python/gid_to_tile_shared.h"
#include "../../python/gid_to_tile_collapse.h"
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
// #include "../../python/tilemap_level28_nes.h"
// #include "../../python/tilemap_level29_nes.h"
// #include "../../python/tilemap_level30_nes.h"
// #include "../../python/tilemap_level31_nes.h"

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

static volatile uint8_t s_inputState = 0;
static uint8_t s_oamIndex = 0;
static uint8_t s_scrollY = 0; // Vertical scroll position

// List of collapse tile object indices (built once during level load)
static uint8_t s_collapseTileIndices[30]; // Max 30 objects
static uint8_t s_collapseTileCount = 0;

// Pre-calculated VRAM write operations for collapse tiles (to move calculations before vblank)
#define MAX_COLLAPSE_TILE_WRITES 30
typedef struct {
    uint16_t addr_top;
    uint16_t addr_bottom;
    const unsigned char *tile_data;  // Pointer to tile entry (TL, TR, BL, BR, palette, flip)
} CollapseTileWrite;

extern struct sActiveLevelData GLOBAL_ActiveLevel;
extern struct sPlayerData GLOBAL_PlayerData;
extern OBJ_DATA GLOBAL_OBJList[];

// Fixed level dimensions (all levels are 16x16 = 256 tiles)
#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 16
#define LEVEL_TILE_COUNT 256
#define LEVEL_BG_PALETTE_COUNT 1

typedef struct {
    const unsigned char *tilemap_compressed;
    const unsigned char *objects;
    uint16_t object_count;
    const unsigned char (*bg_palettes)[4];
    const unsigned char (*sprite_palettes)[4];
    uint8_t spawn_x;
    uint8_t spawn_y;
} LevelData;

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
    }
    /*
    // Level 27
    {
        tilemap_level27_compressed,
        object_level27, OBJECT_LEVEL27_COUNT,
        palette_background_level27,
        palette_sprite_level27,
        SPAWN_X_LEVEL27, SPAWN_Y_LEVEL27
    }
    // Level 28
    {
        tilemap_level28_compressed,
        object_level28, OBJECT_LEVEL28_COUNT,
        palette_background_level28,
        palette_sprite_level28,
        SPAWN_X_LEVEL28, SPAWN_Y_LEVEL28
    },
    Level 29
    {
        tilemap_level29_compressed,
        object_level29, OBJECT_LEVEL29_COUNT,
        palette_background_level29,
        palette_sprite_level29,
        SPAWN_X_LEVEL29, SPAWN_Y_LEVEL29
    },
    Level 30
    {
        tilemap_level30_compressed,
        object_level30, OBJECT_LEVEL30_COUNT,
        palette_background_level30,
        palette_sprite_level30,
        SPAWN_X_LEVEL30, SPAWN_Y_LEVEL30
    },
    Level 31
    {
        tilemap_level31_compressed,
        object_level31, OBJECT_LEVEL31_COUNT,
        palette_background_level31,
        palette_sprite_level31,
        SPAWN_X_LEVEL31, SPAWN_Y_LEVEL31
    } */
};

#define LEVEL_DATA_COUNT (sizeof(level_data) / sizeof(level_data[0]))

// Shared GID mapping (used by all levels)
#define GID_TO_TILE_MAP gid_to_tile_shared
#define GID_TO_TILE_MAP_COUNT GID_TO_TILE_SHARED_COUNT
#define GID_TO_COLLISION_COUNT 72

static void decompress_tilemap(const unsigned char *compressed_data, uint8_t *output) {
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
                output[out_idx++] = compression_dict_shared[dict_idx][0];
                if (out_idx < LEVEL_TILE_COUNT) output[out_idx++] = compression_dict_shared[dict_idx][1];
            }
            comp_idx++;
        }
    }
    while (out_idx < LEVEL_TILE_COUNT) output[out_idx++] = 0;
}

static void oam_upload(void) {
    OAM_ADDR = 0x00;
    OAM_DMA = (uint8_t)(((uintptr_t)OAM_BUF) >> 8);
}

static const unsigned char* get_object_sprite_data(uint8_t tile_index);
static void hide_sprites(uint16_t oamOffset);
static void render_16x16_sprite(const unsigned char *sprite_data, uint8_t baseX, uint8_t baseY, uint8_t oamProps, uint16_t oamOffset);
static void render_object_sprite(OBJ_DATA *obj, uint16_t oamOffset);

static const unsigned char* get_object_sprite_data(uint8_t tile_index) {
    if (tile_index >= OBJECT_SPRITE_DICT_LOOKUP_TABLE_SIZE) return NULL;
    uint8_t compact_idx = object_sprite_lookup_table[tile_index];
    if (compact_idx == 0xFF || compact_idx >= OBJECT_SPRITE_DICT_COMPACT_COUNT) return NULL;
    return &object_sprite_dict_compact[compact_idx][0];
}

static void hide_sprites(uint16_t oamOffset) {
    OAM_BUF[oamOffset + 0] = 240;
    OAM_BUF[oamOffset + 4] = 240;
    OAM_BUF[oamOffset + 8] = 240;
    OAM_BUF[oamOffset + 12] = 240;
}

static void render_object_sprite(OBJ_DATA *obj, uint16_t oamOffset) {
    if (obj->eType == OBJ_UNUSED) {
        hide_sprites(oamOffset);
        return;
    }
    uint8_t baseX = (uint8_t)obj->pos.x;
    uint8_t spriteY = (uint8_t)obj->pos.y;
    uint8_t baseY = (spriteY < 240) ? (spriteY - s_scrollY) : spriteY;
    uint8_t tile_index = obj->oamTile;
    const unsigned char *sprite_data = get_object_sprite_data(tile_index);
    if (sprite_data == NULL) {
        hide_sprites(oamOffset);
        return;
    }
    render_16x16_sprite(sprite_data, baseX, baseY, obj->oamProps, oamOffset);
}



void port_init(void) {
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    

    ppu_off();
    oam_clear();
    oam_size(0);
    
    // Enable both background and sprites
    ppu_on_all();
    
    PPU_CTRL = 0x88; // NMI enabled, sprite pattern $1000, nametable $2000
    ppu_wait_nmi();

}

void port_beginSpriteBuild(const struct sPlayerData *playerObj) {
    (void)playerObj;
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
    uint8_t spriteY = (uint8_t)playerData->pos.y;
    uint8_t baseY = (spriteY < 240) ? (spriteY - s_scrollY) : spriteY;
    uint8_t tile_index = playerData->oamTile;
    const unsigned char *player_sprite = get_object_sprite_data(tile_index);
    if (player_sprite == NULL) {
        hide_sprites(0);
        return;
    }
    
    render_16x16_sprite(player_sprite, baseX, baseY, playerData->oamProps, 0);
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

static void render_16x16_sprite(const unsigned char *sprite_data, uint8_t baseX, uint8_t baseY, uint8_t oamProps, uint16_t oamOffset) {
    if (sprite_data == NULL) return;
    uint8_t palette_idx = sprite_data[0];
    uint8_t tl_tile = sprite_data[1], tr_tile = sprite_data[2];
    uint8_t bl_tile = sprite_data[3], br_tile = sprite_data[4];
    uint8_t baseProps = (palette_idx & 0x03) & ~0x20;
    bool flip_horizontal = (oamProps & 0x40) != 0;
    if (flip_horizontal) baseProps |= 0x40;
    
    if (flip_horizontal) {
        OAM_BUF[oamOffset + 0] = baseY; OAM_BUF[oamOffset + 1] = tr_tile; OAM_BUF[oamOffset + 2] = baseProps; OAM_BUF[oamOffset + 3] = baseX;
        OAM_BUF[oamOffset + 4] = baseY; OAM_BUF[oamOffset + 5] = tl_tile; OAM_BUF[oamOffset + 6] = baseProps; OAM_BUF[oamOffset + 7] = baseX + 8;
        OAM_BUF[oamOffset + 8] = baseY + 8; OAM_BUF[oamOffset + 9] = br_tile; OAM_BUF[oamOffset + 10] = baseProps; OAM_BUF[oamOffset + 11] = baseX;
        OAM_BUF[oamOffset + 12] = baseY + 8; OAM_BUF[oamOffset + 13] = bl_tile; OAM_BUF[oamOffset + 14] = baseProps; OAM_BUF[oamOffset + 15] = baseX + 8;
    } else {
        OAM_BUF[oamOffset + 0] = baseY; OAM_BUF[oamOffset + 1] = tl_tile; OAM_BUF[oamOffset + 2] = baseProps; OAM_BUF[oamOffset + 3] = baseX;
        OAM_BUF[oamOffset + 4] = baseY; OAM_BUF[oamOffset + 5] = tr_tile; OAM_BUF[oamOffset + 6] = baseProps; OAM_BUF[oamOffset + 7] = baseX + 8;
        OAM_BUF[oamOffset + 8] = baseY + 8; OAM_BUF[oamOffset + 9] = bl_tile; OAM_BUF[oamOffset + 10] = baseProps; OAM_BUF[oamOffset + 11] = baseX;
        OAM_BUF[oamOffset + 12] = baseY + 8; OAM_BUF[oamOffset + 13] = br_tile; OAM_BUF[oamOffset + 14] = baseProps; OAM_BUF[oamOffset + 15] = baseX + 8;
    }
}

void port_buildSpring(uint8_t index) {
    OBJ_DATA *spring = &GLOBAL_OBJList[index];
    uint16_t oamOffset = ((uint16_t)index * 4 + 4) * 4;
    if (spring->eType == OBJ_UNUSED || spring->data.spring.isDisabled) {
        hide_sprites(oamOffset);
        return;
    }
    render_object_sprite(spring, oamOffset);
}

static void build_collapse_tile_list(void) {
    if (s_collapseTileCount > 0) return;
    s_collapseTileCount = 0;
    for (uint8_t i = 0; i < 30; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_COLLAPSE_TILE) {
            s_collapseTileIndices[s_collapseTileCount++] = i;
        }
    }
}

static void prepare_collapse_tiles_nametable(CollapseTileWrite *writes, uint8_t *write_count) {
    *write_count = 0;
    build_collapse_tile_list();
    for (uint8_t list_idx = 0; list_idx < s_collapseTileCount; list_idx++) {
        uint8_t i = s_collapseTileIndices[list_idx];
        OBJ_DATA *collapseTile = &GLOBAL_OBJList[i];
        if (!(collapseTile->flags & OBJ_FLAG_DIRTY)) continue;
        uint8_t tileX = collapseTile->pos.x / 16;
        uint8_t tileY = (collapseTile->pos.y + 1) / 16;
        const unsigned char *tile_entry = NULL;
        if (collapseTile->data.collapseTile.state != 2 && collapseTile->oamTile >= COLLAPSE_TILE_SPRITE_1 && collapseTile->oamTile <= COLLAPSE_TILE_SPRITE_3) {
            tile_entry = gid_to_tile_collapse[collapseTile->oamTile - COLLAPSE_TILE_SPRITE_1];
        }
        uint8_t nes_tile_x = tileX * 2;
        uint8_t nes_tile_y_top = tileY * 2;
        uint8_t nes_tile_y_bottom = nes_tile_y_top + 1;
        uint16_t nametable_base_top = (nes_tile_y_top < 30) ? 0x2000 : 0x2800;
        uint16_t nametable_base_bottom = (nes_tile_y_bottom < 30) ? 0x2000 : 0x2800;
        uint8_t nes_tile_y_adj_top = (nes_tile_y_top < 30) ? nes_tile_y_top : (nes_tile_y_top - 30);
        uint8_t nes_tile_y_adj_bottom = (nes_tile_y_bottom < 30) ? nes_tile_y_bottom : (nes_tile_y_bottom - 30);
        uint16_t addr_top = nametable_base_top + ((uint16_t)nes_tile_y_adj_top * 32) + nes_tile_x;
        uint16_t addr_bottom = nametable_base_bottom + ((uint16_t)nes_tile_y_adj_bottom * 32) + nes_tile_x;
        if (*write_count < MAX_COLLAPSE_TILE_WRITES) {
            CollapseTileWrite *write = &writes[(*write_count)++];
            write->addr_top = addr_top;
            write->addr_bottom = addr_bottom;
            write->tile_data = tile_entry;
            collapseTile->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
        }
    }
}

static void execute_collapse_tiles_nametable_writes(const CollapseTileWrite *writes, uint8_t write_count) {
    for (uint8_t i = 0; i < write_count; i++) {
        const CollapseTileWrite *write = &writes[i];
        if (write->tile_data != NULL) {
            vram_adr(write->addr_top);
            vram_put(write->tile_data[0]);
            vram_put(write->tile_data[1]);
            vram_adr(write->addr_bottom);
            vram_put(write->tile_data[2]);
            vram_put(write->tile_data[3]);
        } else {
            vram_adr(write->addr_top);
            vram_put(0);
            vram_put(0);
            vram_adr(write->addr_bottom);
            vram_put(0);
            vram_put(0);
        }
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

void port_buildStrawberry(uint8_t index) {
    OBJ_DATA *strawberry = &GLOBAL_OBJList[index];
    uint16_t oamOffset = ((uint16_t)index * 4 + 4) * 4;
    if (strawberry->eType == OBJ_UNUSED || 
        (strawberry->data.strawberry.isCollected && strawberry->data.strawberry.frameCount > 60)) {
        hide_sprites(oamOffset);
        return;
    }
    render_object_sprite(strawberry, oamOffset);
}

void port_buildPlatMov(uint8_t index) {
}

void port_buildFlyingBerry(uint8_t index) {
    OBJ_DATA *berry = &GLOBAL_OBJList[index];
    uint16_t oamOffset = ((uint16_t)index * 4 + 4) * 4;
    if (berry->eType == OBJ_UNUSED || 
        (berry->data.strawberry.isCollected && berry->data.strawberry.frameCount > 60)) {
        hide_sprites(oamOffset);
        return;
    }
    render_object_sprite(berry, oamOffset);
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

static char (*volatile pad_poll_fn)(char) = pad_poll;
static void update_player_hair_color(void);
static void prepare_collapse_tiles_nametable(CollapseTileWrite *writes, uint8_t *write_count);
static void execute_collapse_tiles_nametable_writes(const CollapseTileWrite *writes, uint8_t write_count);

__attribute__((noinline)) void port_vblank(void) {
    int16_t playerY = (int16_t)GLOBAL_PlayerData.objData.pos.y;
    int16_t scrollCalc = playerY - 16 - (int16_t)GLOBAL_ActiveLevel.scrollPointY;
    s_scrollY = (uint8_t)CLAMP(scrollCalc, 0, 16);
    CollapseTileWrite collapseTileWrites[MAX_COLLAPSE_TILE_WRITES];
    uint8_t collapseTileWriteCount = 0;
    prepare_collapse_tiles_nametable(collapseTileWrites, &collapseTileWriteCount);
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
    oam_upload();
    execute_collapse_tiles_nametable_writes(collapseTileWrites, collapseTileWriteCount);
    update_player_hair_color();
    PPU_SCROLL = 0;
    PPU_SCROLL = s_scrollY;
}

uint8_t port_getInputs(void) {
    return s_inputState;
}

static uint8_t nes_6bit_to_palette_index(uint8_t nes_6bit) {
    return nes_6bit;
}

// Palette data stored in RAM (for background and sprite palettes)
// Lower 16 bytes (0-15): 4 background palettes * 4 colors
// Upper 16 bytes (16-31): 4 sprite palettes * 4 colors
static uint8_t palette_ram[32];

static void load_background_palettes(void) {
    for (uint8_t i = 0; i < 16; i++) palette_ram[i] = 0x0D;
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) level_idx = 0;
    const LevelData *level = &level_data[level_idx];
    for (uint8_t pal_idx = 0; pal_idx < 4; pal_idx++) {
        for (uint8_t col_idx = 0; col_idx < 4; col_idx++) {
            palette_ram[pal_idx * 4 + col_idx] = nes_6bit_to_palette_index(level->bg_palettes[pal_idx][col_idx]);
        }
    }
    pal_bg(palette_ram);
}

static void load_sprite_palettes(void) {
    for (uint8_t i = 0; i < 16; i++) palette_ram[16 + i] = 0x0D;
    uint16_t level_idx = GLOBAL_ActiveLevel.currentRoomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) level_idx = 0;
    const LevelData *level = &level_data[level_idx];
    for (uint8_t pal_idx = 0; pal_idx < 4; pal_idx++) {
        for (uint8_t col_idx = 0; col_idx < 4; col_idx++) {
            palette_ram[16 + pal_idx * 4 + col_idx] = nes_6bit_to_palette_index(level->sprite_palettes[pal_idx][col_idx]);
        }
    }
    pal_spr(&palette_ram[16]);
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
    const LevelData *level = &level_data[level_idx];
    uint8_t *decompressed_tilemap = (uint8_t *)GLOBAL_OBJList;
    decompress_tilemap(level->tilemap_compressed, decompressed_tilemap);
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
    if (total_nes_rows > 30) {
        vram_adr(0x2BC0);
        uint8_t map_attr_row = (LEVEL_HEIGHT / 2) - 1;
        for (uint8_t attr_x = 0; attr_x < LEVEL_WIDTH / 2; attr_x++) {
            uint16_t base_idx = (map_attr_row * 2) * LEVEL_WIDTH + (attr_x * 2);
            uint8_t tl_palette = get_palette_from_gid((base_idx < LEVEL_TILE_COUNT) ? tilemap_gids[base_idx] : 0, gid_to_tile_map, gid_map_count);
            uint8_t tr_palette = get_palette_from_gid((base_idx + 1 < LEVEL_TILE_COUNT) ? tilemap_gids[base_idx + 1] : 0, gid_to_tile_map, gid_map_count);
            uint8_t bl_palette = get_palette_from_gid((base_idx + LEVEL_WIDTH < LEVEL_TILE_COUNT) ? tilemap_gids[base_idx + LEVEL_WIDTH] : 0, gid_to_tile_map, gid_map_count);
            uint8_t br_palette = get_palette_from_gid((base_idx + LEVEL_WIDTH + 1 < LEVEL_TILE_COUNT) ? tilemap_gids[base_idx + LEVEL_WIDTH + 1] : 0, gid_to_tile_map, gid_map_count);
            vram_put(tl_palette | (tr_palette << 2) | (bl_palette << 4) | (br_palette << 6));
        }
    }
}

static void fix_collapse_tile_palettes(const uint8_t *decompressed_tilemap) {
    for (uint8_t map_y = 0; map_y < LEVEL_HEIGHT; map_y++) {
        for (uint8_t map_x = 0; map_x < LEVEL_WIDTH; map_x++) {
            uint8_t gid = decompressed_tilemap[map_y * LEVEL_WIDTH + map_x];
            if (gid >= 24 && gid <= 26) {
                uint8_t attr_x = map_x / 2;
                uint8_t attr_y = map_y / 2;
                uint8_t quadrant = ((map_y & 1) << 1) | (map_x & 1);
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

void port_LoadRoomData(uint16_t roomID) {
    uint16_t level_idx = roomID - 1;
    if (level_idx >= LEVEL_DATA_COUNT) level_idx = 0;
    const LevelData *level = &level_data[level_idx];
    s_collapseTileCount = 0;
    ppu_off();
    GLOBAL_ActiveLevel.currentRoomID = roomID;
    GLOBAL_ActiveLevel.roomSizeX = LEVEL_WIDTH;
    GLOBAL_ActiveLevel.roomSizeY = LEVEL_HEIGHT;
    uint8_t *decompressed_tilemap = (uint8_t *)GLOBAL_OBJList;
    decompress_tilemap(level->tilemap_compressed, decompressed_tilemap);
    for (uint16_t i = 0; i < LEVEL_TILE_COUNT && i < 256; i++) {
        uint8_t gid = decompressed_tilemap[i];
        uint8_t collision_flag = (gid < GID_TO_COLLISION_COUNT) ? gid_to_collision[gid] : 0;
        GLOBAL_ActiveLevel.collisionFlagsReset[i] = collision_flag;
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = collision_flag;
    }
    GLOBAL_ActiveLevel.playerSpawnX = level->spawn_x;
    GLOBAL_ActiveLevel.playerSpawnY = level->spawn_y;
    GLOBAL_ActiveLevel.objectCount = level->object_count;
    memcpy(GLOBAL_ActiveLevel.objectData, level->objects, level->object_count * 3);
    GLOBAL_ActiveLevel.scrollPointY = 72;
    write_nametable();
    fix_collapse_tile_palettes(decompressed_tilemap);
    load_background_palettes();
    load_sprite_palettes();
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    ppu_on_all();
    ppu_wait_nmi();
}

