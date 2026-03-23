// Jump arc tracer - compare exact Y positions per frame to ccleste
// Build: mos-sim-clang -Os -o bench/trace_jump bench/trace_jump.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run:   mos-sim bench/trace_jump

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
    GLOBAL_ActiveLevel.playerSpawnY = 13;
    // Floor at tile row 14 (y=112 in 128px)
    for (i = 0; i < 256; i++) {
        uint8_t r = i >> 4;
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = (r >= 14) ? 0x01 : 0;
    }
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(OBJ_DATA));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = 0xFF;
    }
    memset(&GLOBAL_PlayerData, 0, sizeof(GLOBAL_PlayerData));
    GLOBAL_PlayerData.objData.eType = OBJ_PLAYER;
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 104;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(104);
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
    uint8_t i;

    fputs("=== Jump Arc Trace ===\n", stdout);
    fputs("Frame | pos.y | spd.y(fix) | render.y\n", stdout);

    setup_ground();

    // Clear button state
    GLOBAL_InputState = 0;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    // Reset position after clearing frame
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 104;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(104);
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = 0;
    GLOBAL_PlayerData.graceTimer = 6;

    // Frame 0: press jump
    GLOBAL_InputState = PORT_INPUT_B_MASK;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    fputs("  0   | ", stdout);
    print_int(GLOBAL_PlayerData.objData.pos.y);
    fputs("  | ", stdout);
    print_int(GLOBAL_PlayerData.spd.y);
    fputs("  | ", stdout);
    print_int(GLOBAL_PlayerData.objData.pos.y * 2);
    putchar('\n');

    // Release jump, trace arc
    GLOBAL_InputState = 0;
    for (i = 1; i <= 25; i++) {
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        fputs("  ", stdout);
        print_int(i);
        fputs("  | ", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.y);
        fputs("  | ", stdout);
        print_int(GLOBAL_PlayerData.spd.y);
        fputs("  | ", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.y * 2);
        putchar('\n');
    }

    fputs("\n=== Run Right Trace (maxrun check) ===\n", stdout);
    setup_ground();
    GLOBAL_InputState = 0;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;
    for (i = 0; i < 15; i++) {
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        fputs("  ", stdout);
        print_int(i);
        fputs("  | x=", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.x);
        fputs(" spd.x=", stdout);
        print_int(GLOBAL_PlayerData.spd.x);
        putchar('\n');
    }

    fputs("\n=== Dash Right Trace ===\n", stdout);
    setup_ground();
    GLOBAL_InputState = 0;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    GLOBAL_InputState = PORT_INPUT_Y_MASK | PORT_INPUT_RIGHT_MASK;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    fputs("  0  | x=", stdout);
    print_int(GLOBAL_PlayerData.objData.pos.x);
    fputs(" spd.x=", stdout);
    print_int(GLOBAL_PlayerData.spd.x);
    fputs(" dashCtr=", stdout);
    print_int(GLOBAL_PlayerData.dashCounter);
    fputs(" target=", stdout);
    print_int(GLOBAL_PlayerData.dashTarget.x);
    putchar('\n');

    // Continue dash
    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;
    for (i = 1; i <= 8; i++) {
        // Skip freeze frames
        if (GLOBAL_FreezeFrames > 0) {
            GLOBAL_FreezeFrames--;
            continue;
        }
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        fputs("  ", stdout);
        print_int(i);
        fputs("  | x=", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.x);
        fputs(" spd.x=", stdout);
        print_int(GLOBAL_PlayerData.spd.x);
        fputs(" dashCtr=", stdout);
        print_int(GLOBAL_PlayerData.dashCounter);
        putchar('\n');
    }

    return 0;
}
