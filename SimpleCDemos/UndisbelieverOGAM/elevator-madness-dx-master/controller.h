#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include "snes_regs_xc.h"

// Button definitions (16-bit combined from JOY1L and JOY1H)
#define JOY_B          0x8000
#define JOY_Y          0x4000
#define JOY_SELECT     0x2000
#define JOY_START      0x1000
#define JOY_UP         0x0800
#define JOY_DOWN       0x0400
#define JOY_LEFT       0x0200
#define JOY_RIGHT      0x0100
#define JOY_A          0x0080
#define JOY_X          0x0040
#define JOY_L          0x0020
#define JOY_R          0x0010

#define JOY_BUTTONS    (JOY_B | JOY_Y | JOY_A | JOY_X)
#define JOY_DPAD       (JOY_UP | JOY_DOWN | JOY_LEFT | JOY_RIGHT)

// Game-specific button combinations
#define BUTTON_JUMP           JOY_B
#define BUTTON_ELEVATOR_DOOR   JOY_Y
#define BUTTON_ELEVATOR_UP     (JOY_UP | JOY_X)
#define BUTTON_ELEVATOR_DOWN   (JOY_DOWN | JOY_A)

// AUTOJOY status bit in REG_HVBJOY
#define HVJOY_AUTOJOY  0x01

// Controller module state
extern uint16_t Controller_pressed;  // New buttons pressed this frame
extern uint16_t Controller_current;  // Current button state

// Update controller state (must be called during VBlank or when AUTOJOY is ready)
void Controller_Update(void);

#endif // CONTROLLER_H
