// Full benchmark: all player actions x object counts
// mos-sim-clang -Os -flto -o bench/bench_all bench/bench_all.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython

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

static void setup(uint8_t nObjs) {
    uint16_t i;
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = 1;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    GLOBAL_ActiveLevel.playerSpawnX = 4;
    GLOBAL_ActiveLevel.playerSpawnY = 12;
    for (i = 0; i < 256; i++) {
        uint8_t r = (uint8_t)(i >> 4);
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = (r >= 14) ? 0x01 : 0;
        GLOBAL_ActiveLevel.collisionFlagsReset[i] = GLOBAL_ActiveLevel.collisionFlagsArr[i];
    }
    // walls
    for (i = 0; i < 16; i++) {
        GLOBAL_ActiveLevel.collisionFlagsArr[i*16] |= 0x01;
        GLOBAL_ActiveLevel.collisionFlagsArr[i*16+15] |= 0x01;
    }
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(OBJ_DATA));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = 0xFF;
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
    GLOBAL_PlayerData.graceTimer = 6;
    GLOBAL_FrameCount = 10;
    GLOBAL_FreezeFrames = 0;
    GLOBAL_PausePlayerFrames = 0;

    for (i = 0; i < nObjs && (i+1) < GLOBAL_OBJ_LIST_SIZE; i++) {
        uint8_t s = (uint8_t)(i + 1);
        GLOBAL_OBJList[s].flags = OBJ_FLAG_DIRTY;
        GLOBAL_OBJList[s].pos.x = (int16_t)(48 + i * 16);
        GLOBAL_OBJList[s].oamProps = 0x32;
        switch (i % 5) {
        case 0: GLOBAL_OBJList[s].eType=OBJ_COLLAPSE_TILE; GLOBAL_OBJList[s].pos.y=208;
                GLOBAL_OBJList[s].data.collapseTile.linkedSpringIndex=-1; break;
        case 1: GLOBAL_OBJList[s].eType=OBJ_BALLOON; GLOBAL_OBJList[s].pos.y=96; break;
        case 2: GLOBAL_OBJList[s].eType=OBJ_STRAWBERRY; GLOBAL_OBJList[s].pos.y=128;
                GLOBAL_OBJList[s].data.strawberry.startY=128; break;
        case 3: GLOBAL_OBJList[s].eType=OBJ_SPRING; GLOBAL_OBJList[s].pos.y=208;
                GLOBAL_OBJList[s].data.spring.linkedCollapseTileIndex=-1; break;
        case 4: GLOBAL_OBJList[s].eType=OBJ_DECO_TREE; GLOBAL_OBJList[s].pos.y=208; break;
        }
    }
}

static void print_padded(unsigned long n, uint8_t width) {
    char buf[8]; uint8_t len = 0;
    unsigned long v = n;
    if (v == 0) { buf[len++] = '0'; }
    else { while(v) { buf[len++] = '0'+(char)(v%10); v/=10; } }
    while (len < width) { putchar(' '); width--; }
    while (len--) putchar(buf[len]);
}

static void clamp(void) {
    if (GLOBAL_PlayerData.objData.pos.y > 210 || GLOBAL_PlayerData.objData.pos.y < 0) {
        GLOBAL_PlayerData.objData.pos.y = 192;
        GLOBAL_PlayerData.posF.y = INT_TO_FIXED(192);
        GLOBAL_PlayerData.spd.y = 0;
        GLOBAL_PlayerData.graceTimer = 6;
    }
    if (GLOBAL_PlayerData.objData.pos.x < 20 || GLOBAL_PlayerData.objData.pos.x > 220) {
        GLOBAL_PlayerData.objData.pos.x = 100;
        GLOBAL_PlayerData.posF.x = INT_TO_FIXED(100);
    }
}

#define BUDGET 25780

static void run_test(const char *name, uint8_t nObjs, uint8_t input, uint8_t frames) {
    uint8_t f, j;
    unsigned long total = 0, mx = 0, c;
    setup(nObjs);
    for (f = 0; f < frames; f++) {
        clamp();
        GLOBAL_InputState = input;
        GLOBAL_FrameCount++;
        reset_clock();
        for (j = 0; j < GLOBAL_OBJ_LIST_SIZE; j++) processObject(j);
        playerUpdate(&GLOBAL_PlayerData);
        c = clock();
        total += c;
        if (c > mx) mx = c;
    }
    printf("%-11s %2dobj ", name, nObjs);
    printf("avg="); print_padded(total/frames, 5);
    printf(" max="); print_padded(mx, 5);
    printf(" %3lu%%", (mx * 100) / BUDGET);
    if (mx > BUDGET) printf(" OVER!");
    putchar('\n');
}

int main(void) {
    printf("NES Celeste Frame Benchmark\n");
    printf("Budget: %d cyc (29780-4000 vblank)\n\n", BUDGET);

    uint8_t objCounts[] = {0, 3, 6, 10};
    uint8_t oi;
    for (oi = 0; oi < 4; oi++) {
        uint8_t n = objCounts[oi];
        run_test("run_right",  n, 0x01, 5);                    // RIGHT
        run_test("jump+drift", n, 0x81, 5);                    // RIGHT+JUMP
        run_test("dash_right", n, 0x41, 5);                    // RIGHT+DASH
        run_test("dash_diag",  n, 0x49, 5);                    // RIGHT+UP+DASH
        run_test("wall_slide", n, 0x01, 5);                    // run into wall
        run_test("air_fall",   n, 0x00, 5);                    // no input (fall)
        putchar('\n');
    }

    printf("Done.\n");
    return 0;
}
