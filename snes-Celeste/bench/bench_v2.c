// Minimal frame benchmark — measures actual game functions on 6502 sim
// mos-sim-clang -Os -flto -o bench/bench_v2 bench/bench_v2.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// mos-sim bench/bench_v2

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

// --- Minimal setup ---
static void setup(uint8_t nObjs) {
    uint8_t i;
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = 1;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    for (i = 0; i < 256; i++) {
        uint8_t r = i >> 4;
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = (r >= 14) ? 0x01 : 0;
        GLOBAL_ActiveLevel.collisionFlagsReset[i] = GLOBAL_ActiveLevel.collisionFlagsArr[i];
    }
    // Left/right walls
    for (i = 0; i < 16; i++) {
        GLOBAL_ActiveLevel.collisionFlagsArr[i * 16] |= 0x01;
        GLOBAL_ActiveLevel.collisionFlagsArr[i * 16 + 15] |= 0x01;
    }

    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(OBJ_DATA));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    }
    GLOBAL_ActiveLevel.movingPlatformCount = 0;

    memset(&GLOBAL_PlayerData, 0, sizeof(GLOBAL_PlayerData));
    GLOBAL_PlayerData.objData.eType = OBJ_PLAYER;
    GLOBAL_PlayerData.objData.pos.x = 64;
    GLOBAL_PlayerData.objData.pos.y = 192;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(64);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(192);
    GLOBAL_PlayerData.dashesLeft = 1;
    GLOBAL_PlayerData.movingPlatformIndex = -1;

    // Place some objects
    for (i = 0; i < nObjs && (i+1) < GLOBAL_OBJ_LIST_SIZE; i++) {
        uint8_t s = i + 1;
        GLOBAL_OBJList[s].flags = OBJ_FLAG_DIRTY;
        GLOBAL_OBJList[s].pos.x = 48 + i * 16;
        GLOBAL_OBJList[s].oamProps = 0x32;
        switch (i % 5) {
        case 0:
            GLOBAL_OBJList[s].eType = OBJ_COLLAPSE_TILE;
            GLOBAL_OBJList[s].pos.y = 208;
            GLOBAL_OBJList[s].data.collapseTile.state = 0;
            GLOBAL_OBJList[s].data.collapseTile.linkedSpringIndex = -1;
            break;
        case 1:
            GLOBAL_OBJList[s].eType = OBJ_BALLOON;
            GLOBAL_OBJList[s].pos.y = 96;
            GLOBAL_OBJList[s].data.balloon.state = 0;
            break;
        case 2:
            GLOBAL_OBJList[s].eType = OBJ_STRAWBERRY;
            GLOBAL_OBJList[s].pos.y = 128;
            GLOBAL_OBJList[s].data.strawberry.startY = 128;
            break;
        case 3:
            GLOBAL_OBJList[s].eType = OBJ_SPRING;
            GLOBAL_OBJList[s].pos.y = 208;
            GLOBAL_OBJList[s].data.spring.linkedCollapseTileIndex = -1;
            break;
        case 4:
            GLOBAL_OBJList[s].eType = OBJ_DECO_TREE;
            GLOBAL_OBJList[s].pos.y = 208;
            break;
        }
    }
    GLOBAL_FrameCount = 10;
}

static void clamp_player(void) {
    if (GLOBAL_PlayerData.objData.pos.y > 210) {
        GLOBAL_PlayerData.objData.pos.y = 192;
        GLOBAL_PlayerData.posF.y = INT_TO_FIXED(192);
        GLOBAL_PlayerData.spd.y = 0;
    }
    if (GLOBAL_PlayerData.objData.pos.y < 10) {
        GLOBAL_PlayerData.objData.pos.y = 50;
        GLOBAL_PlayerData.posF.y = INT_TO_FIXED(50);
    }
    if (GLOBAL_PlayerData.objData.pos.x < 20) {
        GLOBAL_PlayerData.objData.pos.x = 32;
        GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    }
    if (GLOBAL_PlayerData.objData.pos.x > 220) {
        GLOBAL_PlayerData.objData.pos.x = 200;
        GLOBAL_PlayerData.posF.x = INT_TO_FIXED(200);
    }
}

#define FRAMES_PER_TEST 20
#define BUDGET 25780

static void pnum(unsigned long n) {
    char b[12]; int i=0;
    if (!n){putchar('0');return;}
    while(n){b[i++]='0'+n%10;n/=10;}
    while(--i>=0)putchar(b[i]);
}

static unsigned long test_scenario(const char *name, uint8_t nObjs, const uint8_t *inputs) {
    uint8_t f;
    unsigned long total = 0, mx = 0, c;
    setup(nObjs);
    for (f = 0; f < FRAMES_PER_TEST; f++) {
        GLOBAL_InputState = inputs[f];
        GLOBAL_FrameCount++;
        reset_clock();
        {
            uint8_t i;
            for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++)
                processObject(i);
        }
        playerUpdate(&GLOBAL_PlayerData);
        c = clock();
        total += c;
        if (c > mx) mx = c;
        clamp_player();
    }
    printf("%-12s %2d obj  avg=", name, nObjs);
    pnum(total / FRAMES_PER_TEST);
    printf("  max=");
    pnum(mx);
    if (mx > BUDGET) printf(" **OVER**");
    printf("  [%lu%%]\n", (mx * 100) / BUDGET);
    return mx;
}

int main(void) {
    unsigned long worst = 0, c;

    printf("=== NES Celeste Frame Benchmark ===\n");
    printf("Budget: %d cyc (29780 - 4000 vblank)\n\n", BUDGET);

    // --- Test each action with varying object counts ---
    static const uint8_t inp_run[FRAMES_PER_TEST] = {
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
    static const uint8_t inp_jump[FRAMES_PER_TEST] = {
        0x81,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
    static const uint8_t inp_dash[FRAMES_PER_TEST] = {
        0x41,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x81,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
    static const uint8_t inp_dashdiag[FRAMES_PER_TEST] = {
        0x49,0x09,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
    static const uint8_t inp_walljump[FRAMES_PER_TEST] = {
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x82,0x02,0x02,0x81,0x01,0x01,0x82,0x02,0x02,0x01};
    static const uint8_t inp_air[FRAMES_PER_TEST] = {
        0x81,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};

    uint8_t objCounts[] = {0, 3, 6, 10, 15};
    uint8_t oi;

    for (oi = 0; oi < 5; oi++) {
        uint8_t n = objCounts[oi];
        printf("--- %d objects ---\n", n);
        c = test_scenario("run", n, inp_run); if(c>worst)worst=c;
        c = test_scenario("jump+drift", n, inp_jump); if(c>worst)worst=c;
        c = test_scenario("dash_right", n, inp_dash); if(c>worst)worst=c;
        c = test_scenario("dash_diag", n, inp_dashdiag); if(c>worst)worst=c;
        c = test_scenario("wall_jump", n, inp_walljump); if(c>worst)worst=c;
        c = test_scenario("air_fall", n, inp_air); if(c>worst)worst=c;
    }

    printf("\n=== Worst case: ");
    pnum(worst);
    printf(" / %d = ", BUDGET);
    pnum((worst * 100) / BUDGET);
    printf("%% ===\n");
    if (worst <= BUDGET) printf("PASS: Fits in frame budget!\n");
    else printf("FAIL: Over budget by %lu cycles\n", worst - BUDGET);

    return 0;
}
