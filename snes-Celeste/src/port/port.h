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

    uint8_t collisionFlagsReset[256];
    uint8_t collisionFlagsArr[256];

    uint8_t movingPlatformCount;
    uint8_t movingPlatformDir[16];
    uint8_t movingPlatformHitboxes[16 * 4];

    uint8_t shakeFrames;
    uint8_t textScrollOffsetX;
    uint8_t textScrollOffsetY;
    uint8_t scrollPointY;
    uint8_t playerSpawnX;
    uint8_t playerSpawnY;
    uint8_t objectCount;
    uint8_t objectData[64];
    bool isLevelLoadedVRAM;
};

#define OBJ_FLAG_DIRTY 0x80u
#define PORT_EXTRA_SLOT_UNUSED 0xFFu

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
void port_resetSprites(void);
void port_drawText(const unsigned char *text, uint8_t x, uint8_t y);
uint8_t port_getInputs(void);
void port_vblank(void);
void port_LoadRoomData(uint16_t roomID);

#ifdef __mos__
// For LLVM-MOS UNROM-512, use mapper.h API for bank switching
#include <mapper.h>
#define prg_bank_switch set_prg_bank
#elif defined(__NES_UNROM_512__)
// PRG-ROM bank switching for UNROM-512 (non-LLVM-MOS compilers)
void prg_bank_switch(uint8_t bank);
#endif

#endif /* PORT_H */

