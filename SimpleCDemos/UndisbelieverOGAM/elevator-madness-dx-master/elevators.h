#ifndef ELEVATORS_H
#define ELEVATORS_H

#include <stdint.h>
#include "game.h"
#include "npcs.h"

// Elevator states
typedef enum {
    ELEVATOR_DOOR_CLOSED = 0,
    ELEVATOR_DOOR_OPENING = 2,
    ELEVATOR_DOOR_OPEN = 4,
    ELEVATOR_DOOR_CLOSING = 6,
    ELEVATOR_MOVING_UP = 8,
    ELEVATOR_MOVING_DOWN = 10,
    ELEVATOR_CRASHED = 12,
    ELEVATOR_NPC_ENTERING = 14,
    ELEVATOR_NPC_LEAVING = 16
} ElevatorState;

// Constants
#define N_DOOR_FRAMES 16
#define N_ELEVATOR_MOVING_FRAMES 40
#define DOOR_CLOSED_FRAME 0
#define DOOR_OPEN_FRAME (N_DOOR_FRAMES - 1)

#define ELEVATOR_DOOR_HEIGHT 4
#define ELEVATOR_DOOR_ROWS 4
#define ELEVATOR_DOOR_TILE_CLEAR 0
#define ELEVATOR_DOOR_TILE_OFFSET 1
#define ELEVATOR_TILE_CHARATTR (7 << 10) | 0x2000  // palette shift 10, order flag

#define ELEVATOR_ROPE_TILE_MASK 0x07
#define ELEVATOR_ROPE_FRAME_DELAY 4
#define ELEVATOR_ROPE_HEIGHT 28
#define ELEVATOR_LEFT_ROPE_TILE_OFFSET (32 | (7 << 10))
#define ELEVATOR_RIGHT_ROPE_TILE_OFFSET (32 | (7 << 10) | 0x4000)  // with H_FLIP

#define ELEVATOR_LIGHT_LEFT_TILE (10 | (7 << 10) | 0x2000)
#define ELEVATOR_LIGHT_RIGHT_TILE (11 | (7 << 10) | 0x2000)
#define ELEVATOR_SWITCH_OFF_TILE (12 | (7 << 10) | 0x2000)
#define ELEVATOR_SWITCH_ON_TILE (13 | (7 << 10) | 0x2000)

#define ELEVATOR_FLOOR_0_COLUMN 2
#define ELEVATOR_FLOOR_COLUMN_SPACING 6
#define ELEVATOR_SWITCH_COLUMN 4
#define ELEVATOR_LIGHT_COLUMN 0
#define ELEVATOR_ARROW_COLUMN 1
#define ELEVATOR_DOOR_COLUMN 1

#define ELEVATOR_LEFT_ROPE_ROW 11
#define ELEVATOR_LEFT_SWITCH_ROW 12
#define ELEVATOR_LEFT_LIGHT_ROW 8
#define ELEVATOR_LEFT_ARROW_ROW 6
#define ELEVATOR_LEFT_DOOR_ROW 7

#define ELEVATOR_RIGHT_ROPE_ROW 20
#define ELEVATOR_RIGHT_SWITCH_ROW 19
#define ELEVATOR_RIGHT_DOOR_ROW 21
#define ELEVATOR_RIGHT_LIGHT_ROW 22
#define ELEVATOR_RIGHT_ARROW_ROW 26

// Forward declarations
void Npcs_OccupiedElevatorDoorOpening(void *npc, uint8_t floor);
void Npcs_OccupiedElevatorOpen(void *npc, uint8_t floor);

void Elevators_Init(void);
void Elevators_Process(void);
void Elevators_StrikeAnimationRopeBreaking(void);

// Player interaction functions (side: 0 = left, 1 = right)
uint8_t Elevators_PlayerPressDoor(uint8_t side, uint8_t floor);
uint8_t Elevators_PlayerPressUp(uint8_t side);
uint8_t Elevators_PlayerPressDown(uint8_t side);

// NPC interaction functions (side: 0 = left, 1 = right)
uint8_t Elevators_NpcEnterElevator(void *npc, uint8_t floor, uint8_t side);
void Elevators_NpcInsideElevator(void *npc, uint8_t side);

#endif // ELEVATORS_H
