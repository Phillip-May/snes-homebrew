#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "game.h"
#include "controller.h"
#include "elevators.h"

// Player constants
#define PLAYER_START_X_POS  128
#define PLAYER_START_Y_POS  199

#define PLAYER_LEFT_LIMIT   100
#define PLAYER_RIGHT_LIMIT  156

#define TOP_RAFTER    55
#define BOTTOM_RAFTER 199
#define RAFTER_SPACING 6  // in 4 pixel blocks

#define RAFTER_LEFT_POS    120
#define RAFTER_RIGHT_POS   140
#define RAFTER_CENTER_LEFT  112
#define RAFTER_CENTER_RIGHT 146

#define SWITCH_LEFT_LIMIT   (PLAYER_LEFT_LIMIT + 10)
#define SWITCH_RIGHT_LIMIT  (PLAYER_RIGHT_LIMIT - 10)

#define PLAYER_WALK_VELOCITY      225  // 1/256 pixels per frame
#define PLAYER_AIR_WALK_VELOCITY  155  // 1/256 pixels per frame
#define PLAYER_JUMP_VELOCITY      500  // 1/256 pixels per frame
#define PLAYER_JUMP_HOLD          20   // frames
#define GRAVITY_PER_FRAME         41   // 1/256 pixels per frame per frame

#define PLAYER_WALK_ANIMATION_DELAY   10
#define PLAYER_TURN_ANIMATION_DELAY   7
#define PLAYER_BUTTON_ANIMATION_DELAY 6
#define PLAYER_ZAPPED_ANIMATION_DELAY 30

// Player states
typedef enum {
    PLAYER_WALKING = 0,
    PLAYER_PUSH_BUTTON = 2,
    PLAYER_ZAPPED = 4,
    PLAYER_JUMPING = 6,
    PLAYER_FALLING = 8
} PlayerState;

// Forward declarations for elevator functions
uint8_t Elevators_PlayerPressDoor(uint8_t side, uint8_t floor);
uint8_t Elevators_PlayerPressUp(uint8_t side);
uint8_t Elevators_PlayerPressDown(uint8_t side);

void Player_Init(void);
void Player_Process(void);
void Player_DrawSprites(void);

#endif // PLAYER_H
