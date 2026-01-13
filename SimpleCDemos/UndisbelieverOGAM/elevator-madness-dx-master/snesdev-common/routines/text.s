
.include "includes/import_export.inc"
.include "includes/structure.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "routines/math.h"
.include "routines/text.h"

.setcpu "65816"

MODULE Text

;; Public Variables
;; ================

.zeropage
	;; The position of the string
	FARADDR	stringPtr

.segment "WRAM7E"
	;; The text buffer
	;; In $7E so shadow RAM can also be accessed.
	WORD buffer, 32*32

.if TEXT_N_WINDOWS > 1
	;; Storage of the various Text Windows
	STRUCT windowArray, TextWindow, TEXT_N_WINDOWS
.endif

.segment "SHADOW"
	;; If zero, then update buffer to VRAM on VBlank
	BYTE updateBufferIfZero

	;; Word address of the tilemap in VRAM
	WORD vramMapAddr

	;; The current Window settings
	STRUCT window, TextWindow

	;; The character to use when padding a string.
	BYTE paddingCharacter

.if TEXT_N_WINDOWS > 1
	;; The current window index.
	BYTE currentWindow
.endif


;; Private Variables
;; =================

.segment "SHADOW"
	;; Storage area for '-' of signed character (worse case)
	BYTE decimalStringPrependSign
	;; Storage area to store decoded string.
	;; Long enough to cover the unsigned 32 bit integer and string terminator.
	BYTE decimalString, 11

.zeropage
	; Temporary values used by this module
	WORD	tmp
	WORD	tmp2
	WORD	tmp3


.code
.A8
.I16
ROUTINE SetColor
	; tilemapOffset = (tilemapOffset & $E3FF) | ((A & $7) << 10)
	AND	#$7
	ASL
	ASL

	STA	tmp

	LDA	window + TextWindow::tilemapOffset + 1
	AND	#$E3
	ORA	tmp
	STA	window + TextWindow::tilemapOffset + 1

	RTS



.A8
.I16
ROUTINE PrintDecimalPadded_U8A_3
	STA	tmp
	STZ	tmp + 1
	LDY	tmp
	LDA	#3
	BRA	PrintDecimalPadded_U16Y

.A8
.I16
ROUTINE PrintDecimalPadded_U8A_2
	STA	tmp
	STZ	tmp + 1
	LDY	tmp
	LDA	#2
	BRA	PrintDecimalPadded_U16Y

.A8
.I16
ROUTINE PrintDecimalPadded_U8A_1
	STA	tmp
	STZ	tmp + 1
	LDY	tmp
	LDA	#1

	.assert * = PrintDecimalPadded_U16Y, lderror, "Bad Flow Control" ; Faster than BRA

.A8
.I16
ROUTINE PrintDecimalPadded_U16Y
	JSR	ConvertDecimalStringPadded_U16Y
	BRA	PrintString


.A8
.I16
ROUTINE PrintDecimalPadded_S16Y
	JSR	ConvertDecimalStringPadded_S16Y
	BRA	PrintString


; INPUT: XY = value
.A8
.I16
ROUTINE PrintDecimal_U32XY
	LDA	#1
	.assert * = PrintDecimalPadded_U32XY, lderror, "Bad Flow Control"

ROUTINE PrintDecimalPadded_U32XY
	JSR	ConvertDecimalString_U32XY
	BRA	PrintString



; INPUT: XY = value
.A8
.I16
ROUTINE PrintDecimal_S32XY
	LDA	#1
	.assert * = PrintDecimalPadded_S32XY, lderror, "Bad Flow Control"

ROUTINE PrintDecimalPadded_S32XY
	JSR	ConvertDecimalString_S32XY
	BRA	PrintString



.A8
.I16
ROUTINE PrintDecimal_S8A
	; Convert to S16Y

	STA	tmp
	CMP	#$80
	IF_GE
		LDA	#$FF
		STA	tmp + 1
	ELSE
		STZ	tmp + 1
	ENDIF
	LDY	tmp

	.assert * = PrintDecimal_S16Y, lderror, "Bad Flow Control"

.A8
.I16
ROUTINE PrintDecimal_S16Y
	JSR	ConvertDecimalString_S16Y
	BRA	PrintString


.A8
.I16
ROUTINE PrintDecimal_U8A
	STA	tmp
	STZ	tmp + 1
	LDY	tmp

	.assert * = PrintDecimal_U16Y, lderror, "Bad Flow Control"


.A8
.I16
ROUTINE PrintDecimal_U16Y
	JSR	ConvertDecimalString_U16Y
	.assert * = PrintString, lderror, "Bad Flow Control"


.A8
.I16
ROUTINE	PrintString
	STX	stringPtr
	STA	stringPtr + 2

	JMP	(window + TextWindow::printStringAddr)


.A8
.I16
ROUTINE PrintHex_8A
	TAX

	LSR
	LSR
	LSR
	LSR
	CMP	#10
	IF_GE
		CLC
		ADC	#'A' - 10
	ELSE
		ADC	#'0'
	ENDIF
	STA	decimalString + 0

	TXA
	AND	#$0F
	CMP	#10
	IF_GE
		CLC
		ADC	#'A' - 10
	ELSE
		ADC	#'0'
	ENDIF
	STA	decimalString + 1
	STZ	decimalString + 2

	LDA	#.bankbyte(decimalString)
	LDX	#decimalString

	BRA	PrintString


.I16
ROUTINE PrintHex_16Y
	PHP
	REP	#$20
.A16
	TYA
	XBA
	SEP	#$20
.A8
	TAX

	LSR
	LSR
	LSR
	LSR
	CMP	#10
	IF_GE
		CLC
		ADC	#'A' - 10
	ELSE
		ADC	#'0'
	ENDIF
	STA	decimalString + 0

	TXA
	AND	#$0F
	CMP	#10
	IF_GE
		CLC
		ADC	#'A' - 10
	ELSE
		ADC	#'0'
	ENDIF
	STA	decimalString + 1


	TYA
	LSR
	LSR
	LSR
	LSR
	CMP	#10
	IF_GE
		CLC
		ADC	#'A' - 10
	ELSE
		ADC	#'0'
	ENDIF
	STA	decimalString + 2

	TYA
	AND	#$0F
	CMP	#10
	IF_GE
		CLC
		ADC	#'A' - 10
	ELSE
		ADC	#'0'
	ENDIF
	STA	decimalString + 3

	STZ	decimalString + 4

	LDA	#.bankbyte(decimalString)
	LDX	#decimalString

	PLP
	JRA	PrintString



.A8
.I16
ROUTINE PrintChar
	LDX	window + TextWindow::textInterfaceAddr
	JSR	(TextInterface::PrintChar, X)

	STZ	Text__updateBufferIfZero
	RTS



ROUTINE NewLine
	LDX	window + TextWindow::textInterfaceAddr
	JMP	(TextInterface::NewLine, X)



ROUTINE SetCursor
	REP	#$30
.A16
.I16
	CPX	window + TextWindow::lineTilesWidth
	IF_GE
		LDX	#0
	ENDIF

	; lineStart = windowStart + Y * 64
	; if lineStart > windowEnd
	;	linestart = windowStart
	; bufferPos = lineStart + X * 2
	TYA
	XBA
	AND	#$FF00
	LSR
	LSR
	ADD	window + TextWindow::windowStart

	CMP	window + TextWindow::windowEnd
	IF_GE
		LDA	window + TextWindow::windowStart
	ENDIF
	STA	tmp

	TXA
	ASL
	ADD	tmp
	STA	window + TextWindow::bufferPos

	; LineWidth = lineTilesWidth - X
	SEP	#$20
.A8
	TXA
	STA	tmp

	LDA	window + TextWindow::lineTilesWidth
	SUB	tmp
	STA	window + TextWindow::tilesLeftInLine

	; Goto Callback
	LDX	window + TextWindow::textInterfaceAddr
	JMP	(TextInterface::CursorMoved, X)


.if TEXT_N_WINDOWS > 1
ROUTINE SelectWindow
	PHP
	SEP	#$30
.A8
.I8

	CMP	#TEXT_N_WINDOWS
	IF_GE
		LDA	#0
	ENDIF

	; Y = A * 15
	.assert .sizeof(TextWindow) = 15, error, "Incorrect TextWindow Size"
	.assert TEXT_N_WINDOWS * .sizeof(TextWindow) <= 255, error, "8 bit overflow"

	STA	tmp
	ASL
	ASL
	ASL
	ASL
	SUB	tmp

	TAY

_SelectWindowSkip:

	; Don't change bank, (Change Bank = 16 cycles, but only saves 8)

	LDX	currentWindow

	; Copy 15 bytes to windowArray
	LDA	window + 0
	STA	f:windowArray + 0, X

	REP	#$20
.A16
	; Save buffer
	LDA	window + 1
	STA	f:windowArray + 1, X
	LDA	window + 3
	STA	f:windowArray + 3, X
	LDA	window + 5
	STA	f:windowArray + 5, X
	LDA	window + 7
	STA	f:windowArray + 7, X
	LDA	window + 9
	STA	f:windowArray + 9, X
	LDA	window + 11
	STA	f:windowArray + 11, X
	LDA	window + 13
	STA	f:windowArray + 13, X


	TYX
	STX	currentWindow

	; Copy 15 bytes from windowArray
	; Save buffer
	LDA	f:windowArray + 0, X
	STA	window + 0
	LDA	f:windowArray + 2, X
	STA	window + 2
	LDA	f:windowArray + 4, X
	STA	window + 4
	LDA	f:windowArray + 6, X
	STA	window + 6
	LDA	f:windowArray + 8, X
	STA	window + 8
	LDA	f:windowArray + 10, X
	STA	window + 10
	LDA	f:windowArray + 12, X
	STA	window + 12

	SEP	#$20
.A8

	LDA	f:windowArray + 14, X
	STA	window + 14

	PLP
	RTS
.endif


.A8
.I16
ROUTINE SetupWindow
	STX	window + TextWindow::windowStart
	STY	window + TextWindow::windowEnd
	STA	window + TextWindow::flags

	REP	#$30
.A16
	; lineTilesWidth = ((windowEnd & $3F) - (windowStart & $3F)) / 2 + 1
	TXA
	AND	#$003F
	STA	tmp
	TYA
	AND	#$003F
	SUB	tmp
	LSR
	INC
	STA	window + TextWindow::lineTilesWidth

	SEP	#$20
.A8
	LDA	window + TextWindow::flags
	BIT	#Text__WINDOW_BORDER
	BNE	DrawBorder

	JMP	ClearWindow



ROUTINE DrawBorder
	REP	#$30
.A16
.I16

topLeft  := window + TextWindow::windowStart
topRight := <tmp
bottomLeft  := <tmp2
bottomRight := window + TextWindow::windowEnd

	LDA	window + TextWindow::windowStart
	AND	#$003F
	STA	bottomLeft
	LDA	window + TextWindow::windowEnd
	AND	#$FFC0
	ORA	bottomLeft
	STA	bottomLeft

	LDA	window + TextWindow::windowEnd
	AND	#$003F
	STA	topRight
	LDA	window + TextWindow::windowStart
	AND	#$FFC0
	ORA	topRight
	STA	topRight

	; Draw four corners
	LDA	window + TextWindow::tilemapOffset
	ADD	#BORDER_TOP_LEFT - Text__ASCII_DELTA
	LDX	topLeft
	STA	f:buffer - 66, X

	LDA	window + TextWindow::tilemapOffset
	ADD	#BORDER_TOP_RIGHT - Text__ASCII_DELTA
	LDX	topRight
	STA	f:buffer - 62, X

	LDA	window + TextWindow::tilemapOffset
	ADD	#BORDER_BOTTOM_LEFT - Text__ASCII_DELTA
	LDX	bottomLeft
	STA	f:buffer + 62, X

	LDA	window + TextWindow::tilemapOffset
	ADD	#BORDER_BOTTOM_RIGHT - Text__ASCII_DELTA
	LDX	bottomRight
	STA	f:buffer + 66, X

	LDA	window + TextWindow::tilemapOffset
	ADD	#BORDER_TOP - Text__ASCII_DELTA
	LDX	topLeft
	REPEAT
		STA	f:buffer - 64, X
		CPX	topRight
	WHILE_LT
		INX
		INX
	WEND

	LDA	window + TextWindow::tilemapOffset
	ADD	#BORDER_BOTTOM - Text__ASCII_DELTA
	LDX	bottomLeft
	REPEAT
		STA	f:buffer + 64, X
		CPX	bottomRight
	WHILE_LT
		INX
		INX
	WEND

	LDA	window + TextWindow::tilemapOffset
	ADD	#BORDER_LEFT - Text__ASCII_DELTA
	TAY

	LDX	topLeft
	REPEAT
		STA	f:buffer - 2, X

		CPX	bottomLeft
	WHILE_LT
		TXA
		ADD	#64
		TAX
		TYA
	WEND

	LDA	window + TextWindow::tilemapOffset
	ADD	#BORDER_RIGHT - Text__ASCII_DELTA
	TAY

	LDX	topRight
	REPEAT
		STA	f:buffer + 2, X

		CPX	bottomRight
	WHILE_LT
		TXA
		ADD	#64
		TAX
		TYA
	WEND

	BRA	_ClearWindow_skip_php



ROUTINE ClearWindow
	REP	#$30
.A16
.I16
_ClearWindow_skip_php:

	LDA	#TEXT_CLEAR - Text__ASCII_DELTA
	ADD	window + TextWindow::tilemapOffset
	TAY

	BRA	_FillWindow_p_on_stack



ROUTINE RemoveWindow
	PHP

	REP	#$30
.A16
.I16
	LDA	window + TextWindow::flags
	IF_BIT	#Text__WINDOW_BORDER
		; if has a border, then ensure that gets cleaned too
		LDA	window + TextWindow::windowStart
		SUB	#66
		STA	window + TextWindow::windowStart
		LDA	window + TextWindow::windowEnd
		ADD	#66
		STA	window + TextWindow::windowEnd
	ENDIF

	LDY	#0
	BRA	_FillWindow_p_on_stack



;; A helper function to fill the entirety of the window.
;; REQUIRES: 16 bit A, 16 bit Index, P on stack
;; INPUT: Y = the Tile to copy
.A16
.I16
ROUTINE _FillWindow_p_on_stack
startLine := <tmp
endLine	  := <tmp2

	; startLine = windowStart
	; endLine = (windowStart & $FFC0) + (windowEnd & $3F)
	; A = Y
	; repeat
	;    buffer[x] = A
	;    if (x >= windowEnd)
	;       break loop
	;    elseif x >= endLine
	;       startLine += 64
	;       endLine += 64
	;       x = startLine
	;    else
	;       x += 2
	LDA	window + TextWindow::windowStart
	STA	startLine

	AND	#$FFC0
	STA	endLine
	LDA	window + TextWindow::windowEnd
	AND	#$003F
	ORA	endLine
	STA	endLine

	TYA

	LDX	window + TextWindow::windowStart
	REPEAT
		STA	f:buffer, X

		CPX	window + TextWindow::windowEnd
	WHILE_LT
		CPX	endLine
		IF_EQ
			LDA	startLine
			ADD	#64
			STA	startLine
			TAX

			LDA	endLine
			ADD	#64
			STA	endLine

			TYA
			CONTINUE
		ENDIF

		INX
		INX
	WEND

	SEP	#$20
.A8
	; bufferPos = windowStart
	; tilesLeftInLine = lineTilesWidth
	LDX	window + TextWindow::windowStart
	STX	window + TextWindow::bufferPos

	LDA	window + TextWindow::lineTilesWidth
	STA	window + TextWindow::tilesLeftInLine

	; update on VBlank flag
	STZ	updateBufferIfZero

	LDX	window + TextWindow::textInterfaceAddr
	JMP	(TextInterface::CursorMoved, X)




ROUTINE ClearEntireBuffer
	PHP
	PHB
	REP	#$30
.A16
.I16
	LDA	#0
	STA	f:buffer

	LDX	#.loword(buffer)
	LDY	#.loword(buffer) + 2
	LDA	#.sizeof(buffer) - 3
	MVN	#.bankbyte(buffer), #.bankbyte(buffer)

; MVN changes DB to .bankbyte(buffer)
.assert .bankbyte(buffer) = $7e, lderror, "buffer in wrong Data Bank"

	SEP	#$20
.A8
	STZ	updateBufferIfZero

	PLB
	PLP
	RTS



;; Print String Methods
;; ====================

.A8
.I16
ROUTINE PrintStringBasic

	REPEAT
		LDA	[stringPtr]
	WHILE_NOT_ZERO
		LDX	window + TextWindow::textInterfaceAddr
		JSR	(TextInterface::PrintChar, X)

		REP	#$20
.A16
		INC	stringPtr
.A8
		SEP	#$20
	WEND

	STZ	Text__updateBufferIfZero

	RTS



.A8
.I16
ROUTINE PrintStringWordWrapping
	; repeat
	;	length, lengthAfterSpaces = getWordLength()
	; 	if length > tilesLeftInLine AND length * 2 < lineTilesWidth
	;		NewLine()
	;	if lengthAfterSpaces > tilesLeftInLine
	;		toPrint = length - 1
	;	else
	;		toPrint = lengthAfterSpaces - 1
	;
	;	repeat
	;		char = [stringPtr]
	;		if char == 0
	;			return
	;		stringPtr++
	;		print char
	;	until --toPrint < 0
	;
	;	; Skip any spaces that may have not been printed
	;	while [stringPtr] == ' '
	;		stringPtr++
	; forever

	REPEAT
		LDX	window + TextWindow::textInterfaceAddr
		JSR	(TextInterface::GetWordLength, X)
		STA	tmp
		STY	tmp2

		DEC	; faster than IF_GT
		CMP	window + TextWindow::tilesLeftInLine
		IF_GE
			ASL
			CMP	window + TextWindow::lineTilesWidth
			IF_LT
				LDX	window + TextWindow::textInterfaceAddr
				JSR	(TextInterface::NewLine, X)
				LDX	window + TextWindow::textInterfaceAddr
			ENDIF
		ENDIF

		LDA	tmp2
		DEC
		CMP	window + TextWindow::tilesLeftInLine
		IF_GE
			LDA	tmp
			DEC
		ENDIF
		STA	tmp

		REPEAT
			LDA	[stringPtr]
			BEQ	_EndPrintWrapLoop

			REP	#$20
			INC	stringPtr
			SEP	#$20
.A8

			LDX	window + TextWindow::textInterfaceAddr
			JSR	(TextInterface::PrintChar, X)

			DEC	tmp
		UNTIL_MINUS

		; Skip any spaces that may have not been printed
		REPEAT
			LDA	[stringPtr]
			CMP	#' '
		WHILE_EQ
			REP	#$20
			INC	stringPtr
			SEP	#$20
		WEND
	FOREVER

_EndPrintWrapLoop:

	STZ	Text__updateBufferIfZero

	RTS



;; Helpful Functions
;; =================

padding := tmp2
position := tmp3


.A8
.I16
ROUTINE ConvertDecimalString_S16Y
	REP	#$20
.A16
	TYA
	IF_MINUS
		NEG16
		TAY

		SEP	#$20
.A8
		JSR	ConvertDecimalString_U16Y

		; Draw negative sign.
		DEX
		LDA	#'-'
		STA	0, X

		LDA	#.bankbyte(decimalString)
		RTS
	ENDIF

	SEP	#$20
.A8
	.assert * = ConvertDecimalString_U16Y, lderror, "Bad Flow"


.A8
.I16
ROUTINE ConvertDecimalString_U16Y
	LDX	#decimalString + .sizeof(decimalString) - 1

	REPEAT
		STY	WRDIVL

		LDA	#10
		STA	WRDIVB

		; Wait 16 Cycles
		PHD			; 4
		PLD			; 5
		PHB			; 3
		PLB			; 4

		LDA	RDMPY		; remainder
		ADD	#'0'
		DEX
		STA	0, X

		LDY	RDDIV		; result
	UNTIL_ZERO

	STZ	decimalString + .sizeof(decimalString) - 1

	LDA	#.bankbyte(decimalString)
	RTS



.A8
.I16
ROUTINE ConvertDecimalStringPadded_S16Y
	STA	tmp

	REP	#$20
.A16
	TYA
	IF_MINUS
		NEG16
		TAY

		; Decrement padding, an extra character would be printed.
		; Must match check in ConvertDecimalStringPadded_U16Y
		LDA	tmp
		AND	#$0007
		DEC
		IF_MINUS
			LDA	#0
		ENDIF

		JSR	_ConvertDecimalStringPadded_U16Y_AfterCheckA

		SEP	#$20
.A8
		; Draw negative sign.
		DEX
		LDA	#'-'
		STA	0, X

		LDA	#.bankbyte(decimalString)
		RTS
	ENDIF

	LDA	tmp
	.assert * = ConvertDecimalStringPadded_U16Y, lderror, "Bad Flow"


.A8
.I16
ROUTINE ConvertDecimalStringPadded_U16Y
	REP	#$20
.A16
	; Faster than alternative
	; Never need more than 7 chars anyway
	.assert .sizeof(decimalString) > 7, error, "decimalString too small"
	AND	#$0007
_ConvertDecimalStringPadded_U16Y_AfterCheckA:
	STA	tmp

	LDA	#decimalString + .sizeof(decimalString) - 1 + 1
	SUB	tmp
	STA	tmp

	SEP	#$20
.A8

	JSR	ConvertDecimalString_U16Y

	CPX	tmp
	IF_GE
		LDA	paddingCharacter

		REPEAT
			DEX
			STA	0, X

			CPX	tmp
		UNTIL_LT
	ENDIF

	LDA	#.bankbyte(decimalString)
	RTS


.A8
.I16
ROUTINE ConvertDecimalString_S32XY
	REP	#$20
.A16

	AND	#$00FF
	STA	padding

	TXA
	IF_MINUS
		EOR	#$FFFF
		STA	Math__dividend32 + 2

		TYA
		EOR	#$FFFF
		STA	Math__dividend32

		INC32	Math__dividend32

		SEP	#$20
.A8

		; Decrement padding, an extra character would be printed.
		DEC	padding
		IF_MINUS
			STZ	padding
		ENDIF

		JSR	_ConvertDecimalStringPadded_U32XY_AfterVarStore

		; Draw negative sign.
		DEX
		LDA	#'-'
		STA	0, X

		LDA	#.bankbyte(decimalString)
		RTS
	ENDIF

	SEP	#$20
.A8
	STXY	Math__dividend32

	BRA	_ConvertDecimalStringPadded_U32XY_AfterVarStore


.A8
.I16
ROUTINE ConvertDecimalString_U32XY

	STXY	Math__dividend32

	STA	padding
	STZ	padding + 1

_ConvertDecimalStringPadded_U32XY_AfterVarStore:

	LDX	#.sizeof(decimalString) - 2
	STX	position

	REPEAT
		LDA	#10
		JSR	Math__Divide_U32_U8A

		LDX	position

		STA	decimalString, X

		DEC	position
	UNTIL_MINUS

	STZ	decimalString + .sizeof(decimalString) - 1

	REP	#$20
.A16
	; no need for range checking, that is what the BMI is for
	LDA	#.sizeof(decimalString) - 1
	SUB	padding
	TAY
	SEP	#$20
.A8

	LDX	#decimalString
	REPEAT
		LDA	0, X
	WHILE_ZERO
		DEY
		BMI	BREAK_LABEL

		INX
	WEND

	TXY
	REPEAT
		CPY	#decimalString + .sizeof(decimalString) - 2
	WHILE_LT
		LDA	0, Y
		BNE	BREAK_LABEL

		LDA	paddingCharacter
		STA	0, Y

		INY
	WEND

	REPEAT
		CPY	#decimalString + .sizeof(decimalString) - 1
	WHILE_LT
		LDA	0, Y
		ADD	#'0'
		STA	0, Y

		INY
	WEND

	LDA	#.bankbyte(decimalString)
	RTS


ENDMODULE


