// Physics verification test for 128x128 / 30fps conversion
// Build: mos-sim-clang -Os -o bench/test_128px bench/test_128px.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run:   mos-sim bench/test_128px

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

static int test_pass = 0;
static int test_fail = 0;

static void print_int(long v) {
    char buf[12]; int n = 0;
    if (v < 0) { putchar('-'); v = -v; }
    if (v == 0) { putchar('0'); return; }
    while(v) { buf[n++] = '0' + (v % 10); v /= 10; }
    while(n--) putchar(buf[n]);
}

static void check(const char *name, int condition) {
    if (condition) {
        test_pass++;
    } else {
        test_fail++;
        fputs("FAIL: ", stdout);
        fputs(name, stdout);
        putchar('\n');
    }
}

static void check_eq(const char *name, long actual, long expected) {
    if (actual == expected) {
        test_pass++;
    } else {
        test_fail++;
        fputs("FAIL: ", stdout);
        fputs(name, stdout);
        fputs(" got=", stdout);
        print_int(actual);
        fputs(" exp=", stdout);
        print_int(expected);
        putchar('\n');
    }
}

static void check_range(const char *name, long actual, long lo, long hi) {
    if (actual >= lo && actual <= hi) {
        test_pass++;
    } else {
        test_fail++;
        fputs("FAIL: ", stdout);
        fputs(name, stdout);
        fputs(" got=", stdout);
        print_int(actual);
        fputs(" range=[", stdout);
        print_int(lo);
        putchar(',');
        print_int(hi);
        putchar(']');
        putchar('\n');
    }
}

// Setup: 128px space, floor at tile row 14 (y >= 112 is solid)
static void setup(void) {
    uint16_t i;
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = 1;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    GLOBAL_ActiveLevel.playerSpawnX = 4;
    GLOBAL_ActiveLevel.playerSpawnY = 13;
    // Floor at tile rows 14-15 (y=112..127), solid
    for (i = 0; i < 256; i++) {
        uint8_t r = i >> 4; // row = index / 16
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = (r >= 14) ? 0x01 : 0;
        // NES: collision reset is handled by port_restoreCollisionFlags, no reset array
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
    GLOBAL_FrameCount = 10;
    GLOBAL_FreezeFrames = 0;
    GLOBAL_PausePlayerFrames = 0;
    GLOBAL_DoubleDashUnlocked = false;
}

// Place player on the ground in 128px coordinates
static void place_on_ground(int16_t x) {
    // Floor is at tile row 14 = y=112. Player hitbox bottom is y+7.
    // So player pos.y should be ~104 to have feet on floor (104+7=111, just above 112)
    // Actually with OBJ_isSolidAt checking (checkY+7), solid at y=112 means:
    //   checkY+7 >= 112 -> checkY >= 105 -> pos.y + offset_y >= 105
    // For onGround check: OBJ_isSolidAt(this, 0, 1) -> checkY = pos.y + 1
    //   isTileSolidAtPoint(checkX+1, checkY+1+7) = point at pos.y+8
    //   pos.y+8 must be in tile row 14 -> pos.y+8 >= 112 -> pos.y >= 104
    //   But pos.y+7 must NOT be in row 14 -> pos.y+7 < 112 -> pos.y < 105
    // So pos.y = 104 should be on ground
    GLOBAL_PlayerData.objData.pos.x = x;
    GLOBAL_PlayerData.objData.pos.y = 104;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(x);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(104);
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = 0;
    GLOBAL_PlayerData.graceTimer = 6;
    GLOBAL_PlayerData.dashCounter = 0;
}

// Clear static button edge-detection state by running a no-input frame
static void clear_button_state(void) {
    GLOBAL_InputState = 0;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);
}

// ---- Tests ----

static void test_spawn(void) {
    setup();
    // playerInit should place at spawnX*8, spawnY*8
    playerInit(&GLOBAL_PlayerData);
    check_eq("spawn_x", GLOBAL_PlayerData.objData.pos.x, 4 * 8);   // 32
    check_eq("spawn_y", GLOBAL_PlayerData.objData.pos.y, 13 * 8);   // 104
}

static void test_ground_detection(void) {
    setup();
    place_on_ground(32);
    GLOBAL_InputState = 0; // no input
    GLOBAL_FrameCount = 11;
    playerUpdate(&GLOBAL_PlayerData);
    // Player should stay at same Y (on ground, no gravity applied)
    check_eq("ground_y", GLOBAL_PlayerData.objData.pos.y, 104);
    check("ground_spd_y_zero", GLOBAL_PlayerData.spd.y == 0);
}

static void test_gravity_freefall(void) {
    setup();
    // Place player in mid-air
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 50;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(50);
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = 0;
    GLOBAL_PlayerData.graceTimer = 0;
    GLOBAL_PlayerData.dashCounter = 0;

    GLOBAL_InputState = 0;
    GLOBAL_FrameCount = 11;

    // Run several frames of freefall
    int16_t prevY = 50;
    uint8_t i;
    for (i = 0; i < 30; i++) {
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);
    }

    // After 30 frames, speed should be capped at maxfall (2.0 in fixed)
    // FP_MAXFALL = 0x00020000 = 2.0
    check("gravity_maxfall", GLOBAL_PlayerData.spd.y <= 0x00020000);
    check("gravity_fell", GLOBAL_PlayerData.objData.pos.y > 50);
}

static void test_maxrun(void) {
    setup();
    place_on_ground(32);
    GLOBAL_InputState = 0x01; // right

    // Run for many frames to reach max speed
    uint8_t i;
    for (i = 0; i < 60; i++) {
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);
    }

    // maxrun is 1.0 = 0x00010000
    // Speed should be at or near maxrun
    fixed_t absSpd = FIXED_ABS(GLOBAL_PlayerData.spd.x);
    check("maxrun_reached", absSpd >= 0x0000F000); // at least ~0.94
    check("maxrun_not_exceeded", absSpd <= 0x00010100); // not much over 1.0
}

static void test_jump_velocity(void) {
    setup();
    place_on_ground(32);
    clear_button_state(); // clear static btnJumpLastFrame
    // Press jump (B button)
    GLOBAL_InputState = PORT_INPUT_B_MASK;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    // Jump speed should be -2.0 = 0xFFFE0000 (INT_TO_FIXED(-2))
    check_eq("jump_spd_y", GLOBAL_PlayerData.spd.y, INT_TO_FIXED(-2));
}

static void test_jump_height(void) {
    setup();
    place_on_ground(32);
    clear_button_state(); // clear static btnJumpLastFrame

    // Jump
    GLOBAL_InputState = PORT_INPUT_B_MASK;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);
    // Release jump
    GLOBAL_InputState = 0;

    int16_t startY = GLOBAL_PlayerData.objData.pos.y;
    int16_t minY = startY;

    // Let the jump arc play out
    uint8_t i;
    for (i = 0; i < 40; i++) {
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);
        if (GLOBAL_PlayerData.objData.pos.y < minY) {
            minY = GLOBAL_PlayerData.objData.pos.y;
        }
    }

    // Original ccleste: jump spd=-2, gravity=0.21, peak ~10-12px above start
    int16_t jumpHeight = startY - minY;
    check_range("jump_height_px", jumpHeight, 8, 15);
}

static void test_dash_speed(void) {
    setup();
    place_on_ground(32);
    clear_button_state(); // clear static btnDashLastFrame

    // Dash right
    GLOBAL_InputState = PORT_INPUT_Y_MASK | PORT_INPUT_RIGHT_MASK;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    // Dash sets spd.x = 5.0, then decel (0.15) reduces it to ~4.85 same frame
    // This matches original ccleste behavior (clamp runs after dash init)
    fixed_t absSpd = FIXED_ABS(GLOBAL_PlayerData.spd.x);
    check_range("dash_spd_x", absSpd, FLOAT_TO_FIXED(4.7f), FLOAT_TO_FIXED(5.0f));
    check_eq("dash_spd_y", GLOBAL_PlayerData.spd.y, 0);
}

static void test_dash_target(void) {
    setup();
    place_on_ground(32);
    clear_button_state(); // clear static btnDashLastFrame

    // Dash right
    GLOBAL_InputState = PORT_INPUT_Y_MASK | PORT_INPUT_RIGHT_MASK;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    // Dash target should be 2.0
    check_eq("dash_target_x", GLOBAL_PlayerData.dashTarget.x, INT_TO_FIXED(2));
    check_eq("dash_target_y", GLOBAL_PlayerData.dashTarget.y, 0);
}

static void test_wall_detection(void) {
    setup();
    // Put a wall at tile column 6 (x=48..55 solid)
    uint16_t i;
    for (i = 0; i < 16; i++) {
        GLOBAL_ActiveLevel.collisionFlagsArr[i * 16 + 6] |= 0x01;
    }

    // Place player next to wall
    GLOBAL_PlayerData.objData.pos.x = 41; // hitbox right edge at 41+6=47, wall at 48
    GLOBAL_PlayerData.objData.pos.y = 50;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(41);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(50);
    GLOBAL_PlayerData.spd.x = INT_TO_FIXED(1);
    GLOBAL_PlayerData.spd.y = 0;
    GLOBAL_PlayerData.graceTimer = 0;
    GLOBAL_PlayerData.dashCounter = 0;

    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;
    GLOBAL_FrameCount = 11;
    playerUpdate(&GLOBAL_PlayerData);

    // Player should not pass through the wall
    check("wall_block", GLOBAL_PlayerData.objData.pos.x <= 42);
}

static void test_spike_floor(void) {
    setup();
    // Add floor spike at tile (4, 13) - just above the floor
    GLOBAL_ActiveLevel.collisionFlagsArr[13 * 16 + 4] |= 0x04; // floor spike flag

    // Place player above the spike, moving down
    GLOBAL_PlayerData.objData.pos.x = 32; // tile col 4
    GLOBAL_PlayerData.objData.pos.y = 98;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(98);
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = INT_TO_FIXED(2);
    GLOBAL_PlayerData.graceTimer = 0;
    GLOBAL_PlayerData.dashCounter = 0;

    // Simulate falling onto spike
    GLOBAL_InputState = 0;
    uint8_t i;
    for (i = 0; i < 10; i++) {
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);
    }

    // Player should have died and been reset to spawn
    check_eq("spike_death_x", GLOBAL_PlayerData.objData.pos.x, 4 * 8);
}

static void test_death_fall(void) {
    setup();
    // Place player near bottom
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 126;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(126);
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = INT_TO_FIXED(3);
    GLOBAL_PlayerData.graceTimer = 0;
    GLOBAL_PlayerData.dashCounter = 0;

    GLOBAL_InputState = 0;
    GLOBAL_FrameCount = 11;
    playerUpdate(&GLOBAL_PlayerData);

    // Should die at y > 128 and reset to spawn
    check_eq("death_fall_x", GLOBAL_PlayerData.objData.pos.x, 4 * 8);
}

static void test_room_transition(void) {
    setup();
    GLOBAL_ActiveLevel.currentRoomID = 1;

    // Place player near top
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = -3;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(-3);
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = INT_TO_FIXED(-2);
    GLOBAL_PlayerData.graceTimer = 0;
    GLOBAL_PlayerData.dashCounter = 0;

    GLOBAL_InputState = 0;
    GLOBAL_FrameCount = 11;
    playerUpdate(&GLOBAL_PlayerData);

    // Should have triggered room transition (y < -4)
    check_eq("room_transition", GLOBAL_ActiveLevel.currentRoomID, 2);
}

static void test_position_clamping(void) {
    setup();
    // Place player at far right
    GLOBAL_PlayerData.objData.pos.x = 120;
    GLOBAL_PlayerData.objData.pos.y = 50;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(120);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(50);
    GLOBAL_PlayerData.spd.x = INT_TO_FIXED(5);
    GLOBAL_PlayerData.spd.y = 0;
    GLOBAL_PlayerData.graceTimer = 0;
    GLOBAL_PlayerData.dashCounter = 0;

    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;
    GLOBAL_FrameCount = 11;
    playerUpdate(&GLOBAL_PlayerData);

    // Should be clamped to max X = 121
    check("clamp_x_max", GLOBAL_PlayerData.objData.pos.x <= 121);
}

static void test_spring_velocity(void) {
    setup();
    // Spring at tile (4,12) = pixel (32,96). Need solid below at tile row 13.
    GLOBAL_ActiveLevel.collisionFlagsArr[13 * 16 + 4] |= 0x01; // solid below spring

    // Create a spring at (32, 96)
    initObject(OBJ_SPRING, 32, 96);

    // Place player overlapping the spring, moving down
    // Touch check: playerX > thisX-4(28) && playerX < thisX+12(44)
    //              playerY > thisY-3(93) && playerY < thisY+10(106)
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 94; // 94 > 93 ✓, 94 < 106 ✓
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(94);
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = INT_TO_FIXED(1); // moving down

    GLOBAL_InputState = 0;
    GLOBAL_FrameCount = 11;

    // Run spring update to trigger
    updateAllObjects();

    // Spring should set spd.y = -3 (original value)
    check_eq("spring_velocity", GLOBAL_PlayerData.spd.y, INT_TO_FIXED(-3));
}

static void test_freeze_frames(void) {
    setup();
    place_on_ground(32);
    clear_button_state(); // clear static btnDashLastFrame

    // Dash to trigger freeze frames
    GLOBAL_InputState = PORT_INPUT_Y_MASK | PORT_INPUT_RIGHT_MASK;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    // GLOBAL_FreezeFrames should be 2 (original value)
    check_eq("freeze_frames", GLOBAL_FreezeFrames, 2);
}

int main(void) {
    fputs("=== 128px/30fps Physics Tests ===\n", stdout);

    test_spawn();
    test_ground_detection();
    test_gravity_freefall();
    test_maxrun();
    test_jump_velocity();
    test_jump_height();
    test_dash_speed();
    test_dash_target();
    test_wall_detection();
    test_spike_floor();
    test_death_fall();
    test_room_transition();
    test_position_clamping();
    test_spring_velocity();
    test_freeze_frames();

    putchar('\n');
    fputs("Pass: ", stdout);
    print_int(test_pass);
    fputs("  Fail: ", stdout);
    print_int(test_fail);
    putchar('\n');

    if (test_fail > 0) {
        fputs("SOME TESTS FAILED\n", stdout);
        return 1;
    }
    fputs("ALL TESTS PASSED\n", stdout);
    return 0;
}
