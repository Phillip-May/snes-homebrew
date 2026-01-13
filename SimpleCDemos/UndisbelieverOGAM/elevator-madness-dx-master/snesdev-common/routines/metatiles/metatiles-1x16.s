
.include "metatiles-1x16.h"
.include "includes/registers.inc"
.include "includes/structure.inc"
.include "includes/synthetic.inc"
.include "routines/block.h"
.include "routines/screen.h"
.include "routines/text.h"
.include "routines/math.h"

.import METATILES_BG1_MAP

MODULE MetaTiles1x16

METATILE_SIZE = 16
METATILE_TILES = 2

METATILE_DISPLAY_WIDTH = 16
METATILE_DISPLAY_HEIGHT = 14

;; Ensure MetaTiles1x16_VBlank is used
.forceimport _MetaTiles1x16_VBlank__Called:zp


.segment "SHADOW"
	UINT16	mapWidth
	UINT16	mapHeight

	UINT16	xPos
	UINT16	yPos

	UINT16	maxXPos
	UINT16	maxYPos

	UINT16	displayXoffset
	UINT16	displayYoffset

	WORD	sizeOfMapRow

	;; Buffer state.
	BYTE	updateBgBuffer

	BYTE	mapDirty

	;; VRAM word address to store vertical buffer.
	ADDR	bgVerticalBufferVramLocation
	;; VRAM word address to store the horizontal buffer (left tilemap)
	ADDR	bgHorizontalBufferVramLocation1
	;; VRAM word address to store the horizontal buffer (right tilemap)
	;; Equal to bgHorizontalBufferVramLocation1 + 32 * 32.
	ADDR	bgHorizontalBufferVramLocation2


.segment "WRAM7E"
	STRUCT	metaTiles, MetaTile16Struct
	ADDR	map, METATILES_MAP_TILE_ALLOCATION
	ADDR	mapRowAddressTable, METATILES_MAX_ROWS

	;; The buffer to use when updating the whole display
	WORD	bgBuffer, 32 * 32

	;; The left 8x8 tiles of the vertical tile update
	WORD	bgVerticalBufferLeft, 32
	;; The right 8x8 tiles of the vertical tile update
	WORD	bgVerticalBufferRight, 32

	;; The horizontal tiles to update during VRAM
	;; Uses the same space as bgBuffer because both cannot be used
	;; at the same time.
	SAME_VARIABLE	bgHorizontalBuffer, bgBuffer

	;; Pixel location on the map that represents tile 0,0 of the SNES tilemap
	UINT16	displayScreenDeltaX
	UINT16	displayScreenDeltaY

	;; Number of bytes in a single Map Row * METATILE_DISPLAY_HEIGHT
	sizeOfMapRowDisplayHeight = mapRowAddressTable + METATILE_DISPLAY_HEIGHT * 2

	;; Tile index within `map` that represents the top left of the visible display.
	WORD	visibleTopLeftMapIndex

	;; Pixel location of `visibleTopLeftMapIndex`
	UINT16	visibleTopLeftMapXpos
	UINT16	visibleTopLeftMapYpos

	;; The ending position of the draw loop.
	UINT16	endOfLoop

	;; The topmost tile within `bgVerticalBufferLeft` that is updated within `_ProcessVerticalBuffer`
	;; Changes with y movement.
	WORD	columnBufferIndex
	;; The leftmost tile within `bgHorizontalBuffer` that is updated within `_ProcessHorizontalBuffer`
	;; Changes with x movement.
	WORD	rowBufferIndex

	;; Tile offset for the vertical update MetaTile address.
	;; Number of 16x16 tiles in the vertical offset.
	;; Bitwise math is used to convert this value into `bgVerticalBufferVramLocation`.
	WORD	columnVramMetaTileOffset

	;; VRAM word address offset for the horizontal update.
	;; Number of 8x8 tiles in the horizontal offset.
	;; Unlike the column no fancy bitwise math is needed.
	WORD	rowVramMetaTileOffset

.code

.A8
.I16
ROUTINE MapInit
	; sizeOfMapRow = (mapWidth + METATILE_SIZE) / METATILE_SIZE * 2
	; maxXPos = mapWidth - 256
	; maxYPos = mapHeight - 224
	;
	; a = 0
	; for y = mapHeight / 16 to 0
	;	mapRowAddressTable[x] = a
	;	a += sizeOfMapRow
	;	x++
	;
	; DrawEntireScreen()

	REP	#$31			; also clear carry
.A16

	.assert METATILE_SIZE = 16, error, "METATILE_SIZE"
	LDA	mapWidth
	ADC	#METATILE_SIZE - 1	; carry clear from REP
	LSR
	LSR
	LSR
	AND	#$FFFE
	STA	sizeOfMapRow

	LDA	mapWidth
	SUB	#256
	STA	maxXPos

	LDA	mapHeight
	SUB	#224
	STA	maxYPos

	LDA	mapHeight
	LSR
	LSR
	LSR
	LSR
	TAY

	LDA	#0
	LDX	#0
	REPEAT
		STA	f:mapRowAddressTable, X
		ADD	sizeOfMapRow
		INX
		INX
		DEY
	UNTIL_ZERO

	SEP	#$20

	.assert * = DrawEntireScreen, lderror, "Bad Flow"



.A8
.I16
ROUTINE DrawEntireScreen
	PHB
	LDA	#$7E
	PHA
	PLB

	REP	#$30
.A16
	.assert * = _DrawEntireScreen_Bank7E, lderror, "Bad Flow"


;; REQUIRES: DB = $7E, DB on stack, 16 bit A, 16 bit Index
.A16
.I16
ROUTINE _DrawEntireScreen_Bank7E
	; // Building from bottom-right to top-left because it saves a comparison.
	;
	; visibleTopLeftMapIndex = (xPos / METATILE_SIZE * 2) + mapRowAddressTable[yPos / METATILE_SIZE]
	; x = visibleTopLeftMapIndex + sizeOfMapRowDisplayHeight + METATILE_DISPLAY_WIDTH * 2 - 2
	; y = (METATILE_DISPLAY_HEIGHT + 1) * METATILE_TILES * 32 * 2 - 2
	; mapColumnIndex = (xPos / 16)
	;
	; repeat
	;	endOfLoop = y - 64
	;	repeat
	;		bgBuffer[y] = metaTiles.bottomRight[map[x]]
	;		bgBuffer[y - 2] = metaTiles.bottomLeft[map[x]]
	;		bgBuffer[y - 64] = metaTiles.topRight[map[x]]
	;		bgBuffer[y - 64 - 2] = metaTiles.topLeft[map[x]]
	;		x -= 2
	;		y -= 4
	;	until y == endOfLoop
	;	x = x - sizeOfMapRow - METATILE_DISPLAY_WIDTH * 2
	;	y -= 64
	; until y < 0
	;
	; set data bank to $7E
	;
	; displayScreenDeltaX = xPos & ~(METATILE_SIZE - 1)
	; displayScreenDeltaY = yPos & ~(METATILE_SIZE - 1)
	;
	; visibleTopLeftMapXpos = xPos & ~(METATILE_SIZE - 1)
	; visibleTopLeftMapYpos = yPos & ~(METATILE_SIZE - 1)
	;
	; displayXoffset = xPos & (METATILE_SIZE - 1)
	; displayYoffset = yPos & (METATILE_SIZE - 1) - 1
	;
	; columnBufferIndex = 0
	; rowBufferIndex = 0
	; columnVramMetaTileOffset = 0
	; rowVramMetaTileOffset = 0
	;
	; _ProcessVerticalBuffer(visibleTopLeftMapIndex + (METATILE_DISPLAY_WIDTH + 1) * 2)
	; bgVerticalBufferVramLocation = METATILES_BG1_MAP + 32 * 32
	;
	; updateBgBuffer = METATILES_UPDATE_WHOLE_BUFFER
	; mapDirty = False

	.assert METATILE_SIZE = 16, error, "METATILE_SIZE"
	LDA	yPos
	LSR
	LSR
	LSR
	AND	#$FFFE		; / 16 * 2
	TAX

	LDA	xPos
	LSR
	LSR
	LSR
	AND	#$FFFE		; / 16 * 2
	ADD	mapRowAddressTable, X
	STA	visibleTopLeftMapIndex

	ADD	a:sizeOfMapRowDisplayHeight
	ADD	#METATILE_DISPLAY_WIDTH * 2 - 2
	TAX

	; Building from the bottom-right to top-left because it saves a comparison.
	LDA	#(METATILE_DISPLAY_HEIGHT + 1) * METATILE_TILES * 32 * 2 - 2
	REPEAT
		TAY
		SUB	#64
		STA	a:endOfLoop

		REPEAT
			PHX

			LDA	a:map, X
			TAX

			LDA	a:metaTiles + MetaTile16Struct::bottomRight, X
			STA	a:bgBuffer, Y

			LDA	a:metaTiles + MetaTile16Struct::bottomLeft, X
			STA	a:bgBuffer - 2, Y

			LDA	a:metaTiles + MetaTile16Struct::topRight, X
			STA	a:bgBuffer - 64, Y

			LDA	a:metaTiles + MetaTile16Struct::topLeft, X
			STA	a:bgBuffer - 64 - 2, Y

			PLX
			DEX
			DEX

			DEY
			DEY
			DEY
			DEY

			CPY	a:endOfLoop
		UNTIL_EQ

		TXA
		SUB	a:sizeOfMapRow
		ADD	#METATILE_DISPLAY_WIDTH * 2
		TAX

		TYA
		SUB	#64
	UNTIL_MINUS

	LDA	xPos
	AND	#$FFFF - (METATILE_SIZE - 1)
	STA	displayScreenDeltaX
	STA	visibleTopLeftMapXpos

	LDA	yPos
	AND	#$FFFF - (METATILE_SIZE - 1)
	STA	displayScreenDeltaY
	STA	visibleTopLeftMapYpos

	LDA	xPos
	AND	#(METATILE_SIZE - 1)
	STA	displayXoffset

	LDA	yPos
	AND	#(METATILE_SIZE - 1)
	DEC
	STA	displayYoffset

	STZ	columnBufferIndex
	STZ	rowBufferIndex

	STZ	columnVramMetaTileOffset
	STZ	rowVramMetaTileOffset

	; Process right column

	LDA	visibleTopLeftMapIndex
	ADD	#(METATILE_DISPLAY_WIDTH + 1) * 2
	JSR	_ProcessVerticalBuffer

	LDA	#METATILES_BG1_MAP + 32 * 32
	STA	bgVerticalBufferVramLocation

	SEP	#$20
.A8
	LDA	#METATILE16_UPDATE_WHOLE_BUFFER
	STA	updateBgBuffer

	STZ	mapDirty

	PLB
	RTS



.A8
.I16
ROUTINE Update
	; if mapDirty
	;	DrawEntireScreen()
	;	return
	;
	; if xPos - visibleTopLeftMapXpos > 0
	;	if xPos - visibleTopLeftMapXpos > METATILE_SIZE
	;		if xPos - visibleTopLeftMapXpos > METATILE_SIZE * 2
	;			DrawEntireScreen()
	;			return
	; 		visibleTopLeftMapXpos += METATILE_SIZE
	;		visibleTopLeftMapIndex += 2
	;		_ProcessVerticalBuffer(visibleTopLeftMapIndex + (METATILE_DISPLAY_WIDTH + 1) * 2)
	;
	;		rowBufferIndex += METATILE_TILES * 2
	;		columnVramMetaTileOffset++
	;		a = (columnVramMetaTileOffset + 32) & ($003F / METATILE_TILES)
	;		if a & ($0020 / METATILE_TILES)
	;			a ^= ($0420 / METATILE_TILES) 	// (The equivalent of a = a | $0400 & ~$0020)
	;		bgVerticalBufferVramLocation = METATILES_BG1_MAP + a * METATILE_TILES
	;
	;		updateBgBuffer |= METATILE16_UPDATE_VERTICAL_BUFFER
	; else
	;	if xPos - visibleTopLeftMapXpos < -METATILE_SIZE
	;		DrawEntireScreen()
	;		return
	;
	;	visibleTopLeftMapXpos -= METATILE_SIZE
	;	visibleTopLeftMapIndex -= 2
	;	_ProcessVerticalBuffer(visibleTopLeftMapIndex + 2)
	;
	;	rowBufferIndex -= METATILE_TILES * 2
	;	columnVramMetaTileOffset -= METATILE_TILES / 2
	;	a = columnVramMetaTileOffset & ($003F / METATILE_TILES)
	;	if a & ($0020 / METATILE_TILES)
	;		a ^= ($0420 / METATILE_TILES) 	// (The equivalent of a = a | $0400 & ~$0020)
	;	bgVerticalBufferVramLocation = METATILES_BG1_MAP + a * METATILE_TILES
	;
	;	updateBgBuffer |= METATILE16_UPDATE_VERTICAL_BUFFER
	;
	; displayXoffset = xPos - displayScreenDeltaX
	;
	;
	; if yPos - visibleTopLeftMapYpos > 0
	;	if yPos - visibleTopLeftMapYpos > METATILE_SIZE
	;		if yPos - visibleTopLeftMapYpos > METATILE_SIZE * 2
	;			DrawEntireScreen()
	;			return
	; 		visibleTopLeftMapYpos += METATILE_SIZE
	;		visibleTopLeftMapIndex += sizeOfMapRow
	;		_ProcessHorizontalBuffer(visibleTopLeftMapIndex + sizeOfMapRowDisplayHeight)
	;
	;		columnBufferIndex += METATILE_TILES * 2
	;		rowVramMetaTileOffset += 32 * METATILE_TILES
	;		a = (columnVramMetaTileOffset + 28 * 32) & $03FF
	;		bgHorizontalBufferVramLocation1 = a + METATILES_BG1_MAP
	;		bgHorizontalBufferVramLocation2 = a + METATILES_BG1_MAP + 32 * 32
	;
	;		updateBgBuffer |= METATILE16_UPDATE_HORIZONAL_BUFFER
	; else
	;	if yPos - visibleTopLeftMapYpos < -METATILE_SIZE
	;		DrawEntireScreen()
	;		return
	;	visibleTopLeftMapYpos -= METATILE_SIZE
	;	visibleTopLeftMapIndex -= sizeOfMapRow
	;	_ProcessHorizontalBuffer(visibleTopLeftMapIndex - sizeOfMapRow)
	;
	;	columnBufferIndex -= METATILE_TILES * 2
	;	rowVramMetaTileOffset -= 32 * METATILE_TILES
	;	a = (columnVramMetaTileOffset - 32 * 2) & $03FF
	;	bgHorizontalBufferVramLocation1 = a + METATILES_BG1_MAP
	;	bgHorizontalBufferVramLocation2 = a + METATILES_BG1_MAP + 32 * 32
	;
	;	updateBgBuffer |= METATILE16_UPDATE_HORIZONAL_BUFFER
	;
	; displayYoffset = yPos - displayScreenDeltaY - 1
	; updateBgBuffer |= METATILE16_UPDATE_POSITION

	LDA	mapDirty
	JNE	DrawEntireScreen

	PHB

	LDA	#$7E
	PHA
	PLB

	REP	#$30
.A16

	LDA	xPos
	SUB	visibleTopLeftMapXpos
	IF_GE
		CMP	#METATILE_SIZE
		IF_GE
			CMP	#METATILE_SIZE * 2
			JGE	_DrawEntireScreen_Bank7E

			; c clear from branch.
			LDA	visibleTopLeftMapXpos
			ADC	#METATILE_SIZE
			STA	visibleTopLeftMapXpos

			LDA	visibleTopLeftMapIndex
			INC
			INC
			STA	visibleTopLeftMapIndex
			ADD	#(METATILE_DISPLAY_WIDTH + 1) * 2
			JSR	_ProcessVerticalBuffer


			LDA	rowBufferIndex
			ADD	#METATILE_TILES * 2
			STA	rowBufferIndex

			LDA	columnVramMetaTileOffset
			INC
			AND	#$003F / METATILE_TILES
			STA	columnVramMetaTileOffset

			EOR	#$0020 / METATILE_TILES
			BIT	#$0020 / METATILE_TILES
			IF_NOT_ZERO
				EOR	#$0420 / METATILE_TILES
			ENDIF
			.assert METATILE_TILES = 2, error, "METATILE_TILES"
			ASL
			ADD	#METATILES_BG1_MAP
			STA	bgVerticalBufferVramLocation

			SEP	#$20
.A8
			LDA	#METATILE16_UPDATE_VERTICAL_BUFFER
			TSB	updateBgBuffer

			REP	#$20
		ENDIF
	ELSE
.A16
		; A = xPos - visibleTopLeftMapXpos
		CMP	#.loword(-METATILE_SIZE)
		JMI	_DrawEntireScreen_Bank7E

		LDA	visibleTopLeftMapXpos
		SUB	#METATILE_SIZE
		STA	visibleTopLeftMapXpos

		LDA	visibleTopLeftMapIndex
		DEC
		DEC
		STA	visibleTopLeftMapIndex
		INC
		INC
		JSR	_ProcessVerticalBuffer

		LDA	rowBufferIndex
		SUB	#METATILE_TILES * 2
		STA	rowBufferIndex

		LDA	columnVramMetaTileOffset
		DEC
		AND	#$003F / METATILE_TILES
		STA	columnVramMetaTileOffset

		BIT	#$0020 / METATILE_TILES
		IF_NOT_ZERO
			EOR	#$0420 / METATILE_TILES
		ENDIF
		.assert METATILE_TILES = 2, error, "METATILE_TILES"
		ASL
		ADD	#METATILES_BG1_MAP
		STA	bgVerticalBufferVramLocation

		SEP	#$20
.A8
		LDA	#METATILE16_UPDATE_VERTICAL_BUFFER
		TSB	updateBgBuffer

		REP	#$20
	ENDIF
.A16
	LDA	xPos
	SUB	displayScreenDeltaX
	STA	displayXoffset



	LDA	yPos
	SUB	visibleTopLeftMapYpos
	IF_GE
		CMP	#METATILE_SIZE
		IF_GE
			CMP	#METATILE_SIZE * 2
			JGE	_DrawEntireScreen_Bank7E

			; c clear from branch.
			LDA	visibleTopLeftMapYpos
			ADC	#METATILE_SIZE
			STA	visibleTopLeftMapYpos

			LDA	visibleTopLeftMapIndex
			ADD	sizeOfMapRow
			STA	visibleTopLeftMapIndex
			ADD	sizeOfMapRowDisplayHeight
			JSR	_ProcessHorizontalBuffer


			LDA	columnBufferIndex
			ADD	#METATILE_TILES * 2
			STA	columnBufferIndex

			LDA	rowVramMetaTileOffset
			ADD	#32 * METATILE_TILES
			STA	rowVramMetaTileOffset

			ADD	#28 * 32
			AND	#$03FF

			ADD	#METATILES_BG1_MAP
			STA	bgHorizontalBufferVramLocation1
			ADD	#32 * 32
			STA	bgHorizontalBufferVramLocation2

			SEP	#$20
.A8
			LDA	#METATILE16_UPDATE_HORIZONAL_BUFFER
			TSB	updateBgBuffer

			REP	#$30
.A16
		ENDIF
	ELSE
.A16
		; A = yPos - visibleTopLeftMapXpos
		CMP	#.loword(-METATILE_SIZE)
		JMI	_DrawEntireScreen_Bank7E

		; c clear from branch.
		LDA	visibleTopLeftMapYpos
		SUB	#METATILE_SIZE
		STA	visibleTopLeftMapYpos

		LDA	visibleTopLeftMapIndex
		SUB	sizeOfMapRow
		STA	visibleTopLeftMapIndex
		JSR	_ProcessHorizontalBuffer


		LDA	columnBufferIndex
		SUB	#METATILE_TILES * 2
		STA	columnBufferIndex

		LDA	rowVramMetaTileOffset
		SUB	#32 * METATILE_TILES
		STA	rowVramMetaTileOffset

		AND	#$03FF
		ADD	#METATILES_BG1_MAP
		STA	bgHorizontalBufferVramLocation1
		ADD	#32 * 32
		STA	bgHorizontalBufferVramLocation2

		SEP	#$20
.A8
		LDA	#METATILE16_UPDATE_HORIZONAL_BUFFER
		TSB	updateBgBuffer

		REP	#$20
	ENDIF
.A16

	LDA	yPos
	CLC
	SBC	displayScreenDeltaY
	STA	displayYoffset

	SEP	#$20
.A8
	LDA	#METATILE16_UPDATE_POSITION
	TSB	updateBgBuffer

	PLB
	RTS



;; Builds bgVerticalBufferLeft and bgVerticalBufferRight depending on the tile selected.
;; You will need to set `bgVerticalBufferVramLocation` and `updateBgBuffer` afterwards
;; REQUIRES: 16 bit A, 16 bit Index, DB = $7E
;; INPUT: A = tile index of the topmost displayed tile.
.A16
.I16
ROUTINE _ProcessVerticalBuffer
	; endOfLoop = tileIndex - sizeOfMapRow * 2
	; x = tileIndex + sizeOfMapRowDisplayHeight - 2
	; y = (columnBufferIndex + 30 * 2 - 2) MOD 64
	;
	; repeat
	;	bgVerticalBufferRight[y] = metaTiles.bottomRight[map[x]]
	;	bgVerticalBufferLeft[y] = metaTiles.bottomLeft[map[x]]
	;	bgVerticalBufferRight[y - 2] = metaTiles.topRight[map[x]]
	;	bgVerticalBufferLeft[y - 2] = metaTiles.topLeft[map[x]]
	;	x -= sizeOfMapRow
	;	y -= 4
	;	if y < 0
	;		y = 32 * 2 - 2
	; until x < endOfLoop

	TAY
	SUB	sizeOfMapRow
	SUB	sizeOfMapRow		; ::HACK::
	STA	endOfLoop
	TYA

	ADD	sizeOfMapRowDisplayHeight
	DEC
	DEC
	TAX

	LDA	columnBufferIndex
	ADD	#30 * 2 - 2
	AND	#$3F
	TAY

	REPEAT
		PHX

		LDA	map, X
		TAX

		LDA	a:metaTiles + MetaTile16Struct::bottomRight, X
		STA	a:bgVerticalBufferRight, Y

		LDA	a:metaTiles + MetaTile16Struct::bottomLeft, X
		STA	a:bgVerticalBufferLeft, Y

		LDA	a:metaTiles + MetaTile16Struct::topRight, X
		STA	a:bgVerticalBufferRight - 2, Y

		LDA	a:metaTiles + MetaTile16Struct::topLeft, X
		STA	a:bgVerticalBufferLeft - 2, Y

		PLA
		SUB	sizeOfMapRow
		TAX

		DEY
		DEY
		DEY
		DEY
		IF_MINUS
			LDY	#32 * 2 - 2
		ENDIF

		CPX	endOfLoop
	UNTIL_MINUS

	RTS



;; Builds bgHorizontalBuffer depending on the tile selected.
;; You will need to set `bgHorizontalBufferVramLocation` and `updateBgBuffer` afterwards
;; REQUIRES: 16 bit A, 16 bit Index, DB = $7E
;; INPUT: A = tile index of the leftmost displayed tile.
.A16
.I16
ROUTINE _ProcessHorizontalBuffer
	; endOfLoop = tileIndex
	; x = tileIndex + (METATILE_DISPLAY_WIDTH + 1) * 2
	; y = (rowBufferIndex + (32 + METATILE_TILES) * 2 - 2) MOD 128
	;
	; repeat
	;	x -= 2
	;	bgHorizontalBuffer[y] = metaTiles.topRight[map[x]]
	;	bgHorizontalBuffer[y - 2] = metaTiles.topLeft[map[x]]
	;	bgHorizontalBuffer[y + 64 * 2] = metaTiles.bottomRight[map[x]]
	;	bgHorizontalBuffer[y + 64 * 2 - 2] = metaTiles.bottomLeft[map[x]]
	;	y -= 4
	;	if y < 0
	;		y = 64 * 2 - 2
	; until x < endOfLoop

	STA	endOfLoop
	ADD	#(METATILE_DISPLAY_WIDTH + 1) * 2
	TAX

	LDA	rowBufferIndex
	ADD	#(32 + METATILE_TILES) * 2 - 2
	AND	#$7F
	TAY

	REPEAT
		DEX
		DEX
		PHX

		LDA	map, X
		TAX

		LDA	a:metaTiles + MetaTile16Struct::topRight, X
		STA	a:bgHorizontalBuffer, Y

		LDA	a:metaTiles + MetaTile16Struct::topLeft, X
		STA	a:bgHorizontalBuffer - 2, Y

		LDA	a:metaTiles + MetaTile16Struct::bottomRight, X
		STA	a:bgHorizontalBuffer + 64 * 2, Y

		LDA	a:metaTiles + MetaTile16Struct::bottomLeft, X
		STA	a:bgHorizontalBuffer + 64 * 2 - 2, Y

		PLX

		DEY
		DEY
		DEY
		DEY
		IF_MINUS
			LDY	#64 * 2 - 2
		ENDIF

		CPX	endOfLoop
	UNTIL_MINUS

	RTS



; IN: X = xpos, Y = ypos
; OUT: A = tilemap index
; DB = $7E
.A16
.I16
ROUTINE LocationToTilePos
	; return mapRowAddressTable[Y / 16 * 2] + X / 16 * 2

	TYA
	LSR
	LSR
	LSR
	AND	#$FFFE	; / 16 * 2
	TAY

	TXA
	LSR
	LSR
	LSR
	AND	#$FFFE	; / 16 * 2
	CLC
	ADC	mapRowAddressTable, Y
	RTS


ENDMODULE

