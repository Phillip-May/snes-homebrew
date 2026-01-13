; Randomizer module

.include "random.h"
.include "includes/registers.inc"
.include "routines/math.h"

MODULE Random

.segment "SHADOW"
	UINT32	seed
	WORD	prevJoypadState
	WORD	tmp
.code


.A8
.I16
ROUTINE AddJoypadEntropy
	LDX	JOY1
	CPX	prevJoypadState
	IF_NE
		STX	prevJoypadState
		JSR	Rnd
	ENDIF

	.assert * = Rnd, lderror, "Bad flow"


.A8
.I16
ROUTINE Rnd
    ; LCPR parameters are the same as cc65 (MIT license)
    ;
	; seed = seed * 0x010101 + 0x31415927

	CLC
	LDA	seed + 0
	ADC	seed + 1
	STA	seed + 1
	ADC	seed + 2
	STA	seed + 2
	ADC	seed + 3
	STA	seed + 3

	REP	#$31
.A16
	; carry clear
	LDA	seed + 0
	ADC	#$5927
	STA	seed + 0

	LDA	seed + 2
	ADC	#$3141
	STA	seed + 2

	SEP	#$20
.A8
	RTS


.A8
.I16
ROUTINE Rnd_4
	JSR	Rnd
	LDA	seed + 3
	AND	#$03
	RTS


.A8
.I16
ROUTINE Rnd_3
	JSR	Rnd
	LDA	seed + 3
	AND	#$03
	IF_ZERO
		LDA	seed + 3
		LSR
		LSR
		AND	#$03
		IF_ZERO
			LDA	#2
		ENDIF
	ENDIF
	DEC	; faster than CMP

	RTS


.A8
.I16
ROUTINE Rnd_2
	JSR	Rnd
	LDA	seed + 3
	ASL

	LDA	#0
	ROL

	RTS


; A = number of probabilities
; DB unknown
.A8
.I16
ROUTINE	Rnd_U8A
	TAY

    ; put the higher entropy seed bytes in the lower
    ; bits in the event that A is a power of 2

	LDA	seed + 3
	STA	f:WRDIVL
	LDA	seed + 2
	STA	f:WRDIVH

	TYA
	STA	f:WRDIVB

	JSR	Rnd

	LDA	f:RDMPY		; remainder

	RTS


; Y = number of probabilities
; DB unknown
.A8
.I16
ROUTINE	Rnd_U16Y
	TYX
	LDY	seed + 2

	JSR	Math__Divide_U16Y_U16X

	PHX
	JSR	Rnd
	PLY

	RTS


; X = min
; Y = max
; DB unknown
.A8
.I16
ROUTINE	Rnd_U16X_U16Y
	STX	tmp

	REP	#$31
.A16
	TYA
	SUB	tmp
	TAX

	LDY	seed + 2
	JSR	Math__Divide_U16Y_U16X

	TXA
	ADD	tmp
	PHA

	SEP	#$20
.A8
	JSR	Rnd

	PLY

	RTS

ENDMODULE

; vim: set ft=asm:

