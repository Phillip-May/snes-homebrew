#include "port.h"
#include "snes_farcall.h"


#include "../../shared/src/snes_regs_xc.h"
#include "../../shared/src/initsnes.h"
// llvm-mos has 16-bit pointers, so ROM data must be explicitly bank-placed
// via #pragma clang section rodata="rom_data_bank_N" to be reachable.
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

extern struct sPlayerData GLOBAL_PlayerData;
extern bool GLOBAL_FlagOverlayShow;
extern bool GLOBAL_FlagOverlayDirty;
extern bool GLOBAL_FlagOverlayRevealDirty;
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
// Title-flash CGRAM staging — written by handleTitleScreenFrame in bank 7,
// flushed by port_vblank() at the start of vblank via port_titleFlashFlush
// (also in bank 7; keeps the fixed-mirror code small).
extern uint8_t GLOBAL_TitleFlashCgramDirty;
extern void port_titleFlashFlush(void);

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
// Three full 16-cell screen rows. Higher values push llvm-mos static-stack
// allocation into the end of the SNES near-RAM window and make small code
// changes corrupt memory.
#define PORT_BG1_TEXT_SLOT_COUNT 48u
#define PORT_BG1_TEXT_SLOT_ROW_COUNT ((PORT_BG1_TEXT_SLOT_COUNT + 7u) / 8u)
#define PORT_BG1_TEXT_SLOT_FREE 0xFFu
#define PORT_BG1_TEXT_CELL_SLOT_MASK 0x7Fu
#define PORT_BG1_TEXT_CELL_VISIBLE 0x80u
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
#define PORT_BG1_TEXT_MAX_MAP_ROWS_PER_VBLANK PORT_BG1_TEXT_SCREEN_MACRO_H
#define PORT_BG1_TEXT_MAX_SLOTS_PER_VBLANK 4u
#define PORT_BG1_TEXT_CODE PORT_FUNC_BANK4
#define PORT_BG1_TEXT_CORE_CODE PORT_FUNC_BANK4
#define PORT_BG1_TEXT_FLUSH_CODE PORT_FUNC_BANK5
#define PORT_SNOW_STEP_CODE PORT_FUNC_BANK5
#define PORT_PICO8_FONT_GLYPH_W_PX 4u
#define PORT_PICO8_FONT_ADVANCE_PX 5u
#define PORT_PICO8_FONT_SCALE 2u
#define PORT_PICO8_FONT_CELL_Y_OFFSET 4u
#define PORT_PICO8_TEXT_INK_X_OFFSET_PX 2u
#define PORT_PICO8_RUN_INK_X_OFFSET_PX 1u
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
static uint8_t s_bg1TextSlotCell[PORT_BG1_TEXT_SLOT_COUNT];
static uint8_t s_bg1TextCellSlot[PORT_BG1_TEXT_CELL_COUNT];
static uint8_t s_bg1TextSlotDirtyBits[PORT_BG1_TEXT_SLOT_ROW_COUNT];
static bool s_bg1TextAnySlotDirty;
static uint16_t s_bg1TextMapDirtyRowBits;
static bool s_bg1TextPublishHidden;
// Caller-controlled flags for port_drawTextWithColorsLen. Default 0 = full
// glyph + publish (so .bss zero-init does the right thing). The flag overlay
// sets and clears these to split a char render across multiple sub-steps.
// Callers must restore to 0 when done.
//   bits 0-1 : glyph row quarter 0..3 (only meaningful when bit 2 set;
//              rows [quarter*2, quarter*2 + 2))
//   bit 2    : quarter mode (else full 0..8 row range)
//   bit 7    : suppress trailing publishRect
#define BG1_TEXT_RENDER_QUARTER_MASK   0x03u
#define BG1_TEXT_RENDER_QUARTER_MODE   0x04u
#define BG1_TEXT_RENDER_NO_PUBLISH     0x80u
static uint8_t s_bg1TextRenderFlags;
// Color 0 stays transparent. Color 1 is black ink/fill, color 3 is white.
static uint16_t s_bg1TextPalette[4] = {0x0000u, 0x0000u, 0x0000u, 0x7FFFu};
static uint16_t s_scoreSpritePalette[16];
static uint8_t s_pico8GlyphRows[8];

#define s_bg1TextCoverBits s_bg1TextCoverScratch.coverBits
#define s_tilemapBg3 s_bg1TextCoverScratch.bg3Tilemap

// NMI-driven VBlank: pre-built staging for text DMA
#define TEXT_MAX_MAP_DMA PORT_BG1_TEXT_MAX_MAP_ROWS_PER_VBLANK
#define TEXT_MAX_SLOT_DMA PORT_BG1_TEXT_DMA_STAGE_COUNT
#define s_textMapDmaBufs ((uint16_t (*)[PORT_BG1_TEXT_SCREEN_MACRO_W])s_tilemapBg2)
static uint8_t  s_textMapDmaRows[TEXT_MAX_MAP_DMA];
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
// Set by port_LoadRoomData when force-blank is engaged for a room transition.
// port_vblank checks this *after* the OAM DMA so the display only un-blanks
// once hardware OAM reflects the new room's sprite positions; otherwise the
// new BG appears for one frame with the previous room's sprites overlaid.
bool s_pendingDisplayEnable = false;
// Konami-code dedication screen. While true, port_vblank flushes the BG1
// text DMA even though we're still nominally in title mode, so caller-
// drawn text becomes visible.
static bool s_dedicationActive = false;
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

// OAM layout: 0-28 are object indices (GLOBAL_OBJ_LIST_SIZE = 29). 30-34
// are reserved for player hair particles. Dynamic extra sprites start at 35.
#define PORT_HAIR_OAM_BASE 30u
#define PORT_HAIR_COUNT 5u
// OAM map: 0..28 are OBJ slots (GLOBAL_OBJ_LIST_SIZE = 29). Slot 29 is unused.
// 30..34 are player-hair particles. 35..42 (8 slots) are death particles (see
// DEAD_PARTICLE_OAM_BASE in mainBankZero.c). Dynamic extras start at 43.
#define PORT_EXTRA_SPRITE_START 43u
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

    writeConditionalSprite(index, obj, false);

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

    writeConditionalSprite(index, obj, false);

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

PORT_OPTNONE static bool writeBalloonSprite(uint8_t index, OBJ_DATA *obj)
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

static bool writePlatMovSprite(uint8_t index, OBJ_DATA *obj)
{
    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, PORT_PLATMOV_EXTRA_SPRITES)) {
        return false;
    }
    obj->extraSpriteCount = PORT_PLATMOV_EXTRA_SPRITES;

    uint8_t rightSlot = obj->extraSpriteBase;
    uint8_t wrapSlot = (uint8_t)(rightSlot + 1u);
    uint8_t screenY = (uint8_t)sprite_to_screen_y((int16_t)(obj->pos.y << 1));
    uint8_t props = obj->oamProps;
    // vbcc-era rendering: both tiles use natural uint8 OBJX wrap; an X-MSB=1
    // ghost slot extends the cloud across the screen-wrap boundary. The mod-128
    // pos.x wrap (see platMovUpdate) keeps the screen position continuous.
    uint8_t leftX = (uint8_t)(obj->pos.x << 1);
    uint8_t rightX = (uint8_t)(leftX + PORT_PLATMOV_RIGHT_OFFSET);

    // Main LEFT tile.
    uint8_t leftT2i = (uint8_t)(index / 4u);
    GLOBAL_OAMCopy.arr.OAMTable2[leftT2i] = calcOAMTable2Byte(index, 1u, 0u,
        GLOBAL_OAMCopy.arr.OAMTable2[leftT2i]);
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = leftX;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = screenY;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = PORT_PLATMOV_TILE_LEFT;
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = props;

    // Main RIGHT tile.
    uint8_t rightT2i = (uint8_t)(rightSlot / 4u);
    GLOBAL_OAMCopy.arr.OAMTable2[rightT2i] = calcOAMTable2Byte(rightSlot, 1u, 0u,
        GLOBAL_OAMCopy.arr.OAMTable2[rightT2i]);
    GLOBAL_OAMCopy.arr.OAMArray[rightSlot].OBJX = rightX;
    GLOBAL_OAMCopy.arr.OAMArray[rightSlot].OBJY = screenY;
    GLOBAL_OAMCopy.arr.OAMArray[rightSlot].CHARNUM = PORT_PLATMOV_TILE_RIGHT;
    GLOBAL_OAMCopy.arr.OAMArray[rightSlot].PROPERTIES = props;

    // Wrap ghost: X-MSB=1 puts the straddling tile at signed screen X − 256
    // (i.e. screen -16..-1) to bridge the wrap. Mirrors the vbcc-era logic.
    uint8_t wrapT2i = (uint8_t)(wrapSlot / 4u);
    uint8_t wrapCur = GLOBAL_OAMCopy.arr.OAMTable2[wrapT2i];
    if (leftX > 240u) {
        GLOBAL_OAMCopy.arr.OAMTable2[wrapT2i] = calcOAMTable2Byte(wrapSlot, 1u, 1u, wrapCur);
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].OBJX = leftX;
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].OBJY = screenY;
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].CHARNUM = PORT_PLATMOV_TILE_LEFT;
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].PROPERTIES = props;
    } else if (rightX > 240u) {
        GLOBAL_OAMCopy.arr.OAMTable2[wrapT2i] = calcOAMTable2Byte(wrapSlot, 1u, 1u, wrapCur);
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].OBJX = rightX;
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].OBJY = screenY;
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].CHARNUM = PORT_PLATMOV_TILE_RIGHT;
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].PROPERTIES = props;
    } else {
        GLOBAL_OAMCopy.arr.OAMTable2[wrapT2i] = calcOAMTable2Byte(wrapSlot, 1u, 0u, wrapCur);
        GLOBAL_OAMCopy.arr.OAMArray[wrapSlot].OBJY = 240;
    }
    return true;
}

static bool writeFlyingBerrySprite(uint8_t index, OBJ_DATA *obj)
{
    if (!ensureExtraSpriteRange(&obj->extraSpriteBase, PORT_FLYING_BERRY_EXTRA_SPRITES)) {
        return false;
    }

    // pos.y < 3 hides both body (game keeps it alive until pos.y < -16) and the
    // wings 4-px above it — without this guard the uint8 OBJY cast wraps them to
    // the bottom edge once the berry flies off the top of the screen.
    bool hideMain = (obj->data.flyingBerry.isCollected != 0u) || (obj->pos.y < 3);
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
    writeConditionalSprite(index, obj, false);

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

PORT_BG1_TEXT_CODE
static bool bg1TextSlotIsDirty(uint8_t slot)
{
    return (s_bg1TextSlotDirtyBits[slot >> 3] & (uint8_t)(1u << (slot & 7u))) != 0u;
}

PORT_BG1_TEXT_CODE
static uint8_t bg1TextCellSlot(uint8_t cell)
{
    uint8_t value = s_bg1TextCellSlot[cell];
    if (value == PORT_BG1_TEXT_SLOT_FREE) {
        return PORT_BG1_TEXT_SLOT_FREE;
    }
    return (uint8_t)(value & PORT_BG1_TEXT_CELL_SLOT_MASK);
}

PORT_BG1_TEXT_CODE
static bool bg1TextCellIsVisible(uint8_t cell)
{
    uint8_t value = s_bg1TextCellSlot[cell];
    return value != PORT_BG1_TEXT_SLOT_FREE && (value & PORT_BG1_TEXT_CELL_VISIBLE) != 0u;
}

PORT_BG1_TEXT_CODE
static void bg1TextSetCellVisible(uint8_t cell)
{
    if (s_bg1TextCellSlot[cell] != PORT_BG1_TEXT_SLOT_FREE) {
        s_bg1TextCellSlot[cell] |= PORT_BG1_TEXT_CELL_VISIBLE;
    }
}

PORT_BG1_TEXT_CORE_CODE
static void bg1TextPublishCell(uint8_t cell)
{
    uint8_t slot;
    slot = bg1TextCellSlot(cell);
    if (slot == PORT_BG1_TEXT_SLOT_FREE) {
        return;
    }

    bg1TextMarkSlotDirty(slot);
    if (!s_bg1TextPublishHidden && !bg1TextCellIsVisible(cell)) {
        bg1TextMarkMapDirty(cell);
    }
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
    uint8_t slot = bg1TextCellSlot(cell);
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
static PORT_NOINLINE PORT_OPTNONE void bg1TextFetchGlyphRows(uint8_t ch)
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
static void bg1TextHideCell(uint8_t cell)
{
    uint8_t slot = bg1TextCellSlot(cell);
    bool visible = bg1TextCellIsVisible(cell);
    if (slot == PORT_BG1_TEXT_SLOT_FREE) {
        return;
    }

    s_bg1TextSlotCell[slot] = PORT_BG1_TEXT_SLOT_FREE;
    s_bg1TextCellSlot[cell] = PORT_BG1_TEXT_SLOT_FREE;
    bg1TextClearSlotDirty(slot);
    if (visible) {
        bg1TextMarkMapDirty(cell);
    }
}

PORT_DATA_BANK0
static const uint8_t s_bg1TextFill10FirstMask0[16] = {
    0xFFu, 0x7Fu, 0x3Fu, 0x1Fu, 0x0Fu, 0x07u, 0x03u, 0x01u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
};
PORT_DATA_BANK0
static const uint8_t s_bg1TextFill10FirstMask1[16] = {
    0xC0u, 0xE0u, 0xF0u, 0xF8u, 0xFCu, 0xFEu, 0xFFu, 0xFFu,
    0xFFu, 0x7Fu, 0x3Fu, 0x1Fu, 0x0Fu, 0x07u, 0x03u, 0x01u
};
PORT_DATA_BANK0
static const uint8_t s_bg1TextFill10SecondMask0[16] = {
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u,
    0xC0u, 0xE0u, 0xF0u, 0xF8u, 0xFCu, 0xFEu, 0xFFu, 0xFFu
};
PORT_DATA_BANK0
static const uint8_t s_bg1TextFill10SecondMask1[16] = {
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u
};

PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE void bg1TextFillBgSlotMaskedNoPublish(uint8_t slot, uint8_t mask0, uint8_t mask1)
{
    uint8_t offset;

    if (slot == PORT_BG1_TEXT_SLOT_FREE) {
        return;
    }
    for (offset = 0u; offset < PORT_BG1_TEXT_SLOT_MASK_BYTES; offset += 2u) {
        s_bg1TextCoverBits[slot][offset] |= mask0;
        s_bg1TextCoverBits[slot][(uint8_t)(offset + 1u)] |= mask1;
        s_bg1TextInkBits[slot][offset] &= (uint8_t)~mask0;
        s_bg1TextInkBits[slot][(uint8_t)(offset + 1u)] &= (uint8_t)~mask1;
    }
}

PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE void bg1TextFill10BgNoPublish(uint16_t sx, uint16_t sy)
{
    uint8_t cellY = (uint8_t)(sy >> 4);
    uint8_t cellX = (uint8_t)(sx >> 4);
    uint8_t localX = (uint8_t)(sx & 0x0Fu);
    uint8_t cell = (uint8_t)((cellY * PORT_BG1_TEXT_SCREEN_MACRO_W) + cellX);

    bg1TextFillBgSlotMaskedNoPublish(bg1TextGetSlot(cell, true),
                                     s_bg1TextFill10FirstMask0[localX],
                                     s_bg1TextFill10FirstMask1[localX]);
    if (localX >= 7u && cellX < (PORT_BG1_TEXT_SCREEN_MACRO_W - 1u)) {
        bg1TextFillBgSlotMaskedNoPublish(bg1TextGetSlot((uint8_t)(cell + 1u), true),
                                         s_bg1TextFill10SecondMask0[localX],
                                         s_bg1TextFill10SecondMask1[localX]);
    }
}

PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE void bg1TextFillRectNoPublish(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint8_t color)
{
    uint16_t px;
    uint16_t py;

    if (!bg1TextClipRect(&sx, &sy, &w, &h)) {
        return;
    }

    if (h == 16u &&
        w == (PORT_PICO8_FONT_ADVANCE_PX * PORT_PICO8_FONT_SCALE) &&
        (sy & 0x0Fu) == 0u &&
        color == PORT_BG1_TEXT_COLOR_BG) {
        bg1TextFill10BgNoPublish(sx, sy);
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

// Fast solid 16x16 INK fill for a single cell. Bypasses the per-pixel inner
// loop (~25k cycles/cell) by writing slot bitmaps via memset. Caller is
// expected to wrap with s_bg1TextPublishHidden=true; map publish is skipped.
PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE void bg1TextFillCellSolidInk(uint8_t cell)
{
    uint8_t slot = bg1TextGetSlot(cell, true);
    if (slot == PORT_BG1_TEXT_SLOT_FREE) {
        return;
    }
    memset(s_bg1TextInkBits[slot], 0xFFu, PORT_BG1_TEXT_SLOT_MASK_BYTES);
    memset(s_bg1TextCoverBits[slot], 0xFFu, PORT_BG1_TEXT_SLOT_MASK_BYTES);
    bg1TextMarkSlotDirty(slot);
}


PORT_BG1_TEXT_CODE
static PORT_NOINLINE bool bg1TextHideRect(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h)
{
    uint8_t cellX0;
    uint8_t cellY0;
    uint8_t cellX1;
    uint8_t cellY1;
    uint8_t cellX;
    uint8_t cellY;

    if (!bg1TextClipRect(&sx, &sy, &w, &h)) {
        return false;
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
    return true;
}

PORT_BG1_TEXT_CODE
static PORT_NOINLINE bool bg1TextBlankRectCells(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, bool allocate)
{
    uint8_t cellX0;
    uint8_t cellY0;
    uint8_t cellX1;
    uint8_t cellY1;
    uint8_t cellX;
    uint8_t cellY;
    bool ok = true;

    if (!bg1TextClipRect(&sx, &sy, &w, &h)) {
        return false;
    }

    cellX0 = (uint8_t)(sx >> 4);
    cellY0 = (uint8_t)(sy >> 4);
    cellX1 = (uint8_t)((sx + w - 1u) >> 4);
    cellY1 = (uint8_t)((sy + h - 1u) >> 4);
    for (cellY = cellY0; cellY <= cellY1; ++cellY) {
        for (cellX = cellX0; cellX <= cellX1; ++cellX) {
            uint8_t cell = (uint8_t)((cellY * PORT_BG1_TEXT_SCREEN_MACRO_W) + cellX);
            uint8_t slot = bg1TextGetSlot(cell, allocate);
            if (slot == PORT_BG1_TEXT_SLOT_FREE) {
                ok = false;
                continue;
            }
            bg1TextClearSlot(slot);
            bg1TextClearSlotDirty(slot);
        }
    }
    return ok;
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

    // Stage tile data first. New tilemap references must not become visible
    // until the referenced dynamic tile has been uploaded.
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

    // Map rows are cheap to DMA, and hiding text only needs tilemap updates.
    // Reuse the BG2 room-load scratch buffer so all text rows can clear in one frame.
    if (s_bg1TextMapDirtyRowBits != 0u) {
        for (row = 0u; row < PORT_BG1_TEXT_SCREEN_MACRO_H; ++row) {
            bool deferredSlot = false;
            if (s_textMapDmaCount >= TEXT_MAX_MAP_DMA) break;
            uint16_t dirtyMask = (uint16_t)(1u << row);
            if ((s_bg1TextMapDirtyRowBits & dirtyMask) == 0u) continue;

            uint8_t idx = s_textMapDmaCount;
            volatile uint8_t col;
            memset(s_textMapDmaBufs[idx], 0, sizeof(s_textMapDmaBufs[0]));
            for (col = 0u; col < PORT_BG1_TEXT_SCREEN_MACRO_W; ++col) {
                uint8_t cell = (uint8_t)(row * PORT_BG1_TEXT_SCREEN_MACRO_W + (uint8_t)col);
                uint8_t activeSlot = bg1TextCellSlot(cell);
                if (activeSlot == PORT_BG1_TEXT_SLOT_FREE) {
                    continue;
                }
                if (bg1TextSlotIsDirty(activeSlot) && !bg1TextCellIsVisible(cell)) {
                    deferredSlot = true;
                    continue;
                }
                s_textMapDmaBufs[idx][col] = (uint16_t)(PORT_BG1_TEXT_ATTR | bg1TextSlotTile(activeSlot));
                bg1TextSetCellVisible(cell);
            }
            s_textMapDmaRows[idx] = row;
            if (!deferredSlot) {
                s_bg1TextMapDirtyRowBits &= (uint16_t)~dirtyMask;
            }
            s_textMapDmaCount++;
        }
    }
}

// DMA-only text flush.  Called from NMI handler — zero computation.
PORT_BG1_TEXT_FLUSH_CODE
static void bg1TextDmaFlush(void)
{
    snesXC_setDataBank(0x7Eu);

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

    for (uint8_t i = 0u; i < s_textMapDmaCount; i++) {
        LoadVram((const unsigned char *)s_textMapDmaBufs[i],
                 (uint16_t)(PORT_BG1_TEXT_TILEMAP_BASE + ((uint16_t)s_textMapDmaRows[i] * 32u * 2u)),
                 sizeof(s_textMapDmaBufs[0]));
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
// Stores rawSpd (2..40); the effective 16-bit increment is rawSpd << 5 and
// is reconstructed at the two use sites. Halves the BSS footprint vs storing
// the precomputed uint16_t value.
static uint8_t s_snowSpd[SNOW_COUNT];
static uint8_t  s_snowPhase[SNOW_COUNT];
static uint8_t  s_snowPrevCell[SNOW_COUNT];
static uint16_t s_snowRng = 0x1234u;
static bool     s_snowActive = false;
static uint8_t  s_snowScanStart = 0u; // round-robin start row for DMA budget

// Pre-built DMA staging: row buffers built before VBlank, blasted during VBlank.
// Capped at 4 rows to fit RAM budget; excess rows deferred to next frame.
#define SNOW_MAX_DMA_ROWS 3u
static uint16_t s_snowDmaBufs[SNOW_MAX_DMA_ROWS][SNOW_VISIBLE_COLS]; // 128 bytes
static uint8_t  s_snowDmaRowNums[SNOW_MAX_DMA_ROWS];
static uint8_t  s_snowDmaCount = 0u;

// Per-row bucket chain so the tile-placement loop walks only the particles
// on a given row instead of scanning all 25 each time. The per-particle
// "next" pointer aliases s_snowPrevCell: snowUpdate writes prev cells,
// snowPreBuildRows reads them once (to compute prevOnlyRows) and then
// repurposes the same byte as the chain link.
#define SNOW_PARTICLE_NONE 0xFFu
static uint8_t s_snowRowHead[SNOW_VISIBLE_ROWS];
#define s_snowRowNext s_snowPrevCell

// (1u << row) lookup; llvm-mos otherwise calls a runtime shift helper for
// non-constant shift counts. Touched in two hot loops in snowPreBuildRows.
static const uint16_t SNOW_ROW_BIT[SNOW_VISIBLE_ROWS] = {
    1u<<0, 1u<<1, 1u<<2, 1u<<3, 1u<<4, 1u<<5, 1u<<6,
    1u<<7, 1u<<8, 1u<<9, 1u<<10, 1u<<11, 1u<<12, 1u<<13,
};

// Sentinel byte for Mesen Lua-based cycle profiling. The harness installs a
// write callback on this address and samples cpu.cycleCount on each write,
// so diffs between consecutive markers yield per-phase cost. Markers:
//   1/2 enter/leave snowPreBuildRows
//   3/4 enter/leave snowFlushTilemap
//   5/6 enter/leave snowUpdate
// Volatile so the compiler keeps every write even when LTO inlines.
volatile uint8_t g_snowPerfMarker;

PORT_SNOW_STEP_CODE
static uint8_t snow_rand_u8(void)
{
    s_snowRng = (uint16_t)((s_snowRng * 109u) + 89u);
    return (uint8_t)(s_snowRng >> 8);
}

PORT_SNOW_STEP_CODE
static uint8_t snow_rand_range_u8(uint8_t lo, uint8_t hi)
{
    return (uint8_t)(lo + (snow_rand_u8() % (uint8_t)(hi - lo + 1u)));
}

PORT_SNOW_STEP_CODE
static int8_t snowSineValue(uint8_t phase)
{
    if (phase >= 6u && phase <= 26u) return 1;
    if (phase >= 38u && phase <= 58u) return -1;
    return 0;
}

PORT_SNOW_STEP_CODE
static uint8_t snowCellFromPosition(uint16_t x, uint8_t y)
{
    uint8_t px = (uint8_t)(x >> 8);
    uint8_t cx = (uint8_t)(px >> 4);
    uint8_t cy = (uint8_t)(y >> 4);
    return (cx < SNOW_VISIBLE_COLS && cy < SNOW_VISIBLE_ROWS)
         ? (uint8_t)(cy * SNOW_VISIBLE_COLS + cx)
         : SNOW_CELL_NONE;
}

PORT_SNOW_STEP_CODE
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

PORT_SNOW_STEP_CODE
static void snowUploadPalette(void)
{
    // Direct register writes — avoids DMA bank-pointer issues
    REG_CGADD = SNOW_CGRAM_OFFSET;
    REG_CGDATA = 0x00u; REG_CGDATA = 0x00u; // color 0: transparent
    REG_CGDATA = 0xFFu; REG_CGDATA = 0x7Fu; // color 1: white  (0x7FFF)
    REG_CGDATA = 0x18u; REG_CGDATA = 0x63u; // color 2: lt grey (0x6318)
    REG_CGDATA = 0x00u; REG_CGDATA = 0x00u; // color 3: unused
}

PORT_SNOW_STEP_CODE
static void snowInitParticles(void)
{
    for (uint8_t i = 0u; i < SNOW_COUNT; i++) {
        s_snowX[i]    = (uint16_t)snow_rand_u8() << 8;
        s_snowY[i]    = snow_rand_range_u8(0u, 223u);
        // PICO-8 speed 0.25-5.0 px/frame at 30fps, 8.8 fixed point
        uint8_t rawSpd = snow_rand_range_u8(2u, 40u);
        s_snowSpd[i]   = rawSpd;
        s_snowPhase[i] = snow_rand_u8() & 63u;
        s_snowPrevCell[i] = SNOW_CELL_NONE;
    }
    s_snowActive = true;
}

PORT_SNOW_STEP_CODE
static void snowInit(void)
{
    snowGenerateTiles();
    snowUploadPalette();
    snowInitParticles();
}

PORT_SNOW_STEP_CODE
static void snowResetPrevCells(void)
{
    for (uint8_t i = 0u; i < SNOW_COUNT; i++) {
        s_snowPrevCell[i] = SNOW_CELL_NONE;
    }
}

PORT_SNOW_STEP_CODE
static void snowUpdate(void)
{
    g_snowPerfMarker = 5u;
    if (!s_snowActive || s_titleMode) { g_snowPerfMarker = 6u; return; }
    for (uint8_t i = 0u; i < SNOW_COUNT; i++) {
        // Inline of snowCellFromPosition + drop the column part.
        // s_snowPrevCell now stores just the row (snowPreBuildRows only
        // uses `prev >> 4` of the old cell encoding).
        uint16_t oldX = s_snowX[i];
        uint8_t  oldY = s_snowY[i];
        uint8_t  prevRow = ((uint8_t)(oldX >> 8) < 0xF0u && oldY < 0xE0u)
                         ? (uint8_t)(oldY >> 4)
                         : SNOW_CELL_NONE;

        uint8_t rawSpd = s_snowSpd[i];
        uint16_t newX = oldX + ((uint16_t)rawSpd << 5);
        s_snowX[i] = newX;

        uint8_t y;
        if (newX < oldX) {
            y = snow_rand_range_u8(0u, 223u);
            s_snowPhase[i] = snow_rand_u8() & 63u;
        } else {
            y = oldY;
        }

        uint8_t phase = (uint8_t)((s_snowPhase[i] + (uint8_t)((rawSpd >> 3) + 1u)) & 63u);
        s_snowPhase[i] = phase;

        // Inline of snowSineValue (-1/0/+1) with uint8_t clamp.
        if (phase >= 6u && phase <= 26u) {
            s_snowY[i] = (y >= 223u) ? 223u : (uint8_t)(y + 1u);
        } else if (phase >= 38u && phase <= 58u) {
            s_snowY[i] = (y == 0u) ? 0u : (uint8_t)(y - 1u);
        } else {
            s_snowY[i] = y;
        }
        s_snowPrevCell[i] = prevRow;
    }
    g_snowPerfMarker = 6u;
}

// Pre-build all snow DMA row buffers.  Called BEFORE the VBlank wait so
// the computation is free.  Merges text + snow entries per row.
PORT_BG1_TEXT_CODE
static void snowPreBuildRows(void)
{
    g_snowPerfMarker = 1u;
    s_snowDmaCount = 0u;
    if (!s_snowActive || s_titleMode) { g_snowPerfMarker = 2u; return; }

    // Collect dirty rows: current positions first (high priority),
    // then previous positions (low priority — just clearing). Also bucket
    // particles into per-row chains so the placement loop later walks just
    // the relevant particles instead of all 25.
    for (uint8_t r = 0u; r < SNOW_VISIBLE_ROWS; r++) {
        s_snowRowHead[r] = SNOW_PARTICLE_NONE;
    }
    uint16_t currentRows = 0u;
    uint16_t prevOnlyRows = 0u;
    for (uint8_t i = 0u; i < SNOW_COUNT; i++) {
        // Read prev cell FIRST since s_snowRowNext aliases s_snowPrevCell;
        // the write below overwrites this byte for the current particle.
        uint8_t prev = s_snowPrevCell[i];  // now stores just the row directly
        if (prev != SNOW_CELL_NONE)
            prevOnlyRows |= SNOW_ROW_BIT[prev];

        uint8_t cy = s_snowY[i] >> 4;
        uint8_t cx = (uint8_t)(s_snowX[i] >> 8) >> 4;
        if (cx < SNOW_VISIBLE_COLS && cy < SNOW_VISIBLE_ROWS) {
            currentRows |= SNOW_ROW_BIT[cy];
            s_snowRowNext[i] = s_snowRowHead[cy];
            s_snowRowHead[cy] = i;
        }
    }
    prevOnlyRows &= (uint16_t)~currentRows; // only rows that JUST have departures
    uint16_t priorityMask = currentRows; // do these first

    // Build each dirty row into the staging buffer (current rows first).
    // Round-robin the scan start so all rows get fair DMA budget over time.
    // Walk the row index incrementally with a conditional wrap instead of
    // `% SNOW_VISIBLE_ROWS` (=14, not a power of two) — llvm-mos otherwise
    // emits a __divmodqi3 call for every iteration.
    for (uint8_t pass = 0u; pass < 2u; pass++) {
        uint16_t mask = (pass == 0u) ? priorityMask : prevOnlyRows;
        uint8_t row = s_snowScanStart;
        for (uint8_t offset = 0u; offset < SNOW_VISIBLE_ROWS && mask != 0u; offset++) {
            uint16_t rowBit = SNOW_ROW_BIT[row];
            uint8_t curRow = row;
            row++;
            if (row >= SNOW_VISIBLE_ROWS) row = 0u;
            if (!(mask & rowBit)) continue;
            if (s_snowDmaCount >= SNOW_MAX_DMA_ROWS) goto done;

            uint8_t idx = s_snowDmaCount;
            s_snowDmaRowNums[idx] = curRow;

            // Lay text + initialise empty cells to 0 in ONE pass — replaces a
            // separate memset(32 bytes). Encoding of s_bg1TextCellSlot:
            // 0xFF = free, 0x00..0x7F = non-free hidden, 0x80..0xFE = visible.
            {
                const uint8_t *cellRow = &s_bg1TextCellSlot[curRow * SNOW_VISIBLE_COLS];
                uint16_t *dst = s_snowDmaBufs[idx];
                for (uint8_t col = 0u; col < SNOW_VISIBLE_COLS; col++) {
                    uint8_t cellVal = cellRow[col];
                    if ((cellVal & 0x80u) != 0u && cellVal != 0xFFu) {
                        uint8_t slot = cellVal & 0x7Fu;
                        dst[col] = (uint16_t)(PORT_BG1_TEXT_ATTR | bg1TextSlotTile(slot));
                    } else {
                        dst[col] = 0u;
                    }
                }
            }

            // Snow entries for empty cells — walk only this row's chain.
            for (uint8_t p = s_snowRowHead[curRow]; p != SNOW_PARTICLE_NONE; p = s_snowRowNext[p]) {
                uint8_t px = (uint8_t)(s_snowX[p] >> 8);
                uint8_t cx = px >> 4;
                if (cx >= SNOW_VISIBLE_COLS || s_snowDmaBufs[idx][cx] != 0u) continue;

                uint8_t qx = (px & 15u) >> 2;
                uint8_t qy = (s_snowY[p] & 15u) >> 2;
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
    {
        uint8_t nextStart = s_snowScanStart + SNOW_MAX_DMA_ROWS;
        if (nextStart >= SNOW_VISIBLE_ROWS) nextStart -= SNOW_VISIBLE_ROWS;
        s_snowScanStart = nextStart;
    }
    g_snowPerfMarker = 2u;
}

// VBlank-only: tight DMA loop, zero computation.
PORT_BG1_TEXT_FLUSH_CODE
static void snowFlushTilemap(void)
{
    g_snowPerfMarker = 3u;
    snesXC_setDataBank(0x7Eu);
    for (uint8_t i = 0u; i < s_snowDmaCount; i++) {
        LoadVram((const unsigned char *)s_snowDmaBufs[i],
                 (uint16_t)(PORT_BG1_TEXT_TILEMAP_BASE + (uint16_t)s_snowDmaRowNums[i] * 64u),
                 sizeof(s_snowDmaBufs[0]));
    }
    g_snowPerfMarker = 4u;
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
    port_prg_bank_enter(5);
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
    // Don't un-blank yet: shadow OAM is being updated by the caller after we
    // return, but hardware OAM still reflects the previous room. Let
    // port_vblank flip INIDISP back on once it has DMA'd the new shadow OAM.
    s_pendingDisplayEnable = true;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
}

// Forward declarations for dead-particle assets defined later in this file.
extern const unsigned char dead_particle_tile_top[192];
extern const unsigned char dead_particle_tile_bot[192];
extern const unsigned char dead_particle_palette_pink[2];
extern const unsigned char dead_particle_palette_peach[2];

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
    // Hair + dead-particle assets uploaded by a bank-1 helper (keeps them
    // out of fixed-mirror code).
    port_prg_bank_enter(1);
    port_loadHairAssets();
    port_prg_bank_leave();
    snesXC_setDataBank(BANK_00);
    scoreSpriteUploadPalette(false);
    bg1TextUploadPalette();
    port_prg_bank_enter(5);
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

// Easter-egg "dedication" screen — shown after the title-screen Konami
// code. BG1 (the text overlay) is routed exactly like gameplay so
// port_drawText works, but only BG1 is enabled in TM and sprites are
// cleared. The deferred-un-blank flag keeps the screen black until the
// next vblank has DMA'd the rendered text into VRAM. Placed in
// rom_bank_4 alongside port_drawText so callers can share a single
// bank switch (otherwise the cheat handler in fixed blows its budget).
// Dedication screen uses BG3 directly with 8x8 tiles, bypassing the
// BG1 text slot system (capped at 48 slots; the full message needs more).
// Since this runs before the main game, we freely repurpose the title's
// BG3 tile/tilemap VRAM area — LoadRoomData reloads BG3 on room start so
// nothing leaks into gameplay.

// 8-byte scratch buffer for pulling one char's font rows out of
// rom_data_bank_1 via DMA. Tiny BSS footprint.
static uint8_t s_dedicationCharRows[8];

// 2x-replicate the high nibble of a pico-8 glyph byte across 8 SNES
// pixels: 0bABCD_xxxx → 0bAABB_CCDD. Indexed by the top 4 bits.
static const uint8_t s_dedicationScale2x[16] = {
    0x00, 0x03, 0x0C, 0x0F, 0x30, 0x33, 0x3C, 0x3F,
    0xC0, 0xC3, 0xCC, 0xCF, 0xF0, 0xF3, 0xFC, 0xFF,
};

PORT_FUNC_BANK7
static void uploadDedicationFontAndTilemap(void)
{
    // Each ASCII char becomes a 8x16 character — 2x scale of the pico-8
    // 4x8 glyph. That's two stacked 8x8 SNES tiles per char: tile
    // (ch*2) holds the top half (source rows 0-3 each duplicated to two
    // SNES rows), tile (ch*2+1) holds the bottom half (rows 4-7). 256
    // tiles total fit in BG3's 4 KB tile bank at VRAM 0x6000.
    // Source rows pulled via snesXC_memcpy_banked (direct cross-bank
    // CPU dereference was unreliable).
    REG_VMAIN = 0x80;
    REG_VMADD = (uint16_t)(0x6000u >> 1);
    for (uint16_t ch = 0; ch < 128u; ++ch) {
        snesXC_setDataBank(BANK_ASSETS);
        snesXC_memcpy_banked(s_dedicationCharRows, pico8_font_rows[ch], 8u);
        snesXC_setDataBank(BANK_00);
        // Top tile: source rows 0-3.
        for (uint8_t r = 0; r < 4u; ++r) {
            uint8_t scaled = s_dedicationScale2x[s_dedicationCharRows[r] >> 4];
            REG_VMDATAL = scaled; REG_VMDATAH = 0u; // copy 1 of 2
            REG_VMDATAL = scaled; REG_VMDATAH = 0u; // copy 2 of 2
        }
        // Bottom tile: source rows 4-7.
        for (uint8_t r = 4; r < 8u; ++r) {
            uint8_t scaled = s_dedicationScale2x[s_dedicationCharRows[r] >> 4];
            REG_VMDATAL = scaled; REG_VMDATAH = 0u;
            REG_VMDATAL = scaled; REG_VMDATAH = 0u;
        }
    }

    // Wipe BG3 tilemap (32x32 cells × 1 word per cell).
    REG_VMAIN = 0x80;
    REG_VMADD = (uint16_t)(0x4000u >> 1);
    for (uint16_t i = 0; i < (uint16_t)(32u * 32u); ++i) {
        REG_VMDATAL = 0u;
        REG_VMDATAH = 0u;
    }

    // Five lines, centered horizontally; 8x16 chars take 2 tilemap rows.
    // 1 empty row between lines for breathing → 3 rows per line, 15 total
    // across a 28-row visible screen → start at row 6.
    static const unsigned char L1[] = "DEDICATED TO";
    static const unsigned char L2[] = "FAE";
    static const unsigned char L3[] = "THE JOY OF MY LIFE";
    static const unsigned char L4[] = "LOVING ME THROUGH";
    static const unsigned char L5[] = "ALL MY TRIALS";
    const unsigned char *lines[5] = { L1, L2, L3, L4, L5 };
    const uint8_t lineTopRow[5] = { 6u, 9u, 12u, 15u, 18u };
    for (uint8_t l = 0; l < 5u; ++l) {
        const unsigned char *t = lines[l];
        uint8_t len = 0;
        while (t[len]) ++len;
        uint8_t startCol = (uint8_t)((32u - len) / 2u);
        uint8_t topRow = lineTopRow[l];

        // Top half of each char.
        REG_VMADD = (uint16_t)((0x4000u >> 1) +
                               (uint16_t)topRow * 32u + startCol);
        for (uint8_t i = 0; i < len; ++i) {
            REG_VMDATAL = (uint8_t)((uint16_t)t[i] * 2u);
            REG_VMDATAH = 0u;
        }
        // Bottom half: same chars, +1 tile id, next cell row.
        REG_VMADD = (uint16_t)((0x4000u >> 1) +
                               (uint16_t)(topRow + 1u) * 32u + startCol);
        for (uint8_t i = 0; i < len; ++i) {
            REG_VMDATAL = (uint8_t)((uint16_t)t[i] * 2u + 1u);
            REG_VMDATAH = 0u;
        }
    }
}

PORT_FUNC_BANK7
void port_showDedicationScreen(void)
{
    REG_INIDISP = 0x8Fu;
    REG_BGMODE = 0x00; // mode 0, all BGs 8x8 — gives us 32 cells per line
    REG_BG3SC = (uint8_t)((0x4000ul >> 9) | 0x00u);
    // BG34NBA: BG3 tile data base = 0x6000 (lower nibble = 3), BG4 doesn't
    // matter since TM hides it. 0x6000 >> 13 = 0x3.
    REG_BG34NBA = 0x03u;
    REG_TM = 0x04; // BG3 only

    uploadDedicationFontAndTilemap();

    // BG3 palette 0: color 0 transparent/black, color 1 white text.
    REG_CGADD = 0x40u;
    REG_CGDATA = 0x00u; REG_CGDATA = 0x00u;
    REG_CGDATA = 0xFFu; REG_CGDATA = 0x7Fu;

    port_resetSprites();
    s_dedicationActive = true;
    s_pendingDisplayEnable = true;
}

PORT_FUNC_BANK7
void port_endDedicationScreen(void)
{
    REG_INIDISP = 0x8Fu;
    s_dedicationActive = false;
    // No VRAM cleanup needed — LoadRoomData + port_showGameplayScreen
    // overwrite BG3 tilemap (at 0x4000) and re-point BG3 tile data
    // before the next un-blank.
}

// Stub kept so existing port.h declaration resolves; the BG3 path bakes
// the text into uploadDedicationFontAndTilemap, so the standalone draw
// call is a no-op now.
void port_drawDedicationText(void)
{
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
    // Leave INIDISP at force-blank (initSNES set 0x8F). LoadInitialGraphics
    // populated gameplay tiles + palettes but no tilemap is set up yet —
    // un-blanking here would briefly flash garbage before the caller's
    // first display target (port_showTitleScreen or LoadRoomData) is ready.
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

// === Player hair =============================================================
// Five 16x16 sprites trailing the player's head. Each hair particle is a
// 6x6 red dot centered in a 16x16 sprite cell (rest transparent). Particles
// soft-follow the previous one with a ~2/3 lerp, matching ccleste's
// draw_hair() (celeste.c:917). v1: single dot size, fixed red color.

// Tile slots 0xC0-C3 / 0xD0-D3 are used by the strawberry "1000" score
// sprite (python/score_1000_snes.h). Hair takes the next four 16x16 slots:
//   0xC4 (TL,TR) + 0xD4 (BL,BR) = "large" particle (r=2 rounded square)
//   0xC6 (TL,TR) + 0xD6 (BL,BR) = "small" particle (r=1 plus)
// matching ccleste's draw_hair sizes [2,2,1,1,1] (celeste.c:909).
#define PORT_HAIR_LARGE_CHARNUM  0xC4u
#define PORT_HAIR_SMALL_CHARNUM  0xC6u
#define PORT_HAIR_PROPS          0x38u  // priority 3, palette 4 (player palette)

// Hair tile data: two 16x16 sprites in 4bpp, both using only color index 5
// (bp0+bp2 set, bp1+bp3 clear) — matches the player's red in palette 4.
//
// Layout per 8x8 sub-tile: bytes 0-15 = bp0/bp1 interleaved per row,
// bytes 16-31 = bp2/bp3 interleaved. Both bitplane pairs share the same
// row mask for color index 5.
//
// LARGE = ccleste P8circfill r=2 (5x5 rounded square), scaled 2x → 10x10
// centered in 16x16 (rows 3-12, cols 3-12). Rounded top/bottom: 6-col
// caps; middle 6 rows: 10 cols.
// SMALL = ccleste P8circfill r=1 (plus, 5 pixels), scaled 2x → 6x6 plus
// centered in 16x16 (rows 5-10, cols 5-10). Center 2x2 row of arms,
// horizontal/vertical bars span 6 px in their axis.
//
// Both placed in rom_data_bank_1 alongside sprite_gfx_4bpp so the
// LoadInitialGraphics DMA (data bank = BANK_ASSETS) can reach them.
PORT_DATA_HIGH_BANK1
static const unsigned char hair_tile_top[128] = {
    // LARGE TL (cols 0-7 of 16x16): rows 3-4 mask 0x07 (cols 5-7),
    //                              rows 5-7 mask 0x1F (cols 3-7).
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x1F, 0x00, 0x1F, 0x00, 0x1F, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x1F, 0x00, 0x1F, 0x00, 0x1F, 0x00,
    // LARGE TR (cols 8-15): rows 3-4 mask 0xE0 (cols 8-10),
    //                       rows 5-7 mask 0xF8 (cols 8-12).
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00,
    // SMALL TL: rows 5-6 mask 0x01 (col 7 = vertical arm),
    //           row 7 mask 0x07 (cols 5-7 = left half of horizontal bar).
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x07, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x07, 0x00,
    // SMALL TR: rows 5-6 mask 0x80 (col 8 = vertical arm continues),
    //           row 7 mask 0xE0 (cols 8-10 = right half of horizontal bar).
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0xE0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0xE0, 0x00,
};
PORT_DATA_HIGH_BANK1
static const unsigned char hair_tile_bot[128] = {
    // LARGE BL: rows 0-2 (sprite rows 8-10) mask 0x1F,
    //           rows 3-4 (sprite rows 11-12) mask 0x07.
    0x1F, 0x00, 0x1F, 0x00, 0x1F, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1F, 0x00, 0x1F, 0x00, 0x1F, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // LARGE BR: rows 0-2 mask 0xF8, rows 3-4 mask 0xE0.
    0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // SMALL BL: row 0 (sprite row 8) mask 0x07 (cols 5-7 = horizontal bar),
    //           rows 1-2 (sprite rows 9-10) mask 0x01 (col 7 = vertical arm).
    0x07, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // SMALL BR: row 0 mask 0xE0, rows 1-2 mask 0x80.
    0xE0, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xE0, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// === Dead particles (ccleste celeste.c:1582-1604 / 1867-1879) ================
// Three 16x16 sprite shapes matching ccleste's P8rectfill at p->t/5 half-width:
//   BIG    (t=10):  10x10 filled square (pico-8 5x5) — tile 0xCE
//   MED    (t=5-9): 6x6  filled square (pico-8 3x3) — tile 0xD0
//   SMALL  (t=1-4): 2x2  filled square (pico-8 1x1) — tile 0xD2
// Each sprite's 4 tiles are laid out as standard SNES 16x16: TL=N, TR=N+1,
// BL=N+16, BR=N+17. So BIG uses 0xCE/0xCF/0xDE/0xDF, MED uses 0xD0..0xD3 etc.
// Colour uses palette index 5; flicker handled by alternating sprite palette
// 5/6 via oamProps (see deadParticleUpdate).
#define PORT_DEAD_PARTICLE_BIG_CHARNUM    0xCEu
#define PORT_DEAD_PARTICLE_MED_CHARNUM    0xD0u
#define PORT_DEAD_PARTICLE_SMALL_CHARNUM  0xD2u

// Tile data: 4bpp 8×8 = 32 bytes each. pixels-at-index-5 means bp0=1, bp2=1
// (bp1=bp3=0) for every "on" pixel. Layout: bytes 0-15 = bp0/bp1 interleaved
// per row, bytes 16-31 = bp2/bp3.
PORT_DATA_HIGH_BANK1
const unsigned char dead_particle_tile_top[192] = {
    // BIG TL (0xCE): rows 3-7 cols 3-7 → row mask 0x1F
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x1F,0x00, 0x1F,0x00, 0x1F,0x00, 0x1F,0x00, 0x1F,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x1F,0x00, 0x1F,0x00, 0x1F,0x00, 0x1F,0x00, 0x1F,0x00,
    // BIG TR (0xCF): rows 3-7 cols 0-4 of TR-local → row mask 0xF8
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0xF8,0x00, 0xF8,0x00, 0xF8,0x00, 0xF8,0x00, 0xF8,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0xF8,0x00, 0xF8,0x00, 0xF8,0x00, 0xF8,0x00, 0xF8,0x00,
    // MED TL (0xD0): rows 5-7 cols 5-7 → row mask 0x07
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x07,0x00, 0x07,0x00, 0x07,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x07,0x00, 0x07,0x00, 0x07,0x00,
    // MED TR (0xD1): rows 5-7 cols 0-2 of TR-local → row mask 0xE0
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0xE0,0x00, 0xE0,0x00, 0xE0,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0xE0,0x00, 0xE0,0x00, 0xE0,0x00,
    // SMALL TL (0xD2): row 7 col 7 → row mask 0x01
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x01,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x01,0x00,
    // SMALL TR (0xD3): row 7 col 0 → row mask 0x80
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x80,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x80,0x00,
};
PORT_DATA_HIGH_BANK1
const unsigned char dead_particle_tile_bot[192] = {
    // BIG BL (0xDE): rows 0-4 (sprite rows 8-12) cols 3-7 → 0x1F
    0x1F,0x00, 0x1F,0x00, 0x1F,0x00, 0x1F,0x00, 0x1F,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x1F,0x00, 0x1F,0x00, 0x1F,0x00, 0x1F,0x00, 0x1F,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    // BIG BR (0xDF): rows 0-4 cols 0-4 → 0xF8
    0xF8,0x00, 0xF8,0x00, 0xF8,0x00, 0xF8,0x00, 0xF8,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0xF8,0x00, 0xF8,0x00, 0xF8,0x00, 0xF8,0x00, 0xF8,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    // MED BL (0xE0): rows 0-2 cols 5-7 → 0x07
    0x07,0x00, 0x07,0x00, 0x07,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x07,0x00, 0x07,0x00, 0x07,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    // MED BR (0xE1): rows 0-2 cols 0-2 → 0xE0
    0xE0,0x00, 0xE0,0x00, 0xE0,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0xE0,0x00, 0xE0,0x00, 0xE0,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    // SMALL BL (0xE2): row 0 col 7 → 0x01
    0x01,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x01,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    // SMALL BR (0xE3): row 0 col 0 → 0x80
    0x80,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x80,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
};

// Palette entries for the 2-color flicker (pico-8 colors 14/15 converted to
// BGR15). Stored as little-endian 16-bit. Each palette only needs index 5
// populated; the rest stays 0 (transparent). Written to sprite palettes 5
// and 6 (oamProps 0x3A and 0x3C respectively).
PORT_DATA_HIGH_BANK1
const unsigned char dead_particle_palette_pink[2]  = { 0xDF, 0x55 }; // pico8 14 #FF77A8
PORT_DATA_HIGH_BANK1
const unsigned char dead_particle_palette_peach[2] = { 0x3F, 0x57 }; // pico8 15 #FFCCAA

// Banked: hair + dead-particle assets uploaded together to keep all this
// out of fixed-mirror code. Caller (LoadInitialGraphics) bank-switches once.
PORT_FUNC_BANK1
void port_loadHairAssets(void)
{
    // Hair tiles live just above the score-1000 sprite in the unused VRAM
    // gap before sprite_gfx_2bpp (0xA000). Dead-particle tiles slot into the
    // gap between hair (0xC4-C7 / 0xD4-D7) and the score-1000 bottom row at
    // 0xD0-D3 — namely 0xC8-CD / 0xD8-DD (VRAM 0x9900 / 0x9B00). The earlier
    // 0xCE-D3 placement collided with the score bottom and cut off the "1000".
    LoadVram(hair_tile_top, 0x9880, (uint16_t)sizeof(hair_tile_top));
    LoadVram(hair_tile_bot, 0x9A80, (uint16_t)sizeof(hair_tile_bot));
    LoadVram(dead_particle_tile_top, 0x9900, (uint16_t)sizeof(dead_particle_tile_top));
    LoadVram(dead_particle_tile_bot, 0x9B00, (uint16_t)sizeof(dead_particle_tile_bot));
    // Palette 5 / 6 index 5 = pico-8 14/15 (flicker colors).
    LoadCGRam(dead_particle_palette_pink,  0x00D5u, 2u);
    LoadCGRam(dead_particle_palette_peach, 0x00E5u, 2u);
}

PORT_FUNC_BANK1
void port_resetPlayerHair(struct sPlayerData *player)
{
    if (player == 0) {
        return;
    }
    // Snap all particles to the player's head so the trail doesn't fly in
    // from the previous room's coordinates.
    int16_t x_fx = (int16_t)((int16_t)(player->objData.pos.x + 4) << 8);
    int16_t y_fx = (int16_t)((int16_t)(player->objData.pos.y + 3) << 8);
    for (uint8_t i = 0; i < PORT_HAIR_COUNT; ++i) {
        player->hair[i].x = x_fx;
        player->hair[i].y = y_fx;
    }
}

PORT_FUNC_BANK1
void port_updatePlayerHair(struct sPlayerData *player)
{
    if (player == 0) {
        return;
    }
    int8_t facing = player->isFliped ? -1 : 1;
    // ccleste anchor: last_x = x + 4 - facing*2, last_y = y + 3 (v1 ignores 'down')
    int16_t last_x = (int16_t)((int16_t)(player->objData.pos.x + 4 - facing * 2) << 8);
    int16_t last_y = (int16_t)((int16_t)(player->objData.pos.y + 3) << 8);

    for (uint8_t i = 0; i < PORT_HAIR_COUNT; ++i) {
        // ccleste lerp: h += (last - h) / 1.5  →  delta * 171/256 in fixed point.
        int16_t dx = (int16_t)(last_x - player->hair[i].x);
        int16_t dy = (int16_t)(last_y - player->hair[i].y);
        player->hair[i].x = (int16_t)(player->hair[i].x + (int16_t)(((int32_t)dx * 171) >> 8));
        player->hair[i].y = (int16_t)(player->hair[i].y + (int16_t)(((int32_t)dy * 171) >> 8));

        // Convert from 8.8 internal coords to 256-screen, then center the
        // 16x16 sprite on the dot (sprite top-left = center − 8).
        int16_t cx = (int16_t)((player->hair[i].x >> 8) << 1);
        int16_t cy = (int16_t)((player->hair[i].y >> 8) << 1);
        int16_t sx = (int16_t)(cx - 8);
        int16_t sy = sprite_to_screen_y((int16_t)(cy - 8));

        uint8_t slot = (uint8_t)(PORT_HAIR_OAM_BASE + i);
        uint8_t table2Index = (uint8_t)(slot / 4u);
        uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
        uint8_t xBit = (uint8_t)((sx < 0 || sx >= 256) ? 1u : 0u);
        GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = calcOAMTable2Byte(slot, 1u, xBit, currentByte);
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = (uint8_t)sx;
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)sy;
        // ccleste sizes [2,2,1,1,1] → first two particles use the LARGE
        // (r=2 rounded) tile, the trailing three use the SMALL (r=1 plus).
        GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM =
            (uint8_t)((i < 2u) ? PORT_HAIR_LARGE_CHARNUM : PORT_HAIR_SMALL_CHARNUM);
        GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES = PORT_HAIR_PROPS;

        last_x = player->hair[i].x;
        last_y = player->hair[i].y;
    }
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
    writeConditionalSprite(index, obj, false);
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
    writeConditionalSprite(index, obj, false);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildKey(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    writeConditionalSprite(index, obj, false);
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
    writeConditionalSprite(index, obj, false);
    obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

void port_buildStaticDecor(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    writeConditionalSprite(index, obj, false);
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
static PORT_NOINLINE PORT_OPTNONE void port_drawTextWithColorsLen(const unsigned char *text,
                                                                              uint8_t textLength,
                                                                              uint8_t x,
                                                                              uint8_t y,
                                                                              uint8_t inkColor,
                                                                              uint8_t bgColor,
                                                                              bool fillBackground,
                                                                              uint8_t advancePx,
                                                                              uint8_t inkXOffsetPx,
                                                                              uint8_t cellYOffsetPx,
                                                                              uint8_t bgPadRightPx)
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
    bgW = (uint16_t)(((uint16_t)textLength * advancePx + bgPadRightPx) * PORT_PICO8_FONT_SCALE);
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
            uint8_t flags = s_bg1TextRenderFlags;
            uint8_t gyStart = 0u;
            uint8_t gyEnd = 8u;
            if ((flags & BG1_TEXT_RENDER_QUARTER_MODE) != 0u) {
                gyStart = (uint8_t)((flags & BG1_TEXT_RENDER_QUARTER_MASK) << 1);
                gyEnd = (uint8_t)(gyStart + 2u);
            }
            bg1TextFetchGlyphRows(ch);
            for (gy = gyStart; gy < gyEnd; ++gy) {
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

    if ((s_bg1TextRenderFlags & BG1_TEXT_RENDER_NO_PUBLISH) == 0u) {
        bg1TextPublishRect(bgX, sy, bgW, 16u);
    }
}

PORT_BG1_TEXT_CORE_CODE
static PORT_NOINLINE PORT_OPTNONE void port_drawCharWithColors(uint8_t ch,
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
static PORT_NOINLINE PORT_OPTNONE void port_drawTextWithColors(const unsigned char *text,
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
                               PORT_PICO8_FONT_CELL_Y_OFFSET,
                               PORT_PICO8_TEXT_BG_PAD_RIGHT_PX);
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
                               PORT_PICO8_FONT_CELL_Y_OFFSET,
                               PORT_PICO8_TEXT_BG_PAD_RIGHT_PX);
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
                               PORT_PICO8_FONT_ADVANCE_PX,
                               PORT_PICO8_RUN_INK_X_OFFSET_PX,
                               PORT_PICO8_FONT_CELL_Y_OFFSET,
                               0u);
}

PORT_BG1_TEXT_CODE
PORT_NOINLINE bool port_reservePico8RunN(uint8_t x, uint8_t y, uint8_t length, uint8_t advancePx)
{
    uint16_t sx = (uint16_t)x << 1;
    uint16_t sy = (uint16_t)y << 1;
    uint16_t bgW;

    if (length == 0u || sx >= 256u || sy >= 256u) {
        return false;
    }

    bgW = (uint16_t)((((uint16_t)(length - 1u) * advancePx) +
                      PORT_PICO8_FONT_GLYPH_W_PX +
                      PORT_PICO8_TEXT_BG_PAD_RIGHT_PX) * PORT_PICO8_FONT_SCALE);
    return bg1TextBlankRectCells(sx, sy, bgW, 16u, true);
}

PORT_BG1_TEXT_CODE
PORT_NOINLINE bool port_clearReservedPico8RunN(uint8_t x, uint8_t y, uint8_t length, uint8_t advancePx)
{
    uint16_t sx = (uint16_t)x << 1;
    uint16_t sy = (uint16_t)y << 1;
    uint16_t bgW;

    if (length == 0u || sx >= 256u || sy >= 256u) {
        return false;
    }

    bgW = (uint16_t)((((uint16_t)(length - 1u) * advancePx) +
                      PORT_PICO8_FONT_GLYPH_W_PX +
                      PORT_PICO8_TEXT_BG_PAD_RIGHT_PX) * PORT_PICO8_FONT_SCALE);
    return bg1TextHideRect(sx, sy, bgW, 16u);
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

// Used by the flag overlay's pre-render: skips the per-pixel BG fill since
// box cells are already solid INK (from row-fill steps), so the BG fill
// would be redundant.
PORT_BG1_TEXT_CODE
PORT_NOINLINE static void port_drawTextWhiteOnBlackNoFillN(const unsigned char *text, uint8_t length, uint8_t x, uint8_t y)
{
    port_drawTextWithColorsLen(text,
                               length,
                               x,
                               y,
                               PORT_BG1_TEXT_COLOR_BG,
                               PORT_BG1_TEXT_COLOR_INK,
                               false,
                               PORT_PICO8_FONT_ADVANCE_PX,
                               PORT_PICO8_TEXT_INK_X_OFFSET_PX,
                               PORT_PICO8_FONT_CELL_Y_OFFSET,
                               PORT_PICO8_TEXT_BG_PAD_RIGHT_PX);
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

// Pre-render layout (finer-grained chunking):
//   Box fill phase (Show=false only): 8 sub-steps
//     Substeps 1..8 = (row * 2) + halfIndex + 1
//       Half 0 fills cells [0..4] of the row (5 cells)
//       Half 1 fills cells [5..8] of the row (4 cells)
//     Substep 1 also runs hideRect over the full 144x64 region.
//   Per-char phase: 5 phases, encoded as (charIdx, charPhase):
//     Phase 0: BG fill (Show=true only; not used for Show=false)
//     Phase 1: glyph rows 0..1, publish suppressed
//     Phase 2: glyph rows 2..3, publish suppressed
//     Phase 3: glyph rows 4..5, publish suppressed
//     Phase 4: glyph rows 6..7 + publish (publishes the merged rect covering
//              all glyph quarters and any companion BG fill).
//   Show=false starts each char at phase 1 (4 sub-steps/char total).
//   Show=true  starts each char at phase 0 (5 sub-steps/char total).
//   Total: 8 + (Show ? 5T : 4T) sub-steps. T = L0 + L1 + L2.
#define FLAG_OVERLAY_BOX_FIRST_CELL 4u   // cellX = 64/16, cellY = 0
#define FLAG_OVERLAY_BOX_W_CELLS 9u      // 144 px / 16
#define FLAG_OVERLAY_BOX_H_CELLS 4u      // 64 px / 16
#define FLAG_OVERLAY_BOX_HALVES_PER_ROW 2u
#define FLAG_OVERLAY_BOX_HALF_LEFT_CELLS 5u  // cells [0..4]
#define FLAG_OVERLAY_BOX_FILL_STEPS \
    (FLAG_OVERLAY_BOX_H_CELLS * FLAG_OVERLAY_BOX_HALVES_PER_ROW)
#define FLAG_OVERLAY_FIRST_TEXT_STEP (1u + FLAG_OVERLAY_BOX_FILL_STEPS)
#define FLAG_OVERLAY_PER_CHAR_BG_W \
    ((PORT_PICO8_FONT_ADVANCE_PX + PORT_PICO8_TEXT_BG_PAD_RIGHT_PX) * PORT_PICO8_FONT_SCALE)
#define FLAG_OVERLAY_LAST_CHAR_PHASE 4u  // glyph quarter Q3 + publish
// Box phase: 1..FLAG_OVERLAY_BOX_FILL_STEPS active, 0 = done. Skipped for
// Show=true since the box stays revealed across re-renders.
static uint8_t s_flagOverlayBoxStep;
// Per-char text phase. Avoids decoding from a single step counter so neither
// /N nor %N needs an LLVM helper for the LTO-tight fixed mirror.
static uint8_t s_flagOverlayCharIdx;
static uint8_t s_flagOverlayCharPhase;     // 0..4; phase 0 only used for Show=true

PORT_BG1_TEXT_CODE
static void flagOverlayDoBoxStep(uint8_t step)
{
    uint8_t boxOffset = (uint8_t)(step - 1u);
    uint8_t row = (uint8_t)(boxOffset >> 1);
    uint8_t half = (uint8_t)(boxOffset & 1u);
    uint8_t colStart = (half == 0u) ? 0u : FLAG_OVERLAY_BOX_HALF_LEFT_CELLS;
    uint8_t colEnd = (half == 0u) ? FLAG_OVERLAY_BOX_HALF_LEFT_CELLS
                                  : FLAG_OVERLAY_BOX_W_CELLS;
    uint8_t col;
    if (step == 1u) {
        (void)bg1TextHideRect(64u, 0u, 144u, 64u);
    }
    for (col = colStart; col < colEnd; ++col) {
        bg1TextFillCellSolidInk((uint8_t)(row * PORT_BG1_TEXT_SCREEN_MACRO_W +
                                          FLAG_OVERLAY_BOX_FIRST_CELL + col));
    }
}

PORT_BG1_TEXT_CODE
static void flagOverlayDoCharStep(uint8_t idx, uint8_t phase)
{
    // Phase encoding (unified across Show=true and Show=false):
    //   0 = BG fill, no publish (Show=true sub 0; not used by Show=false)
    //   1 = glyph rows 0..3, publish suppressed
    //   2 = glyph rows 4..7, publish (publishes merged rect)
    const unsigned char *line;
    uint8_t baseX;
    uint8_t baseY;
    uint8_t glyphX;

    if (idx < GLOBAL_FlagOverlayLine0Len) {
        line = GLOBAL_FlagOverlayLine0;
        baseX = 64u;
        baseY = 9u;
    } else {
        idx = (uint8_t)(idx - GLOBAL_FlagOverlayLine0Len);
        if (idx < GLOBAL_FlagOverlayLine1Len) {
            line = GLOBAL_FlagOverlayLine1;
            baseX = 50u;
            baseY = 17u;
        } else {
            idx = (uint8_t)(idx - GLOBAL_FlagOverlayLine1Len);
            line = GLOBAL_FlagOverlayLine2;
            baseX = 48u;
            baseY = 24u;
        }
    }
    glyphX = (uint8_t)(baseX + idx * PORT_PICO8_FONT_ADVANCE_PX);

    if (phase == 0u) {
        bg1TextFillRectNoPublish((uint16_t)glyphX << 1,
                                 (uint16_t)baseY << 1,
                                 (uint16_t)FLAG_OVERLAY_PER_CHAR_BG_W,
                                 16u,
                                 PORT_BG1_TEXT_COLOR_BG);
    } else {
        // Phase n (1..4): glyph quarter (n-1), rendering rows
        // [(n-1)*2, (n-1)*2 + 2). Publish only on the last quarter.
        uint8_t flags = (uint8_t)(BG1_TEXT_RENDER_QUARTER_MODE |
                                  (uint8_t)(phase - 1u));
        if (phase < FLAG_OVERLAY_LAST_CHAR_PHASE) {
            flags |= BG1_TEXT_RENDER_NO_PUBLISH;
        }
        s_bg1TextRenderFlags = flags;
        port_drawTextWhiteOnBlackNoFillN(line + idx, 1u, glyphX, baseY);
        s_bg1TextRenderFlags = 0u;
    }
}

PORT_BG1_TEXT_CODE
static void renderFlagOverlay(void)
{
    if (GLOBAL_FlagOverlayDirty) {
        s_flagOverlayCharIdx = 0u;
        if (GLOBAL_FlagOverlayShow) {
            // Show=true: skip box fill, run all 3 phases per char.
            s_flagOverlayBoxStep = 0u;
            s_flagOverlayCharPhase = 0u;
        } else {
            // Show=false: run box fill, then 2 glyph phases per char (skip
            // BG-fill phase 0 since cells are already INK).
            s_flagOverlayBoxStep = 1u;
            s_flagOverlayCharPhase = 1u;
        }
        GLOBAL_FlagOverlayDirty = false;
    }

    if (s_flagOverlayBoxStep != 0u) {
        s_bg1TextPublishHidden = true;
        flagOverlayDoBoxStep(s_flagOverlayBoxStep);
        s_bg1TextPublishHidden = false;
        if (s_flagOverlayBoxStep >= FLAG_OVERLAY_BOX_FILL_STEPS) {
            s_flagOverlayBoxStep = 0u;
        } else {
            ++s_flagOverlayBoxStep;
        }
    } else {
        uint8_t totalChars = (uint8_t)(GLOBAL_FlagOverlayLine0Len +
                                       GLOBAL_FlagOverlayLine1Len +
                                       GLOBAL_FlagOverlayLine2Len);
        if (s_flagOverlayCharIdx < totalChars) {
            s_bg1TextPublishHidden = true;
            flagOverlayDoCharStep(s_flagOverlayCharIdx, s_flagOverlayCharPhase);
            s_bg1TextPublishHidden = false;
            // Phase 4 is the last for both Show modes; on completion advance
            // to the next char and reset phase to its starting value (0 for
            // Show=true, 1 for Show=false).
            if (s_flagOverlayCharPhase >= FLAG_OVERLAY_LAST_CHAR_PHASE) {
                s_flagOverlayCharPhase = GLOBAL_FlagOverlayShow ? 0u : 1u;
                ++s_flagOverlayCharIdx;
            } else {
                ++s_flagOverlayCharPhase;
            }
        }
    }

    if (GLOBAL_FlagOverlayRevealDirty) {
        if (GLOBAL_FlagOverlayShow) {
            // If pre-render hasn't finished yet, the box reveals empty/partial
            // and subsequent per-char steps will fill it in over a few frames.
            s_bg1TextMapDirtyRowBits |= 0x000Fu;
        } else {
            (void)bg1TextHideRect(64u, 0u, 144u, 64u);
        }
        GLOBAL_FlagOverlayRevealDirty = false;
    }
}

void port_renderTextOverlays(void)
{
    port_prg_bank_enter(4);
    renderFlagOverlay();
    port_prg_bank_leave();
}

// Run the flag overlay state machine to completion in one tight loop with
// the display force-blanked. Called from LoadRoomData when the room contains
// a flag, so the per-A-frame state machine doesn't burn gameplay cycles
// during background pre-render. Lines must already be populated and dirty
// flag set (flagUpdate handles this from processObject's first pass).
PORT_FUNC_BANK4
static void drainFlagOverlayPreRender(void)
{
    // Called from inside LoadRoomData, after port_LoadRoomData has already
    // engaged force-blank for the duration of the transition. No need to
    // touch INIDISP here — the deferred un-blank in port_vblank handles it
    // once hardware OAM is back in sync.
    uint16_t guard;
    for (guard = 0u; guard < 256u; ++guard) {
        uint8_t totalChars = (uint8_t)(GLOBAL_FlagOverlayLine0Len +
                                       GLOBAL_FlagOverlayLine1Len +
                                       GLOBAL_FlagOverlayLine2Len);
        if (totalChars == 0u) break;
        if (s_flagOverlayBoxStep == 0u &&
            s_flagOverlayCharIdx >= totalChars &&
            !GLOBAL_FlagOverlayDirty) {
            break;
        }
        renderFlagOverlay();
    }
}

void port_drainFlagOverlayPreRender(void)
{
    port_prg_bank_enter(4);
    drainFlagOverlayPreRender();
    port_prg_bank_leave();
}

// Fixed-mirror trampoline so bank-5 code can reach bank-4 preBuildDma.
PORT_NOINLINE
static void bg1TextPreBuildDmaViaBank4(void)
{
    port_prg_bank_enter(4);
    bg1TextPreBuildDma();
    port_prg_bank_leave();
}

// Bank-5 drain loop. Each iteration stages up to PORT_BG1_TEXT_DMA_STAGE_COUNT
// slot tiles via bg1TextPreBuildDma (bank 4) and DMAs them via bg1TextDmaFlush
// (bank 5, same bank — intra-bank JSR). Loops until both dirty-flag groups
// clear. The 32-iter guard prevents an infinite loop if the dirty state
// somehow doesn't settle (each pass should clean ≥2 slots so 32 covers all
// 48 slots with headroom).
PORT_FUNC_BANK5
static void drainBg1TextDmaLoop(void)
{
    uint8_t guard;
    for (guard = 0u; guard < 32u; ++guard) {
        if (!s_bg1TextAnySlotDirty && s_bg1TextMapDirtyRowBits == 0u) break;
        bg1TextPreBuildDmaViaBank4();
        bg1TextDmaFlush();
    }
}

// Public entry. Call from LoadRoomData (under force-blank) to push all pending
// BG1 text slot tiles and map rows to VRAM before the next un-blank — saves
// the ~7-vblank wait that the normal 2-slots-per-vblank throttle would impose.
void port_drainBg1TextDma(void)
{
    port_prg_bank_enter(5);
    drainBg1TextDmaLoop();
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

    if (!s_titleMode || s_dedicationActive) {
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
    // Apply title-screen start-flash CGRAM update first (4 BG3 palette 0
    // colours + master backdrop reset). Doing this before OAM DMA ensures
    // the palette swap lands at the very start of vblank, well clear of
    // active display — staged by applyTitleFlash() during the prior
    // handleTitleScreenFrame() call. 1-frame display lag is invisible at
    // the strobe's cadence.
    if (GLOBAL_TitleFlashCgramDirty != 0u) {
        port_prg_bank_enter(7);
        port_titleFlashFlush();
        port_prg_bank_leave();
        GLOBAL_TitleFlashCgramDirty = 0u;
    }

    snesXC_setDataBank(0x7Eu);
    LoadOAMCopy((char *)GLOBAL_OAMCopy.Bytes, 0x0000, sizeof(union uOAMCopy));
    snesXC_setDataBank(BANK_00);

    // After the OAM DMA, hardware OAM is in sync with the new room's
    // sprites — safe to un-blank if a transition was waiting. LoadRoomData
    // now calls port_drainBg1TextDma before returning, so by the time we
    // reach this check all BG1 text slots are already in VRAM — no need
    // to gate on s_bg1TextAnySlotDirty here.
    if (s_pendingDisplayEnable) {
        REG_INIDISP = 0x0Fu;
        s_pendingDisplayEnable = false;
    }

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

    if (!s_titleMode || s_dedicationActive) {
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
            port_prg_bank_enter(5);
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
            port_prg_bank_enter(5);
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
