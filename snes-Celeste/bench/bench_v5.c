// Simplest possible benchmark: measure one playerUpdate call
// mos-sim-clang -Os -o bench/bench_v5 bench/bench_v5.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// mos-sim --cycles bench/bench_v5

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

static void setup(void) {
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
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = (r >= 14) ? 0x01 : 0;
        GLOBAL_ActiveLevel.collisionFlagsReset[i] = GLOBAL_ActiveLevel.collisionFlagsArr[i];
    }
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(OBJ_DATA));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = 0xFF;
    }
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
}

int main(void) {
    unsigned long c;

    // Just test one thing at a time, no printf overhead
    // Test: playerUpdate running right on ground
    setup();
    GLOBAL_InputState = 0x01;
    GLOBAL_FrameCount = 11;
    reset_clock();
    playerUpdate(&GLOBAL_PlayerData);
    c = clock();
    // Print using putchar to minimize stack
    putchar('P'); putchar('U'); putchar(':');
    // Print cycle count digit by digit
    {
        char buf[8]; int n = 0;
        unsigned long v = c;
        if (v == 0) { putchar('0'); }
        else { while(v) { buf[n++] = '0' + (v % 10); v /= 10; } while(n--) putchar(buf[n]); }
    }
    putchar('\n');

    return 0;
}
