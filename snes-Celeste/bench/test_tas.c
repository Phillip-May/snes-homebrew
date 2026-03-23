// TAS validation test for NES port physics
// Build: mos-sim-clang -Os -o bench/test_tas bench/test_tas.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run:   mos-sim bench/test_tas

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

// Include tilemap headers for collision/object/spawn data
#include "tilemap_level1.h"
#include "tilemap_level3.h"

// ---- Utility ----

static void print_int(long v) {
    char buf[12]; int n = 0;
    if (v < 0) { putchar('-'); v = -v; }
    if (v == 0) { putchar('0'); return; }
    while(v) { buf[n++] = '0' + (v % 10); v /= 10; }
    while(n--) putchar(buf[n]);
}

// ---- TAS button mapping ----
// PICO-8 TAS: bit0=left, bit1=right, bit2=up, bit3=down, bit4=jump(O), bit5=dash(X)
// NES port:   PORT_INPUT_LEFT_MASK=0x02, RIGHT=0x01, UP=0x08, DOWN=0x04, B(jump)=0x80, Y(dash)=0x40

static uint8_t tas_to_nes_input(uint8_t tas) {
    uint8_t nes = 0;
    if (tas & 0x01) nes |= PORT_INPUT_LEFT_MASK;   // bit0 -> left
    if (tas & 0x02) nes |= PORT_INPUT_RIGHT_MASK;  // bit1 -> right
    if (tas & 0x04) nes |= PORT_INPUT_UP_MASK;      // bit2 -> up
    if (tas & 0x08) nes |= PORT_INPUT_DOWN_MASK;    // bit3 -> down
    if (tas & 0x10) nes |= PORT_INPUT_B_MASK;       // bit4 -> jump (B)
    if (tas & 0x20) nes |= PORT_INPUT_Y_MASK;       // bit5 -> dash (Y)
    return nes;
}

// ---- Level setup ----

static void setup_level(
    const unsigned char *collision, uint16_t collision_count,
    const unsigned char *objects, uint8_t object_count,
    const unsigned char *spawn,
    uint16_t room_id
) {
    uint16_t i;

    // Clear everything
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = room_id;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    GLOBAL_ActiveLevel.playerSpawnX = spawn[0];
    GLOBAL_ActiveLevel.playerSpawnY = spawn[1];

    // Load collision data
    for (i = 0; i < 256 && i < collision_count; i++) {
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = collision[i];
    }

    // Store object data for LoadRoomData pattern
    GLOBAL_ActiveLevel.objectCount = object_count;
    for (i = 0; i < object_count * 3u && i < sizeof(GLOBAL_ActiveLevel.objectData); i++) {
        GLOBAL_ActiveLevel.objectData[i] = objects[i];
    }

    // Clear object list
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(OBJ_DATA));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = 0xFF;
    }

    // Spawn objects from object data (type, tileX, tileY triples)
    for (i = 0; i < object_count; i++) {
        uint8_t type  = objects[i * 3 + 0];
        uint8_t tileX = objects[i * 3 + 1];
        uint8_t tileY = objects[i * 3 + 2];
        initObject((enum eOBJType)type, (int16_t)(tileX * 8), (int16_t)(tileY * 8));
    }

    // Clear player
    memset(&GLOBAL_PlayerData, 0, sizeof(GLOBAL_PlayerData));
    GLOBAL_PlayerData.objData.eType = OBJ_PLAYER;
    GLOBAL_PlayerData.dashesLeft = 1;
    GLOBAL_PlayerData.movingPlatformIndex = -1;
    GLOBAL_FrameCount = 10;
    GLOBAL_FreezeFrames = 0;
    GLOBAL_PausePlayerFrames = 0;
    GLOBAL_DoubleDashUnlocked = false;

    // Initialize player at spawn point
    playerInit(&GLOBAL_PlayerData);
}

// ---- TAS runner ----
// Returns: 1 = room transition (success), 0 = ran out of frames, -1 = death (respawned)

static int run_tas(
    const uint8_t *inputs, uint16_t input_count,
    const unsigned char *collision, uint16_t collision_count,
    const unsigned char *objects, uint8_t object_count,
    const unsigned char *spawn,
    uint16_t room_id
) {
    uint16_t frame;
    uint16_t start_room;
    int16_t spawn_x, spawn_y;

    setup_level(collision, collision_count, objects, object_count, spawn, room_id);

    start_room = GLOBAL_ActiveLevel.currentRoomID;
    spawn_x = (int16_t)(spawn[0] * 8);
    spawn_y = (int16_t)(spawn[1] * 8);

    // Run one idle frame to clear button edge-detection state
    GLOBAL_InputState = 0;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    for (frame = 0; frame < input_count; frame++) {
        GLOBAL_InputState = tas_to_nes_input(inputs[frame]);
        GLOBAL_FrameCount++;

        // Handle freeze frames like the game loop does
        if (GLOBAL_FreezeFrames > 0) {
            GLOBAL_FreezeFrames--;
            // Still count the frame but skip physics
            continue;
        }

        // Update objects (springs, balloons, etc.)
        updateAllObjects();

        // Update player
        playerUpdate(&GLOBAL_PlayerData);

        // Position trace (only for small input counts, i.e., TAS debugging)
        if (input_count < 120) {
            if (frame < 10) fputs(" ", stdout);
            print_int(frame);
            fputs(": (", stdout); print_int(GLOBAL_PlayerData.objData.pos.x);
            putchar(','); print_int(GLOBAL_PlayerData.objData.pos.y);
            fputs(") spd=(", stdout); print_int(GLOBAL_PlayerData.spd.x);
            putchar(','); print_int(GLOBAL_PlayerData.spd.y);
            fputs(") in=", stdout); print_int(inputs[frame]);
            putchar('\n');
        }

        // Check for room transition (currentRoomID incremented by LoadNextRoom)
        if (GLOBAL_ActiveLevel.currentRoomID != start_room) {
            fputs("  Room transition at frame ", stdout);
            print_int(frame);
            fputs("  pos=(", stdout);
            print_int(GLOBAL_PlayerData.objData.pos.x);
            putchar(',');
            print_int(GLOBAL_PlayerData.objData.pos.y);
            fputs(")\n", stdout);
            return 1;
        }

        // Check for death (player reset to spawn)
        // After death, playerInit resets position to spawn
        if (frame > 5 &&
            GLOBAL_PlayerData.objData.pos.x == spawn_x &&
            GLOBAL_PlayerData.objData.pos.y == spawn_y &&
            GLOBAL_PlayerData.spd.x == 0 &&
            GLOBAL_PlayerData.spd.y == 0) {
            fputs("  Death detected at frame ", stdout);
            print_int(frame);
            putchar('\n');
            return -1;
        }
    }

    fputs("  Ran out of inputs at frame ", stdout);
    print_int(input_count);
    fputs("  final pos=(", stdout);
    print_int(GLOBAL_PlayerData.objData.pos.x);
    putchar(',');
    print_int(GLOBAL_PlayerData.objData.pos.y);
    fputs(") spd=(", stdout);
    print_int(GLOBAL_PlayerData.spd.x);
    putchar(',');
    print_int(GLOBAL_PlayerData.spd.y);
    fputs(")\n", stdout);
    return 0;
}

// ---- Hardcoded TAS inputs ----
// Copied from tas_clean/*.tas files (comma-separated PICO-8 button values)

// TAS1.tas - Level 1
static const uint8_t tas1_inputs[] = {
    18,2,2,2,2,2,2,2,34,0,0,0,0,0,0,0,0,0,17,1,1,1,1,1,1,37,0,0,0,0,0,0,
    0,0,0,1,1,32,0,0,0,0,0,0,42,0,0,0,0,0,42,0,0,0,0,0,0,34,0,0,0,0,0,0,
    0,16,0,0,0,0,0,2,2,2,2,2,2,2,38,0,0,0,0,0,0,0,0,16,0,0,0,
};

// TAS3.tas - Level 3
static const uint8_t tas3_inputs[] = {
    18,2,2,2,2,2,2,2,36,0,0,0,0,0,0,0,0,0,0,18,2,2,2,2,42,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,38,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,1,17,1,1,1,1,1,1,36,0,0,0,0,0,0,0,0,0,0,0,0,16,
    0,0,16,0,0,0,0,0,0,0,0,0,
};

// ---- Main ----

int main(void) {
    int result;
    int pass = 0, fail = 0;

    fputs("=== TAS Validation Tests ===\n\n", stdout);

    // ---- TAS1: Level 1 ----
    fputs("TAS1 (Level 1, room 1):\n", stdout);
    result = run_tas(
        tas1_inputs, sizeof(tas1_inputs),
        collision_level1, COLLISION_LEVEL1_COUNT,
        object_level1, OBJECT_LEVEL1_COUNT,
        spawn_level1, 1
    );
    if (result == 1) {
        fputs("  PASS: Room transition achieved\n", stdout);
        pass++;
    } else {
        fputs("  FAIL: No room transition (result=", stdout);
        print_int(result);
        fputs(")\n", stdout);
        fail++;
    }

    fputs("\n", stdout);

    // ---- TAS3: Level 3 ----
    fputs("TAS3 (Level 3, room 3):\n", stdout);
    result = run_tas(
        tas3_inputs, sizeof(tas3_inputs),
        collision_level3, COLLISION_LEVEL3_COUNT,
        object_level3, OBJECT_LEVEL3_COUNT,
        spawn_level3, 3
    );
    if (result == 1) {
        fputs("  PASS: Room transition achieved\n", stdout);
        pass++;
    } else {
        fputs("  FAIL: No room transition (result=", stdout);
        print_int(result);
        fputs(")\n", stdout);
        fail++;
    }

    // ---- Summary ----
    fputs("\n", stdout);
    fputs("Pass: ", stdout);
    print_int(pass);
    fputs("  Fail: ", stdout);
    print_int(fail);
    putchar('\n');

    if (fail > 0) {
        fputs("SOME TESTS FAILED\n", stdout);
        return 1;
    }
    fputs("ALL TESTS PASSED\n", stdout);
    return 0;
}
