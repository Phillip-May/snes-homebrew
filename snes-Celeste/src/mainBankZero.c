//LoROM mememory map
#include "fixedPointSNES.h"

#include <inttypes.h>
//#include <STDINT.H> // STDINT is cursed on wdc816cc

#include <STDIO.H>
#include <STRING.H>
#include <STDARG.h>
#include <stdbool.h>
#include <limits.h>

#include "../shared/src/snes_regs_xc.h"
#include "../shared/src/initsnes.h"
#include "port/sprite_animation_enums.h"
#include "port/port.h"

// 60fps vs 30fps physics scaling factor

//Prototypes
int16_t randint16(int16_t min, int16_t max);

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



uint8_t GLOBAL_InputState = 0;

#define GLBOAL_OBJ_LIST_SIZE 30
OBJ_DATA GLOBAL_OBJList[GLBOAL_OBJ_LIST_SIZE] = {0};

uint16_t GLOBAL_FrameCountVBLANK = 0;
uint16_t GLOBAL_FrameCount = 0;

uint8_t GLOBAL_FreezeFrames = 0;

uint8_t GLOBAL_PausePlayerFrames = 0;
//Game state globals
bool GLOBAL_DoubleDashUnlocked = false;


enum eMovingPlatformDir {MOVING_PLATFORM_DIR_IDLE = 0, MOVING_PLATFORM_DIR_LEFT = 1, MOVING_PLATFORM_DIR_RIGHT = 2};

struct sActiveLevelData GLOBAL_ActiveLevel;

struct sPlayerData GLOBAL_PlayerData;

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
    {
        uint8_t properties = 0x30u; // priority 3
        properties |= 0x04u; // palette 2
        properties |= ((uint8_t)GLOBAL_OBJList[index].data.smoke.flipX) << 6;
        properties |= ((uint8_t)GLOBAL_OBJList[index].data.smoke.flipY) << 7;
        GLOBAL_OBJList[index].oamTile = SMOKE_SPRITE_1;
        GLOBAL_OBJList[index].oamProps = properties;
        GLOBAL_OBJList[index].flags |= OBJ_FLAG_DIRTY;
    }
}

void smokeUpdate(uint8_t index) {
    OBJ_DATA *smoke = &GLOBAL_OBJList[index];
    smoke->data.smoke.frameCount++;
    // 1/3 of it's time per state
    smoke->data.smoke.smokeSpriteState = SMOKE_SPRITE_1 + ((smoke->data.smoke.frameCount / 10) * 2);
    // 0.5 seconds
    if (smoke->data.smoke.frameCount >= 30) {
        //Destroy self
        smoke->eType = OBJ_UNUSED;
        smoke->flags |= OBJ_FLAG_DIRTY;
        return;
    }

    smoke->oamTile = smoke->data.smoke.smokeSpriteState;
    smoke->flags |= OBJ_FLAG_DIRTY;
}


#define COLLISION_FLAG_INDEX_FROM_TILE_XY(x,y) ((x) + (y) * 16)
void breakableWallInit(uint8_t index) {
    OBJ_DATA *wall = &GLOBAL_OBJList[index];
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
    wall->oamTile = BREAKABLE_WALL_SPRITE_1;
    wall->oamProps = properties;
    wall->flags |= OBJ_FLAG_DIRTY;

}

void breakableWallUpdate(uint8_t index) {
    //Check if player is touching the wall
    OBJ_DATA *wall = &GLOBAL_OBJList[index];
    uint8_t thisX = wall->pos.x;
    uint8_t thisY = wall->pos.y;
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
        wall->eType = OBJ_UNUSED;
        wall->flags |= OBJ_FLAG_DIRTY;
        //Restore the original collision data
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)]     = GLOBAL_ActiveLevel.collisionFlagsReset[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)];
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY)]   = GLOBAL_ActiveLevel.collisionFlagsReset[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY)];
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY+1)]   = GLOBAL_ActiveLevel.collisionFlagsReset[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY+1)];
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY+1)] = GLOBAL_ActiveLevel.collisionFlagsReset[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY+1)];
        return;
    }
    wall->flags |= OBJ_FLAG_DIRTY;
}

static void initSimpleDecorSprite(uint8_t index, uint8_t tile, uint8_t properties) {
    OBJ_DATA *decor = &GLOBAL_OBJList[index];
    decor->oamTile = tile;
    decor->oamProps = properties;
    decor->flags |= OBJ_FLAG_DIRTY;
}

static void updateSimpleDecorSprite(uint8_t index) {
    GLOBAL_OBJList[index].flags |= OBJ_FLAG_DIRTY;
}

void flowerInit(uint8_t index) {
    initSimpleDecorSprite(index, FLOWER_SPRITE_1, 0x32); // priority 3, palette 1
}

void flowerUpdate(uint8_t index) {
    updateSimpleDecorSprite(index);
}

enum eCollapseTileState {
    COLLAPSE_TILE_STATE_IDLE = 0,
    COLLAPSE_TILE_STATE_COLLAPSING = 1,
    COLLAPSE_TILE_STATE_HIDDEN = 2,
};

void collapseTileInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t tileX = GLOBAL_OBJList[index].pos.x / 16;
    uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 16;
    uint8_t i;
    GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)]     |= 0x01; //Set the solid flag
    this->data.collapseTile.state = COLLAPSE_TILE_STATE_IDLE;
    this->data.collapseTile.linkedSpringIndex = -1;

    // Assign fixed OAM slot (4 sprites per 16x16 object)
    // Player uses slots 0-3, objects start at slot 4
    // Each object gets 4 consecutive slots based on its index
    this->extraSpriteBase = 4 + (index * 4);
    this->extraSpriteCount = 4;

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

    this->oamTile = COLLAPSE_TILE_SPRITE_1;
    this->oamProps = 0x32; //Set palette to 7
    this->flags |= OBJ_FLAG_DIRTY;
}

void collapseTileUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint16_t thisX = this->pos.x;
    uint16_t thisY = this->pos.y;
    uint16_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint16_t playerY = GLOBAL_PlayerData.objData.pos.y;
    bool isPlayerTouching = (thisX <= playerX + 20 && thisX + 20 >= playerX && 
                            thisY <= playerY + 20 && thisY + 20 >= playerY);

    this->oamProps = 0x32; //Ensure consistent properties
    this->flags |= OBJ_FLAG_DIRTY;

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
            this->oamTile = COLLAPSE_TILE_SPRITE_1;
            break;
            
        case COLLAPSE_TILE_STATE_COLLAPSING:
            this->data.collapseTile.frameCount -= 1;
            if (this->data.collapseTile.frameCount == 0) {
                uint8_t tileX = GLOBAL_OBJList[index].pos.x / 16;
                uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 16;
                GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)] &= ~0x01; //Unset the solid flag
                this->data.collapseTile.state = COLLAPSE_TILE_STATE_HIDDEN;
                this->data.collapseTile.frameCount = 120;
                this->oamTile = COLLAPSE_TILE_SPRITE_1;
                return;
            }
            // Animation frame: 0->SPRITE_3, 10->SPRITE_2, 20->SPRITE_1
            // On NES: values are consecutive (23, 24, 25), so add 1 per frame
            // On SNES: values are 2 apart (0x48, 0x4A, 0x4C), so add 2 per frame
            uint8_t frame_offset = (uint8_t)(2 - (this->data.collapseTile.frameCount / 10));
#ifdef __NES__
            this->oamTile = (uint8_t)COLLAPSE_TILE_SPRITE_1 + frame_offset;
#else
            this->oamTile = (uint8_t)COLLAPSE_TILE_SPRITE_1 + (frame_offset * 2);
#endif
            break;
            
        case COLLAPSE_TILE_STATE_HIDDEN:
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
}

void springInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->data.spring.frameCount = 0;
    this->data.spring.isDisabled = false;
    this->data.spring.linkedCollapseTileIndex = -1;

    // Assign fixed OAM slot (4 sprites per 16x16 object)
    // Player uses slots 0-3, objects start at slot 4
    // Each object gets 4 consecutive slots based on its index
    this->extraSpriteBase = 4 + (index * 4);
    this->extraSpriteCount = 4;

    this->oamTile = SPRING_SPRITE_1;
    this->oamProps = 0x32; // priority 3, palette 2
    this->flags |= OBJ_FLAG_DIRTY;
}

void springUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    struct sPlayerData *player = &GLOBAL_PlayerData;
    uint8_t thisX = this->pos.x;
    uint8_t thisY = this->pos.y;
    uint8_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint8_t playerY = GLOBAL_PlayerData.objData.pos.y;
    bool isPlayerTouching = false;

    if (this->data.spring.isDisabled) {
        this->data.spring.frameCount = 0;
        this->oamTile = SPRING_SPRITE_1;
        this->oamProps = 0x32;
        this->flags |= OBJ_FLAG_DIRTY;
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
    //Spring animation state machine
    if (this->data.spring.frameCount > 0) {
        this->data.spring.frameCount--;
        this->oamTile = SPRING_SPRITE_2;
        this->oamProps = 0x36; // priority 3, palette 6
    }
    else {
        this->oamTile = SPRING_SPRITE_1;
        this->oamProps = 0x32; // priority 3, palette 2
    }

    this->flags |= OBJ_FLAG_DIRTY;
}

enum eBalloonState {BALLOON_STATE_IDLE = 0, BALLOON_STATE_POPPED = 1};
void balloonInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->data.balloon.state = BALLOON_STATE_IDLE;
    this->data.balloon.frameCount = 0;
    this->data.balloon.yTableIndex = 0;
    this->data.balloon.hideFrameCount = 0;
    this->data.balloon.spriteYOffset = 0;
    this->data.balloon.stringTile = BALLOON_STRING_1;
    this->oamTile = BALLOON_SPRITE_1;
    this->oamProps = 0x36; // priority 3, palette 0
    this->flags |= OBJ_FLAG_DIRTY;
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
        this->data.balloon.hideFrameCount = 60 * 2;
        GLOBAL_PlayerData.dashesLeft = GLOBAL_PlayerData.doubleDashUnlocked ? 2 : 1;
        playSoundEffect(SOUND_EFFECT_BALLOON_POP);
        this->flags |= OBJ_FLAG_DIRTY;
        return;
    }

    if (this->data.balloon.state == BALLOON_STATE_POPPED) {
        if (this->data.balloon.hideFrameCount > 0) {
            this->data.balloon.hideFrameCount -= 1;
            this->flags |= OBJ_FLAG_DIRTY;
            return;
        }
        this->data.balloon.state = BALLOON_STATE_IDLE;
    }

    this->data.balloon.spriteYOffset = yOffset;
    this->data.balloon.stringTile = balloonStringFrames[this->data.balloon.frameCount];
    this->oamTile = BALLOON_SPRITE_1;
    this->oamProps = 0x36; // priority 3, palette 0
    this->flags |= OBJ_FLAG_DIRTY;
}

void platMovInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->pos.y -= 2;
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
    
    this->oamTile = PLATMOV_SPRITE_1;
    this->oamProps = 0x34; // priority 3, palette 2
    //Add the hitbox to the list
    hitboxIndex = GLOBAL_ActiveLevel.movingPlatformCount * 4;
    this->data.platMov.hitboxIndex = hitboxIndex;
    GLOBAL_ActiveLevel.movingPlatformCount++;
    this->data.platMov.acc = 0;
    this->flags |= OBJ_FLAG_DIRTY;
}

void platMovUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t hitboxIndex = this->data.platMov.hitboxIndex;

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


    this->flags |= OBJ_FLAG_DIRTY;
}

enum eKeyState {KEY_STATE_1 = 0, KEY_STATE_2 = 1, KEY_STATE_3 = 2, KEY_STATE_4 = 3};

void keyInit(uint8_t index) {
    uint8_t i;
    OBJ_DATA *this = &GLOBAL_OBJList[index];

    this->data.key.frameCount = 0;
    this->data.key.linkedChestIndex = (uint8_t)-1;
    this->data.key.state = KEY_STATE_1;
    this->data.key.isFlipped = false;
    this->data.key.spriteValue = KEY_SPRITE_1;

    for (i = 0; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_CHEST) {
            this->data.key.linkedChestIndex = i;
        }
    }

    this->oamTile = KEY_SPRITE_1;
    this->oamProps = 0x32; // priority 3, palette 2
    this->flags |= OBJ_FLAG_DIRTY;
}

void keyUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t properties = 0x30; // priority 3 baseline
    bool isPlayerTouching = false;
    this->data.key.frameCount += 1;

    isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 16) &&
                       (GLOBAL_PlayerData.objData.pos.x < this->pos.x + 16) &&
                       (GLOBAL_PlayerData.objData.pos.y > this->pos.y - 18) &&
                       (GLOBAL_PlayerData.objData.pos.y < this->pos.y + 2);

    if (isPlayerTouching) {
        OBJ_DATA *chest = &GLOBAL_OBJList[this->data.key.linkedChestIndex];
        chest->data.chest.keyIsCollected = true;
        this->eType = OBJ_UNUSED;
        this->flags |= OBJ_FLAG_DIRTY;
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

    properties |= this->data.key.isFlipped ? 0x40 : 0x00; //Set flipX
    this->oamProps = properties;
    this->oamTile = this->data.key.spriteValue;
    this->flags |= OBJ_FLAG_DIRTY;
}

enum eChestState {CHEST_STATE_IDLE = 0, CHEST_STATE_SHAKING = 1, CHEST_STATE_OPEN = 2};

void chestInit(uint8_t index) {   
    uint8_t i;
    OBJ_DATA *this = &GLOBAL_OBJList[index];
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

    this->oamTile = CHEST_SPRITE_1;
    this->oamProps = 0x32; // priority 3, palette 2
    this->flags |= OBJ_FLAG_DIRTY;
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
            this->eType = OBJ_UNUSED;
            this->flags |= OBJ_FLAG_DIRTY;
            return;
            break;
        default:
            break;
    }

    this->oamTile = CHEST_SPRITE_1;
    this->oamProps = 0x32; // priority 3, palette 2
    this->flags |= OBJ_FLAG_DIRTY;
}

void monumentInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->extraSpriteCount = 0;
    this->oamTile = MONUMENT_SPRITE_1;
    this->oamProps = 0x36; // priority 3, palette 5
    this->flags |= OBJ_FLAG_DIRTY;
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

    this->flags |= OBJ_FLAG_DIRTY;

    isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 8) && 
                       (GLOBAL_PlayerData.objData.pos.x < this->pos.x+32) && 
                       (GLOBAL_PlayerData.objData.pos.y > this->pos.y) && 
                       (GLOBAL_PlayerData.objData.pos.y < this->pos.y+32);

    if (isPlayerTouching) {
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
            port_drawText((const unsigned char *)outputText, 40, (uint8_t)(184 + (curLineNum * 8)));
        }
    }
    else if (isTextDisplayed) {
        isTextDisplayed = false;
        curLineNum = 0;
        port_drawText((const unsigned char *)"                      ", 40, 184);
        port_drawText((const unsigned char *)"                      ", 40, 192);
        port_drawText((const unsigned char *)"                      ", 40, 200);
    }
}

enum eBigChestState {BIG_CHEST_STATE_IDLE = 0, BIG_CHEST_STATE_OPEN_ANIM = 1, BIG_CHEST_STATE_OPENED = 2};

// eBigChestSprite is defined in sprite_animation_enums.h
void bigChestInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->extraSpriteCount = 0;
    this->data.bigChest.state = BIG_CHEST_STATE_IDLE;
    this->data.bigChest.frameCount = 0;
    this->oamTile = BIG_CHEST_SPRITE_1;
    this->oamProps = 0x38; // priority 3, palette 4
    this->flags |= OBJ_FLAG_DIRTY;
}

void bigChestUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    bool isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 8) && 
                            (GLOBAL_PlayerData.objData.pos.x < this->pos.x+32) && 
                            (GLOBAL_PlayerData.objData.pos.y > this->pos.y) && 
                            (GLOBAL_PlayerData.objData.pos.y < this->pos.y+32);

    this->oamTile = BIG_CHEST_SPRITE_1;
    this->oamProps = 0x38; // priority 3, palette 4
    this->flags |= OBJ_FLAG_DIRTY;

    switch (this->data.bigChest.state) {
        case BIG_CHEST_STATE_IDLE:
            if (isPlayerTouching) {
                playSoundEffect(SOUND_EFFECT_BIG_CHEST);
                initObject(OBJ_SMOKE,this->pos.x,this->pos.y);
                initObject(OBJ_SMOKE,this->pos.x+16,this->pos.y);
                this->data.bigChest.state = BIG_CHEST_STATE_OPEN_ANIM;
                this->data.bigChest.frameCount = 0;
                GLOBAL_ActiveLevel.shakeFrames = 120;
                GLOBAL_PausePlayerFrames = 120;
            }
            break;
        case BIG_CHEST_STATE_OPEN_ANIM:
            this->data.bigChest.frameCount += 1;
            if (this->data.bigChest.frameCount > 120) {
                this->data.bigChest.state = BIG_CHEST_STATE_OPENED;
            }
            break;
        case BIG_CHEST_STATE_OPENED:
            GLOBAL_ActiveLevel.swapCloudPal = true;
            initObject(OBJ_DOUBLE_JUMP_ORB,this->pos.x+8,this->pos.y+16);
            this->eType = OBJ_UNUSED;
            this->flags |= OBJ_FLAG_DIRTY;
            return;
    }
}

void doubleDashOrbInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->extraSpriteCount = 0;
    this->oamTile = DOUBLE_JUMP_ORB_SPRITE_1;
    this->oamProps = 0x38; // priority 3, palette 4
    this->data.doubleJumpOrb.frameCount = 0;
    this->flags |= OBJ_FLAG_DIRTY;
}

void doubleDashOrbUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    //Speed starts at -8 and goes down by 0.5 every frame
    //alternatively 1 every 2 frames
    static int16_t speedY = -4;  // -2 * 2 (initial speed)
    static int16_t accelAccumulator = 0;

    this->oamTile = DOUBLE_JUMP_ORB_SPRITE_1;
    this->oamProps = 0x38; // priority 3, palette 4
    this->flags |= OBJ_FLAG_DIRTY;

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
            GLOBAL_ActiveLevel.swapActivePalette = true;
            this->eType = OBJ_UNUSED;
            this->flags |= OBJ_FLAG_DIRTY;
            return;
        }
    }
    else {
        this->pos.y += speedY;
    }
}

void strawberryInit(uint8_t index) {
    OBJ_DATA *strawberry = &GLOBAL_OBJList[index];

    // Assign fixed OAM slot (4 sprites per 16x16 object)
    // Player uses slots 0-3, objects start at slot 4
    // Each object gets 4 consecutive slots based on its index
    strawberry->extraSpriteBase = 4 + (index * 4);
    strawberry->extraSpriteCount = 4;

    strawberry->data.strawberry.startY = strawberry->pos.y;
    strawberry->data.strawberry.frameCount = 0;
    strawberry->data.strawberry.isCollected = false;
    strawberry->oamTile = STRAWBERRY_SPRITE_1;
    strawberry->oamProps = 0x32; // priority 3, palette 2
    strawberry->flags |= OBJ_FLAG_DIRTY;
}

void decoTreeInit(uint8_t index) {
    initSimpleDecorSprite(index, DECO_TREE_SPRITE_1, 0x32); // priority 3, palette 2
}
void decoTreeUpdate(uint8_t index) {
    updateSimpleDecorSprite(index);
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
        if (this->data.strawberry.frameCount == 0) {
            GLOBAL_ActiveLevel.textFlashActive = true;
            GLOBAL_ActiveLevel.swapActivePalette = true;
        }
        this->data.strawberry.frameCount += 1;
        if ((this->data.strawberry.frameCount % 4) == 0) {
            if (GLOBAL_ActiveLevel.textScrollActive && GLOBAL_ActiveLevel.textScrollOffsetY < 0xFFu) {
                GLOBAL_ActiveLevel.textScrollOffsetY += 1;
            }
        }

        if (this->data.strawberry.frameCount > (30*2)) {
            GLOBAL_ActiveLevel.textFlashActive = false;
            GLOBAL_ActiveLevel.swapActivePalette = true;
            GLOBAL_ActiveLevel.textScrollActive = false;
            GLOBAL_ActiveLevel.textScrollOffsetX = 0;
            GLOBAL_ActiveLevel.textScrollOffsetY = 0;
            port_drawText((const unsigned char *)"    ", this->data.strawberry.bgTextX, this->data.strawberry.bgTextY);
            this->eType = OBJ_UNUSED;
            this->flags |= OBJ_FLAG_DIRTY;
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
        port_drawText((const unsigned char *)"1000", this->data.strawberry.bgTextX, this->data.strawberry.bgTextY);
        GLOBAL_ActiveLevel.textScrollActive = true;
        GLOBAL_ActiveLevel.textScrollOffsetX = (uint8_t)(8u - remainderX);
        GLOBAL_ActiveLevel.textScrollOffsetY = (uint8_t)(8u - remainderY);
        this->pos.y = 240;
        this->flags |= OBJ_FLAG_DIRTY;
        return;
    }
    this->data.strawberry.frameCount += 1;
    if (this->data.strawberry.frameCount >= 40) {
        this->data.strawberry.frameCount = 0;
    }
    this->pos.y = this->data.strawberry.startY + y_positions[this->data.strawberry.frameCount];

    this->flags |= OBJ_FLAG_DIRTY;
}

void flyingBerryInit(uint8_t index) {
    OBJ_DATA *berry = &GLOBAL_OBJList[index];
    berry->data.strawberry.frameCount = 0;
    berry->data.strawberry.isCollected = false;
    berry->data.strawberry.startY = berry->pos.y;
    berry->oamTile = FLYING_BERRY_SPRITE_1;
    berry->oamProps = 0x32; // priority 3, palette 0
    berry->flags |= OBJ_FLAG_DIRTY;
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
        if (this->data.strawberry.frameCount == 0) {
            GLOBAL_ActiveLevel.textFlashActive = true;
            GLOBAL_ActiveLevel.swapActivePalette = true;
        }
        this->data.strawberry.frameCount += 1;
        if ((this->data.strawberry.frameCount % 4) == 0) {
            if (GLOBAL_ActiveLevel.textScrollActive && GLOBAL_ActiveLevel.textScrollOffsetY < 0xFFu) {
                GLOBAL_ActiveLevel.textScrollOffsetY += 1;
            }
        }

        if (this->data.strawberry.frameCount > (30*2)) {
            GLOBAL_ActiveLevel.textFlashActive = false;
            GLOBAL_ActiveLevel.swapActivePalette = true;
            GLOBAL_ActiveLevel.textScrollActive = false;
            GLOBAL_ActiveLevel.textScrollOffsetX = 0;
            GLOBAL_ActiveLevel.textScrollOffsetY = 0;
            port_drawText((const unsigned char *)"    ", this->data.strawberry.bgTextX, this->data.strawberry.bgTextY);
            this->eType = OBJ_UNUSED;
            this->flags |= OBJ_FLAG_DIRTY;
            return;
        }
        //Draw code for this state
        this->flags |= OBJ_FLAG_DIRTY;
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
        port_drawText((const unsigned char *)"1000", this->data.strawberry.bgTextX, this->data.strawberry.bgTextY);
        GLOBAL_ActiveLevel.textScrollActive = true;
        GLOBAL_ActiveLevel.textScrollOffsetX = (uint8_t)(8u - remainderX);
        GLOBAL_ActiveLevel.textScrollOffsetY = (uint8_t)(8u - remainderY);
        this->pos.y = 240;
        this->flags |= OBJ_FLAG_DIRTY;
        return;
    }

    this->pos.y = this->data.strawberry.startY + y_positions[this->data.strawberry.frameCount];
    this->data.strawberry.frameCount += 1;
    if (this->data.strawberry.frameCount >= 40) {
        this->data.strawberry.frameCount = 0;
    }

    this->flags |= OBJ_FLAG_DIRTY;
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
            GLOBAL_OBJList[i].flags = OBJ_FLAG_DIRTY;
            GLOBAL_OBJList[i].oamTile = 0;
            GLOBAL_OBJList[i].oamProps = 0;
            GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
            GLOBAL_OBJList[i].extraSpriteCount = 0;
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
                GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
                GLOBAL_OBJList[i].extraSpriteCount = 0;
                break;
            }
            return;
        }
    }
}


static void clearObjectDirtyFlag(uint8_t index)
{
    GLOBAL_OBJList[index].flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

static void buildSpriteIfDirty(uint8_t index, void (*builder)(uint8_t))
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    if ((obj->flags & OBJ_FLAG_DIRTY) == 0U) {
        return;
    }
    if (index == 0U) {
        clearObjectDirtyFlag(index);
        return;
    }
    if (obj->eType == OBJ_UNUSED) {
        port_buildUnused(index);
        return;
    }
    builder(index);
}

static void processObject(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    switch (obj->eType)
    {
    case OBJ_UNUSED:
        if (obj->flags & OBJ_FLAG_DIRTY) {
            if (index == 0U) {
                clearObjectDirtyFlag(index);
            } else {
                port_buildUnused(index);
            }
        }
        break;
    case OBJ_SMOKE:
        smokeUpdate(index);
        buildSpriteIfDirty(index, port_buildSmoke);
        break;
    case OBJ_BREAKABLE_WALL:
        breakableWallUpdate(index);
        buildSpriteIfDirty(index, port_buildBreakableWall);
        break;
    case OBJ_DECO_FLOWER:
        flowerUpdate(index);
        buildSpriteIfDirty(index, port_buildStaticDecor);
        break;
    case OBJ_STRAWBERRY:
        strawberryUpdate(index);
        buildSpriteIfDirty(index, port_buildStrawberry);
        break;
    case OBJ_DECO_TREE:
        decoTreeUpdate(index);
        buildSpriteIfDirty(index, port_buildStaticDecor);
        break;
    case OBJ_SPRING:
        springUpdate(index);
        buildSpriteIfDirty(index, port_buildSpring);
        break;
    case OBJ_FLYING_BERRY:
        flyingBerryUpdate(index);
        buildSpriteIfDirty(index, port_buildFlyingBerry);
        break;
    case OBJ_COLLAPSE_TILE:
        collapseTileUpdate(index);
        buildSpriteIfDirty(index, port_buildCollapseTile);
        break;
    case OBJ_BALLOON:
        balloonUpdate(index);
        buildSpriteIfDirty(index, port_buildBalloon);
        break;
    case OBJ_PLATMOV_L:
    case OBJ_PLATMOV_R:
        platMovUpdate(index);
        buildSpriteIfDirty(index, port_buildPlatMov);
        break;
    case OBJ_KEY:
        keyUpdate(index);
        buildSpriteIfDirty(index, port_buildKey);
        break;
    case OBJ_CHEST:
        chestUpdate(index);
        buildSpriteIfDirty(index, port_buildChest);
        break;
    case OBJ_MONUMENT:
        monumentUpdate(index);
        buildSpriteIfDirty(index, port_buildMonument);
        break;
    case OBJ_BIG_CHEST:
        bigChestUpdate(index);
        buildSpriteIfDirty(index, port_buildBigChest);
        break;
    case OBJ_DOUBLE_JUMP_ORB:
        doubleDashOrbUpdate(index);
        buildSpriteIfDirty(index, port_buildDoubleDashOrb);
        break;
    default:
        if (obj->flags & OBJ_FLAG_DIRTY) {
            clearObjectDirtyFlag(index);
        }
        break;
    }
}

void updateAllObjects(void) {
    uint8_t i;
    port_beginSpriteBuild(&GLOBAL_PlayerData);
    for (i = 0; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        processObject(i);
    }
    port_finishSpriteBuild();
}


void playerInit(struct sPlayerData* this);
void onVblank(void);



void LoadRoomData(uint16_t roomID) {
    uint8_t i = 0;
    
    GLOBAL_ActiveLevel.currentRoomID = roomID;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.textChanged = false;
    GLOBAL_ActiveLevel.swapCloudPal = false;
    GLOBAL_ActiveLevel.swapActivePalette = true;
    GLOBAL_ActiveLevel.textFlashActive = false;
    port_LoadRoomData(roomID);
    playerInit(&GLOBAL_PlayerData);
    port_updatePlayerSprite(&GLOBAL_PlayerData);
    port_beginSpriteBuild(&GLOBAL_PlayerData);
    for (i = 0; i < GLBOAL_OBJ_LIST_SIZE; ++i) {
        processObject(i);
    }
    port_finishSpriteBuild();

}



void LoadNextRoom(void) {
    GLOBAL_ActiveLevel.currentRoomID++;
    LoadRoomData(GLOBAL_ActiveLevel.currentRoomID);
}

int main(void){
	//Variables
	static int something = 5;
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers
	uint8_t *test_heap;
	uint32_t counter = 40000;
	uint32_t i;
	uint32_t j;
    uint8_t regWrite1;

    port_init();
 
    //Player is hardcoded to slot 0 for now
    //Setup game state
    GLOBAL_ActiveLevel.currentRoomID = 7; //6  test for balloon, 8, 7 for stress test
    GLOBAL_ActiveLevel.currentRoomID = 1; //12 for monument 20, 22 for big chest
    LoadRoomData(GLOBAL_ActiveLevel.currentRoomID); //Test room

    for (;;) { 
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
    this->objData.flags = OBJ_FLAG_DIRTY;
    this->objData.oamTile = PLAYER_SPRITE_IDLE;
    this->objData.oamProps = 0x38;
    this->objData.extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->objData.extraSpriteCount = 0;
    this->objData.flags |= OBJ_FLAG_DIRTY;

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
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = GLOBAL_ActiveLevel.collisionFlagsReset[i];
    }
    GLOBAL_ActiveLevel.movingPlatformCount = 0;


    //Clear out the object array
    for (i = 1; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
        GLOBAL_OBJList[i].extraSpriteCount = 0;
    }

    port_resetSprites();
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
    int16_t prevPosX = this->objData.pos.x;
    int16_t prevPosY = this->objData.pos.y;
    uint8_t prevTile = this->objData.oamTile;
    uint8_t prevProps = this->objData.oamProps;
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
    if(GLOBAL_InputState & PORT_INPUT_LEFT_MASK){
        inputX = -1;
        this->isFliped = true;
    }
    if(GLOBAL_InputState & PORT_INPUT_RIGHT_MASK){
        inputX = 1;
        this->isFliped = false;
    }
    if(GLOBAL_InputState & PORT_INPUT_UP_MASK){
        inputY = -1;
    }
    if(GLOBAL_InputState & PORT_INPUT_DOWN_MASK){
        inputY = 1;
    }

    //Button handling with proper edge detection
    {
    static bool btnJumpLastFrame = false;
    static bool btnDashLastFrame = false;

    if(GLOBAL_InputState & PORT_INPUT_B_MASK) {
        if (!btnJumpLastFrame) {
            btnJump = true;
        }
        btnJumpLastFrame = true;
    } else {
        btnJumpLastFrame = false;
    }

    if(GLOBAL_InputState & PORT_INPUT_Y_MASK) {
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
            GLOBAL_ActiveLevel.shakeFrames = 6 * 2;
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
        int16_t stepY;
        uint16_t checkX = this->objData.pos.x;
        uint16_t checkY = this->objData.pos.y + 1;
        for (i = 0; i < GLOBAL_ActiveLevel.movingPlatformCount; i++) {
            // Check each step of the Y movement
            for (stepY = 0; stepY <= spdYStepInt; stepY++) {
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
        GLOBAL_ActiveLevel.shakeFrames = 10 * 2;        
        playerInit(this);
        return;
    }

    }

    // next level
    if (this->objData.pos.y <= -14 && GLOBAL_ActiveLevel.currentRoomID < 31) { 
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

    this->objData.oamTile = (uint8_t)this->eSriteState;
    this->objData.oamProps = this->isFliped ? (uint8_t)(0x38u | 0x40u) : 0x38u;
    this->objData.flags |= OBJ_FLAG_DIRTY;
}


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
static uint8_t global_randSeed = 0xDEADBEEE; // You can set this to time(NULL) for more randomness
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

void onVblank(void) {
    //Start of vblank critical code
    port_vblank();


    //Display FPS
    /*
    if ((GLOBAL_FrameCount % (60*4)) == 0) {
        static uint16_t lastVBlankAmount = 0;
        static uint16_t lastFrameCount = 0;
        lastVBlankAmount = GLOBAL_FrameCountVBLANK;
        lastFrameCount = GLOBAL_FrameCount;
        GLOBAL_FrameCountVBLANK = 0;
        GLOBAL_FrameCount = 0;
        //sprintf(testStringRam, "FPS: %02d", (uint16_t)((lastFrameCount * 60) / lastVBlankAmount));
        port_drawText((const unsigned char *)testStringRam, 0, 0);
    }
    */
    //End of vblank critical code

    //Calculate next frame (globals)
    GLOBAL_FrameCount += 1;
    if (GLOBAL_FreezeFrames > 0) {
        GLOBAL_FreezeFrames--;
        return;
    }
    if (GLOBAL_ActiveLevel.shakeFrames > 0) {
        GLOBAL_ActiveLevel.shakeFrames--;
    }

    GLOBAL_InputState = port_getInputs();

    updateAllObjects();
    playerUpdate(&GLOBAL_PlayerData);
    port_updatePlayerSprite(&GLOBAL_PlayerData);
}

//Interupt handler for VBlank
void interuptVBlank(void){
    GLOBAL_FrameCountVBLANK++;
}

// Cross-compiler interrupt handlers, must be present
void snesXC_cop(void) {
}

void snesXC_brk(void) {
}

void snesXC_abort(void) {
}

void snesXC_nmi(void) {
    interuptVBlank();
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


