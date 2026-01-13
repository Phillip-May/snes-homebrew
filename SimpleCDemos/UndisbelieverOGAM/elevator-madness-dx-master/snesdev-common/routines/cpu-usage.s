
.include "cpu-usage.h"
.include "includes/synthetic.inc"
.include "includes/structure.inc"
.include "includes/registers.inc"


MODULE CpuUsage


.segment "SHADOW"
	UINT8 vBlankCounter
	UINT8 missedFrames
	UINT16 referenceBogo
	UINT16 currentBogo

.code

.A8
.I16
ROUTINE CalcReference
	; Enable VBlank, Disable IRQ
	LDA	#NMITIMEN_VBLANK_FLAG
	STA	NMITIMEN

	; Wait until the start of the frame
	STZ	vBlankCounter
	REPEAT
		WAI
		LDA	vBlankCounter
	UNTIL_NOT_ZERO

	STZ	missedFrames

	JSR	WaitFrame

	LDY	currentBogo
	STY	referenceBogo

	; Disable VBlank
	STZ	NMITIMEN

	RTS


ROUTINE WaitFrame
	; Save state
	PHP
	REP	#$30
	PHA
	PHX

	SEP	#$20
.A8
.I16

	LDA	vBlankCounter
	STA	missedFrames

	LDX	#0
	REPEAT
		INX
		LDA	vBlankCounter
		CMP	missedFrames
	UNTIL_NE

	STX	currentBogo

	; Reset the counter
	STZ	vBlankCounter

	; Load State
	REP	#$30
	PLX
	PLA
	PLP

	RTS


.A8
ROUTINE WaitLimited
	DEC
	JSR	WaitFrame
	CMP	missedFrames
	IF_GE
		; Save MissedFrames
		PHA
		PHX
		PHP

		SEP	#$30
.I8
		SUB	missedFrames
		TAX

		REP	#$20
.A16
		LDA	currentBogo

		REPEAT
			JSR	WaitFrame

			; Increment CurrentBogo to handle next frames
			ADD	currentBogo

			DEX
		UNTIL_ZERO

		STA	currentBogo

		PLP
		PLX
		PLA
		STA	missedFrames
	ENDIF

	RTS

ENDMODULE

