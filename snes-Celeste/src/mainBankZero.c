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
#include "port/port.h"

// 60fps vs 30fps physics scaling factor

//Prototypes
int16_t randint16(int16_t min, int16_t max);

//Basic math functions that a compiler should have
PORT_FUNC_BANK6
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

PORT_FUNC_BANK6
void playSoundEffect(enum eSoundEffect soundEffect){
    // TODO: export SFX from FamiStudio, then:
    // sfx_play((char)soundEffect, 0);
    (void)soundEffect;
}



uint8_t GLOBAL_InputState = 0;

OBJ_DATA GLOBAL_OBJList[GLOBAL_OBJ_LIST_SIZE] = {0};

uint16_t GLOBAL_FrameCount = 0;

uint8_t GLOBAL_FreezeFrames = 0;

uint8_t GLOBAL_PausePlayerFrames = 0;
//Game state globals
bool GLOBAL_DoubleDashUnlocked = false;



enum eMovingPlatformDir {MOVING_PLATFORM_DIR_IDLE = 0, MOVING_PLATFORM_DIR_LEFT = 1, MOVING_PLATFORM_DIR_RIGHT = 2};

struct sActiveLevelData GLOBAL_ActiveLevel;

struct sPlayerData GLOBAL_PlayerData;

void initObject(enum eOBJType eType, int16_t x, int16_t y);

PORT_FUNC_BANK6
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

PORT_FUNC_BANK6
void smokeUpdate(uint8_t index) {
    OBJ_DATA *smoke = &GLOBAL_OBJList[index];
    smoke->data.smoke.frameCount++;
    // 1/3 of it's time per state
    smoke->data.smoke.smokeSpriteState = SMOKE_SPRITE_1 + ((smoke->data.smoke.frameCount / 5) * 2);
    // 0.5 seconds at 30fps
    if (smoke->data.smoke.frameCount >= 15) {
        //Destroy self
        smoke->eType = OBJ_UNUSED;
        smoke->flags |= OBJ_FLAG_DIRTY;
        return;
    }

    smoke->oamTile = smoke->data.smoke.smokeSpriteState;
    smoke->flags |= OBJ_FLAG_DIRTY;
}


#define COLLISION_FLAG_INDEX_FROM_TILE_XY(x,y) ((x) + (y) * 16)
PORT_FUNC_BANK6
void breakableWallInit(uint8_t index) {
    OBJ_DATA *wall = &GLOBAL_OBJList[index];
    uint8_t properties = 0;
    uint8_t tileX = GLOBAL_OBJList[index].pos.x / 8;
    uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 8;
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
    // Update nametable to show the breakable wall tile
    port_updateCollapseTileNametable(index);

}

PORT_FUNC_BANK6
void breakableWallUpdate(uint8_t index) {
    //Check if player is touching the wall
    OBJ_DATA *wall = &GLOBAL_OBJList[index];
    uint8_t thisX = wall->pos.x;
    uint8_t thisY = wall->pos.y;
    uint8_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint8_t playerY = GLOBAL_PlayerData.objData.pos.y;

    //Fixed collision detection to properly detect hits from above
    bool isPlayerTouching = playerX > thisX-4 && playerX < thisX+20 && playerY > thisY-10 && playerY < thisY+20;
    if (isPlayerTouching && (GLOBAL_PlayerData.dashCounter > 0)) {
        uint8_t tileX = GLOBAL_OBJList[index].pos.x / 8;
        uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 8;
        //Player collision with wall
        {
            int16_t knockDir = -sign(FIXED_TO_INT(GLOBAL_PlayerData.spd.x));
            GLOBAL_PlayerData.spd.x = (knockDir > 0) ? FLOAT_TO_FIXED(1.5f) : ((knockDir < 0) ? FLOAT_TO_FIXED(-1.5f) : 0);
        }
        GLOBAL_PlayerData.spd.y = FLOAT_TO_FIXED(-1.5f);
        GLOBAL_PlayerData.dashCounter = -1;
        playSoundEffect(SOUND_EFFECT_BREAKABLE_WALL_HIT);
        initObject(OBJ_SMOKE, thisX, thisY);
        initObject(OBJ_SMOKE, thisX + 8, thisY);
        initObject(OBJ_SMOKE, thisX, thisY + 8);
        initObject(OBJ_SMOKE, thisX + 8, thisY + 8);
        initObject(OBJ_STRAWBERRY, thisX + 4, thisY + 4);

        // Update nametable to clear the breakable wall tile (queue before destroying)
        port_updateCollapseTileNametable(index);
        //Destroy the object
        wall->eType = OBJ_UNUSED;
        wall->flags |= OBJ_FLAG_DIRTY;
        //Restore the original collision data
        // Clear solid flag (bit 0) that breakableWallInit set
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)]     &= ~0x01;
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY)]   &= ~0x01;
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY+1)]   &= ~0x01;
        GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX+1, tileY+1)] &= ~0x01;
        return;
    }
    wall->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_BANK6
static void initSimpleDecorSprite(uint8_t index, uint8_t tile, uint8_t properties) {
    OBJ_DATA *decor = &GLOBAL_OBJList[index];
    decor->oamTile = tile;
    decor->oamProps = properties;
    decor->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_BANK6
static void updateSimpleDecorSprite(uint8_t index) {
    GLOBAL_OBJList[index].flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_BANK6
void flowerInit(uint8_t index) {
    initSimpleDecorSprite(index, FLOWER_SPRITE_1, 0x32); // priority 3, palette 1
}

PORT_FUNC_BANK6
void flowerUpdate(uint8_t index) {
    updateSimpleDecorSprite(index);
}

enum eCollapseTileState {
    COLLAPSE_TILE_STATE_IDLE = 0,
    COLLAPSE_TILE_STATE_COLLAPSING = 1,
    COLLAPSE_TILE_STATE_HIDDEN = 2,
};

PORT_FUNC_BANK6
void collapseTileInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t tileX = GLOBAL_OBJList[index].pos.x / 8;
    uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 8;
    uint8_t i;
    GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)]     |= 0x01; //Set the solid flag
    this->data.collapseTile.state = COLLAPSE_TILE_STATE_IDLE;
    this->data.collapseTile.linkedSpringIndex = -1;

    // Collapse tiles render as background tiles, not sprites, so no OAM slots are needed

    //Check if there is a spring linked to this tile
    // Note: We only search for springs that are already initialized (eType == OBJ_SPRING)
    // If a spring hasn't been initialized yet, the link will be established when the spring initializes (in springInit)
    // This ensures bidirectional linking works regardless of initialization order
    for (i = 1; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_SPRING) {
            uint8_t springTileX = GLOBAL_OBJList[i].pos.x / 8;
            uint8_t springTileY = (GLOBAL_OBJList[i].pos.y + 1) / 8;
            
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
    port_updateCollapseTileNametable(index);
}

PORT_FUNC_BANK6
void collapseTileUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint16_t thisX = this->pos.x;
    uint16_t thisY = this->pos.y;
    uint16_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint16_t playerY = GLOBAL_PlayerData.objData.pos.y;
    bool isPlayerTouching = (thisX <= playerX + 10 && thisX + 10 >= playerX &&
                            thisY <= playerY + 10 && thisY + 10 >= playerY);

    this->oamProps = 0x32; //Ensure consistent properties

    // State machine for collapse tile
    switch (this->data.collapseTile.state) {
        case COLLAPSE_TILE_STATE_IDLE:
            if (this->data.collapseTile.linkedSpringIndex > 0) {
                OBJ_DATA *linkedSpringTile = &GLOBAL_OBJList[this->data.collapseTile.linkedSpringIndex];
                linkedSpringTile->data.spring.isDisabled = false;
            }
            if (isPlayerTouching) {
                this->data.collapseTile.state = COLLAPSE_TILE_STATE_COLLAPSING;
                this->data.collapseTile.frameCount = 15;
                port_updateCollapseTileNametable(index);
            }
            this->oamTile = COLLAPSE_TILE_SPRITE_1;
            this->flags |= OBJ_FLAG_DIRTY;
            break;

        case COLLAPSE_TILE_STATE_COLLAPSING:
            this->data.collapseTile.frameCount -= 1;
            if (this->data.collapseTile.frameCount == 0) {
                uint8_t tileX = GLOBAL_OBJList[index].pos.x / 8;
                uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 8;
                GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)] &= ~0x01; //Unset the solid flag
                this->data.collapseTile.state = COLLAPSE_TILE_STATE_HIDDEN;
                this->data.collapseTile.frameCount = 60;
                this->oamTile = COLLAPSE_TILE_SPRITE_1;
                port_updateCollapseTileNametable(index);
                this->flags |= OBJ_FLAG_DIRTY;
                return;
            }
            // Animation frame: 0->SPRITE_3, 10->SPRITE_2, 20->SPRITE_1
            // On NES: values are consecutive (23, 24, 25), so add 1 per frame
            // On SNES: values are 2 apart (0x48, 0x4A, 0x4C), so add 2 per frame
            uint8_t frame_offset = (uint8_t)(2 - (this->data.collapseTile.frameCount / 5));
            uint8_t newOamTile;
#ifdef __NES__
            newOamTile = (uint8_t)COLLAPSE_TILE_SPRITE_1 + frame_offset;
#else
            newOamTile = (uint8_t)COLLAPSE_TILE_SPRITE_1 + (frame_offset * 2);
#endif
            if (this->oamTile != newOamTile) {
                this->oamTile = newOamTile;
                port_updateCollapseTileNametable(index);
            }
            this->flags |= OBJ_FLAG_DIRTY;
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
                uint8_t tileX = GLOBAL_OBJList[index].pos.x / 8;
                uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 8;
                GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)] |= 0x01; //Set the solid flag
                this->data.collapseTile.state = COLLAPSE_TILE_STATE_IDLE;
                this->oamTile = COLLAPSE_TILE_SPRITE_1;
                port_updateCollapseTileNametable(index);
                this->flags |= OBJ_FLAG_DIRTY;
            } else {
                //Don't draw the tile, but still mark dirty so sprite position updates with scroll
                this->flags |= OBJ_FLAG_DIRTY;
                return;
            }
            break;
        default:
            break;
    }
}

PORT_FUNC_BANK6
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

PORT_FUNC_BANK6
void springUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    struct sPlayerData *player = &GLOBAL_PlayerData;
    uint8_t thisX = this->pos.x;
    uint8_t thisY = this->pos.y;
    uint8_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint8_t playerY = GLOBAL_PlayerData.objData.pos.y;
    bool isPlayerTouching = false;

    // Check if the block underneath the spring is broken
    // Spring is at (thisX, thisY) in pixels, block underneath is one tile row below
    uint8_t tileX = thisX / 8;
    uint8_t tileY = (thisY + 1) / 8;
    uint8_t tileBelowY = tileY + 1;
    
    // Check if there's a solid block underneath the spring
    // Spring is 16px wide (1 tile), check the tile directly below it
    // Also check tileX+1 in case the spring is positioned at a boundary
    bool hasSolidBelow = false;
    if (tileBelowY < 16) {  // Make sure we're within bounds
        uint8_t collisionBelow = GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileBelowY)];
        hasSolidBelow = ((collisionBelow & 0x01) != 0);
        // Also check the tile to the right in case the block is 2 tiles wide
        if (!hasSolidBelow && tileX + 1 < 16) {
            uint8_t collisionBelowRight = GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX + 1, tileBelowY)];
            hasSolidBelow = ((collisionBelowRight & 0x01) != 0);
        }
    }
    
    // If there's no solid block underneath, disable the spring
    if (!hasSolidBelow && !this->data.spring.isDisabled) {
        this->data.spring.isDisabled = true;
    }

    if (this->data.spring.isDisabled) {
        this->data.spring.frameCount = 0;
        this->oamTile = SPRING_SPRITE_1;
        this->oamProps = 0x32;
        this->flags |= OBJ_FLAG_DIRTY;
        return;
    }

    //Check AABB overlap: spring hitbox {0,0,8,8} vs player hitbox {1,3,6,5}
    // player.x+1+6 > spring.x  &&  player.x+1 < spring.x+8
    // player.y+3+5 > spring.y  &&  player.y+3 < spring.y+8
    isPlayerTouching = (playerX + 7 > thisX) &&
                       (playerX + 1 < thisX + 8) &&
                       (playerY + 8 > thisY) &&
                       (playerY + 3 < thisY + 8) &&
                       (GLOBAL_PlayerData.spd.y >= 0);

    if (isPlayerTouching) {
        //Set player position to spring top
        GLOBAL_PlayerData.objData.pos.y = thisY - 4;
        GLOBAL_PlayerData.posF.y = 0; // reset remainder after position snap
        
        //Apply spring physics
        GLOBAL_PlayerData.spd.x = FIXED_MUL_1_5(GLOBAL_PlayerData.spd.x); //Reduce horizontal speed (~0.2)
        GLOBAL_PlayerData.spd.y = INT_TO_FIXED(-3); //Set upward velocity (original)
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
                linkedCollapseTile->data.collapseTile.frameCount = 15;
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
PORT_FUNC_BANK6
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

PORT_DATA_BANK6 static const uint8_t balloonStringFrames[75] = {
    BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2,
    BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1,
    BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2,
    BALLOON_STRING_2, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_1, BALLOON_STRING_1,
};
#define BALLON_YTABLE_SIZE 304
PORT_DATA_BANK6 static const uint8_t balloon_ytable[] = {
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


PORT_FUNC_BANK6
void balloonUpdate(uint8_t index) {
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

    isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x-8) &&
                       (GLOBAL_PlayerData.objData.pos.x < this->pos.x+8) &&
                       (GLOBAL_PlayerData.objData.pos.y > this->pos.y-4) &&
                       (GLOBAL_PlayerData.objData.pos.y < this->pos.y+9);

    if (isPlayerTouching && this->data.balloon.state == BALLOON_STATE_IDLE) {
        this->data.balloon.state = BALLOON_STATE_POPPED;
        this->data.balloon.hideFrameCount = 60;
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

PORT_FUNC_BANK6
void platMovInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    // ccleste: this->x -= 4 (center the 16px sprite on spawn tile)
    this->pos.x -= 4;
    uint8_t hitboxIndex;
    if (this->eType == OBJ_PLATMOV_L) {
        this->data.platMov.isMovingLeft = false;
        GLOBAL_ActiveLevel.movingPlatformDir[GLOBAL_ActiveLevel.movingPlatformCount] = MOVING_PLATFORM_DIR_RIGHT;
    }
    else {
        this->data.platMov.isMovingLeft = true;
        GLOBAL_ActiveLevel.movingPlatformDir[GLOBAL_ActiveLevel.movingPlatformCount] = MOVING_PLATFORM_DIR_LEFT;
    }
    
    this->oamTile = PLATMOV_SPRITE_1;
    this->oamProps = 0x34; // priority 3, palette 2
    //Add the hitbox to the list (max 12 platforms)
    if (GLOBAL_ActiveLevel.movingPlatformCount >= 12) return;
    hitboxIndex = GLOBAL_ActiveLevel.movingPlatformCount * 4;
    this->data.platMov.hitboxIndex = hitboxIndex;
    GLOBAL_ActiveLevel.movingPlatformCount++;
    this->data.platMov.acc = 0;
    this->data.platMov.lastDelta = 0;
    this->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_BANK6
void platMovUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t hitboxIndex = this->data.platMov.hitboxIndex;

    //Fixed point math too expensive, trickery ensues, 0.65f speed is cursed
    // Platform has solids=false in ccleste, so OBJ_move applies amount directly (no <=loop).
    // Speed 0.65: accumulator triggers ~65% of frames, moving 1px each = 0.65 px/frame.
    {
    int8_t platDelta = 0;
    this->data.platMov.acc += 65;
    if (this->data.platMov.acc >= 100) {
        this->data.platMov.acc -= 100;
        if (this->data.platMov.isMovingLeft) {
            this->pos.x -= 1;
            platDelta = -1;
        }
        else {
            this->pos.x += 1;
            platDelta = 1;
        }
    }
    // Store delta for player carry (ccleste carries player by platform delta, not spd.x)
    this->data.platMov.lastDelta = platDelta;
    }



    // Platform hitbox: {0, 0, 16, 4} in game space
    // ccleste uses {0,0,16,8} at 128px with 8px-tall sprites.
    // NES renders platform via render_16x8_sprite = 8 screen px = 4 game px tall.
    // Hitbox height matches the visual: 4 game pixels.
    GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex]     = GLOBAL_OBJList[index].pos.x;
    GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex + 1] = GLOBAL_OBJList[index].pos.y;
    GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex + 2] = GLOBAL_OBJList[index].pos.x + 16;
    GLOBAL_ActiveLevel.movingPlatformHitboxes[hitboxIndex + 3] = GLOBAL_OBJList[index].pos.y + 4;

    // Wrap around (ccleste: if x<-16 then x=128, if x>128 then x=-16)
    if (this->pos.x < -16) {
        this->pos.x = 128;
    } else if (this->pos.x > 128) {
        this->pos.x = -16;
    }


    this->flags |= OBJ_FLAG_DIRTY;
}

enum eKeyState {KEY_STATE_1 = 0, KEY_STATE_2 = 1, KEY_STATE_3 = 2, KEY_STATE_4 = 3};

PORT_FUNC_BANK6
void keyInit(uint8_t index) {
    uint8_t i;
    OBJ_DATA *this = &GLOBAL_OBJList[index];

    this->data.key.frameCount = 0;
    this->data.key.linkedChestIndex = (uint8_t)-1;
    this->data.key.state = KEY_STATE_1;
    this->data.key.isFlipped = false;
    this->data.key.spriteValue = KEY_SPRITE_1;

    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_CHEST) {
            this->data.key.linkedChestIndex = i;
        }
    }

    this->oamTile = KEY_SPRITE_1;
    this->oamProps = 0x32; // priority 3, palette 2
    this->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_BANK6
void keyUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t properties = 0x30; // priority 3 baseline
    bool isPlayerTouching = false;
    this->data.key.frameCount += 1;

    isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 8) &&
                       (GLOBAL_PlayerData.objData.pos.x < this->pos.x + 8) &&
                       (GLOBAL_PlayerData.objData.pos.y > this->pos.y - 9) &&
                       (GLOBAL_PlayerData.objData.pos.y < this->pos.y + 1);

    if (isPlayerTouching) {
        OBJ_DATA *chest = &GLOBAL_OBJList[this->data.key.linkedChestIndex];
        chest->data.chest.keyIsCollected = true;
        this->eType = OBJ_UNUSED;
        this->flags |= OBJ_FLAG_DIRTY;
        port_buildKey(index);
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

PORT_FUNC_BANK6
void chestInit(uint8_t index) {   
    uint8_t i;
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->pos.x -= 4;

    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
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

PORT_FUNC_BANK6
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
            initObject(OBJ_STRAWBERRY, this->pos.x, this->pos.y - 4);
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

PORT_FUNC_BANK6
void monumentInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->extraSpriteCount = 0;
    this->oamTile = MONUMENT_SPRITE_1;
    this->oamProps = 0x36; // priority 3, palette 5
    this->flags |= OBJ_FLAG_DIRTY;
    // Update nametable to show the monument tile (monuments render as background tiles)
    port_updateCollapseTileNametable(index);
}


static const char monumentText[][25] = {
    "-- CELESTE MOUNTAIN -- ",
    "THIS MEMORIAL TO THOSE ",
    " PERISHED ON THE CLIMB "
};

PORT_FUNC_BANK6
void monumentUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    bool isPlayerTouching;
    static bool isTextDisplayed = false;
    static uint8_t curLineCharCount = 0;
    static uint8_t curLineNum = 0;

    this->flags |= OBJ_FLAG_DIRTY;

    isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 4) &&
                       (GLOBAL_PlayerData.objData.pos.x < this->pos.x+16) &&
                       (GLOBAL_PlayerData.objData.pos.y > this->pos.y) &&
                       (GLOBAL_PlayerData.objData.pos.y < this->pos.y+16);

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
            port_drawText((const unsigned char *)outputText, 20, (uint8_t)(92 + (curLineNum * 4)));
        }
    }
    else if (isTextDisplayed) {
        isTextDisplayed = false;
        curLineNum = 0;
        port_drawText((const unsigned char *)"                      ", 20, 92);
        port_drawText((const unsigned char *)"                      ", 20, 96);
        port_drawText((const unsigned char *)"                      ", 20, 100);
    }
}

enum eBigChestState {BIG_CHEST_STATE_IDLE = 0, BIG_CHEST_STATE_OPEN_ANIM = 1, BIG_CHEST_STATE_OPENED = 2};

// eBigChestSprite is defined in sprite_animation_enums.h
PORT_FUNC_BANK6
void bigChestInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->extraSpriteCount = 0;
    this->data.bigChest.state = BIG_CHEST_STATE_IDLE;
    this->data.bigChest.frameCount = 0;
    this->oamTile = BIG_CHEST_SPRITE_1;
    this->oamProps = 0x38; // priority 3, palette 4
    this->flags |= OBJ_FLAG_DIRTY;
    // Update nametable to show the big chest tile (big chest renders as background tiles)
    port_updateCollapseTileNametable(index);
}

PORT_FUNC_BANK6
void bigChestUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    bool isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 4) &&
                            (GLOBAL_PlayerData.objData.pos.x < this->pos.x+16) &&
                            (GLOBAL_PlayerData.objData.pos.y > this->pos.y) &&
                            (GLOBAL_PlayerData.objData.pos.y < this->pos.y+16);

    this->oamTile = BIG_CHEST_SPRITE_1;
    this->oamProps = 0x38; // priority 3, palette 4
    this->flags |= OBJ_FLAG_DIRTY;

    switch (this->data.bigChest.state) {
        case BIG_CHEST_STATE_IDLE:
            if (isPlayerTouching) {
                playSoundEffect(SOUND_EFFECT_BIG_CHEST);
                initObject(OBJ_SMOKE,this->pos.x,this->pos.y);
                initObject(OBJ_SMOKE,this->pos.x+8,this->pos.y);
                this->data.bigChest.state = BIG_CHEST_STATE_OPEN_ANIM;
                this->data.bigChest.frameCount = 0;
                GLOBAL_ActiveLevel.shakeFrames = 60;
                GLOBAL_PausePlayerFrames = 60;
                // Queue nametable update to clear top tiles when opening starts
                port_updateCollapseTileNametable(index);
            }
            break;
        case BIG_CHEST_STATE_OPEN_ANIM:
            this->data.bigChest.frameCount += 1;
            if (this->data.bigChest.frameCount > 60) {
                this->data.bigChest.state = BIG_CHEST_STATE_OPENED;
                // Queue nametable update to show open state (top tiles cleared)
                port_updateCollapseTileNametable(index);
                // Spawn the double dash orb only once when transitioning to OPENED state
                GLOBAL_ActiveLevel.swapCloudPal = true;
                initObject(OBJ_DOUBLE_JUMP_ORB,this->pos.x+4,this->pos.y+8);
            }
            break;
        case BIG_CHEST_STATE_OPENED:
            // Don't set to OBJ_UNUSED - keep as OBJ_BIG_CHEST so bottom tiles (GIDs 112, 113) remain visible
            // Top tiles are already cleared by the rendering logic checking state != IDLE
            this->flags |= OBJ_FLAG_DIRTY;
            break;
    }
}

PORT_FUNC_BANK6
void doubleDashOrbInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->extraSpriteCount = 0;
    this->oamTile = DOUBLE_JUMP_ORB_SPRITE_1;
    this->oamProps = 0x38; // priority 3, palette 4
    this->data.doubleJumpOrb.frameCount = 0;
    this->data.doubleJumpOrb.speedY = -2;  // original speed
    this->data.doubleJumpOrb.accelAccumulator = 0;
    this->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_BANK6
void doubleDashOrbUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    //Speed starts at -8 and goes down by 0.5 every frame
    //alternatively 1 every 2 frames
    int16_t *speedY = &this->data.doubleJumpOrb.speedY;
    int16_t *accelAccumulator = &this->data.doubleJumpOrb.accelAccumulator;

    this->oamTile = DOUBLE_JUMP_ORB_SPRITE_1;
    this->oamProps = 0x38; // priority 3, palette 4
    this->flags |= OBJ_FLAG_DIRTY;

    // Every frame at 30fps: accumulate 2x to match original deceleration curve
    *accelAccumulator += 2;
    if (*accelAccumulator >= 4) {
        *speedY += *accelAccumulator / 4;
        *accelAccumulator %= 4;
    }

    if (*speedY >= 0) {
        bool isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 8) &&
                                (GLOBAL_PlayerData.objData.pos.x < this->pos.x + 8) &&
                                (GLOBAL_PlayerData.objData.pos.y > this->pos.y - 8) &&
                                (GLOBAL_PlayerData.objData.pos.y < this->pos.y+8);
        if (isPlayerTouching) {
            GLOBAL_FreezeFrames = 10;
            GLOBAL_DoubleDashUnlocked = true;
            GLOBAL_PlayerData.dashesLeft = 2;
            GLOBAL_PlayerData.doubleDashUnlocked = true;
            GLOBAL_ActiveLevel.swapActivePalette = true;
            this->eType = OBJ_UNUSED;
            this->flags |= OBJ_FLAG_DIRTY;
            port_buildDoubleDashOrb(index);
            return;
        }
    }
    else {
        this->pos.y += *speedY;
    }
}

PORT_FUNC_BANK6
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

PORT_FUNC_BANK6
void decoTreeInit(uint8_t index) {
    initSimpleDecorSprite(index, DECO_TREE_SPRITE_1, 0x32); // priority 3, palette 2
}
PORT_FUNC_BANK6
void decoTreeUpdate(uint8_t index) {
    updateSimpleDecorSprite(index);
}

PORT_FUNC_BANK6
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
    bool isPlayerTouching = (playerX > thisX-8) &&
                            (playerX < thisX+8) &&
                            (playerY > thisY-5) &&
                            (playerY < thisY+8);

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

        if (this->data.strawberry.frameCount > 30) {
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
        GLOBAL_ActiveLevel.textScrollOffsetX = (uint8_t)(8u - ((remainderX * 2u) % 8u));
        GLOBAL_ActiveLevel.textScrollOffsetY = (uint8_t)(8u - ((remainderY * 2u) % 8u));
        this->pos.y = 128;
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

PORT_FUNC_BANK6
void flyingBerryInit(uint8_t index) {
    OBJ_DATA *berry = &GLOBAL_OBJList[index];
    berry->data.strawberry.frameCount = 0;
    berry->data.strawberry.isCollected = false;
    berry->data.strawberry.startY = berry->pos.y;
    berry->oamTile = FLYING_BERRY_SPRITE_1;
    berry->oamProps = 0x32; // priority 3, palette 0
    berry->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_BANK6
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
    bool isPlayerTouching = (playerX > thisX-8) &&
                            (playerX < thisX+8) &&
                            (playerY > thisY) &&
                            (playerY < thisY+8);

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

        if (this->data.strawberry.frameCount > 30) {
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
        GLOBAL_ActiveLevel.textScrollOffsetX = (uint8_t)(8u - ((remainderX * 2u) % 8u));
        GLOBAL_ActiveLevel.textScrollOffsetY = (uint8_t)(8u - ((remainderY * 2u) % 8u));
        this->pos.y = 128;
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
PORT_FUNC_BANK6
void initObject(enum eOBJType eType, int16_t x, int16_t y) {
    // Find a free slot
    uint8_t i;
    //Starts from 1 to account for the fact that hardcoded player is using slot 0
    for (i = 1; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType == OBJ_UNUSED) {
            GLOBAL_OBJList[i].eType = eType;
            GLOBAL_OBJList[i].pos.x = x;
            GLOBAL_OBJList[i].pos.y = y;
            GLOBAL_OBJList[i].flags = OBJ_FLAG_DIRTY;
            GLOBAL_OBJList[i].oamTile = 0;
            GLOBAL_OBJList[i].oamProps = 0;
            GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
            GLOBAL_OBJList[i].extraSpriteCount = 0;
            // Use if-else instead of switch to avoid jump table issues with banking
            if (eType == OBJ_SMOKE) {
                smokeInit(i);
            } else if (eType == OBJ_BREAKABLE_WALL) {
                breakableWallInit(i);
            } else if (eType == OBJ_DECO_FLOWER) {
                flowerInit(i);
            } else if (eType == OBJ_STRAWBERRY) {
                strawberryInit(i);
            } else if (eType == OBJ_DECO_TREE) {
                decoTreeInit(i);
            } else if (eType == OBJ_SPRING) {
                springInit(i);
            } else if (eType == OBJ_FLYING_BERRY) {
                flyingBerryInit(i);
            } else if (eType == OBJ_COLLAPSE_TILE) {
                collapseTileInit(i);
            } else if (eType == OBJ_BALLOON) {
                balloonInit(i);
            } else if (eType == OBJ_PLATMOV_L || eType == OBJ_PLATMOV_R) {
                platMovInit(i);
            } else if (eType == OBJ_KEY) {
                keyInit(i);
            } else if (eType == OBJ_CHEST) {
                chestInit(i);
            } else if (eType == OBJ_MONUMENT) {
                monumentInit(i);
            } else if (eType == OBJ_BIG_CHEST) {
                bigChestInit(i);
            } else if (eType == OBJ_DOUBLE_JUMP_ORB) {
                doubleDashOrbInit(i);
            } else {
                GLOBAL_OBJList[i].eType = OBJ_UNUSED;
                GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
                GLOBAL_OBJList[i].extraSpriteCount = 0;
            }
            return;
        }
    }
}


PORT_FUNC_BANK6
static void clearObjectDirtyFlag(uint8_t index)
{
    GLOBAL_OBJList[index].flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

// NES staggered updates: non-critical objects skip logic on some frames.
// Sprites still render every frame (OAM must be rebuilt for flicker rotation).
PORT_FUNC_BANK6
static void processObject(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
#ifdef __NES__
    uint8_t frameSlot = (uint8_t)((GLOBAL_FrameCount >> 1) & 0x01); // alternating game frames
#endif
    // Use if-else instead of switch to avoid jump table issues with banking
    // Ordered: UNUSED first (most slots empty), then gameplay-critical, then cosmetic
    if (obj->eType == OBJ_UNUSED) {
        if (obj->flags & OBJ_FLAG_DIRTY) {
            if (index == 0U) {
                clearObjectDirtyFlag(index);
            } else {
                port_buildUnused(index);
            }
        }
    // --- Always update every frame (gameplay-critical) ---
    } else if (obj->eType == OBJ_COLLAPSE_TILE) {
        collapseTileUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_SPRING) {
        springUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_PLATMOV_R || obj->eType == OBJ_PLATMOV_L) {
        platMovUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_BREAKABLE_WALL) {
        breakableWallUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_CHEST) {
        chestUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
#ifdef __NES__
    // --- NES: stagger every other frame (non-critical animations) ---
    } else if (obj->eType == OBJ_BALLOON) {
        if (!frameSlot) balloonUpdate(index);
        else obj->flags |= OBJ_FLAG_DIRTY;
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_STRAWBERRY) {
        if (!frameSlot) strawberryUpdate(index);
        else obj->flags |= OBJ_FLAG_DIRTY;
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_FLYING_BERRY) {
        if (!frameSlot) flyingBerryUpdate(index);
        else obj->flags |= OBJ_FLAG_DIRTY;
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_KEY) {
        if (!frameSlot) keyUpdate(index);
        else obj->flags |= OBJ_FLAG_DIRTY;
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_DOUBLE_JUMP_ORB) {
        if (!frameSlot) doubleDashOrbUpdate(index);
        else obj->flags |= OBJ_FLAG_DIRTY;
        port_buildSpriteIfDirty(index, obj->eType);
    // --- NES: skip update entirely for purely decorative (render still runs) ---
    } else if (obj->eType == OBJ_SMOKE) {
        if (!frameSlot) smokeUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_DECO_TREE) {
        // Decorative: only needs initial dirty flag, no per-frame update needed
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_DECO_FLOWER) {
        port_buildSpriteIfDirty(index, obj->eType);
#else
    // --- SNES: update everything every frame ---
    } else if (obj->eType == OBJ_SMOKE) {
        smokeUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_DOUBLE_JUMP_ORB) {
        doubleDashOrbUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_KEY) {
        keyUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_BALLOON) {
        balloonUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_STRAWBERRY) {
        strawberryUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_FLYING_BERRY) {
        flyingBerryUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_DECO_TREE) {
        decoTreeUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_DECO_FLOWER) {
        flowerUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
#endif
    } else if (obj->eType == OBJ_MONUMENT) {
        monumentUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else if (obj->eType == OBJ_BIG_CHEST) {
        bigChestUpdate(index);
        port_buildSpriteIfDirty(index, obj->eType);
    } else {
        if (obj->flags & OBJ_FLAG_DIRTY) {
            clearObjectDirtyFlag(index);
        }
    }
}

void updateAllObjects(void) {
    uint8_t i;
    port_beginSpriteBuild(&GLOBAL_PlayerData);
    port_prg_bank_switch(6);
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        processObject(i);
    }
    port_prg_bank_switch(0);
    port_finishSpriteBuild();
}

// Sprite-only rebuild: no game logic, just push existing positions to OAM
// with fresh flicker rotation. Keeps OAM rotation at 60Hz.
void rebuildAllSprites(void) {
    uint8_t i;
    port_beginSpriteBuild(&GLOBAL_PlayerData);
    port_prg_bank_switch(6);
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        OBJ_DATA *obj = &GLOBAL_OBJList[i];
        if (obj->eType == OBJ_UNUSED) {
            if (obj->flags & OBJ_FLAG_DIRTY) {
                if (i != 0U) port_buildUnused(i);
                else obj->flags &= (uint8_t)~OBJ_FLAG_DIRTY;
            }
        } else {
            obj->flags |= OBJ_FLAG_DIRTY; // force rebuild for rotation
            port_buildSpriteIfDirty(i, obj->eType);
        }
    }
    port_prg_bank_switch(0);
    port_finishSpriteBuild();
}


PORT_FUNC_BANK6
void playerInit(struct sPlayerData* this);

// onVblank is in fixed bank (bank 0), not bank 6
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

    port_prg_bank_switch(6);
    playerInit(&GLOBAL_PlayerData);
    port_prg_bank_switch(0);
    port_updatePlayerSprite(&GLOBAL_PlayerData);

    port_beginSpriteBuild(&GLOBAL_PlayerData);
    port_prg_bank_switch(6);
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; ++i) {
        processObject(i);
    }
    port_prg_bank_switch(0);
    port_finishSpriteBuild();
}

void LoadNextRoom(void) {
    GLOBAL_ActiveLevel.currentRoomID++;
    LoadRoomData(GLOBAL_ActiveLevel.currentRoomID);
}

int main(void){
    port_init();
    GLOBAL_ActiveLevel.currentRoomID = 17;
    LoadRoomData(GLOBAL_ActiveLevel.currentRoomID);

    for (;;) { 
        onVblank();
    }
}


PORT_FUNC_BANK6
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

PORT_FUNC_BANK6
void playerInit(struct sPlayerData* this){
    uint16_t i;
    this->objData.eType = OBJ_PLAYER;
    this->objData.pos.x = GLOBAL_ActiveLevel.playerSpawnX * 8;
    this->objData.pos.y = GLOBAL_ActiveLevel.playerSpawnY * 8;
    this->objData.flags = OBJ_FLAG_DIRTY;
    this->objData.oamTile = PLAYER_SPRITE_IDLE;
    this->objData.oamProps = 0x38;
    this->objData.extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->objData.extraSpriteCount = 0;
    this->objData.flags |= OBJ_FLAG_DIRTY;

    this->posF.x = 0; // remainder accumulator (ccleste rem.x)
    this->posF.y = 0; // remainder accumulator (ccleste rem.y)

    this->movingPlatformIndex = -1;

    this->spd.x = FLOAT_TO_FIXED(0.0);
    this->spd.y = FLOAT_TO_FIXED(0.0);

    this->eSriteState = PLAYER_SPRITE_IDLE;
    this->isFliped = false;

    this->graceTimer = 0;

    this->doubleDashUnlocked = GLOBAL_DoubleDashUnlocked;
    this->dashesLeft = this->doubleDashUnlocked ? 2 : 1;
    this->dashCounter = 0;
    
    //Reset collision flags
#ifdef __NES__
    port_restoreCollisionFlags(); // Re-derive from ROM
#else
    for (i = 0; i < 256; i++) {
        GLOBAL_ActiveLevel.collisionFlagsArr[i] = GLOBAL_ActiveLevel.collisionFlagsReset[i];
    }
#endif
    GLOBAL_ActiveLevel.movingPlatformCount = 0;


    //Clear out the object array
    for (i = 1; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
        GLOBAL_OBJList[i].extraSpriteCount = 0;
    }

    port_resetSprites();
    // playerInit is now in bank 6, and initObject is also in bank 6, so we're already in the correct bank
    for (i = 0; i < (GLOBAL_ActiveLevel.objectCount*3); i+=3) {
        if (GLOBAL_ActiveLevel.objectData[i] != 0) {
            //Player spawn, remove from generated code in future
            if (GLOBAL_ActiveLevel.objectData[i] == 1) {
                continue;
            }
            initObject(GLOBAL_ActiveLevel.objectData[i], GLOBAL_ActiveLevel.objectData[i+1]*8, GLOBAL_ActiveLevel.objectData[i+2]*8);
        }
    }
    // Don't switch banks here - let the caller handle it. Switching banks while executing from bank 6 causes a crash.

}

PORT_FUNC_BANK6
bool isDeathAtPoint(int16_t x, int16_t y, int16_t w, int16_t h, int16_t xspd, int16_t yspd) {
    // Optimized: hitbox is 6x5, always fits in one tile row/col.
    // Direct tile lookups instead of loops.
    uint8_t tX, tY, idx;

    // Floor spikes (moving down)
    if (yspd >= 0) {
        uint8_t bottomY = (uint8_t)(y + h - 1);
        if ((bottomY & 0x07) >= 6) {
            tY = bottomY >> 3;
            if (tY < 16) {
                tX = (uint8_t)((uint16_t)x >> 3);
                idx = (tY << 4) + tX;
                if (tX < 16 && (GLOBAL_ActiveLevel.collisionFlagsArr[idx] & 0x04)) return true;
                tX = (uint8_t)((uint16_t)(x + w - 1) >> 3);
                idx = (tY << 4) + tX;
                if (tX < 16 && (GLOBAL_ActiveLevel.collisionFlagsArr[idx] & 0x04)) return true;
            }
        }
    }

    // Ceiling spikes (moving up)
    if (yspd <= 0) {
        uint8_t topY = (uint8_t)((uint16_t)y);
        if ((topY & 0x07) <= 2) {
            tY = topY >> 3;
            if (tY < 16) {
                tX = (uint8_t)((uint16_t)x >> 3);
                idx = (tY << 4) + tX;
                if (tX < 16 && (GLOBAL_ActiveLevel.collisionFlagsArr[idx] & 0x08)) return true;
                tX = (uint8_t)((uint16_t)(x + w - 1) >> 3);
                idx = (tY << 4) + tX;
                if (tX < 16 && (GLOBAL_ActiveLevel.collisionFlagsArr[idx] & 0x08)) return true;
            }
        }
    }

    // Left wall spikes (moving left)
    if (xspd <= 0) {
        uint8_t leftX = (uint8_t)((uint16_t)x);
        if ((leftX & 0x07) <= 2) {
            tX = leftX >> 3;
            if (tX < 16) {
                tY = (uint8_t)((uint16_t)y >> 3);
                idx = (tY << 4) + tX;
                if (tY < 16 && (GLOBAL_ActiveLevel.collisionFlagsArr[idx] & 0x10)) return true;
                tY = (uint8_t)((uint16_t)(y + h - 1) >> 3);
                idx = (tY << 4) + tX;
                if (tY < 16 && (GLOBAL_ActiveLevel.collisionFlagsArr[idx] & 0x10)) return true;
            }
        }
    }

    // Right wall spikes (moving right)
    if (xspd >= 0) {
        uint8_t rightX = (uint8_t)(x + w - 1);
        if ((rightX & 0x07) >= 6) {
            tX = rightX >> 3;
            if (tX < 16) {
                tY = (uint8_t)((uint16_t)y >> 3);
                idx = (tY << 4) + tX;
                if (tY < 16 && (GLOBAL_ActiveLevel.collisionFlagsArr[idx] & 0x20)) return true;
                tY = (uint8_t)((uint16_t)(y + h - 1) >> 3);
                idx = (tY << 4) + tX;
                if (tY < 16 && (GLOBAL_ActiveLevel.collisionFlagsArr[idx] & 0x20)) return true;
            }
        }
    }

    return false;
}

PORT_FUNC_BANK6
static bool OBJ_isDeathAt(struct sPlayerData* this, int16_t xOffset, int16_t yOffset) {
    int16_t x = this->objData.pos.x + xOffset;
    int16_t y = this->objData.pos.y + yOffset;

    // Pass hitbox bounds {1,3,6,5} instead of full sprite bounds
    return isDeathAtPoint(x + 1, y + 3, 6, 5, FIXED_TO_INT(this->spd.x), FIXED_TO_INT(this->spd.y));
}

PORT_FUNC_BANK6
static bool isTileSolidAtPoint(int16_t x, int16_t y) {
    // Convert to unsigned for shift (avoids sign extension issues)
    // and use uint8_t to keep computation in 8-bit where possible
    uint8_t tileX = (uint8_t)((uint16_t)x >> 3);
    uint8_t tileY = (uint8_t)((uint16_t)y >> 3);

    // Bounds check: uint8_t so only need >= 16
    if (tileX >= 16 || tileY >= 16) {
        return false;
    }

    return GLOBAL_ActiveLevel.collisionFlagsArr[(tileY << 4) + tileX] & 0x01;
}

PORT_FUNC_BANK6
static bool OBJ_isSolidAt(struct sPlayerData* this, int16_t xOffset, int16_t yOffset) {
    int16_t x = this->objData.pos.x;
    int16_t y = this->objData.pos.y;
    int16_t checkX;
    int16_t checkY;

    // Calculate check positions based on offset
    checkX = x + xOffset;
    checkY = y + yOffset;


    // One-way platform check using AABB overlap (matches ccleste OBJ_is_solid)
    // ccleste: platform only solid when oy>0, player hitbox overlaps at (ox,oy) but NOT at (ox,0)
    // Player hitbox: {1, 3, 6, 5} → left=x+1, right=x+7, top=y+3, bottom=y+8
    // Platform hitbox stored as: [left, top, right, bottom]
    if (GLOBAL_ActiveLevel.movingPlatformCount > 0 && yOffset > 0) {
        uint8_t pi;
        // Player hitbox at offset position
        int16_t pLeft  = checkX + 1;
        int16_t pRight = checkX + 7;
        int16_t pTop   = checkY + 3;
        int16_t pBot   = checkY + 8;
        // Player hitbox at current position (for one-way check)
        int16_t cTop   = y + 3;
        int16_t cBot   = y + 8;
        for (pi = 0; pi < GLOBAL_ActiveLevel.movingPlatformCount; pi++) {
            uint8_t base = pi * 4;
            int16_t platL = GLOBAL_ActiveLevel.movingPlatformHitboxes[base];
            int16_t platT = GLOBAL_ActiveLevel.movingPlatformHitboxes[base+1];
            int16_t platR = GLOBAL_ActiveLevel.movingPlatformHitboxes[base+2];
            int16_t platB = GLOBAL_ActiveLevel.movingPlatformHitboxes[base+3];
            // AABB overlap at offset: player hitbox overlaps platform?
            if (pRight > platL && pLeft < platR && pBot > platT && pTop < platB) {
                // One-way: NOT already overlapping at current Y (can jump through from below)
                if (!(cBot > platT && cTop < platB)) {
                    return true;
                }
            }
        }
    }

    // Check tile collision (original hitbox {1,3,6,5} corners)
    if (isTileSolidAtPoint(checkX + 1, checkY + 3) ||
        isTileSolidAtPoint(checkX + 6, checkY + 3) ||
        isTileSolidAtPoint(checkX + 1, checkY + 7) ||
        isTileSolidAtPoint(checkX + 6, checkY + 7)) {
        return true;
    }
    
    
    return false;
}

#define FPS60_SCALE_FACTOR 1.0f
void playerUpdate(struct sPlayerData* this) {
    port_prg_bank_switch(6);
    int16_t spdXstepInt;
    int16_t spdYStepInt;

    int8_t inputX = 0;
    int8_t inputY = 0;

    // Precomputed fixed-point constants (avoids fix16_from_float calls)
    #define FP_ACCEL       0x00009999   // 0.6
    #define FP_DECCEL      0x00002666   // 0.15
    #define FP_ACCEL_AIR   0x00006666   // 0.4
    #define FP_MAXRUN      0x00010000   // 1.0 (original)
    #define FP_MAXFALL     0x00020000   // 2.0 (original)
    #define FP_MAXFALL_WALL 0x00006666  // 0.4 (original)
    #define FP_GRAVITY     0x000035C2   // 0.21
    #define FP_SPD_THRESH  0x00004400   // ~0.266 (original 0.15, extended for +1 frame hover at peak)
    #define FP_ZERO        0x00000000
    fixed_t accel = FP_ACCEL;
    fixed_t deccel = FP_DECCEL;
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
        this->graceTimer=6;
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
        maxrun = FP_MAXRUN;

        if (!onGround) {
            accel = FP_ACCEL_AIR;
        }

        maxfall = FP_MAXFALL;
        gravity = FP_GRAVITY;

        if (FIXED_ABS(this->spd.y) <= FP_SPD_THRESH) {
            gravity = FIXED_MUL_HALF(gravity);
        }

        // wall slide
        if (inputX!=0 && OBJ_isSolidAt(this, inputX,0)) {
            maxfall=FP_MAXFALL_WALL;
            onWall = true;
            if (randint16(0,10)<2) {
                initObject(OBJ_SMOKE,this->objData.pos.x+inputX*6,this->objData.pos.y);
            }
        }

        //Gravity
        if (onGround) {
            this->spd.y = FP_ZERO;
            this->graceTimer = 6;
        }
        else {
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
                this->spd.y = INT_TO_FIXED(-2);
                initObject(OBJ_SMOKE,this->objData.pos.x,this->objData.pos.y+4);
            } else {
                //Wall jump
                int8_t wall_dir=(OBJ_isSolidAt(this, -3,0) ? -1 : (OBJ_isSolidAt(this, 3,0) ? 1 : 0));
                if (wall_dir!=0) {
                    playSoundEffect(SOUND_EFFECT_JUMP);
                    jumpBuffer = 0;
                    this->spd.y = INT_TO_FIXED(-2);
                    { fixed_t wallSpd = FIXED_ADD(maxrun, INT_TO_FIXED(1));
                    this->spd.x = (wall_dir < 0) ? wallSpd : -wallSpd; }
                        initObject(OBJ_SMOKE,this->objData.pos.x+wall_dir*6,this->objData.pos.y);
                }
            }
        }

        //Dash handling
        if (btnDash && this->dashesLeft > 0) {
            //Dash code
            // Dash speeds as constants (avoids runtime multiply)
            #define DASH_FULL_SPEED (FLOAT_TO_FIXED(5.0f))
            #define DASH_HALF_SPEED (FLOAT_TO_FIXED(5.0f * 0.70710678118f))

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

            // vInput.x/y are -1, 0, or 1 — just sign-flip the constant
            if (vInput.x != 0) {
                if (vInput.y != 0) {
                    this->spd.x = (vInput.x > 0) ? DASH_HALF_SPEED : -DASH_HALF_SPEED;
                    this->spd.y = (vInput.y > 0) ? DASH_HALF_SPEED : -DASH_HALF_SPEED;
                } else {
                    this->spd.x = (vInput.x > 0) ? DASH_FULL_SPEED : -DASH_FULL_SPEED;
                    this->spd.y = INT_TO_FIXED(0);
                }
            } else if (vInput.y != 0) {
                this->spd.x = INT_TO_FIXED(0);
                this->spd.y = (vInput.y > 0) ? DASH_FULL_SPEED : -DASH_FULL_SPEED;
            }

            playSoundEffect(SOUND_EFFECT_DASH_START);
            GLOBAL_FreezeFrames = 2;
            GLOBAL_ActiveLevel.shakeFrames = 6;
            this->dashTarget.x = (vInput.x > 0) ? INT_TO_FIXED(2) : ((vInput.x < 0) ? INT_TO_FIXED(-2) : 0);
            this->dashTarget.y = (vInput.y > 0) ? INT_TO_FIXED(2) : ((vInput.y < 0) ? INT_TO_FIXED(-2) : 0);
            this->dashAccel.x=0x00018000; // 1.5 (original)
            this->dashAccel.y=0x00018000;

            if (this->spd.y<0) {
                this->dashTarget.y = FIXED_MUL_DASH_DIAG(this->dashTarget.y); // ~0.9
            }

            //0.1f is a small adjustment to make the dash feel more natural
            if (this->spd.y!=0) {
                this->dashAccel.y = FIXED_MUL_DASH_DIAG(this->dashAccel.y); // ~0.807
            }
            if (this->spd.x!=0) {
                this->dashAccel.x = FIXED_MUL_DASH_DIAG(this->dashAccel.x); // ~0.807
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

    // ccleste-style OBJ_move: remainder accumulation
    // posF.x/y are used as remainder accumulators (like ccleste's rem.x/y)
    // P8flr(rem + 0.5) in fix16 = (rem + 32768) >> 16 (arithmetic shift = floor)
    #define P8FLR_ROUND(rem) ((int16_t)(((rem) + 32768) >> 16))

    // [X] accumulate remainder and compute integer movement
    this->posF.x += this->spd.x;
    spdXstepInt = P8FLR_ROUND(this->posF.x);
    this->posF.x -= INT_TO_FIXED(spdXstepInt);

    // [Y] accumulate remainder and compute integer movement
    this->posF.y += this->spd.y;
    spdYStepInt = P8FLR_ROUND(this->posF.y);
    this->posF.y -= INT_TO_FIXED(spdYStepInt);

    //Handle moving platforms — detect if player is standing ON TOP of one
    // ccleste: !OBJ_check(plat, player, 0, 0) && OBJ_collide(plat, player, 0, -1)
    // = player NOT overlapping platform now, but WOULD overlap if platform shifted up 1px
    // Equivalent: player hitbox at current pos doesn't overlap, but at (0,+1) does
    this->movingPlatformIndex = -1;
    if ((GLOBAL_PlayerData.spd.y >= 0) && GLOBAL_ActiveLevel.movingPlatformCount > 0) {
        uint8_t pi;
        // Player hitbox at current position: {x+1, y+3} to {x+7, y+8}
        int16_t cLeft  = this->objData.pos.x + 1;
        int16_t cRight = this->objData.pos.x + 7;
        int16_t cTop   = this->objData.pos.y + 3;
        int16_t cBot   = this->objData.pos.y + 8;
        // Player hitbox shifted down by 1 (ground probe)
        int16_t gTop   = cTop + 1;
        int16_t gBot   = cBot + 1;
        for (pi = 0; pi < GLOBAL_ActiveLevel.movingPlatformCount; pi++) {
            uint8_t base = pi * 4;
            int16_t platL = GLOBAL_ActiveLevel.movingPlatformHitboxes[base];
            int16_t platT = GLOBAL_ActiveLevel.movingPlatformHitboxes[base+1];
            int16_t platR = GLOBAL_ActiveLevel.movingPlatformHitboxes[base+2];
            int16_t platB = GLOBAL_ActiveLevel.movingPlatformHitboxes[base+3];
            // One-way: overlaps at ground probe but NOT at current position
            bool overlapsNow   = (cRight > platL && cLeft < platR && cBot > platT && cTop < platB);
            bool overlapsBelow = (cRight > platL && cLeft < platR && gBot > platT && gTop < platB);
            if (!overlapsNow && overlapsBelow) {
                playerOnMovingPlatform = true;
                this->movingPlatformIndex = pi;
                // Don't zero spdYStepInt here — let the per-pixel Y loop
                // land the player on the surface naturally via OBJ_isSolidAt
                break;
            }
        }
    }

    // Per-pixel movement with collision (matches ccleste OBJ_move_x/OBJ_move_y)
    // PICO-8 "for i=0,abs(amount) do" is inclusive → |amount|+1 iterations via <=

    //Move X
    if (spdXstepInt != 0) {
        step = (spdXstepInt < 0) ? -1 : 1;
        int16_t absStep = (spdXstepInt < 0) ? -spdXstepInt : spdXstepInt;
        for (i = 0; i <= absStep; i++) {
            if (!OBJ_isSolidAt(this, step, 0)) {
                this->objData.pos.x += step;
            } else {
                this->spd.x = 0;
                this->posF.x = 0; // zero remainder on collision (ccleste behavior)
                break;
            }
        }
    }

    //Move Y

    if (spdYStepInt != 0) {
        step = (spdYStepInt < 0) ? -1 : 1;
        int16_t absStep = (spdYStepInt < 0) ? -spdYStepInt : spdYStepInt;
        for (i = 0; i <= absStep; i++) {
            if (!OBJ_isSolidAt(this, 0, step)) {
                this->objData.pos.y += step;
            } else {
                this->spd.y = 0;
                this->posF.y = 0; // zero remainder on collision (ccleste behavior)
                break;
            }
        }
    }

    // Platform carry: move player by platform's per-frame delta (ccleste OBJ_move_x(hit, delta, 1))
    if (playerOnMovingPlatform && this->movingPlatformIndex >= 0) {
        uint8_t pi;
        int8_t platDelta = 0;
        // Find the platform object to get its lastDelta
        for (pi = 1; pi < GLOBAL_OBJ_LIST_SIZE; pi++) {
            if ((GLOBAL_OBJList[pi].eType == OBJ_PLATMOV_L || GLOBAL_OBJList[pi].eType == OBJ_PLATMOV_R) &&
                GLOBAL_OBJList[pi].data.platMov.hitboxIndex == (uint8_t)(this->movingPlatformIndex * 4)) {
                platDelta = GLOBAL_OBJList[pi].data.platMov.lastDelta;
                break;
            }
        }
        if (platDelta != 0) {
            // Carry player by platform delta (with collision check)
            if (!OBJ_isSolidAt(this, platDelta, 0)) {
                this->objData.pos.x += platDelta;
            }
        }
    }

    // Death logic (before clamping — original has no position clamp)
    if (OBJ_isDeathAt(this, 0, 0) || this->objData.pos.y > 128) {
        playSoundEffect(SOUND_EFFECT_DEATH);
        this->objData.pos.y = 128; //offscreen
        GLOBAL_ActiveLevel.shakeFrames = 10;
        port_prg_bank_switch(6);
        playerInit(this);
        port_prg_bank_switch(0);
        return;
    }

    // next level (before clamping — player must be able to reach y < -4)
    if (this->objData.pos.y < -4 && GLOBAL_ActiveLevel.currentRoomID < 31) {
        port_prg_bank_switch(0);
        LoadNextRoom();
        return;
    }

    // Clamp position (original clamps X in draw, not physics, but needed for NES)
    this->objData.pos.x = FIXED_CLAMP(this->objData.pos.x, -1, 121);
    this->objData.pos.y = FIXED_CLAMP(this->objData.pos.y, -4, 128);

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
PORT_DATA_BANK6
const uint16_t rand_table[256] = {
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
PORT_FUNC_BANK6
uint16_t my_rand() {
    global_randSeed = (global_randSeed + 1) % 256;
    return rand_table[global_randSeed];
}

// Returns a random integer in [min, max]
PORT_FUNC_BANK6
int16_t randint16(int16_t min, int16_t max) {
    uint16_t range;
    uint16_t randomValue;
    if (max <= min) return min; // handle edge case
    range = (uint16_t)(max - min) + 1;
    randomValue = my_rand();
    return (int16_t)((randomValue % range) + min);
}

// FamiStudio update — called from fixed bank, switches to bank 7 internally
extern void famistudio_update(void);
static void music_update(void) {
    port_prg_bank_switch(7);
    famistudio_update();
    port_prg_bank_switch(0);
}

void onVblank(void) {
    static bool gameFrameToggle = false;

    //Start of vblank critical code
    port_vblank();

    // Music update — runs at 60Hz regardless of game logic rate
    music_update();

    // Frame counter at 60Hz for animation timing
    GLOBAL_FrameCount += 1;

    // 30fps: spread compute across 2 VBlank frames
    // Frame A (game):    player physics + player sprite (latency-sensitive)
    // Frame B (display): object updates + object sprite builds
    gameFrameToggle = !gameFrameToggle;

    if (gameFrameToggle) {
        // --- GAME FRAME: player physics + sprite-only OAM rebuild ---
        if (GLOBAL_FreezeFrames > 0) {
            GLOBAL_FreezeFrames--;
            return;
        }
        if (GLOBAL_ActiveLevel.shakeFrames > 0) {
            GLOBAL_ActiveLevel.shakeFrames--;
        }

        GLOBAL_InputState = port_getInputs();
        playerUpdate(&GLOBAL_PlayerData);
        port_updatePlayerSprite(&GLOBAL_PlayerData);
        // Rebuild object sprites with fresh flicker rotation (no game logic)
        rebuildAllSprites();
    } else {
        // --- DISPLAY FRAME: object updates + sprite builds ---
        if (GLOBAL_FreezeFrames > 0) {
            return; // freeze blocks objects too
        }

        updateAllObjects();
    }
}



