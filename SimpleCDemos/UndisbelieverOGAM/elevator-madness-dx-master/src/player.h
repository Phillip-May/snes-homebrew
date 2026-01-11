.ifndef ::_PLAYER_H_
::_PLAYER_H_ = 1

.include "game.h"
.setcpu "65816"

; Common includes
.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"
.include "routines/metasprite.h"

PLAYER_START_X_POS = 128
PLAYER_START_Y_POS = 199

PLAYER_LEFT_LIMIT = 100
PLAYER_RIGHT_LIMIT = 156

TOP_RAFTER = 55
BOTTOM_RAFTER = 199
RAFTER_SPACING = 6			; in 4 pixel blocks.

RAFTER_LEFT_POS = 120
RAFTER_RIGHT_POS = 140
RAFTER_CENTER_LEFT = 112
RAFTER_CENTER_RIGHT = 146

SWITCH_LEFT_LIMIT = PLAYER_LEFT_LIMIT + 10
SWITCH_RIGHT_LIMIT = PLAYER_RIGHT_LIMIT - 10

PLAYER_WALK_VELOCITY = 225		; Walking speed in 1/256 pixels / frame
PLAYER_AIR_WALK_VELOCITY = 155		; Velocity while in the air (1/256 pixels/frame).

PLAYER_JUMP_VELOCITY = 500		; Jumping velocity in 1/256 pixels / frame
PLAYER_JUMP_HOLD = 20			; Number of frames to hold the jump for
GRAVITY_PER_FRAME = 41			; Acceleration due to gravity in 1/256 pixels per frame per frame

PLAYER_WALK_ANIMATION_DELAY = 10	; Number of frames in-between walking frames.
PLAYER_TURN_ANIMATION_DELAY = 7		; Number of frames in-between turning frames.
PLAYER_BUTTON_ANIMATION_DELAY = 6	; Number of frames to show the players hand pressing that button.
PLAYER_ZAPPED_ANIMATION_DELAY = 30	; Number of frames to show the player being zapped by the button.

.enum
	PLAYER_WALKING		=  0
	PLAYER_PUSH_BUTTON	=  2
	PLAYER_ZAPPED		=  4
	PLAYER_JUMPING		=  6
	PLAYER_FALLING		=  8
.endenum

IMPORT_MODULE Player

	;; Initialises the player's variables
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	ROUTINE Init

	;; Processes the player for a single frame.
	ROUTINE Process

	;; Draws the NPCs to the MetaSprite code
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	ROUTINE	DrawSprites

ENDMODULE

.endif ; ::_PLAYER_H_

; vim: set ft=asm:

