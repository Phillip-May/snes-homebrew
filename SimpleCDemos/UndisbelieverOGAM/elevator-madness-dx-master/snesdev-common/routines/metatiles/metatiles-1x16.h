;;
;; 1 layer of 16x16 MetaTiles
;; ==========================
;;
;; This module manages a single layer of 16x16 pixel MetaTile map
;; with scroll support.
;;
;; In order to simplify the system, both the map and the MetaTileSet are
;; stored in RAM. This allows the data to be compressed and loaded by the
;; loading module to save ROM space.
;;
;; The map data is stored as the offsets within the MetaTile table
;; (Which is a structure of arrays, that is `tile * 2`).
;; This is done so that other modules can create a clean
;; map to data mapping without worrying about alignment.
;;
;; The module is configured by setting the `METATILES_BG1_MAP`
;; global that indicates the *word* address of the SNES tilemap
;; within VRAM.
;;
;; This module also uses DMA channels 0 and 1 during the VBlank routine.

.ifndef ::_METATILES_1x16_H_
::_METATILES_1x16_H_ = 1

.include "includes/import_export.inc"
.include "includes/structure.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/config.inc"

.setcpu "65816"

;; Maximum number of tiles in map.
; Enough to fit a single Illusion of Gaia map.
; Uses 12.5 KiB of space.
CONFIG METATILES_MAP_TILE_ALLOCATION, 80 * 80

;; Maximum number of rows in a map.
CONFIG METATILES_MAX_ROWS, 128

;; Number of MetaTiles per map
CONFIG_DEFINE METATILES_N_METATILES, 512

;; Structure of arrays that contains the SNES BG tile data for each MetaTile
.struct MetaTile16Struct
	topLeft		.addr METATILES_N_METATILES
	topRight	.addr METATILES_N_METATILES
	bottomLeft	.addr METATILES_N_METATILES
	bottomRight	.addr METATILES_N_METATILES
.endstruct

METATILE16_DONT_UPDATE_BUFFER		= $00
METATILE16_UPDATE_HORIZONAL_BUFFER	= $01
METATILE16_UPDATE_POSITION		= $02
METATILE16_UPDATE_VERTICAL_BUFFER	= $80
METATILE16_UPDATE_WHOLE_BUFFER		= $FF


IMPORT_MODULE MetaTiles1x16
	;; x Position of the screen
	UINT16	xPos
	;; y Position of the screen
	UINT16	yPos

	;; Maximum value of xPos before out of bounds
	UINT16	maxXPos
	;; Maximum value of yPos before out of bounds
	UINT16	maxYPos

	;; Width of the map in pixels
	UINT16	mapWidth

	;; Height of the map in pixels
	;; Must be less than (METATILES_MAX_ROWS * 16)
	UINT16	mapHeight

	;; Number of bytes in a single map row.
	WORD	sizeOfMapRow

	;; IF non-zero then the map is declared dirty and a full redraw is preformed.
	BYTE	mapDirty

	;; MetaTile table, mapping of MetaTiles to their inner tiles.
	;; Structure of Arrays.
	;;
	;; ACCESS: WRAM7E
	STRUCT	metaTiles, MetaTile16Struct

	;; The map data.
	;; Stored as a multidimensional array[y][x], with a width of
	;; `mapWidth / 16` and a height of `mapHeight / 16`.
	;;
	;; Each cell contains the word address within the `metaTiles` table
	;; (`tile * 2`), for speed purposes.
	;;
	;; ACCESS: WRAM7E
	ADDR	map, METATILES_MAP_TILE_ALLOCATION

	;; The table containing the starting tile address of each row in the map.
	;; Contains METATILES_MAX_ROWS .addr fields.
	;;
	;; ACCESS: WRAM7E
	ADDR	mapRowAddressTable, METATILES_MAX_ROWS


	;; Initialize the MetaTile system. 
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; There is no bounds checking. xPos and yPos MUST be >= 0 and less
	;; than (mapWidth - 256) and (mapHeight - 224) respectively.
	;;
	;; INPUT:
	;;	xPos - screen position
	;;	yPos - screen position
	;;	mapWidth - the width of the map in pixels
	;;	mapHeight - the height of the map in pixels (MUST BE less than (METATILES_MAX_ROWS * 16))
	;;	metaTiles - the MetaTile data to use (loaded into memory)
	;;	map	- the map data to use (loaded into memory)
	ROUTINE	MapInit

	;; Updates the position of the screen, loading new tiles as necessary.
	;;
	;; There are 3 types of tile updates.
	;;	* Horizontal
	;;	* Vertical
	;;	* Whole Screen
	;;
	;; And will fill the various buffers as necessary.
	;;
	;; So long as the screen only moves < 16 pixels per update only
	;; horizontal and vertical updates will be preformed.
	;;
	;; There is no bounds checking, xPos and yPos MUST BE positive
	;; and less than maxXPos (mapWidth - 256) and maxYPos (mapHeight - 224)
	;; respectively.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; INPUT:
	;;	xPos - screen position
	;;	yPos - screen position
	ROUTINE	Update

	;; Converts xPos/yPos coordinates into a tile index.
	;;
	;; REQUIRES: 16 bit A, 16 bit Index , DB = $7E
	;; PARAM:
	;;	X - the xPos variable address
	;;	Y - the yPos variable address
	;; OUTPUT: A - The index of the tile within `map`
	ROUTINE LocationToTilePos


	;; Loads the buffers into VRAM
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, DB=0, DMA channels 0 and 1 free.
	.macro MetaTiles1x16_VBlank
		.export _MetaTiles1x16_VBlank__Called = 1
		.import METATILES_BG1_MAP

		.global MetaTiles1x16__displayXoffset
		.global MetaTiles1x16__displayYoffset
		.global MetaTiles1x16__bgBuffer
		.global MetaTiles1x16__bgVerticalBufferLeft
		.global MetaTiles1x16__bgVerticalBufferRight
		.global MetaTiles1x16__bgVerticalBufferVramLocation
		.global MetaTiles1x16__bgHorizontalBuffer
		.global MetaTiles1x16__bgHorizontalBufferVramLocation1
		.global MetaTiles1x16__bgHorizontalBufferVramLocation2
		.global MetaTiles1x16__updateBgBuffer

		LDA	MetaTiles1x16__updateBgBuffer
		IFL_NOT_ZERO
			IF_MINUS
				; Update Vertical Buffer
				LDA	#VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_32
				STA	VMAIN

				LDX	#DMAP_DIRECTION_TO_PPU | DMAP_TRANSFER_2REGS | (.lobyte(VMDATA) << 8)
				STX	DMAP0			; also sets BBAD0

				LDX	#.loword(MetaTiles1x16__bgVerticalBufferLeft)
				STX	A1T0
				LDA	#.bankbyte(MetaTiles1x16__bgVerticalBufferLeft)
				STA	A1B0

				LDX	MetaTiles1x16__bgVerticalBufferVramLocation
				LDY	#32 * 2
				LDA	#MDMAEN_DMA0

				STX	VMADD
				STY	DAS0
				STA	MDMAEN

				.assert MetaTiles1x16__bgVerticalBufferLeft + 32 * 2 = MetaTiles1x16__bgVerticalBufferRight, error, "MetaTiles1x16__bgVerticalBufferRight must be after MetaTiles1x16__bgVerticalBufferLeft"

				INX
				STX	VMADD
				STY	DAS0
				STA	MDMAEN

				LDA	MetaTiles1x16__updateBgBuffer
			ENDIF

			CMP	#$FF
			IF_EQ
				; Update Whole Buffer
				; As the vertical buffer is already updated, DMA registers are already set.

				LDA	#VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_1
				STA	VMAIN

				LDX	#.loword(MetaTiles1x16__bgBuffer)
				STX	A1T0

				LDX	#METATILES_BG1_MAP
				STX	VMADD

				LDY	#30 * 32 * 2
				STY	DAS0

				LDA	#MDMAEN_DMA0
				STA	MDMAEN
			ELSE
				LSR
				IF_C_SET
					; Update Horizontal Buffer
					; Need to use 2 DMA channels to handle the split tilemap.
					; Inspired by The Secret of Mana's DMA upload patterns.

					LDA	#VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_1
					STA	VMAIN

					LDX	#DMAP_DIRECTION_TO_PPU | DMAP_TRANSFER_2REGS | (.lobyte(VMDATA) << 8)
					STX	DMAP0			; also sets BBAD0
					STX	DMAP1			; also sets BBAD1

					LDX	#.loword(MetaTiles1x16__bgHorizontalBuffer)
					STX	A1T0
					LDX	#.loword(MetaTiles1x16__bgHorizontalBuffer + 64 * 2)
					STX	A1T1

					LDA	#.bankbyte(MetaTiles1x16__bgHorizontalBuffer)
					STA	A1B0
					STA	A1B1

					LDY	#2 * 32
					LDA	#MDMAEN_DMA0 | MDMAEN_DMA1

					LDX	MetaTiles1x16__bgHorizontalBufferVramLocation1
					STX	VMADD
					STY	DAS0
					STY	DAS1
					STA	MDMAEN

					LDX	MetaTiles1x16__bgHorizontalBufferVramLocation2
					STX	VMADD
					STY	DAS0
					STY	DAS1
					STA	MDMAEN
				ENDIF
			ENDIF

			LDA	MetaTiles1x16__displayXoffset
			STA	BG1HOFS
			LDA	MetaTiles1x16__displayXoffset + 1
			STA	BG1HOFS

			LDA	MetaTiles1x16__displayYoffset
			STA	BG1VOFS
			LDA	MetaTiles1x16__displayYoffset + 1
			STA	BG1VOFS

			STZ	MetaTiles1x16__updateBgBuffer
		ENDIF
	.endmacro

	;; Converts xPos/yPos coordinates into a tile index.
	;;
	;; REQUIRES: 16 bit A, 16 bit Index , DB access registers
	;; PARAM:
	;;	xPos - the xPos variable address
	;;	yPos - the yPos variable address
	;;	index - (optional) the index of the xPos/yPos, can only be Y
	;; MODIFIES: A, X
	;; OUTPUT: A - The index of the tile within `map`
	.macro MetaTiles1x16_LocationToTile xPos, yPos, index
		; return (xPos / 16 * 2) + mapRowAddressTable[yPos / 16]

		.ifblank index
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
			AND	#$FFFE
			CLC
			ADC	f:MetaTiles1x16__mapRowAddressTable, X
		.else
			.if .xmatch(index, Y)
				PHY
			.else
				.fatal .sprintf("only allow Y index")
			.endif
			LDA	yPos, Y
			LSR
			LSR
			LSR
			AND	#$FFFE
			TAX

			LDA	xPos, Y
			LSR
			LSR
			LSR
			AND	#$FFFE
			CLC
			ADC	f:MetaTiles1x16__mapRowAddressTable, X
		.endif
	.endmacro

ENDMODULE

.endif ; ::_METATILES_1x16_H_

; vim: ft=asm:

