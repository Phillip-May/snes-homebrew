//LoROM mememory map
#include "fixedPointSNES.h"

#include <inttypes.h>
//#include <STDINT.H> // STDINT is cursed on wdc816cc

#include <STDIO.H>
#include <STRING.H>
#include <STDARG.h>
#include <stdbool.h>
#include <limits.h>

#include "../lib/SNES.h"
#include "mainBankZero.h"
#include "../lib/initsnes.h"

#include "celesteSimple.h"

#include "clouds.h"
#include "sprite_data.h"

#include "snes_font.h"

// 60fps vs 30fps physics scaling factor

//Level data, ideally place these ocntiguiously in the same bank
//Each one is 2347 bytes pre object data
//32 levels, 2347 * 32 = 75104 bytes
//So it'll need to be split into 2 banks
//13 ish levels per bank
#pragma SECTION CONST=CEL_K_00
/**/
#include "levelDat/tilemap_level1.h"
#include "levelDat/tilemap_level2.h"
#include "levelDat/tilemap_level3.h"
#include "levelDat/tilemap_level4.h"
#include "levelDat/tilemap_level5.h"
#include "levelDat/tilemap_level6.h"
#include "levelDat/tilemap_level7.h"
#include "levelDat/tilemap_level8.h"
#include "levelDat/tilemap_level9.h"
#include "levelDat/tilemap_level10.h"
#include "levelDat/tilemap_level11.h"
#include "levelDat/tilemap_level12.h"
#include "levelDat/tilemap_level13.h"
#pragma SECTION CONST=CEL_K_01
#include "levelDat/tilemap_level14.h"
#include "levelDat/tilemap_level15.h"
#include "levelDat/tilemap_level16.h"
#include "levelDat/tilemap_level17.h"
#include "levelDat/tilemap_level18.h"
#include "levelDat/tilemap_level19.h"
#include "levelDat/tilemap_level20.h"
#include "levelDat/tilemap_level21.h"
#include "levelDat/tilemap_level22.h"
#include "levelDat/tilemap_level23.h"
#include "levelDat/tilemap_level24.h"
#include "levelDat/tilemap_level25.h"
#include "levelDat/tilemap_level26.h"
#pragma SECTION CONST=CEL_K_02
#include "levelDat/tilemap_level27.h"
#include "levelDat/tilemap_level28.h"
#include "levelDat/tilemap_level29.h"
#include "levelDat/tilemap_level30.h"
#include "levelDat/tilemap_level31.h"
#include "levelDat/tilemap_level32.h"


//These I've manually set, ideally a map editor would set these



#pragma SECTION CONST=CONST

//8x8 sprites, 4bpp indexed sprite data
//16x4 sprite sheet is stored sequentially

//Prototypes
int16_t randint16(int16_t min, int16_t max);
void drawTextBG1(const unsigned char *text,uint8_t x, uint8_t y);

//Basic math functions that a compiler should have
static int16_t sign(int16_t v) {
    return v > 0 ? 1 : (v < 0 ? -1 : 0);
}

enum eSoundEffect {
    SOUND_EFFECT_JUMP = 0,
    SOUND_EFFECT_DASH_START = 1,
    SOUND_EFFECT_DASH_END = 2,
    SOUND_EFFECT_DASH_RESTORED = 3,
    SOUND_EFFECT_DASH_MISFIRE = 4,
    SOUND_EFFECT_DEATH = 5,
    SOUND_EFFECT_BREAKABLE_WALL_HIT = 6,
    SOUND_EFFECT_STRAWBERRY = 7,
    SOUND_EFFECT_SPRING = 8,
    SOUND_EFFECT_BALLOON_POP = 9,
    SOUND_EFFECT_KEY_COLLECT = 10,
    SOUND_EFFECT_TEXT_DISPLAY = 11,
    SOUND_EFFECT_BIG_CHEST = 12
};

void playSoundEffect(enum eSoundEffect soundEffect){
    //Play sound effect
    return;
}

char testStringRam[40];


//Black and white by default
bool GLOBAL_RELOADBG1VRAM = false;
uint8_t GLOBAL_BG1Pal[16];
bool GLOBAL_SwapCloudPal = false;
uint8_t GLBOAL_M0BG1TileMap[32*32];

union uOAMCopy GLOBAL_OAMCopy;
uint8_t GLOBAL_InputLo = 0;
uint8_t GLOBAL_InputHi = 0;



typedef struct {
    fixed_t x;
    fixed_t y;
} VEC_F;

typedef struct {
    int16_t x;
    int16_t y;
} VEC_I;

enum smokeStates {SMOKE_SPRITE_1 = 0x62,SMOKE_SPRITE_2 = 0x64,SMOKE_SPRITE_3 = 0x66};

typedef struct {
    VEC_I pos;
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

#define GLBOAL_OBJ_LIST_SIZE 30
OBJ_DATA GLOBAL_OBJList[GLBOAL_OBJ_LIST_SIZE] = {0};
struct gameStateData {
    uint16_t currentRoomID;
} GLOBAL_GameState;

//Global data used to update hardware registers
uint16_t GLOBAL_ScrollBG1X = 0;
uint16_t GLOBAL_ScrollBG1Y = 0;

uint16_t GLOBAL_ScrollBG2X = 0;
uint16_t GLOBAL_ScrollBG2Y = 0;

uint16_t GLOBAL_ScrollBG3X = 0;
uint16_t GLOBAL_ScrollBG3Y = 0;

uint16_t GLOBAL_ScrollBG4X = 0;
uint16_t GLOBAL_ScrollBG4Y = 0;


uint16_t GLOBAL_FrameCountVBLANK = 0;
uint16_t GLOBAL_FrameCount = 0;

uint8_t GLOBAL_FreezeFrames = 0;
uint8_t GLOBAL_ShakeFrames = 0;

uint8_t GLOBAL_PausePlayerFrames = 0;
//Game state globals
bool GLOBAL_DoubleDashUnlocked = false;


enum eMovingPlatformDir {MOVING_PLATFORM_DIR_IDLE = 0, MOVING_PLATFORM_DIR_LEFT = 1, MOVING_PLATFORM_DIR_RIGHT = 2};

struct sActiveLevelData
{
    //Setup data
    uint16_t roomSizeX, roomSizeY;

    uint8_t *tilemapBg2;
    uint8_t *tilemapBg3;
    uint8_t *paletteBg;

    //Runtime data
    const uint8_t* collisionFlagsPTR; //Points to const data
    uint8_t collisionFlagsArr[256];

    //List of moving platform hitboxes
    //StartX, StartY, EndX, EndY
    uint8_t movingPlatformCount;
    uint8_t movingPlatformDir[16];
    uint8_t movingPlatformHitboxes[16*4];

    uint8_t scrollPointY;
    uint8_t playerSpawnX, playerSpawnY;
    //Something to handle object data here
    uint8_t objectCount;
    uint8_t *objectData;
    bool isLevelLoadedVRAM;
} GLOBAL_ActiveLevel;

struct sPlayerData
{
    OBJ_DATA objData;
    VEC_F posF;
    int8_t movingPlatformIndex;

    VEC_F spd;

    enum ePlayerSprite {PLAYER_SPRITE_IDLE = 0,
        PLAYER_SPRITE_WALK_1 = 2,
        PLAYER_SPRITE_WALK_2 = 4,
        PLAYER_SPRITE_WALK_3 = 6,
        PLAYER_SPRITE_WALL = 8,
        PLAYER_SPRITE_DOWN = 10,
        PLAYER_SPRITE_UP = 12
    } eSriteState;
    uint8_t graceTimer;

    bool doubleDashUnlocked;
    VEC_F dashTarget;
    VEC_F dashAccel;
    uint8_t dashesLeft;
    int8_t dashCounter;


    bool isFliped;
} GLOBAL_PlayerData;

#define CALC_OAM_TABLE2_BYTE(index, sizeBit, xBit, currentByte) \
    (((currentByte) & ~(0x03 << (((index) % 4) * 2))) | ((((sizeBit) << 1) | (xBit)) << (((index) % 4) * 2)))

void initObject(enum eOBJType eType, int16_t x, int16_t y);

void smokeInit(uint8_t index) {
    GLOBAL_OBJList[index].data.smoke.frameCount = 0;
    GLOBAL_OBJList[index].data.smoke.smokeSpriteState = SMOKE_SPRITE_1;
    GLOBAL_OBJList[index].pos.x += randint16(-1,1);
    GLOBAL_OBJList[index].pos.y += randint16(-1,1);

    //Number of frames to wait before moving pixels
    //Effectively pixels/s = 1/value
    GLOBAL_OBJList[index].data.smoke.speedX = randint16(3,5);
    GLOBAL_OBJList[index].data.smoke.speedY = randint16(3,5);
    GLOBAL_OBJList[index].data.smoke.flipX = randint16(0,1);
    GLOBAL_OBJList[index].data.smoke.flipY = randint16(0,1);    
    { //One time setup of the sprite
        uint8_t properties = 0;
        uint8_t table2Index = index / 4; //4 entries per byte
        uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
        GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;

        properties |= 0x30; //Set priority to 3
        properties |= 0x04; //Set palette to 2
        properties |= ((uint8_t)GLOBAL_OBJList[index].data.smoke.flipX) << 6;
        properties |= ((uint8_t)GLOBAL_OBJList[index].data.smoke.flipY) << 7;
        GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    }
}

void smokeUpdate(uint8_t index) {
    uint8_t properties = 0;
    uint8_t table2Write = 0;
    GLOBAL_OBJList[index].data.smoke.frameCount++;
    // 1/3 of it's time per state
    GLOBAL_OBJList[index].data.smoke.smokeSpriteState = SMOKE_SPRITE_1 + ((GLOBAL_OBJList[index].data.smoke.frameCount / 10) * 2);
    // 0.5 seconds
    if (GLOBAL_OBJList[index].data.smoke.frameCount >= 30) {
        //Destroy self
        GLOBAL_OBJList[index].eType = OBJ_UNUSED;
        // Hide the sprite by moving it off-screen
        GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;        
        return;
    }

    //Drawing code
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = GLOBAL_OBJList[index].pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = GLOBAL_OBJList[index].pos.y - GLOBAL_ScrollBG2Y;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = GLOBAL_OBJList[index].data.smoke.smokeSpriteState;

}


#define COLLISION_FLAG_INDEX_FROM_TILE_XY(x,y) ((x) + (y) * 16)
void breakableWallInit(uint8_t index) {
    enum eBreakableWallSprite {BREAKABLE_WALL_SPRITE_1 = 0x82};
    uint8_t properties = 0;
    uint8_t tileX = GLOBAL_OBJList[index].pos.x / 16;
    uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 16;
    GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)]     |= 0x01; //Set the solid flag
    GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY)]   |= 0x01; //Set the solid flag
    GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY+1)]   |= 0x01; //Set the solid flag
    GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY+1)] |= 0x01; //Set the solid flag

    properties |= 0x30; //Set priority to 3
    properties |= 0x02; //Set palette to 1
    properties |= 0x00; //Set flipX to 0
    properties |= 0x00; //Set flipY to 0
    { //One time setup of the sprite
        uint8_t i;
        uint8_t properties = 0;
        uint8_t table2Index = index / 4; //4 entries per byte
        uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
        GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
        //Coop some slots for the rest of this object
        for (i = 1; i < 4; i++) {
            table2Index = (GLBOAL_OBJ_LIST_SIZE+i) / 4; //4 entries per byte
            byteWrite = CALC_OAM_TABLE2_BYTE(GLBOAL_OBJ_LIST_SIZE+i, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
            GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
        }
    }
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = BREAKABLE_WALL_SPRITE_1;
    //Coop some slots for the rest of this object
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+1].CHARNUM = BREAKABLE_WALL_SPRITE_1;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+1].PROPERTIES = properties | 0xC0; //Set XY and flip to 1
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+2].PROPERTIES = properties | 0x40; //Set flipY to 1
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+2].CHARNUM = BREAKABLE_WALL_SPRITE_1;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+3].PROPERTIES = properties | 0x80; //Set flipX to 1
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+3].CHARNUM = BREAKABLE_WALL_SPRITE_1;

}

void breakableWallUpdate(uint8_t index) {
    //Check if player is touching the wall
    uint8_t thisX = GLOBAL_OBJList[index].pos.x;
    uint8_t thisY = GLOBAL_OBJList[index].pos.y;
    uint8_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint8_t playerY = GLOBAL_PlayerData.objData.pos.y;

    //Fixed collision detection to properly detect hits from above
    bool isPlayerTouching = playerX > thisX-8 && playerX < thisX+40 && playerY > thisY-20 && playerY < thisY+40;
    if (isPlayerTouching && (GLOBAL_PlayerData.dashCounter > 0)) {
        uint8_t tileX = GLOBAL_OBJList[index].pos.x / 16;
        uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 16;
        //Player collision with wall
        GLOBAL_PlayerData.spd.x = FIXED_MUL(-sign(GLOBAL_PlayerData.spd.x), FLOAT_TO_FIXED(1.5f*2));
        GLOBAL_PlayerData.spd.y = FLOAT_TO_FIXED(-1.5f*2);
        GLOBAL_PlayerData.dashCounter = -1;
        playSoundEffect(SOUND_EFFECT_BREAKABLE_WALL_HIT);
        initObject(OBJ_SMOKE, thisX, thisY);
        initObject(OBJ_SMOKE, thisX + 16, thisY);
        initObject(OBJ_SMOKE, thisX, thisY + 16);
        initObject(OBJ_SMOKE, thisX + 16, thisY + 16);
        initObject(OBJ_STRAWBERRY, thisX + 8, thisY + 8);

        //Destroy the object
        GLOBAL_OBJList[index].eType = OBJ_UNUSED;
        GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240; //Hide the sprite
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+1].OBJY = 240; //Hide the sprite
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+2].OBJY = 240; //Hide the sprite
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+3].OBJY = 240; //Hide the sprite
        //Restore the original collision data
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)]     = GLOBAL_ActiveLevel.collisionFlagsPTR[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)];
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY)]   = GLOBAL_ActiveLevel.collisionFlagsPTR[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY)];
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY+1)]   = GLOBAL_ActiveLevel.collisionFlagsPTR[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY+1)];
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY+1)] = GLOBAL_ActiveLevel.collisionFlagsPTR[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY+1)];
        return;
    }
    
    //Drawing code
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = GLOBAL_OBJList[index].pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = GLOBAL_OBJList[index].pos.y - GLOBAL_ScrollBG2Y;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+1].OBJX = thisX+16;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+1].OBJY = thisY+16 - GLOBAL_ScrollBG2Y;

    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+2].OBJX = thisX+16;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+2].OBJY = thisY - GLOBAL_ScrollBG2Y;

    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+3].OBJX = thisX;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+3].OBJY = thisY+16 - GLOBAL_ScrollBG2Y;

}

enum eFlowerSprite {FLOWER_SPRITE_1 = 0x80};

void flowerInit(uint8_t index) {
    uint8_t properties = 0;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    properties |= 0x30; //Set priority to 3
    properties |= 0x02; //Set palette to 1
    properties |= 0x00; //Set flipX to 0
    properties |= 0x00; //Set flipY to 0

    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = FLOWER_SPRITE_1;
}

void flowerUpdate(uint8_t index) {
    //Drawing code
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = GLOBAL_OBJList[index].pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = GLOBAL_OBJList[index].pos.y - GLOBAL_ScrollBG2Y;
}

enum eCollapseTileSprite {
    COLLAPSE_TILE_SPRITE_1 = 0x48,
    COLLAPSE_TILE_SPRITE_2 = 0x4A,
    COLLAPSE_TILE_SPRITE_3 = 0x4C,
};

enum eCollapseTileState {
    COLLAPSE_TILE_STATE_IDLE = 0,
    COLLAPSE_TILE_STATE_COLLAPSING = 1,
    COLLAPSE_TILE_STATE_HIDDEN = 2,
};

void collapseTileInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    uint8_t tileX = GLOBAL_OBJList[index].pos.x / 16;
    uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 16;
    uint8_t i;
    GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)]     |= 0x01; //Set the solid flag
    this->data.collapseTile.state = COLLAPSE_TILE_STATE_IDLE;
    this->data.collapseTile.linkedSpringIndex = -1;

    //Check if there is a spring linked to this tile
    for (i = 0; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_SPRING) {
            uint8_t springTileX = GLOBAL_OBJList[i].pos.x / 16;
            uint8_t springTileY = (GLOBAL_OBJList[i].pos.y + 1) / 16;
            
            // Check if spring is adjacent to this collapse tile
            if ((springTileX == tileX && (springTileY == tileY - 1 || springTileY == tileY + 1)) ||
                (springTileY == tileY && (springTileX == tileX - 1 || springTileX == tileX + 1))) {
                //Link the two objects
                this->data.collapseTile.linkedSpringIndex = i;
                GLOBAL_OBJList[i].data.spring.linkedCollapseTileIndex = index;
                break;
            }
        }
    }

    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = COLLAPSE_TILE_SPRITE_1;
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = 0x32; //Set palette to 7
}

void collapseTileUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint16_t thisX = this->pos.x;
    uint16_t thisY = this->pos.y;
    uint16_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint16_t playerY = GLOBAL_PlayerData.objData.pos.y;
    bool isPlayerTouching = (thisX <= playerX + 20 && thisX + 20 >= playerX && 
                            thisY <= playerY + 20 && thisY + 20 >= playerY);

    // State machine for collapse tile
    switch (this->data.collapseTile.state) {
        case COLLAPSE_TILE_STATE_IDLE:
            if (this->data.collapseTile.linkedSpringIndex > 0) {
                OBJ_DATA *linkedSpringTile = &GLOBAL_OBJList[this->data.collapseTile.linkedSpringIndex];
                linkedSpringTile->data.spring.isDisabled = false;
            }
            if (isPlayerTouching) {
                this->data.collapseTile.state = COLLAPSE_TILE_STATE_COLLAPSING;
                this->data.collapseTile.frameCount = 30;
            }
            break;
            
        case COLLAPSE_TILE_STATE_COLLAPSING:
            this->data.collapseTile.frameCount -= 1;
            if (this->data.collapseTile.frameCount == 0) {
                uint8_t tileX = GLOBAL_OBJList[index].pos.x / 16;
                uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 16;
                GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)] &= ~0x01; //Unset the solid flag
                this->data.collapseTile.state = COLLAPSE_TILE_STATE_HIDDEN;
                this->data.collapseTile.frameCount = 120;
                GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
                GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = COLLAPSE_TILE_SPRITE_1;
                return;
            }
            GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = COLLAPSE_TILE_SPRITE_1 + (2 - (this->data.collapseTile.frameCount / 10)) * 2;
            break;
            
        case COLLAPSE_TILE_STATE_HIDDEN:
            GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
            if (this->data.collapseTile.linkedSpringIndex > 0) {
                OBJ_DATA *linkedSpringTile = &GLOBAL_OBJList[this->data.collapseTile.linkedSpringIndex];
                linkedSpringTile->data.spring.isDisabled = true;
            }
            if (this->data.collapseTile.frameCount > 0) {
                this->data.collapseTile.frameCount -= 1;
            }
            if (this->data.collapseTile.frameCount == 0 && !isPlayerTouching) {
                uint8_t tileX = GLOBAL_OBJList[index].pos.x / 16;
                uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 16;
                GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)] |= 0x01; //Set the solid flag
                this->data.collapseTile.state = COLLAPSE_TILE_STATE_IDLE;
            } else {
                //Don't draw the tile
                return;
            }
            break;
        default:
            break;
    }

    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = this->pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = this->pos.y - GLOBAL_ScrollBG2Y;
}

enum eSpringSprite {SPRING_SPRITE_1 = 0x2E, SPRING_SPRITE_2 = 0x40};

void springInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t properties = 0;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    this->data.spring.frameCount = 0;
    this->data.spring.isDisabled = false;
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    this->data.spring.linkedCollapseTileIndex = -1;

    properties |= 0x30; //Set priority to 3
    properties |= 0x02; //Set palette to 2
    properties |= 0x00; //Set flipX to 0
    properties |= 0x00; //Set flipY to 0
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = SPRING_SPRITE_1;
}

void springUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    struct sPlayerData *player = &GLOBAL_PlayerData;
    uint8_t thisX = this->pos.x;
    uint8_t thisY = this->pos.y;
    uint8_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint8_t playerY = GLOBAL_PlayerData.objData.pos.y;
    uint8_t properties = 0;
    bool isPlayerTouching = false;

    if (this->data.spring.isDisabled) {
        GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
        this->data.spring.frameCount = 0;
        return;
    }

    //Check if player is touching and moving downward
    isPlayerTouching = (playerX > thisX-8) && 
                       (playerX < thisX+24) && 
                       (playerY > thisY-6) && 
                       (playerY < thisY+20) &&
                       (GLOBAL_PlayerData.spd.y >= 0); //Only trigger when player is moving downward

    if (isPlayerTouching) {
        //Set player position to spring top
        GLOBAL_PlayerData.objData.pos.y = thisY - 4;
        GLOBAL_PlayerData.posF.y = INT_TO_FIXED(GLOBAL_PlayerData.objData.pos.y);
        
        //Apply spring physics
        GLOBAL_PlayerData.spd.x = FIXED_MUL(GLOBAL_PlayerData.spd.x, FLOAT_TO_FIXED(0.2f)); //Reduce horizontal speed
        GLOBAL_PlayerData.spd.y = INT_TO_FIXED(-3*2); //Set upward velocity (scaled for 60fps/256x256)
        GLOBAL_PlayerData.dashesLeft = player->doubleDashUnlocked ? 2 : 1; //Restore dashes
        
        //Spring animation and effects
        playSoundEffect(SOUND_EFFECT_SPRING);
        this->data.spring.frameCount = 10; //Original was 10, not 20
        initObject(OBJ_SMOKE, thisX, thisY);
        
        //Handle linked collapse tile
        if (this->data.spring.linkedCollapseTileIndex > 0) {
            OBJ_DATA *linkedCollapseTile = &GLOBAL_OBJList[this->data.spring.linkedCollapseTileIndex];
            if (linkedCollapseTile->data.collapseTile.state == COLLAPSE_TILE_STATE_IDLE) {
                linkedCollapseTile->data.collapseTile.state = COLLAPSE_TILE_STATE_COLLAPSING;
                linkedCollapseTile->data.collapseTile.frameCount = 30;
            }
        }
    }
    properties |= 0x30; //Set priority to 3
    properties |= 0x00; //Set flipX to 0
    properties |= 0x00; //Set flipY to 0

    //Spring animation state machine
    if (this->data.spring.frameCount > 0) {
        this->data.spring.frameCount--;
        properties |= 0x06; //Set palette to 6
        GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = SPRING_SPRITE_2;
        GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;        
    }
    else {
        properties |= 0x02; //Set palette to 2
        GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = SPRING_SPRITE_1;
        GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;        
    }

    //Drawing code
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = this->pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = this->pos.y - GLOBAL_ScrollBG2Y;
}

enum eBalloonSprite {BALLOON_SPRITE_1 = 0x46, BALLOON_STRING_1 = 0x28, BALLOON_STRING_2 = 0x2A, BALLOON_STRING_3 = 0x2C};
enum eBalloonState {BALLOON_STATE_IDLE = 0, BALLOON_STATE_POPPED = 1};
void balloonInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t stringIndex = GLBOAL_OBJ_LIST_SIZE+16+index;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    uint8_t properties = 0;
    this->data.balloon.state = BALLOON_STATE_IDLE;
    this->data.balloon.frameCount = 0;
    this->data.balloon.yTableIndex = 0;
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;

    properties |= 0x30; //Set priority to 3
    properties |= 0x06; //Set palette to 0
    properties |= 0x00; //Set flipX to 0
    properties |= 0x00; //Set flipY to 0
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = BALLOON_SPRITE_1;
    //Coop a slot for the string
    table2Index = stringIndex / 4;
    byteWrite = CALC_OAM_TABLE2_BYTE(stringIndex, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    properties = 0;
    properties |= 0x30; //Set priority to 3
    properties |= 0x04; //Set palette to 4
    properties |= 0x00; //Set flipX to 0
    properties |= 0x00; //Set flipY to 0
    GLOBAL_OAMCopy.arr.OAMArray[stringIndex].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[stringIndex].CHARNUM = BALLOON_STRING_1;

}



void balloonUpdate(uint8_t index) {
    static const uint8_t balloonStringFrames[75] = {
        40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 42, 42, 42, 42, 42, 42, 42, 42, 42,
        42, 42, 42, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 40, 40, 40, 40,
        40, 40, 40, 40, 40, 40, 40, 40, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
        42, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 40, 40,
    };
    #define BALLON_YTABLE_SIZE 304
    static const uint8_t balloon_ytable[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    OBJ_DATA *this = &GLOBAL_OBJList[index];
    int8_t yOffset = balloon_ytable[this->data.balloon.yTableIndex];
    uint8_t stringIndex = GLBOAL_OBJ_LIST_SIZE+16+index;
    bool isPlayerTouching = false;

    this->data.balloon.frameCount += 1;
    if (this->data.balloon.frameCount >= sizeof(balloonStringFrames)) {
        this->data.balloon.frameCount = 0;
    }

    this->data.balloon.yTableIndex += 1;
    if (this->data.balloon.yTableIndex >= BALLON_YTABLE_SIZE) {
        this->data.balloon.yTableIndex = 0;
    }
    isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x-16) && 
                       (GLOBAL_PlayerData.objData.pos.x < this->pos.x+16) && 
                       (GLOBAL_PlayerData.objData.pos.y > this->pos.y-8) && 
                       (GLOBAL_PlayerData.objData.pos.y < this->pos.y+18);

    if (isPlayerTouching && this->data.balloon.state == BALLOON_STATE_IDLE) {
        this->data.balloon.state = BALLOON_STATE_POPPED;
        this->data.balloon.hideFrameCount = 60*2;
        GLOBAL_PlayerData.dashesLeft = GLOBAL_PlayerData.doubleDashUnlocked ? 2 : 1;
        GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
        GLOBAL_OAMCopy.arr.OAMArray[stringIndex].OBJY = 240;
        playSoundEffect(SOUND_EFFECT_BALLOON_POP);
    }

    if (this->data.balloon.state == BALLOON_STATE_POPPED) {
        if (this->data.balloon.hideFrameCount > 0) {
            this->data.balloon.hideFrameCount -= 1;
        }
        if (this->data.balloon.hideFrameCount == 0) {
            this->data.balloon.state = BALLOON_STATE_IDLE;
        }
        return;
    }

    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = this->pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = this->pos.y - GLOBAL_ScrollBG2Y + yOffset;
    //Draw the string
    GLOBAL_OAMCopy.arr.OAMArray[stringIndex].OBJX = this->pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[stringIndex].OBJY = this->pos.y - GLOBAL_ScrollBG2Y + 14 + yOffset;
    GLOBAL_OAMCopy.arr.OAMArray[stringIndex].CHARNUM = balloonStringFrames[this->data.balloon.frameCount];
}


enum ePlatMovSprite {PLATMOV_SPRITE_1 = 0x24, PLATMOV_SPRITE_2 = 0x26};
void platMovInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->pos.y -= 2;
    uint8_t properties = 0;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    uint8_t rightSpriteIndex = index + GLBOAL_OBJ_LIST_SIZE + 20;
    uint8_t extraSpriteIndex = index + GLBOAL_OBJ_LIST_SIZE + 20 + GLBOAL_OBJ_LIST_SIZE;

    uint8_t table2IndexRight = rightSpriteIndex / 4;
    uint8_t byteWriteRight = CALC_OAM_TABLE2_BYTE(rightSpriteIndex, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2IndexRight]);
    uint8_t table2IndexExtra = extraSpriteIndex / 4;
    uint8_t byteWriteExtra = CALC_OAM_TABLE2_BYTE(extraSpriteIndex, 1, 1, GLOBAL_OAMCopy.arr.OAMTable2[table2IndexExtra]);
    uint8_t hitboxIndex;    
    if (this->eType == OBJ_PLATMOV_L) {
        this->data.platMov.isMovingLeft = false;
        GLOBAL_ActiveLevel.movingPlatformDir[GLOBAL_ActiveLevel.movingPlatformCount] = MOVING_PLATFORM_DIR_RIGHT;
    }
    else {
        this->data.platMov.isMovingLeft = true;
        GLOBAL_ActiveLevel.movingPlatformDir[GLOBAL_ActiveLevel.movingPlatformCount] = MOVING_PLATFORM_DIR_LEFT;
        this->pos.x -= 16;
    }
    
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    GLOBAL_OAMCopy.arr.OAMTable2[table2IndexRight] = byteWriteRight;
    GLOBAL_OAMCopy.arr.OAMTable2[table2IndexExtra] = byteWriteExtra;
    properties |= 0x30; //Set priority to 3
    properties |= 0x04; //Set palette to 2
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = PLATMOV_SPRITE_1;
    GLOBAL_OAMCopy.arr.OAMArray[rightSpriteIndex].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[rightSpriteIndex].CHARNUM = PLATMOV_SPRITE_2;
    //So the extra sprite is for the left side transition
    GLOBAL_OAMCopy.arr.OAMArray[extraSpriteIndex].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[extraSpriteIndex].CHARNUM = PLATMOV_SPRITE_2;
    //Add the hitbox to the list
    hitboxIndex = GLOBAL_ActiveLevel.movingPlatformCount * 4;
    this->data.platMov.hitboxIndex = hitboxIndex;
    GLOBAL_ActiveLevel.movingPlatformCount++;
    this->data.platMov.acc = 0;
}

void platMovUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t rightSpriteIndex = index + GLBOAL_OBJ_LIST_SIZE + 20;
    uint8_t extraSpriteIndex = index + GLBOAL_OBJ_LIST_SIZE + 20 + GLBOAL_OBJ_LIST_SIZE;
    uint8_t hitboxIndex = this->data.platMov.hitboxIndex;
    bool isPlayerTouching = false;

    //Fixed point math too expensive, trickery ensues, 0.65f speed is cursed
    this->data.platMov.acc += 65; // numerator
    if (this->data.platMov.acc >= 100) { // denominator
        this->data.platMov.acc -= 100;
        // Trigger event
        if (this->data.platMov.isMovingLeft) {
            this->pos.x -= 1;
        }
        else {
            this->pos.x += 1;
        }
    }



    GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex] = GLOBAL_OBJList[index].pos.x - 12;
    GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex + 1] = GLOBAL_OBJList[index].pos.y - 7;
    GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex + 2] = GLOBAL_OBJList[index].pos.x + 28;
    GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex + 3] = GLOBAL_OBJList[index].pos.y - 7;

    if (this->data.platMov.isMovingLeft) {
        if (GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex + 0] > 224) {
            GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex + 0] = 0;
        }
    }
    else {
        if (GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex + 2] < 16) {
            GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex + 2] = 255;
        }
    }


    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = GLOBAL_OBJList[index].pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = GLOBAL_OBJList[index].pos.y - GLOBAL_ScrollBG2Y;
    GLOBAL_OAMCopy.arr.OAMArray[rightSpriteIndex].OBJX = GLOBAL_OBJList[index].pos.x + 16;
    GLOBAL_OAMCopy.arr.OAMArray[rightSpriteIndex].OBJY = GLOBAL_OBJList[index].pos.y - GLOBAL_ScrollBG2Y;
    
    {
    uint8_t x = this->pos.x;
    int16_t xMod = x % 256;
    int16_t extraX = 0;
    uint8_t charNum = 0;
    
    if ((xMod > 240)  && (!this->data.platMov.isMovingLeft)) { // Right side transition
        extraX = x - 256;
        charNum = PLATMOV_SPRITE_1;
    } else if (((xMod + 16) > 240) && (!this->data.platMov.isMovingLeft)) {
        extraX = x - 256 + 16;
        charNum = PLATMOV_SPRITE_2;
    } else if ((xMod > 240) && this->data.platMov.isMovingLeft) { // Left side transition
        extraX = x + 256;
        charNum = PLATMOV_SPRITE_1;     
    } else if ((xMod > 224) && this->data.platMov.isMovingLeft) { // Left side transition
        extraX = x + 256 + 16;
        charNum = PLATMOV_SPRITE_2;     
    } else {
        GLOBAL_OAMCopy.arr.OAMArray[extraSpriteIndex].OBJY = 240;
        return;
    }
    
    GLOBAL_OAMCopy.arr.OAMArray[extraSpriteIndex].OBJX = extraX;
    GLOBAL_OAMCopy.arr.OAMArray[extraSpriteIndex].OBJY = this->pos.y - GLOBAL_ScrollBG2Y;
    GLOBAL_OAMCopy.arr.OAMArray[extraSpriteIndex].CHARNUM = charNum;
    }
}

enum eKeySprite {KEY_SPRITE_1 = 0x0E, KEY_SPRITE_2 = 0x20, KEY_SPRITE_3 = 0x22};
enum eKeyState {KEY_STATE_1 = 0, KEY_STATE_2 = 1, KEY_STATE_3 = 2, KEY_STATE_4 = 3};

void keyInit(uint8_t index) {
    uint8_t i;
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = KEY_SPRITE_1;
    this->data.key.frameCount = 0;
    this->data.key.linkedChestIndex = -1;
    this->data.key.state = KEY_STATE_1;

    for (i = 0; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_CHEST) {
            this->data.key.linkedChestIndex = i;
        }
    }

}

void keyUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t properties = 0;
    bool isPlayerTouching = false;
    this->data.key.frameCount += 1;
    
    isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x-16) && 
                       (GLOBAL_PlayerData.objData.pos.x < this->pos.x+16) && 
                       (GLOBAL_PlayerData.objData.pos.y > this->pos.y-18) && 
                       (GLOBAL_PlayerData.objData.pos.y < this->pos.y+2);

    if (isPlayerTouching) {
        OBJ_DATA *chest = &GLOBAL_OBJList[this->data.key.linkedChestIndex];
        chest->data.chest.keyIsCollected = true;
        this->eType = OBJ_UNUSED;
        GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
        playSoundEffect(SOUND_EFFECT_KEY_COLLECT);
        return;
    }

    //Animation
    switch (this->data.key.state) {
        case KEY_STATE_1:
            properties |= 0x02; //Set palette to 2
            this->data.key.spriteValue = KEY_SPRITE_1;
            if (this->data.key.frameCount > 18) {
                this->data.key.frameCount = 0;
                this->data.key.state = KEY_STATE_2;
            }
            break;
        case KEY_STATE_2:
            properties |= 0x02; //Set palette to 3
            this->data.key.spriteValue = KEY_SPRITE_2;
            if (this->data.key.frameCount > 10) {
                this->data.key.frameCount = 0;
                this->data.key.state = KEY_STATE_3;
            }
            break;
        case KEY_STATE_3:
        properties |= 0x04; //Set palette to 3
        this->data.key.spriteValue = KEY_SPRITE_3;
            if (this->data.key.frameCount > 18) {
                this->data.key.frameCount = 0;
                this->data.key.state = KEY_STATE_4;
                this->data.key.isFlipped = !this->data.key.isFlipped;
            }
            break;
        case KEY_STATE_4:
            properties |= 0x02; //Set palette to 2
            this->data.key.spriteValue = KEY_SPRITE_2;
            if (this->data.key.frameCount > 10) {
                this->data.key.frameCount = 0;
                this->data.key.state = KEY_STATE_1;
            }
            break;
    }

    properties |= 0x30; //Set priority to 3
    properties |= this->data.key.isFlipped ? 0x40 : 0x00; //Set flipX
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = this->data.key.spriteValue;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = GLOBAL_OBJList[index].pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = GLOBAL_OBJList[index].pos.y - GLOBAL_ScrollBG2Y;
}

enum eChestSprite {CHEST_SPRITE_1 = 0x42};
enum eChestState {CHEST_STATE_IDLE = 0, CHEST_STATE_SHAKING = 1, CHEST_STATE_OPEN = 2};

void chestInit(uint8_t index) {   
    uint8_t i;
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t properties = 0;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    this->pos.x -= 8;

    for (i = 0; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_KEY) {
            OBJ_DATA *key = &GLOBAL_OBJList[i];
            key->data.key.linkedChestIndex = index;
        }
    }
    this->data.chest.keyIsCollected = false;
    this->data.chest.frameCount = 0;
    this->data.chest.state = CHEST_STATE_IDLE;

    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    properties |= 0x30; //Set priority to 3
    properties |= 0x02; //Set palette to 2
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
}

void chestUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    int16_t shakeAmount = 0;
    switch (this->data.chest.state) {
        case CHEST_STATE_IDLE:
            this->data.chest.frameCount = 0;
            if (this->data.chest.keyIsCollected) {
                this->data.chest.state = CHEST_STATE_SHAKING;
            }
            break;
        case CHEST_STATE_SHAKING:
            shakeAmount = this->data.chest.frameCount > 0 ? ((this->data.chest.frameCount & 1) ? 2 : -2) : 0;
            this->pos.x += shakeAmount;
            this->data.chest.frameCount += 1;
            if (this->data.chest.frameCount > 40) {
                this->data.chest.frameCount = 0;
                this->data.chest.state = CHEST_STATE_OPEN;
            }
            break;
        case CHEST_STATE_OPEN:
            initObject(OBJ_STRAWBERRY, this->pos.x, this->pos.y - 8);
            GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
            this->eType = OBJ_UNUSED;
            return;
            break;
        default:
            break;
    }

    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = CHEST_SPRITE_1;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = GLOBAL_OBJList[index].pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = GLOBAL_OBJList[index].pos.y - GLOBAL_ScrollBG2Y;
}

enum eMonumentSprite {MONUMENT_SPRITE_1 = 0x84, MONUMENT_SPRITE_2 = 0x86, MONUMENT_SPRITE_3 = 0x88, MONUMENT_SPRITE_4 = 0x8A};
void monumentInit(uint8_t index) {
    uint8_t properties = 0;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    properties |= 0x30; //Set priority to 3
    properties |= 0x06; //Set palette to 5
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = MONUMENT_SPRITE_1;
    //Coop some slots for the rest of the monument
    table2Index = (GLBOAL_OBJ_LIST_SIZE+20) / 4;
    byteWrite = CALC_OAM_TABLE2_BYTE(GLBOAL_OBJ_LIST_SIZE+20, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    table2Index = (GLBOAL_OBJ_LIST_SIZE+21) / 4;
    byteWrite = CALC_OAM_TABLE2_BYTE(GLBOAL_OBJ_LIST_SIZE+21, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    table2Index = (GLBOAL_OBJ_LIST_SIZE+22) / 4;
    byteWrite = CALC_OAM_TABLE2_BYTE(GLBOAL_OBJ_LIST_SIZE+22, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+20].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+20].CHARNUM = MONUMENT_SPRITE_2;
    properties = 0x32; //Set palette to 5

    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+21].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+21].CHARNUM = MONUMENT_SPRITE_3;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+22].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+22].CHARNUM = MONUMENT_SPRITE_4;   
}


static const char monumentText[][25] = {
    "-- CELESTE MOUNTAIN -- ",
    "THIS MEMORIAL TO THOSE ",
    " PERISHED ON THE CLIMB "
};

void monumentUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    bool isPlayerTouching;
    static bool isTextDisplayed = false;
    static uint8_t curLineCharCount = 0;
    static uint8_t curLineNum = 0;


    isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 8) && 
                       (GLOBAL_PlayerData.objData.pos.x < this->pos.x+32) && 
                       (GLOBAL_PlayerData.objData.pos.y > this->pos.y) && 
                       (GLOBAL_PlayerData.objData.pos.y < this->pos.y+32);


    if (isPlayerTouching) {
        //Print text, but only at 15fps, this is both to emulate the original,
        //but also because I don't have enough spare time to do it at 60 anyway
        if (GLOBAL_FrameCount % 4 > 0) {        
            return;
        }
        char *curLineStr = monumentText[curLineNum];
        uint8_t curLineLength = strlen(curLineStr);
        char outputText[40];
        playSoundEffect(SOUND_EFFECT_TEXT_DISPLAY);
        isTextDisplayed = true;
        if (curLineNum < 3) {
            if (curLineCharCount < curLineLength) {
                curLineCharCount++;
            }
            else {
                curLineNum++;
                curLineCharCount = 0;
            }
            strncpy(outputText, curLineStr, curLineCharCount);
            outputText[curLineCharCount] = '\0';
            drawTextBG1(outputText, 40, 184 + (curLineNum * 8));
        }
    }
    else if (isTextDisplayed) {
        isTextDisplayed = false;
        curLineNum = 0;
        drawTextBG1("                      ", 40, 184);
        drawTextBG1("                      ", 40, 192);
        drawTextBG1("                      ", 40, 200);        
    }

    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = this->pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = this->pos.y - GLOBAL_ScrollBG2Y;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+20].OBJX = this->pos.x + 16;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+20].OBJY = this->pos.y - GLOBAL_ScrollBG2Y;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+21].OBJX = this->pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+21].OBJY = this->pos.y + 16 - GLOBAL_ScrollBG2Y;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+22].OBJX = this->pos.x + 16;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+22].OBJY = this->pos.y + 16 - GLOBAL_ScrollBG2Y;
}

enum eBigChestSprite {BIG_CHEST_SPRITE_1 = 0x8C, BIG_CHEST_SPRITE_2 = 0x8E};
void bigChestInit(uint8_t index) {
    uint8_t properties = 0;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    table2Index = (GLBOAL_OBJ_LIST_SIZE+10) / 4;
    byteWrite = CALC_OAM_TABLE2_BYTE(GLBOAL_OBJ_LIST_SIZE+10, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    properties |= 0x30; //Set priority to 3
    properties |= 0x08; //Set palette to 4
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = BIG_CHEST_SPRITE_1;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].CHARNUM = BIG_CHEST_SPRITE_2;
}

enum eBigChestState {BIG_CHEST_STATE_IDLE = 0, BIG_CHEST_STATE_OPEN_ANIM = 1, BIG_CHEST_STATE_OPENED = 2};


void bigChestUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    static uint16_t curColour = 0;
    bool isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 8) && 
                            (GLOBAL_PlayerData.objData.pos.x < this->pos.x+32) && 
                            (GLOBAL_PlayerData.objData.pos.y > this->pos.y) && 
                            (GLOBAL_PlayerData.objData.pos.y < this->pos.y+32);
    switch (this->data.bigChest.state) {
        case BIG_CHEST_STATE_IDLE:
            GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = this->pos.x;
            GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = this->pos.y - GLOBAL_ScrollBG2Y;
            GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].OBJX = this->pos.x + 16;
            GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].OBJY = this->pos.y - GLOBAL_ScrollBG2Y;
            if (isPlayerTouching) {
                //Stop music
                playSoundEffect(SOUND_EFFECT_BIG_CHEST);
                initObject(OBJ_SMOKE,this->pos.x,this->pos.y);
                initObject(OBJ_SMOKE,this->pos.x+16,this->pos.y);
                GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
                GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].OBJY = 240;
                this->data.bigChest.state = BIG_CHEST_STATE_OPEN_ANIM;
                this->data.bigChest.frameCount = 0;
                curColour = 0;
                GLOBAL_ShakeFrames = 120;
                GLOBAL_PausePlayerFrames = 120;
            }
            break;
        case BIG_CHEST_STATE_OPEN_ANIM:
            this->data.bigChest.frameCount += 1;
            curColour += 171;
            GLOBAL_BG1Pal[0] = curColour;
            GLOBAL_BG1Pal[1] = curColour >> 8;            
            if (this->data.bigChest.frameCount > 120) {
                GLOBAL_BG1Pal[0] = 0x90;
                GLOBAL_BG1Pal[1] = 0x28;
                this->data.bigChest.state = BIG_CHEST_STATE_OPENED;
            }
            break;
        case BIG_CHEST_STATE_OPENED:
            GLOBAL_SwapCloudPal = true;
            initObject(OBJ_DOUBLE_JUMP_ORB,this->pos.x+8,this->pos.y+16);
            GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
            GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].OBJY = 240;
            this->eType = OBJ_UNUSED;
            return;
            break;
    }
}

enum eDoubleJumpOrbSprite {DOUBLE_JUMP_ORB_SPRITE_1 = 0xA0};
void doubleDashOrbInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t properties = 0;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    properties |= 0x30; //Set priority to 3
    properties |= 0x08; //Set palette to 4
    properties |= 0x00; //Set flipX to 0
    properties |= 0x00; //Set flipY to 0
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = DOUBLE_JUMP_ORB_SPRITE_1;
    this->data.doubleJumpOrb.frameCount = 0;
}

void doubleDashOrbUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    //Speed starts at -8 and goes down by 0.5 every frame
    //alternatively 1 every 2 frames
    static int16_t speedY = -4;  // -2 * 2 (initial speed)
    static int16_t accelAccumulator = 0;

    // Every frame:
    accelAccumulator += 1;  // 0.5 * 2
    if (accelAccumulator >= 4) {  // 2 * 2
        speedY += accelAccumulator / 4;
        accelAccumulator %= 4;
    }



    if (speedY >= 0) {
        bool isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 16) && 
                                (GLOBAL_PlayerData.objData.pos.x < this->pos.x + 16) && 
                                (GLOBAL_PlayerData.objData.pos.y > this->pos.y - 16) && 
                                (GLOBAL_PlayerData.objData.pos.y < this->pos.y+16);
        if (isPlayerTouching) {
            GLOBAL_FreezeFrames = 20;
            GLOBAL_DoubleDashUnlocked = true;
            GLOBAL_PlayerData.dashesLeft = 2;
            GLOBAL_PlayerData.doubleDashUnlocked = true;
            GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = 240;
            this->eType = OBJ_UNUSED;
            return;
        }
    }
    else {
        this->pos.y += speedY;
    }

    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = this->pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = this->pos.y - GLOBAL_ScrollBG2Y;
}

void strawberryInit(uint8_t index) {
    enum eStrawberrySprite {STRAWBERRY_SPRITE_1 = 0x4E};
    uint8_t properties = 0;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OBJList[index].data.strawberry.startY = GLOBAL_OBJList[index].pos.y;
    GLOBAL_OBJList[index].data.strawberry.frameCount = 0;
    GLOBAL_OBJList[index].data.strawberry.isCollected = false;

    properties |= 0x30; //Set priority to 3
    properties |= 0x02; //Set palette to 2
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = STRAWBERRY_SPRITE_1;
}

#define TEXT_X_Y_TO_BUFFER_OFFSET(x,y) ((y*32) + x)

void drawTextBG1(const unsigned char *text,uint8_t x, uint8_t y) {
    uint16_t bufferOffset = TEXT_X_Y_TO_BUFFER_OFFSET(x/8,y/8);
    uint8_t i;
    for (i = 0; i < (strlen(text)); i++) {
        GLBOAL_M0BG1TileMap[bufferOffset+i] = text[i];
    }
    GLOBAL_RELOADBG1VRAM = true;
}


void decoTreeInit(uint8_t index) {
    enum eDecoTreeSprite {DECO_TREE_SPRITE_1 = 0x6E};
    uint8_t properties = 0;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    properties |= 0x30; //Set priority to 3
    properties |= 0x02; //Set palette to 2
    properties |= 0x00; //Set flipX to 0
    properties |= 0x00; //Set flipY to 0
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = DECO_TREE_SPRITE_1;
}
void decoTreeUpdate(uint8_t index) {
    //Drawing code
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = GLOBAL_OBJList[index].pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = GLOBAL_OBJList[index].pos.y - GLOBAL_ScrollBG2Y;
}

void strawberryUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    //Pre computed sine table
    static const int8_t y_positions[40] = {
        0,   1,   2,   2,   3,   4,   4,   4,   5,   5,
        5,   5,   5,   4,   4,   4,   3,   2,   2,   1,
        0,  -1,  -2,  -2,  -3,  -4,  -4,  -4,  -5,  -5,
        -5,  -5,  -5,  -4,  -4,  -4,  -3,  -2,  -2,  -1
    };

    uint8_t thisX = this->pos.x;
    uint8_t thisY = this->pos.y;
    uint8_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint8_t playerY = GLOBAL_PlayerData.objData.pos.y;
    bool isPlayerTouching = (playerX > thisX-16) && 
                            (playerX < thisX+16) && 
                            (playerY > thisY-10) && 
                            (playerY < thisY+16);

    if (this->data.strawberry.isCollected) {
        //Do the text display animation
        this->data.strawberry.frameCount += 1;
        if ((this->data.strawberry.frameCount % 2) == 0) {
            //White
            GLOBAL_BG1Pal[2] = 0xFF;
            GLOBAL_BG1Pal[3] = 0x7F;
        }
        else {
            GLOBAL_BG1Pal[2] = 0x1F;
            GLOBAL_BG1Pal[3] = 0x00;
        }
        if ((this->data.strawberry.frameCount % 4) == 0) {
            GLOBAL_ScrollBG1Y += 1;
        }

        if (this->data.strawberry.frameCount > (30*2)) {
            drawTextBG1("    ", this->data.strawberry.bgTextX, this->data.strawberry.bgTextY);
            this->eType = OBJ_UNUSED;
            return;
        }
        //Draw code for this state
        return;
    }

    if (isPlayerTouching) {
        uint8_t remainderX = this->pos.x%8;
        uint8_t remainderY = this->pos.y%8;

        GLOBAL_PlayerData.dashesLeft = GLOBAL_PlayerData.doubleDashUnlocked ? 2 : 1;
        playSoundEffect(SOUND_EFFECT_STRAWBERRY);        
        //got_fruit[level_index()] = true;
        this->data.strawberry.isCollected = true;
        this->data.strawberry.frameCount = 0;
        this->pos.x -= 2;
        this->pos.y -= 4;        
        //Draw the text for the next state
        this->data.strawberry.bgTextX = this->pos.x;
        this->data.strawberry.bgTextY = this->pos.y;
        drawTextBG1("1000", this->data.strawberry.bgTextX, this->data.strawberry.bgTextY);
        GLOBAL_ScrollBG1X = 8-remainderX;
        GLOBAL_ScrollBG1Y = 8-remainderY;
        GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = this->pos.y = 240;
        return;
    }
    this->data.strawberry.frameCount += 1;
    if (this->data.strawberry.frameCount >= 40) {
        this->data.strawberry.frameCount = 0;
    }
    this->pos.y = this->data.strawberry.startY + y_positions[this->data.strawberry.frameCount];

    //Drawing code
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = this->pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = this->pos.y - GLOBAL_ScrollBG2Y;
}

enum eFlyingBerrySprite {FLYING_BERRY_SPRITE_1 = 0x60, WING_SPRITE_1 = 0x68,
                         WING_SPRITE_2 = 0x6A, WING_SPRITE_3 = 0x6C};

void flyingBerryInit(uint8_t index) {
    uint8_t properties = 0;
    uint8_t propertiesWingLeft = 0;
    uint8_t propertiesWingRight = 0;
    uint8_t table2Index = index / 4; //4 entries per byte
    uint8_t byteWrite = CALC_OAM_TABLE2_BYTE(index, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OBJList[index].data.strawberry.frameCount = 0;
    GLOBAL_OBJList[index].data.strawberry.isCollected = false;
    GLOBAL_OBJList[index].data.strawberry.startY = GLOBAL_OBJList[index].pos.y;

    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    //Wings
    table2Index = (GLBOAL_OBJ_LIST_SIZE+10) / 4;
    byteWrite = CALC_OAM_TABLE2_BYTE(GLBOAL_OBJ_LIST_SIZE+10, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    table2Index = (GLBOAL_OBJ_LIST_SIZE+11) / 4;
    byteWrite = CALC_OAM_TABLE2_BYTE(GLBOAL_OBJ_LIST_SIZE+11, 1, 0, GLOBAL_OAMCopy.arr.OAMTable2[table2Index]);
    GLOBAL_OAMCopy.arr.OAMTable2[table2Index] = byteWrite;
    properties |= 0x30; //Set priority to 3
    properties |= 0x02; //Set palette to 0
    properties |= 0x00; //Set flipX to 0
    properties |= 0x00; //Set flipY to 0
    GLOBAL_OAMCopy.arr.OAMArray[index].PROPERTIES = properties;
    GLOBAL_OAMCopy.arr.OAMArray[index].CHARNUM = FLYING_BERRY_SPRITE_1;
    //Coop some slots for the wings
    propertiesWingLeft |= 0x30; //Set priority to 3
    propertiesWingLeft |= 0x04; //Set palette to 1
    propertiesWingRight |= 0x30; //Set priority to 3
    propertiesWingRight |= 0x04; //Set palette to 1
    propertiesWingRight |= 0x40; //Set flipX to 1
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].PROPERTIES = propertiesWingLeft;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].CHARNUM = WING_SPRITE_1;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+11].PROPERTIES = propertiesWingRight;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+11].CHARNUM = WING_SPRITE_1;


}

void flyingBerryUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    static const int8_t y_positions[40] = {
        0,   1,   2,   2,   3,   4,   4,   4,   5,   5,
        5,   5,   5,   4,   4,   4,   3,   2,   2,   1,
        0,  -1,  -2,  -2,  -3,  -4,  -4,  -4,  -5,  -5,
        -5,  -5,  -5,  -4,  -4,  -4,  -3,  -2,  -2,  -1
    };

    uint8_t thisX = this->pos.x;
    uint8_t thisY = this->pos.y;
    uint8_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint8_t playerY = GLOBAL_PlayerData.objData.pos.y;
    bool isPlayerTouching = (playerX > thisX-16) && 
                            (playerX < thisX+16) && 
                            (playerY > thisY) && 
                            (playerY < thisY+16);

    if (this->data.strawberry.isCollected) {
        //Do the text display animation
        this->data.strawberry.frameCount += 1;
        if ((this->data.strawberry.frameCount % 2) == 0) {
            //White
            GLOBAL_BG1Pal[2] = 0xFF;
            GLOBAL_BG1Pal[3] = 0x7F;
        }
        else {
            GLOBAL_BG1Pal[2] = 0x1F;
            GLOBAL_BG1Pal[3] = 0x00;
        }
        if ((this->data.strawberry.frameCount % 4) == 0) {
            GLOBAL_ScrollBG1Y += 1;
        }

        if (this->data.strawberry.frameCount > (30*2)) {
            drawTextBG1("    ", this->data.strawberry.bgTextX, this->data.strawberry.bgTextY);
            this->eType = OBJ_UNUSED;
            return;
        }
        //Draw code for this state
        return;
    }


    if (isPlayerTouching) {
        uint8_t remainderX = this->pos.x%8;
        uint8_t remainderY = this->pos.y%8;

        GLOBAL_PlayerData.dashesLeft = GLOBAL_PlayerData.doubleDashUnlocked ? 2 : 1;
        playSoundEffect(SOUND_EFFECT_STRAWBERRY);        
        //got_fruit[level_index()] = true;
        this->data.strawberry.isCollected = true;
        this->data.strawberry.frameCount = 0;
        this->pos.x -= 2;
        this->pos.y -= 4;        
        //Draw the text for the next state
        this->data.strawberry.bgTextX = this->pos.x;
        this->data.strawberry.bgTextY = this->pos.y;
        drawTextBG1("1000", this->data.strawberry.bgTextX, this->data.strawberry.bgTextY);
        GLOBAL_ScrollBG1X = 8-remainderX;
        GLOBAL_ScrollBG1Y = 8-remainderY;
        GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = this->pos.y = 240;
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].OBJY = this->pos.y = 240;
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+11].OBJY = this->pos.y = 240;
        return;
    }

    this->pos.y = this->data.strawberry.startY + y_positions[this->data.strawberry.frameCount];
    this->data.strawberry.frameCount += 1;
    if (this->data.strawberry.frameCount >= 40) {
        this->data.strawberry.frameCount = 0;
    }

    //Drawing code
    if (this->pos.y < this->data.strawberry.startY) {
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].CHARNUM = WING_SPRITE_1;
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+11].CHARNUM = WING_SPRITE_1;
    }
    else  if (this->pos.y > this->data.strawberry.startY) {
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].CHARNUM = WING_SPRITE_3;
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+11].CHARNUM = WING_SPRITE_3;
    }
    else {
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].CHARNUM = WING_SPRITE_2;
        GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+11].CHARNUM = WING_SPRITE_2;
    }

    GLOBAL_OAMCopy.arr.OAMArray[index].OBJX = GLOBAL_OBJList[index].pos.x;
    GLOBAL_OAMCopy.arr.OAMArray[index].OBJY = GLOBAL_OBJList[index].pos.y - GLOBAL_ScrollBG2Y;
    //Wings
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].OBJX = GLOBAL_OBJList[index].pos.x + 14;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+10].OBJY = GLOBAL_OBJList[index].pos.y - 4 - GLOBAL_ScrollBG2Y;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+11].OBJX = GLOBAL_OBJList[index].pos.x - 14;
    GLOBAL_OAMCopy.arr.OAMArray[GLBOAL_OBJ_LIST_SIZE+11].OBJY = GLOBAL_OBJList[index].pos.y - 4 - GLOBAL_ScrollBG2Y;
}

// Returns the index of the object in the list
void initObject(enum eOBJType eType, int16_t x, int16_t y) {
    // Find a free slot
    uint8_t i;
    //Starts from 1 to account for the fact that hardcoded player is using slot 0
    for (i = 1; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_UNUSED) {
            GLOBAL_OBJList[i].eType = eType;
            GLOBAL_OBJList[i].pos.x = x;
            GLOBAL_OBJList[i].pos.y = y;
            switch (eType)
            {
            case OBJ_SMOKE:
                smokeInit(i);
                break;
            case OBJ_BREAKABLE_WALL:
                breakableWallInit(i);
                break;
            case OBJ_DECO_FLOWER:
                flowerInit(i);
                break;
            case OBJ_STRAWBERRY:
                strawberryInit(i);
                break;
            case OBJ_DECO_TREE:
                decoTreeInit(i);
                break;
            case OBJ_SPRING:
                springInit(i);
                break;
            case OBJ_FLYING_BERRY:
                flyingBerryInit(i);
                break;
            case OBJ_COLLAPSE_TILE:
                collapseTileInit(i);
                break;
            case OBJ_BALLOON:
                balloonInit(i);
                break;
            case OBJ_PLATMOV_L:
            case OBJ_PLATMOV_R:
                platMovInit(i);
                break;
            case OBJ_KEY:
                keyInit(i);
                break;
            case OBJ_CHEST:
                chestInit(i);
                break;
            case OBJ_MONUMENT:
                monumentInit(i);
                break;
            case OBJ_BIG_CHEST:
                bigChestInit(i);
                break;
            case OBJ_DOUBLE_JUMP_ORB:
                doubleDashOrbInit(i);
                break;            
            default:
                GLOBAL_OBJList[i].eType = OBJ_UNUSED;
                break;
            }
            return;
        }
    }
}


void updateAllObjects(void) {
    uint8_t i;
    for (i = 0; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType != OBJ_UNUSED) {
            switch (GLOBAL_OBJList[i].eType)
            {
            case OBJ_SMOKE:
                smokeUpdate(i);
                break;
            case OBJ_BREAKABLE_WALL:
                breakableWallUpdate(i);
                break;
            case OBJ_DECO_FLOWER:
                flowerUpdate(i);
                break;
            case OBJ_STRAWBERRY:
                strawberryUpdate(i);
                break;
            case OBJ_DECO_TREE:
                decoTreeUpdate(i);
                break;
            case OBJ_SPRING:
                springUpdate(i);
                break;
            case OBJ_FLYING_BERRY:
                flyingBerryUpdate(i);
                break;
            case OBJ_COLLAPSE_TILE:
                collapseTileUpdate(i);
                break;
            case OBJ_BALLOON:
                balloonUpdate(i);
                break;
            case OBJ_PLATMOV_L:
            case OBJ_PLATMOV_R:
                platMovUpdate(i);
                break;
            case OBJ_KEY:
                keyUpdate(i);
                break;
            case OBJ_CHEST:
                chestUpdate(i);
                break;
            case OBJ_MONUMENT:
                monumentUpdate(i);
                break;
            case OBJ_BIG_CHEST:
                bigChestUpdate(i);
                break;
            case OBJ_DOUBLE_JUMP_ORB:
                doubleDashOrbUpdate(i);
                break;
            default:
                break;
            }
        }
    }
    return;
}


void playerInit(struct sPlayerData* this);
void onVblank(void);

void LoadRoomDataSwitchCase(uint16_t roomID) {
    uint8_t *collisionFlags;
    uint16_t i;
    switch (roomID) {
        case 1:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level1_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level1_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level1;
            collisionFlags = (uint8_t *)collision_level1;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level1[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level1[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL1_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level1;
            break;
        case 2:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level2_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level2_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level2;
            collisionFlags = (uint8_t *)collision_level2;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level2[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level2[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL2_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level2;
            break;
        case 3:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level3_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level3_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level3;
            collisionFlags = (uint8_t *)collision_level3;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level3[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level3[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL3_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level3;
            break;
        case 4:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level4_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level4_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level4;
            collisionFlags = (uint8_t *)collision_level4;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level4[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level4[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL4_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level4;
            break;
        case 5:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level5_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level5_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level5;
            collisionFlags = (uint8_t *)collision_level5;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level5[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level5[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL5_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level5;
            break;
        case 6:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level6_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level6_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level6;
            collisionFlags = (uint8_t *)collision_level6;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level6[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level6[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL6_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level6;
            break;
        case 7:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level7_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level7_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level7;
            collisionFlags = (uint8_t *)collision_level7;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level7[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level7[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL7_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level7;
            break;
        case 8:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level8_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level8_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level8;
            collisionFlags = (uint8_t *)collision_level8;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level8[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level8[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL8_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level8;
            break;
        case 9:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level9_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level9_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level9;
            collisionFlags = (uint8_t *)collision_level9;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level9[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level9[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL9_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level9;
            break;
        case 10:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level10_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level10_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level10;
            collisionFlags = (uint8_t *)collision_level10;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level10[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level10[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL10_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level10;
            break;
        case 11:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level11_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level11_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level11;
            collisionFlags = (uint8_t *)collision_level11;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level11[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level11[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL11_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level11;
            break;
        case 12:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level12_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level12_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level12;
            collisionFlags = (uint8_t *)collision_level12;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level12[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level12[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL12_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level12;
            break;
        case 13:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level13_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level13_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level13;
            collisionFlags = (uint8_t *)collision_level13;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level13[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level13[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL13_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level13;
            break;
        case 14:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level14_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level14_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level14;
            collisionFlags = (uint8_t *)collision_level14;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level14[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level14[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL14_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level14;
            break;
        case 15:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level15_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level15_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level15;
            collisionFlags = (uint8_t *)collision_level15;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level15[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level15[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL15_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level15;
            break;
        case 16:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level16_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level16_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level16;
            collisionFlags = (uint8_t *)collision_level16;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level16[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level16[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL16_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level16;
            break;
        case 17:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level17_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level17_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level17;
            collisionFlags = (uint8_t *)collision_level17;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level17[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level17[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL17_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level17;
            break;
        case 18:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level18_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level18_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level18;
            collisionFlags = (uint8_t *)collision_level18;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level18[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level18[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL18_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level18;
            break;
        case 19:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level19_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level19_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level19;
            collisionFlags = (uint8_t *)collision_level19;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level19[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level19[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL19_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level19;
            break;
        case 20:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level20_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level20_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level20;
            collisionFlags = (uint8_t *)collision_level20;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level20[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level20[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL20_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level20;
            break;
        case 21:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level21_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level21_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level21;
            collisionFlags = (uint8_t *)collision_level21;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level21[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level21[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL21_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level21;
            break;
        case 22:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level22_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level22_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level22;
            collisionFlags = (uint8_t *)collision_level22;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level22[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level22[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL22_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level22;
            break;
        case 23:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level23_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level23_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level23;
            collisionFlags = (uint8_t *)collision_level23;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level23[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level23[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL23_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level23;
            break;
        case 24:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level24_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level24_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level24;
            collisionFlags = (uint8_t *)collision_level24;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level24[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level24[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL24_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level24;
            break;
        case 25:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level25_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level25_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level25;
            collisionFlags = (uint8_t *)collision_level25;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level25[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level25[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL25_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level25;
            break;
        case 26:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level26_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level26_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level26;
            collisionFlags = (uint8_t *)collision_level26;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level26[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level26[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL26_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level26;
            break;
        case 27:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level27_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level27_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level27;
            collisionFlags = (uint8_t *)collision_level27;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level27[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level27[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL27_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level27;
            break;
        case 28:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level28_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level28_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level28;
            collisionFlags = (uint8_t *)collision_level28;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level28[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level28[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL28_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level28;
            break;
        case 29:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level29_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level29_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level29;
            collisionFlags = (uint8_t *)collision_level29;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level29[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level29[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL29_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level29;
            break;
        case 30:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level30_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level30_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level30;
            collisionFlags = (uint8_t *)collision_level30;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level30[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level30[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL30_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level30;
            break;
        case 31:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level31_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level31_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level31;
            collisionFlags = (uint8_t *)collision_level31;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level31[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level31[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL31_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level31;
            break;
        case 32:
            GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level32_bg2;
            GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level32_bg3;
            GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level32;
            collisionFlags = (uint8_t *)collision_level32;

            GLOBAL_ActiveLevel.playerSpawnX = spawn_level32[0];
            GLOBAL_ActiveLevel.playerSpawnY = spawn_level32[1];
            GLOBAL_ActiveLevel.objectCount = OBJECT_LEVEL32_COUNT;
            GLOBAL_ActiveLevel.objectData = (uint8_t *)object_level32;
            break;
        default:
            break;
    }    
    GLOBAL_ActiveLevel.scrollPointY = 72;
    GLOBAL_ActiveLevel.collisionFlagsPTR = collisionFlags;
}

void LoadRoomData(uint16_t roomID) {
    uint8_t i = 0;
    
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.isLevelLoadedVRAM = false;
    LoadRoomDataSwitchCase(roomID);

    playerInit(&GLOBAL_PlayerData);

}

void LoadRoomDataVRAM(void) {
    LoadVram(GLOBAL_ActiveLevel.tilemapBg2,0x2000, GLOBAL_ActiveLevel.roomSizeX * GLOBAL_ActiveLevel.roomSizeY * 4,6); //Bg2 tilemap
    LoadVram(GLOBAL_ActiveLevel.tilemapBg3,0x4000, GLOBAL_ActiveLevel.roomSizeX * GLOBAL_ActiveLevel.roomSizeY * 4,6); //Bg2 tilemap
    LoadCGRam(GLOBAL_ActiveLevel.paletteBg,0x0020, 0x40,0); //Bg2 palette
    LoadCGRam(GLOBAL_ActiveLevel.paletteBg,0x0040, 0x40,1); //Bg3 palette
}

void LoadNextRoom(void) {
    GLOBAL_GameState.currentRoomID++;
    LoadRoomData(GLOBAL_GameState.currentRoomID);
}

void main(void){
	//Variables
	static int something = 5;
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers
	uint8_t *test_heap;
	uint32_t counter = 40000;
	uint32_t i;
	uint32_t j;
    uint8_t regWrite1;

	initSNES(SLOWROM);
	initOAMCopy(GLOBAL_OAMCopy.Bytes);


    REG_CGADD = 0;
    REG_CGDATA = 0x00;
    REG_CGDATA = 0x00;

	REG_BGMODE  = 0x00;
    {
    //Use BG1 for text
    LoadLoVram(SNESFONT_bin,0xE000, sizeof(SNESFONT_bin),2);
    for (i = 0; i < sizeof(GLBOAL_M0BG1TileMap); i++) {
        GLBOAL_M0BG1TileMap[i] = 0x20;
    }
    //{0x00,0x1F,0x1F,0x00};
    GLOBAL_BG1Pal[0] = 0x00;
    GLOBAL_BG1Pal[1] = 0x00;
    GLOBAL_BG1Pal[2] = 0xff;
    GLOBAL_BG1Pal[3] = 0x7f;

	LoadCGRam(GLOBAL_BG1Pal, 0x0000, sizeof(GLOBAL_BG1Pal), 0); // Load BG Palette Data
    LoadLoVram(GLBOAL_M0BG1TileMap,0xF800, sizeof(GLBOAL_M0BG1TileMap),0);
    
    REG_BG1SC  = 0xFC;
	REG_BG1HOFS = 0x00;
	REG_BG1HOFS = 0x00;
	REG_BG1VOFS = 0x00;
	REG_BG1VOFS = 0x00;
    }

    //Send the tile data to 0xC000 
    LoadVram(clouds_tiles,0xC000, sizeof(clouds_tiles),0);
    LoadCGRam((char *)clouds_palette,0x0060, sizeof(clouds_palette),1);
    //tilemaps must be in the first half of VRAM, Tilemap will go in VRAM at 0x0000
    LoadVram((char *)clouds_map,0x0000, sizeof(clouds_map),2);

    //Send the sprite data to 0x6000
    LoadVram(sprite_gfx_4bpp,0x8000, sizeof(sprite_gfx_4bpp),3);
    LoadVram(sprite_gfx_2bpp,0xA000, sizeof(sprite_gfx_2bpp),4);
    //Player palette
    LoadCGRam((char *)sprite_palettes_4bpp[0],0x0080, sizeof(sprite_palettes_4bpp[0]),5);
    //Smoke palette
    LoadCGRam((char *)sprite_palettes_4bpp[0],0x0090, sizeof(sprite_palettes_4bpp[0]),5);
    //Breakable wall palette
    LoadCGRam((char *)sprite_palettes_4bpp[1],0x00A0, sizeof(sprite_palettes_4bpp[0]),5);
    //Flower palette
    LoadCGRam((char *)sprite_palettes_4bpp[2],0x00B0, sizeof(sprite_palettes_4bpp[0]),5);
    //Straberry palette
    LoadCGRam((char *)sprite_palettes_4bpp[3],0x00C0, sizeof(sprite_palettes_4bpp[0]),5);
    //Deco tree palette
    LoadCGRam((char *)sprite_palettes_4bpp[4],0x00D0, sizeof(sprite_palettes_4bpp[0]),5);
    //Spring palette
    LoadCGRam((char *)sprite_palettes_4bpp[5],0x00E0, sizeof(sprite_palettes_4bpp[0]),5);
    //Flying berry palette
    LoadCGRam((char *)sprite_palettes_4bpp[6],0x00F0, sizeof(sprite_palettes_4bpp[0]),5);
 
    regWrite1 = 0x00; //8x8 or 16x16
    regWrite1 = regWrite1 | VRAM_ADD_TO_OBSEL_VALUE(0x8000); 
    REG_OBSEL = regWrite1; 



    GLOBAL_OAMCopy.Names.OBJ000X = 0;
    GLOBAL_OAMCopy.Names.OBJ000Y = 0x00;
    GLOBAL_OAMCopy.Names.CHARNUM000 = 0x00;
    GLOBAL_OAMCopy.Names.OAMTABLE2BYTE00 = 0x56; //Enable the first sprite, set size to 16x16

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

    //Player is hardcoded to slot 0 for now
    //Setup game state
    GLOBAL_GameState.currentRoomID = 7; //6  test for balloon, 8, 7 for stress test
    GLOBAL_GameState.currentRoomID = 1; //12 for monument 20, 22 for big chest
    LoadRoomData(GLOBAL_GameState.currentRoomID); //Test room
    LoadRoomDataVRAM();

    //REG_NMITIMEN = 0x81;

    REG_INIDISP = 0x0F;

    for (;;) {
        int8_t regRead1 = 0;
        do{ //Wait for Vblank
            regRead1 = REG_RDNMI;
        } while(regRead1 > 0);
        onVblank();

    }
}


fixed_t approachFixed(fixed_t current, fixed_t target, fixed_t amount){
    fixed_t diff = FIXED_SUB(target, current);
    if (diff > amount) {
        return FIXED_ADD(current, amount);
    }
    if (diff < -amount) {
        return FIXED_SUB(current, amount);
    }
    return target;
}


void playerInit(struct sPlayerData* this){
    uint16_t i;
    this->objData.eType = OBJ_PLAYER;
    this->objData.pos.x = GLOBAL_ActiveLevel.playerSpawnX * 16;
    this->objData.pos.y = GLOBAL_ActiveLevel.playerSpawnY * 16 - 1;

    this->posF.x = INT_TO_FIXED(this->objData.pos.x);
    this->posF.y = INT_TO_FIXED(this->objData.pos.y);

    this->movingPlatformIndex = -1;

    this->spd.x = FLOAT_TO_FIXED(0.0);
    this->spd.y = FLOAT_TO_FIXED(0.0);

    this->eSriteState = PLAYER_SPRITE_IDLE;
    this->isFliped = false;

    this->graceTimer = 0;

    this->doubleDashUnlocked = GLOBAL_DoubleDashUnlocked;
    this->dashesLeft = this->doubleDashUnlocked ? 2 : 1;
    this->dashCounter = 0;
    

    //Reset some global flags, maybe move this to a death function
    //Reset the collision flags
    for (i = 0; i < 256; i++) {
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = GLOBAL_ActiveLevel.collisionFlagsPTR[i];
    }
    GLOBAL_ActiveLevel.movingPlatformCount = 0;


    //Clear out the object array
    for (i = 1; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;        
    }
    //Clear all sprites
    for (i = 0; i < 128; i++) {
        GLOBAL_OAMCopy.arr.OAMArray[i].OBJY = 240;
    }
    for (i = 0; i < (GLOBAL_ActiveLevel.objectCount*3); i+=3) {
        if (GLOBAL_ActiveLevel.objectData[i] != 0) {
            //Player spawn, remove from generated code in future
            if (GLOBAL_ActiveLevel.objectData[i] == 1) {
                continue;
            }
            initObject(GLOBAL_ActiveLevel.objectData[i], GLOBAL_ActiveLevel.objectData[i+1]*16, GLOBAL_ActiveLevel.objectData[i+2]*16-1);
        }
    }

}

bool isDeathAtPoint(int16_t x, int16_t y, int16_t w, int16_t h, int16_t xspd, int16_t yspd) {
    int i, j;
    int collisionIndex;
    
    // For floor spikes, only check if player is moving down and touching the bottom edge
    if (yspd >= 0) {
        int bottomY = y + h - 1;
        int tileY = bottomY / 16;
        int localY = bottomY % 16;
        
        // Only check if player's bottom edge is in the spike area (bottom 4 pixels)
        if (localY >= 10) {
            for (i = (int)((x + 4) / 16); i <= (int)((x + w - 5) / 16); i++) {
                if (i < 0 || i >= 16 || tileY < 0 || tileY >= 16) {
                    continue;
                }
                collisionIndex = tileY * 16 + i;
                if (GLOBAL_ActiveLevel.collisionFlagsArr[collisionIndex] & 0x04) {
                    return true;
                }
            }
        }
    }
    
    // For ceiling spikes, only check if player is moving up and touching the top edge
    if (yspd <= 0) {
        int tileY = y / 16;
        int localY = y % 16;
        
        // Only check if player's top edge is in the spike area (top 8 pixels - over half of 16)
        if (localY <= 2) {
            for (i = (int)((x + 4) / 16); i <= (int)((x + w + 5) / 16); i++) {
                if (i < 0 || i >= 16 || tileY < 0 || tileY >= 16) {
                    continue;
                }
                collisionIndex = tileY * 16 + i;
                if (GLOBAL_ActiveLevel.collisionFlagsArr[collisionIndex] & 0x08) {
                    return true;
                }
            }
        }
    }
    
    // For left wall spikes, only check if player is moving left and touching the left edge
    if (xspd <= 0) {
        int tileX = x / 16;
        int localX = x % 16;
        
        // Only check if player's left edge is in the spike area (left 4 pixels)
        if (localX <= 4) {
            for (j = (int)(y / 16); j <= (int)((y + h - 1) / 16); j++) {
                if (tileX < 0 || tileX >= 16 || j < 0 || j >= 16) {
                    continue;
                }
                collisionIndex = j * 16 + tileX;
                if (GLOBAL_ActiveLevel.collisionFlagsArr[collisionIndex] & 0x10) {
                    return true;
                }
            }
        }
    }
    
    // For right wall spikes, only check if player is moving right and touching the right edge
    if (xspd >= 0) {
        int rightX = x + w - 6;
        int tileX = rightX / 16;
        int localX = rightX % 16;
        
        // Only check if player's right edge is in the spike area (right 4 pixels)
        if (localX >= 10) {
            for (j = (int)(y / 16); j <= (int)((y + h - 1) / 16); j++) {
                if (tileX < 0 || tileX >= 16 || j < 0 || j >= 16) {
                    continue;
                }
                collisionIndex = j * 16 + tileX;
                if (GLOBAL_ActiveLevel.collisionFlagsArr[collisionIndex] & 0x20) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

static bool OBJ_isDeathAt(struct sPlayerData* this, int16_t xOffset, int16_t yOffset) {
    int16_t x = this->objData.pos.x + xOffset;
    int16_t y = this->objData.pos.y + yOffset;
    int16_t w = 16; // Player width
    int16_t h = 16; // Player height
    
    // Check tile collision using the object bounds and current speed
    return isDeathAtPoint(x, y, w, h, FIXED_TO_INT(this->spd.x), FIXED_TO_INT(this->spd.y));
}



static bool isTileSolidAtPoint(int16_t x, int16_t y) {
    int tileX;
    int tileY;
    int collisionIndex;
    uint8_t i;

    // Convert world coordinates to tile coordinates
    tileX = (int)(x / 16);
    tileY = (int)(y / 16);
    
    // Check bounds for tile coordinates
    if (tileX < 0 || tileX >= 16 || tileY < 0 || tileY >= 16) {
        return false;
    }

    // Get collision flag from collision_test array
    // Each row is 16 tiles, so multiply Y by 16 and add X
    collisionIndex = tileY * 16 + tileX;
    //Bit 1 contains the collision flag
    return GLOBAL_ActiveLevel.collisionFlagsArr[collisionIndex] & 0x01;
}

static bool OBJ_isSolidAt(struct sPlayerData* this, int16_t xOffset, int16_t yOffset) {
    int16_t x = this->objData.pos.x; 
    int16_t y = this->objData.pos.y;
    int16_t checkX;
    int16_t checkY;
    uint8_t i;
    
    // Calculate check positions based on offset
    checkX = x + xOffset;
    checkY = y + yOffset;


    if (GLOBAL_PlayerData.movingPlatformIndex >= 0) {
        uint8_t i = GLOBAL_PlayerData.movingPlatformIndex;
        if (checkX >= GLOBAL_ActiveLevel.movingPlatformHitboxes[i*4] && checkX <= GLOBAL_ActiveLevel.movingPlatformHitboxes[i*4+2] &&
            checkY >= GLOBAL_ActiveLevel.movingPlatformHitboxes[i*4+1] && checkY <= GLOBAL_ActiveLevel.movingPlatformHitboxes[i*4+3]) {
                return true;
        }
    }
    
    // Check tile collision
    if (isTileSolidAtPoint(checkX, checkY + 8) ||
        isTileSolidAtPoint(checkX + 14, checkY + 8) ||
        isTileSolidAtPoint(checkX, checkY + 16) ||
        isTileSolidAtPoint(checkX + 14, checkY + 16)) {
        return true;
    }
    
    
    return false;
}

#define FPS60_SCALE_FACTOR 1.0f
void playerUpdate(struct sPlayerData* this) {
    int16_t curX = this->objData.pos.x;
    int16_t curY = this->objData.pos.y;
    int16_t spdXstepInt;
    int16_t spdYStepInt;

    int8_t inputX = 0;
    int8_t inputY = 0;

    fixed_t accel = FLOAT_TO_FIXED(0.6f * FPS60_SCALE_FACTOR); // Adjusted for 60fps vs 30fps
    fixed_t deccel = FLOAT_TO_FIXED(0.15f * FPS60_SCALE_FACTOR); // Adjusted for 60fps vs 30fps
    fixed_t maxrun;
    fixed_t maxfall;
    fixed_t gravity;
    bool  onGround = false;
    bool  onWall = false;
    bool  btnJump = false;
    bool  btnDash = false;
    int8_t i;

    if (!GLOBAL_ActiveLevel.isLevelLoadedVRAM) {
        //Prevents double room transitions
        return;
    }

    //Input handling
    if(GLOBAL_InputHi & JOY_LEFT_MASK){
        inputX = -1;
        this->isFliped = true;
    }
    if(GLOBAL_InputHi & JOY_RIGHT_MASK){
        inputX = 1;
        this->isFliped = false;
    }
    if(GLOBAL_InputHi & JOY_UP_MASK){
        inputY = -1;
    }
    if(GLOBAL_InputHi & JOY_DOWN_MASK){
        inputY = 1;
    }

    //Button handling with proper edge detection
    {
    static bool btnJumpLastFrame = false;
    static bool btnDashLastFrame = false;

    if(GLOBAL_InputHi & JOY_B_MASK) {
        if (!btnJumpLastFrame) {
            btnJump = true;
        }
        btnJumpLastFrame = true;
    } else {
        btnJumpLastFrame = false;
    }

    if(GLOBAL_InputHi & JOY_Y_MASK) {
        if (!btnDashLastFrame) {
            btnDash = true;
        }
        btnDashLastFrame = true;
    } else {
        btnDashLastFrame = false;
    }
    }

    //Updates on every frame
    if(this->graceTimer > 0){
        this->graceTimer--;
    }
    onGround=OBJ_isSolidAt(this, 0, 1);

    //Smoke particles when landing
    static bool wasOnGround = false;
    if (onGround && !wasOnGround) {
        initObject(OBJ_SMOKE, this->objData.pos.x, this->objData.pos.y + 4);
    }
    wasOnGround = onGround;

    if (onGround) {
        this->graceTimer=6*2;
        if (this->dashesLeft< (this->doubleDashUnlocked ? 2 : 1)) {
            playSoundEffect(SOUND_EFFECT_DASH_RESTORED);
            this->dashesLeft=this->doubleDashUnlocked ? 2 : 1;
        }
    }

    //Dash effect time
    static int8_t dashEffectTime = 0;
    if (dashEffectTime > 0) {
        dashEffectTime--;
    }

    //Move code
    if (this->dashCounter > 0){
        //Override input
        this->dashCounter--;
        initObject(OBJ_SMOKE,this->objData.pos.x,this->objData.pos.y);
        this->spd.x = approachFixed(this->spd.x, this->dashTarget.x, this->dashAccel.x);
        this->spd.y = approachFixed(this->spd.y, this->dashTarget.y, this->dashAccel.y);
    }
    else {
        maxrun = FLOAT_TO_FIXED(1.0f * 2);

        //Acceleration adjustments
        if (!onGround) {
            accel = FLOAT_TO_FIXED(0.4f * FPS60_SCALE_FACTOR); // Adjusted for 60fps vs 30fps
        }
        //Note: Ice handling would go here but not implemented in current code

        //Gravity
        maxfall = FLOAT_TO_FIXED(2.0f * 2);
        gravity = FLOAT_TO_FIXED(0.21f * FPS60_SCALE_FACTOR); // Adjusted for 60fps vs 30fps

        if (FIXED_ABS(this->spd.y) <= FLOAT_TO_FIXED(0.30f)) {
            gravity = FIXED_MUL(gravity, FLOAT_TO_FIXED(0.5f));
        }

        // wall slide
        if (inputX!=0 && OBJ_isSolidAt(this, inputX,0)) {
            maxfall=FLOAT_TO_FIXED(0.4f * 2);
            onWall = true;
            if (randint16(0,10)<2) {
                initObject(OBJ_SMOKE,this->objData.pos.x+inputX*6*2,this->objData.pos.y);
            }
        }

        //Gravity
        if (onGround) {
            this->spd.y = FLOAT_TO_FIXED(0.0f);
            this->graceTimer = 10;
        }
        else {
            //Compensation for 60 fps, slowing it down more breaks other things
            //Like a spring would push you too high into death when before it did not
            if (this->spd.y > 0) {
                gravity = FIXED_MUL(gravity, FLOAT_TO_FIXED(0.75f));
            }
            this->spd.y = approachFixed(this->spd.y, maxfall, gravity); 
        }

        //Jump handling with buffer
        static int8_t jumpBuffer = 0;
        if (btnJump) {
            jumpBuffer = 4;
        } else if (jumpBuffer > 0) {
            jumpBuffer--;
        }

        if (jumpBuffer > 0) {
            if (this->graceTimer > 0) {
                //Normal jump
                playSoundEffect(SOUND_EFFECT_JUMP);
                jumpBuffer = 0;
                this->graceTimer = 0;
                this->spd.y = INT_TO_FIXED(-2*2);
                initObject(OBJ_SMOKE,this->objData.pos.x,this->objData.pos.y+4);
            } else {
                //Wall jump
                int8_t wall_dir=(OBJ_isSolidAt(this, -3,0) ? -1 : (OBJ_isSolidAt(this, 3,0) ? 1 : 0));
                if (wall_dir!=0) {
                    playSoundEffect(SOUND_EFFECT_JUMP);
                    jumpBuffer = 0;
                    this->spd.y = INT_TO_FIXED(-2*2);
                    this->spd.x = FIXED_MUL(INT_TO_FIXED(-wall_dir), FIXED_ADD(maxrun, INT_TO_FIXED(1*2)));
                    initObject(OBJ_SMOKE,this->objData.pos.x+wall_dir*6*2,this->objData.pos.y);
                }
            }
        }

        //Dash handling
        if (btnDash && this->dashesLeft > 0) {
            //Dash code
            #define DASH_FULL_SPEED (FLOAT_TO_FIXED(5.0f*2))
            #define DASH_HALF_SPEED (FIXED_MUL(DASH_FULL_SPEED, FLOAT_TO_FIXED(0.70710678118f)))

            VEC_I vInput;

            initObject(OBJ_SMOKE,this->objData.pos.x,this->objData.pos.y);
            this->dashesLeft-=1;
            this->dashCounter=4; //Original was 4, not 8
            dashEffectTime = 10;

            vInput.x = inputX;
            vInput.y = inputY;
            //No direction pressed
            if ((inputX == 0) && (inputY == 0)) {
                vInput.x = this->isFliped ? -1 : 1;
            }

            if (vInput.x != 0) {
                if (vInput.y != 0) {
                    this->spd.x = FIXED_MUL(INT_TO_FIXED(vInput.x), DASH_HALF_SPEED);
                    this->spd.y = FIXED_MUL(INT_TO_FIXED(vInput.y), DASH_HALF_SPEED);
                } else {
                    this->spd.x = FIXED_MUL(INT_TO_FIXED(vInput.x), DASH_FULL_SPEED);
                    this->spd.y = INT_TO_FIXED(0);
                }
            } else if (vInput.y != 0) {
                this->spd.x = INT_TO_FIXED(0);
                this->spd.y = FIXED_MUL(INT_TO_FIXED(vInput.y), DASH_FULL_SPEED);
            }

            playSoundEffect(SOUND_EFFECT_DASH_START);
            GLOBAL_FreezeFrames = 2 * 2;
            GLOBAL_ShakeFrames = 6 * 2;
            this->dashTarget.x=FIXED_MUL(INT_TO_FIXED(2*2), INT_TO_FIXED(vInput.x));
            this->dashTarget.y=FIXED_MUL(INT_TO_FIXED(2*2), INT_TO_FIXED(vInput.y));
            this->dashAccel.x=FLOAT_TO_FIXED(1.5f*2);
            this->dashAccel.y=FLOAT_TO_FIXED(1.5f*2);

            if (this->spd.y<0) {
                this->dashTarget.y=FIXED_MUL(this->dashTarget.y, FLOAT_TO_FIXED(0.75f+0.15f));
            }

            //0.1f is a small adjustment to make the dash feel more natural
            if (this->spd.y!=0) {
                this->dashAccel.y=FIXED_MUL(this->dashAccel.y, FLOAT_TO_FIXED(0.70710678118f+0.1f)); 
            }
            if (this->spd.x!=0) {
                this->dashAccel.x=FIXED_MUL(this->dashAccel.x, FLOAT_TO_FIXED(0.70710678118f+0.1f));
            }
        } else if (btnDash && (this->dashesLeft == 0)) {
            playSoundEffect(SOUND_EFFECT_DASH_MISFIRE);
            initObject(OBJ_SMOKE,this->objData.pos.x,this->objData.pos.y);
        }

        //End of normal move code
        if (FIXED_ABS(this->spd.x) > maxrun) {
            this->spd.x = approachFixed(this->spd.x, FIXED_SIGN(this->spd.x) * maxrun, deccel);
        } else {
            this->spd.x = approachFixed(this->spd.x, inputX*maxrun, accel);
        }

        //Facing direction
        if (this->spd.x != 0) {
            this->isFliped = (this->spd.x < 0);
        }
    }

    //Part of updating every object
    // Clip speed to prevent collision with solid objects
    {
    int8_t step;
    uint8_t i;
    bool playerOnMovingPlatform = false;

    spdXstepInt = FIXED_TO_INT(FIXED_ADD(this->posF.x, this->spd.x)) - this->objData.pos.x;
    spdYStepInt = FIXED_TO_INT(FIXED_ADD(this->posF.y, this->spd.y)) - this->objData.pos.y;

    //Handle moving platforms    
    this->movingPlatformIndex = -1;
    if ((GLOBAL_PlayerData.spd.y >= 0)) {
        uint8_t i;
        uint16_t checkX = this->objData.pos.x;
        uint16_t checkY = this->objData.pos.y + 1;
        for (i = 0; i < GLOBAL_ActiveLevel.movingPlatformCount; i++) {
            // Check each step of the Y movement
            for (int16_t stepY = 0; stepY <= spdYStepInt; stepY++) {
                uint16_t currentCheckY = checkY + stepY;
                if (checkX >= GLOBAL_ActiveLevel.movingPlatformHitboxes[i*4] && checkX <= GLOBAL_ActiveLevel.movingPlatformHitboxes[i*4+2] &&
                    currentCheckY >= GLOBAL_ActiveLevel.movingPlatformHitboxes[i*4+1] && currentCheckY <= GLOBAL_ActiveLevel.movingPlatformHitboxes[i*4+3]) {                
                    playerOnMovingPlatform = true;
                    this->movingPlatformIndex = i;
                    if (inputX == 0) { //Only when the player isn't moving
                        if (GLOBAL_ActiveLevel.movingPlatformDir[i] == MOVING_PLATFORM_DIR_LEFT) {
                            this->spd.x = FLOAT_TO_FIXED(-0.65f);
                        }
                        else {
                            this->spd.x = FLOAT_TO_FIXED(0.65f);
                        }
                    }
                    spdYStepInt = stepY;
                    break; // Found a platform, no need to check further steps
                }
            }
            if (playerOnMovingPlatform) break; // Found a platform, no need to check other platforms
        }
    }

    // 1. Accumulate speed into posF
    this->posF.x = FIXED_ADD(this->posF.x, this->spd.x);
    this->posF.y = FIXED_ADD(this->posF.y, this->spd.y);

    // 2. Calculate integer movement delta
    spdXstepInt = FIXED_TO_INT(this->posF.x) - this->objData.pos.x;
    spdYStepInt = FIXED_TO_INT(this->posF.y) - this->objData.pos.y;

    // 3. Do collision/movement using deltaX/deltaY (update objData.pos.x/y as needed)
    //Move X
    if (spdXstepInt != 0) {
        if (OBJ_isSolidAt(this, spdXstepInt, 0)) {
            step = (spdXstepInt < 0) ? -1 : 1;
            int16_t absStep = (spdXstepInt < 0) ? -spdXstepInt : spdXstepInt;
            for (i = 0; i < absStep; i++) {
                if (!OBJ_isSolidAt(this, step, 0)) {
                    this->objData.pos.x += step;
                } else {
                    spdXstepInt = 0;
                    break;
                }
            }
        } else {
            this->objData.pos.x += spdXstepInt;
        }
    }

    //Move Y
    if ((this->movingPlatformIndex >= 0) && (spdYStepInt > 0)) {
        spdYStepInt = 1;
    }
    
    if (spdYStepInt != 0) {
        if (OBJ_isSolidAt(this, 0, spdYStepInt)) {
            step = (spdYStepInt < 0) ? -1 : 1;
            int16_t absStep = (spdYStepInt < 0) ? -spdYStepInt : spdYStepInt;
            for (i = 0; i < absStep; i++) {
                if (!OBJ_isSolidAt(this, 0, step)) {
                    this->objData.pos.y += step;
                } else {
                    break;
                }
            }
        } else {
            this->objData.pos.y += spdYStepInt;
        }
    }

    // 4. At the end, update posF to match the new integer position, preserving the fractional part
    //Clamp pos.x and pos.y to be between 0 and 255
    this->objData.pos.x = FIXED_CLAMP(this->objData.pos.x, 0, 256-16);
    this->objData.pos.y = FIXED_CLAMP(this->objData.pos.y, -15, 255);
    this->posF.x = FIXED_ADD(INT_TO_FIXED(this->objData.pos.x), FIXED_SUB(this->posF.x, INT_TO_FIXED(FIXED_TO_INT(this->posF.x))));
    this->posF.y = FIXED_ADD(INT_TO_FIXED(this->objData.pos.y), FIXED_SUB(this->posF.y, INT_TO_FIXED(FIXED_TO_INT(this->posF.y))));

    // Death logic
    if (OBJ_isDeathAt(this, 0, 0) || this->objData.pos.y >= (255)) {
        playSoundEffect(SOUND_EFFECT_DEATH);
        this->objData.pos.y = 240; //offscreen
        GLOBAL_ShakeFrames = 10 * 2;        
        playerInit(this);
        return;
    }

    }

    // next level
    if (this->objData.pos.y <= -14 && GLOBAL_GameState.currentRoomID<31) { 
        LoadNextRoom();
        playerInit(this);
     }

    //Animation
    if (!onGround) {
        if (onWall) {
            this->eSriteState = PLAYER_SPRITE_WALL;
        } else {
            this->eSriteState = PLAYER_SPRITE_WALK_2;
        }
    }
    else if (inputY < 0) {
        this->eSriteState = PLAYER_SPRITE_UP;
    } else if (inputY > 0) {
        this->eSriteState = PLAYER_SPRITE_DOWN;
    } else if (inputX==0) {
        this->eSriteState = PLAYER_SPRITE_IDLE;
    } else {
        this->eSriteState = PLAYER_SPRITE_WALK_1 + (((GLOBAL_FrameCount % 8) >> 2) << 1);
    }
}


static uint32_t global_randSeed = 0xDEADBEEE; // You can set this to time(NULL) for more randomness
// Table-based random number generator
static const uint16_t rand_table[256] = {
    0x6f6a, 0xba79, 0xc063, 0x76ee, 0x61ba, 0xbd9f, 0xdab5, 0x340e,
    0xa160, 0x864f, 0x2447, 0x3e2b, 0xe9fd, 0x12e2, 0x683b, 0x9d53,
    0x20a7, 0x20f2, 0xfc0d, 0xe967, 0x9e1b, 0xf48b, 0x8e7a, 0x7102,
    0xef3c, 0x1f38, 0x5272, 0x40e1, 0x33c2, 0x76aa, 0xa07e, 0xfe26,
    0xabb6, 0xa8b9, 0xc06a, 0x4e66, 0x8767, 0x63c4, 0x300a, 0x94cd,
    0xad96, 0x747a, 0xb324, 0x6c00, 0x7da9, 0xd729, 0xb4d1, 0x11f2,
    0xed7e, 0x9468, 0x992f, 0x02ac, 0xc938, 0x428e, 0x3dbf, 0xffe3,
    0xc733, 0xa313, 0x61de, 0x3986, 0x6e99, 0xbd22, 0xac99, 0xea69,
    0x9dab, 0x47b2, 0x704a, 0x4589, 0x28b9, 0x7fe0, 0xd2e0, 0x5bb7,
    0x495b, 0xed0b, 0x4b1b, 0xbcaa, 0x4562, 0x303f, 0x8284, 0xd2a0,
    0x2fcc, 0x2650, 0xddf3, 0xaabd, 0x2b95, 0xc044, 0x6c32, 0x9402,
    0xf8f6, 0xb08c, 0x73a7, 0x8b1e, 0x154d, 0x0aea, 0x226f, 0xf2da,
    0x38ca, 0xb4a8, 0x5666, 0x93a1, 0x1e94, 0xb8b3, 0xc984, 0xb6ef,
    0xe81f, 0xa593, 0x7aef, 0x01e6, 0xf4ac, 0x6e3a, 0x05d0, 0x025a,
    0xdc26, 0x2293, 0xc419, 0xcfc8, 0xe293, 0xf83e, 0x7891, 0x18e1,
    0x4325, 0x1ed4, 0x3146, 0xd955, 0xd563, 0xfb98, 0x0806, 0xd6d1,
    0x0fcd, 0xb766, 0xf38a, 0x8e0e, 0x3a2b, 0xbf33, 0xcf9c, 0xd5a2,
    0x72c0, 0x9df2, 0x51a4, 0x3eb8, 0x12d7, 0xa0f0, 0x58c5, 0x0a7d,
    0xdd7d, 0x5235, 0x943c, 0x366e, 0xf01c, 0xbfe4, 0x2e7d, 0x29e2,
    0x5886, 0x7bac, 0x8a96, 0x3615, 0x28b6, 0xf85d, 0xf5b3, 0x0dc3,
    0xfaad, 0x38bf, 0x92c6, 0x12b8, 0x72c5, 0x9bfd, 0xccd8, 0x89e9,
    0xbfe7, 0x9248, 0x1f8a, 0xdac2, 0x4f67, 0xd5a4, 0x7585, 0x3bb3,
    0x2b7c, 0xfb36, 0x97e1, 0x4b87, 0x59ac, 0x609e, 0x202d, 0xbc63,
    0xa257, 0xd010, 0x6ee1, 0xf632, 0xc0df, 0x2df7, 0x7cbe, 0x2b43,
    0x061d, 0xda66, 0x603b, 0x1037, 0xd571, 0x0e00, 0x6e6e, 0xd6d6,
    0x5542, 0x954f, 0xd232, 0xcfe3, 0x6632, 0xd11c, 0xc0bc, 0x38a2,
    0x7de3, 0x66a0, 0x550d, 0xd60f, 0x5c98, 0x810e, 0x1ea4, 0x272c,
    0xb64c, 0x3aec, 0x173f, 0xf8bd, 0x7036, 0x56cd, 0x0ec6, 0x992e,
    0x86c0, 0x430b, 0x3090, 0x4914, 0x7dfa, 0x820d, 0x93e8, 0xce3a,
    0xb08f, 0xbfb5, 0x6147, 0xfe8f, 0x9f69, 0x4a1f, 0x7c82, 0x2298,
    0x3c1d, 0x55e8, 0x186e, 0xc1fa, 0xee00, 0xdf50, 0x18b7, 0xe356,
    0xbab2, 0x0162, 0xa5bc, 0xcaf5, 0xa970, 0x1e1a, 0xf436, 0x28d9,
};

// Table-based random number generator
uint16_t my_rand() {
    global_randSeed = (global_randSeed + 1) % 256;
    return rand_table[global_randSeed];
}

// Returns a random integer in [min, max]
int16_t randint16(int16_t min, int16_t max) {
    uint16_t range;
    uint16_t randomValue;
    if (max <= min) return min; // handle edge case
    range = (uint16_t)(max - min) + 1;
    randomValue = my_rand();
    return (int16_t)((randomValue % range) + min);
}


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


#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))


void onVblank(void) {
    int8_t regRead1;
    //Update hardware registers
    if (!GLOBAL_ActiveLevel.isLevelLoadedVRAM) {
        LoadRoomDataVRAM();
        GLOBAL_ActiveLevel.isLevelLoadedVRAM = true;
        //Loading new level takes too long so skip an extra frame
        return;
    } 
    LoadCGRam(GLOBAL_BG1Pal, 0x0000, sizeof(GLOBAL_BG1Pal), 0); // Load BG Palette Data
    if (GLOBAL_SwapCloudPal) {
        LoadCGRam((char *)clouds_palette_2,0x0060, sizeof(clouds_palette_2),1);
        GLOBAL_SwapCloudPal = false;
    }


    if (GLOBAL_RELOADBG1VRAM) {
        LoadLoVram(GLBOAL_M0BG1TileMap,0xF800, sizeof(GLBOAL_M0BG1TileMap),0);
        GLOBAL_RELOADBG1VRAM = false;
    }

    //Display FPS
    if ((GLOBAL_FrameCount % (60*4)) == 0) {
        static uint16_t lastVBlankAmount = 0;
        static uint16_t lastFrameCount = 0;
        lastVBlankAmount = GLOBAL_FrameCountVBLANK;
        lastFrameCount = GLOBAL_FrameCount;
        GLOBAL_FrameCountVBLANK = 0;
        GLOBAL_FrameCount = 0;
        //sprintf(testStringRam, "FPS: %02d", (uint16_t)((lastFrameCount * 60) / lastVBlankAmount));
        drawTextBG1(testStringRam, 0, 0);
    }

    //Update player hair colour
    {
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
    LoadCGRam((char *)&hairColour,0x00C5, sizeof(hairColour),1);
    }
    
    
    LoadOAMCopy((char *)GLOBAL_OAMCopy.Bytes,0x0000,sizeof(union uOAMCopy), 0);

    //Update background scrolls
    REG_BG4HOFS = GLOBAL_ScrollBG4X;
    REG_BG4HOFS = GLOBAL_ScrollBG4X >> 8;    
    REG_BG4VOFS = GLOBAL_ScrollBG4Y;
    REG_BG4VOFS = (GLOBAL_ScrollBG4Y >> 8);


    REG_BG1HOFS = GLOBAL_ScrollBG1X;
    REG_BG1HOFS = GLOBAL_ScrollBG1X >> 8;
    REG_BG1VOFS = GLOBAL_ScrollBG1Y;
    REG_BG1VOFS = GLOBAL_ScrollBG1Y >> 8;

    REG_BG2HOFS = GLOBAL_ScrollBG2X;
    REG_BG2HOFS = GLOBAL_ScrollBG2X >> 8;
    REG_BG2VOFS = GLOBAL_ScrollBG2Y;
    REG_BG2VOFS = GLOBAL_ScrollBG2Y >> 8;

    REG_BG3HOFS = GLOBAL_ScrollBG3X;
    REG_BG3HOFS = GLOBAL_ScrollBG3X >> 8;
    REG_BG3VOFS = GLOBAL_ScrollBG3Y;
    REG_BG3VOFS = GLOBAL_ScrollBG3Y >> 8;

    REG_BG4VOFS = GLOBAL_ScrollBG4Y;
    REG_BG4VOFS = GLOBAL_ScrollBG4Y >> 8;

    //Player Draw
    GLOBAL_OAMCopy.Names.OBJ000X = GLOBAL_PlayerData.objData.pos.x;
    GLOBAL_OAMCopy.Names.OBJ000Y = GLOBAL_PlayerData.objData.pos.y - GLOBAL_ScrollBG2Y;
    GLOBAL_OAMCopy.Names.CHARNUM000 = GLOBAL_PlayerData.eSriteState;
    GLOBAL_OAMCopy.Names.PROPERTIES000 = GLOBAL_PlayerData.isFliped ? (0x38+0x40) : (0x38);

    do{ //Wait for reg read
        regRead1 = REG_HVBJOY;
    } while(regRead1 & 0x01);

    GLOBAL_InputLo = REG_JOY1L;
    GLOBAL_InputHi = REG_JOY1H;
    
    //Calculate next frame (globals)
    GLOBAL_FrameCount += 1;
    if (GLOBAL_FreezeFrames > 0) {
        GLOBAL_FreezeFrames--;
        return;
    }
    if (GLOBAL_ShakeFrames > 0) {
        GLOBAL_ShakeFrames--;
    }

    //Update all active objects

    //Calculate hardware scrolls, do so before objects as they may rely on these values being correct
    {
    int16_t shakeAmount = GLOBAL_ShakeFrames > 0 ? ((GLOBAL_FrameCount & 1) ? 2 : -2) : 0;
    int16_t smoothScrollY = ((int16_t)GLOBAL_PlayerData.objData.pos.y - GLOBAL_ActiveLevel.scrollPointY) >> 2;

    GLOBAL_ScrollBG2Y = CLAMP(GLOBAL_PlayerData.objData.pos.y - 16 - GLOBAL_ActiveLevel.scrollPointY, 0, 31);
    GLOBAL_ScrollBG2X = 0;
    
    GLOBAL_ScrollBG3X = GLOBAL_ScrollBG2X + (shakeAmount);
    GLOBAL_ScrollBG3Y = GLOBAL_ScrollBG2Y + (shakeAmount);

    GLOBAL_ScrollBG4Y =  smoothScrollY + (shakeAmount >> 1) - (GLOBAL_GameState.currentRoomID << 6);
    }

    updateAllObjects();
    playerUpdate(&GLOBAL_PlayerData);
    onUpdateBG4CloudsEffect();

}

//Interupt handler for VBlank
void interuptVBlank(void){
    GLOBAL_FrameCountVBLANK++;
}


#ifdef __VBCC__
/*
__near __interrupt void __irq_vblank(void) {
    interuptVBlank();
}
*/
#endif

#ifdef __WDC816CC__
void far IRQHandler(void){
    interuptVBlank();
}
#endif


