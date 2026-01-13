;; Division routines
;; -----------------
;;
;; included by `routines/math.s`

.code

.I16
ROUTINE Divide_S16Y_U16X
	CPY	#$8000
	IF_GE
		PHP
		REP	#$30
.A16

		TYA
		NEG16
		TAY

		JSR	Divide_U16Y_U16X

		; Result is negative
		TYA
		NEG16
		TAY

		PLP
		RTS		
	ENDIF

	BRA	Divide_U16Y_U16X


.I16
ROUTINE Divide_U16Y_S16X
	CPX	#$8000
	IF_GE
		PHP
		REP	#$30
.A16

		TXA
		NEG16
		TAX

		JSR	Divide_U16Y_U16X

		; Result is negative
		TYA
		NEG16
		TAY

		PLP
		RTS		
	ENDIF

	BRA	Divide_U16Y_U16X



.I16
ROUTINE Divide_S16Y_S16X
	CPY	#$8000
	IF_GE
		PHP
		REP	#$30
.A16
		; dividend Negative
		TYA
		NEG16
		TAY
		
		CPX	#$8000
		IF_GE
			; divisor Negative
			TXA
			NEG16
			TAX

			JSR	Divide_U16Y_U16X
			
			; Result is positive
			PLP
			RTS
		ENDIF

		; Else - divisor is positive
		JSR	Divide_U16Y_U16X
		
		; Result is negative
		TYA
		NEG16
		TAY

		PLP
		RTS
	ENDIF
	; Else - dividend is positive

	CPX	#$8000
	IF_GE
		PHP
		REP	#$30
.A16

		TXA
		NEG16
		TAX

		JSR	Divide_U16Y_U16X

		; Result is negative
		TYA
		NEG16
		TAY

		PLP
		RTS		
	ENDIF

	.assert * = Divide_U16Y_U16X, lderror, "Bad Flow"


; INPUT:
;	Y: 16 bit unsigned Dividend
;	X: 16 bit unsigned Divisor
;
; OUTPUT:
;	Y: 16 bit unsigned Result
;	X: 16 bit unsigned Remainder
;
; Timings:
;  Y < 256:   44 cycles 
;  tmp dp:    474 - 517 cycles
;  tmp addr:  508 - 572 cycles
;
; You could save 152 (dp) or 169 (addr) cycles if the loop is unfolded
; But increases size by 186 (dp) or 199 (addr) bytes.
;
; if divisor < 256
;	calculate using SNES registers
; else
; 	remainder = 0
; 	repeat 16 times 
; 		remainder << 1 | MSB(dividend)
; 		dividend << 1
; 		if (remainder >= divisor)
;			remainder -= divisor
;			result++

.I16
ROUTINE Divide_U16Y_U16X
.scope
counter := mathTmp1
divisor := mathTmp3

	PHP
	CPX	#$0100
	IF_GE
		REP	#$20
.A16
		STX	divisor
		LDX	#0			; Remainder
		LDA	#16
		STA	counter

		REPEAT
			TYA			; Dividend / result
			ASL A
			TAY
			TXA 			; Remainder
			ROL A
			TAX

			SUB	divisor
			IF_C_SET		; C set if positive
				TAX
				INY 		; Result
			ENDIF

			DEC	counter
		UNTIL_ZERO

		PLP
		RTS
	ENDIF

	; Otherwise Use registers instead
		STY	WRDIV
		SEP	#$30			; 8 bit Index
.A8
.I8
		STX	WRDIVB

		; Wait 16 Cycles
		PHD				; 4
		PLD				; 5
		PLP				; 4
.I16
		BRA	Divide_U16X_U8A_Result  ; 3

	; Endif
.endscope


; INPUT:
;	Y: 16 bit unsigned Dividend
;	A: 8 bit unsigned Divisor
;
; OUTPUT:
;	Y: 16 bit unsigned Result
;	X: 16 bit unsigned Remainder
.A8
.I16
ROUTINE Divide_U16Y_U8A
	STY	WRDIV
	STA	WRDIVB			; Load to SNES division registers

	; Wait 16 Cycles
	PHD				; 4
	PLD				; 5
	PHB				; 3
	PLB				; 4

Divide_U16X_U8A_Result:

	LDY	RDDIV			; result
	LDX	RDMPY			; remainder

	RTS



ROUTINE Divide_S32_S32
	PHP
	REP	#$30
.A16
.I16
	LDA	dividend32 + 2
	IF_MINUS
		EOR	#$FFFF
		STA	dividend32 + 2

		LDA	dividend32
		EOR	#$FFFF
		STA	dividend32

		INC32	dividend32

		LDA	divisor32 + 2
		IF_MINUS
		; divisor is negative
			EOR	#$FFFF
			STA	divisor32 + 2

			LDA	divisor32
			EOR	#$FFFF
			STA	divisor32

			INC32	divisor32

			BRA	_Divide_U32_U32_AfterPHP
		ENDIF

		; Else, divisor is positive

		JSR	Divide_U32_U32

		; only 1 negative, result negative
		NEG32	result32

		PLP
		RTS
	ENDIF

	; dividend is positive

	LDA	divisor32 + 2
	IF_MINUS
	; divisor is negative
		EOR	#$FFFF
		STA	divisor32 + 2

		LDA	divisor32
		EOR	#$FFFF
		STA	divisor32

		INC32	divisor32

		JSR	Divide_U32_U32

		; only 1 negative, result negative
		NEG32 result32

		PLP
		RTS
	ENDIF

	BRA	_Divide_U32_U32_AfterPHP



; remainder = 0
; Repeat 32 times:
; 	remainder << 1 | MSB(dividend)
; 	dividend << 1
; 	if (remainder >= divisor)
;		remainder -= divisor
;		result++
ROUTINE Divide_U32_U32
	PHP
	REP	#$30
.A16
.I16

_Divide_U32_U32_AfterPHP:
	STZ	remainder32
	STZ	remainder32 + 2

	FOR_X #32, DEC, #0
		ASL	dividend32
		ROL	dividend32 + 2
		ROL	remainder32
		ROL	remainder32 + 2

		LDA	remainder32
		SEC
		SBC	divisor32
		TAY
		LDA	remainder32 + 2
		SBC	divisor32 + 2
		IF_C_SET
			STY	remainder32
			STA	remainder32 + 2
			INC	result32	; result32 = dividend32, no overflow possible
		ENDIF
	NEXT

	PLP
	RTS



; INPUT:
;	dividend32: uint32 dividend
;	A : 8 bit divisor
;
; OUTPUT:
;	result32: uint32 result
;       A: uint8 remainder
ROUTINE Divide_U32_U8A
	PHP
	SEP	#$30			; 8 bit A, 8 bit Index
.A8
.I8

	TAY				; divisor
	LDA	#0			; remainder

	LDX	#3			; loop bytes 3 to (including) 0
	REPEAT
		STA	WRDIVH		; remainder from previous division (always < divisor)
		LDA	dividend32, X
		STA	WRDIVL

		STY	WRDIVB		; Load to SNES division registers

		; Wait 16 Cycles
		PHD			; 4
		PLD			; 5
		PHB			; 3
		PLB			; 4

		LDA	RDDIV
		STA	result32, X	; store result (8 bit as remainder < divisor)
		LDA	RDMPY		; remainder (also 8 bit)

		DEX
	UNTIL_MINUS

	PLP
	RTS

