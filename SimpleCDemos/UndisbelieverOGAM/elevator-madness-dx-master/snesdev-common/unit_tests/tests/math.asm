; Test of Math routines
.include "routines/math.h"

;; Call math routine through DB set handler.
;; Will set DB to $FF (so it can't access registers) for call
.macro JSR_MTH routine
	PEA	$80FF
	PLB
	JSR	.ident(.sprintf("%s_DB", .string(routine)))
	PLB
.endmacro

PAGE_ROUTINE Math_Multiply_1
	Text_SetColor	4
	Text_PrintString "Multiply_U8Y_U8X_UY"

	.macro Test_Multiply_U8Y_U8X_UY
		.local factorX, factorY

		STATIC_RANDOM_MIN_MAX	factorX, 0, 255
		STATIC_RANDOM_MIN_MAX	factorY, 0, 255

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%10u * %3u = ", factorY, factorX)

		LDY	#factorY
		LDX	#factorX
		JSR_MTH	Math__Multiply_U8Y_U8X_UY

		Check_16Y (factorY * factorX)
		JSR	Text__PrintDecimal_U16Y
	.endmacro

	Test_Multiply_U8Y_U8X_UY
	Test_Multiply_U8Y_U8X_UY
	Test_Multiply_U8Y_U8X_UY
	Test_Multiply_U8Y_U8X_UY
	Test_Multiply_U8Y_U8X_UY


	Text_SetColor	4
	Text_NewLine
	Text_NewLine
	Text_PrintString "Multiply_U16Y_U8A_U16Y"

	.macro Test_Multiply_U16Y_U8A_U16Y
		.local factorA, factorY

		STATIC_RANDOM_MIN_MAX	factorA, 0, 255
		STATIC_RANDOM_MIN_MAX	factorY, 0, 65535

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%10u * %3u = ", factorY, factorA)

		LDY	#factorY
		LDA	#factorA
		JSR_MTH	Math__Multiply_U16Y_U8A_U16Y

		Check_16Y (factorY * factorA)
		JSR	Text__PrintDecimal_U16Y
	.endmacro

	Test_Multiply_U16Y_U8A_U16Y
	Test_Multiply_U16Y_U8A_U16Y
	Test_Multiply_U16Y_U8A_U16Y
	Test_Multiply_U16Y_U8A_U16Y
	Test_Multiply_U16Y_U8A_U16Y

	RTS



PAGE_ROUTINE Math_Multiply_2
	Text_SetColor	4
	Text_PrintString "Multiply_S16Y_U8A_S16Y"

	.macro Test_Multiply_S16Y_U8A_S16Y
		.local factorA, factorY

		STATIC_RANDOM_MIN_MAX	factorA, 0, 255
		STATIC_RANDOM_MIN_MAX	factorY, -32768, 32767

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%10i * %3u = ", factorY, factorA)

		LDY	#.loword(factorY)
		LDA	#factorA
		JSR_MTH	Math__Multiply_S16Y_U8A_S16Y

		Check_16Y (factorY * factorA)
		JSR	Text__PrintDecimal_U16Y
	.endmacro

	Test_Multiply_S16Y_U8A_S16Y
	Test_Multiply_S16Y_U8A_S16Y
	Test_Multiply_S16Y_U8A_S16Y
	Test_Multiply_S16Y_U8A_S16Y
	Test_Multiply_S16Y_U8A_S16Y


	Text_SetColor	4
	Text_NewLine
	Text_NewLine
	Text_PrintString "Multiply_U16Y_U8A_U32XY"

	.macro Test_Multiply_U16Y_U8A_U32
		.local factorA, factorY

		STATIC_RANDOM_MIN_MAX	factorA, 0, 255
		STATIC_RANDOM_MIN_MAX	factorY, 0, 65535

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%10u * %3u = ", factorY, factorA)

		LDY	#factorY
		LDA	#factorA
		JSR_MTH	Math__Multiply_U16Y_U8A_U32
		LDXY	Math__product32

		Check_32XY (factorY * factorA)
		JSR	Text__PrintDecimal_U32XY
	.endmacro

	Test_Multiply_U16Y_U8A_U32
	Test_Multiply_U16Y_U8A_U32
	Test_Multiply_U16Y_U8A_U32
	Test_Multiply_U16Y_U8A_U32
	Test_Multiply_U16Y_U8A_U32

	RTS



PAGE_ROUTINE Math_Multiply_3
	Text_SetColor	4
	Text_PrintString "Multiply_U16Y_U16X_U16Y"

	.macro Test_Multiply_U16Y_U16X_U16Y
		.local factorX, factorY

		STATIC_RANDOM_MIN_MAX	factorX, 0, 65535
		STATIC_RANDOM_MIN_MAX	factorY, 0, 65535

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%7i * %6i = ", factorY, factorX)

		LDY	#.loword(factorY)
		LDX	#.loword(factorX)
		JSR_MTH	Math__Multiply_U16Y_U16X_U16Y

		Check_16Y (factorY * factorX)
		JSR	Text__PrintDecimal_U16Y
	.endmacro

	Test_Multiply_U16Y_U16X_U16Y
	Test_Multiply_U16Y_U16X_U16Y
	Test_Multiply_U16Y_U16X_U16Y
	Test_Multiply_U16Y_U16X_U16Y
	Test_Multiply_U16Y_U16X_U16Y


	Text_SetColor	4
	Text_NewLine
	Text_NewLine
	Text_PrintString "Multiply_U16Y_U16X_U32XY"

	.macro Test_Multiply_U16Y_U16X_U32XY
		.local factorX, factorY

		STATIC_RANDOM_MIN_MAX	factorX, 0, 65535
		STATIC_RANDOM_MIN_MAX	factorY, 0, 65535

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%7u * %6u = ", factorY, factorX)

		LDY	#factorY
		LDX	#factorX
		JSR_MTH	Math__Multiply_U16Y_U16X_U32XY

		Check_32XY (factorY * factorX)
		JSR	Text__PrintDecimal_U32XY
	.endmacro

	Test_Multiply_U16Y_U16X_U32XY
	Test_Multiply_U16Y_U16X_U32XY
	Test_Multiply_U16Y_U16X_U32XY
	Test_Multiply_U16Y_U16X_U32XY
	Test_Multiply_U16Y_U16X_U32XY

	RTS



PAGE_ROUTINE Math_Multiply_4
	Text_SetColor	4
	Text_PrintString "Multiply_S16Y_S16X_S16Y"

	.macro Test_Multiply_S16Y_S16X_S16Y
		.local factorX, factorY

		STATIC_RANDOM_MIN_MAX	factorX, -32768, 32767
		STATIC_RANDOM_MIN_MAX	factorY, -32768, 32767

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%7i * %6i = ", factorY, factorX)

		LDY	#.loword(factorY)
		LDX	#.loword(factorX)
		JSR_MTH	Math__Multiply_S16Y_S16X_S16Y

		Check_16Y (factorY * factorX)
		JSR	Text__PrintDecimal_S16Y
	.endmacro

	Test_Multiply_S16Y_S16X_S16Y
	Test_Multiply_S16Y_S16X_S16Y
	Test_Multiply_S16Y_S16X_S16Y
	Test_Multiply_S16Y_S16X_S16Y
	Test_Multiply_S16Y_S16X_S16Y


	Text_SetColor	4
	Text_NewLine
	Text_NewLine
	Text_PrintString "Multiply_S16Y_S16X_S32XY"

	.macro Test_Multiply_S16Y_S16X_S32XY
		.local factorX, factorY

		STATIC_RANDOM_MIN_MAX	factorX, -32768, 32767
		STATIC_RANDOM_MIN_MAX	factorY, -32768, 32767

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%7i * %6i = ", factorY, factorX)

		LDY	#.loword(factorY)
		LDX	#.loword(factorX)
		JSR_MTH	Math__Multiply_S16Y_S16X_S32XY

		Check_32XY (factorY * factorX)
		JSR	Text__PrintDecimal_S32XY
	.endmacro

	Test_Multiply_S16Y_S16X_S32XY
	Test_Multiply_S16Y_S16X_S32XY
	Test_Multiply_S16Y_S16X_S32XY
	Test_Multiply_S16Y_S16X_S32XY
	Test_Multiply_S16Y_S16X_S32XY

	RTS



PAGE_ROUTINE Math_Multiply_5
	Text_SetColor	4
	Text_PrintString "Multiply_U32XY_U8A_U32XY"

	.macro Test_Multiply_U32XY_U8A_U32XY
		.local factor32, factorA

		; Prevent overflow
		STATIC_RANDOM_MIN_MAX	factor32, 0, 999999999
		STATIC_RANDOM_MIN_MAX	factorA, 0, 256

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%11u*%3u = ", factor32, factorA)

		LDXY	#factor32
		LDA	#factorA
		JSR_MTH	Math__Multiply_U32XY_U8A_U32XY

		Check_32XY (factor32 * factorA)
		JSR	Text__PrintDecimal_U32XY
	.endmacro

	Test_Multiply_U32XY_U8A_U32XY
	Test_Multiply_U32XY_U8A_U32XY
	Test_Multiply_U32XY_U8A_U32XY
	Test_Multiply_U32XY_U8A_U32XY
	Test_Multiply_U32XY_U8A_U32XY


	Text_SetColor	4
	Text_NewLine
	Text_NewLine
	Text_PrintString "Multiply_S32XY_U8A_S32XY"

	.macro Test_Multiply_S32XY_U8A_S32XY
		.local factor32, factorA

		; prevent overflow
		STATIC_RANDOM_MIN_MAX	factor32, -999999999, 999999999
		STATIC_RANDOM_MIN_MAX	factorA, 0, 256

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%11i*%3u = ", factor32, factorA)

		LDXY	#factor32
		LDA	#factorA
		JSR_MTH	Math__Multiply_S32XY_U8A_S32XY

		Check_32XY (factor32 * factorA)
		JSR	Text__PrintDecimal_U32XY
	.endmacro

	Test_Multiply_S32XY_U8A_S32XY
	Test_Multiply_S32XY_U8A_S32XY
	Test_Multiply_S32XY_U8A_S32XY
	Test_Multiply_S32XY_U8A_S32XY
	Test_Multiply_S32XY_U8A_S32XY

	RTS



PAGE_ROUTINE Math_Multiply_6
	Text_SetColor	4
	Text_PrintString "Multiply_U32_U16Y_U32XY"

	.macro Test_Multiply_U32_U16Y_U32XY 
		.local factor32, factorY

		; Prevent overflow
		STATIC_RANDOM_MIN_MAX	factor32, 0, 999999999
		STATIC_RANDOM_MIN_MAX	factorY, 0, 65535

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%11u*%5u=", factor32, factorY)

		LDXY	#factor32
		STXY	Math__factor32
		LDY	#factorY
		JSR_MTH	Math__Multiply_U32_U16Y_U32XY

		Check_32XY (factor32 * factorY)
		JSR	Text__PrintDecimal_U32XY
	.endmacro

	Test_Multiply_U32_U16Y_U32XY
	Test_Multiply_U32_U16Y_U32XY
	Test_Multiply_U32_U16Y_U32XY
	Test_Multiply_U32_U16Y_U32XY
	Test_Multiply_U32_U16Y_U32XY


	Text_SetColor	4
	Text_NewLine
	Text_NewLine
	Text_PrintString "Multiply_S32_U16Y_S32XY"

	.macro Test_Multiply_S32_U16Y_S32XY
		.local factor32, factorY

		; Prevent overflow
		STATIC_RANDOM_MIN_MAX	factor32, -999999999, 999999999
		STATIC_RANDOM_MIN_MAX	factorY, 0, 65535

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%10i*%5i=", factor32, factorY)

		LDXY	#factor32
		STXY	Math__factor32
		LDY	#factorY
		JSR_MTH	Math__Multiply_S32_U16Y_S32XY

		Check_32XY (factor32 * factorY)
		JSR	Text__PrintDecimal_S32XY
	.endmacro

	Test_Multiply_S32_U16Y_S32XY
	Test_Multiply_S32_U16Y_S32XY
	Test_Multiply_S32_U16Y_S32XY
	Test_Multiply_S32_U16Y_S32XY
	Test_Multiply_S32_U16Y_S32XY

	RTS



PAGE_ROUTINE Math_Multiply_7
	Text_SetColor	4
	Text_PrintString "Multiply_U32_S16Y_32XY"

	.macro Test_Multiply_U32_S16Y_32XY
		.local factor32, factorY

		; Prevent overflow
		STATIC_RANDOM_MIN_MAX	factor32, 999999999
		STATIC_RANDOM_MIN_MAX	factorY, -32768, 32767

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%10i*%6i=", factor32, factorY)

		LDXY	#factor32
		STXY	Math__factor32
		LDY	#.loword(factorY)
		JSR_MTH	Math__Multiply_U32_S16Y_32XY

		Check_32XY (factor32 * factorY)
		JSR	Text__PrintDecimal_S32XY
	.endmacro

	Test_Multiply_U32_S16Y_32XY
	Test_Multiply_U32_S16Y_32XY
	Test_Multiply_U32_S16Y_32XY
	Test_Multiply_U32_S16Y_32XY
	Test_Multiply_U32_S16Y_32XY


	Text_SetColor	4
	Text_NewLine
	Text_NewLine
	Text_PrintString "Multiply_S32_S16Y_S32XY"

	.macro Test_Multiply_S32_S16Y_S32XY
		.local factor32, factorY

		; Prevent overflow
		STATIC_RANDOM_MIN_MAX	factor32, -999999999, 999999999
		STATIC_RANDOM_MIN_MAX	factorY, -32768, 32767

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%10i*%6i=", factor32, factorY)

		LDXY	#factor32
		STXY	Math__factor32
		LDY	#.loword(factorY)
		JSR_MTH	Math__Multiply_S32_S16Y_S32XY

		Check_32XY (factor32 * factorY)
		JSR	Text__PrintDecimal_S32XY
	.endmacro

	Test_Multiply_S32_S16Y_S32XY
	Test_Multiply_S32_S16Y_S32XY
	Test_Multiply_S32_S16Y_S32XY
	Test_Multiply_S32_S16Y_S32XY
	Test_Multiply_S32_S16Y_S32XY

	RTS



PAGE_ROUTINE Math_Multiply_8
	Text_SetColor	4
	Text_PrintString "Multiply_U32_U32XY_U32XY (hex)"

	.macro Test_Multiply_U32_U32XY_U32XY
		.local factor32, factorXY

		; Prevent overflow
		STATIC_RANDOM_MIN_MAX	factor32, 0, 999999999
		STATIC_RANDOM_MIN_MAX	factorXY, 0, 999999999

		; no new line, due to overflow.
		Text_SetColor	0
		Text_PrintString .sprintf("%8X * %8X = ", factor32, factorXY)

		LDXY	#factor32
		STXY	Math__factor32
		LDXY	#factorXY
		JSR_MTH	Math__Multiply_U32_U32XY_U32XY

		Check_32XY (factor32 * factorXY)
		Text_PrintHex	Math__product32
	.endmacro

	Test_Multiply_U32_U32XY_U32XY
	Test_Multiply_U32_U32XY_U32XY
	Test_Multiply_U32_U32XY_U32XY
	Test_Multiply_U32_U32XY_U32XY
	Test_Multiply_U32_U32XY_U32XY


	Text_SetColor	4
	Text_NewLine
	Text_PrintString "Multiply_S32_S32XY_S32XY (hex)"

	.macro Test_Multiply_S32_S32XY_S32XY
		.local factor32, factorXY

		; Prevent overflow
		STATIC_RANDOM_MIN_MAX	factor32, -999999999, 999999999
		STATIC_RANDOM_MIN_MAX	factorXY, -999999999, 999999999

		; no new line, due to overflow.
		Text_SetColor	0
		Text_PrintString .sprintf("%8X * %8X = ", factor32, factorXY)

		LDXY	#factor32
		STXY	Math__factor32
		LDXY	#factorXY
		JSR_MTH	Math__Multiply_S32_S32XY_S32XY

		Check_32XY (factor32 * factorXY)
		Text_PrintHex	Math__product32
	.endmacro

	Test_Multiply_S32_S32XY_S32XY
	Test_Multiply_S32_S32XY_S32XY
	Test_Multiply_S32_S32XY_S32XY
	Test_Multiply_S32_S32XY_S32XY
	Test_Multiply_S32_S32XY_S32XY

	RTS



PAGE_ROUTINE Math_Divide_1
	Text_SetColor	4
	Text_PrintString "Divide_U16Y_U16X"

	.macro Test_Divide_U16Y_U16X
		.local dividend, divisor

		STATIC_RANDOM_MIN_MAX	dividend, 1000, 65535
		STATIC_RANDOM_MIN_MAX	divisor, 1, 5000

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%8u / %5u = ", dividend, divisor)

		LDY	#dividend
		LDX	#divisor
		JSR_MTH	Math__Divide_U16Y_U16X

		PHX
		Check_16Y (dividend / divisor)
		JSR	Text__PrintDecimal_U16Y

		Text_SetColor	0
		Text_PrintString " r "

		PLY
		Check_16Y (dividend .mod divisor)
		JSR	Text__PrintDecimal_U16Y
	.endmacro

	Test_Divide_U16Y_U16X
	Test_Divide_U16Y_U16X
	Test_Divide_U16Y_U16X
	Test_Divide_U16Y_U16X
	Test_Divide_U16Y_U16X


	Text_SetColor	4
	Text_NewLine
	Text_NewLine
	Text_PrintString "Divide_S16Y_U16X"

	.macro Test_Divide_S16Y_U16X dividend, divisor
		.local dividend, divisor

		STATIC_RANDOM_MIN_MAX	dividend, -32768, 32767
		STATIC_RANDOM_MIN_MAX	divisor, 1, 5000

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%8i / %5i = ", dividend, divisor)

		LDY	#.loword(dividend)
		LDX	#.loword(divisor)
		JSR_MTH	Math__Divide_S16Y_U16X

		PHX
		Check_16Y (dividend / divisor)
		JSR	Text__PrintDecimal_S16Y

		Text_SetColor	0
		Text_PrintString " r "

		PLY
		.if dividend .mod divisor > 0
			Check_16Y (dividend .mod divisor)
		.else
			Check_16Y -(dividend .mod divisor)
		.endif
		JSR	Text__PrintDecimal_S16Y
	.endmacro

	Test_Divide_U16Y_U16X
	Test_Divide_U16Y_U16X
	Test_Divide_U16Y_U16X
	Test_Divide_U16Y_U16X
	Test_Divide_U16Y_U16X

	RTS



PAGE_ROUTINE Math_Divide_2
	Text_SetColor	4
	Text_PrintString "Divide_U16Y_S16X"

	.macro Test_Divide_U16Y_S16X
		.local dividend, divisor

		STATIC_RANDOM_MIN_MAX	dividend, 0, 65535
		STATIC_RANDOM_MIN_MAX	divisor, -5000, 5000

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%8i / %5i = ", dividend, divisor)

		LDY	#.loword(dividend)
		LDX	#.loword(divisor)
		JSR_MTH	Math__Divide_U16Y_S16X

		PHX
		Check_16Y (dividend / divisor)
		JSR	Text__PrintDecimal_S16Y

		Text_SetColor	0
		Text_PrintString " r "

		PLY
		.if dividend .mod divisor > 0
			Check_16Y (dividend .mod divisor)
		.else
			Check_16Y -(dividend .mod divisor)
		.endif
		JSR	Text__PrintDecimal_S16Y
	.endmacro

	Test_Divide_U16Y_S16X
	Test_Divide_U16Y_S16X
	Test_Divide_U16Y_S16X
	Test_Divide_U16Y_S16X
	Test_Divide_U16Y_S16X


	Text_SetColor	4
	Text_NewLine
	Text_NewLine
	Text_PrintString "Divide_S16Y_S16X"

	.macro Test_Divide_S16Y_S16X
		.local dividend, divisor

		STATIC_RANDOM_MIN_MAX	dividend, -32768, 32767
		STATIC_RANDOM_MIN_MAX	divisor, -5000, 5000

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%8i /%5i = ", dividend, divisor)

		LDY	#.loword(dividend)
		LDX	#.loword(divisor)
		JSR_MTH	Math__Divide_S16Y_S16X

		PHX
		Check_16Y (dividend / divisor)
		JSR	Text__PrintDecimal_S16Y

		Text_SetColor	0
		Text_PrintString " r "

		PLY
		.if dividend .mod divisor > 0
			Check_16Y (dividend .mod divisor)
		.else
			Check_16Y -(dividend .mod divisor)
		.endif
		JSR	Text__PrintDecimal_S16Y
	.endmacro

	Test_Divide_S16Y_S16X
	Test_Divide_S16Y_S16X
	Test_Divide_S16Y_S16X
	Test_Divide_S16Y_S16X
	Test_Divide_S16Y_S16X

	RTS



PAGE_ROUTINE Math_Divide_3
	Text_SetColor	4
	Text_PrintString "Divide_U16Y_U8A"

	.macro Test_Divide_U16Y_U8A
		.local dividend, divisor

		STATIC_RANDOM_MIN_MAX	dividend, 0, 65535
		STATIC_RANDOM_MIN_MAX	divisor, 1, 256

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%8u/%4u = ", dividend, divisor)

		LDY	#dividend
		LDA	#divisor
		JSR_MTH	Math__Divide_U16Y_U8A
		PHX

		Check_16Y (dividend / divisor)
		JSR	Text__PrintDecimal_U16Y

		Text_SetColor	0
		Text_PrintString " r "

		PLY
		Check_16Y (dividend .mod divisor)
		JSR	Text__PrintDecimal_U16Y
	.endmacro

	Test_Divide_U16Y_U8A
	Test_Divide_U16Y_U8A
	Test_Divide_U16Y_U8A
	Test_Divide_U16Y_U8A
	Test_Divide_U16Y_U8A


	Text_SetColor	4
	Text_NewLine
	Text_NewLine
	Text_PrintString "Divide_U32_U8A"

	.macro Test_Divide_U32_U8A
		.local dividend, divisor

		STATIC_RANDOM_MIN_MAX	dividend, 0, 999999999
		STATIC_RANDOM_MIN_MAX	divisor, 1, 256

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%9u/%3u = ", dividend, divisor)

		LDXY	#dividend
		STXY	Math__dividend32
		LDA	#divisor
		JSR_MTH	Math__Divide_U32_U8A

		PHA
		LDXY	Math__result32
		Check_32XY (dividend / divisor)
		JSR	Text__PrintDecimal_U32XY

		Text_SetColor	0
		Text_PrintString " r "

		PLA
		Check_U8A (dividend .mod divisor)
		JSR	Text__PrintDecimal_U8A
	.endmacro

	Test_Divide_U32_U8A
	Test_Divide_U32_U8A
	Test_Divide_U32_U8A
	Test_Divide_U32_U8A
	Test_Divide_U32_U8A

	RTS


PAGE_ROUTINE Math_Divide_4
	Text_SetColor	4
	Text_PrintString "Divide_U32_U32 (hex)"
	.macro Test_Divide_U32_U32
		.local dividend, divisor

		STATIC_RANDOM_MIN_MAX	dividend, 0, 99999999
		STATIC_RANDOM_MIN_MAX	divisor, 1, 9999999

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%10d / %10d =", dividend, divisor)
		Text_NewLine
		Text_PrintString "        "

		LDXY	#dividend
		STXY	Math__dividend32
		LDXY	#divisor
		STXY	Math__divisor32
		JSR_MTH	Math__Divide_U32_U32

		LDXY	Math__result32
		Check_32XY (dividend / divisor)
		JSR	Text__PrintDecimal_U32XY

		Text_SetColor	0
		Text_PrintString " r "

		LDXY	Math__remainder32
		Check_32XY (dividend .mod divisor)
		JSR	Text__PrintDecimal_U32XY
	.endmacro

	Test_Divide_U32_U32
	Test_Divide_U32_U32
	Test_Divide_U32_U32
	Test_Divide_U32_U32
	Test_Divide_U32_U32

	RTS



PAGE_ROUTINE Math_Divide_5
	Text_SetColor	4
	Text_PrintString "Divide_S32_S32"
	.macro Test_Divide_S32_S32
		.local dividend, divisor

		STATIC_RANDOM_MIN_MAX	dividend, -999999999, 999999999
		STATIC_RANDOM_MIN_MAX	divisor, -9999999, 9999999

		Text_NewLine
		Text_SetColor	0
		Text_PrintString .sprintf("%10i / %10i =", dividend, divisor)
		Text_NewLine
		Text_PrintString "        "

		LDXY	#dividend
		STXY	Math__dividend32
		LDXY	#divisor
		STXY	Math__divisor32
		JSR_MTH	Math__Divide_S32_S32

		LDXY	Math__result32
		Check_32XY (dividend / divisor)
		JSR	Text__PrintDecimal_S32XY

		Text_SetColor	0
		Text_PrintString " r "

		LDXY	Math__remainder32
		.if dividend .mod divisor > 0
			Check_32XY (dividend .mod divisor)
		.else
			Check_32XY -(dividend .mod divisor)
		.endif
		JSR	Text__PrintDecimal_S32XY
	.endmacro

	Test_Divide_S32_S32
	Test_Divide_S32_S32
	Test_Divide_S32_S32
	Test_Divide_S32_S32
	Test_Divide_S32_S32

	RTS

