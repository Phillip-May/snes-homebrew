// Spring trajectory comparison: NES vs ccleste float, frame-by-frame
// Build: mos-sim-clang -Os -o bench/trace_spring bench/trace_spring.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run:   mos-sim bench/trace_spring

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

// --- ccleste float simulation ---
static float cc_flr(float x) { return (float)(int)(x < 0 ? (x == (int)x ? x : x - 1) : x); }
static float cc_abs(float x) { return x < 0 ? -x : x; }
static float cc_sign(float x) { return x > 0 ? 1 : (x < 0 ? -1 : 0); }
static float cc_appr(float v, float t, float a) {
    return (v > t) ? ((v - a < t) ? t : v - a) : ((v + a > t) ? t : v + a);
}

static void setup_nes(int16_t startX, int16_t startY, fixed_t spdX, fixed_t spdY) {
    uint16_t i;
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = 1;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
    GLOBAL_ActiveLevel.playerSpawnX = 4;
    GLOBAL_ActiveLevel.playerSpawnY = 12;
    for (i = 0; i < 256; i++) {
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = ((i >> 4) >= 13) ? 0x01 : 0;
    }
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(OBJ_DATA));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = 0xFF;
    }
    memset(&GLOBAL_PlayerData, 0, sizeof(GLOBAL_PlayerData));
    GLOBAL_PlayerData.objData.eType = OBJ_PLAYER;
    GLOBAL_PlayerData.objData.pos.x = startX;
    GLOBAL_PlayerData.objData.pos.y = startY;
    GLOBAL_PlayerData.posF.x = 0;
    GLOBAL_PlayerData.posF.y = 0;
    GLOBAL_PlayerData.spd.x = spdX;
    GLOBAL_PlayerData.spd.y = spdY;
    GLOBAL_PlayerData.dashesLeft = 1;
    GLOBAL_PlayerData.movingPlatformIndex = -1;
    GLOBAL_PlayerData.graceTimer = 0;
    GLOBAL_PlayerData.dashCounter = 0;
    GLOBAL_FrameCount = 10;
    GLOBAL_FreezeFrames = 0;
    GLOBAL_PausePlayerFrames = 0;
    GLOBAL_DoubleDashUnlocked = false;
    // Clear button state
    GLOBAL_InputState = 0;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);
    // Reset after clearing
    GLOBAL_PlayerData.objData.pos.x = startX;
    GLOBAL_PlayerData.objData.pos.y = startY;
    GLOBAL_PlayerData.posF.x = 0;
    GLOBAL_PlayerData.posF.y = 0;
    GLOBAL_PlayerData.spd.x = spdX;
    GLOBAL_PlayerData.spd.y = spdY;
    GLOBAL_PlayerData.graceTimer = 0;
}

// ccleste: OBJ_move with solids=true (player)
static void cc_move_solid(float *x, float *y, float *spd_x, float *spd_y, float *rem_x, float *rem_y) {
    // X
    *rem_x += *spd_x;
    float ax = cc_flr(*rem_x + 0.5f);
    *rem_x -= ax;
    if (ax != 0) {
        float step = cc_sign(ax);
        for (int i = 0; i <= (int)cc_abs(ax); i++) {
            // simplified: no wall collision in this test
            *x += step;
        }
    }
    // Y
    *rem_y += *spd_y;
    float ay = cc_flr(*rem_y + 0.5f);
    *rem_y -= ay;
    if (ay != 0) {
        float step = cc_sign(ay);
        for (int i = 0; i <= (int)cc_abs(ay); i++) {
            // Check floor at y=104 (tile row 13)
            if (step > 0 && *y + step + 8 > 104) {
                *spd_y = 0; *rem_y = 0; break;
            }
            *y += step;
        }
    }
}

int main(void) {
    int i;
    int total_match = 0, total_diff = 0;

    fputs("=== Spring Trajectory: NES vs ccleste ===\n\n", stdout);

    // Test 1: Straight up spring (no horizontal)
    fputs("== Vertical spring (spd.x=0, spd.y=-3) ==\n", stdout);
    setup_nes(32, 84, 0, INT_TO_FIXED(-3));
    float cc_x=32, cc_y=84, cc_sx=0, cc_sy=-3, cc_rx=0, cc_ry=0;
    int16_t nes_minY=84, cc_minY=84;

    fputs("Fr | NES(x,y) | CC(x,y) | match\n", stdout);
    GLOBAL_InputState = 0;
    for (i = 0; i < 30; i++) {
        // NES
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);
        if (GLOBAL_PlayerData.objData.pos.y < nes_minY) nes_minY = GLOBAL_PlayerData.objData.pos.y;

        // ccleste
        float grav = 0.21f;
        if (cc_abs(cc_sy) <= 0.15f) grav *= 0.5f;
        cc_sy = cc_appr(cc_sy, 2.0f, grav);
        cc_move_solid(&cc_x, &cc_y, &cc_sx, &cc_sy, &cc_rx, &cc_ry);
        if ((int)cc_y < cc_minY) cc_minY = (int)cc_y;

        int ym = (GLOBAL_PlayerData.objData.pos.y == (int)cc_y);
        if (!ym) total_diff++; else total_match++;

        if (i < 10) fputs(" ", stdout);
        print_int(i); fputs(" | (", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.x); putchar(',');
        print_int(GLOBAL_PlayerData.objData.pos.y); fputs(") | (", stdout);
        print_int((long)cc_x); putchar(',');
        print_int((long)cc_y); fputs(") | ", stdout);
        fputs(ym ? "OK" : "DIFF", stdout); putchar('\n');
    }
    fputs("Peak: NES=", stdout); print_int(nes_minY);
    fputs(" CC=", stdout); print_int(cc_minY);
    fputs(" height: NES=", stdout); print_int(84 - nes_minY);
    fputs(" CC=", stdout); print_int(84 - cc_minY);
    fputs("\n\n", stdout);

    // Test 2: Spring with horizontal speed (running right then hitting spring)
    fputs("== Diagonal spring (spd.x=0.2, spd.y=-3) ==\n", stdout);
    // ccleste: after spring, spd.x = old_spd * 0.2. If player was running at maxrun=1.0, spd.x=0.2
    fixed_t nesHSpd = 13107; // 0.2 in fix16 (exact ccleste value)
    setup_nes(32, 84, nesHSpd, INT_TO_FIXED(-3));
    cc_x=32; cc_y=84; cc_sx=0.2f; cc_sy=-3; cc_rx=0; cc_ry=0;

    fputs("Fr | NES(x,y) | CC(x,y) | match\n", stdout);
    GLOBAL_InputState = 0;
    for (i = 0; i < 30; i++) {
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        float grav = 0.21f;
        if (cc_abs(cc_sy) <= 0.15f) grav *= 0.5f;
        cc_sy = cc_appr(cc_sy, 2.0f, grav);
        // In ccleste after spring, no input → spd.x decelerates toward 0 via deccel
        // But actually approach(spd.x, 0, 0.15) since no input
        if (cc_abs(cc_sx) > 1.0f)
            cc_sx = cc_appr(cc_sx, cc_sign(cc_sx), 0.15f);
        else
            cc_sx = cc_appr(cc_sx, 0, 0.6f); // no input, approach 0
        cc_move_solid(&cc_x, &cc_y, &cc_sx, &cc_sy, &cc_rx, &cc_ry);

        int xm = (GLOBAL_PlayerData.objData.pos.x == (int)cc_x);
        int ym = (GLOBAL_PlayerData.objData.pos.y == (int)cc_y);
        if (xm && ym) total_match++; else total_diff++;

        if (i < 10) fputs(" ", stdout);
        print_int(i); fputs(" | (", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.x); putchar(',');
        print_int(GLOBAL_PlayerData.objData.pos.y); fputs(") | (", stdout);
        print_int((long)cc_x); putchar(',');
        print_int((long)cc_y); fputs(") | ", stdout);
        fputs((xm && ym) ? "OK" : "DIFF", stdout); putchar('\n');
    }

    // Test 3: Spring with player holding RIGHT during bounce
    fputs("\n== Spring + hold right (spd.x=0.2, spd.y=-3, hold right) ==\n", stdout);
    setup_nes(32, 84, nesHSpd, INT_TO_FIXED(-3));
    cc_x=32; cc_y=84; cc_sx=0.2f; cc_sy=-3; cc_rx=0; cc_ry=0;

    fputs("Fr | NES(x,y) | CC(x,y) | match\n", stdout);
    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;
    for (i = 0; i < 30; i++) {
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        float grav = 0.21f;
        if (cc_abs(cc_sy) <= 0.15f) grav *= 0.5f;
        cc_sy = cc_appr(cc_sy, 2.0f, grav);
        // Holding right: approach maxrun=1.0 with air accel 0.4
        cc_sx = cc_appr(cc_sx, 1.0f, 0.4f);
        cc_move_solid(&cc_x, &cc_y, &cc_sx, &cc_sy, &cc_rx, &cc_ry);

        int xm = (GLOBAL_PlayerData.objData.pos.x == (int)cc_x);
        int ym = (GLOBAL_PlayerData.objData.pos.y == (int)cc_y);
        if (xm && ym) total_match++; else total_diff++;

        if (i < 10) fputs(" ", stdout);
        print_int(i); fputs(" | (", stdout);
        print_int(GLOBAL_PlayerData.objData.pos.x); putchar(',');
        print_int(GLOBAL_PlayerData.objData.pos.y); fputs(") | (", stdout);
        print_int((long)cc_x); putchar(',');
        print_int((long)cc_y); fputs(") | ", stdout);
        fputs((xm && ym) ? "OK" : "DIFF", stdout); putchar('\n');
    }

    fputs("\n--- Summary ---\n", stdout);
    fputs("Matching frames: ", stdout); print_int(total_match);
    fputs("  Different: ", stdout); print_int(total_diff);
    putchar('\n');
    return 0;
}
