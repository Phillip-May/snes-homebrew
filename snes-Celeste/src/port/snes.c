#include "port.h"
#include "snes_farcall.h"


#include "../../shared/src/snes_regs_xc.h"
#include "../../shared/src/initsnes.h"
//Thank you llvm mos
//making me micro manage where my graphics data in ROM
#ifdef __SNESXC_16BIT_POINTERS__
#pragma clang section rodata="rom_data_bank_1"
#endif
#include "../clouds.h"
#include "../sprite_data.h"
#include "../../python/pico8_font_snes.h"
#include "../../python/score_1000_snes.h"
#include <string.h>

//Level data, ideally place these ocntiguiously in the same bank
//Each one is 2347 bytes pre object data
//32 levels, 2347 * 32 = 75104 bytes
//So it'll need to be split into 3 banks
//13 ish levels per bank
#pragma SECTION CONST=CEL_K_00
#ifdef __SNESXC_16BIT_POINTERS__
#pragma clang section rodata="rom_data_bank_2"
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
#pragma clang section rodata="rom_data_bank_3"
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
#pragma clang section rodata="rom_data_bank_4"
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
#pragma clang section rodata="rom_data_bank_5"
#endif
#include "../../python/title_screen_snes.h"
#ifdef __SNESXC_16BIT_POINTERS__
#pragma clang section rodata="rom_data_bank_6"
#endif
#include "../../spc700/testrom/spc_payload.h"
#ifdef __SNESXC_16BIT_POINTERS__
#pragma clang section rodata=""
#endif
#pragma SECTION CONST=CONST

#ifdef __mos__
#define PORT_NOINLINE __attribute__((noinline))
#else
#define PORT_NOINLINE
#endif

extern struct sPlayerData GLOBAL_PlayerData;
extern bool GLOBAL_FlagOverlayShow;
extern uint8_t GLOBAL_FlagOverlayLine0Len;
extern uint8_t GLOBAL_FlagOverlayLine1Len;
extern uint8_t GLOBAL_FlagOverlayLine2Len;
extern unsigned char GLOBAL_FlagOverlayLine0[17];
extern unsigned char GLOBAL_FlagOverlayLine1[17];
extern unsigned char GLOBAL_FlagOverlayLine2[17];
//Compiles without this, linter just gets confused
extern const unsigned short clouds_palette_2[4];
uint8_t paletteBg[64];


union uOAMCopy GLOBAL_OAMCopy;
extern uint16_t GLOBAL_ScrollBG2Y;
extern OBJ_DATA GLOBAL_OBJList[];

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

#define PORT_BG_TILEMAP_WORDS 512u
#define PORT_BG_TILEMAP_BYTES (PORT_BG_TILEMAP_WORDS * sizeof(uint16_t))
#define PORT_BG1_TEXT_SCREEN_MACRO_W 16u
#define PORT_BG1_TEXT_SCREEN_MACRO_H 16u
#define PORT_BG1_TEXT_CELL_COUNT (PORT_BG1_TEXT_SCREEN_MACRO_W * PORT_BG1_TEXT_SCREEN_MACRO_H)
#define PORT_BG1_TEXT_SLOT_COUNT 50u
#define PORT_BG1_TEXT_SLOT_ROW_COUNT ((PORT_BG1_TEXT_SLOT_COUNT + 7u) / 8u)
#define PORT_BG1_TEXT_SLOT_FREE 0xFFu
#define PORT_BG1_TEXT_CELL_NONE 0xFFFFu
#define PORT_GAME_2BPP_TILE_COUNT SPRITE_GFX_2BPP_TILE_COUNT
#define PORT_BG1_TEXT_TILE_BASE 18u
#define PORT_BG1_TEXT_TILEDATA_BASE 0xC000u
#define PORT_BG1_TEXT_TILEMAP_BASE 0x6000u
#define PORT_BG1_TEXT_PALETTE 7u
#define PORT_BG1_TEXT_PRIORITY_ATTR 0x2000u
#define PORT_BG1_TEXT_ATTR ((uint16_t)(PORT_BG1_TEXT_PRIORITY_ATTR | ((uint16_t)PORT_BG1_TEXT_PALETTE << 10)))
#define PORT_BG1_TEXT_CGRAM_OFFSET (PORT_BG1_TEXT_PALETTE * 4u)
#define PORT_BG1_TEXT_COLOR_TRANSPARENT 0u
#define PORT_BG1_TEXT_COLOR_INK 1u
#define PORT_BG1_TEXT_COLOR_BG 3u
#define PORT_BG1_TEXT_IN_VBLANK() ((REG_HVBJOY & 0x80u) != 0u)
#define PORT_BG1_TEXT_MAX_MAP_ROWS_PER_VBLANK 2u
#define PORT_BG1_TEXT_MAX_SLOTS_PER_VBLANK 4u
#define PORT_BG1_TEXT_CODE PORT_FUNC_BANK4
#define PORT_BG1_TEXT_CORE_CODE PORT_FUNC_BANK4
#define PORT_PICO8_FONT_GLYPH_W_PX 4u
#define PORT_PICO8_FONT_ADVANCE_PX 5u
#define PORT_PICO8_FONT_SCALE 2u
#define PORT_PICO8_FONT_CELL_Y_OFFSET 4u
#define PORT_PICO8_TEXT_INK_X_OFFSET_PX 2u
#define PORT_PICO8_TEXT_BG_PAD_RIGHT_PX 2u
#define PORT_BG1_TEXT_SLOT_MASK_BYTES 32u
#define PORT_BG1_TEXT_DMA_SLOT_BYTES 64u
#define PORT_BG1_TEXT_SLOT_BOTTOM_OFFSET 32u
#define PORT_BG1_TEXT_DMA_STAGE_COUNT 2u

static uint8_t s_bg1TextInkBits[PORT_BG1_TEXT_SLOT_COUNT][PORT_BG1_TEXT_SLOT_MASK_BYTES];
static union {
    uint8_t coverBits[PORT_BG1_TEXT_SLOT_COUNT][PORT_BG1_TEXT_SLOT_MASK_BYTES];
    uint16_t bg3Tilemap[PORT_BG_TILEMAP_WORDS];
} s_bg1TextCoverScratch;
static uint8_t s_bg1TextDmaSlotBytes[PORT_BG1_TEXT_DMA_STAGE_COUNT][PORT_BG1_TEXT_DMA_SLOT_BYTES];
static uint8_t s_bg1TextDmaStageIndex;
static uint8_t s_bg1TextSlotCell[PORT_BG1_TEXT_SLOT_COUNT];
static uint8_t s_bg1TextCellSlot[PORT_BG1_TEXT_CELL_COUNT];
static uint8_t s_bg1TextSlotDirtyBits[PORT_BG1_TEXT_SLOT_ROW_COUNT];
static bool s_bg1TextAnySlotDirty;
static uint16_t s_bg1TextMapDirtyRowBits;
// Color 0 stays transparent. Color 1 is black ink/fill, color 3 is white.
static uint16_t s_bg1TextPalette[4] = {0x0000u, 0x0000u, 0x0000u, 0x7FFFu};
static uint16_t s_scoreSpritePalette[16];
static uint8_t s_pico8GlyphRows[8];

#define s_bg1TextCoverBits s_bg1TextCoverScratch.coverBits
#define s_tilemapBg3 s_bg1TextCoverScratch.bg3Tilemap

// NMI-driven VBlank: pre-built staging for text DMA
#define TEXT_MAX_MAP_DMA 1u
#define TEXT_MAX_SLOT_DMA PORT_BG1_TEXT_DMA_STAGE_COUNT
static uint16_t s_textMapDmaBufs[TEXT_MAX_MAP_DMA][PORT_BG1_TEXT_SCREEN_MACRO_W]; // 32 bytes
static uint16_t s_textMapDmaDst[TEXT_MAX_MAP_DMA];
static uint8_t  s_textMapDmaCount;
static uint16_t s_textSlotDmaTile[TEXT_MAX_SLOT_DMA];
static uint8_t  s_textSlotDmaCount;

uint8_t GLOBAL_InputLo;
uint8_t GLOBAL_InputHi;

#define SPC_CMD_PLAY_SFX_A 0x01u
#define SPC_CMD_STOP_ALL   0x03u
#define SPC_CMD_PLAY_SFX_B 0x04u
#define SPC_CMD_PLAY_MUSIC 0x06u
#define BANK_SPC_PAYLOAD 13  // rom_data_bank_6 (SPC payload)

static bool s_spcReady = false;
static bool s_spcInitTried = false;
static bool s_spcRuntimeReady = false;
static uint8_t s_spcLastData = 0xFFu;
static uint8_t s_spcForceData = 0x80u;
static uint8_t s_spcSfxCmdToggle = 0u;
volatile uint8_t GLOBAL_SpcDebugStage = 0u;
volatile uint8_t GLOBAL_SpcDebugApu0 = 0u;
volatile uint8_t GLOBAL_SpcDebugApu1 = 0u;
volatile uint8_t GLOBAL_SpcDebugApu2 = 0u;
volatile uint8_t GLOBAL_SpcDebugApu3 = 0u;
volatile uint8_t GLOBAL_SpcDebugLastCmd = 0u;
volatile uint8_t GLOBAL_SpcDebugLastData = 0u;
volatile uint8_t GLOBAL_SpcDebugSendCount = 0u;
volatile uint8_t GLOBAL_SpcDebugSendFail = 0u;
static uint8_t s_spcUploadBuf[SPC_CHUNK_SIZE];

static bool spc_wait_boot(void) {
    uint32_t timeout = 1500000u;
    while (timeout > 0u) {
        if (REG_APUIO0 == 0xAAu && REG_APUIO1 == 0xBBu) {
            return true;
        }
        timeout--;
    }
    return false;
}

static bool spc_transfer_block(const uint8_t *src, uint16_t dest, uint16_t size) {
    uint8_t token;
    uint16_t i;
    uint32_t timeout;

    REG_APUIO2 = (uint8_t)(dest & 0xFFu);
    REG_APUIO3 = (uint8_t)(dest >> 8);

    token = (uint8_t)(REG_APUIO0 + 0x22u);
    if (token == 0u) {
        token = 1u;
    }

    REG_APUIO1 = token;
    REG_APUIO0 = token;

    timeout = 250000u;
    while (REG_APUIO0 != token) {
        if (timeout == 0u) {
            return false;
        }
        timeout--;
    }

    for (i = 0; i < size; i++) {
        REG_APUIO1 = src[i];
        REG_APUIO0 = (uint8_t)i;

        timeout = 250000u;
        while (REG_APUIO0 != (uint8_t)i) {
            if (timeout == 0u) {
                return false;
            }
            timeout--;
        }
    }

    return true;
}

static bool spc_execute(uint16_t startAddr) {
    uint8_t execToken;
    uint32_t timeout;

    REG_APUIO2 = (uint8_t)(startAddr & 0xFFu);
    REG_APUIO3 = (uint8_t)(startAddr >> 8);
    REG_APUIO1 = 0u;

    execToken = (uint8_t)(REG_APUIO0 + 2u);
    REG_APUIO0 = execToken;

    timeout = 250000u;
    while (REG_APUIO0 != execToken) {
        if (timeout == 0u) {
            return false;
        }
        timeout--;
    }

    return true;
}

static const uint8_t *spc_chunk_ptr(uint16_t ci) {
    switch (ci) {
        case 0u: return spc_chunk_000;
        case 1u: return spc_chunk_001;
        case 2u: return spc_chunk_002;
        case 3u: return spc_chunk_003;
        case 4u: return spc_chunk_004;
        case 5u: return spc_chunk_005;
        case 6u: return spc_chunk_006;
        case 7u: return spc_chunk_007;
        case 8u: return spc_chunk_008;
        case 9u: return spc_chunk_009;
        case 10u: return spc_chunk_010;
        case 11u: return spc_chunk_011;
        case 12u: return spc_chunk_012;
        case 13u: return spc_chunk_013;
        case 14u: return spc_chunk_014;
        case 15u: return spc_chunk_015;
        case 16u: return spc_chunk_016;
        case 17u: return spc_chunk_017;
        case 18u: return spc_chunk_018;
        case 19u: return spc_chunk_019;
        case 20u: return spc_chunk_020;
        case 21u: return spc_chunk_021;
        case 22u: return spc_chunk_022;
        case 23u: return spc_chunk_023;
        case 24u: return spc_chunk_024;
        case 25u: return spc_chunk_025;
        case 26u: return spc_chunk_026;
        case 27u: return spc_chunk_027;
        case 28u: return spc_chunk_028;
        case 29u: return spc_chunk_029;
        case 30u: return spc_chunk_030;
        case 31u: return spc_chunk_031;
        case 32u: return spc_chunk_032;
        case 33u: return spc_chunk_033;
        case 34u: return spc_chunk_034;
        case 35u: return spc_chunk_035;
        case 36u: return spc_chunk_036;
        case 37u: return spc_chunk_037;
        case 38u: return spc_chunk_038;
        case 39u: return spc_chunk_039;
        case 40u: return spc_chunk_040;
        case 41u: return spc_chunk_041;
        case 42u: return spc_chunk_042;
        case 43u: return spc_chunk_043;
        case 44u: return spc_chunk_044;
        case 45u: return spc_chunk_045;
        case 46u: return spc_chunk_046;
        case 47u: return spc_chunk_047;
        case 48u: return spc_chunk_048;
        case 49u: return spc_chunk_049;
        case 50u: return spc_chunk_050;
        case 51u: return spc_chunk_051;
        case 52u: return spc_chunk_052;
        default: return 0;
    }
}
static bool spc_upload_image(void) {
    uint16_t ci;
    snesXC_setDataBank(BANK_SPC_PAYLOAD);
    for (ci = 0; ci < SPC_CHUNK_COUNT; ci++) {
        uint16_t dest = (uint16_t)(spc_load_addr + (uint16_t)(ci * SPC_CHUNK_SIZE));
        uint16_t remaining = (uint16_t)(spc_load_size - (uint16_t)(ci * SPC_CHUNK_SIZE));
        uint16_t size = remaining < (uint16_t)SPC_CHUNK_SIZE ? remaining : (uint16_t)SPC_CHUNK_SIZE;
        // For 16-bit pointer builds, ROM pointers are near pointers and direct
        // CPU reads from other banks are unsafe. DMA-copy each chunk to WRAM first.
        const uint8_t *chunk = spc_chunk_ptr(ci);
        if (chunk == 0) {
            snesXC_setDataBank(0u);
            return false;
        }
        snesXC_memcpy_banked(s_spcUploadBuf, chunk, size);
        if (!spc_transfer_block(s_spcUploadBuf, dest, size)) {
            snesXC_setDataBank(0u);
            return false;
        }
    }
    snesXC_setDataBank(0u);
    return true;
}

static bool spc_send_cmd(uint8_t cmd, uint8_t data) {
    uint32_t timeout = 20000u;
    if (REG_APUIO3 != 0x99u) {
        return false;
    }
    GLOBAL_SpcDebugLastCmd = cmd;
    GLOBAL_SpcDebugLastData = data;
    GLOBAL_SpcDebugSendCount++;
    REG_APUIO1 = cmd;
    REG_APUIO0 = data;
    while (REG_APUIO0 != data) {
        if (timeout == 0u) {
            GLOBAL_SpcDebugSendFail++;
            return false;
        }
        timeout--;
    }
    s_spcLastData = data;
    return true;
}

static bool spc_wait_runtime_ready(uint32_t timeout) {
    while (timeout > 0u) {
        if (REG_APUIO3 == 0x99u) {
            return true;
        }
        timeout--;
    }
    return false;
}

static bool spc_force_stop(void) {
    uint8_t token0;
    uint8_t token1;

    do {
        s_spcForceData++;
    } while (s_spcForceData == s_spcLastData || s_spcForceData == REG_APUIO0);
    token0 = s_spcForceData;

    if (!spc_send_cmd(SPC_CMD_STOP_ALL, token0)) {
        return false;
    }

    do {
        s_spcForceData++;
    } while (s_spcForceData == token0 || s_spcForceData == REG_APUIO0);
    token1 = s_spcForceData;
    return spc_send_cmd(SPC_CMD_STOP_ALL, token1);
}

uint8_t port_getInputs(void)
{
    uint8_t rawLo;
    uint8_t rawHi;
    uint8_t buttons;

    (void)REG_HVBJOY; // Latch controller state
    rawLo = REG_JOY1L;
    rawHi = REG_JOY1H;

    GLOBAL_InputLo = rawLo;
    GLOBAL_InputHi = rawHi;

    // JOY1H bit layout is B,Y,Select,Start,Up,Down,Left,Right (bit7..bit0),
    // which matches PORT_INPUT_* mask bits directly. Also project SNES A/X
    // onto the gameplay-facing B/Y bits so title and gameplay accept either
    // face-button pair.
    buttons = rawHi;
    if ((rawLo & PORT_INPUT_A_MASK) != 0u) {
        buttons |= PORT_INPUT_B_MASK;
    }
    if ((rawLo & PORT_INPUT_X_MASK) != 0u) {
        buttons |= PORT_INPUT_Y_MASK;
    }
    return buttons;
}

static uint8_t s_bg1PaletteCurrent[16];
static bool s_altPaletteApplied = false;
static bool s_prevTextFlashActive = false;
static uint8_t s_textFlashPhase = 0;
static bool s_titleMode = false;
static uint16_t s_effectsStepFrame = 0u;
static uint16_t s_cloudCurHofs = 0u;
static uint8_t s_cloudMoveAmount = 1u;
static uint8_t s_cloudTargetSpeed = 1u;
static uint8_t s_cloudGustState = 0u; // 0=pause, 1=ramp up, 2=hold, 3=ramp down
static uint16_t s_cloudGustTimer = 0u;
static uint16_t s_cloudRng = 0xACE1u;

static uint8_t cloud_rand_range_u8(uint8_t minInclusive, uint8_t maxInclusive)
{
    uint16_t span = (uint16_t)(maxInclusive - minInclusive) + 1u;
    s_cloudRng = (uint16_t)((s_cloudRng * 109u) + 89u);
    return (uint8_t)(minInclusive + (uint8_t)(s_cloudRng % span));
}

// Called at 30fps. Timers decrement by 2 and ramp steps by 2 to match
// the same real-time gust dynamics as the original 60fps design.
// Movement is doubled to maintain the same visual scroll speed.
static void updateCloudsVblankSafe(void)
{
    if (s_cloudGustTimer > 0u) {
        s_cloudGustTimer = (s_cloudGustTimer > 2u) ? (uint16_t)(s_cloudGustTimer - 2u) : 0u;
    } else {
        switch (s_cloudGustState) {
            case 0u:
                s_cloudGustState = 1u;
                s_cloudGustTimer = cloud_rand_range_u8(60u, 255u);
                s_cloudTargetSpeed = cloud_rand_range_u8(5u, 10u);
                break;
            case 1u:
                s_cloudGustState = 2u;
                s_cloudGustTimer = cloud_rand_range_u8(0u, 120u);
                break;
            case 2u:
                s_cloudGustState = 3u;
                s_cloudGustTimer = cloud_rand_range_u8(60u, 255u);
                s_cloudTargetSpeed = 1u;
                break;
            default:
                s_cloudGustState = 0u;
                s_cloudGustTimer = cloud_rand_range_u8(0u, 120u);
                s_cloudMoveAmount = 1u;
                break;
        }
    }

    if (s_cloudGustState == 1u) {
        if (s_cloudMoveAmount + 2u <= s_cloudTargetSpeed) {
            s_cloudMoveAmount += 2u;
        } else if (s_cloudMoveAmount < s_cloudTargetSpeed) {
            s_cloudMoveAmount = s_cloudTargetSpeed;
        } else {
            s_cloudGustState = 2u;
        }
    } else if (s_cloudGustState == 3u) {
        if (s_cloudMoveAmount >= s_cloudTargetSpeed + 2u) {
            s_cloudMoveAmount -= 2u;
        } else if (s_cloudMoveAmount > s_cloudTargetSpeed) {
            s_cloudMoveAmount = s_cloudTargetSpeed;
        } else {
            s_cloudGustState = 0u;
        }
    }

    s_cloudCurHofs = (uint16_t)(s_cloudCurHofs + (uint16_t)s_cloudMoveAmount * 2u);
    GLOBAL_ScrollBG4X = s_cloudCurHofs;
}

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
#define PORT_BERRY_SCORE_EXTRA_SPRITES 2u
#define PORT_BERRY_SCORE_PROPERTIES 0x3Eu
#define PORT_BERRY_SCORE_CGRAM_OFFSET 0x00F0u
#define PORT_OAM_ENTRY_COUNT 128u
// Global visual alignment tweak for SNES OAM Y coordinates.
#define PORT_SPRITE_Y_BIAS (-1)
static int16_t sprite_to_screen_y(int16_t y2x)
{
    return (int16_t)(y2x - (int16_t)GLOBAL_ScrollBG2Y + PORT_SPRITE_Y_BIAS);
}

static uint8_t sprite_to_screen_x_byte(int16_t x2x)
{
    return (uint8_t)x2x;
}

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
    int16_t screenY = sprite_to_screen_y((int16_t)(obj->pos.y << 1));
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(index, 1u, 0u, currentByte);
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = sprite_to_screen_x_byte((int16_t)(obj->pos.x << 1));
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = (uint8_t)screenY;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = obj->oamTile;
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = obj->oamProps;
}

static void writeConditionalSprite(uint8_t index, const OBJ_DATA *obj, bool hide)
{
    uint8_t table2Index = (uint8_t)(index / 4u);
    uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(index, 1u, 0u, currentByte);
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = sprite_to_screen_x_byte((int16_t)(obj->pos.x << 1));
    if (hide) {
        GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
    } else {
        int16_t screenY = sprite_to_screen_y((int16_t)(obj->pos.y << 1));
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
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = sprite_to_screen_x_byte((int16_t)((uint16_t)(obj->pos.x << 1) + offsetX));
        int16_t extraScreenY = sprite_to_screen_y((int16_t)((obj->pos.y << 1) + offsetY));
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

        int16_t spriteX = (int16_t)(obj->pos.x << 1) + kOffsetsX[extra];
        int16_t spriteY = sprite_to_screen_y((int16_t)((obj->pos.y << 1) + kOffsetsY[extra]));
        GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = kTiles[extra];
        GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = kProps[extra];
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = sprite_to_screen_x_byte(spriteX);
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
    int16_t rightXFull = (int16_t)((obj->pos.x << 1) + PORT_BIG_CHEST_OFFSET_RIGHT);
    uint8_t xBit = (uint8_t)((rightXFull < 0 || rightXFull >= 256) ? 1u : 0u);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(slot, 1u, xBit, currentByte);

    GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = PORT_BIG_CHEST_TILE_RIGHT;
    GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = obj->oamProps;
    GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = (uint8_t)rightXFull;
    if (hide) {
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = 240;
    } else {
        int16_t screenY = sprite_to_screen_y((int16_t)(obj->pos.y << 1));
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)screenY;
    }

    return true;
}

__attribute__((optnone)) static bool writeBalloonSprite(uint8_t index, OBJ_DATA *obj)
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
    GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = sprite_to_screen_x_byte((int16_t)(obj->pos.x << 1));
    if (hideMain) {
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = 240;
    } else {
        int16_t stringY = sprite_to_screen_y((int16_t)((obj->pos.y << 1) + 14 + (obj->data.balloon.spriteYOffset << 1)));
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)stringY;
    }
    GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = obj->data.balloon.stringTile;
    // String uses palette 2 (priority 3, palette 2)
    GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = 0x32;

    return true;
}

static bool computePlatMovWrap(const OBJ_DATA *obj, int16_t *outX, uint8_t *outTile, bool *outHide)
{
    int16_t baseX = (int16_t)((uint8_t)(obj->pos.x << 1));

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

    int16_t screenY = sprite_to_screen_y((int16_t)(obj->pos.y << 1));

    int16_t rightXFull = (int16_t)((uint16_t)(uint8_t)(obj->pos.x << 1) + PORT_PLATMOV_RIGHT_OFFSET);
    uint8_t rightXBit = (uint8_t)((rightXFull < 0 || rightXFull >= 256) ? 1u : 0u);
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
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].OBJX = sprite_to_screen_x_byte((int16_t)wrapXByte);
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].OBJY = (uint8_t)screenY;
    }

    return true;
}

static bool writeFlyingBerrySprite(uint8_t index, OBJ_DATA *obj)
{
    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, PORT_FLYING_BERRY_EXTRA_SPRITES)) {
        return false;
    }

    bool hideMain = (obj->data.flyingBerry.isCollected != 0u);
    writeConditionalSprite(index, obj, hideMain);

    obj->extraSpriteCount = PORT_FLYING_BERRY_EXTRA_SPRITES;
    uint8_t baseSlot = obj->extraSpriteBase;
    uint8_t wingTile = PORT_FLYING_BERRY_WING_TILE_MID;
    int16_t delta = (int16_t)obj->pos.y - (int16_t)obj->data.flyingBerry.startY;
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
        int16_t wingX = (int16_t)(obj->pos.x << 1) + xOffset;
        int16_t wingY = sprite_to_screen_y((int16_t)((obj->pos.y << 1) - PORT_FLYING_BERRY_WING_OFFSET_Y));

        GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = wingTile;
        GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = properties;
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = sprite_to_screen_x_byte(wingX);
        if (hideMain) {
            GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = 240;
        } else {
            GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)wingY;
        }
    }

    return true;
}

static bool writeBerryScoreSprite(uint8_t index, OBJ_DATA *obj)
{
    writeConditionalSprite(index, obj, true);

    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, PORT_BERRY_SCORE_EXTRA_SPRITES)) {
        return false;
    }

    obj->extraSpriteCount = PORT_BERRY_SCORE_EXTRA_SPRITES;

    int16_t baseX = (int16_t)(obj->pos.x << 1);
    int16_t baseY = sprite_to_screen_y((int16_t)(obj->pos.y << 1));
    for (uint8_t part = 0u; part < PORT_BERRY_SCORE_EXTRA_SPRITES; ++part) {
        uint8_t slot = (uint8_t)(obj->extraSpriteBase + part);
        int16_t fullX = (int16_t)(baseX + ((int16_t)part * 16));
        uint8_t table2Index = (uint8_t)(slot / 4u);
        uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
        uint8_t xBit = (uint8_t)((fullX < 0 || fullX >= 256) ? 1u : 0u);

        if (slot >= PORT_OAM_ENTRY_COUNT) {
            break;
        }

        GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(slot, 1u, xBit, currentByte);
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = (uint8_t)fullX;
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)baseY;
        GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = (part == 0u) ? SCORE_1000_SPRITE_TILE_LEFT : SCORE_1000_SPRITE_TILE_RIGHT;
        GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = PORT_BERRY_SCORE_PROPERTIES;
    }

    return true;
}

static bool writeFlagSprite(uint8_t index, OBJ_DATA *obj)
{
    writeStandardSprite(index, obj);

    if (!obj->data.flag.show) {
        if (obj->extraSpriteBase != PORT_EXTRA_SLOT_UNUSED && obj->extraSpriteCount != 0u) {
            releaseExtraSpriteRange(obj->extraSpriteBase, obj->extraSpriteCount);
        }
        obj->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
        obj->extraSpriteCount = 0u;
        return true;
    }

    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, 1u)) {
        return false;
    }

    obj->extraSpriteCount = 1u;
    uint8_t slot = obj->extraSpriteBase;
    uint8_t table2Index = (uint8_t)(slot / 4u);
    uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(slot, 1u, 0u, currentByte);
    GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = 110u;
    GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = 12u;
    GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = STRAWBERRY_SPRITE_1;
    GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = 0x32u;
    return true;
}

//Bank number macros
#define BANK_00  0   // Default/current data bank
#define BANK_ASSETS 8  // rom_data_bank_1 (fonts, sprites, clouds)
#define BANK_LEVELS_00 9  // rom_data_bank_2 (levels 1-12)
#define BANK_LEVELS_01 10 // rom_data_bank_3 (levels 13-24)
#define BANK_LEVELS_02 11 // rom_data_bank_4 (levels 25-32)
#define BANK_TITLE 12 // rom_data_bank_5 (title screen assets)
#define BG12NBA_GAMEPLAY (((0xA000 >> 13) << 4) | ((0xC000 >> 13)))
#define BG34NBA_GAMEPLAY (((0xE000 >> 13) << 4) | ((0xA000 >> 13)))
#define BG34NBA_TITLE    (((0xC000 >> 13) << 4) | ((0x6000 >> 13)))

PORT_BG1_TEXT_CODE
static uint16_t bg1TextSlotTile(uint8_t slot)
{
    // 16x16 BG entries name the top-left 8x8 tile. Each slot is packed as
    // TL,TR in one 16-tile-wide VRAM row and BL,BR in the next row.
    return (uint16_t)(PORT_BG1_TEXT_TILE_BASE + ((uint16_t)(slot >> 3) * 32u) + ((uint16_t)(slot & 7u) * 2u));
}

PORT_BG1_TEXT_CORE_CODE
static uint16_t bg1TextSlotByteOffset(uint8_t slot, uint8_t tilePart, uint8_t localY)
{
    (void)slot;
    return (uint16_t)(((uint16_t)localY * 2u) + (uint16_t)(tilePart & 1u));
}

PORT_BG1_TEXT_CODE
static uint16_t bg1TextCellMapIndex(uint8_t cell)
{
    return (uint16_t)(((uint16_t)(cell / PORT_BG1_TEXT_SCREEN_MACRO_W) * 32u) + (uint16_t)(cell & 0x0Fu));
}

PORT_BG1_TEXT_CORE_CODE
static void bg1TextMarkMapDirty(uint8_t cell)
{
    s_bg1TextMapDirtyRowBits |= (uint16_t)(1u << (cell >> 4));
}

PORT_BG1_TEXT_CORE_CODE
static void bg1TextMarkSlotDirty(uint8_t slot)
{
    s_bg1TextSlotDirtyBits[slot >> 3] |= (uint8_t)(1u << (slot & 7u));
    s_bg1TextAnySlotDirty = true;
}

PORT_BG1_TEXT_CODE
static void bg1TextClearSlotDirty(uint8_t slot)
{
    s_bg1TextSlotDirtyBits[slot >> 3] &= (uint8_t)~(uint8_t)(1u << (slot & 7u));
}

PORT_BG1_TEXT_CORE_CODE
static void bg1TextPublishCell(uint8_t cell)
{
    uint8_t slot;
    slot = s_bg1TextCellSlot[cell];
    if (slot == PORT_BG1_TEXT_SLOT_FREE) {
        return;
    }

    bg1TextMarkSlotDirty(slot);
    bg1TextMarkMapDirty(cell);
}

PORT_BG1_TEXT_CORE_CODE
static bool bg1TextClipRect(uint16_t *sx, uint16_t *sy, uint16_t *w, uint16_t *h)
{
    if (*sx >= 256u || *sy >= 256u || *w == 0u || *h == 0u) {
        return false;
    }
    if ((*sx + *w) > 256u) {
        *w = (uint16_t)(256u - *sx);
    }
    if ((*sy + *h) > 256u) {
        *h = (uint16_t)(256u - *sy);
    }
    return true;
}

PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE void bg1TextPublishRect(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h)
{
    uint8_t cellX0;
    uint8_t cellY0;
    uint8_t cellX1;
    uint8_t cellY1;
    uint8_t cellX;
    uint8_t cellY;

    if (!bg1TextClipRect(&sx, &sy, &w, &h)) {
        return;
    }

    cellX0 = (uint8_t)(sx >> 4);
    cellY0 = (uint8_t)(sy >> 4);
    cellX1 = (uint8_t)((sx + w - 1u) >> 4);
    cellY1 = (uint8_t)((sy + h - 1u) >> 4);
    for (cellY = cellY0; cellY <= cellY1; ++cellY) {
        for (cellX = cellX0; cellX <= cellX1; ++cellX) {
            bg1TextPublishCell((uint8_t)((cellY * PORT_BG1_TEXT_SCREEN_MACRO_W) + cellX));
        }
    }
}

static void bg1TextReset(void)
{
    uint16_t i;
    memset(s_bg1TextInkBits, 0, sizeof(s_bg1TextInkBits));
    memset(s_bg1TextCoverBits, 0, sizeof(s_bg1TextCoverBits));
    memset(s_bg1TextDmaSlotBytes, 0, sizeof(s_bg1TextDmaSlotBytes));
    s_bg1TextDmaStageIndex = 0u;
    memset(s_bg1TextSlotDirtyBits, 0, sizeof(s_bg1TextSlotDirtyBits));
    s_bg1TextAnySlotDirty = false;
    s_bg1TextMapDirtyRowBits = 0u;
    for (i = 0; i < PORT_BG1_TEXT_SLOT_COUNT; ++i) {
        s_bg1TextSlotCell[i] = PORT_BG1_TEXT_SLOT_FREE;
    }
    for (i = 0; i < PORT_BG1_TEXT_CELL_COUNT; ++i) {
        s_bg1TextCellSlot[i] = PORT_BG1_TEXT_SLOT_FREE;
    }
}

static void scoreSpriteUploadPalette(bool flashRed)
{
    memset(s_scoreSpritePalette, 0, sizeof(s_scoreSpritePalette));
    s_scoreSpritePalette[1] = flashRed ? 0x001Fu : 0x7FFFu;
    snesXC_setDataBank(0x7Eu);
    LoadCGRam((const unsigned char *)s_scoreSpritePalette, PORT_BERRY_SCORE_CGRAM_OFFSET, sizeof(s_scoreSpritePalette));
    snesXC_setDataBank(BANK_00);
}

static void bg1TextUploadPalette(void)
{
    snesXC_setDataBank(0x7Eu);
    LoadCGRam((const unsigned char *)s_bg1TextPalette, PORT_BG1_TEXT_CGRAM_OFFSET, sizeof(s_bg1TextPalette));
    snesXC_setDataBank(BANK_00);
}

PORT_BG1_TEXT_CORE_CODE
static void bg1TextClearSlot(uint8_t slot)
{
    memset(s_bg1TextInkBits[slot], 0, PORT_BG1_TEXT_SLOT_MASK_BYTES);
    memset(s_bg1TextCoverBits[slot], 0, PORT_BG1_TEXT_SLOT_MASK_BYTES);
}

PORT_BG1_TEXT_CORE_CODE
static uint8_t bg1TextGetSlot(uint8_t cell, bool allocate)
{
    uint8_t slot = s_bg1TextCellSlot[cell];
    uint8_t i;

    if (slot != PORT_BG1_TEXT_SLOT_FREE) {
        return slot;
    }
    if (!allocate) {
        return PORT_BG1_TEXT_SLOT_FREE;
    }

    for (i = 0; i < PORT_BG1_TEXT_SLOT_COUNT; ++i) {
        if (s_bg1TextSlotCell[i] == PORT_BG1_TEXT_SLOT_FREE) {
            s_bg1TextSlotCell[i] = cell;
            s_bg1TextCellSlot[cell] = i;
            bg1TextClearSlot(i);
            return i;
        }
    }

    return PORT_BG1_TEXT_SLOT_FREE;
}

PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE __attribute__((optnone)) void bg1TextFetchGlyphRows(uint8_t ch)
{
    ch &= 0x7Fu;
    snesXC_setDataBank(BANK_ASSETS);
    snesXC_memcpy_banked(s_pico8GlyphRows, pico8_font_rows[ch], sizeof(s_pico8GlyphRows));
    snesXC_setDataBank(BANK_00);
}

PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE uint16_t bg1TextSetPixelColorNoPublish(uint16_t sx, uint16_t sy, uint8_t color)
{
    uint8_t cellX;
    uint8_t cellY;
    uint8_t cell;
    uint8_t slot;
    uint8_t localX;
    uint8_t localY;
    uint8_t tilePart;
    uint16_t offset;
    uint8_t bit;

    if (sx >= 256u || sy >= 256u) {
        return PORT_BG1_TEXT_CELL_NONE;
    }

    cellX = (uint8_t)(sx >> 4);
    cellY = (uint8_t)(sy >> 4);
    if (cellX >= PORT_BG1_TEXT_SCREEN_MACRO_W || cellY >= PORT_BG1_TEXT_SCREEN_MACRO_H) {
        return PORT_BG1_TEXT_CELL_NONE;
    }

    cell = (uint8_t)((cellY * PORT_BG1_TEXT_SCREEN_MACRO_W) + cellX);
    slot = bg1TextGetSlot(cell, color != PORT_BG1_TEXT_COLOR_TRANSPARENT);
    if (slot == PORT_BG1_TEXT_SLOT_FREE) {
        return PORT_BG1_TEXT_CELL_NONE;
    }

    localX = (uint8_t)(sx & 0x0Fu);
    localY = (uint8_t)(sy & 0x0Fu);
    tilePart = (uint8_t)((localY >= 8u ? 2u : 0u) + (localX >= 8u ? 1u : 0u));
    offset = bg1TextSlotByteOffset(slot, tilePart, localY);
    bit = (uint8_t)(0x80u >> (localX & 7u));

    if (color == PORT_BG1_TEXT_COLOR_TRANSPARENT) {
        s_bg1TextInkBits[slot][offset] &= (uint8_t)~bit;
        s_bg1TextCoverBits[slot][offset] &= (uint8_t)~bit;
    } else {
        s_bg1TextCoverBits[slot][offset] |= bit;
        if (color == PORT_BG1_TEXT_COLOR_INK) {
            s_bg1TextInkBits[slot][offset] |= bit;
        } else {
            s_bg1TextInkBits[slot][offset] &= (uint8_t)~bit;
        }
    }

    return (uint16_t)cell;
}

PORT_BG1_TEXT_CODE
static PORT_NOINLINE void bg1TextSetPixelColor(uint16_t sx, uint16_t sy, uint8_t color)
{
    uint16_t cell = bg1TextSetPixelColorNoPublish(sx, sy, color);
    if (cell != PORT_BG1_TEXT_CELL_NONE) {
        bg1TextPublishCell((uint8_t)cell);
    }
}

PORT_BG1_TEXT_CODE
static void bg1TextHideCell(uint8_t cell)
{
    uint8_t slot = s_bg1TextCellSlot[cell];
    if (slot == PORT_BG1_TEXT_SLOT_FREE) {
        return;
    }

    s_bg1TextSlotCell[slot] = PORT_BG1_TEXT_SLOT_FREE;
    s_bg1TextCellSlot[cell] = PORT_BG1_TEXT_SLOT_FREE;
    bg1TextClearSlotDirty(slot);
    bg1TextMarkMapDirty(cell);
}

PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE void bg1TextFillRectNoPublish(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint8_t color)
{
    uint16_t px;
    uint16_t py;

    if (!bg1TextClipRect(&sx, &sy, &w, &h)) {
        return;
    }

    for (py = sy; py < (uint16_t)(sy + h); ++py) {
        for (px = sx; px < (uint16_t)(sx + w); ++px) {
            (void)bg1TextSetPixelColorNoPublish(px, py, color);
        }
    }
}

PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE void bg1TextFillRect(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint8_t color)
{
    bg1TextFillRectNoPublish(sx, sy, w, h, color);
    bg1TextPublishRect(sx, sy, w, h);
}

PORT_BG1_TEXT_CODE
static PORT_NOINLINE void bg1TextHideRect(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h)
{
    uint8_t cellX0;
    uint8_t cellY0;
    uint8_t cellX1;
    uint8_t cellY1;
    uint8_t cellX;
    uint8_t cellY;

    if (!bg1TextClipRect(&sx, &sy, &w, &h)) {
        return;
    }

    cellX0 = (uint8_t)(sx >> 4);
    cellY0 = (uint8_t)(sy >> 4);
    cellX1 = (uint8_t)((sx + w - 1u) >> 4);
    cellY1 = (uint8_t)((sy + h - 1u) >> 4);
    for (cellY = cellY0; cellY <= cellY1; ++cellY) {
        for (cellX = cellX0; cellX <= cellX1; ++cellX) {
            bg1TextHideCell((uint8_t)((cellY * PORT_BG1_TEXT_SCREEN_MACRO_W) + cellX));
        }
    }
}

PORT_BG1_TEXT_CODE
static uint8_t bg1TextCoverByte(uint8_t slot, uint8_t byteX, uint8_t localY)
{
    if (localY >= 16u || byteX >= 2u) {
        return 0u;
    }
    return s_bg1TextCoverBits[slot][(uint8_t)((localY * 2u) + byteX)];
}

PORT_BG1_TEXT_CODE
static void bg1TextOverlayByte(uint8_t *dst, uint8_t offset, uint8_t cover, uint8_t ink)
{
    uint8_t keepMask = (uint8_t)~cover;

    dst[offset] = (uint8_t)((dst[offset] & keepMask) | cover);
    dst[(uint8_t)(offset + 1u)] =
        (uint8_t)((dst[(uint8_t)(offset + 1u)] & keepMask) | (uint8_t)(cover & (uint8_t)~ink));
}

PORT_BG1_TEXT_CODE
static void bg1TextBuildDmaSlot(uint8_t slot, uint8_t *dst)
{
    uint8_t y;
    const uint8_t *ink = s_bg1TextInkBits[slot];

    memset(dst, 0, PORT_BG1_TEXT_DMA_SLOT_BYTES);

    for (y = 0u; y < 8u; ++y) {
        uint8_t leftOffset = (uint8_t)(y * 2u);
        uint8_t rightOffset = (uint8_t)(leftOffset + 1u);
        uint8_t leftInk = ink[leftOffset];
        uint8_t rightInk = ink[rightOffset];
        uint8_t leftCover = bg1TextCoverByte(slot, 0u, y);
        uint8_t rightCover = bg1TextCoverByte(slot, 1u, y);
        uint8_t topLeft = (uint8_t)(y * 2u);
        uint8_t topRight = (uint8_t)(16u + (y * 2u));
        uint8_t bottomLeft = (uint8_t)(PORT_BG1_TEXT_SLOT_BOTTOM_OFFSET + (y * 2u));
        uint8_t bottomRight = (uint8_t)(PORT_BG1_TEXT_SLOT_BOTTOM_OFFSET + 16u + (y * 2u));

        bg1TextOverlayByte(dst, topLeft, leftCover, leftInk);
        bg1TextOverlayByte(dst, topRight, rightCover, rightInk);

        leftOffset = (uint8_t)((y + 8u) * 2u);
        rightOffset = (uint8_t)(leftOffset + 1u);
        leftInk = ink[leftOffset];
        rightInk = ink[rightOffset];
        leftCover = bg1TextCoverByte(slot, 0u, (uint8_t)(y + 8u));
        rightCover = bg1TextCoverByte(slot, 1u, (uint8_t)(y + 8u));
        bg1TextOverlayByte(dst, bottomLeft, leftCover, leftInk);
        bg1TextOverlayByte(dst, bottomRight, rightCover, rightInk);
    }
}

// Pre-build text DMA staging.  Called from main thread before VBlank.
// Builds map rows and tile slot data into static buffers so the NMI
// handler can DMA them without any computation.
PORT_BG1_TEXT_CODE
static PORT_NOINLINE void bg1TextPreBuildDma(void)
{
    uint8_t row;

    s_textMapDmaCount = 0u;
    s_textSlotDmaCount = 0u;

    if (!s_bg1TextAnySlotDirty && s_bg1TextMapDirtyRowBits == 0u) {
        return;
    }

    // Pre-build up to 2 dirty map rows
    if (s_bg1TextMapDirtyRowBits != 0u) {
        for (row = 0u; row < PORT_BG1_TEXT_SCREEN_MACRO_H; ++row) {
            if (s_textMapDmaCount >= TEXT_MAX_MAP_DMA) break;
            uint16_t dirtyMask = (uint16_t)(1u << row);
            if ((s_bg1TextMapDirtyRowBits & dirtyMask) == 0u) continue;

            uint8_t idx = s_textMapDmaCount;
            volatile uint8_t col;
            memset(s_textMapDmaBufs[idx], 0, sizeof(s_textMapDmaBufs[0]));
            for (col = 0u; col < PORT_BG1_TEXT_SCREEN_MACRO_W; ++col) {
                uint8_t cell = (uint8_t)(row * PORT_BG1_TEXT_SCREEN_MACRO_W + (uint8_t)col);
                uint8_t activeSlot = s_bg1TextCellSlot[cell];
                if (activeSlot != PORT_BG1_TEXT_SLOT_FREE) {
                    s_textMapDmaBufs[idx][col] = (uint16_t)(PORT_BG1_TEXT_ATTR | bg1TextSlotTile(activeSlot));
                }
            }
            s_textMapDmaDst[idx] = (uint16_t)(PORT_BG1_TEXT_TILEMAP_BASE + ((uint16_t)row * 32u * 2u));
            s_bg1TextMapDirtyRowBits &= (uint16_t)~dirtyMask;
            s_textMapDmaCount++;
        }
    }

    // Pre-build up to 2 dirty tile slots
    if (s_bg1TextAnySlotDirty) {
        for (row = 0u; row < PORT_BG1_TEXT_SLOT_ROW_COUNT; ++row) {
            while (s_bg1TextSlotDirtyBits[row] != 0u && s_textSlotDmaCount < PORT_BG1_TEXT_DMA_STAGE_COUNT) {
                uint8_t dirty = s_bg1TextSlotDirtyBits[row];
                uint8_t bit;
                bool found = false;
                for (bit = 0u; bit < 8u; ++bit) {
                    uint8_t slot = (uint8_t)((row * 8u) + bit);
                    if (slot >= PORT_BG1_TEXT_SLOT_COUNT) break;
                    if ((dirty & (uint8_t)(1u << bit)) != 0u) {
                        uint8_t idx = s_textSlotDmaCount;
                        uint8_t *stage = s_bg1TextDmaSlotBytes[idx];
                        s_textSlotDmaTile[idx] = bg1TextSlotTile(slot);
                        bg1TextBuildDmaSlot(slot, stage);
                        s_bg1TextSlotDirtyBits[row] &= (uint8_t)~(uint8_t)(1u << bit);
                        s_textSlotDmaCount++;
                        found = true;
                        break;
                    }
                }
                if (!found) break;
            }
        }
        // Check if all slots are now clean
        bool anyLeft = false;
        for (row = 0u; row < PORT_BG1_TEXT_SLOT_ROW_COUNT; ++row) {
            if (s_bg1TextSlotDirtyBits[row] != 0u) { anyLeft = true; break; }
        }
        if (!anyLeft) s_bg1TextAnySlotDirty = false;
    }
}

// DMA-only text flush.  Called from NMI handler — zero computation.
PORT_BG1_TEXT_CODE
static void bg1TextDmaFlush(void)
{
    snesXC_setDataBank(0x7Eu);

    for (uint8_t i = 0u; i < s_textMapDmaCount; i++) {
        LoadVram((const unsigned char *)s_textMapDmaBufs[i],
                 s_textMapDmaDst[i],
                 sizeof(s_textMapDmaBufs[0]));
    }

    for (uint8_t i = 0u; i < s_textSlotDmaCount; i++) {
        uint16_t tile = s_textSlotDmaTile[i];
        uint8_t *stage = s_bg1TextDmaSlotBytes[i];
        LoadVram(&stage[0u],
                 (uint16_t)(PORT_BG1_TEXT_TILEDATA_BASE + (tile * 16u)),
                 32u);
        LoadVram(&stage[PORT_BG1_TEXT_SLOT_BOTTOM_OFFSET],
                 (uint16_t)(PORT_BG1_TEXT_TILEDATA_BASE + ((uint16_t)(tile + 16u) * 16u)),
                 32u);
    }
}

// Zero a VRAM region via direct register writes.  This bypasses DMA so
// the compiler cannot eliminate the stores through LTO.
static void clearVramZero(uint16_t vramByteAddr, uint16_t wordCount) {
    volatile uint16_t i;
    REG_VMAIN = 0x80;
    REG_VMADD = (uint16_t)(vramByteAddr >> 1);
    for (i = 0; i < wordCount; ++i) {
        REG_VMDATA = 0;
    }
}

// ============================================================
// Snow particle system (BG1 overlay, pre-rendered tile approach)
// ============================================================
#define SNOW_COUNT            25u
#define SNOW_CELL_NONE        0xFFu
#define SNOW_VISIBLE_COLS     16u
#define SNOW_VISIBLE_ROWS     14u
#define SNOW_TILE_CHAR_BASE   256u      // Character 256 = VRAM $D000
#define SNOW_TILE_VRAM_BASE   0xD000u
#define SNOW_TILE_VRAM_WORDS  512u      // 1024 bytes covers chars 256-319
#define SNOW_PALETTE_NUM      2u        // BG1 palette 2 (CGRAM colors 8-11)
#define SNOW_CGRAM_OFFSET     8u
#define SNOW_TILE_ATTR        ((uint16_t)((SNOW_PALETTE_NUM << 10) | 0x2000u))
#define SNOW_TILEMAP_WORD_BASE ((uint16_t)(PORT_BG1_TEXT_TILEMAP_BASE >> 1))

static uint16_t s_snowX[SNOW_COUNT];
static uint8_t  s_snowY[SNOW_COUNT];
static uint16_t s_snowSpd[SNOW_COUNT];
static uint8_t  s_snowPhase[SNOW_COUNT];
static uint8_t  s_snowPrevCell[SNOW_COUNT];
static uint16_t s_snowRng = 0x1234u;
static bool     s_snowActive = false;
static uint8_t  s_snowScanStart = 0u; // round-robin start row for DMA budget

// Pre-built DMA staging: row buffers built before VBlank, blasted during VBlank.
// Capped at 4 rows to fit RAM budget; excess rows deferred to next frame.
#define SNOW_MAX_DMA_ROWS 4u
static uint16_t s_snowDmaBufs[SNOW_MAX_DMA_ROWS][SNOW_VISIBLE_COLS]; // 128 bytes
static uint8_t  s_snowDmaRowNums[SNOW_MAX_DMA_ROWS];
static uint8_t  s_snowDmaCount = 0u;

PORT_BG1_TEXT_CODE
static uint8_t snow_rand_u8(void)
{
    s_snowRng = (uint16_t)((s_snowRng * 109u) + 89u);
    return (uint8_t)(s_snowRng >> 8);
}

PORT_BG1_TEXT_CODE
static uint8_t snow_rand_range_u8(uint8_t lo, uint8_t hi)
{
    return (uint8_t)(lo + (snow_rand_u8() % (uint8_t)(hi - lo + 1u)));
}

PORT_BG1_TEXT_CODE
static int8_t snowSineValue(uint8_t phase)
{
    if (phase >= 6u && phase <= 26u) return 1;
    if (phase >= 38u && phase <= 58u) return -1;
    return 0;
}

PORT_BG1_TEXT_CODE
static uint8_t snowCellFromPosition(uint16_t x, uint8_t y)
{
    uint8_t px = (uint8_t)(x >> 8);
    uint8_t cx = (uint8_t)(px >> 4);
    uint8_t cy = (uint8_t)(y >> 4);
    return (cx < SNOW_VISIBLE_COLS && cy < SNOW_VISIBLE_ROWS)
         ? (uint8_t)(cy * SNOW_VISIBLE_COLS + cx)
         : SNOW_CELL_NONE;
}

PORT_BG1_TEXT_CODE
static void snowGenerateTiles(void)
{
    clearVramZero(SNOW_TILE_VRAM_BASE, SNOW_TILE_VRAM_WORDS);
    REG_VMAIN = 0x80u;
    // 16 variants, each with a 2x2 white dot at a quantised position.
    // PICO-8 screen is 128px; SNES is 256px, so 2x2 matches proportions.
    for (uint8_t v = 0u; v < 16u; v++) {
        uint8_t qx     = v & 3u;
        uint8_t qy     = v >> 2;
        uint8_t dotX   = (uint8_t)(qx * 4u + 1u); // 1, 5, 9, 13
        uint8_t dotY   = (uint8_t)(qy * 4u + 1u);
        uint8_t subCol = (dotX >= 8u) ? 1u : 0u;
        uint8_t subRow = (dotY >= 8u) ? 1u : 0u;
        uint8_t localX = dotX & 7u;
        uint8_t localY = dotY & 7u;
        uint16_t baseChar = (uint16_t)(SNOW_TILE_CHAR_BASE
                            + (uint16_t)(v >> 3) * 32u
                            + (uint16_t)(v & 7u) * 2u);
        uint16_t subChar  = (uint16_t)(baseChar
                            + (uint16_t)subRow * 16u
                            + (uint16_t)subCol);
        uint16_t vramBase = (uint16_t)(0x6000u + subChar * 8u);
        // 2-pixel-wide mask: two adjacent bits in bp0
        uint16_t data = (uint16_t)((1u << (7u - localX)) | (1u << (6u - localX)));
        // Write two rows for a 2x2 dot
        REG_VMADD  = (uint16_t)(vramBase + (uint16_t)localY);
        REG_VMDATA = data;
        REG_VMADD  = (uint16_t)(vramBase + (uint16_t)localY + 1u);
        REG_VMDATA = data;
    }
}

PORT_BG1_TEXT_CODE
static void snowUploadPalette(void)
{
    // Direct register writes — avoids DMA bank-pointer issues
    REG_CGADD = SNOW_CGRAM_OFFSET;
    REG_CGDATA = 0x00u; REG_CGDATA = 0x00u; // color 0: transparent
    REG_CGDATA = 0xFFu; REG_CGDATA = 0x7Fu; // color 1: white  (0x7FFF)
    REG_CGDATA = 0x18u; REG_CGDATA = 0x63u; // color 2: lt grey (0x6318)
    REG_CGDATA = 0x00u; REG_CGDATA = 0x00u; // color 3: unused
}

PORT_BG1_TEXT_CODE
static void snowInitParticles(void)
{
    for (uint8_t i = 0u; i < SNOW_COUNT; i++) {
        s_snowX[i]    = (uint16_t)snow_rand_u8() << 8;
        s_snowY[i]    = snow_rand_range_u8(0u, 223u);
        // PICO-8 speed 0.25-5.0 px/frame at 30fps, 8.8 fixed point
        uint8_t rawSpd = snow_rand_range_u8(2u, 40u);
        s_snowSpd[i]   = (uint16_t)rawSpd << 5;
        s_snowPhase[i] = snow_rand_u8() & 63u;
        s_snowPrevCell[i] = SNOW_CELL_NONE;
    }
    s_snowActive = true;
}

PORT_BG1_TEXT_CODE
static void snowInit(void)
{
    snowGenerateTiles();
    snowUploadPalette();
    snowInitParticles();
}

PORT_BG1_TEXT_CODE
static void snowResetPrevCells(void)
{
    for (uint8_t i = 0u; i < SNOW_COUNT; i++) {
        s_snowPrevCell[i] = SNOW_CELL_NONE;
    }
}

PORT_BG1_TEXT_CODE
static void snowUpdate(void)
{
    if (!s_snowActive || s_titleMode) return;
    for (uint8_t i = 0u; i < SNOW_COUNT; i++) {
        uint8_t oldCell = snowCellFromPosition(s_snowX[i], s_snowY[i]);
        uint16_t oldX = s_snowX[i];
        s_snowX[i] += s_snowSpd[i];
        if (s_snowX[i] < oldX) {
            s_snowY[i]    = snow_rand_range_u8(0u, 223u);
            s_snowPhase[i] = snow_rand_u8() & 63u;
        }
        uint8_t phaseInc = (uint8_t)((s_snowSpd[i] >> 8) + 1u);
        s_snowPhase[i]   = (s_snowPhase[i] + phaseInc) & 63u;
        int16_t newY     = (int16_t)s_snowY[i] + (int16_t)snowSineValue(s_snowPhase[i]);
        if (newY < 0)    newY = 0;
        if (newY > 223)  newY = 223;
        s_snowY[i] = (uint8_t)newY;
        s_snowPrevCell[i] = oldCell;
    }
}

// Pre-build all snow DMA row buffers.  Called BEFORE the VBlank wait so
// the computation is free.  Merges text + snow entries per row.
PORT_BG1_TEXT_CODE
static void snowPreBuildRows(void)
{
    s_snowDmaCount = 0u;
    if (!s_snowActive || s_titleMode) return;

    // Collect dirty rows: current positions first (high priority),
    // then previous positions (low priority — just clearing).
    uint16_t currentRows = 0u;
    uint16_t prevOnlyRows = 0u;
    for (uint8_t i = 0u; i < SNOW_COUNT; i++) {
        uint8_t cy = s_snowY[i] >> 4;
        uint8_t cx = (uint8_t)(s_snowX[i] >> 8) >> 4;
        if (cx < SNOW_VISIBLE_COLS && cy < SNOW_VISIBLE_ROWS)
            currentRows |= (uint16_t)(1u << cy);
        uint8_t prev = s_snowPrevCell[i];
        if (prev != SNOW_CELL_NONE)
            prevOnlyRows |= (uint16_t)(1u << (prev / SNOW_VISIBLE_COLS));
    }
    prevOnlyRows &= (uint16_t)~currentRows; // only rows that JUST have departures
    uint16_t priorityMask = currentRows; // do these first

    // Build each dirty row into the staging buffer (current rows first).
    // Round-robin the scan start so all rows get fair DMA budget over time.
    for (uint8_t pass = 0u; pass < 2u; pass++) {
        uint16_t mask = (pass == 0u) ? priorityMask : prevOnlyRows;
        for (uint8_t offset = 0u; offset < SNOW_VISIBLE_ROWS && mask != 0u; offset++) {
            uint8_t row = (uint8_t)((s_snowScanStart + offset) % SNOW_VISIBLE_ROWS);
            if (!(mask & (1u << row))) continue;
            if (s_snowDmaCount >= SNOW_MAX_DMA_ROWS) goto done;

            uint8_t idx = s_snowDmaCount;
            s_snowDmaRowNums[idx] = row;
            memset(s_snowDmaBufs[idx], 0, sizeof(s_snowDmaBufs[0]));

            // Text entries first (text takes priority over snow)
            for (uint8_t col = 0u; col < SNOW_VISIBLE_COLS; col++) {
                uint8_t cell = (uint8_t)(row * SNOW_VISIBLE_COLS + col);
                uint8_t slot = s_bg1TextCellSlot[cell];
                if (slot != PORT_BG1_TEXT_SLOT_FREE)
                    s_snowDmaBufs[idx][col] = (uint16_t)(PORT_BG1_TEXT_ATTR | bg1TextSlotTile(slot));
            }

            // Snow entries for empty cells
            for (uint8_t i = 0u; i < SNOW_COUNT; i++) {
                uint8_t cy = s_snowY[i] >> 4;
                if (cy != row) continue;
                uint8_t px = (uint8_t)(s_snowX[i] >> 8);
                uint8_t cx = px >> 4;
                if (cx >= SNOW_VISIBLE_COLS || s_snowDmaBufs[idx][cx] != 0u) continue;

                uint8_t qx = (px & 15u) >> 2;
                uint8_t qy = (s_snowY[i] & 15u) >> 2;
                uint8_t variant = (uint8_t)((qy << 2) | qx);
                uint16_t tileChar = (uint16_t)(SNOW_TILE_CHAR_BASE
                                    + (uint16_t)(variant >> 3) * 32u
                                    + (uint16_t)(variant & 7u) * 2u);
                s_snowDmaBufs[idx][cx] = (uint16_t)(SNOW_TILE_ATTR | tileChar);
            }

            s_snowDmaCount++;
        }
    }
done:
    s_snowScanStart = (uint8_t)((s_snowScanStart + SNOW_MAX_DMA_ROWS) % SNOW_VISIBLE_ROWS);
}

// VBlank-only: tight DMA loop, zero computation.
PORT_BG1_TEXT_CODE
static void snowFlushTilemap(void)
{
    snesXC_setDataBank(0x7Eu);
    for (uint8_t i = 0u; i < s_snowDmaCount; i++) {
        LoadVram((const unsigned char *)s_snowDmaBufs[i],
                 (uint16_t)(PORT_BG1_TEXT_TILEMAP_BASE + (uint16_t)s_snowDmaRowNums[i] * 64u),
                 sizeof(s_snowDmaBufs[0]));
    }
}

static void LoadRoomDataVRAM(void) {
    LoadVram((uint8_t *)s_tilemapBg2, 0x2000, GLOBAL_ActiveLevel.roomSizeX * GLOBAL_ActiveLevel.roomSizeY * 4); //Bg2 tilemap
    LoadVram((uint8_t *)s_tilemapBg3, 0x4000, GLOBAL_ActiveLevel.roomSizeX * GLOBAL_ActiveLevel.roomSizeY * 4); //Bg3 tilemap
    // Clear BG1 tilemap and tile data via direct VRAM writes so LTO
    // cannot eliminate the zeroing (DMA source buffers are invisible to
    // the compiler).
    clearVramZero(PORT_BG1_TEXT_TILEMAP_BASE, PORT_BG_TILEMAP_BYTES / 2); //Bg1 tilemap
    clearVramZero(PORT_BG1_TEXT_TILEDATA_BASE, 0x1000 / 2);              //Bg1 tile data ($C000-$CFFF)
    LoadCGRam(paletteBg, 0x0040, 0x40); //Bg3 palette
    LoadCGRam(paletteBg, 0x0020, 0x40); //Bg2 palette
    bg1TextUploadPalette();
    bg1TextReset();
    port_prg_bank_enter(4);
    snowUploadPalette();
    snowResetPrevCells();
    port_prg_bank_leave();
}

void port_restoreCollisionFlags(void) {
    for (uint16_t i = 0; i < 256; i++) {
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = GLOBAL_ActiveLevel.collisionFlagsReset[i];
    }
}

void port_LoadRoomData(uint16_t roomID) {
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = false;
    REG_INIDISP = 0x8F;
    if (roomID >= 1 && roomID <= 12) {
        snesXC_setDataBank(BANK_LEVELS_00);
    } else if (roomID >= 13 && roomID <= 24) {
        snesXC_setDataBank(BANK_LEVELS_01);
    } else if (roomID >= 25 && roomID <= 32) {
        snesXC_setDataBank(BANK_LEVELS_02);
    }
    memset(paletteBg, 0, sizeof(paletteBg));

    switch (roomID) {
        case 1:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level1_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level1_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level1, sizeof(palette_level1));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level1, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL1;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL1;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL1_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level1, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 2:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level2_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level2_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level2, sizeof(palette_level2));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level2, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL2;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL2;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL2_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level2, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 3:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level3_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level3_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level3, sizeof(palette_level3));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level3, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL3;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL3;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL3_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level3, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 4:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level4_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level4_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level4, sizeof(palette_level4));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level4, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL4;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL4;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL4_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level4, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 5:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level5_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level5_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level5, sizeof(palette_level5));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level5, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL5;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL5;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL5_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level5, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 6:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level6_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level6_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level6, sizeof(palette_level6));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level6, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL6;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL6;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL6_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level6, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 7:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level7_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level7_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level7, sizeof(palette_level7));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level7, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL7;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL7;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL7_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level7, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 8:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level8_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level8_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level8, sizeof(palette_level8));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level8, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL8;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL8;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL8_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level8, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 9:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level9_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level9_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level9, sizeof(palette_level9));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level9, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL9;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL9;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL9_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level9, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 10:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level10_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level10_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level10, sizeof(palette_level10));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level10, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL10;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL10;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL10_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level10, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 11:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level11_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level11_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level11, sizeof(palette_level11));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level11, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL11;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL11;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL11_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level11, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 12:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level12_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level12_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level12, sizeof(palette_level12));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level12, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL12;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL12;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL12_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level12, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 13:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level13_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level13_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level13, sizeof(palette_level13));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level13, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL13;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL13;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL13_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level13, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 14:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level14_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level14_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level14, sizeof(palette_level14));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level14, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL14;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL14;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL14_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level14, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 15:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level15_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level15_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level15, sizeof(palette_level15));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level15, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL15;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL15;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL15_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level15, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 16:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level16_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level16_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level16, sizeof(palette_level16));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level16, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL16;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL16;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL16_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level16, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 17:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level17_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level17_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level17, sizeof(palette_level17));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level17, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL17;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL17;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL17_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level17, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 18:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level18_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level18_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level18, sizeof(palette_level18));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level18, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL18;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL18;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL18_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level18, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 19:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level19_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level19_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level19, sizeof(palette_level19));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level19, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL19;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL19;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL19_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level19, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 20:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level20_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level20_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level20, sizeof(palette_level20));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level20, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL20;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL20;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL20_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level20, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 21:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level21_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level21_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level21, sizeof(palette_level21));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level21, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL21;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL21;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL21_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level21, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 22:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level22_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level22_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level22, sizeof(palette_level22));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level22, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL22;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL22;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL22_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level22, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 23:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level23_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level23_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level23, sizeof(palette_level23));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level23, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL23;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL23;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL23_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level23, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 24:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level24_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level24_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level24, sizeof(palette_level24));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level24, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL24;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL24;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL24_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level24, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 25:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level25_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level25_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level25, sizeof(palette_level25));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level25, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL25;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL25;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL25_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level25, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 26:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level26_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level26_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level26, sizeof(palette_level26));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level26, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL26;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL26;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL26_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level26, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 27:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level27_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level27_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level27, sizeof(palette_level27));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level27, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL27;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL27;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL27_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level27, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 28:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level28_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level28_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level28, sizeof(palette_level28));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level28, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL28;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL28;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL28_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level28, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 29:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level29_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level29_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level29, sizeof(palette_level29));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level29, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL29;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL29;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL29_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level29, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 30:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level30_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level30_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level30, sizeof(palette_level30));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level30, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL30;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL30;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL30_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level30, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 31:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level31_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level31_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level31, sizeof(palette_level31));
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.collisionFlagsReset, collision_level31, sizeof(GLOBAL_ActiveLevel.collisionFlagsReset));

            GLOBAL_ActiveLevel.playerSpawnX = SPAWN_X_LEVEL31;
            GLOBAL_ActiveLevel.playerSpawnY = SPAWN_Y_LEVEL31;
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL31_COUNT;
            snesXC_memcpy_banked(GLOBAL_ActiveLevel.objectData, object_level31, GLOBAL_ActiveLevel.objectCount * 3);
            break;
        case 32:
            snesXC_memcpy_banked(s_tilemapBg2, tilemap_level32_bg2, sizeof(s_tilemapBg2));
            snesXC_memcpy_banked(s_tilemapBg3, tilemap_level32_bg3, PORT_BG_TILEMAP_BYTES);
            snesXC_memcpy_banked(paletteBg, palette_level32, sizeof(palette_level32));
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
    snesXC_setDataBank(BANK_ASSETS);
    bg1TextReset();
    s_altPaletteApplied = false;
    s_prevTextFlashActive = false;
    s_textFlashPhase = 0;
    memset(s_bg1PaletteCurrent, 0, sizeof(s_bg1PaletteCurrent));
    s_bg1PaletteCurrent[2] = 0xFF;
    s_bg1PaletteCurrent[3] = 0x7F;
    
    // Load cloud graphics (from rom_data_bank_1). BG4 uses the high BG34
    // tile-data region so dynamic BG1 text can share BG2 tiles at $A000.
    LoadVram(clouds_tiles, 0xE000, sizeof(clouds_tiles));
    LoadCGRam((char *)clouds_palette, 0x0060, sizeof(clouds_palette));
    LoadVram((char *)clouds_map, 0x0000, sizeof(clouds_map));
    
    // Load sprite graphics (from rom_data_bank_1)
    LoadVram(sprite_gfx_4bpp, 0x8000, sizeof(sprite_gfx_4bpp));
    LoadVram(sprite_gfx_2bpp, 0xA000, (uint16_t)(PORT_GAME_2BPP_TILE_COUNT * 16u));
    LoadVram(score_1000_sprite_top_4bpp, SCORE_1000_SPRITE_VRAM_TOP, sizeof(score_1000_sprite_top_4bpp));
    LoadVram(score_1000_sprite_bottom_4bpp, SCORE_1000_SPRITE_VRAM_BOTTOM, sizeof(score_1000_sprite_bottom_4bpp));
    
    // Load all sprite palettes (from rom_data_bank_1)
    LoadCGRam((char *)sprite_palettes_4bpp[0], 0x0080, sizeof(sprite_palettes_4bpp[0])); // Player
    LoadCGRam((char *)sprite_palettes_4bpp[0], 0x0090, sizeof(sprite_palettes_4bpp[0])); // Smoke
    LoadCGRam((char *)sprite_palettes_4bpp[1], 0x00A0, sizeof(sprite_palettes_4bpp[0])); // Breakable wall
    LoadCGRam((char *)sprite_palettes_4bpp[2], 0x00B0, sizeof(sprite_palettes_4bpp[0])); // Flower
    LoadCGRam((char *)sprite_palettes_4bpp[3], 0x00C0, sizeof(sprite_palettes_4bpp[0])); // Strawberry
    LoadCGRam((char *)sprite_palettes_4bpp[4], 0x00D0, sizeof(sprite_palettes_4bpp[0])); // Deco tree
    LoadCGRam((char *)sprite_palettes_4bpp[5], 0x00E0, sizeof(sprite_palettes_4bpp[0])); // Spring
    LoadCGRam((char *)sprite_palettes_4bpp[6], 0x00F0, sizeof(sprite_palettes_4bpp[0])); // Flying berry
    snesXC_setDataBank(BANK_00);
    scoreSpriteUploadPalette(false);
    bg1TextUploadPalette();
    port_prg_bank_enter(4);
    snowInit();
    port_prg_bank_leave();
}


void port_showGameplayScreen(void)
{
    // Restore the gameplay layer routing after title mode. BG1 is the
    // high-priority dynamic text layer in mode 0.
    REG_BGMODE = 0xF0;
    REG_BG1SC = (0x6000ul >> (9)) | 0x00u;
    REG_BG2SC = (0x2000ul >> (9)) | 0x00u;
    REG_BG12NBA = BG12NBA_GAMEPLAY;
    REG_BG3SC = (0x4000ul >> (9)) | 0x00u;
    REG_BG4SC = (0x0000ul >> (9)) | 0x03u;
    REG_BG34NBA = BG34NBA_GAMEPLAY;
    REG_TM = 0x1F;
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

    port_showGameplayScreen();

    REG_NMITIMEN = 0x81u; // Enable NMI + auto-joypad read
    REG_INIDISP = 0x0F;
    port_audioInit();
 
}

void port_showTitleScreen(void)
{
    REG_INIDISP = 0x8F;
    // Title only displays BG3, so keep BG3 at 16x16 in mode 0.
    REG_BGMODE = 0xE0;
    // Route BG3 tile data to dedicated title VRAM region (0x6000)
    // so gameplay tiles at 0xA000 remain untouched.
    REG_BG34NBA = BG34NBA_TITLE;
    snesXC_setDataBank(BANK_TITLE);
    // Title uses BG3 in mode 0 (same layer class as gameplay backgrounds).
    LoadVram(title_tiledata_snes_2bpp, 0x6000, sizeof(title_tiledata_snes_2bpp));
    LoadVram((const unsigned char *)title_tilemap_bg3_snes, 0x4000, sizeof(title_tilemap_bg3_snes));
    // Mode 0 gives each BG its own 32-color CGRAM block. Title uses BG3 with
    // palette index 0, so its colors live at the BG3 base ($40), not CGRAM 0.
    LoadCGRam((const unsigned char *)title_palette_snes_bgr15, 0x0040, sizeof(title_palette_snes_bgr15));
    snesXC_setDataBank(BANK_00);

    // Show only BG3 for title to avoid gameplay-layer artifacts.
    REG_TM = 0x04;
    port_resetSprites();
    REG_INIDISP = 0x0F;
}

void port_setTitleMode(bool enabled)
{
    s_titleMode = enabled;
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
    int16_t screenY = sprite_to_screen_y((int16_t)(playerData->pos.y << 1));
    uint8_t table2Index = 0u; // Sprite 0 is at index 0
    uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
    // Calculate X high bit (bit 8 of X coordinate)
    int16_t renderX = (int16_t)(playerData->pos.x << 1);
    uint8_t xBit = (uint8_t)((renderX < 0 || renderX >= 256) ? 1u : 0u);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(0u, 1u, xBit, currentByte);
    // Write directly to bytes array to avoid vbcc65816 struct layout issues
    // SNES OAM format: byte 0 = X, byte 1 = Y, byte 2 = Tile, byte 3 = Properties
    GLOBAL_OAMCopy.Bytes[0] = (uint8_t)renderX;  // X position (low 8 bits)
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

void port_buildFlag(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    (void)writeFlagSprite(index, obj);
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
    if (obj->data.strawberry.isCollected != 0u) {
        (void)writeBerryScoreSprite(index, obj);
    } else {
        writeConditionalSprite(index, obj, false);
    }
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
    if (obj->data.flyingBerry.isCollected != 0u) {
        (void)writeBerryScoreSprite(index, obj);
    } else {
        (void)writeFlyingBerrySprite(index, obj);
    }
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
    } else if (eType == OBJ_FLAG) {
        port_buildFlag(index);
    }
}

PORT_BG1_TEXT_CODE
static PORT_NOINLINE __attribute__((optnone)) void port_drawTextWithColorsLen(const unsigned char *text,
                                                                              uint8_t textLength,
                                                                              uint8_t x,
                                                                              uint8_t y,
                                                                              uint8_t inkColor,
                                                                              uint8_t bgColor,
                                                                              bool fillBackground,
                                                                              uint8_t advancePx,
                                                                              uint8_t inkXOffsetPx,
                                                                              uint8_t cellYOffsetPx)
{
    uint16_t sx = (uint16_t)x << 1;
    uint16_t sy = (uint16_t)y << 1;
    uint16_t bgX;
    uint16_t bgW;
    uint16_t penX = sx;
    uint16_t i;

    if ((text == NULL) || sx >= 256u || sy >= 256u) {
        return;
    }

    if (textLength == 0u) {
        return;
    }
    bgX = sx;
    bgW = (uint16_t)(((uint16_t)textLength * advancePx +
                      PORT_PICO8_TEXT_BG_PAD_RIGHT_PX) * PORT_PICO8_FONT_SCALE);
    if (textLength > 1u && text[0] == (uint8_t)' ' && text[1] == (uint8_t)' ') {
        bg1TextHideRect(bgX, sy, bgW, 16u);
        return;
    }

    if (fillBackground) {
        bg1TextFillRectNoPublish(bgX, sy,
                                 bgW,
                                 16u,
                                 bgColor);
    }

    for (i = 0; i < textLength; ++i) {
        uint8_t ch = (uint8_t)(text[i] & 0x7Fu);
        uint8_t gy;

        if (ch != (uint8_t)' ') {
            bg1TextFetchGlyphRows(ch);
            for (gy = 0; gy < 8u; ++gy) {
                uint8_t row = s_pico8GlyphRows[gy];
                uint8_t gx;
                if (row == 0u) {
                    continue;
                }
                for (gx = 0; gx < PORT_PICO8_FONT_GLYPH_W_PX; ++gx) {
                    if ((row & (uint8_t)(0x80u >> gx)) != 0u) {
                        uint16_t px = (uint16_t)(penX + ((uint16_t)inkXOffsetPx * PORT_PICO8_FONT_SCALE) + ((uint16_t)gx * PORT_PICO8_FONT_SCALE));
                        uint16_t py = (uint16_t)(sy + cellYOffsetPx + ((uint16_t)gy * PORT_PICO8_FONT_SCALE));
                        (void)bg1TextSetPixelColorNoPublish(px, py, inkColor);
                        (void)bg1TextSetPixelColorNoPublish((uint16_t)(px + 1u), py, inkColor);
                        (void)bg1TextSetPixelColorNoPublish(px, (uint16_t)(py + 1u), inkColor);
                        (void)bg1TextSetPixelColorNoPublish((uint16_t)(px + 1u), (uint16_t)(py + 1u), inkColor);
                    }
                }
            }
        }
        penX += (uint16_t)(advancePx * PORT_PICO8_FONT_SCALE);
        if (penX >= 256u) {
            break;
        }
    }

    bg1TextPublishRect(bgX, sy, bgW, 16u);
}

PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE __attribute__((optnone)) void port_drawCharWithColors(uint8_t ch,
                                                                           uint8_t x,
                                                                           uint8_t y,
                                                                           uint8_t inkColor,
                                                                           uint8_t bgColor)
{
    uint16_t sx = (uint16_t)x << 1;
    uint16_t sy = (uint16_t)y << 1;
    uint16_t pxBase;
    uint8_t gy;

    if (sx >= 256u || sy >= 256u) {
        return;
    }

    bg1TextFillRectNoPublish(sx, sy, 14u, 16u, bgColor);
    ch &= 0x7Fu;
    if (ch != (uint8_t)' ') {
        bg1TextFetchGlyphRows(ch);
        pxBase = (uint16_t)(sx + (PORT_PICO8_TEXT_INK_X_OFFSET_PX * PORT_PICO8_FONT_SCALE));
        for (gy = 0; gy < 8u; ++gy) {
            uint8_t row = s_pico8GlyphRows[gy];
            uint8_t gx;
            uint16_t py;
            if (row == 0u) {
                continue;
            }
            py = (uint16_t)(sy + PORT_PICO8_FONT_CELL_Y_OFFSET + ((uint16_t)gy * PORT_PICO8_FONT_SCALE));
            for (gx = 0; gx < PORT_PICO8_FONT_GLYPH_W_PX; ++gx) {
                if ((row & (uint8_t)(0x80u >> gx)) != 0u) {
                    uint16_t px = (uint16_t)(pxBase + ((uint16_t)gx * PORT_PICO8_FONT_SCALE));
                    (void)bg1TextSetPixelColorNoPublish(px, py, inkColor);
                    (void)bg1TextSetPixelColorNoPublish((uint16_t)(px + 1u), py, inkColor);
                    (void)bg1TextSetPixelColorNoPublish(px, (uint16_t)(py + 1u), inkColor);
                    (void)bg1TextSetPixelColorNoPublish((uint16_t)(px + 1u), (uint16_t)(py + 1u), inkColor);
                }
            }
        }
    }
    bg1TextPublishRect(sx, sy, 14u, 16u);
}

PORT_BG1_TEXT_CODE
static PORT_NOINLINE __attribute__((optnone)) void port_drawTextWithColors(const unsigned char *text,
                                                                           uint8_t x,
                                                                           uint8_t y,
                                                                           uint8_t inkColor,
                                                                           uint8_t bgColor,
                                                                           bool fillBackground)
{
    uint8_t textLength;

    if (text == NULL) {
        return;
    }

    textLength = (uint8_t)strlen((const char *)text);
    port_drawTextWithColorsLen(text,
                               textLength,
                               x,
                               y,
                               inkColor,
                               bgColor,
                               fillBackground,
                               PORT_PICO8_FONT_ADVANCE_PX,
                               PORT_PICO8_TEXT_INK_X_OFFSET_PX,
                               PORT_PICO8_FONT_CELL_Y_OFFSET);
}

PORT_BG1_TEXT_CODE
PORT_NOINLINE void port_drawTextN(const unsigned char *text, uint8_t length, uint8_t x, uint8_t y)
{
    port_drawTextWithColorsLen(text,
                               length,
                               x,
                               y,
                               PORT_BG1_TEXT_COLOR_INK,
                               PORT_BG1_TEXT_COLOR_BG,
                               true,
                               PORT_PICO8_FONT_ADVANCE_PX,
                               PORT_PICO8_TEXT_INK_X_OFFSET_PX,
                               PORT_PICO8_FONT_CELL_Y_OFFSET);
}

PORT_BG1_TEXT_CODE
PORT_NOINLINE void port_drawTextPico8N(const unsigned char *text, uint8_t length, uint8_t x, uint8_t y)
{
    port_drawTextWithColorsLen(text,
                               length,
                               x,
                               y,
                               PORT_BG1_TEXT_COLOR_INK,
                               PORT_BG1_TEXT_COLOR_BG,
                               true,
                               4u,
                               PORT_PICO8_TEXT_INK_X_OFFSET_PX,
                               PORT_PICO8_FONT_CELL_Y_OFFSET);
}

PORT_BG1_TEXT_CODE
PORT_NOINLINE void port_drawText(const unsigned char *text, uint8_t x, uint8_t y)
{
    port_drawTextWithColors(text, x, y, PORT_BG1_TEXT_COLOR_INK, PORT_BG1_TEXT_COLOR_BG, true);
}

PORT_BG1_TEXT_CORE_CODE
PORT_NOINLINE void port_drawChar(uint8_t ch, uint8_t x, uint8_t y)
{
    port_drawCharWithColors(ch, x, y, PORT_BG1_TEXT_COLOR_INK, PORT_BG1_TEXT_COLOR_BG);
}

PORT_BG1_TEXT_CORE_CODE
PORT_NOINLINE void port_drawCharWhiteOnBlack(uint8_t ch, uint8_t x, uint8_t y)
{
    port_drawCharWithColors(ch, x, y, PORT_BG1_TEXT_COLOR_BG, PORT_BG1_TEXT_COLOR_INK);
}

PORT_BG1_TEXT_CORE_CODE
PORT_NOINLINE void port_clearChars(uint8_t x, uint8_t y, uint8_t count)
{
    uint16_t sx = (uint16_t)x << 1;
    uint16_t sy = (uint16_t)y << 1;
    uint8_t len = count > 28u ? 28u : count;
    uint16_t bgW;

    if (len == 0u || sx >= 256u || sy >= 256u) {
        return;
    }

    bgW = (uint16_t)(((uint16_t)len * PORT_PICO8_FONT_ADVANCE_PX +
                      PORT_PICO8_TEXT_BG_PAD_RIGHT_PX) * PORT_PICO8_FONT_SCALE);
    bg1TextFillRect(sx, sy, bgW, 16u, PORT_BG1_TEXT_COLOR_BG);
}

PORT_BG1_TEXT_CODE
PORT_NOINLINE void port_drawTextWhiteOnBlackN(const unsigned char *text, uint8_t length, uint8_t x, uint8_t y)
{
    port_drawTextWithColorsLen(text,
                               length,
                               x,
                               y,
                               PORT_BG1_TEXT_COLOR_BG,
                               PORT_BG1_TEXT_COLOR_INK,
                               true,
                               PORT_PICO8_FONT_ADVANCE_PX,
                               PORT_PICO8_TEXT_INK_X_OFFSET_PX,
                               PORT_PICO8_FONT_CELL_Y_OFFSET);
}

PORT_BG1_TEXT_CODE
PORT_NOINLINE void port_drawTextWhiteOnBlack(const unsigned char *text, uint8_t x, uint8_t y)
{
    port_drawTextWithColors(text, x, y, PORT_BG1_TEXT_COLOR_BG, PORT_BG1_TEXT_COLOR_INK, true);
}

PORT_BG1_TEXT_CORE_CODE
PORT_NOINLINE void port_drawTextBoxBlack(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    uint16_t sx = (uint16_t)x << 1;
    uint16_t sy = (uint16_t)y << 1;
    uint16_t ex = (uint16_t)(sx + ((uint16_t)w << 1));
    uint16_t ey = (uint16_t)(sy + ((uint16_t)h << 1));

    if (sx >= 256u || sy >= 256u || w == 0u || h == 0u) {
        return;
    }
    if (ex > 256u) {
        ex = 256u;
    }
    if (ey > 256u) {
        ey = 256u;
    }

    sx &= 0xFFF0u;
    sy &= 0xFFF0u;
    ex = (uint16_t)((ex + 15u) & 0xFFF0u);
    ey = (uint16_t)((ey + 15u) & 0xFFF0u);
    if (ex > 256u) {
        ex = 256u;
    }
    if (ey > 256u) {
        ey = 256u;
    }

    bg1TextFillRect(sx, sy, (uint16_t)(ex - sx), (uint16_t)(ey - sy), PORT_BG1_TEXT_COLOR_INK);
}

PORT_BG1_TEXT_CODE
static void renderFlagOverlay(void);

PORT_BG1_TEXT_CODE
static void renderFlagOverlay(void)
{
    if (!GLOBAL_FlagOverlayShow) {
        return;
    }

    port_drawTextBoxBlack(32u, 2u, 65u, 30u);
    port_drawTextWhiteOnBlackN(GLOBAL_FlagOverlayLine0, GLOBAL_FlagOverlayLine0Len, 64u, 9u);
    port_drawTextWhiteOnBlackN(GLOBAL_FlagOverlayLine1, GLOBAL_FlagOverlayLine1Len, 50u, 17u);
    port_drawTextWhiteOnBlackN(GLOBAL_FlagOverlayLine2, GLOBAL_FlagOverlayLine2Len, 48u, 24u);
}

void port_renderTextOverlays(void)
{
    port_prg_bank_enter(4);
    renderFlagOverlay();
    port_prg_bank_leave();
}


void port_vblank(void)
{
    uint16_t frameCountBeforeWait = GLOBAL_FrameCount;
    bool doHalfRateEffects = (uint16_t)(frameCountBeforeWait - s_effectsStepFrame) >= 2u;

    if (doHalfRateEffects) {
        s_effectsStepFrame = frameCountBeforeWait;
    }

    // =================================================================
    // PRE-VBLANK: all decisions and data preparation while display is
    // still active — costs nothing against the vblank budget.
    // =================================================================

    // Hair colour
    static uint16_t hairColour;
    if (GLOBAL_PlayerData.dashesLeft == 0) {
        hairColour = 0x7EA5;
    } else if (GLOBAL_PlayerData.dashesLeft == 1) {
        hairColour = 0x241F;
    } else {
        hairColour = 0x1B80;
    }

    bool doPaletteDma = false;
    bool doScorePaletteFlash = false;
    bool doCloudPalSwap = false;

    if (!s_titleMode) {
        bool wantAltPalette = GLOBAL_PlayerData.doubleDashUnlocked;
        bool paletteDirty = false;

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
            memset(s_bg1PaletteCurrent, 0, sizeof(s_bg1PaletteCurrent));
            s_bg1PaletteCurrent[2] = 0xFF;
            s_bg1PaletteCurrent[3] = 0x7F;
            if (s_altPaletteApplied) {
                s_bg1PaletteCurrent[0] = 0x90;
                s_bg1PaletteCurrent[1] = 0x28;
            }
            if (GLOBAL_ActiveLevel.textFlashActive && (s_textFlashPhase & 1u)) {
                s_bg1PaletteCurrent[2] = 0x1F;
                s_bg1PaletteCurrent[3] = 0x00;
            }
            doPaletteDma = true;
            doScorePaletteFlash = GLOBAL_ActiveLevel.textFlashActive && ((s_textFlashPhase & 1u) != 0u);
            memset(s_scoreSpritePalette, 0, sizeof(s_scoreSpritePalette));
            s_scoreSpritePalette[1] = doScorePaletteFlash ? 0x001Fu : 0x7FFFu;
        }

        if (GLOBAL_ActiveLevel.swapCloudPal) {
            doCloudPalSwap = true;
            GLOBAL_ActiveLevel.swapCloudPal = false;
        }

        // Pre-build text + snow DMA staging
        {
            port_prg_bank_enter(4);
            bg1TextPreBuildDma();
            if (doHalfRateEffects) {
                snowPreBuildRows();
            } else {
                s_snowDmaCount = 0u;
            }
            port_prg_bank_leave();
        }
    }

    // =================================================================
    // VBLANK SYNC: poll HVBJOY for VBlank window
    // =================================================================
    while ((REG_HVBJOY & 0x80u) != 0u) {}
    while ((REG_HVBJOY & 0x80u) == 0u) {}

    // =================================================================
    // VBLANK IO: all DMA and register writes (main thread, in VBlank)
    // =================================================================
    snesXC_setDataBank(0x7Eu);
    LoadOAMCopy((char *)GLOBAL_OAMCopy.Bytes, 0x0000, sizeof(union uOAMCopy));
    snesXC_setDataBank(BANK_00);

    REG_BG1HOFS = 0; REG_BG1HOFS = 0;
    REG_BG1VOFS = 0; REG_BG1VOFS = 0;
    {
        uint16_t sx = GLOBAL_ScrollBG2X, sy = GLOBAL_ScrollBG2Y;
        REG_BG2HOFS = (uint8_t)sx; REG_BG2HOFS = (uint8_t)(sx >> 8);
        REG_BG2VOFS = (uint8_t)sy; REG_BG2VOFS = (uint8_t)(sy >> 8);
    }
    {
        uint16_t sx = GLOBAL_ScrollBG3X, sy = GLOBAL_ScrollBG3Y;
        REG_BG3HOFS = (uint8_t)sx; REG_BG3HOFS = (uint8_t)(sx >> 8);
        REG_BG3VOFS = (uint8_t)sy; REG_BG3VOFS = (uint8_t)(sy >> 8);
    }
    {
        uint16_t sx = GLOBAL_ScrollBG4X, sy = GLOBAL_ScrollBG4Y;
        REG_BG4HOFS = (uint8_t)sx; REG_BG4HOFS = (uint8_t)(sx >> 8);
        REG_BG4VOFS = (uint8_t)sy; REG_BG4VOFS = (uint8_t)(sy >> 8);
    }

    REG_CGADD = 0xC5u;
    REG_CGDATA = (uint8_t)(hairColour);
    REG_CGDATA = (uint8_t)(hairColour >> 8);

    if (!s_titleMode) {
        if (doPaletteDma) {
            snesXC_setDataBank(0x7Eu);
            LoadCGRam((char *)s_bg1PaletteCurrent, 0x0000, sizeof(s_bg1PaletteCurrent));
            snesXC_setDataBank(BANK_00);
            snesXC_setDataBank(0x7Eu);
            LoadCGRam((const unsigned char *)s_scoreSpritePalette, PORT_BERRY_SCORE_CGRAM_OFFSET, sizeof(s_scoreSpritePalette));
            snesXC_setDataBank(BANK_00);
        }
        if (doCloudPalSwap) {
            snesXC_setDataBank(BANK_ASSETS);
            LoadCGRam((char *)clouds_palette_2, 0x0060, sizeof(clouds_palette_2));
            snesXC_setDataBank(BANK_00);
        }

        {
            port_prg_bank_enter(4);
            snesXC_setDataBank(0x7Eu);
            bg1TextDmaFlush();
            snowFlushTilemap();
            snesXC_setDataBank(BANK_00);
            port_prg_bank_leave();
        }
    }

    // =================================================================
    // POST-VBLANK: next-frame calculations
    // =================================================================
    int16_t shakeAmount = GLOBAL_ActiveLevel.shakeFrames > 0 ? ((GLOBAL_FrameCount & 1) ? 2 : -2) : 0;
    int16_t playerRenderY = (int16_t)(GLOBAL_PlayerData.objData.pos.y << 1);
    int16_t smoothScrollY = (playerRenderY - GLOBAL_ActiveLevel.scrollPointY) >> 2;

    GLOBAL_ScrollBG2Y = CLAMP(playerRenderY - 16 - GLOBAL_ActiveLevel.scrollPointY, 0, 31);
    GLOBAL_ScrollBG2X = 0;

    GLOBAL_ScrollBG3X = GLOBAL_ScrollBG2X + (shakeAmount);
    GLOBAL_ScrollBG3Y = GLOBAL_ScrollBG2Y + (shakeAmount);

    GLOBAL_ScrollBG4Y = smoothScrollY + (shakeAmount >> 1) - (GLOBAL_ActiveLevel.currentRoomID << 6);

    if (doHalfRateEffects) {
        updateCloudsVblankSafe();
        if (!s_titleMode) {
            port_prg_bank_enter(4);
            snowUpdate();
            port_prg_bank_leave();
        }
    }
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
    GLOBAL_OAMCopy.Bytes[0] = 0;    // X position
    GLOBAL_OAMCopy.Bytes[1] = 240;  // Hide sprite 0 while reset
    GLOBAL_OAMCopy.Bytes[2] = 0;  // Tile number
    GLOBAL_OAMCopy.Bytes[3] = 0;  // Properties
    GLOBAL_OAMCopy.arr.OAMTable2[0] = 0x56; //Enable the first sprite, set size to 16x16

}

void port_audioUpdate(void) {
    (void)0;
}

void port_audioInit(void) {
    if (s_spcReady) {
        GLOBAL_SpcDebugStage = 5u;
        return;
    }
    if (s_spcInitTried) {
        return;
    }
    s_spcInitTried = true;
    s_spcRuntimeReady = false;
    GLOBAL_SpcDebugStage = 1u;
    GLOBAL_SpcDebugApu0 = REG_APUIO0;
    GLOBAL_SpcDebugApu1 = REG_APUIO1;
    GLOBAL_SpcDebugApu2 = REG_APUIO2;
    GLOBAL_SpcDebugApu3 = REG_APUIO3;

    if (REG_APUIO3 == 0x99u) {
        s_spcReady = true;
        s_spcRuntimeReady = true;
        s_spcLastData = REG_APUIO0;
        GLOBAL_SpcDebugStage = 5u;
        return;
    }
    if (!spc_wait_boot()) {
        s_spcInitTried = false;
        GLOBAL_SpcDebugStage = 11u;
        GLOBAL_SpcDebugApu0 = REG_APUIO0;
        GLOBAL_SpcDebugApu1 = REG_APUIO1;
        GLOBAL_SpcDebugApu2 = REG_APUIO2;
        GLOBAL_SpcDebugApu3 = REG_APUIO3;
        return;
    }
    GLOBAL_SpcDebugStage = 2u;
    if (!spc_upload_image()) {
        s_spcInitTried = false;
        GLOBAL_SpcDebugStage = 12u;
        GLOBAL_SpcDebugApu0 = REG_APUIO0;
        GLOBAL_SpcDebugApu1 = REG_APUIO1;
        GLOBAL_SpcDebugApu2 = REG_APUIO2;
        GLOBAL_SpcDebugApu3 = REG_APUIO3;
        return;
    }
    GLOBAL_SpcDebugStage = 3u;
    if (!spc_execute(spc_start_pc)) {
        s_spcInitTried = false;
        GLOBAL_SpcDebugStage = 13u;
        GLOBAL_SpcDebugApu0 = REG_APUIO0;
        GLOBAL_SpcDebugApu1 = REG_APUIO1;
        GLOBAL_SpcDebugApu2 = REG_APUIO2;
        GLOBAL_SpcDebugApu3 = REG_APUIO3;
        return;
    }
    if (!spc_wait_runtime_ready(2000000u)) {
        s_spcInitTried = false;
        GLOBAL_SpcDebugStage = 14u;
        GLOBAL_SpcDebugApu0 = REG_APUIO0;
        GLOBAL_SpcDebugApu1 = REG_APUIO1;
        GLOBAL_SpcDebugApu2 = REG_APUIO2;
        GLOBAL_SpcDebugApu3 = REG_APUIO3;
        return;
    }

    s_spcReady = true;
    s_spcRuntimeReady = true;
    s_spcLastData = REG_APUIO0;
    GLOBAL_SpcDebugStage = 5u;
    GLOBAL_SpcDebugApu0 = REG_APUIO0;
    GLOBAL_SpcDebugApu1 = REG_APUIO1;
    GLOBAL_SpcDebugApu2 = REG_APUIO2;
    GLOBAL_SpcDebugApu3 = REG_APUIO3;
}

void port_audioPlayMusic(uint8_t pattern) {
    if (!s_spcReady) {
        port_audioInit();
    }
    if (!s_spcReady || !s_spcRuntimeReady) {
        return;
    }
    (void)spc_send_cmd(SPC_CMD_PLAY_MUSIC, pattern);
}

void port_audioPlaySfx(uint8_t sfxID) {
    uint8_t cmd;
    if (!s_spcReady) {
        port_audioInit();
    }
    if (!s_spcReady || !s_spcRuntimeReady) {
        return;
    }
    cmd = (s_spcSfxCmdToggle & 1u) ? SPC_CMD_PLAY_SFX_B : SPC_CMD_PLAY_SFX_A;
    s_spcSfxCmdToggle ^= 1u;
    (void)spc_send_cmd(cmd, sfxID);
}

void port_audioStopAll(void) {
    if (!s_spcReady) {
        port_audioInit();
    }
    if (!s_spcReady) {
        return;
    }
    (void)spc_force_stop();
}

void port_levelAnimAdvance(void) {
}


// Cross-compiler interrupt handlers, must be present
void snesXC_cop(void) {
}

void snesXC_brk(void) {
}

void snesXC_abort(void) {
}
