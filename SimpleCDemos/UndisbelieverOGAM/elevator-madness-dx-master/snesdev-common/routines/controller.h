.ifndef ::_CONTROLLER_H_
::_CONTROLLER_H_ = 1

.setcpu "65816"

; Common includes
.include "includes/import_export.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"
.include "includes/config.inc"

CONFIG CONTROLLER_REPEAT_DPAD_DELAY, FPS * 3 / 4
CONFIG CONTROLLER_REPEAT_DPAD_RATE, FPS / 4


IMPORT_MODULE Controller
	;; New buttons pressed on current frame.
	WORD	pressed

	;; The state of the current frame
	WORD	current


	;; Enable repeating D-Pad button 'presses' every CONTROLLER_DPAD_REPEAT_RATE frames
	;; after the D-Pad has been held down for more than CONTROLLER_REPEAT_DPAD_DELAY
	;; frames.
	;;
	;; This routine assumes that the player cannot press left and right (or up and down)
	;; at the same time.
	;;
	;; This routine will need to be called once every frame when this functionality
	;; is in effect, useful for menus and dialogs.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, DP access shadow.
	ROUTINE UpdateRepeatingDPad


	;; Updates the controller variables.
	;;
	;; Should be in VBlank routine AFTER DMA processing.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, DB access registers, AUTOJOY enabled
	.macro Controller_VBlank
		.export _Controller_VBlank__Called = 1

		.global Controller__current
		.global Controller__invertedPrevious
		.global Controller__pressed

		; repeat
		; until HVJOY & HVJOY_AUTOJOY == 0
		;
		;
		; if JOY1 & JOY_TYPE_MASK == JOY_TYPE_CONTROLLER
		;	current = JOY1
		; else
		;	current = 0
		;
		; pressed = current & invertedPrevious
		; invertedPrevious = current ^ 0xFFFF

		LDA	#HVJOY_AUTOJOY
		REPEAT
			BIT	HVJOY
		UNTIL_ZERO

		REP	#$30
	.A16
		LDA	JOY1
		IF_BIT	#JOY_TYPE_MASK
			LDA	#0
		ENDIF

		STA	Controller__current
		AND	Controller__invertedPrevious
		STA	Controller__pressed

		LDA	Controller__current
		EOR	#$FFFF
		STA	Controller__invertedPrevious

		SEP	#$20
	.A8
	.endmacro
ENDMODULE

.endif ; ::_CONTROLLER_H_

; vim: set ft=asm:

