.ifndef ::_GAME_H_
::_GAME_H_ = 1

.setcpu "65816"

; Common includes
.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"
.include "includes/config.inc"

.include "routines/metasprite.h"

.include "resources.h"

.define N_FLOORS 4


;; VRAM Map
;; WORD ADDRESSES
GAME_BG1_MAP	= $0000
GAME_BG2_MAP	= $0400
GAME_BG3_MAP	= $0800
GAME_BG1_TILES	= $5000
GAME_BG2_TILES	= $1000
GAME_OAM_TILES	= $6000

GAME_BG1_SIZE	= BGXSC_SIZE_32X32
GAME_BG2_SIZE	= BGXSC_SIZE_32X32
GAME_BG3_SIZE	= BGXSC_SIZE_32X32

GAME_OAM_SIZE	= OBSEL_SIZE_8_16
GAME_OAM_NAME	= 0




N_STRIKES		= 3
STRIKE_MIN_FRAMES	= 1 * 60
STRIKE_TIMEOUT		= 9 * 60
GAME_OVER_FADEOUT_SPEED	= 5


SCORE_DIGIT_DELTA 	= 16
SCORE_TILE_COLUMN	= 2
SCORE_TILE_ROW		= 14
SCORE_TILE_ATTR 	= 7 << TILEMAP_HIGH_PALETTE_SHIFT

STRIKES_TILE_COLUMN	= 3
STRIKES_TILE_ROW	= 14
STRIKES_TILE_CHARATTR 	= 7 << TILEMAP_PALETTE_SHIFT

STRIKES_FULL_TOP_TILE	= 14
STRIKES_FULL_BOTTOM_TILE= 30
STRIKES_HALF_TOP_TILE	= 15
STRIKES_HALF_BOTTOM_TILE= 31


.global	interactiveBgBuffer
.global updateBgBufferOnZero

IMPORT_MODULE Game
	;; The Number of strikes the player has
	BYTE	strikes

	;; The player's current score
	WORD	score

	;; The new buttons pressed by player 1 in this frame
	WORD	newJoypadPressed


	;; If this function pointer is set then there is a Strike Against the player.
	;;
	;; This pointer is to a routine that is called once per frame.
	ADDR	strikeAntimationRoutinePtr

	;; Builds and processes the game.
	ROUTINE PlayGame

ENDMODULE

.endif ; ::_GAME_H_

; vim: set ft=asm:

