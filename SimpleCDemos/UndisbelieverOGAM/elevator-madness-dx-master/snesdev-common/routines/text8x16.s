
.include "includes/import_export.inc"
.include "includes/structure.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "routines/text.h"
.include "routines/text8x8.h"

.setcpu "65816"


MODULE Text8x16

CONST	SECOND_HALF_TILE, 112

.rodata

LABEL Interface
	.addr	Text8x8__NewLine_DoubleSpacing	; NewLine
	.addr	PrintChar			; PrintChar
	.addr	Text8x8__CursorMoved		; CursorMoved
	.addr	Text8x8__GetWordLength		; GetWordLength
	.addr	Text8x8__SpecialCharacter	; SpecialCharacter


.code

.A8
.I16
ROUTINE PrintChar
	; If character is newline call NewLine
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

	ADD	#SECOND_HALF_TILE
	STA	f:Text__buffer + 64, X

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



