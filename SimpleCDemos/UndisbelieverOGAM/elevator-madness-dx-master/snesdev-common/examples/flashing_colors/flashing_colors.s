; Flashes a screen color
;
; Used for the initial test of the structure macros.

.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"

.include "routines/reset-snes.h"

.segment "SHADOW"
change_color_flag:	.res 1
next_color_index:	.res 2


.code
ROUTINE Main
	REP	#$10        ; X/Y 16-bit
	SEP	#$20        ; A 8-bit
.A8
.I16
	INC	change_color_flag
	STZ	next_color_index
	STZ	next_color_index  + 1

	; Enable V-Blank
	LDA	#NMITIMEN_VBLANK_FLAG
	STA	NMITIMEN

	; Just Loop, flashing the screen
	; change_color_flag will be responsible for changing color on VBlank
	REPEAT
		LDA	#$00
		REPEAT
			STA	INIDISP

			FOR_X #6, DEC, #0
				WAI
			NEXT

			INC
			CMP	#$0F
		UNTIL_EQ

		REPEAT
			STA INIDISP

			FOR_Y #6, DEC, #0
				WAI
			NEXT

			DEC
		UNTIL_ZERO

		INC change_color_flag
	FOREVER

;; Blank Handlers
LABEL IrqHandler
LABEL CopHandler
	RTI


;; VBlank routine
;;
;; if `change_color_flag` is set:
;;	clear `change_color_flag`
;; 	set the BG color to the color at `next_color_index`
;;	increment `next_color_index` to next color (+2), resetting as necessary
LABEL VBlank
	; Save state
	REP	#$30
	PHA
	PHX

	SEP	#$20
.A8
.I16
	; Reset NMI Flag.
	LDA	RDNMI

	LDA	change_color_flag
	IF_NOT_ZERO
		STZ	change_color_flag

		LDX	next_color_index

		; Set background color from table
		STZ	$2121

		LDA	ColorTable, X
		STA	$2122
		LDA	ColorTable + 1, X
		STA	$2122

		; Increment color index for next update
		CPX	#EndColorTable - ColorTable - 2
		IF_LT
			INX
			INX
		ELSE
			LDX #0
		ENDIF

		STX	next_color_index
	ENDIF

	; restore state
	REP	#$30
	PLX
	PLA
	RTI


.rodata
ColorTable:
	.word $7C00
	.word $03E0
	.word $001F
EndColorTable:


