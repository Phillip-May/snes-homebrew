
.include "routines/pixelbuffer.h"

.include "includes/import_export.inc"
.include "includes/structure.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"

.include "routines/block.h"

.setcpu "65816"

.assert PIXELBUFFER_WIDTH_LOG2 >= 3 .and PIXELBUFFER_WIDTH_LOG2 <= 6, error, "PIXELBUFFER_WIDTH_LOG2 must be between 3 and 6"
.assert PIXELBUFFER_HEIGHT >= 1 .and PIXELBUFFER_HEIGHT <= 64, error, "PIXELBUFFER_HEIGHT must be between 1 and 64"


MODULE PixelBuffer

.define TILE_SIZE 16

.segment PIXELBUFFER_BANK
	BYTE	buffer, PIXELBUFFER_WIDTH * PIXELBUFFER_HEIGHT * TILE_SIZE
	WORD	colorBits


	WORD	colorMask
	WORD	cm_tmp
	WORD	cm_tmp2

	WORD	tmp1
	WORD	tmp2
	WORD	tmp3

PixelBuffer__bufferBank = .bankbyte(buffer)
bufferBank = .bankbyte(buffer)
.assert (bufferBank & $FE) = $7E, error, "PIXELBUFFER_BANK must be WRAM7E or WRAM7F"


.code


.if PIXELBUFFER_HEIGHT * PIXELBUFFER_WIDTH < 1023

	; A - tile offset
	.A16
	.I16
	ROUTINE WriteTileMapToVram

		.if PIXELBUFFER_WIDTH = 32

			LDX	#PIXELBUFFER_WIDTH * PIXELBUFFER_HEIGHT
			REPEAT
				STA	VMDATA
				INC

				DEX
			UNTIL_ZERO

			.if PIXELBUFFER_HEIGHT < 32
				LDX	#(32 - PIXELBUFFER_HEIGHT) * 32
				REPEAT
					STZ	VMDATA

					DEX
				UNTIL_ZERO

			.elseif PIXELBUFFER_HEIGHT > 32 .and PIXELBUFFER_HEIGHT < 64
				LDX	#(64 - PIXELBUFFER_HEIGHT) * 32
				REPEAT
					STZ	VMDATA

					DEX
				UNTIL_ZERO
			.endif

		.elseif PIXELBUFFER_WIDTH < 32

			LDY	#PIXELBUFFER_HEIGHT
			REPEAT
				LDX	#PIXELBUFFER_WIDTH
				REPEAT
					STA	VMDATA
					INC

					DEX
				UNTIL_ZERO

				LDX	#32 - PIXELBUFFER_WIDTH
				REPEAT
					STZ	VMDATA

					DEX
				UNTIL_ZERO

				DEY
			UNTIL_ZERO

			.if PIXELBUFFER_HEIGHT < 32
				LDX	#(32 - PIXELBUFFER_HEIGHT) * 32
				REPEAT
					STZ	VMDATA

					DEX
				UNTIL_ZERO

			.elseif PIXELBUFFER_HEIGHT > 32 .and PIXELBUFFER_HEIGHT < 64
				LDX	#(64 - PIXELBUFFER_HEIGHT) * 64
				REPEAT
					STZ	VMDATA

					DEX
				UNTIL_ZERO
			.endif

		.else
			; PIXELBUFFER_WIDTH > 32
			; PIXELBUFFER_HEIGHT <= 32
			.assert PIXELBUFFER_HEIGHT <= 32, error, "Bad values"

			; Part A
			PHA

			LDY	#PIXELBUFFER_HEIGHT
			REPEAT
				LDX	#32
				REPEAT
					STA	VMDATA
					INC

					DEX
				UNTIL_ZERO

				ADD	#PIXELBUFFER_WIDTH - 32

				DEY
			UNTIL_ZERO

			.if PIXELBUFFER_HEIGHT < 32
				LDX	#(32 - PIXELBUFFER_HEIGHT) * 32
				REPEAT
					STZ	VMDATA

					DEX
				UNTIL_ZERO
			.endif

			; Part B
			PLA

			LDY	#PIXELBUFFER_HEIGHT
			REPEAT
				ADD	#32

				LDX	#PIXELBUFFER_WIDTH - 32
				REPEAT
					STA	VMDATA
					INC

					DEX
				UNTIL_ZERO

				DEY
			UNTIL_ZERO

			.if PIXELBUFFER_HEIGHT < 32
				LDX	#(32 - PIXELBUFFER_HEIGHT) * 32
				REPEAT
					STZ	VMDATA

					DEX
				UNTIL_ZERO
			.endif
		.endif

		RTS
.endif



.A16
.I16
ROUTINE FillBuffer
	PHB

	LDA	f:colorBits
	STA	f:buffer

	LDX	#.loword(buffer)
	LDY	#.loword(buffer) + 2
	LDA	#buffer__size - 3

	MVN	#.bankbyte(buffer), #.bankbyte(buffer)

; MVN changes DB to .bankbyte(buffer)

	PLB
	RTS



; IN: X = xpos, Y = ypos
; OUT: A = color
.A16
.I16
ROUTINE GetPixel
	; colorMask, bPos = _CalculateColorMaskAndBufferY(xPos, yPos)
	; color = buffer[bPos] & colorMask)
	;
	; if color & $FF00:
	;	if color & $00FF:
	;		return 3
	;	else
	;		return 2
	; else
	;	if color & $00FF:
	;		return 1
	;	else
	;		return 0

	JSR	_CalculateColorMaskAndBufferY

	LDA	a:buffer, Y
	AND	a:colorMask

	IF_BIT	#$FF00
		IF_BIT	#$00FF
			LDA	#3
			RTS
		ENDIF

		LDA	#2
		RTS
	ENDIF

	IF_BIT	#$00FF
		LDA	#1
		RTS
	ENDIF

	LDA	#0
	RTS



; IN: A = color
.A16
ROUTINE SetColor
	AND	#3
	ASL
	TAX

	LDA	f:ColorBitsTable, X
	STA	f:colorBits

	RTS



; IN, X = xpos, Y = ypos
.A16
.I16
ROUTINE SetPixel
	; colorMask, bPos = _CalculateColorMaskAndBufferY(xPos, yPos)
	; buffer[bPos] = buffer[bPos] ^ ((buffer[bPos] ^ colorBits) & colorMask)

	JSR	_CalculateColorMaskAndBufferY

	LDA	a:buffer, Y
	EOR	a:colorBits
	AND	a:colorMask
	EOR	a:buffer, Y
	STA	a:buffer, Y

	RTS


; IN, X = pos, Y = ypos, A = height (must not be 0)
.A16
.I16
ROUTINE DrawVerticalLine
	; colorMask, bPos = _CalculateColorMaskAndBufferY(xPos, yPos)
	;
	; repeat
	; 	buffer[bPos] = buffer[bPos] ^ ((buffer[bPos] ^ colorBits) & colorMask)
	;	height--
	;	if height == 0:
	;		break
	;	else
	;		bPos += 2
	;		if bPos & (TILE_SIZE - 1) == 0
	;			bPos += PIXELBUFFER_WIDTH * TILE_SIZE - TILE_SIZE

	PHA

	JSR	_CalculateColorMaskAndBufferY

	PLX
	CLC
	REPEAT
		LDA	a:buffer, Y
		EOR	a:colorBits
		AND	a:colorMask
		EOR	a:buffer, Y
		STA	a:buffer, Y

		DEX
		BEQ	BREAK_LABEL

		; c always clear if Y does not overflow
		TYA
		ADC	#2
		IF_NOT_BIT	#TILE_SIZE - 1
			ADC	#PIXELBUFFER_WIDTH * TILE_SIZE - TILE_SIZE
		ENDIF

		TAY
	FOREVER

	RTS


; IN, X = pos, Y = ypos, A = width (must not be 0)
; NOTE: tmp1-tmp3
.A16
.I16
ROUTINE DrawHorizontalLine
	; xTilePos = xPos & 7
	; bPos = _CalculateBufferIndexY(xPos, yPos)
	;
	; if xPosTile + width >= 8:
	; 	colorMask = HorizontalDrawLine_LeftColorMaskTable[xPosTile]
	;	buffer[bPos] = buffer[bPos] ^ ((buffer[bPos] ^ colorBits) & colorMask)
	;
	;	width -= (8 - xTilePos)
	;	tilesTodo = width / 8
	;
	;	while (tilesTodo > 0):
	;		bPos += TILE_SIZE
	;
	; 		buffer[bPos] = colorBits
	;
	;		tilesToDo--
	;
	;	if ((width & 7) > 0):
	;		bPos += TILE_SIZE
	;		colorMask = HorizontalDrawLine_RightColorMaskTable[width & 7]
	; 		buffer[bPos] = buffer[bPos] ^ ((buffer[bPos] ^ colorBits) & colorMask)
	;
	; else:
	; 	colorMask = HorizontalDrawLine_LeftColorMaskTable[xTilePos + width] ^ HorizontalDrawLine_LeftColorMaskTable[xPosTile]
	;	buffer[bPos] = buffer[bPos] ^ ((buffer[bPos] ^ colorBits) & colorMask)

tmp_width		= tmp1
tmp_xTilePos		= tmp2
tmp_xTilePosIndex	= tmp3

	STA	tmp_width

	TXA
	AND	#7
	STA	tmp_xTilePos
	ASL
	STA	tmp_xTilePosIndex

	JSR	_CalculateBufferIndexY

	LDA	tmp_xTilePos
	ADD	tmp_width
	CMP	#8
	IF_GE
		LDX	tmp_xTilePosIndex
		LDA	f:HorizontalDrawLine_LeftColorMaskTable, X
		STA	colorMask

		LDA	a:buffer, Y
		EOR	a:colorBits
		AND	a:colorMask
		EOR	a:buffer, Y
		STA	a:buffer, Y

		LDA	tmp_width
		SUB	#8
		ADD	tmp_xTilePos
		STA	tmp_width

		LSR
		LSR
		LSR		; / 8
		DEC

		IF_PLUS
			TAX

			CLC
			REPEAT
				TYA
				ADC	#TILE_SIZE	; c clear so long as Y doesn't overflow
				TAY

				LDA	colorBits
				STA	a:buffer, Y

				DEX
			UNTIL_MINUS
		ENDIF

		LDA	tmp_width
		AND	#7
		IF_ZERO
			RTS
		ENDIF

		ASL
		TAX

		TYA
		ADC	#TILE_SIZE	; c clear so long as Y doesn't overflow
		TAY

		LDA	f:HorizontalDrawLine_RightColorMaskTable, X
	ELSE
		; A = tmp2_xTilePos + tmp_width
		ASL
		TAX

		LDA	f:HorizontalDrawLine_LeftColorMaskTable, X
		LDX	tmp_xTilePosIndex
		EOR	f:HorizontalDrawLine_LeftColorMaskTable, X
	ENDIF

	STA	colorMask

	LDA	a:buffer, Y
	EOR	a:colorBits
	AND	a:colorMask
	EOR	a:buffer, Y
	STA	a:buffer, Y

	RTS


; IN: X/Y Position
; OUT: X - tile location within buffer
.A16
.I16
ROUTINE TileOffsetForPosition
	; X = ((((yPos & $01F8) >> 3) << PIXELBUFFER_WIDTH_LOG2) + (xPos & $01F8) / 8) * TILE_SIZE

	TXA
	AND	#$01F8
	STA	cm_tmp

	TYA
	AND	#$01F8
	.repeat	PIXELBUFFER_WIDTH_LOG2
		ASL
	.endrepeat

	ADC	cm_tmp	; carry clear
	ASL

	TAX
	RTS


;; Calculates the buffer offset and color mask for the given X/Y position
;; INPUT: X, Y
;; OUTPUT:
;;	colorMask - colorMask
;;	X - buffer Index
.A16
.I16
ROUTINE _CalculateColorMaskAndBufferY
	; colorMask = $8080 >> (xPos & 7)
	; Index = ((((yPos & $01F8) >> 3) << PIXELBUFFER_WIDTH_LOG2) + (xPos & $01F8) / 8) * TILE_SIZE + (yPos & 7) * 2

	TXA
	AND	#$01F8
	STA	cm_tmp

	TYA
	AND	#7
	STA	cm_tmp2

	TXA
	AND	#7
	ASL
	TAX
	LDA	f:ColorMaskTable, X
	STA	colorMask

	TYA
	AND	#$01F8
	.assert TILE_SIZE = 16, error, "Bad Value"
	.repeat	PIXELBUFFER_WIDTH_LOG2
		ASL
	.endrepeat

	ADC	cm_tmp	; carry clear
	ADC	cm_tmp2	; carry clear
	ASL

	TAY
	RTS


;; Calculates the buffer offset (but not the color mask) for the given X/Y position
;; INPUT: X, Y
;; OUTPUT:
;;	X - buffer Index
.A16
.I16
ROUTINE _CalculateBufferIndexY
	; Index = ((((yPos & $01F8) >> 3) << PIXELBUFFER_WIDTH_LOG2) + (xPos & $01F8) / 8) * TILE_SIZE + (yPos & 7) * 2

	TXA
	AND	#$01F8
	STA	cm_tmp

	TYA
	AND	#7
	STA	cm_tmp2

	TYA
	AND	#$01F8
	.repeat	PIXELBUFFER_WIDTH_LOG2
		ASL
	.endrepeat

	ADC	cm_tmp	; carry clear
	ADC	cm_tmp2	; carry clear
	ASL

	TAY
	RTS


.segment "BANK1"

LABEL ColorBitsTable
	.word	$0000
	.word	$00FF
	.word	$FF00
	.word	$FFFF


LABEL ColorMaskTable
	.repeat 8, i
		.word	$8080 >> i
	.endrepeat

LABEL HorizontalDrawLine_LeftColorMaskTable
	_w_ .set $FFFF
	.repeat 8, i
		.word	_w_
		_w_ .set _w_ ^ ($8080 >> i)
	.endrepeat

LABEL HorizontalDrawLine_RightColorMaskTable
	_w_ .set $FFFF
	.repeat 8, i
		.word	$FFFF ^ _w_
		_w_ .set _w_ ^ ($8080 >> i)
	.endrepeat

ENDMODULE

