// Benchmark: measure per-frame cycle cost for all 31 levels
// Uses actual game logic compiled for mos-sim
// Compile: mos-sim-clang -Os -flto -o bench/bench_levels bench/bench_levels.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run: mos-sim bench/bench_levels

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Prevent NES hardware includes
#define _WIN32

// Stub port macros
#define PORT_FUNC_BANK6
#define PORT_FUNC_BANK5
#define PORT_FUNC_BANK4
#define PORT_FUNC_BANK3
#define PORT_DATA_BANK6
#define PORT_DATA_BANK5
#define PORT_DATA_BANK4
#define PORT_DATA_BANK3
#define __NES__

#include "fixedPointSNES.h"
#include "port/port.h"

// Stub NES port functions (no-ops for benchmarking)
void port_buildSpriteIfDirty(uint8_t i, enum eOBJType t) { (void)i; (void)t; }
// port_buildUnused defined after mainBankZero.c include (needs GLOBAL_OBJList)
void port_updateCollapseTileNametable(uint8_t i) { (void)i; }
void port_beginSpriteBuild(const struct sPlayerData *p) { (void)p; }
void port_finishSpriteBuild(void) {}
void port_updatePlayerSprite(const struct sPlayerData *p) { (void)p; }
void port_resetSprites(void) {}
void port_drawText(const unsigned char *t, uint8_t x, uint8_t y) { (void)t; (void)x; (void)y; }
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

// port_prg_bank_switch already defined in port.h as no-op when not NES_UNROM_512

// Exclude mainBankZero's main() and onVblank
#define main game_main_excluded
#define onVblank game_onVblank_excluded
#include "mainBankZero.c"
#undef main
#undef onVblank

// Define after mainBankZero.c so GLOBAL_OBJList is available
void port_buildUnused(uint8_t i) { GLOBAL_OBJList[i].flags &= (uint8_t)~OBJ_FLAG_DIRTY; }

// Level data - include the NES level headers for object/spawn data
// We'll use a simplified approach: just init objects from the objectData arrays

// Collision data from the shared GID tables
#include "../python/gid_to_tile_shared.h"

// Simulated level object data (extracted from level headers)
// For the benchmark we just need object types and positions
typedef struct {
    uint8_t objectCount;
    uint8_t objectData[64]; // type, x, y triplets
    uint8_t spawnX;
    uint8_t spawnY;
} BenchLevelData;

// We'll generate simple test scenarios per level
static void setup_level(uint8_t levelNum) {
    uint8_t i;
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = levelNum;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;

    // Fill collision - bottom 2 rows are solid floor
    for (i = 0; i < 256; i++) {
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = (i >= 224) ? 0x01 : 0x00;
        GLOBAL_ActiveLevel.collisionFlagsReset[i] = GLOBAL_ActiveLevel.collisionFlagsArr[i];
    }

    // Clear objects
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
        GLOBAL_OBJList[i].extraSpriteCount = 0;
        GLOBAL_OBJList[i].flags = 0;
    }
    GLOBAL_ActiveLevel.movingPlatformCount = 0;

    // Player at spawn
    GLOBAL_PlayerData.objData.eType = OBJ_PLAYER;
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 192;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(192);
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = 0;
    GLOBAL_PlayerData.dashesLeft = 1;
    GLOBAL_PlayerData.dashCounter = 0;
    GLOBAL_PlayerData.isFliped = false;
    GLOBAL_PlayerData.movingPlatformIndex = -1;
    GLOBAL_PlayerData.graceTimer = 0;
    GLOBAL_PlayerData.eSriteState = PLAYER_SPRITE_IDLE;
    GLOBAL_PlayerData.doubleDashUnlocked = false;

    // Create a typical object mix based on level complexity
    // Low levels: few objects, high levels: many
    uint8_t numObjects = 3 + (levelNum % 8); // 3-10 objects
    port_prg_bank_switch(6);
    for (i = 0; i < numObjects && i < 12; i++) {
        uint8_t type;
        int16_t ox = 48 + (i * 16);
        int16_t oy = 192 - 16;
        switch (i % 6) {
            case 0: type = OBJ_COLLAPSE_TILE; oy = 176; break;
            case 1: type = OBJ_SPRING; oy = 176; break;
            case 2: type = OBJ_BALLOON; oy = 96; break;
            case 3: type = OBJ_STRAWBERRY; oy = 128; break;
            case 4: type = OBJ_DECO_TREE; oy = 176; break;
            case 5: type = OBJ_SMOKE; break;
        }
        initObject(type, ox, oy);
    }
    // Some levels get moving platforms
    if (levelNum >= 5 && levelNum <= 20) {
        initObject(OBJ_PLATMOV_R, 80, 160);
    }
    // Some levels get breakable walls
    if (levelNum >= 8) {
        initObject(OBJ_BREAKABLE_WALL, 128, 160);
    }
    port_prg_bank_switch(0);
}

static void print_num(unsigned long n) {
    char buf[12];
    int i = 0;
    if (n == 0) { putchar('0'); return; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (--i >= 0) putchar(buf[i]);
}

int main(void) {
    uint8_t level;
    unsigned long cyc_objects, cyc_player, cyc_total;

    printf("Lvl ObjUpd Player Total  Objs\n");

    for (level = 1; level <= 31; level++) {
        setup_level(level);
        GLOBAL_FrameCount = 10; // Avoid frame 0 special cases
        GLOBAL_InputState = PORT_INPUT_RIGHT_MASK; // Simulate holding right

        // Measure object update loop
        reset_clock();
        port_beginSpriteBuild(&GLOBAL_PlayerData);
        port_prg_bank_switch(6);
        {
            uint8_t i;
            for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
                processObject(i);
            }
        }
        port_prg_bank_switch(0);
        port_finishSpriteBuild();
        cyc_objects = clock();

        // Measure playerUpdate
        reset_clock();
        playerUpdate(&GLOBAL_PlayerData);
        cyc_player = clock();

        cyc_total = cyc_objects + cyc_player;

        // Count active objects
        uint8_t activeObjs = 0;
        { uint8_t i; for (i = 1; i < GLOBAL_OBJ_LIST_SIZE; i++) {
            if (GLOBAL_OBJList[i].eType != OBJ_UNUSED) activeObjs++;
        }}

        // Print: Lvl ObjUpdate PlayerUpdate Total ActiveObjects
        printf(" ");
        if (level < 10) putchar(' ');
        print_num(level);
        putchar(' ');
        print_num(cyc_objects);
        putchar(' ');
        print_num(cyc_player);
        putchar(' ');
        print_num(cyc_total);
        printf("  ");
        print_num(activeObjs);
        putchar('\n');
    }

    printf("\nNES has ~29780 cycles/frame at 60fps\n");
    return 0;
}
