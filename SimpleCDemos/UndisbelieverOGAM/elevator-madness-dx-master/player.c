#include "player.h"
#include "metasprite.h"
#include "controller.h"
#include "elevators.h"
#include "resources.h"
#include "resources/metasprites/metasprite_data.h"  // Include for frame tables and constants
#include <string.h>

// Forward declaration for debug function
extern void consoleNocashMessage(const char *format, ...);

// Player state
static uint16_t player_xPos;      // Fixed-point 16.8 (high byte = pixel position)
static uint16_t player_yPos;      // Fixed-point 16.8
static int16_t player_xVelocity;
static int16_t player_yVelocity;
static uint8_t player_counter;
static uint8_t player_standingIfZero;  // 0 = standing, non-zero = falling
static uint8_t player_facingRightOnZero; // 0 = right, non-zero = left
static PlayerState player_state;
static uint8_t player_metaSpriteFrameIndex = PLAYER_STANDRIGHT; // Index into player frame table

// Rafter table (6 rows * (1 + 5 + 1 + 5) = 6 * 12 = 72 bytes)
// Each byte: 0 = no rafter, 1 = left/right rafters, 2 = center rafter
static const uint8_t RafterTable[72] = {
    // Row 0-5: pattern repeats 6 times
    1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,  // Row 0
    1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,  // Row 1
    1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,  // Row 2
    1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,  // Row 3
    1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,  // Row 4
    1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0   // Row 5
};

// Use frame index constants from metasprite_data.h

// Internal functions
static void Player_SetWalkingState(void);
static void Player_ContinueWalking(void);
static void Player_SetPushButtonState(void);
static void Player_ContinuePushButton(void);
static void Player_SetPlayerZapped(void);
static void Player_ContinueZapped(void);
static void Player_SetJumpingState(void);
static void Player_ContinueJumping(void);
static void Player_SetFallingState(void);
static void Player_ContinueFalling(void);
static void Player_CheckWallCollision(void);

void Player_Init(void) {
    player_xPos = PLAYER_START_X_POS << 8;
    player_yPos = PLAYER_START_Y_POS << 8;
    player_xVelocity = 0;
    player_yVelocity = 0;
    player_counter = 0;
    player_facingRightOnZero = 0;
    player_state = PLAYER_WALKING;
    player_metaSpriteFrameIndex = PLAYER_STANDRIGHT;
}

void Player_Process(void) {
    // Process current state
    switch (player_state) {
        case PLAYER_WALKING:
            Player_ContinueWalking();
            break;
        case PLAYER_PUSH_BUTTON:
            Player_ContinuePushButton();
            break;
        case PLAYER_ZAPPED:
            Player_ContinueZapped();
            break;
        case PLAYER_JUMPING:
            Player_ContinueJumping();
            break;
        case PLAYER_FALLING:
            Player_ContinueFalling();
            break;
    }
    
    // Update positions (fixed-point 16.8 math)
    // xPos += xVelocity (signed addition)
    int32_t xPos32 = (int32_t)(int16_t)player_xPos + player_xVelocity;
    if (xPos32 < 0) xPos32 = 0;
    if (xPos32 > 0xFFFF) xPos32 = 0xFFFF;
    player_xPos = (uint16_t)xPos32;
    
    // yPos += yVelocity (signed addition)
    int32_t yPos32 = (int32_t)(int16_t)player_yPos + player_yVelocity;
    if (yPos32 < 0) yPos32 = 0;
    if (yPos32 > 0xFFFF) yPos32 = 0xFFFF;
    player_yPos = (uint16_t)yPos32;
    
    Player_CheckWallCollision();
}

void Player_DrawSprites(void) {
    // Extract pixel positions from fixed-point (high byte)
    // Assembly: LDA xPos + 1; STA MetaSprite__xPos; STZ MetaSprite__xPos + 1
    uint16_t xPixel = (uint16_t)(player_xPos >> 8);
    uint16_t yPixel = (uint16_t)(player_yPos >> 8);
    
    // Get metasprite frame pointer from frame table
    // Assembly: LDX metaSpriteFrame (which is a pointer to metasprite data)
    // In C, we use an index and look it up from the table
    const uint8_t *metaspritePtr = NULL;
    if (player_metaSpriteFrameIndex < 14) {  // 14 player frames total
        metaspritePtr = player[player_metaSpriteFrameIndex];
    }
    
    if (metaspritePtr) {
        // Assembly: LDY #0 (charAttr = 0 for player)
        MetaSprite_ProcessMetaSprite_Y(xPixel, yPixel, 0, metaspritePtr);
    } else {
        consoleNocashMessage("Player: no frame ptr idx=%d", player_metaSpriteFrameIndex);
    }
}

static void Player_SetWalkingState(void) {
    player_state = PLAYER_WALKING;
    player_counter = 1;
    Player_ContinueWalking();
}

static void Player_ContinueWalking(void) {
    // Check if on ground
    if (player_standingIfZero != 0) {
        Player_SetFallingState();
        return;
    }
    
    // Check for elevator buttons
    if (Controller_pressed & (BUTTON_ELEVATOR_DOOR | BUTTON_ELEVATOR_UP | BUTTON_ELEVATOR_DOWN)) {
        Player_SetPushButtonState();
        return;
    }
    
    // Check for jump
    if (Controller_pressed & BUTTON_JUMP) {
        Player_SetJumpingState();
        return;
    }
    
    // Check for left/right movement
    uint8_t joyH = (uint8_t)(Controller_current >> 8);
    
    if (joyH & 0x02) { // JOYH_LEFT
        // Left pressed
        if (player_facingRightOnZero == 0) {
            // Was facing right, now left - turn
            player_metaSpriteFrameIndex = PLAYER_TURNRIGHT;
            player_facingRightOnZero = 1;
            player_counter = PLAYER_TURN_ANIMATION_DELAY;
        }
        
        player_xVelocity = -(int16_t)PLAYER_WALK_VELOCITY;
        
        if (player_counter > 0) {
            player_counter--;
        }
        if (player_counter == 0) {
            if (player_metaSpriteFrameIndex == PLAYER_STANDLEFT) {
                player_metaSpriteFrameIndex = PLAYER_WALKLEFT;
            } else {
                player_metaSpriteFrameIndex = PLAYER_STANDLEFT;
            }
            player_counter = PLAYER_WALK_ANIMATION_DELAY;
        }
    } else if (joyH & JOY_RIGHT_MASK) {
        // Right pressed
        if (player_facingRightOnZero != 0) {
            // Was facing left, now right - turn
            player_metaSpriteFrameIndex = PLAYER_TURNLEFT;
            player_facingRightOnZero = 0;
            player_counter = PLAYER_TURN_ANIMATION_DELAY;
        }
        
        player_xVelocity = PLAYER_WALK_VELOCITY;
        
        if (player_counter > 0) {
            player_counter--;
        }
        if (player_counter == 0) {
            if (player_metaSpriteFrameIndex == PLAYER_STANDRIGHT) {
                player_metaSpriteFrameIndex = PLAYER_WALKRIGHT;
            } else {
                player_metaSpriteFrameIndex = PLAYER_STANDRIGHT;
            }
            player_counter = PLAYER_WALK_ANIMATION_DELAY;
        }
    } else {
        // Standing still
        if (player_facingRightOnZero != 0) {
            player_metaSpriteFrameIndex = PLAYER_STANDLEFT;
        } else {
            player_metaSpriteFrameIndex = PLAYER_STANDRIGHT;
        }
        player_xVelocity = 0;
    }
}

static void Player_SetPushButtonState(void) {
    // Check if standing
    if (player_standingIfZero != 0) {
        return;
    }
    
    // Check if in front of switch
    uint8_t xPixel = (uint8_t)(player_xPos >> 8);
    uint8_t side = 0; // 0 = left, 1 = right
    
    if (xPixel < SWITCH_LEFT_LIMIT && player_facingRightOnZero != 0) {
        side = 0; // left
    } else if (xPixel >= SWITCH_RIGHT_LIMIT && player_facingRightOnZero == 0) {
        side = 1; // right
    } else {
        return; // Not in front of switch
    }
    
    // Check which button was pressed
    uint8_t success = 0;
    
    if (Controller_current & BUTTON_ELEVATOR_DOOR) {
        // Calculate floor (HACK: yPos / 64)
        uint8_t floor = (uint8_t)(player_yPos >> 14); // Divide by 64 (>> 6, but we have << 8, so >> 14)
        success = Elevators_PlayerPressDoor(side, floor);
    } else if (Controller_current & BUTTON_ELEVATOR_UP) {
        success = Elevators_PlayerPressUp(side);
    } else if (Controller_current & BUTTON_ELEVATOR_DOWN) {
        success = Elevators_PlayerPressDown(side);
    } else {
        return;
    }
    
    if (!success) {
        Player_SetPlayerZapped();
        return;
    }
    
    // Push successful
    player_state = PLAYER_PUSH_BUTTON;
    player_xVelocity = 0;
    player_counter = PLAYER_BUTTON_ANIMATION_DELAY;
    
    if (player_facingRightOnZero != 0) {
        player_metaSpriteFrameIndex = PLAYER_PUSHBUTTONLEFT;
    } else {
        player_metaSpriteFrameIndex = PLAYER_PUSHBUTTONRIGHT;
    }
}

static void Player_ContinuePushButton(void) {
    if (player_counter > 0) {
        player_counter--;
    }
    if (player_counter == 0) {
        Player_SetWalkingState();
    }
}

static void Player_SetPlayerZapped(void) {
    player_state = PLAYER_ZAPPED;
    player_xVelocity = 0;
    player_counter = PLAYER_ZAPPED_ANIMATION_DELAY;
    
    if (player_facingRightOnZero != 0) {
        player_metaSpriteFrameIndex = PLAYER_ZAPPEDLEFT;
    } else {
        player_metaSpriteFrameIndex = PLAYER_ZAPPEDRIGHT;
    }
}

static void Player_ContinueZapped(void) {
    if (player_counter > 0) {
        player_counter--;
    }
    if (player_counter == 0) {
        Player_SetWalkingState();
    }
}

static void Player_SetJumpingState(void) {
    player_state = PLAYER_JUMPING;
    player_yVelocity = -(int16_t)PLAYER_JUMP_VELOCITY;
    player_counter = PLAYER_JUMP_HOLD;
    player_standingIfZero = 1;
    
    if (player_facingRightOnZero != 0) {
        player_metaSpriteFrameIndex = PLAYER_JUMPLEFT;
    } else {
        player_metaSpriteFrameIndex = PLAYER_JUMPRIGHT;
    }
}

static void Player_ContinueJumping(void) {
    // Check if jump button released
    if (!(Controller_current & BUTTON_JUMP)) {
        player_counter = 0;
    }
    
    if (player_counter > 0) {
        player_counter--;
        // Keep jump velocity
        player_yVelocity = -(int16_t)PLAYER_JUMP_VELOCITY;
    } else {
        // Apply gravity
        player_yVelocity += GRAVITY_PER_FRAME;
        
        // Check if falling
        if (player_yVelocity >= 0) {
            Player_SetFallingState();
            return;
        }
    }
    
    // Handle horizontal movement in air
    uint8_t joyH = (uint8_t)(Controller_current >> 8);
    
    if (joyH & 0x02) { // JOYH_LEFT
        player_xVelocity = -(int16_t)PLAYER_AIR_WALK_VELOCITY;
        player_metaSpriteFrameIndex = PLAYER_JUMPLEFT;
    } else if (joyH & 0x01) { // JOYH_RIGHT
        player_xVelocity = PLAYER_AIR_WALK_VELOCITY;
        player_metaSpriteFrameIndex = PLAYER_JUMPRIGHT;
    }
}

static void Player_SetFallingState(void) {
    player_state = PLAYER_FALLING;
    
    if (player_facingRightOnZero != 0) {
        player_metaSpriteFrameIndex = PLAYER_FALLLEFT;
    } else {
        player_metaSpriteFrameIndex = PLAYER_FALLRIGHT;
    }
}

static void Player_ContinueFalling(void) {
    // Check if landed
    if (player_standingIfZero == 0) {
        Player_SetWalkingState();
        return;
    }
    
    // Apply gravity
    player_yVelocity += GRAVITY_PER_FRAME;
    
    // Handle horizontal movement in air
    uint8_t joyH = (uint8_t)(Controller_current >> 8);
    
    if (joyH & 0x02) { // JOYH_LEFT
        player_xVelocity = -(int16_t)PLAYER_AIR_WALK_VELOCITY;
        player_metaSpriteFrameIndex = PLAYER_FALLLEFT;
    } else if (joyH & 0x01) { // JOYH_RIGHT
        player_xVelocity = PLAYER_AIR_WALK_VELOCITY;
        player_metaSpriteFrameIndex = PLAYER_FALLRIGHT;
    }
}

static void Player_CheckWallCollision(void) {
    uint8_t xPixel = (uint8_t)(player_xPos >> 8);
    uint8_t yPixel = (uint8_t)(player_yPos >> 8);
    
    // Check X bounds
    if (xPixel < PLAYER_LEFT_LIMIT) {
        player_xPos = PLAYER_LEFT_LIMIT << 8;
        player_xVelocity = 0;
    } else if (xPixel >= PLAYER_RIGHT_LIMIT) {
        player_xPos = PLAYER_RIGHT_LIMIT << 8;
        player_xVelocity = 0;
    }
    
    // Check Y bounds and rafters
    if (yPixel >= BOTTOM_RAFTER) {
        player_yPos = BOTTOM_RAFTER << 8;
        player_yVelocity = 0;
        player_standingIfZero = 0; // Standing
        return;
    }
    
    if (yPixel < TOP_RAFTER) {
        player_standingIfZero = 1; // Falling
        return;
    }
    
    // Check rafters (only if not jumping)
    if (player_state != PLAYER_JUMPING) {
        uint8_t yRow = (yPixel - TOP_RAFTER) / 4;
        if (yRow < 72) {
            uint8_t rafterType = RafterTable[yRow];
            
            if (rafterType == 0) {
                player_standingIfZero = 1; // No rafter - falling
            } else if (rafterType == 1) {
                // Left & right rafters with center hole
                if (xPixel < RAFTER_LEFT_POS || xPixel >= RAFTER_RIGHT_POS) {
                    // On rafter
                    player_standingIfZero = 0;
                    player_yVelocity = 0;
                    player_yPos = (TOP_RAFTER + yRow * 4) << 8;
                } else {
                    player_standingIfZero = 1; // In center hole - falling
                }
            } else { // rafterType == 2
                // Center rafter with left & right holes
                if (xPixel >= RAFTER_CENTER_LEFT && xPixel < RAFTER_CENTER_RIGHT) {
                    // On center rafter
                    player_standingIfZero = 0;
                    player_yVelocity = 0;
                    player_yPos = (TOP_RAFTER + yRow * 4) << 8;
                } else {
                    player_standingIfZero = 1; // In side holes - falling
                }
            }
        }
    }
}
