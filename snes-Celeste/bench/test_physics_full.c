// Full physics comparison: NES port vs ccleste-faithful simulation
// Build: mos-sim-clang -Os -o bench/test_physics_full bench/test_physics_full.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run:   mos-sim bench/test_physics_full

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

// ---- ccleste-faithful float simulation ----
typedef struct {
    float x, y;
    float spd_x, spd_y;
    float rem_x, rem_y;
    int grace, jbuffer, dash_time, djump;
    float dash_target_x, dash_target_y;
    float dash_accel_x, dash_accel_y;
} CC_Player;

// Floor at row 13 (y>=104 solid in 128px)
static bool cc_wall_col6 = false; // test flag to add wall at col 6
static bool cc_tile_solid(int tx, int ty) {
    if (tx < 0 || tx >= 16 || ty < 0 || ty >= 16) return false;
    if (ty >= 13) return true;
    if (cc_wall_col6 && tx == 6) return true;
    return false;
}

static bool cc_solid_at(float px, float py, float ox, float oy, float w, float h) {
    int tx1 = (int)((px + ox) / 8);
    int ty1 = (int)((py + oy) / 8);
    int tx2 = (int)((px + ox + w - 1) / 8);
    int ty2 = (int)((py + oy + h - 1) / 8);
    for (int ty = ty1; ty <= ty2; ty++)
        for (int tx = tx1; tx <= tx2; tx++)
            if (cc_tile_solid(tx, ty)) return true;
    return false;
}

// hitbox {1,3,6,5}
static bool cc_is_solid(CC_Player *p, float ox, float oy) {
    return cc_solid_at(p->x, p->y, 1+ox, 3+oy, 6, 5);
}

static float cc_sign(float x) { return x > 0 ? 1 : (x < 0 ? -1 : 0); }
static float cc_abs(float x) { return x < 0 ? -x : x; }
static float cc_appr(float v, float t, float a) {
    return (v > t) ? ((v - a < t) ? t : v - a) : ((v + a > t) ? t : v + a);
}
static float cc_flr(float x) { return (float)(int)(x < 0 ? (x == (int)x ? x : x - 1) : x); }

static void cc_move(CC_Player *p) {
    // X
    p->rem_x += p->spd_x;
    float amtx = cc_flr(p->rem_x + 0.5f);
    p->rem_x -= amtx;
    if (amtx != 0) {
        float step = cc_sign(amtx);
        for (int i = 0; i <= (int)cc_abs(amtx); i++) {
            if (!cc_is_solid(p, step, 0)) {
                p->x += step;
            } else {
                p->spd_x = 0;
                p->rem_x = 0;
                break;
            }
        }
    }
    // Y
    p->rem_y += p->spd_y;
    float amty = cc_flr(p->rem_y + 0.5f);
    p->rem_y -= amty;
    if (amty != 0) {
        float step = cc_sign(amty);
        for (int i = 0; i <= (int)cc_abs(amty); i++) {
            if (!cc_is_solid(p, 0, step)) {
                p->y += step;
            } else {
                p->spd_y = 0;
                p->rem_y = 0;
                break;
            }
        }
    }
}

static void cc_init(CC_Player *p) {
    p->x = 32; p->y = 96;
    p->spd_x = 0; p->spd_y = 0;
    p->rem_x = 0; p->rem_y = 0;
    p->grace = 6; p->jbuffer = 0;
    p->dash_time = 0; p->djump = 1;
    p->dash_target_x = 0; p->dash_target_y = 0;
    p->dash_accel_x = 0; p->dash_accel_y = 0;
}

// ---- NES port setup ----
static void nes_setup(void) {
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
    GLOBAL_FrameCount = 10;
    GLOBAL_FreezeFrames = 0;
    GLOBAL_PausePlayerFrames = 0;
    GLOBAL_DoubleDashUnlocked = false;
}

static void nes_clear_buttons(void) {
    GLOBAL_InputState = 0;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);
}

static void nes_reset_pos(int16_t x, int16_t y) {
    GLOBAL_PlayerData.objData.pos.x = x;
    GLOBAL_PlayerData.objData.pos.y = y;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(x);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(y);
    GLOBAL_PlayerData.spd.x = 0;
    GLOBAL_PlayerData.spd.y = 0;
    GLOBAL_PlayerData.graceTimer = 6;
    GLOBAL_PlayerData.dashCounter = 0;
    GLOBAL_PlayerData.dashesLeft = 1;
}

// ---- Output helpers ----
static void print_int(long v) {
    char buf[12]; int n = 0;
    if (v < 0) { putchar('-'); v = -v; }
    if (v == 0) { putchar('0'); return; }
    while(v) { buf[n++] = '0' + (v % 10); v /= 10; }
    while(n--) putchar(buf[n]);
}

static int pass_count = 0;
static int fail_count = 0;

static void check_match(const char *test, const char *field, int frame, long nes_val, long cc_val) {
    if (nes_val == cc_val) {
        pass_count++;
    } else {
        fail_count++;
        fputs("MISMATCH ", stdout);
        fputs(test, stdout);
        fputs(" f", stdout); print_int(frame);
        fputs(" ", stdout); fputs(field, stdout);
        fputs(": nes=", stdout); print_int(nes_val);
        fputs(" cc=", stdout); print_int(cc_val);
        putchar('\n');
    }
}

// ---- Test: Jump ----
static void test_jump(void) {
    CC_Player cc; cc_init(&cc);
    nes_setup(); nes_clear_buttons(); nes_reset_pos(32, 96);
    int i;

    fputs("== JUMP ==\n", stdout);

    // Frame 0: jump
    cc.jbuffer = 4;
    // ccleste: on_ground, gravity skipped, jump fires
    cc.spd_y = -2; cc.grace = 0; cc.jbuffer = 0;
    cc_move(&cc);

    GLOBAL_InputState = PORT_INPUT_B_MASK;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    check_match("jump", "y", 0, GLOBAL_PlayerData.objData.pos.y, (long)cc.y);
    GLOBAL_InputState = 0;

    for (i = 1; i <= 20; i++) {
        // ccleste physics
        bool cc_on_ground = cc_is_solid(&cc, 0, 1);
        if (cc_on_ground) {
            cc.spd_y = 0;
            cc.grace = 6;
        } else {
            float grav = 0.21f;
            if (cc_abs(cc.spd_y) <= 0.15f) grav *= 0.5f;
            cc.spd_y = cc_appr(cc.spd_y, 2.0f, grav);
        }
        cc_move(&cc);
        if (cc.y >= 96) { cc.y = 96; cc.spd_y = 0; cc.rem_y = 0; }

        // NES
        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        check_match("jump", "y", i, GLOBAL_PlayerData.objData.pos.y, (long)cc.y);
    }
}

// ---- Test: Run right ----
static void test_run(void) {
    CC_Player cc; cc_init(&cc);
    nes_setup(); nes_clear_buttons(); nes_reset_pos(32, 96);
    int i;

    fputs("== RUN RIGHT ==\n", stdout);
    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;

    for (i = 0; i < 15; i++) {
        // ccleste: on ground, apply horizontal accel
        float accel = 0.6f;
        float maxrun = 1.0f;
        if (cc_abs(cc.spd_x) > maxrun) {
            cc.spd_x = cc_appr(cc.spd_x, cc_sign(cc.spd_x) * maxrun, 0.15f);
        } else {
            cc.spd_x = cc_appr(cc.spd_x, 1.0f * maxrun, accel);
        }
        cc.spd_y = 0;
        cc_move(&cc);

        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        check_match("run", "x", i, GLOBAL_PlayerData.objData.pos.x, (long)cc.x);
    }
    GLOBAL_InputState = 0;
}

// ---- Test: Dash right ----
static void test_dash(void) {
    CC_Player cc; cc_init(&cc);
    nes_setup(); nes_clear_buttons(); nes_reset_pos(32, 96);
    int i;

    fputs("== DASH RIGHT ==\n", stdout);

    // Frame 0: initiate dash
    cc.spd_x = 5.0f; cc.spd_y = 0;
    cc.dash_time = 4;
    cc.dash_target_x = 2.0f; cc.dash_target_y = 0;
    cc.dash_accel_x = 1.5f; cc.dash_accel_y = 1.5f;
    cc.djump = 0;
    // Clamp (runs even on dash init frame in ccleste):
    if (cc_abs(cc.spd_x) > 1.0f) {
        cc.spd_x = cc_appr(cc.spd_x, cc_sign(cc.spd_x) * 1.0f, 0.15f);
    }
    cc_move(&cc);

    GLOBAL_InputState = PORT_INPUT_Y_MASK | PORT_INPUT_RIGHT_MASK;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    check_match("dash", "x", 0, GLOBAL_PlayerData.objData.pos.x, (long)cc.x);
    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;

    for (i = 1; i <= 8; i++) {
        if (GLOBAL_FreezeFrames > 0) { GLOBAL_FreezeFrames--; continue; }

        // ccleste dash frames
        if (cc.dash_time > 0) {
            cc.dash_time--;
            cc.spd_x = cc_appr(cc.spd_x, cc.dash_target_x, cc.dash_accel_x);
            cc.spd_y = cc_appr(cc.spd_y, cc.dash_target_y, cc.dash_accel_y);
        } else {
            // Normal movement after dash
            float accel = 0.6f;
            if (cc_abs(cc.spd_x) > 1.0f)
                cc.spd_x = cc_appr(cc.spd_x, cc_sign(cc.spd_x) * 1.0f, 0.15f);
            else
                cc.spd_x = cc_appr(cc.spd_x, 1.0f, accel);
        }
        cc.spd_y = 0;
        cc_move(&cc);

        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        check_match("dash", "x", i, GLOBAL_PlayerData.objData.pos.x, (long)cc.x);
    }
}

// ---- Test: Wall collision ----
static void test_wall(void) {
    CC_Player cc; cc_init(&cc);
    nes_setup();
    // Add wall at tile col 6 (x=48..55)
    { uint16_t j; for (j = 0; j < 16; j++) GLOBAL_ActiveLevel.collisionFlagsArr[j * 16 + 6] |= 0x01; }
    nes_clear_buttons();
    nes_reset_pos(40, 96);
    cc.x = 40; cc.y = 96;
    int i;

    fputs("== WALL STOP ==\n", stdout);
    cc_wall_col6 = true; // enable wall in ccleste sim too
    GLOBAL_InputState = PORT_INPUT_RIGHT_MASK;
    cc.spd_x = 0;

    for (i = 0; i < 10; i++) {
        // ccleste
        float accel = 0.6f;
        cc.spd_x = cc_appr(cc.spd_x, 1.0f, accel);
        cc.spd_y = 0;
        cc_move(&cc); // will stop at wall

        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        check_match("wall", "x", i, GLOBAL_PlayerData.objData.pos.x, (long)cc.x);
    }
    GLOBAL_InputState = 0;
    cc_wall_col6 = false;
}

// ---- Test: Gravity / freefall ----
static void test_gravity(void) {
    CC_Player cc; cc_init(&cc);
    cc.x = 32; cc.y = 50; cc.grace = 0;
    nes_setup(); nes_clear_buttons();
    GLOBAL_PlayerData.objData.pos.x = 32;
    GLOBAL_PlayerData.objData.pos.y = 50;
    GLOBAL_PlayerData.posF.x = INT_TO_FIXED(32);
    GLOBAL_PlayerData.posF.y = INT_TO_FIXED(50);
    GLOBAL_PlayerData.spd.x = 0; GLOBAL_PlayerData.spd.y = 0;
    GLOBAL_PlayerData.graceTimer = 0;
    int i;

    fputs("== FREEFALL ==\n", stdout);
    GLOBAL_InputState = 0;

    for (i = 0; i < 20; i++) {
        float grav = 0.21f;
        if (cc_abs(cc.spd_y) <= 0.15f) grav *= 0.5f;
        cc.spd_y = cc_appr(cc.spd_y, 2.0f, grav);
        cc_move(&cc);

        GLOBAL_FrameCount++;
        playerUpdate(&GLOBAL_PlayerData);

        check_match("fall", "y", i, GLOBAL_PlayerData.objData.pos.y, (long)cc.y);
    }
}

// ---- Test: Wall jump ----
static void test_walljump(void) {
    nes_setup();
    // Wall at tile col 3 (x=24..31)
    { uint16_t j; for (j = 0; j < 16; j++) GLOBAL_ActiveLevel.collisionFlagsArr[j * 16 + 3] |= 0x01; }
    nes_clear_buttons();
    // Player at x=32 (touching wall at col 3 to the left, hitbox left at 33)
    // Wall check: OBJ_isSolidAt(this, -3, 0) checks x-3+1=30, which is in tile 3 (24-31). Solid!
    nes_reset_pos(32, 80);
    GLOBAL_PlayerData.graceTimer = 0; // in air

    fputs("== WALL JUMP ==\n", stdout);

    // Press jump while next to wall
    GLOBAL_InputState = PORT_INPUT_B_MASK;
    GLOBAL_FrameCount++;
    playerUpdate(&GLOBAL_PlayerData);

    // Wall jump should set spd.y = -2 and spd.x = +(maxrun+1) = +2.0
    long nes_spd_y = GLOBAL_PlayerData.spd.y;
    long nes_spd_x = GLOBAL_PlayerData.spd.x;
    long exp_spd_y = INT_TO_FIXED(-2);
    // Wall jump sets spd.x = 2.0, then speed clamp reduces by deccel (0.15)
    long exp_spd_x = INT_TO_FIXED(2) - FP_DECCEL; // 2.0 - 0.15 = 1.85

    check_match("wjump", "spd_y", 0, nes_spd_y, exp_spd_y);
    check_match("wjump", "spd_x", 0, nes_spd_x, exp_spd_x);
    GLOBAL_InputState = 0;
}

int main(void) {
    fputs("=== Full Physics Comparison ===\n\n", stdout);

    test_jump();
    putchar('\n');
    test_run();
    putchar('\n');
    test_dash();
    putchar('\n');
    test_wall();
    putchar('\n');
    test_gravity();
    putchar('\n');
    test_walljump();

    fputs("\n--- Results ---\n", stdout);
    fputs("Pass: ", stdout); print_int(pass_count);
    fputs("  Fail: ", stdout); print_int(fail_count);
    putchar('\n');
    if (fail_count > 0) {
        fputs("MISMATCHES FOUND\n", stdout);
        return 1;
    }
    fputs("ALL MATCH\n", stdout);
    return 0;
}
