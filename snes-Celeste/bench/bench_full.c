// Full frame benchmark: simulates actual gameplay across all 31 levels
// Tests: running, jumping, dashing, air drift, wall slide, landing
// Compile: mos-sim-clang -Os -flto -o bench/bench_full bench/bench_full.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run: mos-sim bench/bench_full

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define _WIN32  // Prevent NES hardware includes
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

// Forward-declared stubs — defined after mainBankZero.c include
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

// Stubs that need GLOBAL_OBJList (now available after include)
void port_buildSpriteIfDirty(uint8_t i, enum eOBJType t) {
    GLOBAL_OBJList[i].flags &= (uint8_t)~OBJ_FLAG_DIRTY; (void)t;
}
void port_buildUnused(uint8_t i) { GLOBAL_OBJList[i].flags &= (uint8_t)~OBJ_FLAG_DIRTY; }

// --- Level setup ---
// Object mixes that approximate each level's actual complexity
static const uint8_t level_obj_types[][12] = {
    // Lvl 1: simple (2 collapse, 1 strawberry)
    {OBJ_COLLAPSE_TILE, OBJ_COLLAPSE_TILE, OBJ_STRAWBERRY, 0,0,0,0,0,0,0,0,0},
    // Lvl 2-3: springs + collapse
    {OBJ_COLLAPSE_TILE, OBJ_SPRING, OBJ_STRAWBERRY, OBJ_COLLAPSE_TILE, 0,0,0,0,0,0,0,0},
    {OBJ_COLLAPSE_TILE, OBJ_SPRING, OBJ_COLLAPSE_TILE, OBJ_SPRING, OBJ_STRAWBERRY, 0,0,0,0,0,0,0},
    // Lvl 4-5: balloons
    {OBJ_BALLOON, OBJ_COLLAPSE_TILE, OBJ_SPRING, OBJ_STRAWBERRY, OBJ_DECO_TREE, 0,0,0,0,0,0,0},
    {OBJ_BALLOON, OBJ_BALLOON, OBJ_COLLAPSE_TILE, OBJ_SPRING, OBJ_STRAWBERRY, OBJ_PLATMOV_R, 0,0,0,0,0,0},
    // Lvl 6-7: moving platforms + more objects (stress test levels)
    {OBJ_PLATMOV_R, OBJ_PLATMOV_L, OBJ_COLLAPSE_TILE, OBJ_COLLAPSE_TILE, OBJ_SPRING, OBJ_BALLOON, OBJ_STRAWBERRY, OBJ_DECO_TREE, 0,0,0,0},
    {OBJ_PLATMOV_R, OBJ_COLLAPSE_TILE, OBJ_COLLAPSE_TILE, OBJ_COLLAPSE_TILE, OBJ_SPRING, OBJ_SPRING, OBJ_BALLOON, OBJ_STRAWBERRY, OBJ_FLYING_BERRY, OBJ_DECO_TREE, 0,0},
    // Lvl 8+: breakable wall levels
    {OBJ_BREAKABLE_WALL, OBJ_COLLAPSE_TILE, OBJ_SPRING, OBJ_BALLOON, OBJ_STRAWBERRY, OBJ_KEY, OBJ_CHEST, OBJ_DECO_TREE, OBJ_DECO_FLOWER, 0,0,0},
    // Lvl 9-10: heavy levels
    {OBJ_BREAKABLE_WALL, OBJ_PLATMOV_R, OBJ_COLLAPSE_TILE, OBJ_COLLAPSE_TILE, OBJ_SPRING, OBJ_BALLOON, OBJ_BALLOON, OBJ_STRAWBERRY, OBJ_FLYING_BERRY, OBJ_DECO_TREE, OBJ_DECO_FLOWER, 0},
    {OBJ_BREAKABLE_WALL, OBJ_PLATMOV_R, OBJ_PLATMOV_L, OBJ_COLLAPSE_TILE, OBJ_COLLAPSE_TILE, OBJ_COLLAPSE_TILE, OBJ_SPRING, OBJ_SPRING, OBJ_BALLOON, OBJ_STRAWBERRY, OBJ_FLYING_BERRY, OBJ_DECO_TREE},
};

static void setup_level_sim(uint8_t levelNum) {
    uint8_t i;
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = levelNum;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;

    // Collision: floor at row 14-15, walls at col 0 and 15
    for (i = 0; i < 256; i++) {
        uint8_t row = i >> 4, col = i & 0x0F;
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = 0;
        if (row >= 14) GLOBAL_ActiveLevel.collisionFlagsArr[i] |= 0x01; // floor
        if (col == 0 || col == 15) GLOBAL_ActiveLevel.collisionFlagsArr[i] |= 0x01; // walls
        if (row == 0) GLOBAL_ActiveLevel.collisionFlagsArr[i] |= 0x08; // ceiling spikes
        GLOBAL_ActiveLevel.collisionFlagsReset[i] = GLOBAL_ActiveLevel.collisionFlagsArr[i];
    }

    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
        GLOBAL_OBJList[i].extraSpriteCount = 0;
        GLOBAL_OBJList[i].flags = 0;
    }
    GLOBAL_ActiveLevel.movingPlatformCount = 0;

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
    GLOBAL_PlayerData.graceTimer = 6;
    GLOBAL_PlayerData.eSriteState = PLAYER_SPRITE_IDLE;
    GLOBAL_PlayerData.doubleDashUnlocked = false;
    GLOBAL_PlayerData.objData.flags = OBJ_FLAG_DIRTY;

    // Manually set up objects (avoids initObject's complex init functions)
    uint8_t pattern = (levelNum <= 10) ? (levelNum - 1) : ((levelNum - 1) % 10);
    const uint8_t *types = level_obj_types[pattern];
    for (i = 0; i < 12 && types[i] != 0; i++) {
        uint8_t slot = i + 1;
        if (slot >= GLOBAL_OBJ_LIST_SIZE) break;
        GLOBAL_OBJList[slot].eType = types[i];
        GLOBAL_OBJList[slot].pos.x = 32 + ((i + 1) * 16);
        if (types[i] == OBJ_BALLOON) GLOBAL_OBJList[slot].pos.y = 96;
        else if (types[i] == OBJ_STRAWBERRY || types[i] == OBJ_FLYING_BERRY || types[i] == OBJ_KEY) GLOBAL_OBJList[slot].pos.y = 128;
        else GLOBAL_OBJList[slot].pos.y = 208;
        GLOBAL_OBJList[slot].flags = OBJ_FLAG_DIRTY;
        GLOBAL_OBJList[slot].oamTile = 0;
        GLOBAL_OBJList[slot].oamProps = 0x32;
        // Minimal data init per type
        if (types[i] == OBJ_BALLOON) {
            GLOBAL_OBJList[slot].data.balloon.state = 0;
            GLOBAL_OBJList[slot].data.balloon.frameCount = 0;
            GLOBAL_OBJList[slot].data.balloon.yTableIndex = 0;
            GLOBAL_OBJList[slot].data.balloon.hideFrameCount = 0;
            GLOBAL_OBJList[slot].data.balloon.spriteYOffset = 0;
        } else if (types[i] == OBJ_COLLAPSE_TILE) {
            GLOBAL_OBJList[slot].data.collapseTile.state = 0;
            GLOBAL_OBJList[slot].data.collapseTile.frameCount = 0;
            GLOBAL_OBJList[slot].data.collapseTile.linkedSpringIndex = -1;
        } else if (types[i] == OBJ_SPRING) {
            GLOBAL_OBJList[slot].data.spring.frameCount = 0;
            GLOBAL_OBJList[slot].data.spring.isDisabled = false;
            GLOBAL_OBJList[slot].data.spring.linkedCollapseTileIndex = -1;
        } else if (types[i] == OBJ_STRAWBERRY || types[i] == OBJ_FLYING_BERRY) {
            GLOBAL_OBJList[slot].data.strawberry.frameCount = 0;
            GLOBAL_OBJList[slot].data.strawberry.isCollected = false;
            GLOBAL_OBJList[slot].data.strawberry.startY = GLOBAL_OBJList[slot].pos.y;
        } else if (types[i] == OBJ_PLATMOV_R || types[i] == OBJ_PLATMOV_L) {
            GLOBAL_OBJList[slot].data.platMov.acc = 0;
            GLOBAL_OBJList[slot].data.platMov.hitboxIndex = GLOBAL_ActiveLevel.movingPlatformCount * 4;
            GLOBAL_OBJList[slot].data.platMov.isMovingLeft = (types[i] == OBJ_PLATMOV_L);
            GLOBAL_ActiveLevel.movingPlatformCount++;
        } else if (types[i] == OBJ_KEY) {
            GLOBAL_OBJList[slot].data.key.frameCount = 0;
            GLOBAL_OBJList[slot].data.key.state = 0;
            GLOBAL_OBJList[slot].data.key.linkedChestIndex = 0xFF;
        } else if (types[i] == OBJ_CHEST) {
            GLOBAL_OBJList[slot].data.chest.state = 0;
            GLOBAL_OBJList[slot].data.chest.keyIsCollected = false;
        }
    }
}

// Input sequences simulating player actions
#define INPUT_RIGHT  PORT_INPUT_RIGHT_MASK
#define INPUT_LEFT   PORT_INPUT_LEFT_MASK
#define INPUT_JUMP   PORT_INPUT_B_MASK
#define INPUT_DASH   PORT_INPUT_Y_MASK
#define INPUT_UP     PORT_INPUT_UP_MASK
#define INPUT_DOWN   PORT_INPUT_DOWN_MASK

// Each test: 30 frames of a specific action
typedef struct {
    const char *name;
    uint8_t inputs[30];
} TestScenario;

static const TestScenario scenarios[] = {
    {"run_right", {
        INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,
        INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,
        INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,
        INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,
        INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,
        INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,
    }},
    {"jump+drift", {
        INPUT_RIGHT|INPUT_JUMP, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
    }},
    {"dash_right", {
        INPUT_RIGHT|INPUT_DASH, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT|INPUT_JUMP, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
    }},
    {"dash_diag", {
        INPUT_RIGHT|INPUT_UP|INPUT_DASH, INPUT_RIGHT|INPUT_UP, INPUT_RIGHT|INPUT_UP,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_RIGHT, INPUT_RIGHT,
    }},
    {"wall_jump", {
        INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,
        INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,INPUT_RIGHT,
        INPUT_RIGHT,INPUT_RIGHT, // run to wall
        INPUT_LEFT|INPUT_JUMP, INPUT_LEFT, INPUT_LEFT, INPUT_LEFT, INPUT_LEFT,
        INPUT_RIGHT|INPUT_JUMP, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
        INPUT_LEFT|INPUT_JUMP, INPUT_LEFT, INPUT_LEFT, INPUT_LEFT, INPUT_LEFT,
        INPUT_RIGHT, INPUT_RIGHT, INPUT_RIGHT,
    }},
};
#define NUM_SCENARIOS 5

static void print_num(unsigned long n) {
    char buf[12]; int i = 0;
    if (n == 0) { putchar('0'); return; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (--i >= 0) putchar(buf[i]);
}

static void run_frame(uint8_t input) {
    uint8_t i;
    GLOBAL_InputState = input;
    GLOBAL_FrameCount++;
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        processObject(i);
    }
    playerUpdate(&GLOBAL_PlayerData);
    // Clamp player to valid range to prevent death loop
    if (GLOBAL_PlayerData.objData.pos.y > 220)
        GLOBAL_PlayerData.objData.pos.y = 192;
    if (GLOBAL_PlayerData.objData.pos.y < 0)
        GLOBAL_PlayerData.objData.pos.y = 50;
    if (GLOBAL_PlayerData.objData.pos.x < 16)
        GLOBAL_PlayerData.objData.pos.x = 32;
    if (GLOBAL_PlayerData.objData.pos.x > 224)
        GLOBAL_PlayerData.objData.pos.x = 200;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(GLOBAL_PlayerData.objData.pos.x);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(GLOBAL_PlayerData.objData.pos.y);
}

int main(void) {
    uint8_t level, sc, frame;
    unsigned long total_cyc, max_cyc, frame_cyc;
    #define FRAME_BUDGET 25780  // 29780 - ~4000 vblank

    printf("Lvl Scenario   Avg   Max   Over?\n");

    for (level = 1; level <= 31; level++) {
        for (sc = 0; sc < NUM_SCENARIOS; sc++) {
            setup_level_sim(level);

            GLOBAL_FrameCount = 10;
            total_cyc = 0;
            max_cyc = 0;

            // Warm up 2 frames
            run_frame(0);
            run_frame(0);

            for (frame = 0; frame < 30; frame++) {
                reset_clock();
                run_frame(scenarios[sc].inputs[frame]);
                frame_cyc = clock();
                total_cyc += frame_cyc;
                if (frame_cyc > max_cyc) max_cyc = frame_cyc;
            }

            unsigned long avg = total_cyc / 30;

            if (level < 10) putchar(' ');
            print_num(level);
            putchar(' ');
            printf("%-10s ", scenarios[sc].name);
            print_num(avg);
            printf("  ");
            print_num(max_cyc);
            if (max_cyc > FRAME_BUDGET) printf(" OVER");
            putchar('\n');
        }
    }

    printf("\nBudget: %d cyc/frame (29780 - 4000 vblank)\n", FRAME_BUDGET);
    return 0;
}
