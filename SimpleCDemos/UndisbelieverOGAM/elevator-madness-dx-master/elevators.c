#include "elevators.h"
#include "game.h"
#include "npcs.h"
#include <string.h>

// Elevator structure
typedef struct {
    ElevatorState state;
    uint16_t floor;
    uint8_t ropePos;
    uint8_t counter;
    void *occupant;  // Pointer to NPC, or NULL
    // Tile table is stored as an index (0 = left, 1 = right)
    uint8_t tileTableIndex;
} Elevator;

// Two elevator instances
static Elevator leftElevator;
static Elevator rightElevator;

// Strike animation rope tile position
static uint16_t strikeAnimationRopeTile = 0;

// Tile position table structure
typedef struct {
    uint16_t doorsPos[4];      // N_FLOORS
    uint16_t lightsPos[4];     // N_FLOORS
    uint16_t switchesPos[4];   // N_FLOORS
    uint16_t ropePos;
    uint16_t ropeTileOffset;
    uint16_t switchOnTile;
    uint16_t switchOffTile;
} ElevatorTilePositionTable;

// Left elevator tile position table
// Tile positions are calculated as: (column * 32 + row) where column and row are tile coordinates
static const ElevatorTilePositionTable leftTileTable = {
    .doorsPos = {
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 0 + ELEVATOR_DOOR_COLUMN) * 32 + ELEVATOR_LEFT_DOOR_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 1 + ELEVATOR_DOOR_COLUMN) * 32 + ELEVATOR_LEFT_DOOR_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 2 + ELEVATOR_DOOR_COLUMN) * 32 + ELEVATOR_LEFT_DOOR_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 3 + ELEVATOR_DOOR_COLUMN) * 32 + ELEVATOR_LEFT_DOOR_ROW)
    },
    .lightsPos = {
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 0 + ELEVATOR_LIGHT_COLUMN) * 32 + ELEVATOR_LEFT_LIGHT_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 1 + ELEVATOR_LIGHT_COLUMN) * 32 + ELEVATOR_LEFT_LIGHT_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 2 + ELEVATOR_LIGHT_COLUMN) * 32 + ELEVATOR_LEFT_LIGHT_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 3 + ELEVATOR_LIGHT_COLUMN) * 32 + ELEVATOR_LEFT_LIGHT_ROW)
    },
    .switchesPos = {
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 0 + ELEVATOR_SWITCH_COLUMN) * 32 + ELEVATOR_LEFT_SWITCH_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 1 + ELEVATOR_SWITCH_COLUMN) * 32 + ELEVATOR_LEFT_SWITCH_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 2 + ELEVATOR_SWITCH_COLUMN) * 32 + ELEVATOR_LEFT_SWITCH_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 3 + ELEVATOR_SWITCH_COLUMN) * 32 + ELEVATOR_LEFT_SWITCH_ROW)
    },
    .ropePos = ELEVATOR_LEFT_ROPE_ROW,
    .ropeTileOffset = ELEVATOR_LEFT_ROPE_TILE_OFFSET,
    .switchOnTile = ELEVATOR_SWITCH_ON_TILE,
    .switchOffTile = ELEVATOR_SWITCH_OFF_TILE
};

// Right elevator tile position table
static const ElevatorTilePositionTable rightTileTable = {
    .doorsPos = {
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 0 + ELEVATOR_DOOR_COLUMN) * 32 + ELEVATOR_RIGHT_DOOR_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 1 + ELEVATOR_DOOR_COLUMN) * 32 + ELEVATOR_RIGHT_DOOR_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 2 + ELEVATOR_DOOR_COLUMN) * 32 + ELEVATOR_RIGHT_DOOR_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 3 + ELEVATOR_DOOR_COLUMN) * 32 + ELEVATOR_RIGHT_DOOR_ROW)
    },
    .lightsPos = {
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 0 + ELEVATOR_LIGHT_COLUMN) * 32 + ELEVATOR_RIGHT_LIGHT_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 1 + ELEVATOR_LIGHT_COLUMN) * 32 + ELEVATOR_RIGHT_LIGHT_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 2 + ELEVATOR_LIGHT_COLUMN) * 32 + ELEVATOR_RIGHT_LIGHT_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 3 + ELEVATOR_LIGHT_COLUMN) * 32 + ELEVATOR_RIGHT_LIGHT_ROW)
    },
    .switchesPos = {
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 0 + ELEVATOR_SWITCH_COLUMN) * 32 + ELEVATOR_RIGHT_SWITCH_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 1 + ELEVATOR_SWITCH_COLUMN) * 32 + ELEVATOR_RIGHT_SWITCH_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 2 + ELEVATOR_SWITCH_COLUMN) * 32 + ELEVATOR_RIGHT_SWITCH_ROW),
        ((ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * 3 + ELEVATOR_SWITCH_COLUMN) * 32 + ELEVATOR_RIGHT_SWITCH_ROW)
    },
    .ropePos = ELEVATOR_RIGHT_ROPE_ROW,
    .ropeTileOffset = ELEVATOR_RIGHT_ROPE_TILE_OFFSET,
    .switchOnTile = ELEVATOR_SWITCH_ON_TILE | 0x4000,  // with H_FLIP
    .switchOffTile = ELEVATOR_SWITCH_OFF_TILE | 0x4000  // with H_FLIP
};

// Door tiles table (16 frames * 4 rows = 64 words)
static const uint16_t DoorTiles[64] = {
    // Frames 0-7: closed to partially open
    0x0001 | ELEVATOR_TILE_CHARATTR, 0x0002 | ELEVATOR_TILE_CHARATTR, 0x0002 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0001 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0001 | ELEVATOR_TILE_CHARATTR, 0x0003 | ELEVATOR_TILE_CHARATTR, 0x0003 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0001 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0001 | ELEVATOR_TILE_CHARATTR, 0x0004 | ELEVATOR_TILE_CHARATTR, 0x0004 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0001 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0001 | ELEVATOR_TILE_CHARATTR, 0x0005 | ELEVATOR_TILE_CHARATTR, 0x0005 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0001 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0001 | ELEVATOR_TILE_CHARATTR, 0x0006 | ELEVATOR_TILE_CHARATTR, 0x0006 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0001 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0001 | ELEVATOR_TILE_CHARATTR, 0x0007 | ELEVATOR_TILE_CHARATTR, 0x0007 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0001 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0001 | ELEVATOR_TILE_CHARATTR, 0x0008 | ELEVATOR_TILE_CHARATTR, 0x0008 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0001 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0001 | ELEVATOR_TILE_CHARATTR, 0x0009 | ELEVATOR_TILE_CHARATTR, 0x0009 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0001 | ELEVATOR_TILE_CHARATTR | 0x4000,
    // Frames 8-15: more open
    0x0002 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0002 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0003 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0003 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0004 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0004 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0005 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0005 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0006 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0006 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0007 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0007 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0008 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0008 | ELEVATOR_TILE_CHARATTR | 0x4000,
    0x0009 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR, 0x0000 | ELEVATOR_TILE_CHARATTR | 0x4000, 0x0009 | ELEVATOR_TILE_CHARATTR | 0x4000
};

// Internal functions
static void Elevator_ProcessState(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_SetDoorClosed(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_ContinueDoorClosed(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_SetDoorOpening(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_ContinueDoorOpening(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_SetDoorOpen(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_ContinueDoorOpen(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_SetDoorClosing(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_ContinueDoorClosing(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_SetMovingUp(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_ContinueMovingUp(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_SetMovingDown(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_ContinueMovingDown(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_SetCrashed(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_ContinueCrashed(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_ContinueNpcEntering(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_ContinueNpcLeaving(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_DrawDoor(Elevator *elevator, const ElevatorTilePositionTable *tileTable, uint8_t frame);
static void Elevator_DrawLightOn(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_DrawLightOff(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_DrawSwitchOn(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_DrawSwitchOff(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_DrawRope(Elevator *elevator, const ElevatorTilePositionTable *tileTable);
static void Elevator_InitDraw(Elevator *elevator, const ElevatorTilePositionTable *tileTable);

void Elevators_Init(void) {
    // Initialize left elevator
    leftElevator.state = ELEVATOR_DOOR_CLOSED;
    leftElevator.floor = 0;
    leftElevator.ropePos = 0;
    leftElevator.counter = 0;
    leftElevator.occupant = NULL;
    leftElevator.tileTableIndex = 0;
    Elevator_InitDraw(&leftElevator, &leftTileTable);
    
    // Initialize right elevator
    rightElevator.state = ELEVATOR_DOOR_CLOSED;
    rightElevator.floor = 0;
    rightElevator.ropePos = 0;
    rightElevator.counter = 0;
    rightElevator.occupant = NULL;
    rightElevator.tileTableIndex = 1;
    Elevator_InitDraw(&rightElevator, &rightTileTable);
}

void Elevators_Process(void) {
    Elevator_ProcessState(&leftElevator, &leftTileTable);
    Elevator_ProcessState(&rightElevator, &rightTileTable);
}

static void Elevator_ProcessState(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    switch (elevator->state) {
        case ELEVATOR_DOOR_CLOSED:
            Elevator_ContinueDoorClosed(elevator, tileTable);
            break;
        case ELEVATOR_DOOR_OPENING:
            Elevator_ContinueDoorOpening(elevator, tileTable);
            break;
        case ELEVATOR_DOOR_OPEN:
            Elevator_ContinueDoorOpen(elevator, tileTable);
            break;
        case ELEVATOR_DOOR_CLOSING:
            Elevator_ContinueDoorClosing(elevator, tileTable);
            break;
        case ELEVATOR_MOVING_UP:
            Elevator_ContinueMovingUp(elevator, tileTable);
            break;
        case ELEVATOR_MOVING_DOWN:
            Elevator_ContinueMovingDown(elevator, tileTable);
            break;
        case ELEVATOR_CRASHED:
            Elevator_ContinueCrashed(elevator, tileTable);
            break;
        case ELEVATOR_NPC_ENTERING:
            Elevator_ContinueNpcEntering(elevator, tileTable);
            break;
        case ELEVATOR_NPC_LEAVING:
            Elevator_ContinueNpcLeaving(elevator, tileTable);
            break;
    }
}

uint8_t Elevators_PlayerPressDoor(uint8_t side, uint8_t floor) {
    Elevator *elevator = (side == 0) ? &leftElevator : &rightElevator;
    const ElevatorTilePositionTable *tileTable = (side == 0) ? &leftTileTable : &rightTileTable;
    
    if (elevator->floor != floor) {
        return 0; // Wrong floor
    }
    
    if (elevator->state == ELEVATOR_DOOR_CLOSED) {
        Elevator_SetDoorOpening(elevator, tileTable);
        return 1; // Success
    } else if (elevator->state == ELEVATOR_DOOR_OPEN) {
        Elevator_SetDoorClosing(elevator, tileTable);
        return 1; // Success
    }
    
    return 0; // Failed
}

uint8_t Elevators_PlayerPressUp(uint8_t side) {
    Elevator *elevator = (side == 0) ? &leftElevator : &rightElevator;
    const ElevatorTilePositionTable *tileTable = (side == 0) ? &leftTileTable : &rightTileTable;
    
    if (elevator->state == ELEVATOR_DOOR_CLOSED) {
        Elevator_SetMovingUp(elevator, tileTable);
        return 1; // Success
    }
    
    return 0; // Failed
}

uint8_t Elevators_PlayerPressDown(uint8_t side) {
    Elevator *elevator = (side == 0) ? &leftElevator : &rightElevator;
    const ElevatorTilePositionTable *tileTable = (side == 0) ? &leftTileTable : &rightTileTable;
    
    if (elevator->state == ELEVATOR_DOOR_CLOSED) {
        Elevator_SetMovingDown(elevator, tileTable);
        return 1; // Success
    }
    
    return 0; // Failed
}

uint8_t Elevators_NpcEnterElevator(void *npc, uint8_t floor, uint8_t side) {
    Elevator *elevator = (side == 0) ? &leftElevator : &rightElevator;
    const ElevatorTilePositionTable *tileTable = (side == 0) ? &leftTileTable : &rightTileTable;
    
    if (elevator->occupant == NULL && elevator->state == ELEVATOR_DOOR_OPEN && elevator->floor == floor) {
        elevator->occupant = npc;
        elevator->state = ELEVATOR_NPC_ENTERING;
        return 1; // Success
    }
    
    return 0; // Failed
}

void Elevators_NpcInsideElevator(void *npc, uint8_t side) {
    Elevator *elevator = (side == 0) ? &leftElevator : &rightElevator;
    elevator->state = ELEVATOR_DOOR_OPEN;
}

static void Elevator_SetDoorClosed(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    elevator->state = ELEVATOR_DOOR_CLOSED;
    Elevator_DrawDoor(elevator, tileTable, DOOR_CLOSED_FRAME);
}

static void Elevator_ContinueDoorClosed(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    // Do nothing
}

static void Elevator_SetDoorOpening(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    elevator->state = ELEVATOR_DOOR_OPENING;
    elevator->counter = 0;
    
    if (elevator->occupant != NULL) {
        Npcs_OccupiedElevatorDoorOpening(elevator->occupant, elevator->floor);
    }
    
    Elevator_DrawSwitchOn(elevator, tileTable);
}

static void Elevator_ContinueDoorOpening(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    if (elevator->counter >= N_DOOR_FRAMES) {
        Elevator_SetDoorOpen(elevator, tileTable);
    } else {
        Elevator_DrawDoor(elevator, tileTable, elevator->counter);
        elevator->counter++;
    }
}

static void Elevator_SetDoorOpen(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    elevator->state = ELEVATOR_DOOR_OPEN;
    Elevator_DrawDoor(elevator, tileTable, DOOR_OPEN_FRAME);
    
    if (elevator->occupant != NULL) {
        Npcs_OccupiedElevatorOpen(elevator->occupant, elevator->floor);
        elevator->occupant = NULL;
    }
}

static void Elevator_ContinueDoorOpen(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    // Do nothing
}

static void Elevator_SetDoorClosing(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    elevator->state = ELEVATOR_DOOR_CLOSING;
    elevator->counter = N_DOOR_FRAMES - 1;
    Elevator_DrawSwitchOff(elevator, tileTable);
}

static void Elevator_ContinueDoorClosing(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    if (elevator->counter == 0) {
        Elevator_SetDoorClosed(elevator, tileTable);
    } else {
        Elevator_DrawDoor(elevator, tileTable, elevator->counter);
        elevator->counter--;
    }
}

static void Elevator_SetMovingUp(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    elevator->state = ELEVATOR_MOVING_UP;
    elevator->counter = N_ELEVATOR_MOVING_FRAMES;
    Elevator_DrawLightOff(elevator, tileTable);
}

static void Elevator_ContinueMovingUp(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    if (elevator->counter > 0) {
        elevator->counter--;
    }
    
    if (elevator->counter == 0) {
        if (elevator->floor == 0) {
            Elevator_SetCrashed(elevator, tileTable);
        } else {
            elevator->floor--;
            Elevator_DrawLightOn(elevator, tileTable);
            Elevator_SetDoorClosed(elevator, tileTable);
        }
    } else {
        elevator->ropePos++;
        Elevator_DrawRope(elevator, tileTable);
    }
}

static void Elevator_SetMovingDown(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    elevator->state = ELEVATOR_MOVING_DOWN;
    elevator->counter = N_ELEVATOR_MOVING_FRAMES;
    Elevator_DrawLightOff(elevator, tileTable);
}

static void Elevator_ContinueMovingDown(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    if (elevator->counter > 0) {
        elevator->counter--;
    }
    
    if (elevator->counter == 0) {
        if (elevator->floor >= (N_FLOORS - 1)) {
            Elevator_SetCrashed(elevator, tileTable);
        } else {
            elevator->floor++;
            Elevator_DrawLightOn(elevator, tileTable);
            Elevator_SetDoorClosed(elevator, tileTable);
        }
    } else {
        elevator->ropePos--;
        Elevator_DrawRope(elevator, tileTable);
    }
}

static void Elevator_SetCrashed(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    elevator->state = ELEVATOR_CRASHED;
    
    // Set strike animation
    strikeAnimationRopeTile = tileTable->ropePos;
    Game_strikeAnimationType = STRIKE_ANIMATION_ROPE_BREAKING;
}

static void Elevator_ContinueCrashed(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    // Do nothing
}

static void Elevator_ContinueNpcEntering(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    // Do nothing (handled by NPC system)
}

static void Elevator_ContinueNpcLeaving(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    // Do nothing (handled by NPC system)
}

void Elevators_StrikeAnimationRopeBreaking(void) {
    updateBgBufferOnZero = 0;
    
    // strikeAnimationRopeTile is a word index in the buffer
    // Each row is 32 words, so 28 rows = 28 * 32 words
    if (strikeAnimationRopeTile < (28 * 32)) {
        interactiveBgBuffer[strikeAnimationRopeTile] = 0;
        strikeAnimationRopeTile += 32;  // Move to next row
    }
}

static void Elevator_DrawDoor(Elevator *elevator, const ElevatorTilePositionTable *tileTable, uint8_t frame) {
    uint16_t tilePos = tileTable->doorsPos[elevator->floor];
    uint16_t doorFrameData = (frame & 0x0F) * 4;  // 4 words per frame row
    
    // Draw 4 rows, 4 columns each
    // Each column moves down by 32 words (one row in the buffer)
    for (uint8_t row = 0; row < ELEVATOR_DOOR_ROWS; row++) {
        uint16_t tile = DoorTiles[doorFrameData + row];
        for (uint8_t col = 0; col < ELEVATOR_DOOR_HEIGHT; col++) {
            uint16_t pos = tilePos + col * 32 + row;
            if (pos < (32 * 32)) {  // Bounds check
                interactiveBgBuffer[pos] = tile;
            }
        }
    }
}

static void Elevator_DrawLightOn(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    uint16_t tilePos = tileTable->lightsPos[elevator->floor];
    interactiveBgBuffer[tilePos] = ELEVATOR_LIGHT_LEFT_TILE;
    interactiveBgBuffer[tilePos + 1] = ELEVATOR_LIGHT_RIGHT_TILE;
}

static void Elevator_DrawLightOff(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    uint16_t tilePos = tileTable->lightsPos[elevator->floor];
    interactiveBgBuffer[tilePos] = 0;
    interactiveBgBuffer[tilePos + 1] = 0;
}

static void Elevator_DrawSwitchOn(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    uint16_t tilePos = tileTable->switchesPos[elevator->floor];
    interactiveBgBuffer[tilePos] = tileTable->switchOnTile;
}

static void Elevator_DrawSwitchOff(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    uint16_t tilePos = tileTable->switchesPos[elevator->floor];
    interactiveBgBuffer[tilePos] = tileTable->switchOffTile;
}

static void Elevator_DrawRope(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    uint16_t tilePos = tileTable->ropePos;
    uint8_t ropeFrame = (elevator->ropePos / ELEVATOR_ROPE_FRAME_DELAY) & ELEVATOR_ROPE_TILE_MASK;
    uint16_t tile = ropeFrame + tileTable->ropeTileOffset;
    
    // Draw rope for 28 rows (each row is 32 words)
    for (uint8_t col = 0; col < ELEVATOR_ROPE_HEIGHT; col++) {
        interactiveBgBuffer[tilePos + col * 32] = tile;
    }
}

static void Elevator_InitDraw(Elevator *elevator, const ElevatorTilePositionTable *tileTable) {
    // Draw all doors closed, switches off, lights off for all floors
    for (uint8_t f = 0; f < N_FLOORS; f++) {
        elevator->floor = f;
        Elevator_DrawDoor(elevator, tileTable, 0);
        Elevator_DrawLightOff(elevator, tileTable);
        Elevator_DrawSwitchOff(elevator, tileTable);
    }
    
    // Draw light on for bottom floor (floor 0)
    elevator->floor = 0;
    Elevator_DrawLightOn(elevator, tileTable);
    Elevator_DrawRope(elevator, tileTable);
}
