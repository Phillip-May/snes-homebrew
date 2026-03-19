// Minimal: test ONE frame of playerUpdate, print cycle count
// mos-sim-clang -Os -flto -o bench/bench_v4 bench/bench_v4.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// mos-sim --cycles bench/bench_v4

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
    uint8_t i;
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
    uint8_t i;

    // Test 1: playerUpdate only - running right on ground
    setup();
    GLOBAL_InputState = 0x01; // RIGHT
    GLOBAL_FrameCount++;
    reset_clock();
    playerUpdate(&GLOBAL_PlayerData);
    c = clock();
    printf("playerUpdate(run): %lu\n", c);

    // Test 2: playerUpdate - jump
    setup();
    GLOBAL_InputState = 0x81; // RIGHT+JUMP
    GLOBAL_FrameCount++;
    reset_clock();
    playerUpdate(&GLOBAL_PlayerData);
    c = clock();
    printf("playerUpdate(jump): %lu\n", c);

    // Test 3: playerUpdate - in air (falling)
    setup();
    GLOBAL_PlayerData.objData.pos.y = 100;
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(100);
    GLOBAL_PlayerData.spd.y = INT_TO_FIXED(1);
    GLOBAL_PlayerData.graceTimer = 0;
    GLOBAL_InputState = 0x01; // RIGHT
    GLOBAL_FrameCount++;
    reset_clock();
    playerUpdate(&GLOBAL_PlayerData);
    c = clock();
    printf("playerUpdate(air): %lu\n", c);

    // Test 4: playerUpdate - dash
    setup();
    GLOBAL_InputState = 0x41; // RIGHT+DASH(Y)
    GLOBAL_FrameCount++;
    reset_clock();
    playerUpdate(&GLOBAL_PlayerData);
    c = clock();
    printf("playerUpdate(dash): %lu\n", c);

    // Test 5: processObject loop with 0 active objects
    setup();
    reset_clock();
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) processObject(i);
    c = clock();
    printf("processObj(0 active): %lu\n", c);

    // Test 6: processObject loop with 5 active objects
    setup();
    GLOBAL_OBJList[1].eType = OBJ_COLLAPSE_TILE; GLOBAL_OBJList[1].pos.y=208;
    GLOBAL_OBJList[1].data.collapseTile.linkedSpringIndex=-1;
    GLOBAL_OBJList[1].flags = OBJ_FLAG_DIRTY;
    GLOBAL_OBJList[2].eType = OBJ_BALLOON; GLOBAL_OBJList[2].pos.y=96;
    GLOBAL_OBJList[2].flags = OBJ_FLAG_DIRTY;
    GLOBAL_OBJList[3].eType = OBJ_STRAWBERRY; GLOBAL_OBJList[3].pos.y=128;
    GLOBAL_OBJList[3].data.strawberry.startY=128;
    GLOBAL_OBJList[3].flags = OBJ_FLAG_DIRTY;
    GLOBAL_OBJList[4].eType = OBJ_SPRING; GLOBAL_OBJList[4].pos.y=208;
    GLOBAL_OBJList[4].data.spring.linkedCollapseTileIndex=-1;
    GLOBAL_OBJList[4].flags = OBJ_FLAG_DIRTY;
    GLOBAL_OBJList[5].eType = OBJ_DECO_TREE; GLOBAL_OBJList[5].pos.y=208;
    GLOBAL_OBJList[5].flags = OBJ_FLAG_DIRTY;
    reset_clock();
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) processObject(i);
    c = clock();
    printf("processObj(5 active): %lu\n", c);

    // Test 7: processObject with 10 active
    setup();
    {
        uint8_t types[] = {OBJ_COLLAPSE_TILE,OBJ_BALLOON,OBJ_STRAWBERRY,OBJ_SPRING,
                           OBJ_DECO_TREE,OBJ_COLLAPSE_TILE,OBJ_BALLOON,OBJ_SPRING,
                           OBJ_STRAWBERRY,OBJ_DECO_TREE};
        for (i=0;i<10;i++) {
            GLOBAL_OBJList[i+1].eType=types[i];
            GLOBAL_OBJList[i+1].pos.y=(types[i]==OBJ_BALLOON)?96:208;
            GLOBAL_OBJList[i+1].pos.x=32+i*16;
            GLOBAL_OBJList[i+1].flags=OBJ_FLAG_DIRTY;
            if(types[i]==OBJ_STRAWBERRY) GLOBAL_OBJList[i+1].data.strawberry.startY=208;
            if(types[i]==OBJ_COLLAPSE_TILE) GLOBAL_OBJList[i+1].data.collapseTile.linkedSpringIndex=-1;
            if(types[i]==OBJ_SPRING) GLOBAL_OBJList[i+1].data.spring.linkedCollapseTileIndex=-1;
        }
    }
    reset_clock();
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) processObject(i);
    c = clock();
    printf("processObj(10 active): %lu\n", c);

    // Test 8: full frame (10 objects + playerUpdate running right)
    // Don't re-setup, reuse 10 objects from above
    GLOBAL_InputState = 0x01;
    GLOBAL_FrameCount++;
    reset_clock();
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) processObject(i);
    playerUpdate(&GLOBAL_PlayerData);
    c = clock();
    printf("full frame(10obj+run): %lu\n", c);

    printf("\nBudget: 25780 cyc/frame\n");
    return 0;
}
