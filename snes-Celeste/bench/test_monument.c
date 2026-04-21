// Monument text regression test.
// Build:
// C:\llvm-mos\bin\mos-sim-clang.bat -Os -o bench\test_monument bench\test_monument.c src\libfixmath\fix16.c -Isrc -Isrc\port -Isrc\libfixmath -Ipython
// Run:
// C:\llvm-mos\bin\mos-sim.exe bench\test_monument

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#define _WIN32
#define PORT_FUNC_BANK7
#define PORT_FUNC_BANK6
#define PORT_FUNC_BANK5
#define PORT_FUNC_BANK4
#define PORT_FUNC_BANK3
#define PORT_FUNC_BANK2
#define PORT_FUNC_BANK1
#define PORT_DATA_BANK7
#define PORT_DATA_BANK6
#define PORT_DATA_BANK5
#define PORT_DATA_BANK4
#define PORT_DATA_BANK3
#define PORT_DATA_BANK2
#define PORT_DATA_BANK1
#define __NES__
#include "fixedPointSNES.h"
#include "port/port.h"

typedef struct {
    char text[48];
    uint8_t x;
    uint8_t y;
} DrawCall;

static DrawCall draw_calls[160];
static uint8_t draw_call_count = 0;
static uint8_t nametable_update_count = 0;

void port_updateCollapseTileNametable(uint8_t i) { (void)i; nametable_update_count++; }
void port_beginSpriteBuild(const struct sPlayerData *p) { (void)p; }
void port_finishSpriteBuild(void) {}
void port_updatePlayerSprite(const struct sPlayerData *p) { (void)p; }
void port_resetSprites(void) {}
void port_drawText(const unsigned char *t, uint8_t x, uint8_t y)
{
    if (draw_call_count >= (uint8_t)(sizeof(draw_calls) / sizeof(draw_calls[0]))) {
        return;
    }
    strncpy(draw_calls[draw_call_count].text, (const char *)t, sizeof(draw_calls[draw_call_count].text) - 1u);
    draw_calls[draw_call_count].text[sizeof(draw_calls[draw_call_count].text) - 1u] = '\0';
    draw_calls[draw_call_count].x = x;
    draw_calls[draw_call_count].y = y;
    draw_call_count++;
}
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

static void check_true(const char *name, bool cond)
{
    if (cond) {
        pass_count++;
    } else {
        fail_count++;
        fputs("FAIL: ", stdout);
        fputs(name, stdout);
        putchar('\n');
    }
}

static void check_u8(const char *name, uint8_t got, uint8_t expected)
{
    if (got == expected) {
        pass_count++;
    } else {
        fail_count++;
        printf("FAIL: %s got=%u expected=%u\n", name, got, expected);
    }
}

static void check_str(const char *name, const char *got, const char *expected)
{
    if (strcmp(got, expected) == 0) {
        pass_count++;
    } else {
        fail_count++;
        printf("FAIL: %s got=\"%s\" expected=\"%s\"\n", name, got, expected);
    }
}

static void setup(void)
{
    uint8_t i;
    memset(&GLOBAL_ActiveLevel, 0, sizeof(GLOBAL_ActiveLevel));
    GLOBAL_ActiveLevel.currentRoomID = 1;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;

    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; ++i) {
        memset(&GLOBAL_OBJList[i], 0, sizeof(GLOBAL_OBJList[i]));
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    }

    memset(&GLOBAL_PlayerData, 0, sizeof(GLOBAL_PlayerData));
    GLOBAL_PlayerData.objData.eType = OBJ_PLAYER;
    GLOBAL_PlayerData.movingPlatformIndex = -1;

    draw_call_count = 0;
    nametable_update_count = 0;
    GLOBAL_FrameCount = 0;
}

static void set_player_touching(bool touching)
{
    if (touching) {
        GLOBAL_PlayerData.objData.pos.x = 44;
        GLOBAL_PlayerData.objData.pos.y = 56;
    } else {
        GLOBAL_PlayerData.objData.pos.x = 0;
        GLOBAL_PlayerData.objData.pos.y = 0;
    }
}

static const DrawCall *last_draw_at_y(uint8_t y)
{
    int i;
    for (i = (int)draw_call_count - 1; i >= 0; --i) {
        if (draw_calls[i].y == y) {
            return &draw_calls[i];
        }
    }
    return NULL;
}

static void reconstruct_monument_line(uint8_t y, char *out, uint8_t len)
{
    uint8_t i;
    memset(out, ' ', len);
    out[len] = '\0';
    for (i = 0; i < draw_call_count; ++i) {
        if (draw_calls[i].y == y &&
            draw_calls[i].x >= 8u &&
            draw_calls[i].text[0] != '\0') {
            uint8_t index = (uint8_t)((draw_calls[i].x - 8u) / 5u);
            if (index < len) {
                out[index] = draw_calls[i].text[0];
            }
        }
    }
}

int main(void)
{
    const uint8_t monumentIndex = 1;
    const char *line0 = "-- celeste mountain --";
    const char *line1 = "this memorial to those";
    const char *line2 = " perished on the climb";
    const char *blank = "                            ";
    const DrawCall *call;
    char reconstructed[25];
    uint8_t i;

    setup();
    GLOBAL_OBJList[monumentIndex].eType = OBJ_MONUMENT;
    GLOBAL_OBJList[monumentIndex].pos.x = 40;
    GLOBAL_OBJList[monumentIndex].pos.y = 48;
    monumentInit(monumentIndex);

    check_u8("monument_init_updates_nametable", nametable_update_count, 1);
    check_u8("monument_tile", GLOBAL_OBJList[monumentIndex].oamTile, MONUMENT_SPRITE_1);
    check_u8("monument_extra_slot_unused", GLOBAL_OBJList[monumentIndex].extraSpriteBase, PORT_EXTRA_SLOT_UNUSED);

    set_player_touching(false);
    GLOBAL_FrameCount = 0;
    monumentUpdate(monumentIndex);
    check_u8("no_draw_when_not_touching", draw_call_count, 0);

    set_player_touching(true);
    GLOBAL_FrameCount = 1;
    monumentUpdate(monumentIndex);
    check_u8("no_draw_between_text_ticks", draw_call_count, 0);

    GLOBAL_FrameCount = 3;
    monumentUpdate(monumentIndex);
    check_u8("first_touch_draw_count", draw_call_count, 1);
    check_u8("first_touch_x", draw_calls[0].x, 8);
    check_u8("first_touch_y", draw_calls[0].y, 80);
    check_str("first_touch_first_char", draw_calls[0].text, "-");

    for (i = 0; i < 180; ++i) {
        GLOBAL_FrameCount = (uint16_t)(GLOBAL_FrameCount + 2u);
        monumentUpdate(monumentIndex);
    }

    call = last_draw_at_y(80);
    check_true("line0_was_drawn", call != NULL);
    reconstruct_monument_line(80, reconstructed, (uint8_t)strlen(line0));
    check_str("line0_text", reconstructed, line0);

    call = last_draw_at_y(87);
    check_true("line1_was_drawn", call != NULL);
    reconstruct_monument_line(87, reconstructed, (uint8_t)strlen(line1));
    check_str("line1_text", reconstructed, line1);

    call = last_draw_at_y(94);
    check_true("line2_was_drawn", call != NULL);
    reconstruct_monument_line(94, reconstructed, (uint8_t)strlen(line2));
    check_str("line2_text", reconstructed, line2);

    {
        uint8_t beforeLeave = draw_call_count;
        set_player_touching(false);
        monumentUpdate(monumentIndex);
        check_u8("leave_adds_three_clear_draws", (uint8_t)(draw_call_count - beforeLeave), 3);
        check_str("leave_clear_line0", draw_calls[beforeLeave].text, blank);
        check_u8("leave_clear_line0_y", draw_calls[beforeLeave].y, 80);
        check_str("leave_clear_line1", draw_calls[beforeLeave + 1u].text, blank);
        check_u8("leave_clear_line1_y", draw_calls[beforeLeave + 1u].y, 87);
        check_str("leave_clear_line2", draw_calls[beforeLeave + 2u].text, blank);
        check_u8("leave_clear_line2_y", draw_calls[beforeLeave + 2u].y, 94);
    }

    {
        uint8_t beforeRetouch = draw_call_count;
        set_player_touching(true);
        GLOBAL_FrameCount = (uint16_t)(GLOBAL_FrameCount + 2u);
        monumentUpdate(monumentIndex);
        check_u8("retouch_waits_one_object_tick", (uint8_t)(draw_call_count - beforeRetouch), 0);
        GLOBAL_FrameCount = (uint16_t)(GLOBAL_FrameCount + 2u);
        monumentUpdate(monumentIndex);
        check_u8("retouch_restarts_typing", (uint8_t)(draw_call_count - beforeRetouch), 1);
        check_str("retouch_first_char", draw_calls[beforeRetouch].text, "-");
    }

    printf("Pass: %d  Fail: %d\n", pass_count, fail_count);
    return fail_count == 0 ? 0 : 1;
}
