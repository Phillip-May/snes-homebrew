#include "npcs.h"
#include "game.h"
#include "elevators.h"
#include "metasprite.h"
#include "random.h"
#include "math.h"
#include "resources.h"
#include "resources/metasprites/metasprite_data.h"  // Include for frame tables
#include <string.h>

// Forward declaration for debug function
extern void consoleNocashMessage(const char *format, ...);

// Forward declaration
extern StrikeAnimationType Game_strikeAnimationType;

// NPC structure (forward declaration)
typedef struct Npc Npc;

struct Npc {
    NpcState state;
    uint8_t leftSideOnZero;  // 0 = left, non-zero = right
    uint8_t floor;
    uint8_t targetFloor;
    uint16_t xPos;      // Fixed-point 16.8 (3 bytes in assembly, but we use 16-bit for simplicity)
    uint16_t yPos;      // Fixed-point 16.8
    uint8_t animationCounter;
    uint8_t facingLeftOnZero;  // 0 = left, non-zero = right
    uint8_t frame;
    uint8_t feelings;
    uint16_t feelingsTimeout;
    uint16_t spriteCharAttr;
    uint16_t spriteFrameTablePtr;  // Pointer to metasprite table
    uint16_t spriteFramePtr;       // Current frame pointer (index into table)
    Npc *nextNpcInLine;            // Linked list for line queuing
    uint8_t stateTimer;
};

// NPC array
static Npc npcs[N_NPCS];

// NPC system state
static Npc *nextFreeNpcPtr = NULL;
static int16_t countdownToNextNpc = 1;
static int16_t feelingsSpeed = NPC_FEELINGS_SPEED;

// Strike animation state
static uint16_t strike_frame = 0;
static uint8_t strike_frameTimeout = 0;
static Npc *strike_npc = NULL;
static uint8_t strike_hideArrowsOnZero = 0;

// Sprite constants
// Assembly: businessManCharAttr = (7 << 10) | (2 << 13) | (512 - 32)
// Palette 7, order 2, char = 480 (512 - 32 = 480 tiles = 960 bytes = 480 words)
// In 16-bit format: palette (bits 10-12), order (bits 13-14), char (bits 0-9)
#define NPC_SPRITE_CHARATTR ((7 << 10) | (2 << 13) | (512 - 32))

// Arrow location tables (word indices in buffer)
static const uint16_t ArrowsLocationsLeft[4] = {
    ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 0 + ELEVATOR_ARROW_COLUMN) * 32 + ELEVATOR_LEFT_ARROW_ROW),
    ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 1 + ELEVATOR_ARROW_COLUMN) * 32 + ELEVATOR_LEFT_ARROW_ROW),
    ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 2 + ELEVATOR_ARROW_COLUMN) * 32 + ELEVATOR_LEFT_ARROW_ROW),
    ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 3 + ELEVATOR_ARROW_COLUMN) * 32 + ELEVATOR_LEFT_ARROW_ROW)
};

static const uint16_t ArrowsLocationsRight[4] = {
    ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 0 + ELEVATOR_ARROW_COLUMN) * 32 + ELEVATOR_RIGHT_ARROW_ROW),
    ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 1 + ELEVATOR_ARROW_COLUMN) * 32 + ELEVATOR_RIGHT_ARROW_ROW),
    ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 2 + ELEVATOR_ARROW_COLUMN) * 32 + ELEVATOR_RIGHT_ARROW_ROW),
    ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 3 + ELEVATOR_ARROW_COLUMN) * 32 + ELEVATOR_RIGHT_ARROW_ROW)
};

// Arrow tiles table (4 tiles per combination: floor 0-3, target 0-3)
static const uint16_t ArrowsTilesLeft[64] = {
    // Floor 0 -> target 0,1,2,3
    0, 0, 0, 0,
    ARROW_LEFT_DOWN_TILE, 0, 0, 0,
    ARROW_LEFT_DOWN_TILE, ARROW_LEFT_DOWN_TILE, 0, 0,
    ARROW_LEFT_DOWN_TILE, ARROW_LEFT_DOWN_TILE, ARROW_LEFT_DOWN_TILE, 0,
    // Floor 1 -> target 0,1,2,3
    ARROW_LEFT_UP_TILE, 0, 0, 0,
    0, 0, 0, 0,
    ARROW_LEFT_DOWN_TILE, 0, 0, 0,
    ARROW_LEFT_DOWN_TILE, ARROW_LEFT_DOWN_TILE, 0, 0,
    // Floor 2 -> target 0,1,2,3
    ARROW_LEFT_UP_TILE, ARROW_LEFT_UP_TILE, 0, 0,
    ARROW_LEFT_UP_TILE, 0, 0, 0,
    0, 0, 0, 0,
    ARROW_LEFT_DOWN_TILE, 0, 0, 0,
    // Floor 3 -> target 0,1,2,3
    ARROW_LEFT_UP_TILE, ARROW_LEFT_UP_TILE, ARROW_LEFT_UP_TILE, 0,
    ARROW_LEFT_UP_TILE, ARROW_LEFT_UP_TILE, 0, 0,
    ARROW_LEFT_UP_TILE, 0, 0, 0,
    0, 0, 0, 0
};

static const uint16_t ArrowsTilesRight[64] = {
    // Floor 0 -> target 0,1,2,3
    0, 0, 0, 0,
    ARROW_RIGHT_DOWN_TILE, 0, 0, 0,
    ARROW_RIGHT_DOWN_TILE, ARROW_RIGHT_DOWN_TILE, 0, 0,
    ARROW_RIGHT_DOWN_TILE, ARROW_RIGHT_DOWN_TILE, ARROW_RIGHT_DOWN_TILE, 0,
    // Floor 1 -> target 0,1,2,3
    ARROW_RIGHT_UP_TILE, 0, 0, 0,
    0, 0, 0, 0,
    ARROW_RIGHT_DOWN_TILE, 0, 0, 0,
    ARROW_RIGHT_DOWN_TILE, ARROW_RIGHT_DOWN_TILE, 0, 0,
    // Floor 2 -> target 0,1,2,3
    ARROW_RIGHT_UP_TILE, ARROW_RIGHT_UP_TILE, 0, 0,
    ARROW_RIGHT_UP_TILE, 0, 0, 0,
    0, 0, 0, 0,
    ARROW_RIGHT_DOWN_TILE, 0, 0, 0,
    // Floor 3 -> target 0,1,2,3
    ARROW_RIGHT_UP_TILE, ARROW_RIGHT_UP_TILE, ARROW_RIGHT_UP_TILE, 0,
    ARROW_RIGHT_UP_TILE, ARROW_RIGHT_UP_TILE, 0, 0,
    ARROW_RIGHT_UP_TILE, 0, 0, 0,
    0, 0, 0, 0
};

// Forward declarations
static void Npc_Spawn(void);
static void Npc_ProcessState(Npc *npc);
static void Npc_ProcessWalkAnimation(Npc *npc);
static void Npc_ProcessWaitAnimation(Npc *npc);
static void Npc_CalculateSpriteFramePtr(Npc *npc);
static void Npc_DrawArrows(Npc *npc);
static void Npc_ClearArrows(Npc *npc);

void Npcs_Init(void) {
    // Initialize NPC array
    memset(npcs, 0, sizeof(npcs));
    
    // Initialize linked list
    for (uint8_t i = 0; i < N_NPCS - 1; i++) {
        npcs[i].nextNpcInLine = &npcs[i + 1];
    }
    npcs[N_NPCS - 1].nextNpcInLine = NULL;
    
    nextFreeNpcPtr = &npcs[0];
    countdownToNextNpc = 1;
    feelingsSpeed = NPC_FEELINGS_SPEED;
}

void Npcs_Process(void) {
    // Process all NPCs
    for (uint8_t i = 0; i < N_NPCS; i++) {
        if (npcs[i].state != NPC_DEAD) {
            Npc_ProcessState(&npcs[i]);
        }
    }
    
    // Spawn new NPC if needed
    if (countdownToNextNpc > 0) {
        countdownToNextNpc--;
    }
    if (countdownToNextNpc == 0) {
        Npc_Spawn();
        countdownToNextNpc = Random_Rnd_U16X_U16Y(MIN_SPAWN_COUNTDOWN, MAX_SPAWN_COUNTDOWN);
    }
}

void Npcs_DrawSprites(void) {
    for (uint8_t i = 0; i < N_NPCS; i++) {
        if (npcs[i].state != NPC_DEAD && npcs[i].state != NPC_LIMBO) {
            // Extract pixel positions from fixed-point (high byte)
            // Assembly: LDA xPos + 1; STA MetaSprite__xPos; STZ MetaSprite__xPos + 1
            uint16_t xPixel = (uint16_t)(npcs[i].xPos >> 8);
            uint16_t yPixel = (uint16_t)(npcs[i].yPos >> 8);
            
            // Get frame pointer from spriteFramePtr (which is an index into businessMan table)
            // In assembly, spriteFramePtr is a pointer loaded from MetaSpriteFrameTable_npcs
            // In C, we store it as an index and look it up from the businessMan array
            const uint8_t *metaspritePtr = NULL;
            if (npcs[i].spriteFramePtr < 24) {  // 24 frames total (12 left + 12 right)
                metaspritePtr = businessMan[npcs[i].spriteFramePtr];
            }
            
            if (metaspritePtr) {
                // Assembly: LDY spriteCharAttr; LDX spriteFramePtr
                MetaSprite_ProcessMetaSprite_Y(xPixel, yPixel, npcs[i].spriteCharAttr, metaspritePtr);
            } else {
                consoleNocashMessage("NPC%d: no frame idx=%d", i, npcs[i].spriteFramePtr);
            }
        }
    }
}

void Npcs_StrikeAnimationWrongFloor(void) {
    // Assembly: strike_frameTimeout--, if == 0 then toggle arrows and reset timeout
    // strike_frame is NOT updated here - it's set once in SetWrongFloor and remains constant
    if (strike_frameTimeout > 0) {
        strike_frameTimeout--;
    }
    if (strike_frameTimeout == 0) {
        strike_frameTimeout = WRONG_FLOOR_ARROW_DELAY;
        
        if (strike_npc) {
            if (strike_hideArrowsOnZero == 0) {
                Npc_ClearArrows(strike_npc);
                strike_hideArrowsOnZero = 1;
            } else {
                Npc_DrawArrows(strike_npc);
                strike_hideArrowsOnZero = 0;
            }
        }
    }
    
    if (strike_npc) {
        uint16_t xPixel = (uint16_t)(strike_npc->xPos >> 8);
        uint16_t yPixel = (uint16_t)(strike_npc->yPos >> 8);
        
        // Get wrong floor frame pointer
        // Decode: wrongFloorIndex = strike_frame / 2, leftRightIndex = strike_frame % 2
        const uint8_t *metaspritePtr = NULL;
        if (strike_frame < 4) {  // Valid range: 0-3
            uint8_t wrongFloorIndex = strike_frame / 2;  // 0 or 1
            uint8_t leftRightIndex = strike_frame % 2;   // 0 = left, 1 = right
            
            if (wrongFloorIndex == 0) {
                metaspritePtr = wrongFloor0[leftRightIndex];
            } else {
                metaspritePtr = wrongFloor1[leftRightIndex];
            }
        }
        
        if (metaspritePtr) {
            MetaSprite_ProcessMetaSprite_Y(xPixel, yPixel, FIGHTING_CHARATTR, metaspritePtr);
        }
    }
}

void Npcs_StrikeAnimationFighting(void) {
    if (strike_frameTimeout > 0) {
        strike_frameTimeout--;
    }
    if (strike_frameTimeout == 0) {
        strike_frameTimeout = FIGHTING_FRAME_DELAY;
        
        if (strike_frame >= (N_FIGHTING_FRAMES - 1) * 2) {
            strike_frame = 0;
        } else {
            strike_frame += 2;
        }
    }
    
    if (strike_npc) {
        uint16_t xPixel = (uint16_t)(strike_npc->xPos >> 8);
        uint16_t yPixel = (uint16_t)(strike_npc->yPos >> 8);
        
        // Get fighting cloud frame pointer
        // Assembly uses MetaSprite_fightingCloud[strike_frame]
        const uint8_t *metaspritePtr = NULL;
        if (strike_frame < 10) {  // 5 frames * 2 = 10
            metaspritePtr = fightingCloud[strike_frame];
        }
        
        if (metaspritePtr) {
            MetaSprite_ProcessMetaSprite_Y(xPixel, yPixel, FIGHTING_CHARATTR, metaspritePtr);
        }
    }
}

static void Npc_ProcessState(Npc *npc) {
    switch (npc->state) {
        case NPC_DEAD:
            break;
        case NPC_WALK_TO_ELEVATOR:
            // Process walking animation
            Npc_ProcessWalkAnimation(npc);
            
            // Move towards elevator
            if (npc->leftSideOnZero == 0) {
                // Walking from left
                if (npc->xPos < (NPC_ELEVATOR_XPOS << 8)) {
                    npc->xPos += NPC_WALK_SPEED;
                } else {
                    npc->xPos = NPC_ELEVATOR_XPOS << 8;
                    npc->state = NPC_ENTER_ELEVATOR;
                    npc->stateTimer = NPC_ENTER_ELEVATOR_FRAMES;
                }
            } else {
                // Walking from right
                if (npc->xPos > (NPC_ELEVATOR_XPOS << 8)) {
                    npc->xPos -= NPC_WALK_SPEED;
                } else {
                    npc->xPos = NPC_ELEVATOR_XPOS << 8;
                    npc->state = NPC_ENTER_ELEVATOR;
                    npc->stateTimer = NPC_ENTER_ELEVATOR_FRAMES;
                }
            }
            break;
        case NPC_ENTER_ELEVATOR:
            if (npc->stateTimer > 0) {
                npc->stateTimer--;
                // Move towards elevator position
                if (npc->leftSideOnZero == 0) {
                    npc->xPos += NPC_ENTER_ELEVATOR_XSPEED;
                    npc->yPos -= NPC_ENTER_ELEVATOR_YSPEED;
                } else {
                    npc->xPos -= NPC_ENTER_ELEVATOR_XSPEED;
                    npc->yPos -= NPC_ENTER_ELEVATOR_YSPEED;
                }
            } else {
                npc->xPos = NPC_ELEVATOR_XPOS << 8;
                npc->yPos = NPC_ELEVATOR_YPOS << 8;
                npc->state = NPC_WAIT_DOOR_CLOSE;
            }
            break;
        case NPC_WAIT_DOOR_CLOSE:
            // Wait for elevator door to close
            break;
        case NPC_LIMBO:
            break;
        case NPC_WAIT_DOOR_OPEN:
            // Wait for elevator door to open
            break;
        case NPC_EXIT_ELEVATOR:
            if (npc->stateTimer > 0) {
                npc->stateTimer--;
                // Move away from elevator
                if (npc->leftSideOnZero == 0) {
                    npc->xPos += NPC_EXIT_ELEVATOR_XSPEED;
                    npc->yPos += NPC_EXIT_ELEVATOR_YSPEED;
                } else {
                    npc->xPos -= NPC_EXIT_ELEVATOR_XSPEED;
                    npc->yPos += NPC_EXIT_ELEVATOR_YSPEED;
                }
            } else {
                npc->state = NPC_WALK_OFFSCREEN;
            }
            break;
        case NPC_WALK_OFFSCREEN:
            // Process walking animation
            Npc_ProcessWalkAnimation(npc);
            
            // Move offscreen
            if (npc->leftSideOnZero == 0) {
                npc->xPos += NPC_WALK_OFFSCREEN_SPEED;
                if (npc->xPos > ((uint16_t)NPC_RIGHT_OFFSCREEN_XPOS << 8)) {
                    npc->state = NPC_DEAD;
                }
            } else {
                npc->xPos -= NPC_WALK_OFFSCREEN_SPEED;
                if (npc->xPos < ((int16_t)NPC_LEFT_OFFSCREEN_XPOS << 8)) {
                    npc->state = NPC_DEAD;
                }
            }
            break;
        case NPC_FIGHTING:
            // Process wait animation
            Npc_ProcessWaitAnimation(npc);
            break;
        case NPC_WRONG_FLOOR:
            // Process wait animation
            Npc_ProcessWaitAnimation(npc);
            break;
    }
}

static void Npc_ProcessWalkAnimation(Npc *npc) {
    if (npc->animationCounter > 0) {
        npc->animationCounter--;
    }
    
    if (npc->animationCounter == 0) {
        npc->animationCounter = NPC_MOVE_FRAME_DELAY;
        npc->frame = (npc->frame == NPC_FRAME_STAND) ? NPC_FRAME_WALK : NPC_FRAME_STAND;
        Npc_CalculateSpriteFramePtr(npc);
    }
}

static void Npc_ProcessWaitAnimation(Npc *npc) {
    if (npc->animationCounter > 0) {
        npc->animationCounter--;
    }
    
    if (npc->animationCounter == 0) {
        npc->animationCounter = NPC_MOVE_FRAME_DELAY;
        npc->frame = (npc->frame == NPC_FRAME_WAIT) ? NPC_FRAME_STAND : NPC_FRAME_WAIT;
        Npc_CalculateSpriteFramePtr(npc);
    }
}

static void Npc_CalculateSpriteFramePtr(Npc *npc) {
    // Assembly: frameIndex = (facingLeftOnZero ? 12 : 0) + frame + feelings * 3
    // frameIndex is then used as an offset into the businessMan table
    // In C, we store frameIndex directly in spriteFramePtr
    uint8_t base = (npc->facingLeftOnZero == 0) ? 0 : 12;
    uint8_t frameIndex = base + npc->frame + npc->feelings * 3;
    npc->spriteFramePtr = frameIndex;
}

static void Npc_DrawArrows(Npc *npc) {
    // Calculate tile index: floor * 16 + targetFloor * 4 (each combination has 4 tiles)
    uint8_t tileIndex = npc->floor * 16 + npc->targetFloor * 4;
    
    if (npc->leftSideOnZero == 0) {
        // Left side arrows
        uint16_t baseLoc = ArrowsLocationsLeft[npc->floor];
        for (uint8_t i = 0; i < 4; i++) {
            if (ArrowsTilesLeft[tileIndex + i] != 0) {
                interactiveBgBuffer[baseLoc + i] = ArrowsTilesLeft[tileIndex + i];
            }
        }
    } else {
        // Right side arrows
        uint16_t baseLoc = ArrowsLocationsRight[npc->floor];
        for (uint8_t i = 0; i < 4; i++) {
            if (ArrowsTilesRight[tileIndex + i] != 0) {
                interactiveBgBuffer[baseLoc + i] = ArrowsTilesRight[tileIndex + i];
            }
        }
    }
}

static void Npc_ClearArrows(Npc *npc) {
    if (npc->leftSideOnZero == 0) {
        // Left side arrows
        uint16_t baseLoc = ArrowsLocationsLeft[npc->floor];
        for (uint8_t i = 0; i < 4; i++) {
            interactiveBgBuffer[baseLoc + i] = 0;
        }
    } else {
        // Right side arrows
        uint16_t baseLoc = ArrowsLocationsRight[npc->floor];
        for (uint8_t i = 0; i < 4; i++) {
            interactiveBgBuffer[baseLoc + i] = 0;
        }
    }
}

static void Npc_Spawn(void) {
    // Find free NPC
    Npc *npc = nextFreeNpcPtr;
    if (!npc) {
        return; // No free NPCs
    }
    
    // Update next free pointer
    nextFreeNpcPtr = npc->nextNpcInLine;
    
    // Initialize NPC
    npc->state = NPC_WALK_TO_ELEVATOR;
    npc->leftSideOnZero = Random_Rnd_2();
    npc->floor = Random_Rnd_U8A(N_FLOORS);
    npc->targetFloor = Random_Rnd_U8A(N_FLOORS);
    npc->facingLeftOnZero = npc->leftSideOnZero;
    npc->frame = NPC_FRAME_STAND;
    npc->feelings = NPC_FEELINGS_NORMAL;
    npc->feelingsTimeout = NPC_FEELINGS_TIMEOUT;
    npc->spriteCharAttr = NPC_SPRITE_CHARATTR;
    npc->spriteFrameTablePtr = 0;  // Not used in C (we use array directly)
    npc->animationCounter = NPC_MOVE_FRAME_DELAY;
    npc->stateTimer = 0;
    
    // Set initial position
    if (npc->leftSideOnZero == 0) {
        npc->xPos = ((int16_t)NPC_LEFT_XSTART) << 8;
    } else {
        npc->xPos = ((uint16_t)NPC_RIGHT_XSTART) << 8;
    }
    npc->yPos = NPC_YSTART << 8;
    
    // Calculate initial frame pointer
    Npc_CalculateSpriteFramePtr(npc);
}

void Npcs_OccupiedElevatorDoorOpening(void *npc, uint8_t floor) {
    if (!npc) return;
    
    Npc *n = (Npc *)npc;
    
    // Check if NPC is at target floor
    if (n->targetFloor == floor) {
        n->state = NPC_EXIT_ELEVATOR;
        n->stateTimer = NPC_EXIT_ELEVATOR_FRAMES;
        
        // Calculate exit position
        if (n->leftSideOnZero == 0) {
            n->xPos = NPC_WALK_OFFSCREEN_XPOS << 8;
        } else {
            n->xPos = NPC_WALK_OFFSCREEN_XPOS << 8;
        }
        n->yPos = NPC_WALK_OFFSCREEN_YPOS << 8;
    } else {
        // Wrong floor - strike animation
        // Assembly sets Game__strikeAntimationRoutinePtr, but we use enum instead
        // This is handled by the game loop checking Game_strikeAnimationType
        // Assembly: SetWrongFloor sets strike_frameTimeout = 1 and initializes strike_frame
        Game_strikeAnimationType = STRIKE_ANIMATION_WRONG_FLOOR;
        strike_npc = n;
        strike_hideArrowsOnZero = 0;
        strike_frameTimeout = 1;  // Assembly sets this to 1, not 0
        
        // Initialize strike_frame based on leftSideOnZero
        // Encoding: 0 = wrongFloor0_left, 1 = wrongFloor0_right, 2 = wrongFloor1_left, 3 = wrongFloor1_right
        if (n->leftSideOnZero == 0) {
            // Left side (index 0)
            uint8_t wrongFloorIndex = (Random_Rnd_2() == 0) ? 0 : 1;
            strike_frame = wrongFloorIndex * 2 + 0;  // left = 0
        } else {
            // Right side (index 1)
            uint8_t wrongFloorIndex = (Random_Rnd_2() == 0) ? 0 : 1;
            strike_frame = wrongFloorIndex * 2 + 1;  // right = 1
        }
    }
}

void Npcs_OccupiedElevatorOpen(void *npc, uint8_t floor) {
    if (!npc) return;
    
    Npc *n = (Npc *)npc;
    
    // Set NPC position to elevator floor
    n->yPos = (NPC_ELEVATOR_YPOS + floor * ELEVATOR_FLOOR_COLUMN_SPACING * 8) << 8;
    n->state = NPC_WAIT_DOOR_OPEN;
}
