#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include "snes_regs_xc.h"

// Game constants
#define N_FLOORS 4

// VRAM Map (word addresses)
#define GAME_BG1_MAP    0x0000
#define GAME_BG2_MAP    0x0400
#define GAME_BG3_MAP    0x0800
#define GAME_BG1_TILES  0x5000
#define GAME_BG2_TILES  0x1000
#define GAME_OAM_TILES  0x6000

// BG sizes (32x32 = 0)
#define GAME_BG1_SIZE   0x00
#define GAME_BG2_SIZE   0x00
#define GAME_BG3_SIZE   0x00

// OAM size (8x8 and 16x16 = 0)
#define GAME_OAM_SIZE   0x00
#define GAME_OAM_NAME   0

// Game constants
#define N_STRIKES               3
#define STRIKE_MIN_FRAMES       (1 * 60)
#define STRIKE_TIMEOUT          (9 * 60)
#define GAME_OVER_FADEOUT_SPEED 5

// Score display constants
#define SCORE_DIGIT_DELTA    16
#define SCORE_TILE_COLUMN    2
#define SCORE_TILE_ROW       14
#define SCORE_TILE_ATTR      (7 << 10)  // TILEMAP_PALETTE_SHIFT = 10

// Strikes display constants
#define STRIKES_TILE_COLUMN      3
#define STRIKES_TILE_ROW        14
#define STRIKES_TILE_CHARATTR   (7 << 10)  // TILEMAP_PALETTE_SHIFT = 10

#define STRIKES_FULL_TOP_TILE     14
#define STRIKES_FULL_BOTTOM_TILE  30
#define STRIKES_HALF_TOP_TILE     15
#define STRIKES_HALF_BOTTOM_TILE  31

// TILEMAP flags
#define TILEMAP_PALETTE_SHIFT    10
#define TILEMAP_HIGH_PALETTE_SHIFT 2
#define TILEMAP_ORDER_FLAG       0x2000
#define TILEMAP_H_FLIP_FLAG      0x4000

// Strike animation types (replaces function pointer)
typedef enum {
    STRIKE_ANIMATION_NONE = 0,
    STRIKE_ANIMATION_ROPE_BREAKING,
    STRIKE_ANIMATION_WRONG_FLOOR,
    STRIKE_ANIMATION_FIGHTING
} StrikeAnimationType;

// Game module state
extern uint8_t Game_strikes;
extern uint16_t Game_score;
extern StrikeAnimationType Game_strikeAnimationType;
extern uint16_t Game_strikeTimeout;

// Interactive background buffer (32x32 tiles, 2 bytes per tile)
extern uint16_t interactiveBgBuffer[32 * 32];
extern uint8_t updateBgBufferOnZero;

// Game functions
void Game_PlayGame(void);
void Game_InitForLife(void);
void Game_Loop(void);
void Game_StrikeAnimation(void);
void Game_StrikeAnimationFrame(void);
void Game_DrawScore(void);
void Game_DrawStrikes(void);
void Game_SetupScreen(void);

#endif // GAME_H
