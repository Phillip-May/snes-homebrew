
.include "includes/import_export.inc"
.include "includes/structure.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "routines/text.h"

.setcpu "65816"

MODULE Text8x8

.zeropage
	WORD	tmp

.rodata

LABEL SingleSpacingInterface
	.addr	NewLine_SingleSpacing	; NewLine
	.addr	PrintChar		; PrintChar
	.addr	CursorMoved		; CursorMoved
	.addr	GetWordLength		; GetWordLength
	.addr	SpecialCharacter	; SpecialCharacter

LABEL DoubleSpacingInterface
	.addr	NewLine_DoubleSpacing	; NewLine
	.addr	PrintChar		; PrintChar
	.addr	CursorMoved		; CursorMoved
	.addr	GetWordLength		; GetWordLength
	.addr	SpecialCharacter	; SpecialCharacter

.code

.A8
.I16
ROUTINE NewLine_SingleSpacing
	REP	#$30
.A16

	; bufferPos = (Bufferpos & $FFC0) + 64 + (Text__windowStart & $3F)
	LDA	Text__window + TextWindow::bufferPos
	AND	#$FFC0
	ADD	#64
	STA	tmp

	LDA	Text__window + TextWindow::windowStart
	AND	#$003F
	ORA	tmp
	STA	Text__window + TextWindow::bufferPos

	; If out of bounds
	CMP	Text__window + TextWindow::windowEnd
	IF_GT
		LDA	Text__window + TextWindow::windowStart
		STA	Text__window + TextWindow::bufferPos
		AND	#$FFC0
		STA	tmp
	ENDIF

	SEP	#$20
.A8

	; reset tiles Left in line
	LDA	Text__window + TextWindow::lineTilesWidth
	STA	Text__window + TextWindow::tilesLeftInLine

	RTS



.A8
.I16
ROUTINE NewLine_DoubleSpacing
	REP	#$30
.A16

	; bufferPos = (Bufferpos & $FFC0) + 128 + (Text__windowStart & $3F)
	LDA	Text__window + TextWindow::bufferPos
	AND	#$FFC0
	ADD	#128
	STA	tmp

	LDA	Text__window + TextWindow::windowStart
	AND	#$003F
	ORA	tmp
	STA	Text__window + TextWindow::bufferPos

	; If out of bounds
	CMP	Text__window + TextWindow::windowEnd
	IF_GT
		LDA	Text__window + TextWindow::windowStart
		STA	Text__window + TextWindow::bufferPos
		AND	#$FFC0
		STA	tmp
	ENDIF

	SEP	#$20
.A8

	; reset tiles Left in line
	LDA	Text__window + TextWindow::lineTilesWidth
	STA	Text__window + TextWindow::tilesLeftInLine

	RTS



.A8
.I16
ROUTINE PrintChar
	; If character is new line then call NewLine
	CMP	#EOL
	IF_EQ
		LDX	Text__window + TextWindow::textInterfaceAddr
		JMP	(TextInterface::NewLine, X)
	ENDIF

	; if A in range FIRST_CHARACTER to LAST_CHARACTER
	;     A = A - ASCII_DELTA
	; else
	;     A = INVALID_CHARACTER

	SUB	#Text__ASCII_DELTA
	CMP	#Text__LAST_CHARACTER - Text__ASCII_DELTA
	IF_GE
		LDA	#TEXT_INVALID - Text__ASCII_DELTA
	ENDIF

	; Show character
	REP	#$30
.A16
	LDX	Text__window + TextWindow::bufferPos

	AND	#$00FF
	ADD	Text__window + TextWindow::tilemapOffset
	STA	f:Text__buffer, X

	INX
	INX
	STX	Text__window + TextWindow::bufferPos

	SEP	#$20
.A8

	DEC	Text__window + TextWindow::tilesLeftInLine
	IF_ZERO
		LDX	Text__window + TextWindow::textInterfaceAddr
		JSR	(TextInterface::NewLine, X)
	ENDIF

	RTS



.A8
.I16
ROUTINE CursorMoved
	RTS


; No Special Characters
.A8
.I16
ROUTINE GetWordLength
	LDY	#0

	REPEAT
		LDA	[Text__stringPtr], Y
	WHILE_NOT_ZERO
		CMP	#EOL
		BEQ	BREAK_LABEL

		CMP	#' '
		BEQ	BREAK_LABEL

		INY
	WEND

	TYX

	REPEAT
		LDA	[Text__stringPtr], Y
		CMP	#' '
	WHILE_EQ
		INY
	WEND

	TXA

	RTS


; No Special Characters
.A8
.I16
ROUTINE SpecialCharacter
	CLC
	RTS

