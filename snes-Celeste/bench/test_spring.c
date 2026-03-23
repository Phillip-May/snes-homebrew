// Spring behavior comparison: NES port vs ccleste
// Build: mos-sim-clang -Os -o bench/test_spring bench/test_spring.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run:   mos-sim bench/test_spring

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
    if (cond) pass_count++;
    else { fail_count++; fputs("FAIL: ", stdout); fputs(name, stdout); putchar('\n'); }
}
static void check_eq(const char *name, long a, long b) {
    if (a == b) pass_count++;
    else { fail_count++; fputs("FAIL: ", stdout); fputs(name, stdout);
           fputs(" got=", stdout); print_int(a); fputs(" exp=", stdout); print_int(b); putchar('\n'); }
}
static void check_range(const char *name, long v, long lo, long hi) {
    if (v >= lo && v <= hi) pass_count++;
    else { fail_count++; fputs("FAIL: ", stdout); fputs(name, stdout);
           fputs(" got=", stdout); print_int(v);
           fputs(" range=[", stdout); print_int(lo); putchar(','); print_int(hi); putchar(']'); putchar('\n'); }
}

// --- ccleste float spring simulation ---
typedef struct {
    float x, y, spd_x, spd_y, rem_x, rem_y;
    int djump;
} CC;

static float cc_flr(float x) { return (float)(int)(x < 0 ? (x == (int)x ? x : x - 1) : x); }
static float cc_abs(float x) { return x < 0 ? -x : x; }
static float cc_sign(float x) { return x > 0 ? 1 : (x < 0 ? -1 : 0); }
static float cc_appr(float v, float t, float a) {
    return (v > t) ? ((v - a < t) ? t : v - a) : ((v + a > t) ? t : v + a);
}

static bool cc_tile_solid(int tx, int ty) {
    if (tx < 0 || tx >= 16 || ty < 0 || ty >= 16) return false;
    return (ty >= 13);
}

static bool cc_solid_at(float px, float py, float ox, float oy, float w, float h) {
    int tx1 = (int)((px + ox) / 8), ty1 = (int)((py + oy) / 8);
    int tx2 = (int)((px + ox + w - 1) / 8), ty2 = (int)((py + oy + h - 1) / 8);
    for (int ty = ty1; ty <= ty2; ty++)
        for (int tx = tx1; tx <= tx2; tx++)
            if (cc_tile_solid(tx, ty)) return true;
    return false;
}

static void cc_move(CC *p) {
    // X
    p->rem_x += p->spd_x;
    float ax = cc_flr(p->rem_x + 0.5f);
    p->rem_x -= ax;
    if (ax != 0) {
        float step = cc_sign(ax);
        for (int i = 0; i <= (int)cc_abs(ax); i++) {
            if (!cc_solid_at(p->x, p->y, 1+step, 3, 6, 5)) p->x += step;
            else { p->spd_x = 0; p->rem_x = 0; break; }
        }
    }
    // Y
    p->rem_y += p->spd_y;
    float ay = cc_flr(p->rem_y + 0.5f);
    p->rem_y -= ay;
    if (ay != 0) {
        float step = cc_sign(ay);
        for (int i = 0; i <= (int)cc_abs(ay); i++) {
            if (!cc_solid_at(p->x, p->y, 1, 3+step, 6, 5)) p->y += step;
            else { p->spd_y = 0; p->rem_y = 0; break; }
        }
    }
    if (p->y >= 96) { p->y = 96; p->spd_y = 0; p->rem_y = 0; }
}

// --- NES setup ---
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
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = (r >= 13) ? 0x01 : 0;
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
    int i;
    fputs("=== Spring Behavior Tests ===\n\n", stdout);

    // --- Test 1: Spring triggers and sets correct velocity ---
    fputs("== Spring trigger ==\n", stdout);
    setup();
    // Solid block under spring at tile (4, 12)
    GLOBAL_ActiveLevel.collisionFlagsArr[12 * 16 + 4] |= 0x01;
    // Create spring at (32, 88) - tile (4, 11)
    initObject(OBJ_SPRING, 32, 88);
    // Player falling onto spring
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 85;
    GLOBAL_PlayerData.posF.x = 0;
    GLOBAL_PlayerData.posF.y = 0;
    GLOBAL_PlayerData.spd.x = INT_TO_FIXED(1);
    GLOBAL_PlayerData.spd.y = INT_TO_FIXED(1);
    GLOBAL_PlayerData.graceTimer = 0;

    GLOBAL_InputState = 0;
    GLOBAL_FrameCount++;
    updateAllObjects();

    check_eq("spring_spd_y", GLOBAL_PlayerData.spd.y, INT_TO_FIXED(-3));
    check_eq("spring_pos_y", GLOBAL_PlayerData.objData.pos.y, 88 - 4); // thisY - 4 = 84
    check("spring_dashes_restored", GLOBAL_PlayerData.dashesLeft >= 1);

    // Check spd.x was reduced (ccleste: *= 0.2, NES: *= ~0.2265)
    // Original spd.x was 1.0 (65536), after *= 0.2 should be ~0.2 (13107)
    // NES FIXED_MUL_1_5: (65536>>2) - (65536>>5) + (65536>>7) = 16384 - 2048 + 512 = 14848
    // ccleste: 65536 * 0.2 = 13107
    fputs("  NES spd.x after spring: ", stdout); print_int(GLOBAL_PlayerData.spd.x);
    fputs(" (ccleste would be ~13107)\n", stdout);
    check_range("spring_spd_x_reduced", GLOBAL_PlayerData.spd.x, 10000, 20000);

    // --- Test 2: Spring bounce arc compared to ccleste ---
    fputs("\n== Spring bounce arc ==\n", stdout);
    setup();
    GLOBAL_ActiveLevel.collisionFlagsArr[12 * 16 + 4] |= 0x01;

    // NES: player launched from spring
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 84; // spring snapped here
    GLOBAL_PlayerData.posF.x = 0;
    GLOBAL_PlayerData.posF.y = 0;
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = INT_TO_FIXED(-3);
    GLOBAL_PlayerData.graceTimer = 0;

    // ccleste: same starting conditions
    CC cc = { .x=32, .y=84, .spd_x=0, .spd_y=-3, .rem_x=0, .rem_y=0, .djump=1 };

    GLOBAL_InputState = 0;
    int16_t nes_minY = 84, cc_minY = 84;

    fputs("Fr | NES_y | CC_y | NES_spd | CC_spd | match\n", stdout);

    for (i = 0; i < 25; i++) {
        // NES
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);
        if (GLOBAL_PlayerData.objData.pos.y < nes_minY)
            nes_minY = GLOBAL_PlayerData.objData.pos.y;

        // ccleste
        float grav = 0.21f;
        if (cc_abs(cc.spd_y) <= 0.15f) grav *= 0.5f;
        cc.spd_y = cc_appr(cc.spd_y, 2.0f, grav);
        cc_move(&cc);
        if ((int16_t)cc.y < cc_minY)
            cc_minY = (int16_t)cc.y;

        int match = (GLOBAL_PlayerData.objData.pos.y == (int16_t)cc.y);
        if (i < 10) fputs(" ", stdout);
        print_int(i);
        fputs(" | ", stdout); print_int(GLOBAL_PlayerData.objData.pos.y);
        fputs(" | ", stdout); print_int((long)cc.y);
        fputs(" | ", stdout); print_int(GLOBAL_PlayerData.spd.y);
        fputs(" | ", stdout); print_int((long)(cc.spd_y * 1000));
        fputs("/1k | ", stdout); fputs(match ? "OK" : "DIFF", stdout);
        putchar('\n');
    }

    fputs("NES peak: y=", stdout); print_int(nes_minY);
    fputs(" height=", stdout); print_int(84 - nes_minY);
    fputs("  CC peak: y=", stdout); print_int(cc_minY);
    fputs(" height=", stdout); print_int(84 - cc_minY);
    putchar('\n');

    // Spring bounce should be ~50% higher than normal jump (spd=-3 vs spd=-2)
    // Normal jump: 11px with <=loop. Spring: should be ~18-20px
    check_range("spring_height_nes", 84 - nes_minY, 25, 35);
    check_range("spring_height_cc", 84 - cc_minY, 25, 35);

    // Heights should be close (within 2px of each other)
    int16_t height_diff = (84 - nes_minY) - (84 - cc_minY);
    if (height_diff < 0) height_diff = -height_diff;
    check("spring_height_match", height_diff <= 2);

    // --- Test 3: Spring doesn't trigger when moving up ---
    fputs("\n== Spring no trigger when moving up ==\n", stdout);
    setup();
    GLOBAL_ActiveLevel.collisionFlagsArr[12 * 16 + 4] |= 0x01;
    initObject(OBJ_SPRING, 32, 88);
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 85;
    GLOBAL_PlayerData.posF.x = 0;
    GLOBAL_PlayerData.posF.y = 0;
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = INT_TO_FIXED(-1); // moving UP
    GLOBAL_PlayerData.graceTimer = 0;

    GLOBAL_InputState = 0;
    GLOBAL_FrameCount++;
    updateAllObjects();

    // Should NOT trigger spring (spd.y < 0)
    check("spring_no_trigger_up", GLOBAL_PlayerData.spd.y != INT_TO_FIXED(-3));

    // --- Test 4: Spring with horizontal speed ---
    fputs("\n== Spring with horizontal speed ==\n", stdout);
    setup();
    GLOBAL_ActiveLevel.collisionFlagsArr[12 * 16 + 4] |= 0x01;
    initObject(OBJ_SPRING, 32, 88);
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 85;
    GLOBAL_PlayerData.posF.x = 0;
    GLOBAL_PlayerData.posF.y = 0;
    GLOBAL_PlayerData.spd.x = INT_TO_FIXED(2); // fast horizontal
    GLOBAL_PlayerData.spd.y = INT_TO_FIXED(1);
    GLOBAL_PlayerData.graceTimer = 0;

    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;
    GLOBAL_FrameCount++;
    updateAllObjects();

    // Spring should trigger and reduce horizontal speed
    check_eq("spring_h_spd_y", GLOBAL_PlayerData.spd.y, INT_TO_FIXED(-3));
    // Horizontal speed should be reduced to ~20% of 2.0 = ~0.4
    // ccleste: 2.0 * 0.2 = 0.4 = 26214 in fix16
    // NES: FIXED_MUL_1_5(131072) = (131072>>2)-(131072>>5)+(131072>>7) = 32768-4096+1024 = 29696
    fputs("  horiz spd after: ", stdout); print_int(GLOBAL_PlayerData.spd.x);
    fputs(" (ccleste: ~26214)\n", stdout);
    check_range("spring_h_spd_x", GLOBAL_PlayerData.spd.x, 20000, 35000);

    // --- Results ---
    fputs("\n--- Results ---\n", stdout);
    fputs("Pass: ", stdout); print_int(pass_count);
    fputs("  Fail: ", stdout); print_int(fail_count);
    putchar('\n');
    if (fail_count > 0) fputs("ISSUES FOUND\n", stdout);
    else fputs("ALL PASS\n", stdout);
    return fail_count > 0 ? 1 : 0;
}
