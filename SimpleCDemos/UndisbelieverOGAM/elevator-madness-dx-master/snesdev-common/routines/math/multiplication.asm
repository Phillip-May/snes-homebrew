;; Multiplication routines
;; -----------------------
;;
;; included by `routines/math.s`

.code

.A8
.A16
ROUTINE Multiply_U8Y_U8X_UY
	PHP
	SEP	#$30
.I16

	STY	WRMPYA
	STX	WRMPYB

				; Wait 8 Cycles
	REP	#$10		; 3
.A16
	PLP			; 4
	LDY	RDMPY		; 1 instruction fetch
	RTS


; Must not use mathTmp1-4
; Does not use X
.A8
.I16
ROUTINE Multiply_U16Y_U8A_U16Y
ROUTINE Multiply_S16Y_U8A_S16Y
ROUTINE Multiply_U16Y_U8A_U32
	STA	WRMPYA

	TYA
	STA	WRMPYB

				; Wait 8 Cycles - get Yh
	STY	product32  	; 5
	LDA	product32 + 1	; 4

	LDY	RDMPY
	STY	product32

	STA	WRMPYB		; WRMPYA is already set

; 16A uses 24 cycles, 8A also uses 26 cycles.
				; Wait 8 Cycles
	REP	#$31		; 3
.A16	; c clear
	LDA	product32 + 1	; 5
	AND	#$00FF
	ADC	RDMPY
	STA	product32 + 1

	SEP	#$20
.A8

	STZ	product32 + 3
	LDY	product32

	RTS


.I16
ROUTINE Multiply_U16Y_U16X_U16Y
ROUTINE Multiply_U16Y_S16X_16Y
ROUTINE Multiply_S16Y_U16X_16Y
ROUTINE Multiply_S16Y_S16X_S16Y
	;     Y y
	; *   X x
	; -----------
	;      y*x
	; +  Y*x
	; +  y*X

	PHP
	SEP	#$20
.A8

	TXA
	STA	WRMPYA		; Low byte of x

	TYA
	STA	WRMPYB		; Low byte of y

				; Wait 8 Cycles
	STY	mathTmp1  	; 5
	STX	mathTmp3	; 5

	LDX	RDMPY

	LDA	mathTmp1 + 1	; High byte of Y
	STA	WRMPYB		; WRMPYA is already Low x

				; Wait 8 Cycles
	STX	product32	; 5
	LDA	product32 + 1	; 4
	CLC
	ADC	RDMPYL
	STA	product32 + 1

	LDA	mathTmp3 + 1	; High byte of X
	STA	WRMPYA

	TYA			; Low byte of Y
	STA	WRMPYB

				; Wait 8 cycles
	CLC			; 2
	LDA	product32 + 1	; 4
	ADC	RDMPY		; 2 - load address
	STA	product32 + 1

	LDY	product32

	PLP
	RTS




.I16
ROUTINE Multiply_U16Y_U16X_U32XY
ROUTINE Multiply_U16Y_S16X_32XY
ROUTINE Multiply_S16Y_U16X_32XY
	;       Y y
	;   *   X x
	; -----------
	;       y*x
	; + c Y*x
	; + c y*X
	; + Y*X

	PHP
	SEP	#$20
.A8

	TXA
	STA	WRMPYA		; Low byte of x

	TYA
	STA	WRMPYB		; Low byte of y

				; Wait 8 Cycles
	STY	mathTmp1  	; 5
	STX	mathTmp3	; 5

	LDX	RDMPY

	LDA	mathTmp1 + 1	; High byte of Y
	STA	WRMPYB		; WRMPYA is already Low x

				; Wait 8 Cycles
	STX	product32	; 5
	LDA	product32 + 1	; 4
	CLC
	ADC	RDMPYL
	STA	product32 + 1

	LDA	RDMPYH
	REP	#$20
.A16
	ADC	#0
	STA	product32 + 2
	SEP	#$20
.A8

	LDA	mathTmp3 + 1	; High byte of X
	STA	WRMPYA

	LDA	mathTmp1	; Low byte of Y
	STA	WRMPYB

				; Wait 8 cycles
	REP	#$31		; 3
.A16	; c clear
	LDA	product32 + 1	; 5
	ADC	RDMPY
	STA	product32 + 1

	SEP	#$20
.A8
	LDA	#0
	ADC	#0
	STA	product32 + 3

	; Keep Carry for next addition
	
	LDA	mathTmp1 + 1	; High byte of Y
	STA	WRMPYB
	
 				; Wait 8 cycles
	REP	#$31		; 3
.A16	; c clear
	LDA	product32 + 2	; 5
	ADC	RDMPY
	STA	product32 + 2
	TAX
	LDY	product32

	PLP
	RTS




.A8
.I16
ROUTINE Multiply_S16Y_S16X_S32XY
	CPY	#$8000
	IF_LT
		; Y is Positive

		CPX	#$8000
		BLT	Multiply_U16Y_U16X_U32XY

		; Y is Positive, X is Negative
		STX	factor32 + 0
		LDX	#$FFFF
		STX	factor32 + 2

		BRA	Multiply_S32_U16Y_S32XY
	ENDIF

	; Y is Negative
	STY	factor32 + 0
	LDY	#$FFFF
	STY	factor32 + 2

	TXY

	.assert * = Multiply_S32_S16Y_S32XY, lderror, "Bad Flow"


.A8
.I16
ROUTINE Multiply_U32_S16Y_32XY
ROUTINE Multiply_S32_S16Y_S32XY
	CPY	#$8000
	IF_GE
		LDX	#$FFFF
		BRA	Multiply_S32_S32XY_S32XY_SkipXCheck
	ENDIF

	.assert * = Multiply_U32_U16Y_U32XY, lderror, "Bad Flow"

.A8
.I16
ROUTINE Multiply_U32_U16Y_U32XY
ROUTINE Multiply_S32_U16Y_S32XY
	;      f3 f2 f1 f0
	;  *         Yh Yl
	; ------------------
	;          c f0*Yl
	; +     c f1*Yl
	; +  c f2*Yl
	; + f3*Yl
	; +     c f0*Yh
	; +    f1*Yh
	; + f2*Yh
	;
	; tmp     = factor * Yl
	; Product = tmp + (factor * Yh << 8)
	PHY

	LDXY	factor32
	PLA				; Yl
	JSR	Multiply_U32XY_U8A_U32XY

	STXY	mathTmp1		; not used by Multiply_U16Y_U8A

	LDY	factor32 + 0
	PLA				; Yh
	JSR	Multiply_U16Y_U8A_U32
	LDX	product32 + 2

	LDA	factor32 + 2
	STA	WRMPYB			; WRMPYA already contains Yh
					; Wait 8 cycles
	LDA	mathTmp1		; 4
	STA	product32		; 4

	REP	#$31
.A16	; c clear
	TYA
	ADC	mathTmp1 + 1
	STA	product32 + 1

	SEP	#$20
.A8
	TXA
	ADC	mathTmp1 + 3
	CLC
	ADC	RDMPYL
	STA	product32 + 3

	LDXY	product32

	RTS



.A8
.I16
ROUTINE Multiply_U32_U32XY_U32XY
ROUTINE Multiply_U32_S32XY_32XY
ROUTINE Multiply_S32_U32XY_32XY
ROUTINE Multiply_S32_S32XY_S32XY
	;      f3 f2 f1 f0
	;  *   Xh Xl Yh Yl
	; ------------------
	;          c f0*Yl
	; +     c f1*Yl
	; +  c f2*Yl
	; + f3*Yl
	; +     c f0*Yh
	; +    f1*Yh
	; + f2*Yh
	; +  c f0*Xl
	; + f1*Xl
	; + f0*Xh
	;
	; tmp      = (factor * Yl) + (factor * Yh << 8) 
	; product  = tmp + (factor * Xl << 16) + (factor * Xh << 24)

	CPX	#0
	BEQ	Multiply_U32_U16Y_U32XY

Multiply_S32_S32XY_S32XY_SkipXCheck:
	PHX
	PHY
	; first line
	LDXY	factor32
	PLA				; Yl
	JSR	Multiply_U32XY_U8A_U32XY

	STXY	mathTmp1		; not used by Multiply_U16Y_U8A

	LDY	factor32 + 0
	PLA				; Yh
	JSR	Multiply_U16Y_U8A_U32
	LDX	product32 + 2

	LDA	factor32 + 2
	STA	WRMPYB			; WRMPYA already contains Yh
					; Wait 8 cycles
	REP	#$31			; 3
.A16	; c clear
	TYA				; 2
	ADC	mathTmp1 + 1		; 3 instruction fetch, addr fetch
	STA	mathTmp1 + 1

	SEP	#$20
.A8
	TXA
	ADC	mathTmp1 + 3
	CLC
	ADC	RDMPYL
	STA	mathTmp1 + 3


	; product  = tmp + (factor * Xl << 16) + (factor * Xh << 24)
	PLA				; Xl
	LDY	factor32
	JSR	Multiply_U16Y_U8A_U32	; Does not use mathTmp

	REP	#$31
.A16	; c clear
	TYA
	ADC	mathTmp1 + 2 ; & 3
	STA	product32 + 2 ; & 3

	SEP	#$20
.A8

	LDA	factor32
	STA	WRMPYA
	PLA				; Xh
	STA	WRMPYB

					; Wait 8 cycles
	LDX	mathTmp1 + 0 ; & 1	; 5
	STX	product32 + 0 ; & 1	; 5

	LDA	product32 + 3
	ADD	RDMPYL
	STA	product32 + 3

	LDXY	product32

	RTS



.A8
.I16
ROUTINE Multiply_U32XY_U8A_U32XY
ROUTINE Multiply_S32XY_U8A_S32XY
	STA	WRMPYA

	; Low Word
	TYA
	STA	WRMPYB
				; Wait 8 Cycles
	STY	mathTmp1 	; 5
	LDA	mathTmp1 + 1	; 4 (high byte of Y)

	LDY	RDMPY

	STA	WRMPYB		; WRMPYA is already set

				; Wait 8 cycles
	STY	product32 + 0	; 5
	LDA	product32 + 1	; 4
	CLC
	ADC	RDMPYL
	STA	product32 + 1

	LDA	RDMPYH
	STA	product32 + 2

	; High Word
	TXA
	STA	WRMPYB
				; Wait 8 Cycles
	STX	mathTmp1 	; 5

	LDA	product32 + 2	; 4
	ADC	RDMPYL		; keep carry from previous addition
	STA	product32 + 2

	LDA	RDMPYH
	STA	product32 + 3

	LDA	mathTmp1 + 1	; high byte of X
	STA	WRMPYB		; WRMPYA is already set

				; Wait 8 cycles
	LDA	product32 + 3	; 4
	LDY	product32	; 5

	ADC	RDMPYL
	STA	product32 + 3

	LDX	product32 + 2	; may as well, would need to test/save it asap anyway.

	RTS

