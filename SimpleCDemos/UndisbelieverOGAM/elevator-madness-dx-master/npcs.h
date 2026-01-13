#ifndef NPCS_H
#define NPCS_H

#include <stdint.h>
#include "game.h"
#include "elevators.h"

// NPC states
typedef enum {
    NPC_DEAD = 0,
    NPC_WALK_TO_ELEVATOR = 2,
    NPC_ENTER_ELEVATOR = 4,
    NPC_WAIT_DOOR_CLOSE = 6,
    NPC_LIMBO = 8,
    NPC_WAIT_DOOR_OPEN = 10,
    NPC_EXIT_ELEVATOR = 12,
    NPC_WALK_OFFSCREEN = 14,
    NPC_FIGHTING = 16,
    NPC_WRONG_FLOOR = 18
} NpcState;

// NPC frames
typedef enum {
    NPC_FRAME_STAND = 0,
    NPC_FRAME_WALK = 1,
    NPC_FRAME_WAIT = 2
} NpcFrame;

// NPC feelings
typedef enum {
    NPC_FEELINGS_NORMAL = 0,
    NPC_FEELINGS_STRESSED = 1,
    NPC_FEELINGS_ANGRY = 2,
    NPC_FEELINGS_FIGHTING = 3
} NpcFeelings;

// Constants
#define N_NPCS 12

#define NPC_FEELINGS_TIMEOUT    0xFFFF
#define NPC_FEELINGS_SPEED      150
#define NPC_MAX_FEELINGS_SPEED  400

#define MIN_SPAWN_COUNTDOWN     (4 * 60)
#define MAX_SPAWN_COUNTDOWN     (10 * 60)

#define WRONG_FLOOR_ARROW_DELAY 30

#define FIGHTING_FRAME_DELAY    9
#define FIGHTING_CHARATTR       ((1 << 10) | (3 << 13) | 32)  // palette, order, char
#define N_FIGHTING_FRAMES       5

#define NPC_LINE_SPACING        18
#define NPC_LEFT_WAIT_XPOS      60
#define NPC_RIGHT_WAIT_XPOS     (256 - NPC_LEFT_WAIT_XPOS)

#define NPC_YSTART              60
#define NPC_LEFT_XSTART         (-10)
#define NPC_RIGHT_XSTART        (255 + 10)
#define NPC_ELEVATOR_YPOS       52
#define NPC_ELEVATOR_XPOS       72

#define NPC_WALK_OFFSCREEN_YPOS 64
#define NPC_WALK_OFFSCREEN_XPOS 64
#define NPC_LEFT_OFFSCREEN_XPOS (-10)
#define NPC_RIGHT_OFFSCREEN_XPOS (256 + 10)

#define NPC_WALK_SPEED          65
#define NPC_WALK_OFFSCREEN_SPEED 100
#define NPC_MOVE_FRAME_DELAY    12

#define NPC_ENTER_ELEVATOR_FRAMES 20
#define NPC_EXIT_ELEVATOR_FRAMES  20

// Calculated speeds (fixed-point 16.8)
#define NPC_ENTER_ELEVATOR_XSPEED ((NPC_ELEVATOR_XPOS - NPC_LEFT_WAIT_XPOS) * 256 / NPC_ENTER_ELEVATOR_FRAMES)
#define NPC_ENTER_ELEVATOR_YSPEED ((NPC_YSTART - NPC_ELEVATOR_YPOS) * 256 / NPC_ENTER_ELEVATOR_FRAMES)
#define NPC_EXIT_ELEVATOR_XSPEED  ((NPC_ELEVATOR_XPOS - NPC_WALK_OFFSCREEN_XPOS) * 256 / NPC_EXIT_ELEVATOR_FRAMES)
#define NPC_EXIT_ELEVATOR_YSPEED  ((NPC_WALK_OFFSCREEN_YPOS - NPC_ELEVATOR_YPOS) * 256 / NPC_EXIT_ELEVATOR_FRAMES)

// Arrow tile constants
#define ARROW_LEFT_UP_TILE    (27 | (7 << 10) | 0x2000)
#define ARROW_LEFT_DOWN_TILE  (26 | (7 << 10) | 0x2000)
#define ARROW_RIGHT_UP_TILE   (29 | (7 << 10) | 0x2000)
#define ARROW_RIGHT_DOWN_TILE (28 | (7 << 10) | 0x2000)

void Npcs_Init(void);
void Npcs_Process(void);
void Npcs_DrawSprites(void);
void Npcs_StrikeAnimationWrongFloor(void);
void Npcs_StrikeAnimationFighting(void);

// Elevator interaction functions
void Npcs_OccupiedElevatorDoorOpening(void *npc, uint8_t floor);
void Npcs_OccupiedElevatorOpen(void *npc, uint8_t floor);

#endif // NPCS_H
