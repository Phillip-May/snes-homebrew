// Spike hitbox parity checks against ccleste's modulo/boundary rules.
// Build:
// C:\llvm-mos\bin\mos-sim-clang.bat -Os -o bench\test_spikes bench\test_spikes.c src\libfixmath\fix16.c -Isrc -Isrc\port -Isrc\libfixmath -Ipython
// Run:
// C:\llvm-mos\bin\mos-sim.exe bench\test_spikes

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#define _WIN32
#define PORT_FUNC_BANK6
#define PORT_FUNC_BANK5
#define PORT_FUNC_BANK4
#define PORT_FUNC_BANK3
#define PORT_FUNC_BANK2
#define PORT_FUNC_BANK1
#define PORT_DATA_BANK6
#define PORT_DATA_BANK5
#define PORT_DATA_BANK4
#define PORT_DATA_BANK3
#define PORT_DATA_BANK2
#define PORT_DATA_BANK1
#define __NES__
#include "fixedPointSNES.h"
#include "port/port.h"

void port_updateCollapseTileNametable(uint8_t i) { (void)i; }
void port_beginSpriteBuild(const struct sPlayerData *p) { (void)p; }
void port_finishSpriteBuild(void) {}
void port_updatePlayerSprite(const struct sPlayerData *p) { (void)p; }
void port_resetSprites(void) {}
void port_drawText(const unsigned char *t, uint8_t x, uint8_t y) { (void)t; (void)x; (void)y; }
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
void port_buildFlag(uint8_t i) { (void)i; }
void port_buildKey(uint8_t i) { (void)i; }
void port_buildSpring(uint8_t i) { (void)i; }
void port_buildCollapseTile(uint8_t i) { (void)i; }
void port_buildStrawberry(uint8_t i) { (void)i; }
void port_buildPlatMov(uint8_t i) { (void)i; }
void port_buildFlyingBerry(uint8_t i) { (void)i; }
void port_buildDoubleDashOrb(uint8_t i) { (void)i; }
void port_buildStaticDecor(uint8_t i) { (void)i; }
void port_buildSpriteIfDirty(uint8_t i, enum eOBJType t);
void port_buildUnused(uint8_t i);

#define main game_main_excluded
#define onVblank game_onVblank_excluded
#include "mainBankZero.c"
#undef main
#undef onVblank

void port_buildSpriteIfDirty(uint8_t i, enum eOBJType t) { (void)t; GLOBAL_OBJList[i].flags &= (uint8_t)~OBJ_FLAG_DIRTY; }
void port_buildUnused(uint8_t i) { GLOBAL_OBJList[i].flags &= (uint8_t)~OBJ_FLAG_DIRTY; }

static int pass_count = 0;
static int fail_count = 0;

static void check(const char *name, bool got, bool expected) {
    if (got == expected) {
        pass_count++;
    } else {
        fail_count++;
        fputs("FAIL: ", stdout);
        fputs(name, stdout);
        fputs(got ? " got=true" : " got=false", stdout);
        fputs(expected ? " expected=true\n" : " expected=false\n", stdout);
    }
}

static void setup(void) {
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
}

int main(void) {
    setup();
    GLOBAL_ActiveLevel.collisionFlagsArr[13 * 16 + 4] = 0x04;
    check("floor_before_threshold", isDeathAtPoint(33, 105, 6, 5, 0, INT_TO_FIXED(1)), false);
    check("floor_at_threshold", isDeathAtPoint(33, 106, 6, 5, 0, INT_TO_FIXED(1)), true);
    check("floor_exact_bottom", isDeathAtPoint(33, 107, 6, 5, 0, INT_TO_FIXED(1)), true);
    check("floor_wrong_direction", isDeathAtPoint(33, 106, 6, 5, 0, INT_TO_FIXED(-1)), false);

    setup();
    GLOBAL_ActiveLevel.collisionFlagsArr[4 * 16 + 4] = 0x08;
    check("ceiling_before_threshold", isDeathAtPoint(33, 35, 6, 5, 0, INT_TO_FIXED(-1)), false);
    check("ceiling_at_threshold", isDeathAtPoint(33, 34, 6, 5, 0, INT_TO_FIXED(-1)), true);
    check("ceiling_wrong_direction", isDeathAtPoint(33, 34, 6, 5, 0, INT_TO_FIXED(1)), false);

    setup();
    GLOBAL_ActiveLevel.collisionFlagsArr[4 * 16 + 4] = 0x10;
    check("left_before_threshold", isDeathAtPoint(35, 33, 6, 5, INT_TO_FIXED(-1), 0), false);
    check("left_at_threshold", isDeathAtPoint(34, 33, 6, 5, INT_TO_FIXED(-1), 0), true);
    check("left_wrong_direction", isDeathAtPoint(34, 33, 6, 5, INT_TO_FIXED(1), 0), false);

    setup();
    GLOBAL_ActiveLevel.collisionFlagsArr[4 * 16 + 4] = 0x20;
    check("right_before_threshold", isDeathAtPoint(32, 33, 6, 5, INT_TO_FIXED(1), 0), false);
    check("right_at_threshold", isDeathAtPoint(33, 33, 6, 5, INT_TO_FIXED(1), 0), true);
    check("right_exact_edge", isDeathAtPoint(35, 33, 5, 5, INT_TO_FIXED(1), 0), true);
    check("right_wrong_direction", isDeathAtPoint(33, 33, 6, 5, INT_TO_FIXED(-1), 0), false);

    fputs("Pass: ", stdout);
    printf("%d", pass_count);
    fputs("  Fail: ", stdout);
    printf("%d\n", fail_count);
    return fail_count == 0 ? 0 : 1;
}
