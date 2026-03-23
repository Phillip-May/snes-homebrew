// Performance benchmark: measure cycle counts for key operations
// Build: mos-sim-clang -Os -o bench/bench_perf bench/bench_perf.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run:   mos-sim --cycles bench/bench_perf

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
#include "fixedPointSNES.h"
#include "port/port.h"

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

#define main game_main_excluded
#define onVblank game_onVblank_excluded
#include "mainBankZero.c"
#undef main
#undef onVblank

void port_buildSpriteIfDirty(uint8_t i, enum eOBJType t) {
    GLOBAL_OBJList[i].flags &= (uint8_t)~OBJ_FLAG_DIRTY; (void)t;
}
void port_buildUnused(uint8_t i) { GLOBAL_OBJList[i].flags &= (uint8_t)~OBJ_FLAG_DIRTY; }

static void print_int(long v) {
    char buf[12]; int n = 0;
    if (v < 0) { putchar('-'); v = -v; }
    if (v == 0) { putchar('0'); return; }
    while(v) { buf[n++] = '0' + (v % 10); v /= 10; }
    while(n--) putchar(buf[n]);
}

static void setup_ground(void) {
    uint16_t i;
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = 1;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    GLOBAL_ActiveLevel.playerSpawnX = 4;
    GLOBAL_ActiveLevel.playerSpawnY = 12;
    for (i = 0; i < 256; i++) {
        uint8_t r = i >> 4;
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = (r >= 13) ? 0x01 : 0;
    }
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(OBJ_DATA));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = 0xFF;
    }
    memset(&GLOBAL_PlayerData, 0, sizeof(GLOBAL_PlayerData));
    GLOBAL_PlayerData.objData.eType = OBJ_PLAYER;
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 96;
    GLOBAL_PlayerData.posF.x = 0; // remainder accumulator
    GLOBAL_PlayerData.posF.y = 0;
    GLOBAL_PlayerData.dashesLeft = 1;
    GLOBAL_PlayerData.movingPlatformIndex = -1;
    GLOBAL_PlayerData.graceTimer = 6;
    GLOBAL_PlayerData.dashCounter = 0;
    GLOBAL_FrameCount = 10;
    GLOBAL_FreezeFrames = 0;
    GLOBAL_PausePlayerFrames = 0;
    GLOBAL_DoubleDashUnlocked = false;
}

int main(void) {
    unsigned long c;

    fputs("=== NES Performance Benchmark (6502 cycles) ===\n", stdout);
    fputs("NES NTSC VBlank budget: ~29,780 cycles\n\n", stdout);

    // --- playerUpdate: idle on ground ---
    setup_ground();
    GLOBAL_InputState = 0;
    GLOBAL_FrameCount = 12;
    // warm up statics
    playerUpdate(&GLOBAL_PlayerData);

    reset_clock();
    playerUpdate(&GLOBAL_PlayerData);
    c = clock();
    fputs("playerUpdate (idle):      ", stdout); print_int(c); fputs(" cyc\n", stdout);

    // --- playerUpdate: running right ---
    setup_ground();
    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;
    GLOBAL_FrameCount = 12;
    playerUpdate(&GLOBAL_PlayerData); // warm up

    reset_clock();
    playerUpdate(&GLOBAL_PlayerData);
    c = clock();
    fputs("playerUpdate (run):       ", stdout); print_int(c); fputs(" cyc\n", stdout);

    // --- playerUpdate: mid-air (gravity + movement) ---
    setup_ground();
    GLOBAL_PlayerData.objData.pos.y = 50;
    GLOBAL_PlayerData.posF.y = 0; // remainder accumulator
    GLOBAL_PlayerData.graceTimer = 0;
    GLOBAL_PlayerData.spd.y = INT_TO_FIXED(-1);
    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;
    GLOBAL_FrameCount = 12;

    reset_clock();
    playerUpdate(&GLOBAL_PlayerData);
    c = clock();
    fputs("playerUpdate (air):       ", stdout); print_int(c); fputs(" cyc\n", stdout);

    // --- OBJ_isSolidAt: single check ---
    setup_ground();
    reset_clock();
    {
        volatile bool r = OBJ_isSolidAt(&GLOBAL_PlayerData, 0, 1);
        (void)r;
    }
    c = clock();
    fputs("OBJ_isSolidAt (1 call):   ", stdout); print_int(c); fputs(" cyc\n", stdout);

    // --- isTileSolidAtPoint: single check ---
    setup_ground();
    reset_clock();
    {
        volatile bool r = isTileSolidAtPoint(32, 104);
        (void)r;
    }
    c = clock();
    fputs("isTileSolidAtPoint:       ", stdout); print_int(c); fputs(" cyc\n", stdout);

    // --- approachFixed ---
    {
        volatile fixed_t result;
        reset_clock();
        result = approachFixed(INT_TO_FIXED(-2), INT_TO_FIXED(2), 0x000035C2);
        c = clock();
        (void)result;
    }
    fputs("approachFixed:            ", stdout); print_int(c); fputs(" cyc\n", stdout);

    // --- isDeathAtPoint ---
    setup_ground();
    GLOBAL_ActiveLevel.collisionFlagsArr[13 * 16 + 4] |= 0x04; // spike
    reset_clock();
    {
        volatile bool r = isDeathAtPoint(33, 99, 6, 5, 0, 1);
        (void)r;
    }
    c = clock();
    fputs("isDeathAtPoint:           ", stdout); print_int(c); fputs(" cyc\n", stdout);

    // --- updateAllObjects: all unused ---
    setup_ground();
    reset_clock();
    updateAllObjects();
    c = clock();
    fputs("updateAllObjects (empty): ", stdout); print_int(c); fputs(" cyc\n", stdout);

    // --- updateAllObjects: 3 objects ---
    setup_ground();
    initObject(OBJ_SMOKE, 40, 80);
    initObject(OBJ_SMOKE, 50, 80);
    initObject(OBJ_SMOKE, 60, 80);
    GLOBAL_FrameCount = 12;
    reset_clock();
    updateAllObjects();
    c = clock();
    fputs("updateAllObjects (3 obj): ", stdout); print_int(c); fputs(" cyc\n", stdout);

    // --- FIXED_ADD (baseline) ---
    {
        volatile fixed_t a = INT_TO_FIXED(10), b = INT_TO_FIXED(20), r;
        reset_clock();
        r = FIXED_ADD(a, b);
        c = clock();
        (void)r;
    }
    fputs("FIXED_ADD:                ", stdout); print_int(c); fputs(" cyc\n", stdout);

    // --- FIXED_TO_INT with rounding ---
    {
        volatile fixed_t a = 0x0001C000; // 1.75
        volatile int r;
        reset_clock();
        r = FIXED_TO_INT(a);
        c = clock();
        (void)r;
    }
    fputs("FIXED_TO_INT (rounding):  ", stdout); print_int(c); fputs(" cyc\n", stdout);

    fputs("\nDone.\n", stdout);
    return 0;
}
