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

// Per-frame gravity step in fix16 (0.21). Used by playerUpdate, and by
// spring/breakable-wall bounces to pre-compensate for the gravity that
// playerUpdate will apply on the next frame before moving the player —
// without this, externally-set spd.y loses one gravity step on first move.
#define FP_GRAVITY 0x000035C2

//Prototypes
int16_t randint16(int16_t min, int16_t max);

//Basic math functions that a compiler should have
PORT_FUNC_NES6
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
    static bool s_titleStartActive = false;  // true between press and begin_game (timer 0 is mid-sequence, can't gate on timer alone)
#ifndef __NES__
    // Konami-code easter-egg state. On title screen, pressing
    // ↑↓↑↓←→←→Start shows a 10-second dedication screen before the
    // game starts normally.
    static uint8_t  s_cheatStep = 0u;
    static uint8_t  s_prevTitleInput = 0u;
    static uint16_t s_dedicationTimer = 0u;
    #define CHEAT_STEP_COUNT 9u
#endif

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

PORT_FUNC_NES6
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

#ifdef __SNES__
// Room-title overlay. Drawn at room load, cleared ~30 gameplay ticks later.
// Timer at (4, 4) is MM:SS — 5 chars span cells x=0..3, ending before the
// flag overlay's cell range (x=4..12). HH:MM:SS would push into cells 4..5
// and corrupt the shared BG1 slot pool in the summit room.
#define ROOM_TITLE_VISIBLE_TICKS  30u
#define ROOM_TITLE_OLD_SITE_ROOM  12u  // ccleste level_index 11 == room.x=3,y=1
#define ROOM_TITLE_SUMMIT_ROOM    31u  // ccleste level_index 30
#define ROOM_TITLE_BOX_X          24u
// port_drawTextWhiteOnBlack adds PORT_PICO8_FONT_CELL_Y_OFFSET (4 SNES px =
// 2 pico8 px) internally, so pass 60 to land glyphs at ccleste's pico8 y=62.
#define ROOM_TITLE_TEXT_Y         60u
#define ROOM_TITLE_TIMER_X        4u
#define ROOM_TITLE_TIMER_Y        4u
#define ROOM_TITLE_TIMER_LEN      5u   // "MM:SS"
#define ROOM_TITLE_ALT_MAX_LEN    8u   // "old site"
extern bool s_pendingDisplayEnable;
static uint8_t s_roomTitleClearTimer = 0u;
#endif



enum eMovingPlatformDir {MOVING_PLATFORM_DIR_IDLE = 0, MOVING_PLATFORM_DIR_LEFT = 1, MOVING_PLATFORM_DIR_RIGHT = 2};

struct sActiveLevelData GLOBAL_ActiveLevel;

struct sPlayerData GLOBAL_PlayerData;

#ifdef __SNES__
extern uint8_t GLOBAL_InputLo;
#endif

void initObject(enum eOBJType eType, int16_t x, int16_t y);

PORT_FUNC_NES6
static uint8_t currentFruitIndex(void) {
    uint16_t roomID = GLOBAL_ActiveLevel.currentRoomID;
    if (roomID == 0u || roomID > GLOBAL_FRUIT_COUNT) {
        return 0xFFu;
    }
    return (uint8_t)(roomID - 1u);
}

PORT_FUNC_NES6
static bool gotFruitAt(uint8_t fruitIndex) {
    if (fruitIndex >= GLOBAL_FRUIT_COUNT) {
        return false;
    }
    return (GLOBAL_GotFruitBits[fruitIndex >> 3] & (uint8_t)(1u << (fruitIndex & 7u))) != 0u;
}

PORT_FUNC_NES6
static bool currentRoomFruitCollected(void) {
    return gotFruitAt(currentFruitIndex());
}

PORT_FUNC_NES6
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

PORT_FUNC_NES6
static bool objectSkipsWhenFruitCollected(enum eOBJType eType) {
    return eType == OBJ_STRAWBERRY || eType == OBJ_FLYING_BERRY ||
           eType == OBJ_BREAKABLE_WALL || eType == OBJ_KEY ||
           eType == OBJ_CHEST;
}

PORT_FUNC_NES6
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

PORT_FUNC_NES6_SNES2
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

PORT_FUNC_NES6_SNES2
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

#ifdef __SNES__
extern union uOAMCopy GLOBAL_OAMCopy;
// Death particles are managed as a stand-alone subsystem rather than OBJ
// entries, matching ccleste's separate dead_particles[] array. Lives outside
// the OBJ list so it survives playerInit's port_resetSprites + slot reinit,
// and keeps ticking during the post-death PLAYER_SPAWN hop (where the
// updateAllObjects gate would otherwise freeze it).
//
// To avoid spending new near-RAM BSS (the project enforces a 128-byte heap
// headroom), state is parked in GLOBAL_OBJList[0]'s otherwise-unused fields.
// Slot 0 is the "player placeholder" — its eType is always OBJ_UNUSED, it's
// skipped by initObject (loop starts at 1) and by playerInit's clearing loop,
// and the various processObject paths bail out on UNUSED. So pos.x/pos.y and
// data.smoke.frameCount are free scratch (5 bytes).
//
// Rendering uses fixed OAM slots DEAD_PARTICLE_OAM_BASE..+7 (above the player,
// hair, and below the dynamic-extras allocator — see PORT_EXTRA_SPRITE_START
// in snes.c). port_resetSprites clears Y=240 across all OAM; tickDeadParticles
// re-writes these slots on the same gameplay frame, so death does not flicker
// the particles.
// Tile slots placed in the gap between hair (0xC4-C7 / 0xD4-D7) and the
// score-1000 sprite (0xC0-C3 / 0xD0-D3). Previously these lived at 0xCE-D3
// which clobbered the score's bottom-row tiles.
#define DEAD_PARTICLE_BIG_CHARNUM    0xC8u
#define DEAD_PARTICLE_MED_CHARNUM    0xCAu
#define DEAD_PARTICLE_SMALL_CHARNUM  0xCCu
#define DEAD_PARTICLE_PROPS_PINK     0x3Au
#define DEAD_PARTICLE_PROPS_PEACH    0x3Cu
#define DEAD_PARTICLE_COUNT          8u
#define DEAD_PARTICLE_OAM_BASE       35u
#define DEAD_PARTICLE_LIFETIME       10u

// State aliases (zero BSS — these are slot-0 scratch fields).
#define DEAD_PARTICLES_T       (GLOBAL_OBJList[0].data.smoke.frameCount)
#define DEAD_PARTICLES_BASE_X  (GLOBAL_OBJList[0].pos.x)
#define DEAD_PARTICLES_BASE_Y  (GLOBAL_OBJList[0].pos.y)

// ccleste celeste.c:1582-1604 velocity table: sin/cos of (dir/8) * 3 for
// dir = 0..7. Precomputed as signed int8 so no runtime trig / __mulhi3.
static const int8_t s_deadParticleVelX[DEAD_PARTICLE_COUNT] = { 0,  2,  3,  2,  0, -2, -3, -2 };
static const int8_t s_deadParticleVelY[DEAD_PARTICLE_COUNT] = { 3,  2,  0, -2, -3, -2,  0,  2 };

// Forward decls. Particle logic lives in bank 2 (frees fixed-mirror; bank 2
// has spare space after the OBJ_DEAD_PARTICLE dispatch was removed).
static void spawnDeadParticlesViaBank2(int16_t cx, int16_t cy);
static void tickDeadParticlesViaBank2(void);

PORT_FUNC_NES6_SNES2
void spawnDeadParticles(int16_t cx, int16_t cy) {
    // ccleste centres the burst on (cx, cy); our 16x16 tiles anchor top-left,
    // so back off by 4 pico-px to centre.
    DEAD_PARTICLES_BASE_X = (int16_t)(cx - 4);
    DEAD_PARTICLES_BASE_Y = (int16_t)(cy - 4);
    DEAD_PARTICLES_T      = DEAD_PARTICLE_LIFETIME;
}

// Multiply an int8 velocity by a uint8 elapsed (0..LIFETIME). Repeated
// addition stays small with elapsed ≤ 10 and avoids dragging in __mulhi3,
// which has historically caused codegen issues under LTO (see
// project_mulhi3_bug.md).
static int16_t deadParticleDisp(uint8_t elapsed, int8_t vel) {
    int16_t result = 0;
    while (elapsed != 0u) {
        result = (int16_t)(result + (int16_t)vel);
        elapsed--;
    }
    return result;
}

PORT_FUNC_NES6_SNES2
void tickDeadParticles(void) {
    uint8_t t;
    uint8_t elapsed;
    uint8_t i;
    uint8_t tile;
    int16_t baseX;
    int16_t baseY;
    if (DEAD_PARTICLES_T == 0u) {
        return;  // Inactive — slots already hidden from a prior expire.
    }
    DEAD_PARTICLES_T--;
    t = DEAD_PARTICLES_T;
    if (t == 0u) {
        // Final tick: expire all particles, hide their OAM slots.
        for (i = 0u; i < DEAD_PARTICLE_COUNT; ++i) {
            GLOBAL_OAMCopy.arr.OAMArray[DEAD_PARTICLE_OAM_BASE + i].OBJY = 240u;
        }
        return;
    }
    elapsed = (uint8_t)(DEAD_PARTICLE_LIFETIME - t);
    baseX = DEAD_PARTICLES_BASE_X;
    baseY = DEAD_PARTICLES_BASE_Y;
    // ccleste's `dt = ceil(t/5)` collapses to two sizes (big for t≥6, small
    // for t≤5); our tile set keeps a med step at t∈[5,9] for smoother fade.
    if (t >= 5u) {
        tile = DEAD_PARTICLE_MED_CHARNUM;
    } else {
        tile = DEAD_PARTICLE_SMALL_CHARNUM;
    }
    (void)DEAD_PARTICLE_BIG_CHARNUM;  // tile sizes start at MED — see comment.

    for (i = 0u; i < DEAD_PARTICLE_COUNT; ++i) {
        uint8_t slot = (uint8_t)(DEAD_PARTICLE_OAM_BASE + i);
        int16_t physX = (int16_t)(baseX + deadParticleDisp(elapsed, s_deadParticleVelX[i]));
        int16_t physY = (int16_t)(baseY + deadParticleDisp(elapsed, s_deadParticleVelY[i]));

        uint16_t table2Index = (uint16_t)(slot >> 2);
        uint8_t shift = (uint8_t)((slot & 3u) << 1);
        uint8_t currentByte = GLOBAL_OAMCopy.arr.OAMTable2[table2Index];
        uint8_t value = (uint8_t)(0x02u << shift);  // sizeBit=1, xBit=0
        GLOBAL_OAMCopy.arr.OAMTable2[table2Index] =
            (uint8_t)((currentByte & (uint8_t)~(uint8_t)(0x03u << shift)) | value);

        int16_t screenY = (int16_t)((int16_t)(physY << 1) - (int16_t)GLOBAL_ScrollBG2Y - 1);
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJX = (uint8_t)(physX << 1);
        GLOBAL_OAMCopy.arr.OAMArray[slot].OBJY = (uint8_t)screenY;
        GLOBAL_OAMCopy.arr.OAMArray[slot].CHARNUM = tile;
        GLOBAL_OAMCopy.arr.OAMArray[slot].PROPERTIES =
            (uint8_t)((t & 1u) ? DEAD_PARTICLE_PROPS_PEACH : DEAD_PARTICLE_PROPS_PINK);
    }
}

// Fixed-mirror trampolines. Bank switches must originate from fixed-mirror
// code (the JSR return PC after a bank swap only lands correctly when the
// saved low PC matches the new bank's code). Callers in other banks (e.g.
// playerUpdate in bank 1) and in fixed mirror both go through these stubs.
PORT_NOINLINE
static void spawnDeadParticlesViaBank2(int16_t cx, int16_t cy) {
    port_prg_bank_enter(2);
    spawnDeadParticles(cx, cy);
    port_prg_bank_leave();
}

PORT_NOINLINE
static void tickDeadParticlesViaBank2(void) {
    port_prg_bank_enter(2);
    tickDeadParticles();
    port_prg_bank_leave();
}
#endif


#define COLLISION_FLAG_INDEX_FROM_TILE_XY(x,y) ((x) + (y) * 16)
PORT_FUNC_NES6_SNES2
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

PORT_FUNC_NES6_SNES2
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
        // Pre-compensate for one frame of gravity (see spring fix at springUpdate).
        GLOBAL_PlayerData.spd.y = FLOAT_TO_FIXED(-1.5f) - FP_GRAVITY;
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
        // Restore the 4 tiles to their original ROM-loaded collision values.
        // Plain `&= ~0x01` would wipe the solid bit off any static ground tile
        // the wall overlapped (e.g. level 17, where the wall sits on ground).
        {
            uint16_t i0 = COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX,     tileY);
            uint16_t i1 = COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX + 1, tileY);
            uint16_t i2 = COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX,     tileY + 1);
            uint16_t i3 = COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX + 1, tileY + 1);
            GLOBAL_ActiveLevel.collisionFlagsArr[i0] = GLOBAL_ActiveLevel.collisionFlagsReset[i0];
            GLOBAL_ActiveLevel.collisionFlagsArr[i1] = GLOBAL_ActiveLevel.collisionFlagsReset[i1];
            GLOBAL_ActiveLevel.collisionFlagsArr[i2] = GLOBAL_ActiveLevel.collisionFlagsReset[i2];
            GLOBAL_ActiveLevel.collisionFlagsArr[i3] = GLOBAL_ActiveLevel.collisionFlagsReset[i3];
        }
        return;
    }
    wall->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_NES6_SNES2
static void initSimpleDecorSprite(uint8_t index, uint8_t tile, uint8_t properties) {
    OBJ_DATA *decor = &GLOBAL_OBJList[index];
    decor->oamTile = tile;
    decor->oamProps = properties;
    decor->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_NES6_SNES2
static void updateSimpleDecorSprite(uint8_t index) {
    GLOBAL_OBJList[index].flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_NES6_SNES2
void flowerInit(uint8_t index) {
    initSimpleDecorSprite(index, FLOWER_SPRITE_1, 0x32); // priority 3, palette 1
}

PORT_FUNC_NES6_SNES2
void flowerUpdate(uint8_t index) {
    updateSimpleDecorSprite(index);
}

enum eCollapseTileState {
    COLLAPSE_TILE_STATE_IDLE = 0,
    COLLAPSE_TILE_STATE_COLLAPSING = 1,
    COLLAPSE_TILE_STATE_HIDDEN = 2,
};

PORT_FUNC_NES6_SNES2
void collapseTileInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    uint8_t tileX = GLOBAL_OBJList[index].pos.x / 8;
    uint8_t tileY = (GLOBAL_OBJList[index].pos.y+1) / 8;
    uint8_t i;
    GLOBAL_ActiveLevel.collisionFlagsArr[COLLISION_FLAG_INDEX_FROM_TILE_XY(tileX, tileY)]     |= 0x01; //Set the solid flag
    this->data.collapseTile.state = COLLAPSE_TILE_STATE_IDLE;
    this->data.collapseTile.linkedSpringIndex = -1;

    // Collapse tiles render as background tiles, not sprites, so no OAM slots are needed

    // Link to an adjacent spring if it's already been initialized; otherwise
    // springInit will close the link from its side when it runs.
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

PORT_FUNC_NES6_SNES2
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

PORT_FUNC_NES6_SNES2
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

PORT_FUNC_NES6_SNES2
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
        // Pre-compensate for one frame of gravity: playerUpdate applies gravity
        // before moving next frame, so set spd.y = -3 - gravity so that after
        // gravity the actual move uses -3 (matches ccleste's move-then-gravity order).
        GLOBAL_PlayerData.spd.y = INT_TO_FIXED(-3) - FP_GRAVITY;
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
PORT_FUNC_NES6_SNES4
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
PORT_FUNC_NES6_SNES4
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


PORT_FUNC_NES6_SNES4
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

PORT_FUNC_NES6_SNES2
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

PORT_FUNC_NES6_SNES2
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

    // Wrap with a 128-game-pixel period (= 256 screen-px) so the screen X — which
    // is rendered via natural uint8 wrap of pos.x*2 — stays continuous across the
    // snap. ccleste's [-16, 128] range has period 144 and would teleport on the
    // SNES; this modular wrap matches the vbcc-era smooth-wrap behaviour at the
    // cost of ~16 frames of cloud-step time per cycle on the right edge.
    if (this->pos.x < -16) {
        this->pos.x += 128;
    } else if (this->pos.x > 111) {
        this->pos.x -= 128;
    }


    this->flags |= OBJ_FLAG_DIRTY;
}

enum eKeyState {KEY_STATE_1 = 0, KEY_STATE_2 = 1, KEY_STATE_3 = 2, KEY_STATE_4 = 3};

PORT_FUNC_NES6_SNES3
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

PORT_FUNC_NES6_SNES3
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

PORT_FUNC_NES6_SNES3
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

PORT_FUNC_NES6_SNES3
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

PORT_FUNC_NES6_SNES4
static bool monumentReserveHelper(void);
#ifdef __SNES__
static bool s_monumentTextReserved = false;
#endif

PORT_FUNC_NES6_SNES4
void monumentInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->extraSpriteCount = 0;
    this->oamTile = MONUMENT_SPRITE_1;
    this->oamProps = 0x36; // priority 3, palette 5
    this->flags |= OBJ_FLAG_DIRTY;
    // Update nametable to show the monument tile (monuments render as background tiles)
    port_updateCollapseTileNametable(index);
#ifdef __SNES__
    s_monumentTextReserved = false;
    // Don't pre-reserve the monument's text cells here. The full reserve
    // eats ~45 of the 48 BG1 text slots, starving the room-title banner
    // that draws next in LoadRoomData. monumentUpdate reserves lazily on
    // first player touch, by which time the room-title has cleared and
    // freed its slots.
#else
    (void)monumentReserveHelper();
#endif
}

PORT_DATA_BANK0
static const unsigned char monumentText[][25] = {
    "-- celeste mountain -- ",
    "this memorial to those ",
    " perished on the climb "
};
static unsigned char monumentBlankLine[] = "                            ";
#define MONUMENT_TEXT_LINE_LEN 23u
#ifdef __NES__
#define MONUMENT_TEXT_X 20u
#define MONUMENT_TEXT_Y 92u
#define MONUMENT_TEXT_LINE_SPACING 4u
#define MONUMENT_TEXT_CHAR_ADVANCE 5u
#else
#define MONUMENT_TEXT_X 8u
#define MONUMENT_TEXT_Y 80u
#define MONUMENT_TEXT_LINE_SPACING 8u
#define MONUMENT_TEXT_CHAR_ADVANCE 5u
#endif
bool GLOBAL_MonumentTextDisplayed = false;
uint8_t GLOBAL_MonumentCurLineCharCount = 0;
uint8_t GLOBAL_MonumentCurLineNum = 0;
static uint8_t s_monumentTextTick = 0;

PORT_FUNC_NES6_SNES4
static void resetMonumentTextState(void) {
    GLOBAL_MonumentTextDisplayed = false;
    GLOBAL_MonumentCurLineCharCount = 0;
    GLOBAL_MonumentCurLineNum = 0;
    s_monumentTextTick = 0;
}

PORT_FUNC_NES6_SNES4
static bool monumentReserveHelper(void) {
#ifdef __SNES__
    bool ok = true;
    ok = port_reservePico8RunN(MONUMENT_TEXT_X, MONUMENT_TEXT_Y, MONUMENT_TEXT_LINE_LEN, MONUMENT_TEXT_CHAR_ADVANCE) && ok;
    ok = port_reservePico8RunN(MONUMENT_TEXT_X,
                               (uint8_t)(MONUMENT_TEXT_Y + MONUMENT_TEXT_LINE_SPACING),
                               MONUMENT_TEXT_LINE_LEN,
                               MONUMENT_TEXT_CHAR_ADVANCE) && ok;
    ok = port_reservePico8RunN(MONUMENT_TEXT_X,
                               (uint8_t)(MONUMENT_TEXT_Y + (MONUMENT_TEXT_LINE_SPACING * 2u)),
                               MONUMENT_TEXT_LINE_LEN,
                               MONUMENT_TEXT_CHAR_ADVANCE) && ok;
    s_monumentTextReserved = ok;
    return ok;
#endif
    return true;
}

PORT_FUNC_NES6_SNES4
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

PORT_FUNC_NES6_SNES4
static void monumentClearHelper(void) {
#ifdef __SNES__
    (void)port_clearReservedPico8RunN(MONUMENT_TEXT_X, MONUMENT_TEXT_Y, MONUMENT_TEXT_LINE_LEN, MONUMENT_TEXT_CHAR_ADVANCE);
    (void)port_clearReservedPico8RunN(MONUMENT_TEXT_X,
                                      (uint8_t)(MONUMENT_TEXT_Y + MONUMENT_TEXT_LINE_SPACING),
                                      MONUMENT_TEXT_LINE_LEN,
                                      MONUMENT_TEXT_CHAR_ADVANCE);
    (void)port_clearReservedPico8RunN(MONUMENT_TEXT_X,
                                      (uint8_t)(MONUMENT_TEXT_Y + (MONUMENT_TEXT_LINE_SPACING * 2u)),
                                      MONUMENT_TEXT_LINE_LEN,
                                      MONUMENT_TEXT_CHAR_ADVANCE);
#else
    port_drawText(monumentBlankLine, MONUMENT_TEXT_X, MONUMENT_TEXT_Y);
    port_drawText(monumentBlankLine,
                  MONUMENT_TEXT_X,
                  (uint8_t)(MONUMENT_TEXT_Y + MONUMENT_TEXT_LINE_SPACING));
    port_drawText(monumentBlankLine,
                  MONUMENT_TEXT_X,
                  (uint8_t)(MONUMENT_TEXT_Y + (MONUMENT_TEXT_LINE_SPACING * 2u)));
#endif
}

PORT_FUNC_NES6_SNES4
void monumentUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    bool isPlayerTouching;

    this->flags |= OBJ_FLAG_DIRTY;

    isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 4) &&
                       (GLOBAL_PlayerData.objData.pos.x < this->pos.x+16) &&
                       (GLOBAL_PlayerData.objData.pos.y > this->pos.y) &&
                       (GLOBAL_PlayerData.objData.pos.y < this->pos.y+16);
    if (isPlayerTouching) {
#ifdef __SNES__
        if (!s_monumentTextReserved && !monumentReserveHelper()) {
            GLOBAL_MonumentTextDisplayed = false;
            s_monumentTextTick = 0u;
            return;
        }
#endif
        GLOBAL_MonumentTextDisplayed = true;
        s_monumentTextTick ^= 1u;
        if (s_monumentTextTick != 0u) {
            return;
        }

        if (GLOBAL_MonumentCurLineNum < 3u) {
            if (GLOBAL_MonumentCurLineCharCount < MONUMENT_TEXT_LINE_LEN) {
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

#ifdef __SNES__
// Strings live in bank 5 alongside the format helpers that build them.
// "old site" used to live here too but level 12 is now skipped, see
// roomTitlePrep.
PORT_DATA_BANK5
static const unsigned char roomTitle_Summit[]  = "summit";

// Reuse the flag-overlay line buffers as scratch — they're only repopulated
// when the player triggers the end-game flag (long after the room-title
// clears), and the BG1 cells are already populated from those buffers by
// drainFlagOverlayPreRender before we run, so overwriting them is safe.
// Saves 18 bytes of near-RAM BSS.
extern unsigned char GLOBAL_FlagOverlayLine0[17];
extern unsigned char GLOBAL_FlagOverlayLine1[17];
#define s_roomTitleAltBuf   GLOBAL_FlagOverlayLine0
#define s_roomTitleTimerBuf GLOBAL_FlagOverlayLine1
static uint8_t s_roomTitleAltLen = 0u;
static uint8_t s_roomTitleAltX = 0u;

PORT_FUNC_BANK5
static uint8_t roomTitleAppendTwoDigits(unsigned char *out, uint8_t pos, uint8_t value) {
    uint8_t tens = 0u;
    while (value >= 100u) { value = (uint8_t)(value - 100u); }
    while (value >= 10u)  { value = (uint8_t)(value - 10u); tens++; }
    out[pos++] = (unsigned char)('0' + tens);
    out[pos++] = (unsigned char)('0' + value);
    return pos;
}

// Populate s_roomTitleAltBuf / s_roomTitleAltX / s_roomTitleAltLen and
// s_roomTitleTimerBuf from the current room ID and timer globals. Sets
// s_roomTitleAltLen=0 to signal "do not draw" for out-of-range rooms.
PORT_FUNC_BANK5
static void roomTitlePrep(void) {
    uint16_t roomID = GLOBAL_ActiveLevel.currentRoomID;
    uint16_t totalMinutes;
    uint8_t pos;

    if (roomID == ROOM_TITLE_OLD_SITE_ROOM) {
        // Monument room: suppress the banner entirely. The monument reserves
        // ~45 of the 48 BG1 text slots when the player touches it, and the
        // pool can't carry both that and our banner. Same treatment as the
        // summit room, but here we skip the altitude line too (not just the
        // timer) since the monument's reserve happens at unpredictable points
        // and would race with the banner's clear.
        s_roomTitleAltLen = 0u;
        return;
    } else if (roomID == ROOM_TITLE_SUMMIT_ROOM) {
        uint8_t i, len = (uint8_t)(sizeof(roomTitle_Summit) - 1u);
        for (i = 0u; i < len; ++i) s_roomTitleAltBuf[i] = roomTitle_Summit[i];
        s_roomTitleAltBuf[len] = '\0';
        s_roomTitleAltLen = len;
        s_roomTitleAltX = 52u;
        // Suppress the timer in summit room. Its cells span rows 0..1, which
        // are part of the flag-overlay's pre-rendered region (cells y=0..3).
        // Any cell publish in rows 0..1 triggers a full-row map DMA in
        // bg1TextPreBuildDma that auto-promotes the flag overlay's hidden
        // cells to visible. Drawing nothing here avoids that.
        s_roomTitleTimerBuf[0] = '\0';
        return;
    } else if (roomID >= 1u && roomID <= 30u) {
        uint8_t r = (uint8_t)roomID;
        pos = 0u;
        if (r >= 30u) {
            s_roomTitleAltBuf[pos++] = '3'; s_roomTitleAltBuf[pos++] = '0';
        } else if (r >= 20u) {
            s_roomTitleAltBuf[pos++] = '2'; s_roomTitleAltBuf[pos++] = (unsigned char)('0' + (uint8_t)(r - 20u));
        } else if (r >= 10u) {
            s_roomTitleAltBuf[pos++] = '1'; s_roomTitleAltBuf[pos++] = (unsigned char)('0' + (uint8_t)(r - 10u));
        } else {
            s_roomTitleAltBuf[pos++] = (unsigned char)('0' + r);
        }
        s_roomTitleAltBuf[pos++] = '0';
        s_roomTitleAltBuf[pos++] = '0';
        s_roomTitleAltBuf[pos++] = ' ';
        s_roomTitleAltBuf[pos++] = 'm';
        s_roomTitleAltBuf[pos] = '\0';
        s_roomTitleAltLen = pos;
        s_roomTitleAltX = (r < 10u) ? 54u : 52u;
    } else {
        s_roomTitleAltLen = 0u;
        return;
    }

    // MM:SS — minutes wraps at 100 (1h 39m) which is well beyond any
    // first-attempt playthrough. Drop hours rather than push glyphs into
    // the flag overlay's cell column range (cell x=4+).
    totalMinutes = GLOBAL_TimerMinutes;
    while (totalMinutes >= 100u) {
        totalMinutes = (uint16_t)(totalMinutes - 100u);
    }
    pos = roomTitleAppendTwoDigits(s_roomTitleTimerBuf, 0u, (uint8_t)totalMinutes);
    s_roomTitleTimerBuf[pos++] = ':';
    pos = roomTitleAppendTwoDigits(s_roomTitleTimerBuf, pos, GLOBAL_TimerSeconds);
    s_roomTitleTimerBuf[pos] = '\0';
}

PORT_NOINLINE static void roomTitleDrawHelperFromBank5(void);

// Bank-5 orchestrator. Single fixed-bank call site collapses room-ID range
// check + prep + draw + timer-arm into one bank switch.
PORT_FUNC_BANK5
static void roomTitleArm(void) {
    uint16_t roomID = GLOBAL_ActiveLevel.currentRoomID;
    if (roomID == 0u || roomID > ROOM_TITLE_SUMMIT_ROOM) return;
    roomTitlePrep();
    if (s_roomTitleAltLen != 0u) {
        roomTitleDrawHelperFromBank5();
        s_roomTitleClearTimer = ROOM_TITLE_VISIBLE_TICKS;
    }
}

// Bank-4 draw glue: consumes the pre-built strings and writes glyphs.
// Each glyph carries its own dark cell background via port_drawTextWhiteOnBlack,
// so no explicit box-fill is needed — keeps the bank-4 footprint small.
PORT_FUNC_BANK4
static void roomTitleDrawHelper(void) {
    if (s_roomTitleAltLen == 0u) return;
    port_drawTextWhiteOnBlack(s_roomTitleAltBuf, s_roomTitleAltX, ROOM_TITLE_TEXT_Y);
    port_drawTextWhiteOnBlack(s_roomTitleTimerBuf, ROOM_TITLE_TIMER_X, ROOM_TITLE_TIMER_Y);
}

// Bank-4 clear glue. Hides the altitude band (length=16+advance=5 → 81 px,
// covers worst-case altitude line) and the 5-char timer.
PORT_FUNC_BANK4
static void roomTitleClearHelper(void) {
    (void)port_clearReservedPico8RunN(ROOM_TITLE_BOX_X, ROOM_TITLE_TEXT_Y, 16u, 5u);
    (void)port_clearReservedPico8RunN(ROOM_TITLE_TIMER_X, ROOM_TITLE_TIMER_Y, ROOM_TITLE_TIMER_LEN, 5u);
}

// Fixed-mirror trampoline: lets bank-5 code call into the bank-4 draw glue.
PORT_NOINLINE
static void roomTitleDrawHelperFromBank5(void) {
    port_prg_bank_enter(4);
    roomTitleDrawHelper();
    port_prg_bank_leave();
}

// In SNES mode 0, BG1 high-priority tiles sit BELOW sprite priority 3, so the
// room-title text gets drawn under the player/hair/etc. While the banner is
// visible, demote any priority-3 OAM entry to priority 2 — that puts BG1.hi
// (text) above the sprites. Anything BG2.hi would also draw above the player
// during this window, but the level layer doesn't use BG2 high-priority tiles
// so the only visible effect is the intended one.
extern union uOAMCopy GLOBAL_OAMCopy;
PORT_FUNC_BANK5
static void roomTitleDemoteSprites(void) {
    uint8_t i;
    for (i = 0u; i < 128u; ++i) {
        uint8_t props = GLOBAL_OAMCopy.arr.OAMArray[i].PROPERTIES;
        if ((props & 0x30u) == 0x30u) {
            GLOBAL_OAMCopy.arr.OAMArray[i].PROPERTIES = (uint8_t)(props & 0xEFu);
        }
    }
}

#endif

enum eBigChestState {BIG_CHEST_STATE_IDLE = 0, BIG_CHEST_STATE_OPEN_ANIM = 1, BIG_CHEST_STATE_OPENED = 2};

// eBigChestSprite is defined in sprite_animation_enums.h
PORT_FUNC_NES6_SNES4
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

PORT_FUNC_NES6_SNES4
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
                initObject(OBJ_DOUBLE_JUMP_ORB,this->pos.x+4,this->pos.y+4);
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
bool GLOBAL_FlagOverlayDirty = false;
bool GLOBAL_FlagOverlayRevealDirty = false;
uint8_t GLOBAL_FlagOverlayLine0Len = 0u;
uint8_t GLOBAL_FlagOverlayLine1Len = 0u;
uint8_t GLOBAL_FlagOverlayLine2Len = 0u;
unsigned char GLOBAL_FlagOverlayLine0[17] = {0};
unsigned char GLOBAL_FlagOverlayLine1[17] = {0};
unsigned char GLOBAL_FlagOverlayLine2[17] = {0};

PORT_FUNC_NES6_SNES4
static void flagLineReset(char *line) {
    uint8_t i;
    for (i = 0; i < 17u; ++i) {
        line[i] = '\0';
    }
}

PORT_FUNC_NES6_SNES4
static void flagAppendText(char *line, uint8_t *pos, const char *text) {
    while (*text != '\0' && *pos < 16u) {
        line[*pos] = *text;
        (*pos)++;
        text++;
    }
}

PORT_FUNC_NES6_SNES4
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

PORT_FUNC_NES6_SNES4
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

PORT_FUNC_NES6_SNES4
static void flagDrawLineWhiteOnBlack(const char *line, uint8_t length, uint8_t x, uint8_t y) {
    uint8_t i;
    for (i = 0u; i < length; ++i) {
        port_drawCharWhiteOnBlack((uint8_t)line[i], (uint8_t)(x + (i * 5u)), y);
    }
}

#ifdef __SNES__
// Flag overlay text formatters live in bank 5 to free bank 4 space for the
// pre-render dispatch. flagUpdate calls them via a fixed-mirror trampoline
// (flagDrawHelperViaBank5) — bank switching has to originate from fixed
// mirror or RTS would land at the wrong bank's offset.
PORT_FUNC_BANK5
static uint8_t flagOverlayAppendDecimal(unsigned char *line, uint8_t pos, uint16_t value) {
    uint16_t divisor = 10000u;
    bool started = false;
    while (divisor > 0u && pos < 16u) {
        uint8_t digit = 0u;
        while (value >= divisor) {
            value = (uint16_t)(value - divisor);
            digit++;
        }
        if (digit != 0u || started || divisor == 1u) {
            line[pos] = (unsigned char)('0' + digit);
            pos++;
            started = true;
        }
        divisor = (uint16_t)(divisor / 10u);
    }
    line[pos] = '\0';
    return pos;
}

PORT_FUNC_BANK5
static uint8_t flagOverlayAppendTwoDigits(unsigned char *line, uint8_t pos, uint16_t value) {
    uint8_t tens = 0u;
    while (value >= 100u) {
        value = (uint16_t)(value - 100u);
    }
    while (value >= 10u) {
        value = (uint16_t)(value - 10u);
        tens++;
    }
    if (pos < 16u) {
        line[pos++] = (unsigned char)('0' + tens);
    }
    if (pos < 16u) {
        line[pos++] = (unsigned char)('0' + (uint8_t)value);
    }
    line[pos] = '\0';
    return pos;
}

PORT_FUNC_BANK5
static void flagDrawHelper(uint16_t score, uint16_t totalMinutes, uint16_t seconds, uint16_t deaths) {
    uint8_t pos;
    uint16_t hours;
    uint16_t minutes;

    GLOBAL_FlagOverlayLine0[0] = 'x';
    GLOBAL_FlagOverlayLine0Len = flagOverlayAppendDecimal(GLOBAL_FlagOverlayLine0, 1u, score);

    hours = 0u;
    while (totalMinutes >= 60u) {
        totalMinutes = (uint16_t)(totalMinutes - 60u);
        hours++;
    }
    minutes = totalMinutes;

    pos = flagOverlayAppendTwoDigits(GLOBAL_FlagOverlayLine1, 0u, hours);
    GLOBAL_FlagOverlayLine1[pos++] = ':';
    pos = flagOverlayAppendTwoDigits(GLOBAL_FlagOverlayLine1, pos, minutes);
    GLOBAL_FlagOverlayLine1[pos++] = ':';
    GLOBAL_FlagOverlayLine1Len = flagOverlayAppendTwoDigits(GLOBAL_FlagOverlayLine1, pos, seconds);

    GLOBAL_FlagOverlayLine2[0] = 'd';
    GLOBAL_FlagOverlayLine2[1] = 'e';
    GLOBAL_FlagOverlayLine2[2] = 'a';
    GLOBAL_FlagOverlayLine2[3] = 't';
    GLOBAL_FlagOverlayLine2[4] = 'h';
    GLOBAL_FlagOverlayLine2[5] = 's';
    GLOBAL_FlagOverlayLine2[6] = ':';
    GLOBAL_FlagOverlayLine2Len = flagOverlayAppendDecimal(GLOBAL_FlagOverlayLine2, 7u, deaths);
    GLOBAL_FlagOverlayDirty = true;
}

// Fixed-mirror trampoline. Bank switching must originate from fixed-mirror
// code: a JSR/RTS round-trip across banks works only if the call site is at
// an address mirrored into all banks. flagUpdate (bank 4) routes through
// this trampoline to reach flagDrawHelper (bank 5). MUST stay noinline so it
// keeps its fixed-mirror placement instead of being folded into flagUpdate.
PORT_NOINLINE
static void flagDrawHelperViaBank5(uint16_t score, uint16_t totalMinutes,
                                   uint16_t seconds, uint16_t deaths)
{
    port_prg_bank_enter(5);
    flagDrawHelper(score, totalMinutes, seconds, deaths);
    port_prg_bank_leave();
}
#else
PORT_FUNC_NES6_SNES4
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
#endif

PORT_FUNC_NES6_SNES4
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
    GLOBAL_FlagOverlayDirty = false;
    GLOBAL_FlagOverlayRevealDirty = false;
    this->oamTile = FLAG_SPRITE_1;
    this->oamProps = 0x32; // priority 3, palette 2
    this->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_NES6_SNES4
void flagUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    bool isPlayerTouching = (GLOBAL_PlayerData.objData.pos.x > this->pos.x - 8) &&
                            (GLOBAL_PlayerData.objData.pos.x < this->pos.x + 8) &&
                            (GLOBAL_PlayerData.objData.pos.y > this->pos.y - 8) &&
                            (GLOBAL_PlayerData.objData.pos.y < this->pos.y + 8);
    bool statsChanged = !this->data.flag.drawn ||
                        this->data.flag.lastSeconds != GLOBAL_TimerSeconds ||
                        this->data.flag.lastMinutes != GLOBAL_TimerMinutes ||
                        this->data.flag.lastDeaths != GLOBAL_DeathCount ||
                        this->data.flag.score != GLOBAL_FruitCount;

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

    if (statsChanged) {
        this->data.flag.score = GLOBAL_FruitCount;
        this->data.flag.drawn = true;
        this->data.flag.lastSeconds = GLOBAL_TimerSeconds;
        this->data.flag.lastMinutes = GLOBAL_TimerMinutes;
        this->data.flag.lastDeaths = GLOBAL_DeathCount;
#ifdef __SNES__
        flagDrawHelperViaBank5(this->data.flag.score, GLOBAL_TimerMinutes, GLOBAL_TimerSeconds, GLOBAL_DeathCount);
#else
        flagDrawHelper(this->data.flag.score, GLOBAL_TimerMinutes, GLOBAL_TimerSeconds, GLOBAL_DeathCount);
#endif
    }

    if (!this->data.flag.show && isPlayerTouching) {
        this->data.flag.show = true;
        GLOBAL_FlagOverlayShow = true;
        GLOBAL_FlagOverlayRevealDirty = true;
        playSoundEffect(SOUND_EFFECT_FLAG);
    }
    if (!this->data.flag.show) {
        GLOBAL_FlagOverlayShow = false;
        GLOBAL_FlagOverlayRevealDirty = false;
    }

    this->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_NES6_SNES3
void doubleDashOrbInit(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    this->extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
    this->extraSpriteCount = 0;
    this->oamTile = DOUBLE_JUMP_ORB_SPRITE_1;
    this->oamProps = 0x38; // priority 3, palette 4
    this->data.doubleJumpOrb.frameCount = 0;
    this->data.doubleJumpOrb.speedY = -4;  // ccleste ORB_init: spd.y=-4
    this->data.doubleJumpOrb.accelAccumulator = 0;
    this->flags |= OBJ_FLAG_DIRTY;
}

PORT_FUNC_NES6_SNES3
void doubleDashOrbUpdate(uint8_t index) {
    OBJ_DATA *this = &GLOBAL_OBJList[index];
    // Speed starts at -8 and decelerates by 1 every 2 frames.
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

PORT_FUNC_NES6_SNES3
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

PORT_FUNC_NES6_SNES2
void decoTreeInit(uint8_t index) {
    initSimpleDecorSprite(index, DECO_TREE_SPRITE_1, 0x32); // priority 3, palette 2
}
PORT_FUNC_NES6_SNES2
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

PORT_FUNC_NES6_SNES3
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

PORT_FUNC_NES6_SNES3
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

PORT_FUNC_NES6_SNES3
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

PORT_FUNC_NES6
void initObject(enum eOBJType eType, int16_t x, int16_t y) {
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


PORT_FUNC_NES6
static void clearObjectDirtyFlag(uint8_t index)
{
    GLOBAL_OBJList[index].flags &= (uint8_t)~OBJ_FLAG_DIRTY;
}

PORT_FUNC_NES6_SNES2
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

PORT_FUNC_NES6_SNES3
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

PORT_FUNC_NES6_SNES4
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

PORT_FUNC_NES6
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


PORT_FUNC_NES6_SNES1
void playerInit(struct sPlayerData* this);

// onVblank is in fixed bank (bank 0), not bank 6
static bool onVblank(void);
static void runGameplayFrame(void);
static void refreshGameplaySprites(void);
static PORT_NOINLINE bool shouldRunGameplayFrame(void);

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
#ifdef __SNES__
    s_monumentTextReserved = false;
#endif

    port_LoadRoomData(roomID);

#ifdef __NES__
    port_prg_bank_enter(6);
#else
    port_prg_bank_enter(1);
#endif
    playerInit(&GLOBAL_PlayerData);
    port_prg_bank_leave();
    port_updatePlayerSprite(&GLOBAL_PlayerData);
#ifdef __SNES__
    port_prg_bank_enter(1);
    port_updatePlayerHair(&GLOBAL_PlayerData);
    port_prg_bank_leave();
#endif

    port_beginSpriteBuild(&GLOBAL_PlayerData);
    for (i = 0; i < GLOBAL_OBJ_LIST_SIZE; ++i) {
        processObject(i);
    }
    port_finishSpriteBuild();
#ifndef __NES__
    port_prg_bank_enter(7);
    updateRoomMusic(roomID);
    port_prg_bank_leave();

    // If the room has a flag, drain its overlay pre-render in one shot
    // here while the level transition still owns the frame, instead of
    // bleeding 80+ state-machine steps into early gameplay frames.
    {
        uint8_t flagIdx;
        bool hasFlag = false;
        for (flagIdx = 1; flagIdx < GLOBAL_OBJ_LIST_SIZE; ++flagIdx) {
            if (GLOBAL_OBJList[flagIdx].eType == OBJ_FLAG) {
                hasFlag = true;
                break;
            }
        }
        if (hasFlag) {
            port_drainFlagOverlayPreRender();
        }
    }

    // Draw the room-title overlay. roomTitleArm internally filters out
    // roomID 0 (title) and out-of-range rooms.
    port_prg_bank_enter(5);
    roomTitleArm();
    port_prg_bank_leave();

    // Drain all BG1 text DMA synchronously while force-blank is still
    // engaged. Without this, the normal 2-slots-per-vblank throttle would
    // stretch the un-blank gate (snes.c:port_vblank) by ~7 vblanks for
    // typical room-title text, plus more if a flag overlay was pre-rendered.
    port_drainBg1TextDma();
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

#ifdef __VBCC__
/* vbcc's SNES startup pulls in an NMI vector that calls user code
 * snesXC_nmi(). Provide an empty stub so the vbcc link resolves; the
 * shipping llvm-mos path installs its own NMI via assembly vectors. */
void snesXC_nmi(void) { }
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


PORT_FUNC_NES6_SNES1
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

PORT_FUNC_NES6_SNES1
void playerInit(struct sPlayerData* this){
    uint16_t i;
    this->objData.eType = OBJ_PLAYER;
    this->objData.pos.x = GLOBAL_ActiveLevel.playerSpawnX * 8;
    // Start the level-intro hop: player begins at y=128 (just below the room)
    // with upward velocity, hops up to spawnTargetY+16, then falls back to
    // spawnTargetY. playerUpdate gates on spawnState until the hop finishes.
    this->spawnTargetY = (int16_t)(GLOBAL_ActiveLevel.playerSpawnY * 8);
    this->objData.pos.y = 128;
    this->spawnState = 0;
    this->spawnDelay = 0;
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
    this->spd.y = -INT_TO_FIXED(4);  // ccleste PLAYER_SPAWN_init: spd.y=-4

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


    //Clear out the object array.
    for (i = 1; i < GLOBAL_OBJ_LIST_SIZE; i++) {
        GLOBAL_OBJList[i].eType = OBJ_UNUSED;
        GLOBAL_OBJList[i].extraSpriteBase = PORT_EXTRA_SLOT_UNUSED;
        GLOBAL_OBJList[i].extraSpriteCount = 0;
    }

    port_resetSprites();
#ifdef __SNES__
    // Snap hair particles to the new player position before the per-frame
    // OAM update — otherwise the trail lerps in from the previous room.
    port_resetPlayerHair(this);
#endif
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

PORT_FUNC_NES6_SNES1
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

PORT_FUNC_NES6_SNES1
static bool OBJ_isDeathAt(struct sPlayerData* this, int16_t xOffset, int16_t yOffset) {
    int16_t x = this->objData.pos.x + xOffset;
    int16_t y = this->objData.pos.y + yOffset;

    // Pass hitbox bounds {1,3,6,5} instead of full sprite bounds
    return isDeathAtPoint(x + 1, y + 3, 6, 5, this->spd.x, this->spd.y);
}

PORT_FUNC_NES6_SNES1
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

PORT_FUNC_NES6_SNES1
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

// Level-intro hop state machine (ccleste celeste.c:937-988 PLAYER_SPAWN).
// Runs in place of the normal playerUpdate until landing. Skips input,
// collision, dash, jump — just animates the hop with simple gravity and
// snaps to the spawn position on landing.
PORT_FUNC_NES6_SNES1
static void playerSpawnUpdate(struct sPlayerData* this) {
    int16_t step;

    if (this->spawnState == 0u) {
        // Rising: spd.y is -4 from init. Once y has risen to target+16, switch.
        if (this->objData.pos.y < (int16_t)(this->spawnTargetY + 16)) {
            this->spawnState = 1u;
            this->spawnDelay = 3u;
        }
    } else if (this->spawnState == 1u) {
        // Falling: apply gravity (+0.5/frame in 16.16 fixed). On delay > 0,
        // hover at peak (zero spd) for spawnDelay frames. Once spd is positive
        // and y has crossed back to the target, snap and start landing.
        this->spd.y = FIXED_ADD(this->spd.y, 0x8000);
        if (this->spd.y > 0 && this->spawnDelay > 0u) {
            this->spd.y = 0;
            this->spawnDelay--;
        }
        if (this->spd.y > 0 && this->objData.pos.y > this->spawnTargetY) {
            this->objData.pos.y = this->spawnTargetY;
            this->spd.x = 0;
            this->spd.y = 0;
            this->spawnState = 2u;
            this->spawnDelay = 5u;
            GLOBAL_ActiveLevel.shakeFrames = 5;
            initObject(OBJ_SMOKE, this->objData.pos.x,
                       (int16_t)(this->objData.pos.y + 4));
        }
    } else {
        // Landing pause.
        if (this->spawnDelay > 0u) {
            this->spawnDelay--;
        } else {
            this->spawnState = 0xFFu;
        }
    }

    // Apply spd.y → pos.y for states 0 and 1 (rising/falling). No collision
    // checks — the hop is well-defined (target is the spawn position, which
    // sits in open space).
    if (this->spawnState <= 1u) {
        this->posF.y = FIXED_ADD(this->posF.y, this->spd.y);
        step = (int16_t)(((this->posF.y) + 32768) >> 16);
        this->posF.y -= INT_TO_FIXED(step);
        this->objData.pos.y = (int16_t)(this->objData.pos.y + step);
    }

    this->objData.flags |= OBJ_FLAG_DIRTY;
}

#define FPS60_SCALE_FACTOR 1.0f
PORT_FUNC_NES6_SNES1
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
    // FP_GRAVITY is defined at file scope so spring/breakable-wall can reference it
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

    // Level-intro hop: skip all normal gameplay logic until the spawn
    // state machine reaches the "done" sentinel (0xFF). Set in playerInit.
    if (this->spawnState != 0xFFu) {
        playerSpawnUpdate(this);
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
        GLOBAL_ActiveLevel.shakeFrames = 10;
#ifdef __SNES__
        // Snapshot death position before playerInit overwrites it. Particles
        // are now a separate flat array (not OBJ entries) so spawn order vs
        // playerInit no longer matters for slot churn — ccleste celeste.c:1582.
        {
            int16_t deathX = this->objData.pos.x + 4;
            int16_t deathY = this->objData.pos.y + 4;
            this->objData.pos.y = 128;
            playerInit(this);
            spawnDeadParticlesViaBank2(deathX, deathY);
            // playerInit's port_resetSprites cleared OAM for every slot; the
            // spawn-hop freeze gate around updateAllObjects will skip the
            // normal OBJ-OAM rebuild for ~25 frames, so other room sprites
            // (springs, invisible walls, etc.) would briefly disappear.
            // Force an OAM rebuild here so they reappear on the next vblank.
            rebuildAllSprites();
        }
#else
        this->objData.pos.y = 128;
        playerInit(this);
#endif
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


// 16-bit LCG.
static uint16_t global_randSeed = 0xBEEFu;
PORT_FUNC_NES6
uint16_t my_rand() {
    global_randSeed = (uint16_t)(global_randSeed * 109u + 89u);
    return global_randSeed;
}

// Returns a random integer in [min, max]
PORT_FUNC_NES6
int16_t randint16(int16_t min, int16_t max) {
    uint16_t range;
    uint16_t randomValue;
    if (max <= min) return min;
    range = (uint16_t)(max - min) + 1;
    randomValue = my_rand();
    return (int16_t)((randomValue % range) + min);
}

#ifndef __NES__
// Pico-8 → SNES BGR15 conversions used by applyTitleFlash. ccleste's start-
// game flash remaps the title's bright colors (pico-8 7/12/13) to one of
// {10 yellow, 7 white, 2 dk purple, 1 dk blue, 0 black} depending on how
// far into the 80-frame countdown we are.
//   pico-8 0  (0,0,0)        = 0x0000
//   pico-8 1  (29,43,83)     = 0x28A4
//   pico-8 2  (126,37,83)    = 0x288F
//   pico-8 7  (255,241,232)  = 0x73BF  (the original title palette[1])
//   pico-8 10 (255,236,39)   = 0x17BF
// Title palette[2]=0x4DD0 (pico-8 13 lavender) and palette[3]=0x7EA5
// (pico-8 12 blue) — restored verbatim from python/title_screen_snes.h.
#define TITLE_FLASH_BGR15_BLACK   0x0000u
#define TITLE_FLASH_BGR15_DK_BLUE 0x28A4u
#define TITLE_FLASH_BGR15_DK_PURP 0x288Fu
#define TITLE_FLASH_BGR15_WHITE   0x73BFu
#define TITLE_FLASH_BGR15_LAV     0x4DD0u
#define TITLE_FLASH_BGR15_BLUE    0x7EA5u

// Title-flash CGRAM staging. ApplyTitleFlash fills the byte buffer from bank
// 7 during handleTitleScreenFrame; the actual CGRAM register writes are
// deferred to port_vblank's early section (right after the HVBJOY sync), so
// they happen at the *start* of vblank instead of partway through. Mid-
// vblank CGRAM writes — especially after OAM DMA — were landing close
// enough to active display that the backdrop visibly bled the flash colour.
//
// The 8-byte buffer aliases onto GLOBAL_FlagOverlayLine0 (declared at file
// scope, 17 bytes, gameplay-only — dormant on the title screen). The
// 1-byte dirty flag is its own BSS — kept tiny because we're at the
// 128-byte heap-headroom floor.
//
// Buffer holds CGRAM[0x40..0x43] = BG3 palette 0 in mode 0:
// {transparent, pico-8 7 white, pico-8 13 lavender, pico-8 12 blue}. The
// transparent slot (0x40) is unused for display (mode 0 routes pixel-
// value-0 to the master backdrop CGRAM[0]) but we rewrite it as 0 each
// frame so it can't leak.
#define GLOBAL_TitleFlashCgramBytes (GLOBAL_FlagOverlayLine0)
uint8_t GLOBAL_TitleFlashCgramDirty = 0u;

PORT_FUNC_BANK7
static void applyTitleFlash(int8_t flash) {
    uint16_t c = 0u;
    bool doSwap = false;
    if (flash > 10) {
        // ccleste: default c=10 (no remap because c<10 gate is false),
        // flipped to c=7 (remap) every 5 game-frames → strobe effect.
        // ccleste runs at 30Hz, GLOBAL_FrameCount is per-vblank (60Hz),
        // so divide by 2 to get a game-frame clock.
        if (((GLOBAL_FrameCount >> 1) % 10u) < 5u) {
            c = TITLE_FLASH_BGR15_WHITE;
            doSwap = true;
        }
    } else if (flash > 5) {
        c = TITLE_FLASH_BGR15_DK_PURP;
        doSwap = true;
    } else if (flash > 0) {
        c = TITLE_FLASH_BGR15_DK_BLUE;
        doSwap = true;
    } else {
        c = TITLE_FLASH_BGR15_BLACK;
        doSwap = true;
    }

    GLOBAL_TitleFlashCgramBytes[0] = 0x00u;  // CGRAM[0x40] = 0 (transparent)
    GLOBAL_TitleFlashCgramBytes[1] = 0x00u;
    if (doSwap) {
        uint8_t lo = (uint8_t)(c & 0xFFu);
        uint8_t hi = (uint8_t)((c >> 8) & 0xFFu);
        GLOBAL_TitleFlashCgramBytes[2] = lo; GLOBAL_TitleFlashCgramBytes[3] = hi;
        GLOBAL_TitleFlashCgramBytes[4] = lo; GLOBAL_TitleFlashCgramBytes[5] = hi;
        GLOBAL_TitleFlashCgramBytes[6] = lo; GLOBAL_TitleFlashCgramBytes[7] = hi;
    } else {
        // Restore originals for the c==10 strobe-back phase.
        GLOBAL_TitleFlashCgramBytes[2] = (uint8_t)(TITLE_FLASH_BGR15_WHITE & 0xFFu);
        GLOBAL_TitleFlashCgramBytes[3] = (uint8_t)(TITLE_FLASH_BGR15_WHITE >> 8);
        GLOBAL_TitleFlashCgramBytes[4] = (uint8_t)(TITLE_FLASH_BGR15_LAV   & 0xFFu);
        GLOBAL_TitleFlashCgramBytes[5] = (uint8_t)(TITLE_FLASH_BGR15_LAV   >> 8);
        GLOBAL_TitleFlashCgramBytes[6] = (uint8_t)(TITLE_FLASH_BGR15_BLUE  & 0xFFu);
        GLOBAL_TitleFlashCgramBytes[7] = (uint8_t)(TITLE_FLASH_BGR15_BLUE  >> 8);
    }
    GLOBAL_TitleFlashCgramDirty = 1u;
}

// Flush the staged title-flash CGRAM update. Called from port_vblank right
// after the HVBJOY sync (well before OAM DMA) so the colour swap lands at
// the start of vblank and can't bleed into active display. Lives in bank 7
// to keep the fixed-mirror code small; the caller wraps with bank_enter(7)
// / bank_leave().
PORT_FUNC_BANK7
void port_titleFlashFlush(void) {
    REG_CGADD = 0x40u;
    REG_CGDATA = GLOBAL_TitleFlashCgramBytes[0];
    REG_CGDATA = GLOBAL_TitleFlashCgramBytes[1];
    REG_CGDATA = GLOBAL_TitleFlashCgramBytes[2];
    REG_CGDATA = GLOBAL_TitleFlashCgramBytes[3];
    REG_CGDATA = GLOBAL_TitleFlashCgramBytes[4];
    REG_CGDATA = GLOBAL_TitleFlashCgramBytes[5];
    REG_CGDATA = GLOBAL_TitleFlashCgramBytes[6];
    REG_CGDATA = GLOBAL_TitleFlashCgramBytes[7];
    // Force master backdrop CGRAM[0] back to black — the entry whose stray
    // pulse the user originally reported. Keeps the title backdrop locked
    // through the flash even if something else managed to write CGRAM[0].
    REG_CGADD = 0x00u;
    REG_CGDATA = 0x00u;
    REG_CGDATA = 0x00u;
}

// Whole title-screen tick lives in rom_bank_7 (otherwise the fixed mirror
// overflows with the cheat-code state machine + dedication wiring).
// Returns nothing; caller in onVblank just `return false` after it runs.
PORT_FUNC_BANK7
static void handleTitleScreenFrame(void) {
    GLOBAL_InputState = port_getInputs();
    port_resetSprites();
    bool startGame = false;
    if (s_dedicationTimer > 0u) {
        s_dedicationTimer--;
        if (s_dedicationTimer == 0u) {
            port_endDedicationScreen();
            startGame = true;
        }
    } else if (s_titleStartActive) {
        // ccleste runs start_game_flash from 50 down to -30 (80 game-frames):
        //   >10           : strobe pico-8 colors {10 yellow, 7 white}
        //   (5, 10]       : c=2 dark purple
        //   (0, 5]        : c=1 dark blue
        //   <=0           : c=0 black
        // begin_game fires when it reaches -30. ccleste runs at 30Hz, so
        // only tick the timer every other vblank; the CGRAM holds its last
        // write on the off frames, giving a 30Hz-paced flash.
        if ((GLOBAL_FrameCount & 1u) == 0u) {
            s_titleStartTimer--;
            applyTitleFlash(s_titleStartTimer);
            if (s_titleStartTimer <= -30) {
                startGame = true;
                s_titleStartTimer = 0;
                s_titleStartActive = false;
            }
        }
    } else {
        // Cheat-code edge detection (dpad + Start only). Sequence is
        // UP DOWN UP DOWN LEFT RIGHT LEFT RIGHT START — encoded inline.
        uint8_t newly = (uint8_t)(GLOBAL_InputState & ~s_prevTitleInput);
        s_prevTitleInput = GLOBAL_InputState;
        uint8_t newlyDir = (uint8_t)(newly & (PORT_INPUT_UP_MASK |
                                              PORT_INPUT_DOWN_MASK |
                                              PORT_INPUT_LEFT_MASK |
                                              PORT_INPUT_RIGHT_MASK |
                                              PORT_INPUT_START_MASK));
        bool cheatConsumed = false;
        if (newlyDir != 0u) {
            uint8_t expected;
            if (s_cheatStep >= 8u) {
                expected = PORT_INPUT_START_MASK;
            } else if ((s_cheatStep & 4u) == 0u) {
                expected = (s_cheatStep & 1u) ? PORT_INPUT_DOWN_MASK : PORT_INPUT_UP_MASK;
            } else {
                expected = (s_cheatStep & 1u) ? PORT_INPUT_RIGHT_MASK : PORT_INPUT_LEFT_MASK;
            }
            if (newlyDir == expected) {
                s_cheatStep++;
                cheatConsumed = true;
                if (s_cheatStep >= CHEAT_STEP_COUNT) {
                    s_cheatStep = 0u;
                    port_audioStopAll();
                    port_showDedicationScreen();
                    port_drawDedicationText();
                    s_dedicationTimer = 600u; // ~10s at 60fps
                }
            } else {
                // Wrong press — reset, crediting a stray UP as step 1.
                s_cheatStep = (newlyDir == PORT_INPUT_UP_MASK) ? 1u : 0u;
            }
        }
        if (!cheatConsumed) {
            if ((GLOBAL_InputState & (PORT_INPUT_B_MASK | PORT_INPUT_Y_MASK | PORT_INPUT_START_MASK)) != 0u ||
                (GLOBAL_InputLo & (PORT_INPUT_A_MASK | PORT_INPUT_X_MASK)) != 0u) {
                port_audioStopAll();
                playSoundEffect(SOUND_EFFECT_TITLE_START);
                // Match ccleste: set start_game_flash=50 then decrement once
                // before the first draw, so this frame's flash uses 49.
                s_titleStartTimer = 50;
                s_titleStartActive = true;
                s_titleStartTimer--;
                applyTitleFlash(s_titleStartTimer);
            }
        }
    }
    if (startGame) {
        s_inTitleScreen = false;
        port_setTitleMode(false);
        port_showGameplayScreen();
        GLOBAL_ActiveLevel.currentRoomID = 1;
        GLOBAL_TimerFrames = 0;
        GLOBAL_TimerSeconds = 0;
        GLOBAL_TimerMinutes = 0;
        LoadRoomData(1);
    }
}
#endif

static bool onVblank(void) {
    port_vblank();
    port_audioUpdate();
#ifndef __NES__
    if (s_inTitleScreen) {
        port_prg_bank_enter(7);
        handleTitleScreenFrame();
        port_prg_bank_leave();
        return false;
    }
#endif

    return true;
}

static PORT_NOINLINE bool shouldRunGameplayFrame(void)
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
    // Banner clear countdown. Held at full value while force-blank is
    // pending so the visible 30-tick window starts the frame INIDISP
    // un-blanks (not the LoadRoomData frame).
    if (s_roomTitleClearTimer != 0u && !s_pendingDisplayEnable) {
        --s_roomTitleClearTimer;
        if (s_roomTitleClearTimer == 0u) {
            port_prg_bank_enter(4);
            roomTitleClearHelper();
            port_prg_bank_leave();
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
#ifdef __SNES__
    port_prg_bank_enter(1);
    port_updatePlayerHair(&GLOBAL_PlayerData);
    port_prg_bank_leave();
#endif
#ifdef __SNES__
    // Death particles tick every frame regardless of player spawnState — they
    // live outside the OBJ system (separate flat array) so the freeze gate
    // below doesn't apply, matching ccleste's dead_particles[] behaviour.
    tickDeadParticlesViaBank2();
#endif
    // Pause all non-player object ticks during the level-intro hop, matching
    // ccleste's room-frozen feel while PLAYER_SPAWN animates. Player + hair
    // still update above; room sprites stay frozen at their LoadRoomData
    // initial positions until the hop completes (spawnState=0xFF).
    if (GLOBAL_PlayerData.spawnState == 0xFFu) {
        updateAllObjects();
    }
    port_renderTextOverlays();
    port_levelAnimAdvance();
#ifdef __SNES__
    if (s_roomTitleClearTimer != 0u) {
        port_prg_bank_enter(5);
        roomTitleDemoteSprites();
        port_prg_bank_leave();
    }
#endif
}

static void refreshGameplaySprites(void) {
    if (GLOBAL_FreezeFrames > 0) {
        return;
    }
    port_updatePlayerSprite(&GLOBAL_PlayerData);
#ifdef __SNES__
    port_prg_bank_enter(1);
    port_updatePlayerHair(&GLOBAL_PlayerData);
    port_prg_bank_leave();
#endif
    rebuildAllSprites();
#ifdef __SNES__
    if (s_roomTitleClearTimer != 0u) {
        port_prg_bank_enter(5);
        roomTitleDemoteSprites();
        port_prg_bank_leave();
    }
#endif
}
