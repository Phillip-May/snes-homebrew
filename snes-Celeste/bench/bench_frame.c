// Benchmark: measure cycle cost of one game frame's worth of work
// Compile with: mos-sim-clang -Os -o bench_frame bench_frame.c ../src/libfixmath/fix16.c -I../src -I../src/port -I../src/libfixmath
// Run with: mos-sim --cycles bench_frame

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "fixedPointSNES.h"

// Minimal port stubs - we're not building the full NES port
#define PORT_FUNC_BANK6
#define PORT_FUNC_BANK4
#define PORT_FUNC_BANK3
#define PORT_DATA_BANK6
#define PORT_DATA_BANK4
#define PORT_DATA_BANK3
#define GLOBAL_OBJ_LIST_SIZE 29

// Stub the port functions
static inline void port_prg_bank_switch(uint8_t b) { (void)b; }

// --- Inline the key struct definitions from port.h ---
typedef struct { int16_t x; int16_t y; } VEC_I;
typedef struct { fixed_t x; fixed_t y; } VEC_F;

enum ePlayerSprite { PLAYER_SPRITE_IDLE = 0, PLAYER_SPRITE_WALK_1, PLAYER_SPRITE_WALK_2, PLAYER_SPRITE_UP, PLAYER_SPRITE_DOWN, PLAYER_SPRITE_WALL };

typedef struct sOBJ_DATA {
    VEC_I pos;
    uint8_t flags;
    uint8_t oamTile;
    uint8_t oamProps;
    uint8_t extraSpriteBase;
    uint8_t extraSpriteCount;
    enum eOBJType {
        OBJ_UNUSED = 0, OBJ_PLAYER, OBJ_SMOKE, OBJ_DOUBLE_JUMP_ORB,
        OBJ_KEY = 8, OBJ_PLATMOV_R = 11, OBJ_PLATMOV_L = 12,
        OBJ_SPRING = 18, OBJ_CHEST = 20, OBJ_BALLOON = 22,
        OBJ_COLLAPSE_TILE = 23, OBJ_STRAWBERRY = 26, OBJ_FLYING_BERRY = 28,
        OBJ_DECO_TREE = 60, OBJ_DECO_FLOWER = 62, OBJ_BREAKABLE_WALL = 64,
        OBJ_MONUMENT = 70, OBJ_BIG_CHEST = 96, OBJ_BIG_CHEST_2 = 97
    } eType;
    union sOBJData {
        struct { uint8_t frameCount; uint8_t smokeSpriteState; uint8_t speedX; uint8_t speedY; bool flipX; bool flipY; } smoke;
        struct { uint8_t startY; uint16_t frameCount; uint8_t isCollected; uint8_t bgTextX; uint8_t bgTextY; } strawberry;
        struct { bool isDisabled; uint8_t frameCount; int8_t linkedCollapseTileIndex; } spring;
        struct { uint8_t state; uint8_t frameCount; int8_t linkedSpringIndex; } collapseTile;
        struct { uint8_t state; uint8_t frameCount; uint16_t yTableIndex; uint8_t hideFrameCount; int8_t spriteYOffset; uint8_t stringTile; } balloon;
        struct { uint8_t acc; uint8_t hitboxIndex; bool isMovingLeft; } platMov;
        struct { uint8_t linkedChestIndex; bool isFlipped; uint8_t spriteValue; uint8_t frameCount; uint8_t state; } key;
        struct { bool keyIsCollected; uint8_t frameCount; uint8_t state; } chest;
        struct { uint8_t state; } monument;
        struct { uint8_t state; uint8_t frameCount; } bigChest;
        struct { int8_t frameCount; int16_t speedY; int16_t accelAccumulator; } doubleJumpOrb;
    } data;
} OBJ_DATA;

#define OBJ_FLAG_DIRTY 0x80u

struct sPlayerData {
    OBJ_DATA objData;
    VEC_F posF;
    int8_t movingPlatformIndex;
    VEC_F spd;
    enum ePlayerSprite eSriteState;
    uint8_t graceTimer;
    bool doubleDashUnlocked;
    VEC_F dashTarget;
    VEC_F dashAccel;
    uint8_t dashesLeft;
    int8_t dashCounter;
    bool isFliped;
};

struct sActiveLevelData {
    uint16_t currentRoomID;
    uint16_t roomSizeX;
    uint16_t roomSizeY;
    bool textChanged;
    bool swapCloudPal;
    bool swapActivePalette;
    bool textFlashActive;
    bool textScrollActive;
    uint8_t collisionFlagsReset[256];
    uint8_t collisionFlagsArr[256];
    uint8_t movingPlatformCount;
    uint8_t movingPlatformDir[16];
    uint8_t movingPlatformHitboxes[16 * 4];
    uint8_t shakeFrames;
    uint8_t textScrollOffsetX;
    uint8_t textScrollOffsetY;
    uint8_t scrollPointY;
    uint8_t playerSpawnX;
    uint8_t playerSpawnY;
    uint8_t objectCount;
    uint8_t objectData[64];
    bool isLevelLoadedVRAM;
};

// Globals
uint8_t GLOBAL_InputState = 0;
OBJ_DATA GLOBAL_OBJList[GLOBAL_OBJ_LIST_SIZE] = {0};
uint16_t GLOBAL_FrameCount = 0;
uint8_t GLOBAL_FreezeFrames = 0;
uint8_t GLOBAL_PausePlayerFrames = 0;
bool GLOBAL_DoubleDashUnlocked = false;
struct sActiveLevelData GLOBAL_ActiveLevel;
struct sPlayerData GLOBAL_PlayerData;

// ---- Benchmarked functions (inlined from mainBankZero.c) ----

static int16_t sign(int16_t v) { return v > 0 ? 1 : (v < 0 ? -1 : 0); }

fixed_t approachFixed(fixed_t current, fixed_t target, fixed_t amount) {
    fixed_t diff = FIXED_SUB(target, current);
    if (diff > amount) return FIXED_ADD(current, amount);
    if (diff < -amount) return FIXED_SUB(current, amount);
    return target;
}

// Benchmark: approachFixed (called ~4x per frame in playerUpdate)
static void bench_approachFixed(void) {
    fixed_t val = FLOAT_TO_FIXED(1.5f);
    fixed_t target = FLOAT_TO_FIXED(4.0f);
    fixed_t step = FLOAT_TO_FIXED(0.21f);
    unsigned long start, end;

    reset_clock();
    // Simulate ~4 calls per frame
    val = approachFixed(val, target, step);
    val = approachFixed(val, INT_TO_FIXED(0), step);
    val = approachFixed(FLOAT_TO_FIXED(-1.0f), INT_TO_FIXED(0), FLOAT_TO_FIXED(0.5f));
    val = approachFixed(val, FLOAT_TO_FIXED(2.0f), step);
    end = clock();
    printf("approachFixed x4: %lu cycles\n", end);
}

// Benchmark: FIXED_MUL vs shift-based multiply
static void bench_multiply(void) {
    fixed_t val = FLOAT_TO_FIXED(2.5f);
    fixed_t result;
    unsigned long c;

    // FIXED_MUL (full 32-bit multiply)
    reset_clock();
    result = FIXED_MUL(val, FLOAT_TO_FIXED(0.75f));
    c = clock();
    printf("FIXED_MUL(x, 0.75): %lu cycles (result=%ld)\n", c, (long)result);

    // Shift-based: x - x/4
    reset_clock();
    result = val - (val >> 2);
    c = clock();
    printf("x - x>>2  (0.75) : %lu cycles (result=%ld)\n", c, (long)result);

    // FIXED_MUL (full 32-bit multiply) x 0.5
    reset_clock();
    result = FIXED_MUL(val, FLOAT_TO_FIXED(0.5f));
    c = clock();
    printf("FIXED_MUL(x, 0.5): %lu cycles (result=%ld)\n", c, (long)result);

    // Shift-based: x >> 1
    reset_clock();
    result = val >> 1;
    c = clock();
    printf("x >> 1    (0.5) : %lu cycles (result=%ld)\n", c, (long)result);

    // FIXED_MUL with INT_TO_FIXED(-1) * constant (dash pattern)
    reset_clock();
    result = FIXED_MUL(INT_TO_FIXED(-1), FLOAT_TO_FIXED(5.0f * 2 * 0.707f));
    c = clock();
    printf("FIXED_MUL(-1, DASH_HALF): %lu cycles\n", c);

    // Direct constant assignment (replacement)
    reset_clock();
    result = -FLOAT_TO_FIXED(5.0f * 2 * 0.707f);
    c = clock();
    printf("-DASH_HALF constant:      %lu cycles\n", c);
}

// Benchmark: isTileSolidAtPoint (collision check - called ~8x per frame)
static bool isTileSolidAtPoint(int16_t x, int16_t y) {
    int tileX = (int)(x / 16);
    int tileY = (int)(y / 16);
    if (tileX < 0 || tileX >= 16 || tileY < 0 || tileY >= 16) return false;
    int collisionIndex = tileY * 16 + tileX;
    return GLOBAL_ActiveLevel.collisionFlagsArr[collisionIndex] & 0x01;
}

static void bench_collision(void) {
    unsigned long c;
    // Fill some collision data
    for (int i = 0; i < 256; i++) GLOBAL_ActiveLevel.collisionFlagsArr[i] = (i >= 208) ? 0x01 : 0x00;

    reset_clock();
    // Simulate OBJ_isSolidAt with 4 corner checks (typical per-axis)
    volatile bool r = false;
    r |= isTileSolidAtPoint(32, 208 + 8);
    r |= isTileSolidAtPoint(32 + 14, 208 + 8);
    r |= isTileSolidAtPoint(32, 208 + 16);
    r |= isTileSolidAtPoint(32 + 14, 208 + 16);
    r |= isTileSolidAtPoint(32, 209 + 8);
    r |= isTileSolidAtPoint(32 + 14, 209 + 8);
    r |= isTileSolidAtPoint(32, 209 + 16);
    r |= isTileSolidAtPoint(32 + 14, 209 + 16);
    c = clock();
    printf("8x isTileSolidAtPoint: %lu cycles\n", c);
}

// Benchmark: full approachFixed-based movement (the hot path in playerUpdate)
static void bench_movement(void) {
    unsigned long c;
    fixed_t maxrun = FLOAT_TO_FIXED(1.0f * 2);
    fixed_t accel = FLOAT_TO_FIXED(0.6f);
    fixed_t decel = FLOAT_TO_FIXED(0.15f);
    fixed_t gravity = FLOAT_TO_FIXED(0.21f);
    fixed_t maxfall = FLOAT_TO_FIXED(2.0f * 2);
    int8_t inputX = 1;
    fixed_t spdX = FLOAT_TO_FIXED(0.8f);
    fixed_t spdY = FLOAT_TO_FIXED(0.5f);

    reset_clock();

    // Horizontal movement (from playerUpdate)
    spdX = approachFixed(spdX, FIXED_MUL(INT_TO_FIXED(inputX), maxrun), accel);

    // Gravity with 0.5 multiplier via FIXED_MUL
    if (FIXED_ABS(spdY) <= FLOAT_TO_FIXED(0.30f)) {
        gravity = FIXED_MUL(gravity, FLOAT_TO_FIXED(0.5f));
    }
    spdY = approachFixed(spdY, maxfall, gravity);

    c = clock();
    printf("movement (FIXED_MUL 0.5): %lu cycles\n", c);

    // Same with shift
    spdX = FLOAT_TO_FIXED(0.8f);
    spdY = FLOAT_TO_FIXED(0.5f);
    gravity = FLOAT_TO_FIXED(0.21f);

    reset_clock();
    spdX = approachFixed(spdX, FIXED_MUL(INT_TO_FIXED(inputX), maxrun), accel);
    if (FIXED_ABS(spdY) <= FLOAT_TO_FIXED(0.30f)) {
        gravity = gravity >> 1; // shift instead of FIXED_MUL
    }
    spdY = approachFixed(spdY, maxfall, gravity);

    c = clock();
    printf("movement (shift   0.5): %lu cycles\n", c);
}

int main(void) {
    printf("=== NES Celeste Cycle Benchmarks ===\n\n");

    bench_multiply();
    printf("\n");
    bench_approachFixed();
    printf("\n");
    bench_collision();
    printf("\n");
    bench_movement();
    printf("\n");

    printf("=== Done ===\n");
    return 0;
}
