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
#ifndef __NES__
#include "port/snes_farcall.h"
#endif

// 60fps vs 30fps physics scaling factor

//Prototypes
int16_t randint16(int16_t min, int16_t max);

//Basic math functions that a compiler should have
#ifdef __NES__
PORT_FUNC_BANK6
#endif
static int16_t sign(int16_t v) {
    return v > 0 ? 1 : (v < 0 ? -1 : 0);
}

enum eSoundEffect {
    SOUND_EFFECT_JUMP = 0,
    SOUND_EFFECT_WALL_JUMP,
    SOUND_EFFECT_DASH_START,
    SOUND_EFFECT_DASH_END,
    SOUND_EFFECT_DASH_RESTORED,
    SOUND_EFFECT_DASH_MISFIRE,
    SOUND_EFFECT_DEATH,
    SOUND_EFFECT_BREAKABLE_WALL_HIT,
    SOUND_EFFECT_STRAWBERRY,
    SOUND_EFFECT_SPRING,
    SOUND_EFFECT_BALLOON_POP,
    SOUND_EFFECT_KEY_COLLECT,
    SOUND_EFFECT_TEXT_DISPLAY,
    SOUND_EFFECT_BIG_CHEST,
    SOUND_EFFECT_FLYING_BERRY,
    SOUND_EFFECT_TITLE_START,
    SOUND_EFFECT_FLAG
};

#ifndef __NES__
    enum eMusicPattern {
        MUSIC_PATTERN_LEVEL_START = 0,
        MUSIC_PATTERN_ORB = 10,
        MUSIC_PATTERN_WIND = 20,
        MUSIC_PATTERN_WIDE_OPEN = 30,
        MUSIC_PATTERN_TITLE = 40
    };
    static uint8_t s_musicTimer = 0u;
    static bool s_inTitleScreen = false;
    static int8_t s_titleStartTimer = 0;

    static uint8_t mapSoundEffectToSpcID(enum eSoundEffect soundEffect) {
        switch (soundEffect) {
            case SOUND_EFFECT_JUMP: return 1u;
            case SOUND_EFFECT_WALL_JUMP: return 2u;
            case SOUND_EFFECT_DASH_START: return 3u;
            case SOUND_EFFECT_DASH_END: return 2u;
            case SOUND_EFFECT_DASH_RESTORED: return 54u;
            case SOUND_EFFECT_DASH_MISFIRE: return 9u;
            case SOUND_EFFECT_DEATH: return 0u;
            case SOUND_EFFECT_BREAKABLE_WALL_HIT: return 16u;
            case SOUND_EFFECT_STRAWBERRY: return 13u;
            case SOUND_EFFECT_SPRING: return 8u;
            case SOUND_EFFECT_BALLOON_POP: return 6u;
            case SOUND_EFFECT_KEY_COLLECT: return 23u;
            case SOUND_EFFECT_TEXT_DISPLAY: return 35u;
            case SOUND_EFFECT_BIG_CHEST: return 37u;
            case SOUND_EFFECT_FLYING_BERRY: return 14u;
            case SOUND_EFFECT_TITLE_START: return 38u;
            case SOUND_EFFECT_FLAG: return 55u;
            default: return 0u;
        }
    }
#endif

#ifdef __NES__
PORT_FUNC_BANK6
#endif
void playSoundEffect(enum eSoundEffect soundEffect){
#ifdef __NES__
    (void)soundEffect;
#else
    port_audioPlaySfx(mapSoundEffectToSpcID(soundEffect));
#endif
}

void LoadRoomData(uint16_t roomID);
extern uint8_t GLOBAL_InputState;



uint8_t GLOBAL_InputState = 0;

OBJ_DATA GLOBAL_OBJList[GLOBAL_OBJ_LIST_SIZE] = {0};

volatile uint16_t GLOBAL_FrameCount = 0;
static volatile uint16_t s_lastGameplayFrame = 0u;

uint8_t GLOBAL_FreezeFrames = 0;

uint8_t GLOBAL_PausePlayerFrames = 0;
//Game state globals
bool GLOBAL_DoubleDashUnlocked = false;
uint8_t GLOBAL_GotFruitBits[4] = {0};
uint8_t GLOBAL_FruitCount = 0;
uint16_t GLOBAL_DeathCount = 0;
uint8_t GLOBAL_TimerFrames = 0;
uint8_t GLOBAL_TimerSeconds = 0;
uint16_t GLOBAL_TimerMinutes = 0;

#define GLOBAL_FRUIT_COUNT 30u
#define GLOBAL_FRUIT_BITS_SIZE 4u
#define GLOBAL_LEVEL_COUNT 32u



enum eMovingPlatformDir {MOVING_PLATFORM_DIR_IDLE = 0, MOVING_PLATFORM_DIR_LEFT = 1, MOVING_PLATFORM_DIR_RIGHT = 2};

struct sActiveLevelData GLOBAL_ActiveLevel;

struct sPlayerData GLOBAL_PlayerData;

#ifdef __SNES__
extern uint8_t GLOBAL_InputLo;
#endif

void initObject(enum eOBJType eType, int16_t x, int16_t y);

#ifdef __NES__
PORT_FUNC_BANK6
#endif
static uint8_t currentFruitIndex(void) {
    uint16_t roomID = GLOBAL_ActiveLevel.currentRoomID;
    if (roomID == 0u || roomID > GLOBAL_FRUIT_COUNT) {
        return 0xFFu;
    }
    return (uint8_t)(roomID - 1u);
}

#ifdef __NES__
PORT_FUNC_BANK6
#endif
static bool gotFruitAt(uint8_t fruitIndex) {
    if (fruitIndex >= GLOBAL_FRUIT_COUNT) {
        return false;
    }
    return (GLOBAL_GotFruitBits[fruitIndex >> 3] & (uint8_t)(1u << (fruitIndex & 7u))) != 0u;
}

#ifdef __NES__
PORT_FUNC_BANK6
#endif
static bool currentRoomFruitCollected(void) {
    return gotFruitAt(currentFruitIndex());
}

#ifdef __NES__
PORT_FUNC_BANK6
#endif
static void collectCurrentRoomFruit(void) {
    uint8_t fruitIndex = currentFruitIndex();
    uint8_t mask;
    if (fruitIndex >= GLOBAL_FRUIT_COUNT) {
        return;
    }
    mask = (uint8_t)(1u << (fruitIndex & 7u));
    if ((GLOBAL_GotFruitBits[fruitIndex >> 3] & mask) == 0u) {
        GLOBAL_GotFruitBits[fruitIndex >> 3] |= mask;
        GLOBAL_FruitCount++;
    }
}

#ifdef __NES__
PORT_FUNC_BANK6
#endif
static bool objectSkipsWhenFruitCollected(enum eOBJType eType) {
    return eType == OBJ_STRAWBERRY || eType == OBJ_FLYING_BERRY ||
           eType == OBJ_BREAKABLE_WALL || eType == OBJ_KEY ||
           eType == OBJ_CHEST;
}

#ifdef __NES__
PORT_FUNC_BANK6
#endif
static void resetRunState(void) {
    uint8_t i;
    for (i = 0; i < GLOBAL_FRUIT_BITS_SIZE; ++i) {
        GLOBAL_GotFruitBits[i] = 0;
    }
    GLOBAL_FruitCount = 0;
    GLOBAL_DeathCount = 0;
    GLOBAL_FrameCount = 0;
    GLOBAL_TimerFrames = 0;
    GLOBAL_TimerSeconds = 0;
    GLOBAL_TimerMinutes = 0;
    GLOBAL_DoubleDashUnlocked = false;
}

#ifndef __NES__
extern uint16_t GLOBAL_ScrollBG2X;
extern uint16_t GLOBAL_ScrollBG2Y;
extern uint16_t GLOBAL_ScrollBG3X;
extern uint16_t GLOBAL_ScrollBG3Y;
extern uint16_t GLOBAL_ScrollBG4Y;

PORT_FUNC_BANK5
static void syncCameraFromPlayer(void) {
    int16_t playerRenderY = (int16_t)(GLOBAL_PlayerData.objData.pos.y << 1);
    int16_t smoothScrollY = (playerRenderY - GLOBAL_ActiveLevel.scrollPointY) >> 2;
    int16_t shakeAmount = GLOBAL_ActiveLevel.shakeFrames > 0 ? ((GLOBAL_FrameCount & 1u) ? 2 : -2) : 0;

    GLOBAL_ScrollBG2Y = CLAMP(playerRenderY - 16 - GLOBAL_ActiveLevel.scrollPointY, 0, 31);
    GLOBAL_ScrollBG2X = 0;
    GLOBAL_ScrollBG3X = GLOBAL_ScrollBG2X + shakeAmount;
    GLOBAL_ScrollBG3Y = GLOBAL_ScrollBG2Y + shakeAmount;
    GLOBAL_ScrollBG4Y =  smoothScrollY + (shakeAmount >> 1) - (GLOBAL_ActiveLevel.currentRoomID << 6);
}
#endif

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
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
#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
static void initSimpleDecorSprite(uint8_t index, uint8_t tile, uint8_t properties) {
    OBJ_DATA *decor = &GLOBAL_OBJList[index];
    decor->oamTile = tile;
    decor->oamProps = properties;
    decor->flags |= OBJ_FLAG_DIRTY;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
static void updateSimpleDecorSprite(uint8_t index) {
    GLOBAL_OBJList[index].flags |= OBJ_FLAG_DIRTY;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
void flowerInit(uint8_t index) {
    initSimpleDecorSprite(index, FLOWER_SPRITE_1, 0x32); // priority 3, palette 1
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
void flowerUpdate(uint8_t index) {
    updateSimpleDecorSprite(index);
}

enum eCollapseTileState {
    COLLAPSE_TILE_STATE_IDLE = 0,
    COLLAPSE_TILE_STATE_COLLAPSING = 1,
    COLLAPSE_TILE_STATE_HIDDEN = 2,
};

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
void springInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->data.spring.frameCount = 0;
    this->data.spring.isDisabled = false;
    this->data.spring.linkedCollapseTileIndex = -1;

#ifdef __SNES__
    // Springs render as a single standard sprite on SNES; leaving the old
    // fixed extra-sprite reservation here corrupts the dynamic extra-sprite
    // allocator used by balloons and other multi-sprite objects.
    this->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->extraSpriteCount = 0;
#else
    // Assign fixed OAM slot (4 sprites per 16x16 object)
    // Player uses slots 0-3, objects start at slot 4
    // Each object gets 4 consecutive slots based on its index
    this->extraSpriteBase = 4 + (index * 4);
    this->extraSpriteCount = 4;
#endif

    this->oamTile = SPRING_SPRITE_1;
    this->oamProps = 0x32; // priority 3, palette 2
    this->flags |= OBJ_FLAG_DIRTY;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
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
        
        // Apply spring physics (ccleste: spd.x *= 0.2, spd.y = -3).
        GLOBAL_PlayerData.spd.x = GLOBAL_PlayerData.spd.x / 5;
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
#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
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

#ifdef __NES__
PORT_DATA_BANK6
#elif defined(__SNES__)
PORT_DATA_BANK0
#else
PORT_DATA_BANK4
#endif
static const uint8_t balloonStringFrames[75] = {
    BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2,
    BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1,
    BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_1, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2, BALLOON_STRING_2,
    BALLOON_STRING_2, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_3, BALLOON_STRING_1, BALLOON_STRING_1,
};
#define BALLON_YTABLE_SIZE 304
#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static uint8_t balloonYOffset(uint16_t tableIndex) {
    if (tableIndex < 26u) {
        return 0u;
    }
    if (tableIndex < 53u) {
        return 1u;
    }
    if (tableIndex < 85u) {
        return 2u;
    }
    if (tableIndex < 230u) {
        return 3u;
    }
    if (tableIndex < 262u) {
        return 2u;
    }
    if (tableIndex < 289u) {
        return 1u;
    }
    return 0u;
}


#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
void balloonUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    int8_t yOffset = (int8_t)balloonYOffset(this->data.balloon.yTableIndex);
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
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

PORT_DATA_BANK0
static const unsigned char monumentText[][25] = {
    "-- celeste mountain -- ",
    "this memorial to those ",
    " perished on the climb "
};
static unsigned char monumentBlankLine[] = "                            ";
#ifdef __NES__
#define MONUMENT_TEXT_X 20u
#define MONUMENT_TEXT_Y 92u
#define MONUMENT_TEXT_LINE_SPACING 4u
#define MONUMENT_TEXT_CHAR_ADVANCE 5u
#else
#define MONUMENT_TEXT_X 8u
#define MONUMENT_TEXT_Y 80u
#define MONUMENT_TEXT_LINE_SPACING 7u
#define MONUMENT_TEXT_CHAR_ADVANCE 5u
#endif
bool GLOBAL_MonumentTextDisplayed = false;
uint8_t GLOBAL_MonumentCurLineCharCount = 0;
uint8_t GLOBAL_MonumentCurLineNum = 0;
static uint8_t s_monumentTextTick = 0;

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static void resetMonumentTextState(void) {
    GLOBAL_MonumentTextDisplayed = false;
    GLOBAL_MonumentCurLineCharCount = 0;
    GLOBAL_MonumentCurLineNum = 0;
    s_monumentTextTick = 0;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static void monumentDrawCharHelper(uint8_t lineNum, uint8_t charIndex) {
#ifdef __SNES__
    port_drawTextPico8N(&monumentText[lineNum][charIndex],
                        1u,
                        (uint8_t)(MONUMENT_TEXT_X + (charIndex * MONUMENT_TEXT_CHAR_ADVANCE)),
                        (uint8_t)(MONUMENT_TEXT_Y + (lineNum * MONUMENT_TEXT_LINE_SPACING)));
#else
    port_drawTextN(&monumentText[lineNum][charIndex],
                   1u,
                   (uint8_t)(MONUMENT_TEXT_X + (charIndex * MONUMENT_TEXT_CHAR_ADVANCE)),
                   (uint8_t)(MONUMENT_TEXT_Y + (lineNum * MONUMENT_TEXT_LINE_SPACING)));
#endif
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static void monumentClearHelper(void) {
    port_drawText(monumentBlankLine, MONUMENT_TEXT_X, MONUMENT_TEXT_Y);
    port_drawText(monumentBlankLine,
                  MONUMENT_TEXT_X,
                  (uint8_t)(MONUMENT_TEXT_Y + MONUMENT_TEXT_LINE_SPACING));
    port_drawText(monumentBlankLine,
                  MONUMENT_TEXT_X,
                  (uint8_t)(MONUMENT_TEXT_Y + (MONUMENT_TEXT_LINE_SPACING * 2u)));
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
void monumentUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    bool isPlayerTouching;

    this->flags |= OBJ_FLAG_DIRTY;

    isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 4) &&
                       (GLOBAL_PlayerData.objData.pos.x < this->pos.x+16) &&
                       (GLOBAL_PlayerData.objData.pos.y > this->pos.y) &&
                       (GLOBAL_PlayerData.objData.pos.y < this->pos.y+16);
    if (isPlayerTouching) {
        GLOBAL_MonumentTextDisplayed = true;
        s_monumentTextTick ^= 1u;
        if (s_monumentTextTick != 0u) {
            return;
        }

        if (GLOBAL_MonumentCurLineNum < 3u) {
            if (GLOBAL_MonumentCurLineCharCount < 23u) {
                uint8_t charIndex = GLOBAL_MonumentCurLineCharCount;
                GLOBAL_MonumentCurLineCharCount++;
                playSoundEffect(SOUND_EFFECT_TEXT_DISPLAY);
                monumentDrawCharHelper(GLOBAL_MonumentCurLineNum, charIndex);
            } else {
                GLOBAL_MonumentCurLineNum++;
                GLOBAL_MonumentCurLineCharCount = 0;
            }
        }
    }
    else if (GLOBAL_MonumentTextDisplayed) {
        resetMonumentTextState();
        monumentClearHelper();
    }

}

enum eBigChestState {BIG_CHEST_STATE_IDLE = 0, BIG_CHEST_STATE_OPEN_ANIM = 1, BIG_CHEST_STATE_OPENED = 2};

// eBigChestSprite is defined in sprite_animation_enums.h
#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
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
#ifndef __NES__
                port_audioStopAll();
#endif
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

#ifdef __NES__
PORT_DATA_BANK6
#elif defined(__SNES__)
PORT_DATA_BANK0
#else
PORT_DATA_BANK4
#endif
static const unsigned char flagBlankLine[] = "                ";
bool GLOBAL_FlagOverlayShow = false;
uint8_t GLOBAL_FlagOverlayLine0Len = 0u;
uint8_t GLOBAL_FlagOverlayLine1Len = 0u;
uint8_t GLOBAL_FlagOverlayLine2Len = 0u;
unsigned char GLOBAL_FlagOverlayLine0[17] = {0};
unsigned char GLOBAL_FlagOverlayLine1[17] = {0};
unsigned char GLOBAL_FlagOverlayLine2[17] = {0};

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static void flagLineReset(char *line) {
    uint8_t i;
    for (i = 0; i < 17u; ++i) {
        line[i] = '\0';
    }
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static void flagAppendText(char *line, uint8_t *pos, const char *text) {
    while (*text != '\0' && *pos < 16u) {
        line[*pos] = *text;
        (*pos)++;
        text++;
    }
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static void flagAppendDecimal(char *line, uint8_t *pos, uint16_t value) {
    uint16_t divisor = 10000u;
    bool started = false;
    while (divisor > 0u && *pos < 16u) {
        uint8_t digit = (uint8_t)(value / divisor);
        if (digit != 0u || started || divisor == 1u) {
            line[*pos] = (char)('0' + digit);
            (*pos)++;
            started = true;
        }
        value = (uint16_t)(value % divisor);
        divisor /= 10u;
    }
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static void flagAppendTwoDigits(char *line, uint8_t *pos, uint16_t value) {
    if (*pos < 16u) {
        line[*pos] = (char)('0' + ((value / 10u) % 10u));
        (*pos)++;
    }
    if (*pos < 16u) {
        line[*pos] = (char)('0' + (value % 10u));
        (*pos)++;
    }
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static void flagDrawLineWhiteOnBlack(const char *line, uint8_t length, uint8_t x, uint8_t y) {
    uint8_t i;
    for (i = 0u; i < length; ++i) {
        port_drawCharWhiteOnBlack((uint8_t)line[i], (uint8_t)(x + (i * 5u)), y);
    }
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static void flagDrawHelper(uint16_t score, uint16_t totalMinutes, uint16_t seconds, uint16_t deaths) {
    char line[17];
    uint8_t pos;
#ifdef __SNES__
    flagLineReset(line);
    pos = 0;
    flagAppendText(line, &pos, "x");
    flagAppendDecimal(line, &pos, score);
    memcpy(GLOBAL_FlagOverlayLine0, line, sizeof(GLOBAL_FlagOverlayLine0));
    GLOBAL_FlagOverlayLine0Len = pos;
    flagLineReset(line);
    pos = 0;
    flagAppendTwoDigits(line, &pos, (uint16_t)(totalMinutes / 60u));
    flagAppendText(line, &pos, ":");
    flagAppendTwoDigits(line, &pos, (uint16_t)(totalMinutes % 60u));
    flagAppendText(line, &pos, ":");
    flagAppendTwoDigits(line, &pos, seconds);
    memcpy(GLOBAL_FlagOverlayLine1, line, sizeof(GLOBAL_FlagOverlayLine1));
    GLOBAL_FlagOverlayLine1Len = pos;
    flagLineReset(line);
    pos = 0;
    flagAppendText(line, &pos, "deaths:");
    flagAppendDecimal(line, &pos, deaths);
    memcpy(GLOBAL_FlagOverlayLine2, line, sizeof(GLOBAL_FlagOverlayLine2));
    GLOBAL_FlagOverlayLine2Len = pos;
    GLOBAL_FlagOverlayShow = true;
#else
    port_drawText(flagBlankLine, 32, 4);
    port_drawText(flagBlankLine, 32, 12);
    port_drawText(flagBlankLine, 32, 20);
    flagLineReset(line);
    pos = 0;
    flagAppendText(line, &pos, "BERRIES:");
    flagAppendDecimal(line, &pos, score);
    port_drawText((const unsigned char *)line, 32, 4);
    flagLineReset(line);
    pos = 0;
    flagAppendTwoDigits(line, &pos, (uint16_t)(totalMinutes / 60u));
    flagAppendText(line, &pos, ":");
    flagAppendTwoDigits(line, &pos, (uint16_t)(totalMinutes % 60u));
    flagAppendText(line, &pos, ":");
    flagAppendTwoDigits(line, &pos, seconds);
    port_drawText((const unsigned char *)line, 32, 12);
    flagLineReset(line);
    pos = 0;
    flagAppendText(line, &pos, "DEATHS:");
    flagAppendDecimal(line, &pos, deaths);
    port_drawText((const unsigned char *)line, 32, 20);
#endif
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
void flagInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->pos.x += 5;
    this->data.flag.score = GLOBAL_FruitCount;
    this->data.flag.show = false;
    this->data.flag.drawn = false;
    this->data.flag.lastSeconds = 0xFFu;
    this->data.flag.lastMinutes = 0xFFFFu;
    this->data.flag.lastDeaths = 0xFFFFu;
    GLOBAL_FlagOverlayShow = false;
    GLOBAL_FlagOverlayLine0Len = 0u;
    GLOBAL_FlagOverlayLine1Len = 0u;
    GLOBAL_FlagOverlayLine2Len = 0u;
    GLOBAL_FlagOverlayLine0[0] = '\0';
    GLOBAL_FlagOverlayLine1[0] = '\0';
    GLOBAL_FlagOverlayLine2[0] = '\0';
    this->oamTile = FLAG_SPRITE_1;
    this->oamProps = 0x32; // priority 3, palette 2
    this->flags |= OBJ_FLAG_DIRTY;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
void flagUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    bool isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 8) &&
                            (GLOBAL_PlayerData.objData.pos.x < this->pos.x + 8) &&
                            (GLOBAL_PlayerData.objData.pos.y > this->pos.y - 8) &&
                            (GLOBAL_PlayerData.objData.pos.y < this->pos.y + 8);

    switch ((GLOBAL_FrameCount / 10u) % 3u) {
        case 0u:
            this->oamTile = FLAG_SPRITE_1;
            break;
        case 1u:
            this->oamTile = FLAG_SPRITE_2;
            break;
        default:
            this->oamTile = FLAG_SPRITE_3;
            break;
    }
    this->oamProps = 0x32;

    if (!this->data.flag.show && isPlayerTouching) {
        this->data.flag.show = true;
        this->data.flag.score = GLOBAL_FruitCount;
        this->data.flag.drawn = false;
        playSoundEffect(SOUND_EFFECT_FLAG);
    }

    if (this->data.flag.show &&
        (!this->data.flag.drawn ||
         this->data.flag.lastSeconds != GLOBAL_TimerSeconds ||
         this->data.flag.lastMinutes != GLOBAL_TimerMinutes ||
         this->data.flag.lastDeaths != GLOBAL_DeathCount)) {
        this->data.flag.drawn = true;
        this->data.flag.lastSeconds = GLOBAL_TimerSeconds;
        this->data.flag.lastMinutes = GLOBAL_TimerMinutes;
        this->data.flag.lastDeaths = GLOBAL_DeathCount;
    }
    if (!this->data.flag.show) {
        GLOBAL_FlagOverlayShow = false;
    }

    this->flags |= OBJ_FLAG_DIRTY;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
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
#ifndef __NES__
            s_musicTimer = 45u;
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
void strawberryInit(uint8_t index) {
    OBJ_DATA *strawberry = &GLOBAL_OBJList[index];

#ifdef __NES__
    // Assign fixed OAM slot (4 sprites per 16x16 object)
    // Player uses slots 0-3, objects start at slot 4
    // Each object gets 4 consecutive slots based on its index
    strawberry->extraSpriteBase = 4 + (index * 4);
    strawberry->extraSpriteCount = 4;
#else
    strawberry->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    strawberry->extraSpriteCount = 0;
#endif

    strawberry->data.strawberry.startY = strawberry->pos.y;
    strawberry->data.strawberry.frameCount = 0;
    strawberry->data.strawberry.isCollected = false;
    strawberry->oamTile = STRAWBERRY_SPRITE_1;
    strawberry->oamProps = 0x32; // priority 3, palette 2
    strawberry->flags |= OBJ_FLAG_DIRTY;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
void decoTreeInit(uint8_t index) {
    initSimpleDecorSprite(index, DECO_TREE_SPRITE_1, 0x32); // priority 3, palette 2
}
#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
void decoTreeUpdate(uint8_t index) {
    updateSimpleDecorSprite(index);
}

#ifdef __NES__
PORT_DATA_BANK6
#else
PORT_DATA_BANK3
#endif
static const int8_t berry_y_positions[40] = {
    0,   1,   2,   2,   3,   4,   4,   4,   5,   5,
    5,   5,   5,   4,   4,   4,   3,   2,   2,   1,
    0,  -1,  -2,  -2,  -3,  -4,  -4,  -4,  -5,  -5,
    -5,  -5,  -5,  -4,  -4,  -4,  -3,  -2,  -2,  -1
};

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
void strawberryUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
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
            if (this->data.strawberry.bgTextY > 0u) {
                this->data.strawberry.bgTextY--;
            }
            this->pos.y = this->data.strawberry.bgTextY;
        }

        if (this->data.strawberry.frameCount > 30) {
            GLOBAL_ActiveLevel.textFlashActive = false;
            GLOBAL_ActiveLevel.swapActivePalette = true;
            GLOBAL_ActiveLevel.textScrollActive = false;
            GLOBAL_ActiveLevel.textScrollOffsetX = 0;
            GLOBAL_ActiveLevel.textScrollOffsetY = 0;
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
        collectCurrentRoomFruit();
        playSoundEffect(SOUND_EFFECT_STRAWBERRY);        
        this->data.strawberry.isCollected = true;
        this->data.strawberry.frameCount = 0;
        this->pos.x -= 4;
        this->pos.y -= 4;        
        this->data.strawberry.bgTextX = this->pos.x;
        this->data.strawberry.bgTextY = this->pos.y;
        GLOBAL_ActiveLevel.textScrollActive = true;
        GLOBAL_ActiveLevel.textScrollOffsetX = (uint8_t)(8u - ((remainderX * 2u) % 8u));
        GLOBAL_ActiveLevel.textScrollOffsetY = (uint8_t)(8u - ((remainderY * 2u) % 8u));
        this->flags |= OBJ_FLAG_DIRTY;
        return;
    }
    this->data.strawberry.frameCount += 1;
    if (this->data.strawberry.frameCount >= 40) {
        this->data.strawberry.frameCount = 0;
    }
    this->pos.y = this->data.strawberry.startY + berry_y_positions[this->data.strawberry.frameCount];

    this->flags |= OBJ_FLAG_DIRTY;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
void flyingBerryInit(uint8_t index) {
    OBJ_DATA *berry = &GLOBAL_OBJList[index];
    berry->data.flyingBerry.frameCount = 0;
    berry->data.flyingBerry.isCollected = false;
    berry->data.flyingBerry.startY = berry->pos.y;
    berry->data.flyingBerry.isFlying = false;
    berry->data.flyingBerry.sfxDelay = 8;
    berry->data.flyingBerry.speedY = 0;
    berry->data.flyingBerry.remY = 0;
    berry->oamTile = FLYING_BERRY_SPRITE_1;
    berry->oamProps = 0x32; // priority 3, palette 0
    berry->flags |= OBJ_FLAG_DIRTY;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
void flyingBerryUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t thisX = this->pos.x;
    uint8_t thisY = this->pos.y;
    uint8_t playerX = GLOBAL_PlayerData.objData.pos.x;
    uint8_t playerY = GLOBAL_PlayerData.objData.pos.y;
    uint8_t frame;
    bool isPlayerTouching = (playerX > thisX-8) &&
                            (playerX < thisX+8) &&
                            (playerY > thisY) &&
                            (playerY < thisY+8);

    if (this->data.flyingBerry.isCollected) {
        //Do the text display animation
        if (this->data.flyingBerry.frameCount == 0) {
            GLOBAL_ActiveLevel.textFlashActive = true;
            GLOBAL_ActiveLevel.swapActivePalette = true;
        }
        this->data.flyingBerry.frameCount += 1;
        if ((this->data.flyingBerry.frameCount % 4) == 0) {
            if (GLOBAL_ActiveLevel.textScrollActive && GLOBAL_ActiveLevel.textScrollOffsetY < 0xFFu) {
                GLOBAL_ActiveLevel.textScrollOffsetY += 1;
            }
            if (this->data.flyingBerry.bgTextY > 0u) {
                this->data.flyingBerry.bgTextY--;
            }
            this->pos.y = this->data.flyingBerry.bgTextY;
        }

        if (this->data.flyingBerry.frameCount > 30) {
            GLOBAL_ActiveLevel.textFlashActive = false;
            GLOBAL_ActiveLevel.swapActivePalette = true;
            GLOBAL_ActiveLevel.textScrollActive = false;
            GLOBAL_ActiveLevel.textScrollOffsetX = 0;
            GLOBAL_ActiveLevel.textScrollOffsetY = 0;
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
        collectCurrentRoomFruit();
        playSoundEffect(SOUND_EFFECT_STRAWBERRY);        
        this->data.flyingBerry.isCollected = true;
        this->data.flyingBerry.frameCount = 0;
        this->pos.x -= 4;
        this->pos.y -= 4;        
        this->data.flyingBerry.bgTextX = this->pos.x;
        this->data.flyingBerry.bgTextY = this->pos.y;
        GLOBAL_ActiveLevel.textScrollActive = true;
        GLOBAL_ActiveLevel.textScrollOffsetX = (uint8_t)(8u - ((remainderX * 2u) % 8u));
        GLOBAL_ActiveLevel.textScrollOffsetY = (uint8_t)(8u - ((remainderY * 2u) % 8u));
        this->flags |= OBJ_FLAG_DIRTY;
        return;
    }

    if (this->data.flyingBerry.isFlying) {
        int16_t moveY;
        this->data.flyingBerry.remY += this->data.flyingBerry.speedY;
        moveY = (int16_t)((this->data.flyingBerry.remY + 32768) >> 16);
        this->data.flyingBerry.remY -= INT_TO_FIXED(moveY);
        this->pos.y += moveY;
        if (this->pos.y < -16) {
            this->eType = OBJ_UNUSED;
            this->flags |= OBJ_FLAG_DIRTY;
            return;
        }
        if (this->data.flyingBerry.sfxDelay > 0) {
            this->data.flyingBerry.sfxDelay--;
            if (this->data.flyingBerry.sfxDelay == 0) {
                playSoundEffect(SOUND_EFFECT_FLYING_BERRY);
            }
        }
        if (this->data.flyingBerry.speedY > -0x00038000) {
            this->data.flyingBerry.speedY -= 0x00004000;
            if (this->data.flyingBerry.speedY < -0x00038000) {
                this->data.flyingBerry.speedY = -0x00038000;
            }
        }
    } else {
        if (GLOBAL_PlayerData.hasDashed) {
            this->data.flyingBerry.isFlying = true;
        }
        frame = this->data.flyingBerry.frameCount;
        this->pos.y = this->data.flyingBerry.startY + (berry_y_positions[frame] >> 1);
        this->data.flyingBerry.frameCount += 1;
        if (this->data.flyingBerry.frameCount >= 40) {
            this->data.flyingBerry.frameCount = 0;
        }
    }

    this->flags |= OBJ_FLAG_DIRTY;
}

#ifdef __NES__
PORT_FUNC_BANK6
#endif
// Returns the index of the object in the list
void initObject(enum eOBJType eType, int16_t x, int16_t y) {
    // Find a free slot
    uint8_t i;
    if (objectSkipsWhenFruitCollected(eType) && currentRoomFruitCollected()) {
        return;
    }
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
            if (eType == OBJ_SMOKE) {
#ifndef __NES__
                port_prg_bank_enter(2);
#endif
                smokeInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_BREAKABLE_WALL) {
#ifndef __NES__
                port_prg_bank_enter(2);
#endif
                breakableWallInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_DECO_FLOWER) {
#ifndef __NES__
                port_prg_bank_enter(2);
#endif
                flowerInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_STRAWBERRY) {
#ifndef __NES__
                port_prg_bank_enter(3);
#endif
                strawberryInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_DECO_TREE) {
#ifndef __NES__
                port_prg_bank_enter(2);
#endif
                decoTreeInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_SPRING) {
#ifndef __NES__
                port_prg_bank_enter(2);
#endif
                springInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_FLYING_BERRY) {
#ifndef __NES__
                port_prg_bank_enter(3);
#endif
                flyingBerryInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_COLLAPSE_TILE) {
#ifndef __NES__
                port_prg_bank_enter(2);
#endif
                collapseTileInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_BALLOON) {
#ifndef __NES__
                port_prg_bank_enter(4);
#endif
                balloonInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_PLATMOV_L || eType == OBJ_PLATMOV_R) {
#ifndef __NES__
                port_prg_bank_enter(2);
#endif
                platMovInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_KEY) {
#ifndef __NES__
                port_prg_bank_enter(3);
#endif
                keyInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_CHEST) {
#ifndef __NES__
                port_prg_bank_enter(3);
#endif
                chestInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_MONUMENT) {
#ifndef __NES__
                port_prg_bank_enter(4);
#endif
                monumentInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_BIG_CHEST) {
#ifndef __NES__
                port_prg_bank_enter(4);
#endif
                bigChestInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_FLAG) {
#ifndef __NES__
                port_prg_bank_enter(4);
#endif
                flagInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else if (eType == OBJ_DOUBLE_JUMP_ORB) {
#ifndef __NES__
                port_prg_bank_enter(3);
#endif
                doubleDashOrbInit(i);
#ifndef __NES__
                port_prg_bank_leave();
#endif
            } else {
                GLOBAL_OBJList[i].eType = OBJ_UNUSED;
                GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
                GLOBAL_OBJList[i].extraSpriteCount = 0;
            }
            return;
        }
    }
}


#ifdef __NES__
PORT_FUNC_BANK6
#endif
static void clearObjectDirtyFlag(uint8_t index)
{
    GLOBAL_OBJList[index].flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK2
#endif
static void processObjectBank2(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
#ifdef __NES__
    uint8_t frameSlot = (uint8_t)((GLOBAL_FrameCount >> 1) & 0x01);
#endif

    if (obj->eType == OBJ_COLLAPSE_TILE) {
        collapseTileUpdate(index);
    } else if (obj->eType == OBJ_SPRING) {
        springUpdate(index);
    } else if (obj->eType == OBJ_PLATMOV_R || obj->eType == OBJ_PLATMOV_L) {
        platMovUpdate(index);
    } else if (obj->eType == OBJ_BREAKABLE_WALL) {
        breakableWallUpdate(index);
    } else if (obj->eType == OBJ_SMOKE) {
#ifdef __NES__
        if (!frameSlot) smokeUpdate(index);
#else
        smokeUpdate(index);
#endif
    } else if (obj->eType == OBJ_DECO_TREE) {
#ifndef __NES__
        decoTreeUpdate(index);
#endif
    } else if (obj->eType == OBJ_DECO_FLOWER) {
#ifndef __NES__
        flowerUpdate(index);
#endif
    } else {
        return;
    }

    if ((obj->flags & OBJ_FLAG_DIRTY) != 0u) {
        if (obj->eType == OBJ_COLLAPSE_TILE) {
            port_buildCollapseTile(index);
        } else if (obj->eType == OBJ_SPRING) {
            port_buildSpring(index);
        } else if (obj->eType == OBJ_PLATMOV_R || obj->eType == OBJ_PLATMOV_L) {
            port_buildPlatMov(index);
        } else if (obj->eType == OBJ_BREAKABLE_WALL) {
            port_buildBreakableWall(index);
        } else if (obj->eType == OBJ_SMOKE) {
            port_buildSmoke(index);
        } else if (obj->eType == OBJ_DECO_TREE || obj->eType == OBJ_DECO_FLOWER) {
            port_buildStaticDecor(index);
        }
    }
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK3
#endif
static void processObjectBank3(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
#ifdef __NES__
    uint8_t frameSlot = (uint8_t)((GLOBAL_FrameCount >> 1) & 0x01);
#endif

    if (obj->eType == OBJ_CHEST) {
        chestUpdate(index);
    } else if (obj->eType == OBJ_KEY) {
#ifdef __NES__
        if (!frameSlot) keyUpdate(index);
        else obj->flags |= OBJ_FLAG_DIRTY;
#else
        keyUpdate(index);
#endif
    } else if (obj->eType == OBJ_DOUBLE_JUMP_ORB) {
#ifdef __NES__
        if (!frameSlot) doubleDashOrbUpdate(index);
        else obj->flags |= OBJ_FLAG_DIRTY;
#else
        doubleDashOrbUpdate(index);
#endif
    } else if (obj->eType == OBJ_STRAWBERRY) {
#ifdef __NES__
        if (!frameSlot) strawberryUpdate(index);
        else obj->flags |= OBJ_FLAG_DIRTY;
#else
        strawberryUpdate(index);
#endif
    } else if (obj->eType == OBJ_FLYING_BERRY) {
#ifdef __NES__
        if (!frameSlot) flyingBerryUpdate(index);
        else obj->flags |= OBJ_FLAG_DIRTY;
#else
        flyingBerryUpdate(index);
#endif
    } else {
        return;
    }

    if ((obj->flags & OBJ_FLAG_DIRTY) != 0u) {
        if (obj->eType == OBJ_CHEST) {
            port_buildChest(index);
        } else if (obj->eType == OBJ_KEY) {
            port_buildKey(index);
        } else if (obj->eType == OBJ_DOUBLE_JUMP_ORB) {
            port_buildDoubleDashOrb(index);
        } else if (obj->eType == OBJ_STRAWBERRY) {
            port_buildStrawberry(index);
        } else if (obj->eType == OBJ_FLYING_BERRY) {
            port_buildFlyingBerry(index);
        }
    }
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK4
#endif
static void processObjectBank4(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
#ifdef __NES__
    uint8_t frameSlot = (uint8_t)((GLOBAL_FrameCount >> 1) & 0x01);
#endif

    if (obj->eType == OBJ_BALLOON) {
#ifdef __NES__
        if (!frameSlot) balloonUpdate(index);
        else obj->flags |= OBJ_FLAG_DIRTY;
#else
        balloonUpdate(index);
#endif
    } else if (obj->eType == OBJ_MONUMENT) {
        monumentUpdate(index);
    } else if (obj->eType == OBJ_BIG_CHEST) {
        bigChestUpdate(index);
    } else if (obj->eType == OBJ_FLAG) {
        flagUpdate(index);
    } else {
        return;
    }

    if ((obj->flags & OBJ_FLAG_DIRTY) != 0u) {
        if (obj->eType == OBJ_BALLOON) {
            port_buildBalloon(index);
        } else if (obj->eType == OBJ_MONUMENT) {
            port_buildMonument(index);
        } else if (obj->eType == OBJ_BIG_CHEST) {
            port_buildBigChest(index);
        } else if (obj->eType == OBJ_FLAG) {
            port_buildFlag(index);
        }
    }
}

#ifdef __NES__
PORT_FUNC_BANK6
#endif
static void processObject(uint8_t index)
{
    OBJ_DATA *obj = &GLOBAL_OBJList[index];
    if (obj->eType == OBJ_UNUSED) {
        if (obj->flags & OBJ_FLAG_DIRTY) {
            if (index == 0U) {
                clearObjectDirtyFlag(index);
            } else {
                port_buildUnused(index);
            }
        }
    } else if (obj->eType == OBJ_COLLAPSE_TILE || obj->eType == OBJ_SPRING ||
               obj->eType == OBJ_PLATMOV_R || obj->eType == OBJ_PLATMOV_L ||
               obj->eType == OBJ_BREAKABLE_WALL || obj->eType == OBJ_SMOKE ||
               obj->eType == OBJ_DECO_TREE || obj->eType == OBJ_DECO_FLOWER) {
#ifndef __NES__
        port_prg_bank_enter(2);
#endif
        processObjectBank2(index);
#ifndef __NES__
        port_prg_bank_leave();
#endif
    } else if (obj->eType == OBJ_CHEST || obj->eType == OBJ_KEY ||
               obj->eType == OBJ_DOUBLE_JUMP_ORB || obj->eType == OBJ_STRAWBERRY ||
               obj->eType == OBJ_FLYING_BERRY) {
#ifndef __NES__
        port_prg_bank_enter(3);
#endif
        processObjectBank3(index);
#ifndef __NES__
        port_prg_bank_leave();
#endif
    } else if (obj->eType == OBJ_BALLOON || obj->eType == OBJ_MONUMENT ||
               obj->eType == OBJ_BIG_CHEST || obj->eType == OBJ_FLAG) {
#ifndef __NES__
        port_prg_bank_enter(4);
#endif
        processObjectBank4(index);
#ifndef __NES__
        port_prg_bank_leave();
#endif
    } else if (obj->flags & OBJ_FLAG_DIRTY) {
        clearObjectDirtyFlag(index);
    }
}

void updateAllObjects(void) {
    uint8_t i;
    port_beginSpriteBuild(&GLOBAL_PlayerData);
#ifdef __NES__
    port_prg_bank_enter(6);
#endif
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        OBJ_DATA *obj = &GLOBAL_OBJList[i];
        if (obj->eType == OBJ_UNUSED && (obj->flags & OBJ_FLAG_DIRTY)) {
            if (i == 0U) {
                clearObjectDirtyFlag(i);
            } else {
                port_buildUnused(i);
            }
        }
    }
#ifndef __NES__
    port_prg_bank_enter(2);
#endif
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        processObjectBank2(i);
    }
#ifndef __NES__
    port_prg_bank_leave();
#endif
#ifndef __NES__
    port_prg_bank_enter(3);
#endif
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        processObjectBank3(i);
    }
#ifndef __NES__
    port_prg_bank_leave();
#endif
#ifndef __NES__
    port_prg_bank_enter(4);
#endif
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        processObjectBank4(i);
    }
#ifndef __NES__
    port_prg_bank_leave();
#endif
#ifdef __NES__
    port_prg_bank_leave();
#endif
    port_finishSpriteBuild();
}

// Sprite-only rebuild: no game logic, just push existing positions to OAM
// with fresh flicker rotation. Keeps OAM rotation at 60Hz.
void rebuildAllSprites(void) {
    uint8_t i;
    port_beginSpriteBuild(&GLOBAL_PlayerData);
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
    port_finishSpriteBuild();
}


#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK1
#endif
void playerInit(struct sPlayerData* this);

// onVblank is in fixed bank (bank 0), not bank 6
static bool onVblank(void);
static void runGameplayFrame(void);
static void refreshGameplaySprites(void);
static __attribute__((noinline)) bool shouldRunGameplayFrame(void);

#ifndef __NES__
    PORT_FUNC_BANK7
    static void updateRoomMusic(uint16_t roomID) {
        switch (roomID) {
            case 1:
                port_audioPlayMusic(MUSIC_PATTERN_LEVEL_START);
                break;
            case 12: // entered room (3,1), after leaving (2,1)
                port_audioPlayMusic(MUSIC_PATTERN_WIDE_OPEN);
                break;
            case 13: // entered room (4,1), after leaving (3,1)
                port_audioPlayMusic(MUSIC_PATTERN_WIND);
                break;
            case 22: // entered room (5,2), after leaving (4,2)
                port_audioPlayMusic(MUSIC_PATTERN_WIDE_OPEN);
                break;
            case 31: // entered room (6,3), after leaving (5,3)
                port_audioPlayMusic(MUSIC_PATTERN_WIDE_OPEN);
                break;
            default:
                break;
        }
    }
#endif

void LoadRoomData(uint16_t roomID) {
    uint8_t i = 0;

    GLOBAL_ActiveLevel.currentRoomID = roomID;
    GLOBAL_ActiveLevel.roomSizeX = 16;
    GLOBAL_ActiveLevel.roomSizeY = 16;
    GLOBAL_ActiveLevel.textChanged = false;
    GLOBAL_ActiveLevel.swapCloudPal = false;
    GLOBAL_ActiveLevel.swapActivePalette = true;
    GLOBAL_ActiveLevel.textFlashActive = false;
    GLOBAL_MonumentTextDisplayed = false;
    GLOBAL_MonumentCurLineCharCount = 0;
    GLOBAL_MonumentCurLineNum = 0;
    s_monumentTextTick = 0;

    port_LoadRoomData(roomID);

#ifdef __NES__
    port_prg_bank_enter(6);
#else
    port_prg_bank_enter(1);
#endif
    playerInit(&GLOBAL_PlayerData);
    port_prg_bank_leave();
    port_updatePlayerSprite(&GLOBAL_PlayerData);

    port_beginSpriteBuild(&GLOBAL_PlayerData);
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; ++i) {
        processObject(i);
    }
    port_finishSpriteBuild();
#ifndef __NES__
    port_prg_bank_enter(7);
    updateRoomMusic(roomID);
    port_prg_bank_leave();
#endif
}

void LoadNextRoom(void) {
    GLOBAL_ActiveLevel.currentRoomID++;
    LoadRoomData(GLOBAL_ActiveLevel.currentRoomID);
}

#if !defined(NDEBUG) && defined(__SNES__)
static bool debugHandleLevelSkip(uint8_t inputState)
{
    static uint8_t s_prevDebugSkipInput = 0u;
    uint8_t pressed = (uint8_t)(GLOBAL_InputLo & (PORT_INPUT_L_MASK | PORT_INPUT_R_MASK));
    uint8_t newlyPressed = (uint8_t)(pressed & (uint8_t)~s_prevDebugSkipInput);
    uint16_t roomID = GLOBAL_ActiveLevel.currentRoomID;

    (void)inputState;
    s_prevDebugSkipInput = pressed;

    if ((newlyPressed & PORT_INPUT_L_MASK) != 0u && roomID > 1u) {
        LoadRoomData((uint16_t)(roomID - 1u));
        return true;
    } else if ((newlyPressed & PORT_INPUT_R_MASK) != 0u && roomID < GLOBAL_LEVEL_COUNT) {
        LoadRoomData((uint16_t)(roomID + 1u));
        return true;
    }

    return false;
}
#else
#define debugHandleLevelSkip(inputState) false
#endif

int main(void){
    port_init();
    resetRunState();
#ifndef __NES__
    s_inTitleScreen = true;
    port_setTitleMode(true);
    port_showTitleScreen();
    port_audioPlayMusic(MUSIC_PATTERN_TITLE);
#else
    GLOBAL_ActiveLevel.currentRoomID = 1;
    LoadRoomData(GLOBAL_ActiveLevel.currentRoomID);
#endif

    s_lastGameplayFrame = (uint16_t)(GLOBAL_FrameCount - 1u);
    for (;;) {
        bool gameplayReady = onVblank();

        if (!gameplayReady) {
            s_lastGameplayFrame = (uint16_t)(GLOBAL_FrameCount - 1u);
            continue;
        }
        if (shouldRunGameplayFrame()) {
            runGameplayFrame();
        } else {
            refreshGameplaySprites();
        }
    }
}


#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK1
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK1
#endif
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
    this->hasDashed = false;
    
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK1
#endif
bool isDeathAtPoint(int16_t x, int16_t y, int16_t w, int16_t h, fixed_t xspd, fixed_t yspd) {
    int16_t tx0 = x >> 3;
    int16_t ty0 = y >> 3;
    int16_t tx1 = (x + w - 1) >> 3;
    int16_t ty1 = (y + h - 1) >> 3;
    int16_t tx;
    int16_t ty;

    if (tx0 < 0) tx0 = 0;
    if (ty0 < 0) ty0 = 0;
    if (tx1 > 15) tx1 = 15;
    if (ty1 > 15) ty1 = 15;
    if (tx0 > 15 || ty0 > 15 || tx1 < 0 || ty1 < 0) {
        return false;
    }

    for (tx = tx0; tx <= tx1; ++tx) {
        for (ty = ty0; ty <= ty1; ++ty) {
            uint8_t flags = GLOBAL_ActiveLevel.collisionFlagsArr[((uint8_t)ty << 4) + (uint8_t)tx];
            if ((flags & 0x04u) && (((uint8_t)(y + h - 1) & 7u) >= 6u || (y + h) == (ty * 8 + 8)) && yspd >= 0) {
                return true;
            }
            if ((flags & 0x08u) && (((uint8_t)y & 7u) <= 2u) && yspd <= 0) {
                return true;
            }
            if ((flags & 0x10u) && (((uint8_t)x & 7u) <= 2u) && xspd <= 0) {
                return true;
            }
            if ((flags & 0x20u) && (((uint8_t)(x + w - 1) & 7u) >= 6u || (x + w) == (tx * 8 + 8)) && xspd >= 0) {
                return true;
            }
        }
    }
    return false;
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK1
#endif
static bool OBJ_isDeathAt(struct sPlayerData* this, int16_t xOffset, int16_t yOffset) {
    int16_t x = this->objData.pos.x + xOffset;
    int16_t y = this->objData.pos.y + yOffset;

    // Pass hitbox bounds {1,3,6,5} instead of full sprite bounds
    return isDeathAtPoint(x + 1, y + 3, 6, 5, this->spd.x, this->spd.y);
}

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK1
#endif
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

#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK1
#endif
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
#ifdef __NES__
PORT_FUNC_BANK6
#else
PORT_FUNC_BANK1
#endif
void playerUpdate(struct sPlayerData* this) {
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
                    playSoundEffect(SOUND_EFFECT_WALL_JUMP);
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
            this->hasDashed = true;

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
        GLOBAL_DeathCount++;
        this->objData.pos.y = 128; //offscreen
        GLOBAL_ActiveLevel.shakeFrames = 10;
        playerInit(this);
        return;
    }

    // next level (before clamping — player must be able to reach y < -4)
    if (this->objData.pos.y < -4 && GLOBAL_ActiveLevel.currentRoomID < 31) {
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


// Simple 16-bit LCG. Smaller than the old lookup table and sufficient here.
static uint16_t global_randSeed = 0xBEEFu;
#ifdef __NES__
PORT_FUNC_BANK6
#endif
uint16_t my_rand() {
    global_randSeed = (uint16_t)(global_randSeed * 109u + 89u);
    return global_randSeed;
}

// Returns a random integer in [min, max]
#ifdef __NES__
PORT_FUNC_BANK6
#endif
int16_t randint16(int16_t min, int16_t max) {
    uint16_t range;
    uint16_t randomValue;
    if (max <= min) return min; // handle edge case
    range = (uint16_t)(max - min) + 1;
    randomValue = my_rand();
    return (int16_t)((randomValue % range) + min);
}

static bool onVblank(void) {
    //Start of vblank critical code
    port_vblank();

    // Music/audio update — target-specific backend.
    port_audioUpdate();
#ifndef __NES__
    if (s_inTitleScreen) {
        GLOBAL_InputState = port_getInputs();
        port_resetSprites();
        if (s_titleStartTimer > 0) {
            s_titleStartTimer--;
            if (s_titleStartTimer == 0) {
                s_inTitleScreen = false;
                port_setTitleMode(false);
                port_showGameplayScreen();
                GLOBAL_ActiveLevel.currentRoomID = 1;
                GLOBAL_TimerFrames = 0;
                GLOBAL_TimerSeconds = 0;
                GLOBAL_TimerMinutes = 0;
                LoadRoomData(1);
            }
        } else if ((GLOBAL_InputState & (PORT_INPUT_B_MASK | PORT_INPUT_Y_MASK | PORT_INPUT_START_MASK)) != 0u ||
                   (GLOBAL_InputLo & (PORT_INPUT_A_MASK | PORT_INPUT_X_MASK)) != 0u) {
            port_audioStopAll();
            playSoundEffect(SOUND_EFFECT_TITLE_START);
            s_titleStartTimer = 50;
        }
        return false;
    }
#endif

    return true;
}

static __attribute__((noinline)) bool shouldRunGameplayFrame(void)
{
    uint16_t currentFrame = GLOBAL_FrameCount;
    if ((uint16_t)(currentFrame - s_lastGameplayFrame) < 2u) {
        return false;
    }
    s_lastGameplayFrame = currentFrame;
    return true;
}

static void runGameplayFrame(void) {
    if (GLOBAL_ActiveLevel.currentRoomID <= GLOBAL_FRUIT_COUNT) {
        GLOBAL_TimerFrames++;
        if (GLOBAL_TimerFrames >= 30u) {
            GLOBAL_TimerFrames = 0u;
            GLOBAL_TimerSeconds++;
            if (GLOBAL_TimerSeconds >= 60u) {
                GLOBAL_TimerSeconds = 0u;
                GLOBAL_TimerMinutes++;
            }
        }
    }
#ifndef __NES__
    if (s_musicTimer > 0u) {
        s_musicTimer--;
        if (s_musicTimer == 0u) {
            port_audioPlayMusic(MUSIC_PATTERN_ORB);
        }
    }
#endif
    if (GLOBAL_FreezeFrames > 0) {
        GLOBAL_FreezeFrames--;
        return;
    }
    if (GLOBAL_ActiveLevel.shakeFrames > 0) {
        GLOBAL_ActiveLevel.shakeFrames--;
    }

    GLOBAL_InputState = port_getInputs();
    if (debugHandleLevelSkip(GLOBAL_InputState)) {
        return;
    }
#ifdef __NES__
    port_prg_bank_enter(6);
#else
    port_prg_bank_enter(1);
#endif
    playerUpdate(&GLOBAL_PlayerData);
    port_prg_bank_leave();
#ifndef __NES__
    // Keep sprite transforms aligned with the same camera state used for next vblank.
    port_prg_bank_enter(5);
    syncCameraFromPlayer();
    port_prg_bank_leave();
#endif
    port_updatePlayerSprite(&GLOBAL_PlayerData);
    updateAllObjects();
    port_renderTextOverlays();
    port_levelAnimAdvance();
}

static void refreshGameplaySprites(void) {
    if (GLOBAL_FreezeFrames > 0) {
        return;
    }
    port_updatePlayerSprite(&GLOBAL_PlayerData);
    rebuildAllSprites();
}
