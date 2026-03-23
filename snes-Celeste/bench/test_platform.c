// Test: player standing on moving platform
// Build: mos-sim-clang -Os -o bench/test_platform bench/test_platform.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run:   mos-sim bench/test_platform

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

static int pass_count = 0, fail_count = 0;

static void check(const char *name, int cond) {
    if (cond) { pass_count++; }
    else { fail_count++; fputs("FAIL: ", stdout); fputs(name, stdout); putchar('\n'); }
}
static void check_eq(const char *name, long actual, long expected) {
    if (actual == expected) { pass_count++; }
    else { fail_count++; fputs("FAIL: ", stdout); fputs(name, stdout);
           fputs(" got=", stdout); print_int(actual);
           fputs(" exp=", stdout); print_int(expected); putchar('\n'); }
}
static void check_range(const char *name, long actual, long lo, long hi) {
    if (actual >= lo && actual <= hi) { pass_count++; }
    else { fail_count++; fputs("FAIL: ", stdout); fputs(name, stdout);
           fputs(" got=", stdout); print_int(actual);
           fputs(" range=[", stdout); print_int(lo);
           putchar(','); print_int(hi); putchar(']'); putchar('\n'); }
}

static void setup(void) {
    uint16_t i;
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = 1;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    GLOBAL_ActiveLevel.playerSpawnX = 4;
    GLOBAL_ActiveLevel.playerSpawnY = 12;
    // No floor tiles — only the platform provides ground
    for (i = 0; i < 256; i++) {
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = 0;
    }
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(OBJ_DATA));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = 0xFF;
    }
    memset(&GLOBAL_PlayerData, 0, sizeof(GLOBAL_PlayerData));
    GLOBAL_PlayerData.objData.eType = OBJ_PLAYER;
    GLOBAL_PlayerData.dashesLeft = 1;
    GLOBAL_PlayerData.movingPlatformIndex = -1;
    GLOBAL_PlayerData.dashCounter = 0;
    GLOBAL_FrameCount = 10;
    GLOBAL_FreezeFrames = 0;
    GLOBAL_PausePlayerFrames = 0;
    GLOBAL_DoubleDashUnlocked = false;
}

int main(void) {
    uint8_t i;

    fputs("=== Moving Platform Tests ===\n\n", stdout);

    // ---- Test 1: Platform hitbox setup ----
    fputs("== Platform hitbox ==\n", stdout);
    setup();
    // Create a right-moving platform at (40, 80)
    initObject(OBJ_PLATMOV_R, 40, 80);
    // platMovInit adjusts: pos.y -= 1 → 79, and for PLATMOV_R (actually PLATMOV_L gives isMovingLeft=true)
    // Actually OBJ_PLATMOV_L → isMovingLeft=true, OBJ_PLATMOV_R doesn't exist?
    // Let me check: PLATMOV_L sets isMovingLeft=false (RIGHT), PLATMOV_R doesn't exist
    // Wait, looking at the init: if eType == OBJ_PLATMOV_L → isMovingLeft=false (moves RIGHT)
    // else → isMovingLeft=true (moves LEFT), pos.x -= 8

    // Find the platform object
    uint8_t platIdx = 0;
    for (i = 1; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_PLATMOV_L || GLOBAL_OBJList[i].eType == OBJ_PLATMOV_R) {
            platIdx = i;
            break;
        }
    }
    check("plat_created", platIdx > 0);
    if (platIdx > 0) {
        OBJ_DATA *plat = &GLOBAL_OBJList[platIdx];
        fputs("  plat pos: (", stdout); print_int(plat->pos.x);
        fputs(", ", stdout); print_int(plat->pos.y); fputs(")\n", stdout);

        // Hitbox should be set
        uint8_t hi = plat->data.platMov.hitboxIndex;
        fputs("  hitbox: [", stdout);
        print_int(GLOBAL_ActiveLevel.movingPlatformHitboxes[hi]);
        fputs(", ", stdout);
        print_int(GLOBAL_ActiveLevel.movingPlatformHitboxes[hi+1]);
        fputs(", ", stdout);
        print_int(GLOBAL_ActiveLevel.movingPlatformHitboxes[hi+2]);
        fputs(", ", stdout);
        print_int(GLOBAL_ActiveLevel.movingPlatformHitboxes[hi+3]);
        fputs("]\n", stdout);

        // Width should be reasonable (16-20px for a 2-tile platform in 128px space)
        int16_t width = GLOBAL_ActiveLevel.movingPlatformHitboxes[hi+2] - GLOBAL_ActiveLevel.movingPlatformHitboxes[hi];
        check_range("plat_width", width, 14, 24);
    }

    // ---- Test 2: Player detection on platform ----
    fputs("\n== Player on platform ==\n", stdout);
    setup();
    // Create right-moving platform at (40, 80)
    initObject(OBJ_PLATMOV_L, 40, 80);  // PLATMOV_L = moves right
    platIdx = 0;
    for (i = 1; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_PLATMOV_L) { platIdx = i; break; }
    }

    if (platIdx > 0) {
        OBJ_DATA *plat = &GLOBAL_OBJList[platIdx];
        // Run one update to set hitboxes
        updateAllObjects();

        uint8_t hi = plat->data.platMov.hitboxIndex;
        int16_t platTopY = GLOBAL_ActiveLevel.movingPlatformHitboxes[hi+1];
        int16_t platLeftX = GLOBAL_ActiveLevel.movingPlatformHitboxes[hi];
        int16_t platRightX = GLOBAL_ActiveLevel.movingPlatformHitboxes[hi+2];

        fputs("  plat top Y: ", stdout); print_int(platTopY);
        fputs("  plat X range: [", stdout); print_int(platLeftX);
        fputs(", ", stdout); print_int(platRightX); fputs("]\n", stdout);

        // Place player on top of platform
        // Player's ground check: OBJ_isSolidAt(this, 0, 1) → checkY = pos.y + 1
        // Platform detected when checkX in [platLeft, platRight] and checkY in [platTopY, platTopY]
        // So player pos.y + 1 == platTopY → pos.y = platTopY - 1
        int16_t playerY = platTopY - 1;
        int16_t playerX = (platLeftX + platRightX) / 2; // center
        GLOBAL_PlayerData.objData.pos.x = playerX;
        GLOBAL_PlayerData.objData.pos.y = playerY;
        GLOBAL_PlayerData.posF.x = INT_TO_FIXED(playerX);
        GLOBAL_PlayerData.posF.y = INT_TO_FIXED(playerY);
        GLOBAL_PlayerData.spd.x = 0;
        GLOBAL_PlayerData.spd.y = 0;
        GLOBAL_PlayerData.graceTimer = 6;

        fputs("  player pos: (", stdout); print_int(playerX);
        fputs(", ", stdout); print_int(playerY); fputs(")\n", stdout);

        // Clear buttons and run
        GLOBAL_InputState = 0;
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        fputs("  after update: player (", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.x);
        fputs(", ", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.y);
        fputs(") platIdx=", stdout);
        print_int(GLOBAL_PlayerData.movingPlatformIndex);
        fputs(" spd.x=", stdout);
        print_int(GLOBAL_PlayerData.spd.x);
        putchar('\n');

        // Player should be detected on platform
        check("plat_detected", GLOBAL_PlayerData.movingPlatformIndex >= 0);

        // Player should have carry speed (0.65 for right-moving)
        if (GLOBAL_PlayerData.movingPlatformIndex >= 0) {
            fixed_t expectedCarry = FLOAT_TO_FIXED(0.65f);
            check_eq("carry_speed", GLOBAL_PlayerData.spd.x, expectedCarry);
        }

        // Player Y should stay the same (not fall through)
        check_eq("plat_stay_y", GLOBAL_PlayerData.objData.pos.y, playerY);
    }

    // ---- Test 3: Ride platform for multiple frames ----
    fputs("\n== Ride platform ==\n", stdout);
    if (platIdx > 0) {
        OBJ_DATA *plat = &GLOBAL_OBJList[platIdx];
        int16_t startPlayerX = GLOBAL_PlayerData.objData.pos.x;
        int16_t startPlatX = plat->pos.x;
        int16_t startPlayerY = GLOBAL_PlayerData.objData.pos.y;

        GLOBAL_InputState = 0;
        fputs("Fr | playerX | playerY | platX | onPlat | spd.x\n", stdout);

        for (i = 0; i < 15; i++) {
            GLOBAL_FrameCount++;
            updateAllObjects();
            playerUpdate(&GLOBAL_PlayerData);

            if (i < 10) fputs(" ", stdout);
            print_int(i);
            fputs(" | ", stdout); print_int(GLOBAL_PlayerData.objData.pos.x);
            fputs(" | ", stdout); print_int(GLOBAL_PlayerData.objData.pos.y);
            fputs(" | ", stdout); print_int(plat->pos.x);
            fputs(" | ", stdout); print_int(GLOBAL_PlayerData.movingPlatformIndex);
            fputs(" | ", stdout); print_int(GLOBAL_PlayerData.spd.x);
            putchar('\n');
        }

        // Player should have moved right with the platform
        check("plat_moved_right", GLOBAL_PlayerData.objData.pos.x > startPlayerX);
        // Player Y should not have changed (still on platform)
        check_eq("plat_ride_y", GLOBAL_PlayerData.objData.pos.y, startPlayerY);
        // Platform should have moved right
        check("plat_itself_moved", plat->pos.x > startPlatX);
    }

    // ---- Test 4: Jump off platform ----
    fputs("\n== Jump off platform ==\n", stdout);
    if (platIdx > 0) {
        // Clear button state
        GLOBAL_InputState = 0;
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        // Jump
        GLOBAL_InputState = PORT_INPUT_B_MASK;
        GLOBAL_FrameCount++;
        updateAllObjects();
        playerUpdate(&GLOBAL_PlayerData);

        fputs("  after jump: spd.y=", stdout);
        print_int(GLOBAL_PlayerData.spd.y);
        fputs(" platIdx=", stdout);
        print_int(GLOBAL_PlayerData.movingPlatformIndex);
        putchar('\n');

        // Should have jumped (negative Y speed)
        check("plat_jump_spd", GLOBAL_PlayerData.spd.y < 0);
    }

    fputs("\n--- Results ---\n", stdout);
    fputs("Pass: ", stdout); print_int(pass_count);
    fputs("  Fail: ", stdout); print_int(fail_count);
    putchar('\n');
    if (fail_count > 0) fputs("ISSUES FOUND\n", stdout);
    else fputs("ALL PASS\n", stdout);
    return fail_count > 0 ? 1 : 0;
}
