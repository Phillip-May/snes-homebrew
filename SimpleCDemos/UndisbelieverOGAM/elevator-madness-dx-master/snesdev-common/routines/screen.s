;; Screen Routines

.include "routines/screen.h"
.include "includes/import_export.inc"
.include "includes/structure.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"

.setcpu "65816"

; Ensure VBlank macros are used
.forceimport _Screen_VBlank__Called:zp

MODULE Screen

.segment "SHADOW"
	UINT16	frameCounter

.code

; A and I size unknown
; MUST NOT USE A
ROUTINE	WaitFrame
	LDX	frameCounter
	REPEAT
		WAI
		CPX	frameCounter
	UNTIL_NE

	RTS


; A and I size unknown
; IN: A = frames to delay
ROUTINE	WaitManyFrames
	REPEAT
		JSR	WaitFrame
		DEC
	UNTIL_ZERO

	RTS


.A8
ROUTINE FadeIn
	LDA	#1

	.assert * = SlowFadeIn, lderror, "Bad Flow"


.A8
ROUTINE SlowFadeIn
	TAY
	LDA	#0

	REPEAT
		STA	INIDISP
		JSR	_WaitYFrames
		INC
		CMP	#16
	UNTIL_GE

	RTS



.A8
ROUTINE FadeOut
	LDA	#1

	.assert	* = SlowFadeOut, lderror, "Bad Flow"


; IN: A = number of frames per decrement.
.A8
ROUTINE	SlowFadeOut
	TAY
	LDA	#14

	REPEAT
		STA	INIDISP
		JSR	_WaitYFrames
		DEC
	UNTIL_MINUS

	LDA	#INIDISP_FORCE
	STA	INIDISP

	RTS


;; Pauses the system for Y frames
;; IN: Y number of frames to wait
;; MODIFIES: X
.A8
ROUTINE _WaitYFrames
	PHA
	TYA
	REPEAT
		LDX	frameCounter
		REPEAT
			WAI
			CPX	frameCounter
		UNTIL_NE

		DEC
	UNTIL_ZERO
	PLA
	RTS


ENDMODULE

