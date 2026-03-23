// Compare jump: tap vs hold, and check ccleste jbuffer behavior
// Build: mos-sim-clang -Os -o bench/trace_hold bench/trace_hold.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run:   mos-sim bench/trace_hold

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
    GLOBAL_ActiveLevel.playerSpawnY = 12;
    for (i = 0; i < 256; i++) {
        uint8_t r = i >> 4;
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = (r >= 13) ? 0x01 : 0;
    }
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(OBJ_DATA));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = 0xFF;
    }
    memset(&GLOBAL_PlayerData, 0, sizeof(GLOBAL_PlayerData));
    GLOBAL_PlayerData.objData.eType = OBJ_PLAYER;
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 96;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(96);
    GLOBAL_PlayerData.dashesLeft = 1;
    GLOBAL_PlayerData.movingPlatformIndex = -1;
    GLOBAL_PlayerData.graceTimer = 6;
    GLOBAL_PlayerData.dashCounter = 0;
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = 0;
    GLOBAL_FrameCount = 10;
    GLOBAL_FreezeFrames = 0;
    GLOBAL_PausePlayerFrames = 0;
    GLOBAL_DoubleDashUnlocked = false;
}

static void run_jump_trace(const char *label, bool hold_button) {
    uint8_t i;
    int16_t minY;

    fputs(label, stdout);
    putchar('\n');

    setup_ground();
    // Clear button state
    GLOBAL_InputState = 0;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);
    // Reset after clear
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 96;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(96);
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = 0;
    GLOBAL_PlayerData.graceTimer = 6;

    minY = 96;

    fputs("Fr | pos.y | spd.y(fix) | render\n", stdout);

    for (i = 0; i <= 20; i++) {
        if (i == 0) {
            // Press jump
            GLOBAL_InputState = PORT_INPUT_B_MASK;
        } else if (!hold_button) {
            // Release after first frame
            GLOBAL_InputState = 0;
        }
        // else: keep holding B

        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        if (GLOBAL_PlayerData.objData.pos.y < minY)
            minY = GLOBAL_PlayerData.objData.pos.y;

        if (i < 10) fputs(" ", stdout);
        print_int(i);
        fputs(" | ", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.y);
        fputs(" | ", stdout);
        print_int(GLOBAL_PlayerData.spd.y);
        fputs(" | ", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.y * 2);
        putchar('\n');
    }

    fputs("Peak: y=", stdout);
    print_int(minY);
    fputs(" height=", stdout);
    print_int(96 - minY);
    fputs("px (", stdout);
    print_int((96 - minY) * 100 / 8);
    fputs("/100 tiles)\n\n", stdout);
}

// Also simulate ccleste style (held button refreshes jbuffer)
static void run_ccleste_trace(void) {
    int16_t pos_y = 96;
    float rem_y = 0;
    float spd_y = 0;
    float gravity;
    int i;
    int16_t minY = 96;
    bool jumped = false;
    int grace = 6;
    // ccleste: jbuffer refreshed every frame button held

    fputs("=== ccleste-style (float, held button) ===\n", stdout);
    fputs("Fr | pos.y | spd.y | moved\n", stdout);

    for (i = 0; i <= 20; i++) {
        // Gravity
        if (pos_y < 96) { // not on ground
            gravity = 0.21f;
            if (spd_y < 0 && -spd_y <= 0.15f) gravity *= 0.5f;
            else if (spd_y >= 0 && spd_y <= 0.15f) gravity *= 0.5f;

            // approach maxfall
            if (spd_y < 2.0f) {
                spd_y += gravity;
                if (spd_y > 2.0f) spd_y = 2.0f;
            }
        } else {
            spd_y = 0;
            grace = 6;
        }

        // Jump (button always held, jbuffer always 4 in ccleste)
        if (grace > 0 && !jumped) {
            spd_y = -2.0f;
            grace = 0;
            jumped = true;
        }

        // OBJ_move
        rem_y += spd_y;
        int my;
        {
            float rounded = (float)((int)(rem_y + (rem_y >= 0 ? 0.5f : -0.5f)));
            // P8flr(rem_y + 0.5)
            float floored = rem_y + 0.5f;
            if (floored < 0) {
                my = (int)floored - (floored != (int)floored ? 1 : 0);
            } else {
                my = (int)floored;
            }
        }
        rem_y -= my;
        pos_y += my;

        // Land
        if (pos_y >= 96) {
            pos_y = 96;
            spd_y = 0;
            rem_y = 0;
        }

        if (pos_y < minY) minY = pos_y;

        if (i < 10) fputs(" ", stdout);
        print_int(i);
        fputs(" | ", stdout);
        print_int(pos_y);
        fputs(" | ", stdout);
        print_int((long)(spd_y * 1000));
        fputs("/1k | ", stdout);
        print_int(my);
        putchar('\n');
    }

    fputs("Peak: y=", stdout);
    print_int(minY);
    fputs(" height=", stdout);
    print_int(96 - minY);
    fputs("px\n\n", stdout);
}

int main(void) {
    run_jump_trace("=== NES: TAP jump (release after 1 frame) ===", false);
    run_jump_trace("=== NES: HOLD jump (hold B entire arc) ===", true);
    run_ccleste_trace();
    return 0;
}
