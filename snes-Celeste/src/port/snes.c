#include "port.h"


#include "../../shared/src/snes_regs_xc.h"
#include "../../shared/src/initsnes.h"
//Thank you llvm mos
//making me micro manage where my graphics data in ROM
#ifdef __SNESXC_16BIT_POINTERS__
#pragma clang section rodata="rom_bank_1"
#endif
#include "../clouds.h"
#include "../sprite_data.h"
#include "../snes_font.h"
#include <string.h>

//Level data, ideally place these ocntiguiously in the same bank
//Each one is 2347 bytes pre object data
//32 levels, 2347 * 32 = 75104 bytes
//So it'll need to be split into 3 banks
//13 ish levels per bank
#pragma SECTION CONST=CEL_K_00
#ifdef __SNESXC_16BIT_POINTERS__
#pragma clang section rodata="rom_bank_2"
#endif
#include "../levelDat/tilemap_level1.h"
#include "../levelDat/tilemap_level2.h"
#include "../levelDat/tilemap_level3.h"
#include "../levelDat/tilemap_level4.h"
#include "../levelDat/tilemap_level5.h"
#include "../levelDat/tilemap_level6.h"
#include "../levelDat/tilemap_level7.h"
#include "../levelDat/tilemap_level8.h"
#include "../levelDat/tilemap_level9.h"
#include "../levelDat/tilemap_level10.h"
#include "../levelDat/tilemap_level11.h"
#include "../levelDat/tilemap_level12.h"
#pragma SECTION CONST=CEL_K_01
#ifdef __SNESXC_16BIT_POINTERS__
#pragma clang section rodata="rom_bank_3"
#endif
#include "../levelDat/tilemap_level13.h"
#include "../levelDat/tilemap_level14.h"
#include "../levelDat/tilemap_level15.h"
#include "../levelDat/tilemap_level16.h"
#include "../levelDat/tilemap_level17.h"
#include "../levelDat/tilemap_level18.h"
#include "../levelDat/tilemap_level19.h"
#include "../levelDat/tilemap_level20.h"
#include "../levelDat/tilemap_level21.h"
#include "../levelDat/tilemap_level22.h"
#include "../levelDat/tilemap_level23.h"
#include "../levelDat/tilemap_level24.h"
#pragma SECTION CONST=CEL_K_02
#ifdef __SNESXC_16BIT_POINTERS__
#pragma clang section rodata="rom_bank_4"
#endif
#include "../levelDat/tilemap_level25.h"
#include "../levelDat/tilemap_level26.h"
#include "../levelDat/tilemap_level27.h"
#include "../levelDat/tilemap_level28.h"
#include "../levelDat/tilemap_level29.h"
#include "../levelDat/tilemap_level30.h"
#include "../levelDat/tilemap_level31.h"
#include "../levelDat/tilemap_level32.h"
#ifdef __SNESXC_16BIT_POINTERS__
#pragma clang section rodata="rom_bank_0"
#endif
#pragma SECTION CONST=CONST

extern struct sPlayerData GLOBAL_PlayerData;
//Compiles without this, linter just gets confused
extern const unsigned short clouds_palette_2[4];
uint8_t paletteBg[64];


union uOAMCopy GLOBAL_OAMCopy;
extern uint16_t GLOBAL_ScrollBG2Y;
extern OBJ_DATA GLOBAL_OBJList[];

// BG1 tilemap backing store (palette handled in port layer)
uint8_t GLBOAL_M0BG1TileMap[32*32];

//Global data used to update hardware registers
uint16_t GLOBAL_ScrollBG1X = 0;
uint16_t GLOBAL_ScrollBG1Y = 0;

uint16_t GLOBAL_ScrollBG2X = 0;
uint16_t GLOBAL_ScrollBG2Y = 0;

uint16_t GLOBAL_ScrollBG3X = 0;
uint16_t GLOBAL_ScrollBG3Y = 0;

uint16_t GLOBAL_ScrollBG4X = 0;
uint16_t GLOBAL_ScrollBG4Y = 0;

static uint16_t s_tilemapBg2[512];
static uint16_t s_tilemapBg3[512];

uint8_t GLOBAL_InputLo;
uint8_t GLOBAL_InputHi;

uint8_t port_getInputs(void)
{
    uint8_t rawLo;
    uint8_t rawHi;
    uint8_t buttons = 0;

    (void)REG_HVBJOY; // Latch controller state
    rawLo = REG_JOY1L;
    rawHi = REG_JOY1H;

    GLOBAL_InputLo = rawLo;
    GLOBAL_InputHi = rawHi;

    if (rawHi & PORT_INPUT_RIGHT_MASK) {
        buttons |= PORT_INPUT_RIGHT_MASK;
    }
    if (rawHi & PORT_INPUT_LEFT_MASK) {
        buttons |= PORT_INPUT_LEFT_MASK;
    }
    if (rawHi & PORT_INPUT_DOWN_MASK) {
        buttons |= PORT_INPUT_DOWN_MASK;
    }
    if (rawHi & PORT_INPUT_UP_MASK) {
        buttons |= PORT_INPUT_UP_MASK;
    }
    if (rawHi & PORT_INPUT_START_MASK) {
        buttons |= PORT_INPUT_START_MASK;
    }
    if (rawHi & PORT_INPUT_SELECT_MASK) {
        buttons |= PORT_INPUT_SELECT_MASK;
    }
    if (rawHi & PORT_INPUT_Y_MASK) {
        buttons |= PORT_INPUT_Y_MASK;
    }
    if (rawHi & PORT_INPUT_B_MASK) {
        buttons |= PORT_INPUT_B_MASK;
    }

    return buttons;
}

static const uint8_t s_bg1PaletteDefault[16] = {
    0x00, 0x00,
    0xFF, 0x7F,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00
};

static const uint8_t s_bg1PaletteDoubleDash[16] = {
    0x90, 0x28,
    0xFF, 0x7F,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00
};

static uint8_t s_bg1PaletteCurrent[16];
static bool s_altPaletteApplied = false;
static bool s_prevTextFlashActive = false;
static uint8_t s_textFlashPhase = 0;

#define PORT_EXTRA_SPRITE_START 30u
#define PORT_BREAKABLE_WALL_EXTRAS_PER_OBJECT 3u
#define PORT_BALLOON_EXTRA_SPRITES 1u
#define PORT_PLATMOV_EXTRA_SPRITES 2u
#define PORT_PLATMOV_RIGHT_OFFSET 16
#define PORT_PLATMOV_TILE_LEFT 0x24u
#define PORT_PLATMOV_TILE_RIGHT 0x26u
#define PORT_FLYING_BERRY_EXTRA_SPRITES 2u
#define PORT_FLYING_BERRY_WING_OFFSET_X 14
#define PORT_FLYING_BERRY_WING_OFFSET_Y 4
#define PORT_FLYING_BERRY_WING_PROPERTIES_LEFT 0x34u
#define PORT_FLYING_BERRY_WING_PROPERTIES_RIGHT 0x74u
#define PORT_FLYING_BERRY_WING_TILE_UP 0x68u
#define PORT_FLYING_BERRY_WING_TILE_MID 0x6Au
#define PORT_FLYING_BERRY_WING_TILE_DOWN 0x6Cu
#define PORT_MONUMENT_EXTRA_SPRITES 3u
#define PORT_MONUMENT_OFFSET_RIGHT 16
#define PORT_MONUMENT_OFFSET_DOWN 16
#define PORT_MONUMENT_TILE_RIGHT 0x86u
#define PORT_MONUMENT_TILE_BOTTOM_LEFT 0x88u
#define PORT_MONUMENT_TILE_BOTTOM_RIGHT 0x8Au
#define PORT_MONUMENT_PROP_TOP 0x36u
#define PORT_MONUMENT_PROP_BOTTOM 0x32u
#define PORT_BIG_CHEST_EXTRA_SPRITES 1u
#define PORT_BIG_CHEST_OFFSET_RIGHT 16
#define PORT_BIG_CHEST_TILE_RIGHT 0x8Eu
#define PORT_BIG_CHEST_STATE_IDLE 0u
#define PORT_BIG_CHEST_STATE_OPEN_ANIM 1u
#define PORT_OAM_ENTRY_COUNT 128u

static uint8_t s_nextExtraSprite = PORT_EXTRA_SPRITE_START;
static uint8_t s_maxExtraSpriteUsed = PORT_EXTRA_SPRITE_START;
static uint8_t s_previousMaxExtraForCleanup = PORT_EXTRA_SPRITE_START;

#define CALC_OAM_TABLE2_BYTE(index, sizeBit, xBit, currentByte) \
    (((currentByte) & ~(0x03 << (((index) % 4) * 2))) | ((((sizeBit) << 1) | (xBit)) << (((index) % 4) * 2)))


static bool ensureExtraSpriteRange(uint8_t *baseSlot, uint8_t count)
{
    if (count == 0u) {
        return true;
    }

    uint16_t required;
    if (*baseSlot == PORT_EXTRA_SLOT_UNUSED) {
        uint8_t base = s_nextExtraSprite;
        required = (uint16_t)base + (uint16_t)count;
        if (required > PORT_OAM_ENTRY_COUNT) {
            return false;
        }
        *baseSlot = base;
        s_nextExtraSprite = (uint8_t)required;
    } else {
        required = (uint16_t)(*baseSlot) + (uint16_t)count;
        if (required > PORT_OAM_ENTRY_COUNT) {
            return false;
        }
        if ((uint8_t)required > s_nextExtraSprite) {
            s_nextExtraSprite = (uint8_t)required;
        }
    }

    if (s_nextExtraSprite > s_maxExtraSpriteUsed) {
        s_maxExtraSpriteUsed = s_nextExtraSprite;
    }
    return true;
}

static void releaseExtraSpriteRange(uint8_t baseSlot, uint8_t count)
{
    if (baseSlot == PORT_EXTRA_SLOT_UNUSED || count == 0u) {
        return;
    }

    uint16_t end = (uint16_t)baseSlot + (uint16_t)count;
    if (end > PORT_OAM_ENTRY_COUNT) {
        end = PORT_OAM_ENTRY_COUNT;
    }
    for (uint16_t slot = baseSlot; slot < end; ++slot) {
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = 240;
    }

    if ((uint8_t)end == s_nextExtraSprite) {
        s_nextExtraSprite = baseSlot;
    }
    if ((uint8_t)end == s_maxExtraSpriteUsed) {
        s_maxExtraSpriteUsed = baseSlot;
    }
}

enum {
    PORT_COLLAPSE_TILE_STATE_HIDDEN = 2
};

enum {
    PORT_BALLOON_STATE_IDLE = 0,
    PORT_BALLOON_STATE_POPPED = 1
};

static uint8_t calcOAMTable2Byte(uint8_t index, uint8_t sizeBit, uint8_t xBit, uint8_t currentByte)
{
    uint8_t shift = (uint8_t)((index % 4u) * 2u);
    uint8_t mask = (uint8_t)~(uint8_t)(0x03u << shift);
    uint8_t value = (uint8_t)(((sizeBit << 1) | xBit) << shift);
    return (uint8_t)((currentByte & mask) | value);
}

static void writeStandardSprite(uint8_t index, const OBJ_DATA *obj)
{
    uint8_t table2Index = (uint8_t)(index / 4u);
    uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
    int16_t screenY = (int16_t)(obj->pos.y - (int16_t)GLOBAL_ScrollBG2Y);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(index, 1u, 0u, currentByte);
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = (uint8_t)obj->pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = (uint8_t)screenY;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = obj->oamTile;
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = obj->oamProps;
}

static void writeConditionalSprite(uint8_t index, const OBJ_DATA *obj, bool hide)
{
    uint8_t table2Index = (uint8_t)(index / 4u);
    uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(index, 1u, 0u, currentByte);
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = (uint8_t)obj->pos.x;
    if (hide) {
        GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
    } else {
        int16_t screenY = (int16_t)(obj->pos.y - (int16_t)GLOBAL_ScrollBG2Y);
        GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = (uint8_t)screenY;
    }
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = obj->oamTile;
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = obj->oamProps;
}

static bool writeBreakableWallSprite(uint8_t index, OBJ_DATA *obj)
{
    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, PORT_BREAKABLE_WALL_EXTRAS_PER_OBJECT)) {
        return false;
    }

    writeStandardSprite(index, obj);

    obj->extraSpriteCount = PORT_BREAKABLE_WALL_EXTRAS_PER_OBJECT;
    uint8_t extraBase = obj->extraSpriteBase;
    for (uint8_t extra = 0; extra < PORT_BREAKABLE_WALL_EXTRAS_PER_OBJECT; ++extra) {
        uint8_t slot = (uint8_t)(extraBase + extra);
        if (slot >= PORT_OAM_ENTRY_COUNT) {
            break;
        }
        uint8_t extraTable2Index = (uint8_t)(slot / 4u);
        uint8_t extraCurrent = GLOBAL_OAMCopy.arr.OAMTable2[extraTable2Index];
        GLOBAL_OAMCopy.arr.OAMTable2[extraTable2Index] = calcOAMTable2Byte(slot, 1u, 0u, extraCurrent);
        uint8_t offsetX = (extra == 2u) ? 0u : 16u;
        uint8_t offsetY = (extra == 1u) ? 0u : 16u;
        uint8_t propMask = (offsetX != 0u ? 0x40u : 0u) | (offsetY != 0u ? 0x80u : 0u);
        GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = obj->oamTile;
        GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = (uint8_t)(obj->oamProps | propMask);
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = (uint8_t)((uint16_t)obj->pos.x + offsetX);
        int16_t extraScreenY = (int16_t)((int16_t)obj->pos.y + offsetY - (int16_t)GLOBAL_ScrollBG2Y);
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)extraScreenY;
    }

    return true;
}

static bool writeMonumentSprite(uint8_t index, OBJ_DATA *obj)
{
    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, PORT_MONUMENT_EXTRA_SPRITES)) {
        return false;
    }

    writeStandardSprite(index, obj);

    obj->extraSpriteCount = PORT_MONUMENT_EXTRA_SPRITES;
    uint8_t baseSlot = obj->extraSpriteBase;
    static const uint8_t kTiles[PORT_MONUMENT_EXTRA_SPRITES] = {
        PORT_MONUMENT_TILE_RIGHT,
        PORT_MONUMENT_TILE_BOTTOM_LEFT,
        PORT_MONUMENT_TILE_BOTTOM_RIGHT
    };
    static const uint8_t kProps[PORT_MONUMENT_EXTRA_SPRITES] = {
        PORT_MONUMENT_PROP_TOP,
        PORT_MONUMENT_PROP_BOTTOM,
        PORT_MONUMENT_PROP_BOTTOM
    };
    static const int8_t kOffsetsX[PORT_MONUMENT_EXTRA_SPRITES] = {
        PORT_MONUMENT_OFFSET_RIGHT,
        0,
        PORT_MONUMENT_OFFSET_RIGHT
    };
    static const int8_t kOffsetsY[PORT_MONUMENT_EXTRA_SPRITES] = {
        0,
        PORT_MONUMENT_OFFSET_DOWN,
        PORT_MONUMENT_OFFSET_DOWN
    };

    for (uint8_t extra = 0; extra < PORT_MONUMENT_EXTRA_SPRITES; ++extra) {
        uint8_t slot = (uint8_t)(baseSlot + extra);
        if (slot >= PORT_OAM_ENTRY_COUNT) {
            break;
        }
        uint8_t table2Index = (uint8_t)(slot / 4u);
        uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
        GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(slot, 1u, 0u, currentByte);

        int16_t spriteX = (int16_t)obj->pos.x + kOffsetsX[extra];
        int16_t spriteY = (int16_t)obj->pos.y + kOffsetsY[extra] - (int16_t)GLOBAL_ScrollBG2Y;
        GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = kTiles[extra];
        GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = kProps[extra];
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = (uint8_t)spriteX;
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)spriteY;
    }

    return true;
}

static bool writeBigChestSprite(uint8_t index, OBJ_DATA *obj)
{
    bool hide = (obj->data.bigChest.state != PORT_BIG_CHEST_STATE_IDLE);

    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, PORT_BIG_CHEST_EXTRA_SPRITES)) {
        writeConditionalSprite(index, obj, hide);
        return false;
    }

    writeConditionalSprite(index, obj, hide);

    obj->extraSpriteCount = PORT_BIG_CHEST_EXTRA_SPRITES;
    uint8_t slot = obj->extraSpriteBase;
    uint8_t table2Index = (uint8_t)(slot / 4u);
    uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
    uint16_t rightXFull = (uint16_t)((uint16_t)obj->pos.x + PORT_BIG_CHEST_OFFSET_RIGHT);
    uint8_t xBit = (rightXFull >= 256u) ? 1u : 0u;
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(slot, 1u, xBit, currentByte);

    GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = PORT_BIG_CHEST_TILE_RIGHT;
    GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = obj->oamProps;
    GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = (uint8_t)rightXFull;
    if (hide) {
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = 240;
    } else {
        int16_t screenY = (int16_t)(obj->pos.y - (int16_t)GLOBAL_ScrollBG2Y);
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)screenY;
    }

    return true;
}

static bool writeBalloonSprite(uint8_t index, OBJ_DATA *obj)
{
    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, PORT_BALLOON_EXTRA_SPRITES)) {
        return false;
    }

    // Hide balloon and string if popped or during hide frame count
    bool hideMain = (obj->data.balloon.state == PORT_BALLOON_STATE_POPPED) || (obj->data.balloon.hideFrameCount > 0);
    writeConditionalSprite(index, obj, hideMain);

    obj->extraSpriteCount = PORT_BALLOON_EXTRA_SPRITES;
    uint8_t slot = obj->extraSpriteBase;
    uint8_t table2Index = (uint8_t)(slot / 4u);
    uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(slot, 1u, 0u, currentByte);
    GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = (uint8_t)obj->pos.x;
    if (hideMain) {
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = 240;
    } else {
        int16_t stringY = (int16_t)((int16_t)obj->pos.y + 14 + (int16_t)obj->data.balloon.spriteYOffset - (int16_t)GLOBAL_ScrollBG2Y);
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)stringY;
    }
    GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = obj->data.balloon.stringTile;
    // String uses palette 2 (priority 3, palette 2)
    GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = 0x32;

    return true;
}

static bool computePlatMovWrap(const OBJ_DATA *obj, int16_t *outX, uint8_t *outTile, bool *outHide)
{
    int16_t baseX = (int16_t)((uint8_t)obj->pos.x);

    if (!obj->data.platMov.isMovingLeft) {
        if (baseX > 240) {
            *outX = baseX - 256;
            *outTile = PORT_PLATMOV_TILE_LEFT;
            *outHide = false;
            return true;
        } else if ((baseX + PORT_PLATMOV_RIGHT_OFFSET) > 240) {
            *outX = baseX - 256 + PORT_PLATMOV_RIGHT_OFFSET;
            *outTile = PORT_PLATMOV_TILE_RIGHT;
            *outHide = false;
            return true;
        }
    } else {
        if (baseX > 240) {
            *outX = baseX + 256;
            *outTile = PORT_PLATMOV_TILE_LEFT;
            *outHide = false;
            return true;
        } else if (baseX > 224) {
            *outX = baseX + 256 + PORT_PLATMOV_RIGHT_OFFSET;
            *outTile = PORT_PLATMOV_TILE_RIGHT;
            *outHide = false;
            return true;
        }
    }

    *outHide = true;
    *outX = 0;
    *outTile = PORT_PLATMOV_TILE_LEFT;
    return false;
}

static bool writePlatMovSprite(uint8_t index, OBJ_DATA *obj)
{
    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, PORT_PLATMOV_EXTRA_SPRITES)) {
        return false;
    }

    writeStandardSprite(index, obj);

    obj->extraSpriteCount = PORT_PLATMOV_EXTRA_SPRITES;

    uint8_t rightSlot = obj->extraSpriteBase;
    uint8_t wrapSlot = (uint8_t)(rightSlot + 1u);

    int16_t screenY = (int16_t)(obj->pos.y - (int16_t)GLOBAL_ScrollBG2Y);

    uint16_t rightXFull = (uint16_t)((uint16_t)(uint8_t)obj->pos.x + PORT_PLATMOV_RIGHT_OFFSET);
    uint8_t rightXBit = 0u;
    uint8_t rightX = (uint8_t)rightXFull;
    uint8_t rightTable2Index = (uint8_t)(rightSlot / 4u);
    uint8_t rightCurrent = GLOBAL_OAMCopy.arr.OAMTable2[rightTable2Index];
    GLOBAL_OAMCopy.arr.OAMTable2[rightTable2Index] = calcOAMTable2Byte(rightSlot, 1u, rightXBit, rightCurrent);
    GLOBAL_OAMCopy.arr.OAMArray[rightSlot].OBJX = rightX;
    GLOBAL_OAMCopy.arr.OAMArray[rightSlot].OBJY = (uint8_t)screenY;
    GLOBAL_OAMCopy.arr.OAMArray[rightSlot].CHARNUM = PORT_PLATMOV_TILE_RIGHT;
    GLOBAL_OAMCopy.arr.OAMArray[rightSlot].PROPERTIES = obj->oamProps;

    int16_t wrapX;
    uint8_t wrapTile;
    bool hideWrap;
    computePlatMovWrap(obj, &wrapX, &wrapTile, &hideWrap);

    uint8_t wrapTable2Index = (uint8_t)(wrapSlot / 4u);
    uint8_t wrapCurrent = GLOBAL_OAMCopy.arr.OAMTable2[wrapTable2Index];
    uint8_t wrapXBit = (wrapX < 0 || wrapX >= 256) ? 1u : 0u;
    GLOBAL_OAMCopy.arr.OAMTable2[wrapTable2Index] = calcOAMTable2Byte(wrapSlot, 1u, wrapXBit, wrapCurrent);
    GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].CHARNUM = wrapTile;
    GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].PROPERTIES = obj->oamProps;
    if (hideWrap) {
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].OBJY = 240;
    } else {
        uint8_t wrapXByte = (uint8_t)wrapX;
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].OBJX = wrapXByte;
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].OBJY = (uint8_t)screenY;
    }

    return true;
}

static bool writeFlyingBerrySprite(uint8_t index, OBJ_DATA *obj)
{
    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, PORT_FLYING_BERRY_EXTRA_SPRITES)) {
        return false;
    }

    bool hideMain = (obj->data.strawberry.isCollected != 0u);
    writeConditionalSprite(index, obj, hideMain);

    obj->extraSpriteCount = PORT_FLYING_BERRY_EXTRA_SPRITES;
    uint8_t baseSlot = obj->extraSpriteBase;
    uint8_t wingTile = PORT_FLYING_BERRY_WING_TILE_MID;
    int16_t delta = (int16_t)obj->pos.y - (int16_t)obj->data.strawberry.startY;
    if (delta < 0) {
        wingTile = PORT_FLYING_BERRY_WING_TILE_UP;
    } else if (delta > 0) {
        wingTile = PORT_FLYING_BERRY_WING_TILE_DOWN;
    }

    for (uint8_t offset = 0; offset < PORT_FLYING_BERRY_EXTRA_SPRITES; ++offset) {
        uint8_t slot = (uint8_t)(baseSlot + offset);
        if (slot >= PORT_OAM_ENTRY_COUNT) {
            break;
        }
        uint8_t table2Index = (uint8_t)(slot / 4u);
        uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
        GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(slot, 1u, 0u, currentByte);

        uint8_t properties = (offset == 0u) ? PORT_FLYING_BERRY_WING_PROPERTIES_LEFT
                                            : PORT_FLYING_BERRY_WING_PROPERTIES_RIGHT;
        int16_t xOffset = (offset == 0u) ? PORT_FLYING_BERRY_WING_OFFSET_X
                                         : -(int16_t)PORT_FLYING_BERRY_WING_OFFSET_X;
        int16_t wingX = (int16_t)obj->pos.x + xOffset;
        int16_t wingY = (int16_t)obj->pos.y - PORT_FLYING_BERRY_WING_OFFSET_Y - (int16_t)GLOBAL_ScrollBG2Y;

        GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = wingTile;
        GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = properties;
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = (uint8_t)wingX;
        if (hideMain) {
            GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = 240;
        } else {
            GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)wingY;
        }
    }

    return true;
}

//Bank number macros
#define BANK_00  0  // Default/current bank (where code runs)
#define BANK_01  1  // rom_bank_1 (fonts, sprites, clouds)
#define BANK_02  2  // rom_bank_2 (levels 1-12)
#define BANK_03  3  // rom_bank_3 (levels 13-24)
#define BANK_04  4  // rom_bank_4 (levels 25-32)

static void LoadRoomDataVRAM(void) {
    LoadVram((uint8_t *)s_tilemapBg2, 0x2000, GLOBAL_ActiveLevel.roomSizeX * GLOBAL_ActiveLevel.roomSizeY * 4); //Bg2 tilemap
    LoadVram((uint8_t *)s_tilemapBg3, 0x4000, GLOBAL_ActiveLevel.roomSizeX * GLOBAL_ActiveLevel.roomSizeY * 4); //Bg2 tilemap
    LoadCGRam(paletteBg, 0x0020, 0x40); //Bg2 palette
    LoadCGRam(paletteBg, 0x0040, 0x40); //Bg3 palette
}

void port_LoadRoomData(uint16_t roomID) {
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = false;
    REG_INIDISP = 0x8F;

    if (roomID >= 1 && roomID <= 12) {
        snesXC_setDataBank(BANK_02);
    } else if (roomID >= 13 && roomID <= 24) {
        snesXC_setDataBank(BANK_03);
    } else if (roomID >= 25 && roomID <= 32) {
        snesXC_setDataBank(BANK_04);
    }

    switch (roomID) {
        case 1:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level1_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level1_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level1, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level1, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL1;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL1;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL1_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level1, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 2:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level2_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level2_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level2, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level2, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL2;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL2;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL2_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level2, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 3:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level3_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level3_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level3, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level3, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL3;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL3;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL3_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level3, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 4:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level4_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level4_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level4, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level4, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL4;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL4;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL4_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level4, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 5:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level5_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level5_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level5, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level5, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL5;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL5;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL5_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level5, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 6:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level6_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level6_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level6, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level6, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL6;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL6;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL6_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level6, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 7:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level7_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level7_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level7, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level7, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL7;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL7;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL7_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level7, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 8:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level8_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level8_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level8, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level8, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL8;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL8;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL8_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level8, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 9:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level9_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level9_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level9, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level9, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL9;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL9;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL9_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level9, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 10:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level10_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level10_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level10, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level10, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL10;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL10;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL10_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level10, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 11:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level11_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level11_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level11, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level11, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL11;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL11;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL11_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level11, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 12:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level12_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level12_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level12, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level12, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL12;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL12;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL12_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level12, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 13:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level13_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level13_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level13, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level13, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL13;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL13;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL13_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level13, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 14:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level14_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level14_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level14, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level14, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL14;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL14;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL14_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level14, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 15:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level15_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level15_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level15, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level15, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL15;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL15;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL15_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level15, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 16:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level16_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level16_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level16, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level16, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL16;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL16;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL16_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level16, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 17:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level17_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level17_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level17, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level17, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL17;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL17;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL17_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level17, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 18:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level18_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level18_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level18, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level18, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL18;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL18;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL18_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level18, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 19:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level19_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level19_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level19, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level19, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL19;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL19;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL19_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level19, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 20:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level20_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level20_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level20, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level20, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL20;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL20;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL20_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level20, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 21:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level21_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level21_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level21, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level21, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL21;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL21;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL21_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level21, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 22:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level22_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level22_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level22, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level22, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL22;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL22;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL22_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level22, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 23:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level23_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level23_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level23, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level23, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL23;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL23;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL23_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level23, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 24:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level24_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level24_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level24, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level24, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL24;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL24;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL24_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level24, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 25:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level25_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level25_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level25, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level25, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL25;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL25;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL25_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level25, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 26:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level26_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level26_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level26, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level26, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL26;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL26;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL26_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level26, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 27:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level27_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level27_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level27, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level27, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL27;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL27;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL27_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level27, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 28:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level28_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level28_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level28, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level28, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL28;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL28;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL28_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level28, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 29:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level29_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level29_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level29, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level29, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL29;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL29;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL29_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level29, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 30:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level30_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level30_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level30, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level30, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL30;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL30;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL30_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level30, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 31:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level31_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level31_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level31, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level31, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL31;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL31;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL31_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level31, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 32:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level32_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level32_bg3, sizeof(s_tilemapBg3));
            snesXC_memcpy_banked(paletteBg, palette_level32, sizeof(paletteBg));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level32, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL32;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL32;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL32_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level32, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        default:
            break;
    }

    GLOBAL_ActiveLevel.scrollPointY = 72;
    snesXC_setDataBank(BANK_00);
    LoadRoomDataVRAM();
    REG_INIDISP = 0x0F;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
}

// Load all initial graphics data to VRAM/CGRAM
// This function handles all the one-time graphics setup
void LoadInitialGraphics(void) {
    snesXC_setDataBank(BANK_01);
    // Initialize BG1 tilemap and palette (in bank 0 - no switching needed)
    for (uint16_t i = 0; i < sizeof(GLBOAL_M0BG1TileMap); i++) {
        GLBOAL_M0BG1TileMap[i] = 0x20;
    }
    s_altPaletteApplied = false;
    s_prevTextFlashActive = false;
    s_textFlashPhase = 0;
    memcpy(s_bg1PaletteCurrent, s_bg1PaletteDefault, sizeof(s_bg1PaletteCurrent));
    
    // Load BG1 font and tilemap (from rom_bank_1)
    LoadLoVram(SNESFONT_bin, 0xE000, sizeof(SNESFONT_bin));
    
    LoadLoVram(GLBOAL_M0BG1TileMap, 0xF800, sizeof(GLBOAL_M0BG1TileMap));
    LoadCGRam((char *)s_bg1PaletteCurrent, 0x0000, sizeof(s_bg1PaletteCurrent));
    
    // Load cloud graphics (from rom_bank_1)
    LoadVram(clouds_tiles, 0xC000, sizeof(clouds_tiles));
    LoadCGRam((char *)clouds_palette, 0x0060, sizeof(clouds_palette));
    LoadVram((char *)clouds_map, 0x0000, sizeof(clouds_map));
    
    // Load sprite graphics (from rom_bank_1)
    LoadVram(sprite_gfx_4bpp, 0x8000, sizeof(sprite_gfx_4bpp));
    LoadVram(sprite_gfx_2bpp, 0xA000, sizeof(sprite_gfx_2bpp));
    
    // Load all sprite palettes (from rom_bank_1)
    LoadCGRam((char *)sprite_palettes_4bpp[0], 0x0080, sizeof(sprite_palettes_4bpp[0])); // Player
    LoadCGRam((char *)sprite_palettes_4bpp[0], 0x0090, sizeof(sprite_palettes_4bpp[0])); // Smoke
    LoadCGRam((char *)sprite_palettes_4bpp[1], 0x00A0, sizeof(sprite_palettes_4bpp[0])); // Breakable wall
    LoadCGRam((char *)sprite_palettes_4bpp[2], 0x00B0, sizeof(sprite_palettes_4bpp[0])); // Flower
    LoadCGRam((char *)sprite_palettes_4bpp[3], 0x00C0, sizeof(sprite_palettes_4bpp[0])); // Strawberry
    LoadCGRam((char *)sprite_palettes_4bpp[4], 0x00D0, sizeof(sprite_palettes_4bpp[0])); // Deco tree
    LoadCGRam((char *)sprite_palettes_4bpp[5], 0x00E0, sizeof(sprite_palettes_4bpp[0])); // Spring
    LoadCGRam((char *)sprite_palettes_4bpp[6], 0x00F0, sizeof(sprite_palettes_4bpp[0])); // Flying berry
    snesXC_setDataBank(BANK_00);
}


void port_init(void)
{
    uint8_t regWrite1;
    initSNES(SLOWROM);
	initOAMCopy(GLOBAL_OAMCopy.Bytes);

    REG_CGADD = 0;
    REG_CGDATA = 0x00;
    REG_CGDATA = 0x00;

	REG_BGMODE  = 0x00;
    
    REG_BG1SC  = 0xFC;
	REG_BG1HOFS = 0x00;
	REG_BG1HOFS = 0x00;
	REG_BG1VOFS = 0x00;
	REG_BG1VOFS = 0x00;
    regWrite1 = 0x00; //8x8 or 16x16
    regWrite1 = regWrite1 | VRAM_ADD_TO_OBSEL_VALUE(0x8000); 
    REG_OBSEL = regWrite1; 
    LoadInitialGraphics();

    //Set background 4 to 16x16 tiles
    //Set background 3 to 16x16 tiles
    //Set background 2 to 16x16 tiles
    //Set background 1 to 8x8 tiles
    REG_BGMODE = 0xE0;
    //Set background 2 tilemap source address to 0x2000 with single tilemap (YX = 00)
    REG_BG2SC = (0x2000ul >> (9)) | 0x00u;
    REG_BG12NBA = (((0xA000 >> 13) << 4) & 0xF0) | ((0xE000 >> 13) & 0x0F);

    
    //Set background 3 tilemap source address to 0x4000
    REG_BG3SC = (0x4000ul >> (9)) | 0x00u;
    //Set background 4 tilemap source address to 0x0000 and size to bigXY
    REG_BG4SC = (0x0000ul >> (9)) | 0x03u;
    //Set background 3 tile data source address to 0xA000
    REG_BG34NBA = ((0xC000 >> 13) << 4) | ((0xA000 >> 13));
    REG_TM = 0x1F; //Enable background 4, 3, 2, 1 and OAM/sprite only


    //REG_NMITIMEN = 0x81; // Enable VBlank interrupt (bit 7) and auto-joypad read (bit 0)
    REG_NMITIMEN = 0x01; // Enable auto-joypad read (bit 0)
    REG_INIDISP = 0x0F;
    
}

void port_beginSpriteBuild(const struct sPlayerData *playerObj)
{
    s_previousMaxExtraForCleanup = s_maxExtraSpriteUsed;

    (void)playerObj;
}

void port_finishSpriteBuild(void)
{
    for (uint8_t slot = s_nextExtraSprite; slot < s_previousMaxExtraForCleanup; ++slot) {
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = 240;
    }
    s_maxExtraSpriteUsed = s_nextExtraSprite;
}

void port_updatePlayerSprite(const struct sPlayerData *playerObj)
{
    if (playerObj == 0) {
        return;
    }
    const struct sOBJ_DATA *playerData = &playerObj->objData;
    int16_t screenY = (int16_t)(playerData->pos.y - (int16_t)GLOBAL_ScrollBG2Y);
    uint8_t table2Index = 0u; // Sprite 0 is at index 0
    uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
    // Calculate X high bit (bit 8 of X coordinate)
    uint8_t xBit = (uint8_t)((playerData->pos.x >= 256u) ? 1u : 0u);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(0u, 1u, xBit, currentByte);
    // Write directly to bytes array to avoid vbcc65816 struct layout issues
    // SNES OAM format: byte 0 = X, byte 1 = Y, byte 2 = Tile, byte 3 = Properties
    GLOBAL_OAMCopy.Bytes[0] = (uint8_t)playerData->pos.x;  // X position (low 8 bits)
    GLOBAL_OAMCopy.Bytes[1] = (uint8_t)screenY;           // Y position
    GLOBAL_OAMCopy.Bytes[2] = playerData->oamTile;        // Tile number
    GLOBAL_OAMCopy.Bytes[3] = playerData->oamProps;      // Properties
}

void port_buildUnused(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];

    if (obj->extraSpriteBase != PORT_EXTRA_SLOT_UNUSED && obj->extraSpriteCount != 0u) {
        releaseExtraSpriteRange(obj->extraSpriteBase, obj->extraSpriteCount);
    }
    obj->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    obj->extraSpriteCount = 0u;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildSmoke(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    writeStandardSprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildBreakableWall(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    (void)writeBreakableWallSprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildBalloon(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    (void)writeBalloonSprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildMonument(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    (void)writeMonumentSprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildBigChest(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    (void)writeBigChestSprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildChest(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    writeStandardSprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildKey(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    writeStandardSprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildSpring(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    writeConditionalSprite(index, obj, obj->data.spring.isDisabled);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildCollapseTile(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    bool hide = (obj->data.collapseTile.state == PORT_COLLAPSE_TILE_STATE_HIDDEN);
    writeConditionalSprite(index, obj, hide);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_updateCollapseTileNametable(uint8_t index)
{
    // SNES version doesn't need nametable updates for collapse tiles
    (void)index;
}

void port_buildStrawberry(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    bool hide = (obj->data.strawberry.isCollected != 0u);
    writeConditionalSprite(index, obj, hide);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildPlatMov(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    (void)writePlatMovSprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildFlyingBerry(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    (void)writeFlyingBerrySprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildDoubleDashOrb(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    writeStandardSprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildStaticDecor(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    writeStandardSprite(index, obj);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildSpriteIfDirty(uint8_t index, enum eOBJType eType)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    if ((obj->flags & OBJ_FLAG_DIRTY) == 0U) {
        return;
    }
    if (index == 0U) {
        obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
        return;
    }
    if (eType == OBJ_UNUSED) {
        port_buildUnused(index);
        return;
    }

    if (eType == OBJ_SMOKE) {
        port_buildSmoke(index);
    } else if (eType == OBJ_DOUBLE_JUMP_ORB) {
        port_buildDoubleDashOrb(index);
    } else if (eType == OBJ_KEY) {
        port_buildKey(index);
    } else if (eType == OBJ_PLATMOV_R || eType == OBJ_PLATMOV_L) {
        port_buildPlatMov(index);
    } else if (eType == OBJ_SPRING) {
        port_buildSpring(index);
    } else if (eType == OBJ_CHEST) {
        port_buildChest(index);
    } else if (eType == OBJ_BALLOON) {
        port_buildBalloon(index);
    } else if (eType == OBJ_COLLAPSE_TILE) {
        port_buildCollapseTile(index);
    } else if (eType == OBJ_STRAWBERRY) {
        port_buildStrawberry(index);
    } else if (eType == OBJ_FLYING_BERRY) {
        port_buildFlyingBerry(index);
    } else if (eType == OBJ_DECO_TREE || eType == OBJ_DECO_FLOWER) {
        port_buildStaticDecor(index);
    } else if (eType == OBJ_BREAKABLE_WALL) {
        port_buildBreakableWall(index);
    } else if (eType == OBJ_MONUMENT) {
        port_buildMonument(index);
    } else if (eType == OBJ_BIG_CHEST) {
        port_buildBigChest(index);
    }
}

void port_drawText(const unsigned char *text, uint8_t x, uint8_t y)
{
    uint16_t tileX = (uint16_t)(x / 8u);
    uint16_t tileY = (uint16_t)(y / 8u);
    uint16_t bufferOffset = (tileY * 32u) + tileX;
    size_t textLength;
    size_t i;

    if ((text == NULL) || (tileX >= 32u) || (tileY >= 32u)) {
        return;
    }

    textLength = strlen((const char *)text);

    if (bufferOffset >= sizeof(GLBOAL_M0BG1TileMap)) {
        return;
    }

    {
        size_t maxWrite = sizeof(GLBOAL_M0BG1TileMap) - bufferOffset;
        if (textLength > maxWrite) {
            textLength = maxWrite;
        }
    }

    for (i = 0; i < textLength; ++i) {
        GLBOAL_M0BG1TileMap[bufferOffset + i] = text[i];
    }

    GLOBAL_ActiveLevel.textChanged = true;
}


int16_t randint16(int16_t min, int16_t max);

void onUpdateBG4CloudsEffect(void){
    static uint16_t curHOFS = 0;
    static uint8_t moveAmount = 1;
    static uint8_t gustState = 0; // 0=pause, 1=ramp up, 2=peak, 3=ramp down
    static uint16_t gustTimer = 0;
    static uint16_t gustDuration = 0;
    static uint8_t targetSpeed = 0;
    static uint8_t startSpeed = 0;
 
    //Update frame count

    // Handle gust state machine
    if (gustTimer > 0) {
        gustTimer--;
    } else {
        // Transition to next state
        switch (gustState) {
            case 0: // Pause -> Ramp up
                gustState = 1;
                gustDuration = randint16(60, 300); // 1-5 seconds (60fps)
                gustTimer = gustDuration;
                startSpeed = moveAmount;
                targetSpeed = randint16(5, 10); // Random peak speed 3-6
                break;
            case 1: // Ramp up -> Peak
                gustState = 2;
                gustDuration = randint16(0, 120); // 0-2 seconds
                gustTimer = gustDuration;
                moveAmount = targetSpeed;
                break;
            case 2: // Peak -> Ramp down
                gustState = 3;
                gustDuration = randint16(60, 300); // 1-5 seconds
                gustTimer = gustDuration;
                startSpeed = moveAmount;
                targetSpeed = 1;
                break;
            case 3: // Ramp down -> Pause
                gustState = 0;
                gustDuration = randint16(0, 120); // 0-2 seconds
                gustTimer = gustDuration;
                moveAmount = 1;
                break;
        }
    }

    // Calculate current speed based on state
    if (gustState == 1) { // Ramp up
        uint16_t progress = gustDuration - gustTimer;
        moveAmount = startSpeed + ((targetSpeed - startSpeed) * progress) / gustDuration;
    } else if (gustState == 3) { // Ramp down
        uint16_t progress = gustDuration - gustTimer;
        moveAmount = startSpeed - ((startSpeed - targetSpeed) * progress) / gustDuration;
    }

    //Update scroll
    curHOFS += moveAmount;
    
    //It's a write twice register
    GLOBAL_ScrollBG4X = curHOFS;
}


void port_vblank(void)
{
    int8_t regRead1 = 0;
    uint8_t regRead2 = 0;
    uint8_t regRead3 = 0;
    uint16_t scanline;
    
    do{ //Wait for Vblank
        //Latch before reading the interupt
        //This was after before but resulted in an edge case
        //It would be in vblank here, but by the time it latched
        //Vblank would be over
        regRead1 = REG_SLHV;
        regRead1 = REG_RDNMI;
    } while(regRead1 > 0);
    regRead2 = REG_OPVCT;
    regRead3 = REG_OPVCT;
    //9 bits vertical scan line counter
    scanline = ((regRead3 << 8) | regRead2) & 0x01FF; 
    //Hacky workaround to make sure we arn't partially in a vblank already
    if (scanline > 231) {
        return;
    }      

    LoadOAMCopy((char *)GLOBAL_OAMCopy.Bytes, 0x0000, sizeof(union uOAMCopy));

    bool paletteDirty = false;
    bool wantAltPalette = GLOBAL_PlayerData.doubleDashUnlocked;

    if (GLOBAL_ActiveLevel.swapActivePalette || (wantAltPalette != s_altPaletteApplied)) {
        s_altPaletteApplied = wantAltPalette;
        GLOBAL_ActiveLevel.swapActivePalette = false;
        paletteDirty = true;
    }

    if (GLOBAL_ActiveLevel.textFlashActive) {
        if (!s_prevTextFlashActive) {
            s_textFlashPhase = 1;
        } else {
            ++s_textFlashPhase;
        }
        paletteDirty = true;
    } else if (s_prevTextFlashActive) {
        s_textFlashPhase = 0;
        paletteDirty = true;
    }
    s_prevTextFlashActive = GLOBAL_ActiveLevel.textFlashActive;

    if (paletteDirty) {
        const uint8_t *basePalette = s_altPaletteApplied ? s_bg1PaletteDoubleDash : s_bg1PaletteDefault;
        memcpy(s_bg1PaletteCurrent, basePalette, sizeof(s_bg1PaletteCurrent));

        if (GLOBAL_ActiveLevel.textFlashActive) {
            if (s_textFlashPhase & 1u) {
                s_bg1PaletteCurrent[2] = 0x1F;
                s_bg1PaletteCurrent[3] = 0x00;
            } else {
                s_bg1PaletteCurrent[2] = 0xFF;
                s_bg1PaletteCurrent[3] = 0x7F;
            }
        }

        LoadCGRam((char *)s_bg1PaletteCurrent, 0x0000, sizeof(s_bg1PaletteCurrent));
    }

    if (GLOBAL_ActiveLevel.swapCloudPal) {
        snesXC_setDataBank(BANK_01); // clouds_palette_2 is in rom_bank_1
        LoadCGRam((char *)clouds_palette_2,0x0060, sizeof(clouds_palette_2));
        snesXC_setDataBank(BANK_00); // Restore to bank 0
        GLOBAL_ActiveLevel.swapCloudPal = false;
    }

    if (GLOBAL_ActiveLevel.textChanged) {
        LoadLoVram(GLBOAL_M0BG1TileMap,0xF800, sizeof(GLBOAL_M0BG1TileMap));
        GLOBAL_ActiveLevel.textChanged = false;
    }

    uint16_t tmpx1 = 0;
    uint16_t tmpy1 = 0;
    if (GLOBAL_ActiveLevel.textScrollActive) {
        tmpx1 = GLOBAL_ActiveLevel.textScrollOffsetX;
        tmpy1 = GLOBAL_ActiveLevel.textScrollOffsetY;
    }
    GLOBAL_ScrollBG1X = tmpx1;
    GLOBAL_ScrollBG1Y = tmpy1;

    uint8_t lo_x1 = (uint8_t)(tmpx1 & 0xFF);
    uint8_t hi_x1 = (uint8_t)(tmpx1 >> 8);
    REG_BG1HOFS = lo_x1;
    REG_BG1HOFS = hi_x1;

    uint8_t lo_y1 = (uint8_t)(tmpy1 & 0xFF);
    uint8_t hi_y1 = (uint8_t)(tmpy1 >> 8);
    REG_BG1VOFS = lo_y1;
    REG_BG1VOFS = hi_y1;

    //Update background scrolls
    // BG2
    uint16_t tmpx2 = GLOBAL_ScrollBG2X;
    uint8_t lo_x2 = (uint8_t)(tmpx2 & 0xFF);
    uint8_t hi_x2 = (uint8_t)(tmpx2 >> 8);
    REG_BG2HOFS = lo_x2;
    REG_BG2HOFS = hi_x2;

    uint16_t tmpy2 = GLOBAL_ScrollBG2Y;
    uint8_t lo_y2 = (uint8_t)(tmpy2 & 0xFF);
    uint8_t hi_y2 = (uint8_t)(tmpy2 >> 8);
    REG_BG2VOFS = lo_y2;
    REG_BG2VOFS = hi_y2;

    // BG3
    uint16_t tmpx3 = GLOBAL_ScrollBG3X;
    uint8_t lo_x3 = (uint8_t)(tmpx3 & 0xFF);
    uint8_t hi_x3 = (uint8_t)(tmpx3 >> 8);
    REG_BG3HOFS = lo_x3;
    REG_BG3HOFS = hi_x3;

    uint16_t tmpy3 = GLOBAL_ScrollBG3Y;
    uint8_t lo_y3 = (uint8_t)(tmpy3 & 0xFF);
    uint8_t hi_y3 = (uint8_t)(tmpy3 >> 8);
    REG_BG3VOFS = lo_y3;
    REG_BG3VOFS = hi_y3;

    uint16_t tmpx4 = GLOBAL_ScrollBG4X;
    uint8_t lo_x4 = (uint8_t)(tmpx4 & 0xFF);
    uint8_t hi_x4 = (uint8_t)(tmpx4 >> 8);
    REG_BG4HOFS = lo_x4;
    REG_BG4HOFS = hi_x4;

    uint16_t tmpy4 = GLOBAL_ScrollBG4Y;
    uint8_t lo_y4 = (uint8_t)(tmpy4 & 0xFF);
    uint8_t hi_y4 = (uint8_t)(tmpy4 >> 8);
    REG_BG4VOFS = lo_y4;
    REG_BG4VOFS = hi_y4;
    
    //Update player hair colour
    static uint16_t hairColour;
    static uint16_t  constZero = 0x0000;
    if (GLOBAL_PlayerData.dashesLeft == 0) {
        hairColour = 0x7EA5;
    }
    else if (GLOBAL_PlayerData.dashesLeft == 1) {
        hairColour = 0x241F;
    }
    else {
        hairColour = 0x1B80;
    }
    LoadCGRam((char *)&hairColour,0x00C5, sizeof(hairColour));

    //End of vblank critical code
    //Calculate hardware scrolls, do so before objects as they may rely on these values being correct
    int16_t shakeAmount = GLOBAL_ActiveLevel.shakeFrames > 0 ? ((GLOBAL_FrameCount & 1) ? 2 : -2) : 0;
    int16_t smoothScrollY = ((int16_t)GLOBAL_PlayerData.objData.pos.y - GLOBAL_ActiveLevel.scrollPointY) >> 2;

    GLOBAL_ScrollBG2Y = CLAMP(GLOBAL_PlayerData.objData.pos.y - 16 - GLOBAL_ActiveLevel.scrollPointY, 0, 31);
    GLOBAL_ScrollBG2X = 0;

    GLOBAL_ScrollBG3X = GLOBAL_ScrollBG2X + (shakeAmount);
    GLOBAL_ScrollBG3Y = GLOBAL_ScrollBG2Y + (shakeAmount);

    GLOBAL_ScrollBG4Y =  smoothScrollY + (shakeAmount >> 1) - (GLOBAL_ActiveLevel.currentRoomID << 6);

    onUpdateBG4CloudsEffect();
}

void port_resetSprites(void)
{
    //Clear all sprites
    for (uint8_t i = 0; i < 128; i++) {
        GLOBAL_OAMCopy.arr.OAMArray[i].OBJY = 240;
    }

    s_nextExtraSprite = PORT_EXTRA_SPRITE_START;
    s_maxExtraSpriteUsed = PORT_EXTRA_SPRITE_START;
    for (uint8_t slot = PORT_EXTRA_SPRITE_START; slot < PORT_OAM_ENTRY_COUNT; ++slot) {
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = 240;
    }
    // Reset player sprite (sprite 0) using direct byte access for vbcc65816 compatibility
    // SNES OAM format: byte 0 = X, byte 1 = Y, byte 2 = Tile, byte 3 = Properties
    GLOBAL_OAMCopy.Bytes[0] = 0;  // X position
    GLOBAL_OAMCopy.Bytes[1] = 0;  // Y position
    GLOBAL_OAMCopy.Bytes[2] = 0;  // Tile number
    GLOBAL_OAMCopy.Bytes[3] = 0;  // Properties
    GLOBAL_OAMCopy.arr.OAMTable2[0] = 0x56; //Enable the first sprite, set size to 16x16

}


// Cross-compiler interrupt handlers, must be present
void snesXC_cop(void) {
}

void snesXC_brk(void) {
}

void snesXC_abort(void) {
}

void snesXC_nmi(void) {
    // VBlank handling is done in onVblank() which is called from the main loop
}

