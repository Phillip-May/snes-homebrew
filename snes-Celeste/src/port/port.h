#ifndef PORT_H
#define PORT_H

#include <stdint.h>
#include <stdbool.h>

#include "../fixedPointSNES.h"

// Include platform-specific sprite animation enums
#ifdef __NES__
    #include "sprite_animation_enums_nes.h"
#else
    #include "sprite_animation_enums_snes.h"
#endif

#define PORT_INPUT_RIGHT_MASK    0x01u
#define PORT_INPUT_LEFT_MASK     0x02u
#define PORT_INPUT_DOWN_MASK     0x04u
#define PORT_INPUT_UP_MASK       0x08u
#define PORT_INPUT_START_MASK    0x10u
#define PORT_INPUT_SELECT_MASK   0x20u
#define PORT_INPUT_Y_MASK        0x40u
#define PORT_INPUT_B_MASK        0x80u
#define PORT_INPUT_A_MASK        0x80u
#define PORT_INPUT_X_MASK        0x40u
#define PORT_INPUT_L_MASK        0x20u
#define PORT_INPUT_R_MASK        0x10u

#ifndef CLAMP
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#endif

struct sOBJ_DATA;
struct sPlayerData;
typedef struct {
    int16_t x;
    int16_t y;
} VEC_I;

typedef struct {
    fixed_t x;
    fixed_t y;
} VEC_F;

typedef struct sOBJ_DATA {
    VEC_I pos;
    uint8_t flags;
    uint8_t oamTile;
    uint8_t oamProps;
    uint8_t extraSpriteBase;
    uint8_t extraSpriteCount;
    enum eOBJType {
        OBJ_UNUSED = 0,
        OBJ_PLAYER,
        OBJ_SMOKE,
        OBJ_DOUBLE_JUMP_ORB,
        //Map placed objects
        OBJ_KEY = 8,
        OBJ_PLATMOV_R = 11,
        OBJ_PLATMOV_L = 12,
        OBJ_SPRING = 18,
        OBJ_CHEST = 20,
        OBJ_BALLOON = 22,
        OBJ_COLLAPSE_TILE = 23,
        OBJ_STRAWBERRY = 26,
        OBJ_FLYING_BERRY = 28,
        OBJ_DECO_TREE = 60,
        OBJ_DECO_FLOWER = 62,
        OBJ_BREAKABLE_WALL = 64,
        OBJ_MONUMENT = 70,
        OBJ_BIG_CHEST = 96,
        OBJ_BIG_CHEST_2 = 97
    } eType;
    union sOBJData{
        struct sSmokeData{
            uint8_t frameCount;
            uint8_t smokeSpriteState;
            uint8_t speedX;
            uint8_t speedY;
            bool flipX;
            bool flipY;
        } smoke;
        struct sStrawberryData{
            uint8_t startY;
            uint16_t frameCount;
            uint8_t isCollected;    
            uint8_t bgTextX;
            uint8_t bgTextY;
        } strawberry;
        struct sSpringData{
            bool isDisabled;
            uint8_t frameCount;
            int8_t linkedCollapseTileIndex;
        } spring;
        struct sCollapseTileData{
            uint8_t state;
            uint8_t frameCount;
            int8_t linkedSpringIndex;
        } collapseTile;
        struct sBalloonData{
            uint8_t state;
            uint8_t frameCount;
            uint16_t yTableIndex;
            uint8_t hideFrameCount;
            int8_t spriteYOffset;
            uint8_t stringTile;
        } balloon;
        struct sPlatMovData{
            uint8_t acc;
            uint8_t hitboxIndex;
            bool isMovingLeft;
            int8_t lastDelta; // per-frame movement for player carry
        } platMov;
        struct sKeyData{
            uint8_t linkedChestIndex;
            bool isFlipped;
            uint8_t spriteValue;
            uint8_t frameCount;
            uint8_t state;
        } key;
        struct sChestData {
            bool keyIsCollected;
            uint8_t frameCount;
            uint8_t state;
        } chest;
        struct sMonumentData {
            uint8_t state;
        } monument;
        struct sBigChestData {
            uint8_t state;
            uint8_t frameCount;
        } bigChest;
        struct sDoubleJumpOrbData {
            int8_t frameCount;
            int16_t speedY;
            int16_t accelAccumulator;
        } doubleJumpOrb;
    } data;
    
} OBJ_DATA;

struct sPlayerData
{
    OBJ_DATA objData;
    VEC_F posF;
    int8_t movingPlatformIndex;

    VEC_F spd;

    enum ePlayerSprite eSriteState;
    uint8_t graceTimer;

    bool doubleDashUnlocked;
    VEC_F dashTarget;
    VEC_F dashAccel;
    uint8_t dashesLeft;
    int8_t dashCounter;

    bool isFliped;
};

struct sActiveLevelData
{
    uint16_t currentRoomID;
    uint16_t roomSizeX;
    uint16_t roomSizeY;
    bool textChanged;
    bool swapCloudPal;
    bool swapActivePalette;
    bool textFlashActive;
    bool textScrollActive;

#ifdef __NES__
    uint8_t _collisionResetPad; // NES: collision restored from ROM via port_restoreCollisionFlags
#else
    uint8_t collisionFlagsReset[256];
#endif
    uint8_t collisionFlagsArr[256];

    uint8_t movingPlatformCount;
    uint8_t movingPlatformDir[12];
    uint8_t movingPlatformHitboxes[12 * 4];

    uint8_t shakeFrames;
    uint8_t textScrollOffsetX;
    uint8_t textScrollOffsetY;
    uint8_t scrollPointY;
    uint8_t playerSpawnX;
    uint8_t playerSpawnY;
    uint8_t objectCount;
    uint8_t objectData[42]; // max 13 objects * 3 bytes + 3 padding
    bool isLevelLoadedVRAM;
};

#define OBJ_FLAG_DIRTY 0x80u
#define PORT_EXTRA_SLOT_UNUSED 0xFFu

// Object list size - must be defined once and used consistently across all files
#define GLOBAL_OBJ_LIST_SIZE 29

extern struct sActiveLevelData GLOBAL_ActiveLevel;
extern uint16_t GLOBAL_FrameCount;

void port_init(void);
void port_beginSpriteBuild(const struct sPlayerData *playerObj);
void port_finishSpriteBuild(void);
void port_updatePlayerSprite(const struct sPlayerData *playerObj);
void port_buildUnused(uint8_t index);
void port_buildSmoke(uint8_t index);
void port_buildBreakableWall(uint8_t index);
void port_buildBalloon(uint8_t index);
void port_buildMonument(uint8_t index);
void port_buildChest(uint8_t index);
void port_buildBigChest(uint8_t index);
void port_buildKey(uint8_t index);
void port_buildSpring(uint8_t index);
void port_buildCollapseTile(uint8_t index);
void port_updateCollapseTileNametable(uint8_t index);
void port_buildStrawberry(uint8_t index);
void port_buildPlatMov(uint8_t index);
void port_buildFlyingBerry(uint8_t index);
void port_buildDoubleDashOrb(uint8_t index);
void port_buildStaticDecor(uint8_t index);
void port_buildSpriteIfDirty(uint8_t index, enum eOBJType eType);
void port_resetSprites(void);
void port_drawText(const unsigned char *text, uint8_t x, uint8_t y);
uint8_t port_getInputs(void);
void port_vblank(void);
void port_audioInit(void);
void port_audioUpdate(void); // Per-frame audio update hook (target-specific backend)
void port_audioPlayMusic(uint8_t pattern);
void port_audioPlaySfx(uint8_t sfxID);
void port_audioStopAll(void);
void port_LoadRoomData(uint16_t roomID);
void port_restoreCollisionFlags(void); // Re-derive collision from ROM (for future collisionFlagsReset removal)
void port_levelAnimAdvance(void); // Advance level animation on display frame (level 4)

#define BANK_MUSIC 7 // Reserved for FamiStudio driver + song data

#ifdef __NES_UNROM_512__
// PRG-ROM bank switching for UNROM-512 (non-LLVM-MOS compilers)
#include <mapper.h>
#endif

// Macros for function section attributes (bank placement)
#ifdef __NES_UNROM_512__
#define PORT_FUNC_BANK6 __attribute__((section(".prg_rom_6")))
#define PORT_FUNC_BANK5 __attribute__((section(".prg_rom_5")))
#define PORT_FUNC_BANK4 __attribute__((section(".prg_rom_4")))
#define PORT_FUNC_BANK3 __attribute__((section(".prg_rom_3")))
#define PORT_FUNC_BANK2 __attribute__((section(".prg_rom_2")))
#define PORT_FUNC_BANK1 __attribute__((section(".prg_rom_1")))
#define PORT_FUNC_BANK0 __attribute__((section(".prg_rom_0")))
#else
#define PORT_FUNC_BANK6
#define PORT_FUNC_BANK5
#define PORT_FUNC_BANK4
#define PORT_FUNC_BANK3
#define PORT_FUNC_BANK2
#define PORT_FUNC_BANK1
#define PORT_FUNC_BANK0
#endif

// Macros for data/rodata section attributes (bank placement)
// For const/rodata, we need to use .rodata.prg_rom_X to avoid conflicts with code sections
#ifdef __NES_UNROM_512__
#define PORT_DATA_BANK6 __attribute__((section(".prg_rom_6.rodata")))
#define PORT_DATA_BANK5 __attribute__((section(".prg_rom_5.rodata")))
#define PORT_DATA_BANK4 __attribute__((section(".prg_rom_4.rodata")))
#define PORT_DATA_BANK3 __attribute__((section(".prg_rom_3.rodata")))
#define PORT_DATA_BANK2 __attribute__((section(".prg_rom_2.rodata")))
#define PORT_DATA_BANK1 __attribute__((section(".prg_rom_1.rodata")))
#define PORT_DATA_BANK0 __attribute__((section(".prg_rom_0.rodata")))
#else
#define PORT_DATA_BANK6
#define PORT_DATA_BANK5
#define PORT_DATA_BANK4
#define PORT_DATA_BANK3
#define PORT_DATA_BANK2
#define PORT_DATA_BANK1
#define PORT_DATA_BANK0
#endif

// Wrapper function for bank switching (portable interface)
static inline void port_prg_bank_switch(uint8_t bank) {
#ifdef __NES_UNROM_512__
    set_prg_bank(bank);
#endif
}

#endif /* PORT_H */

