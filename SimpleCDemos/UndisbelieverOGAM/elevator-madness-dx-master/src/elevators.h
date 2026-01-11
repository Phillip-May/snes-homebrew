.ifndef ::_ELEVATORS_H_
::_ELEVATORS_H_ = 1

.include "game.h"
.setcpu "65816"

.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"

.enum
	ELEVATOR_DOOR_CLOSED	=  0
	ELEVATOR_DOOR_OPENING	=  2
	ELEVATOR_DOOR_OPEN	=  4
	ELEVATOR_DOOR_CLOSING	=  6
	ELEVATOR_MOVING_UP	=  8
	ELEVATOR_MOVING_DOWN	= 10
	ELEVATOR_CRASHED	= 12
	ELEVATOR_NPC_ENTERING	= 14
	ELEVATOR_NPC_LEAVING	= 16
.endenum


N_DOOR_FRAMES = 16
N_ELEVATOR_MOVING_FRAMES = 40

DOOR_CLOSED_FRAME = 0
DOOR_OPEN_FRAME = N_DOOR_FRAMES - 1

ELEVATOR_DOOR_HEIGHT		= 4
ELEVATOR_DOOR_ROWS		= 4
ELEVATOR_DOOR_TILE_CLEAR 	= 0
ELEVATOR_DOOR_TILE_OFFSET 	= 1
ELEVATOR_TILE_CHARATTR 		= 7 << TILEMAP_PALETTE_SHIFT | TILEMAP_ORDER_FLAG

ELEVATOR_ROPE_TILE_MASK		= $07
ELEVATOR_ROPE_FRAME_DELAY	= 4
ELEVATOR_ROPE_HEIGHT		= 28
ELEVATOR_LEFT_ROPE_TILE_OFFSET	= 32 | 7 << TILEMAP_PALETTE_SHIFT
ELEVATOR_RIGHT_ROPE_TILE_OFFSET	= 32 | 7 << TILEMAP_PALETTE_SHIFT | TILEMAP_H_FLIP_FLAG


ELEVATOR_LIGHT_LEFT_TILE	= 10 | 7 << TILEMAP_PALETTE_SHIFT | TILEMAP_ORDER_FLAG
ELEVATOR_LIGHT_RIGHT_TILE	= 11 | 7 << TILEMAP_PALETTE_SHIFT | TILEMAP_ORDER_FLAG

ELEVATOR_SWITCH_OFF_TILE	= 12 | 7 << TILEMAP_PALETTE_SHIFT | TILEMAP_ORDER_FLAG
ELEVATOR_SWITCH_ON_TILE		= 13 | 7 << TILEMAP_PALETTE_SHIFT | TILEMAP_ORDER_FLAG

ELEVATOR_FLOOR_0_COLUMN		= 2
ELEVATOR_FLOOR_COLUMN_SPACING	= 6

ELEVATOR_SWITCH_COLUMN 	= 4
ELEVATOR_LIGHT_COLUMN 	= 0
ELEVATOR_ARROW_COLUMN 	= 1
ELEVATOR_DOOR_COLUMN 	= 1

ELEVATOR_LEFT_ROPE_ROW   = 11
ELEVATOR_LEFT_SWITCH_ROW = 12
ELEVATOR_LEFT_LIGHT_ROW  = 8
ELEVATOR_LEFT_ARROW_ROW  = 6
ELEVATOR_LEFT_DOOR_ROW   = 7

ELEVATOR_RIGHT_ROPE_ROW   = 20
ELEVATOR_RIGHT_SWITCH_ROW = 19
ELEVATOR_RIGHT_DOOR_ROW   = 21
ELEVATOR_RIGHT_LIGHT_ROW  = 22
ELEVATOR_RIGHT_ARROW_ROW  = 26

IMPORT_MODULE Elevators

	;; Initialises the Elevators' variables
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	ROUTINE Init

	;; Processes the elevators for a single frame.
	ROUTINE Process

	;; Causes the door to open/close if
	;;
	;;  * The Elevator is on the Player's Floor
	;;  * The Elevator door is open or closed.
	;;
	;; If these conditions are not met, returns false so the
	;; Player can be Zapped.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	;; INPUT:
	;;	C: elevator (clear = left, set = right)
	;;	A: player's floor (0-3)
	;; OUTPUT: C set if elevator door is now opening/closing, otherwise false
	ROUTINE PlayerPressDoor

	;; Causes the elevator to move upwards if
	;;
	;;  * The Elevator's door is closed
	;; 
	;; REQUIRE: 8 bit A, 16 bit Index
	;; INPUT:
	;;	C: elevator (clear = left, set = right)
	;; OUTPUT: C set if elevator is now moving, otherwise false
	ROUTINE PlayerPressUp

	;; Causes the elevator to move downwards if
	;;
	;;  * The Elevator's door is closed
	;; 
	;; REQUIRE: 8 bit A, 16 bit Index
	;; INPUT:
	;;	c: elevator (clear = left, set = right)
	;; OUTPUT: C set if elevator is now moving, otherwise false
	ROUTINE PlayerPressDown

	;; Checks if the NPC can enter the elevator.
	;;
	;; If so then:
	;;	* The elevator's state is changed to NPC_LEAVING
	;;	* The elevator's occupant is set to DP.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	;; INPUT:
	;;	DP: The NPC
	;;	 A: NPC floor
	;;	 c: elevator (clear = left, set = right)
	;; RETURN: carry set if NPC is entering elevator.
	ROUTINE	NpcEnterElevator

	;; Notifies the elevator that the NPC is inside.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	;; INPUT:
	;;	DP: The NPC
	;;	 c: elevator (clear = left, set = right)
	ROUTINE	NpcInsideElevator


ENDMODULE

.endif ; ::_ELEVATORS_H_

; vim: set ft=asm:

