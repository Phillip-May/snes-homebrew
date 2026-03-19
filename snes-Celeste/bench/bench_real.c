// Real level benchmark: loads actual level data for all 31 levels
// mos-sim-clang -Os -flto -o bench/bench_real bench/bench_real.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// mos-sim bench/bench_real

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#define _WIN32
#define PORT_FUNC_BANK6
#define PORT_FUNC_BANK5
#define PORT_FUNC_BANK4
#define PORT_FUNC_BANK3
#define PORT_DATA_BANK6
#define PORT_DATA_BANK5
#define PORT_DATA_BANK4
#define PORT_DATA_BANK3
#define __NES__
#define __NES_UNROM_512__  // needed for section attributes in headers
// Stub section attributes for sim
#undef __NES_UNROM_512__

#include "fixedPointSNES.h"
#include "port/port.h"

// Stubs
void port_buildSpriteIfDirty(uint8_t i, enum eOBJType t);
void port_buildUnused(uint8_t i);
void port_updateCollapseTileNametable(uint8_t i) { (void)i; }
void port_beginSpriteBuild(const struct sPlayerData *p) { (void)p; }
void port_finishSpriteBuild(void) {}
void port_updatePlayerSprite(const struct sPlayerData *p) { (void)p; }
void port_resetSprites(void) {}
void port_drawText(const unsigned char *t, uint8_t x, uint8_t y) { (void)t;(void)x;(void)y; }
void port_init(void) {}
uint8_t port_getInputs(void) { return 0; }
void port_vblank(void) {}
void port_LoadRoomData(uint16_t r) { (void)r; }
void port_restoreCollisionFlags(void) {}
void port_buildSmoke(uint8_t i) { (void)i; }
void port_buildBreakableWall(uint8_t i) { (void)i; }
void port_buildBalloon(uint8_t i) { (void)i; }
void port_buildMonument(uint8_t i) { (void)i; }
void port_buildChest(uint8_t i) { (void)i; }
void port_buildBigChest(uint8_t i) { (void)i; }
void port_buildKey(uint8_t i) { (void)i; }
void port_buildSpring(uint8_t i) { (void)i; }
void port_buildCollapseTile(uint8_t i) { (void)i; }
void port_buildStrawberry(uint8_t i) { (void)i; }
void port_buildPlatMov(uint8_t i) { (void)i; }
void port_buildFlyingBerry(uint8_t i) { (void)i; }
void port_buildDoubleDashOrb(uint8_t i) { (void)i; }
void port_buildStaticDecor(uint8_t i) { (void)i; }

// Include game logic
#define main game_main_excluded
#define onVblank game_onVblank_excluded
#include "mainBankZero.c"
#undef main
#undef onVblank

void port_buildSpriteIfDirty(uint8_t i, enum eOBJType t) {
    GLOBAL_OBJList[i].flags &= (uint8_t)~OBJ_FLAG_DIRTY; (void)t;
}
void port_buildUnused(uint8_t i) { GLOBAL_OBJList[i].flags &= (uint8_t)~OBJ_FLAG_DIRTY; }

// Include level data headers
#include "../python/compression_dict_shared.h"
#include "../python/gid_to_tile_shared.h"
#include "../python/tilemap_level1_nes.h"
#include "../python/tilemap_level2_nes.h"
#include "../python/tilemap_level3_nes.h"
#include "../python/tilemap_level4_nes.h"
#include "../python/tilemap_level5_nes.h"
#include "../python/tilemap_level6_nes.h"
#include "../python/tilemap_level7_nes.h"
#include "../python/tilemap_level8_nes.h"
#include "../python/tilemap_level9_nes.h"
#include "../python/tilemap_level10_nes.h"
#include "../python/tilemap_level11_nes.h"
#include "../python/tilemap_level12_nes.h"
#include "../python/tilemap_level13_nes.h"
#include "../python/tilemap_level14_nes.h"
#include "../python/tilemap_level15_nes.h"
#include "../python/tilemap_level16_nes.h"
#include "../python/tilemap_level17_nes.h"
#include "../python/tilemap_level18_nes.h"
#include "../python/tilemap_level19_nes.h"
#include "../python/tilemap_level20_nes.h"
#include "../python/tilemap_level21_nes.h"
#include "../python/tilemap_level22_nes.h"
#include "../python/tilemap_level23_nes.h"
#include "../python/tilemap_level24_nes.h"
#include "../python/tilemap_level25_nes.h"
#include "../python/tilemap_level26_nes.h"
#include "../python/tilemap_level27_nes.h"
#include "../python/tilemap_level28_nes.h"
#include "../python/tilemap_level29_nes.h"
#include "../python/tilemap_level30_nes.h"
#include "../python/tilemap_level31_nes.h"

// Level data table (same as nes.c)
typedef struct {
    const unsigned char *tilemap_compressed;
    const unsigned char *objects;
    uint16_t object_count;
    const unsigned char (*bg_palettes)[4];
    const unsigned char (*sprite_palettes)[4];
    uint8_t spawn_x;
    uint8_t spawn_y;
} LevelData;

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 16
#define LEVEL_TILE_COUNT 256

static const LevelData level_data[] = {
    { tilemap_level1_compressed, object_level1, OBJECT_LEVEL1_COUNT, palette_background_level1, palette_sprite_level1, SPAWN_X_LEVEL1, SPAWN_Y_LEVEL1 },
    { tilemap_level2_compressed, object_level2, OBJECT_LEVEL2_COUNT, palette_background_level2, palette_sprite_level2, SPAWN_X_LEVEL2, SPAWN_Y_LEVEL2 },
    { tilemap_level3_compressed, object_level3, OBJECT_LEVEL3_COUNT, palette_background_level3, palette_sprite_level3, SPAWN_X_LEVEL3, SPAWN_Y_LEVEL3 },
    { tilemap_level4_compressed, object_level4, OBJECT_LEVEL4_COUNT, palette_background_level4, palette_sprite_level4, SPAWN_X_LEVEL4, SPAWN_Y_LEVEL4 },
    { tilemap_level5_compressed, object_level5, OBJECT_LEVEL5_COUNT, palette_background_level5, palette_sprite_level5, SPAWN_X_LEVEL5, SPAWN_Y_LEVEL5 },
    { tilemap_level6_compressed, object_level6, OBJECT_LEVEL6_COUNT, palette_background_level6, palette_sprite_level6, SPAWN_X_LEVEL6, SPAWN_Y_LEVEL6 },
    { tilemap_level7_compressed, object_level7, OBJECT_LEVEL7_COUNT, palette_background_level7, palette_sprite_level7, SPAWN_X_LEVEL7, SPAWN_Y_LEVEL7 },
    { tilemap_level8_compressed, object_level8, OBJECT_LEVEL8_COUNT, palette_background_level8, palette_sprite_level8, SPAWN_X_LEVEL8, SPAWN_Y_LEVEL8 },
    { tilemap_level9_compressed, object_level9, OBJECT_LEVEL9_COUNT, palette_background_level9, palette_sprite_level9, SPAWN_X_LEVEL9, SPAWN_Y_LEVEL9 },
    { tilemap_level10_compressed, object_level10, OBJECT_LEVEL10_COUNT, palette_background_level10, palette_sprite_level10, SPAWN_X_LEVEL10, SPAWN_Y_LEVEL10 },
    { tilemap_level11_compressed, object_level11, OBJECT_LEVEL11_COUNT, palette_background_level11, palette_sprite_level11, SPAWN_X_LEVEL11, SPAWN_Y_LEVEL11 },
    { tilemap_level12_compressed, object_level12, OBJECT_LEVEL12_COUNT, palette_background_level12, palette_sprite_level12, SPAWN_X_LEVEL12, SPAWN_Y_LEVEL12 },
    { tilemap_level13_compressed, object_level13, OBJECT_LEVEL13_COUNT, palette_background_level13, palette_sprite_level13, SPAWN_X_LEVEL13, SPAWN_Y_LEVEL13 },
    { tilemap_level14_compressed, object_level14, OBJECT_LEVEL14_COUNT, palette_background_level14, palette_sprite_level14, SPAWN_X_LEVEL14, SPAWN_Y_LEVEL14 },
    { tilemap_level15_compressed, object_level15, OBJECT_LEVEL15_COUNT, palette_background_level15, palette_sprite_level15, SPAWN_X_LEVEL15, SPAWN_Y_LEVEL15 },
    { tilemap_level16_compressed, object_level16, OBJECT_LEVEL16_COUNT, palette_background_level16, palette_sprite_level16, SPAWN_X_LEVEL16, SPAWN_Y_LEVEL16 },
    { tilemap_level17_compressed, object_level17, OBJECT_LEVEL17_COUNT, palette_background_level17, palette_sprite_level17, SPAWN_X_LEVEL17, SPAWN_Y_LEVEL17 },
    { tilemap_level18_compressed, object_level18, OBJECT_LEVEL18_COUNT, palette_background_level18, palette_sprite_level18, SPAWN_X_LEVEL18, SPAWN_Y_LEVEL18 },
    { tilemap_level19_compressed, object_level19, OBJECT_LEVEL19_COUNT, palette_background_level19, palette_sprite_level19, SPAWN_X_LEVEL19, SPAWN_Y_LEVEL19 },
    { tilemap_level20_compressed, object_level20, OBJECT_LEVEL20_COUNT, palette_background_level20, palette_sprite_level20, SPAWN_X_LEVEL20, SPAWN_Y_LEVEL20 },
    { tilemap_level21_compressed, object_level21, OBJECT_LEVEL21_COUNT, palette_background_level21, palette_sprite_level21, SPAWN_X_LEVEL21, SPAWN_Y_LEVEL21 },
    { tilemap_level22_compressed, object_level22, OBJECT_LEVEL22_COUNT, palette_background_level22, palette_sprite_level22, SPAWN_X_LEVEL22, SPAWN_Y_LEVEL22 },
    { tilemap_level23_compressed, object_level23, OBJECT_LEVEL23_COUNT, palette_background_level23, palette_sprite_level23, SPAWN_X_LEVEL23, SPAWN_Y_LEVEL23 },
    { tilemap_level24_compressed, object_level24, OBJECT_LEVEL24_COUNT, palette_background_level24, palette_sprite_level24, SPAWN_X_LEVEL24, SPAWN_Y_LEVEL24 },
    { tilemap_level25_compressed, object_level25, OBJECT_LEVEL25_COUNT, palette_background_level25, palette_sprite_level25, SPAWN_X_LEVEL25, SPAWN_Y_LEVEL25 },
    { tilemap_level26_compressed, object_level26, OBJECT_LEVEL26_COUNT, palette_background_level26, palette_sprite_level26, SPAWN_X_LEVEL26, SPAWN_Y_LEVEL26 },
    { tilemap_level27_compressed, object_level27, OBJECT_LEVEL27_COUNT, palette_background_level27, palette_sprite_level27, SPAWN_X_LEVEL27, SPAWN_Y_LEVEL27 },
    { tilemap_level28_compressed, object_level28, OBJECT_LEVEL28_COUNT, palette_background_level28, palette_sprite_level28, SPAWN_X_LEVEL28, SPAWN_Y_LEVEL28 },
    { tilemap_level29_compressed, object_level29, OBJECT_LEVEL29_COUNT, palette_background_level29, palette_sprite_level29, SPAWN_X_LEVEL29, SPAWN_Y_LEVEL29 },
    { tilemap_level30_compressed, object_level30, OBJECT_LEVEL30_COUNT, palette_background_level30, palette_sprite_level30, SPAWN_X_LEVEL30, SPAWN_Y_LEVEL30 },
    { tilemap_level31_compressed, object_level31, OBJECT_LEVEL31_COUNT, palette_background_level31, palette_sprite_level31, SPAWN_X_LEVEL31, SPAWN_Y_LEVEL31 },
};

// Sim-safe decompression (no bank switching)
static void decompress_sim(const unsigned char *compressed, uint8_t *output) {
    uint16_t comp_idx = 0, out_idx = 0;
    while (out_idx < LEVEL_TILE_COUNT) {
        uint8_t byte = compressed[comp_idx];
        if (byte < 0x80) {
            output[out_idx++] = byte;
            comp_idx++;
        } else if (byte < 0xC0) {
            uint8_t count = (byte & 0x3F) + 1;
            uint8_t gid = compressed[++comp_idx];
            comp_idx++;
            for (uint8_t i = 0; i < count && out_idx < LEVEL_TILE_COUNT; i++)
                output[out_idx++] = gid;
        } else {
            uint8_t dict_idx = byte & 0x3F;
            if (dict_idx < COMPRESSION_DICT_SHARED_COUNT) {
                output[out_idx++] = compression_dict_shared[dict_idx][0];
                if (out_idx < LEVEL_TILE_COUNT) output[out_idx++] = compression_dict_shared[dict_idx][1];
            }
            comp_idx++;
        }
    }
}

static void load_level(uint8_t lvl) {
    uint16_t i;
    const LevelData *ld = &level_data[lvl - 1];

    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = lvl;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    GLOBAL_ActiveLevel.playerSpawnX = ld->spawn_x;
    GLOBAL_ActiveLevel.playerSpawnY = ld->spawn_y;

    // Decompress tilemap into temp buffer, derive collision
    {
        uint8_t decompressed[256];
        decompress_sim(ld->tilemap_compressed, decompressed);
        for (i = 0; i < 256; i++) {
            uint8_t gid = decompressed[i];
            uint8_t cf = (gid < GID_TO_TILE_SHARED_COUNT) ? gid_to_collision[gid] : 0;
            GLOBAL_ActiveLevel.collisionFlagsArr[i] = cf;
            GLOBAL_ActiveLevel.collisionFlagsReset[i] = cf;
        }
    }

    // Load objects
    GLOBAL_ActiveLevel.objectCount = (uint8_t)ld->object_count;
    memcpy(GLOBAL_ActiveLevel.objectData, ld->objects, ld->object_count * 3);

    // Clear object array
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(OBJ_DATA));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = 0xFF;
    }
    GLOBAL_ActiveLevel.movingPlatformCount = 0;

    // Init player
    memset(&GLOBAL_PlayerData, 0, sizeof(GLOBAL_PlayerData));
    GLOBAL_PlayerData.objData.eType = OBJ_PLAYER;
    GLOBAL_PlayerData.objData.pos.x = ld->spawn_x * 16;
    GLOBAL_PlayerData.objData.pos.y = ld->spawn_y * 16 - 1;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(GLOBAL_PlayerData.objData.pos.x);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(GLOBAL_PlayerData.objData.pos.y);
    GLOBAL_PlayerData.dashesLeft = 1;
    GLOBAL_PlayerData.movingPlatformIndex = -1;
    GLOBAL_PlayerData.graceTimer = 6;
    GLOBAL_PlayerData.objData.flags = OBJ_FLAG_DIRTY;

    // Init actual objects from level data
    for (i = 0; i < GLOBAL_ActiveLevel.objectCount * 3; i += 3) {
        uint8_t type = GLOBAL_ActiveLevel.objectData[i];
        if (type != 0 && type != 1) { // skip empty and player spawn markers
            initObject(type,
                       GLOBAL_ActiveLevel.objectData[i+1] * 16,
                       GLOBAL_ActiveLevel.objectData[i+2] * 16 - 1);
        }
    }

    GLOBAL_FrameCount = 10;
    GLOBAL_FreezeFrames = 0;
    GLOBAL_PausePlayerFrames = 0;
}

static void clamp(void) {
    if (GLOBAL_PlayerData.objData.pos.y > 220 || GLOBAL_PlayerData.objData.pos.y < -10) {
        GLOBAL_PlayerData.objData.pos.y = GLOBAL_ActiveLevel.playerSpawnY * 16 - 1;
        GLOBAL_PlayerData.posF.y = INT_TO_FIXED(GLOBAL_PlayerData.objData.pos.y);
        GLOBAL_PlayerData.spd.y = 0;
        GLOBAL_PlayerData.graceTimer = 6;
        GLOBAL_PlayerData.dashesLeft = 1;
    }
    if (GLOBAL_PlayerData.objData.pos.x < 5 || GLOBAL_PlayerData.objData.pos.x > 240) {
        GLOBAL_PlayerData.objData.pos.x = GLOBAL_ActiveLevel.playerSpawnX * 16;
        GLOBAL_PlayerData.posF.x = INT_TO_FIXED(GLOBAL_PlayerData.objData.pos.x);
    }
}

static void print_padded(unsigned long n, uint8_t w) {
    char b[8]; uint8_t len=0;
    unsigned long v=n;
    if(!v){b[len++]='0';} else {while(v){b[len++]='0'+(char)(v%10);v/=10;}}
    while(len<w){putchar(' ');w--;}
    while(len--)putchar(b[len]);
}

#define BUDGET 25780
#define NF 5

int main(void) {
    uint8_t lvl, f, j;
    unsigned long worst_all = 0;

    printf("Lvl Obj run    jump   dash   air    worst  %%\n");

    for (lvl = 1; lvl <= 31; lvl++) {
        unsigned long results[4] = {0,0,0,0};
        uint8_t inputs[4] = {0x01, 0x81, 0x41, 0x00}; // run, jump, dash, fall
        uint8_t nActive = 0;
        uint8_t ti;

        for (ti = 0; ti < 4; ti++) {
            load_level(lvl);
            // Count active objects
            if (ti == 0) {
                for (j = 1; j < GLOBAL_OBJ_LIST_SIZE; j++)
                    if (GLOBAL_OBJList[j].eType != OBJ_UNUSED) nActive++;
            }
            unsigned long mx = 0;
            for (f = 0; f < NF; f++) {
                unsigned long c;
                clamp();
                GLOBAL_InputState = inputs[ti];
                GLOBAL_FrameCount++;
                reset_clock();
                for (j = 0; j < GLOBAL_OBJ_LIST_SIZE; j++) processObject(j);
                playerUpdate(&GLOBAL_PlayerData);
                c = clock();
                if (c > mx) mx = c;
            }
            results[ti] = mx;
        }

        unsigned long worst = 0;
        for (ti = 0; ti < 4; ti++) if (results[ti] > worst) worst = results[ti];
        if (worst > worst_all) worst_all = worst;

        print_padded(lvl, 2);
        putchar(' ');
        print_padded(nActive, 2);
        putchar(' ');
        for (ti = 0; ti < 4; ti++) {
            print_padded(results[ti], 6);
            putchar(' ');
        }
        print_padded(worst, 6);
        putchar(' ');
        print_padded((worst * 100) / BUDGET, 3);
        if (worst > BUDGET) printf(" OVER!");
        putchar('\n');
    }

    printf("\nWorst: "); print_padded(worst_all, 1);
    printf("/%d = %lu%%\n", BUDGET, (worst_all * 100) / BUDGET);
    if (worst_all <= BUDGET) printf("PASS\n"); else printf("FAIL\n");
    return 0;
}
