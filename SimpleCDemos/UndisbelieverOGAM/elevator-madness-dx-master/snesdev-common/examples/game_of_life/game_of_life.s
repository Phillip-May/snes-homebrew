; a 30x28 grid of Conway's Game of Life

.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"

.include "routines/reset-snes.h"
.include "routines/random.h"
.include "routines/screen.h"
.include "routines/cpu-usage.h"
.include "routines/block.h"

; Ignore interrupts
.import EmptyHandler
.export IrqHandler	= EmptyHandler
.export CopHandler	= EmptyHandler

; Cell format
;
; a---nnnn
;
; a    = alive
; nnnn = number of neighbours
;
; This lets me:
;
;  1) Check if cell is dead and has no neighbours in 1 instruction
;  2) We can check if alive by testing the n Status flag
;  3) We can fit this all in shadow RAM

.define CELL_ALIVE	%10000000

.define	EOL		$0A

BG1_MAP			= $0400
BG1_TILES		= $1000
BG1_SIZE		= BGXSC_SIZE_32X32


.zeropage
	WORD	tmp
	WORD	tmp1
	WORD	tmp2
	WORD	rmp3

.define	GAME_HEIGHT 28
.define GAME_WIDTH  30
.define CELLS_WIDTH 32

.define RANDOM_HEIGHT 12
.define RANDOM_WIDTH  12
.define RANDOM_LIVE_RNG $78
.define RANDOM_FRAMES 160

.define FRAMES_PER_SECOND 60

.segment "SHADOW"
	BYTE	cellsPaddingBefore, 	CELLS_WIDTH
	BYTE	cells,			CELLS_WIDTH * GAME_HEIGHT
	BYTE	cellsPaddingAfter,	CELLS_WIDTH
	BYTE	prevCells,		CELLS_WIDTH * GAME_HEIGHT

	;; Number of frames before refresh
	UINT16	framesLeft

	;; If set then update tilemap on VBlank
	BYTE	updateTilemap

	;; The low bytes of the tilemap
	BYTE	tilemapBuffer, 	32 * 32

	;; The current game number
	BYTE	gameNumber


; -----------------------------------------------------------------------------

.code

ROUTINE Main
	REP	#$10
	SEP	#$20
.A8
.I16
	JSR	CpuUsage__CalcReference

	JSR	SetupPPU
	JSR	LoadTiles

	LDA	#NMITIMEN_VBLANK_FLAG
	STA	NMITIMEN

	; Start with a dull screen
	LDA	#$08
	STA	INIDISP

	STZ	gameNumber


	REPEAT
		JSR	LoadGame

		; Wait a second
		LDA	#FRAMES_PER_SECOND
		JSR	CpuUsage__WaitLimited

		LDA	#$0F
		STA	INIDISP

		JSR	PlayGame

		; Dull the screen and wait a bit
		LDA	#$08
		STA	INIDISP

		LDA	#10
		JSR	CpuUsage__WaitLimited

		; Increment to the next game
		INC	gameNumber
	FOREVER


;; Play game
.A8
.I16
ROUTINE PlayGame
	REPEAT
		JSR	Random__AddJoypadEntropy
		JSR	ProcessFrame

		; Clip the frame-rate (to known worst case)
		LDA	#3
		JSR	CpuUsage__WaitLimited

		LDX	framesLeft
		DEX
		STX	framesLeft
	UNTIL_ZERO

	RTS


; Helper macro to unroll the Process Frame Loop
; REQUIRE: 8 bit A, 16 bit Index
; X = Current position in `cells` array
.macro _ProcessFrame_UnrollLoop cell, prev_cell, buffer
	.local continue

	; Get and check cell's new state
	LDA prev_cell, X
	IF_N_SET
		CMP #CELL_ALIVE + 2
		BEQ continue
		CMP #CELL_ALIVE + 3
		BEQ continue

		; Cell is alive but doesn't have 2 or 3 neighbours
		; Kill it

		; Set cell dead
		LDA	cell, X
		AND	#.lobyte(~CELL_ALIVE)
		STA	cell, X

		; Update buffer
		STZ	buffer, X

		; Increment its neighbours
		DEC	cell - 1, X
		DEC	cell + 1, X
		DEC	cell - CELLS_WIDTH, X
		DEC	cell - CELLS_WIDTH - 1, X
		DEC	cell - CELLS_WIDTH + 1, X
		DEC	cell + CELLS_WIDTH, X
		DEC	cell + CELLS_WIDTH - 1, X
		DEC	cell + CELLS_WIDTH + 1, X
	ELSE
		CMP #3
		IF_EQ
			; No cell, and has 3 live neighbours
			; A new one is born

			; Set cell alive
			LDA	cell, X
			ORA	#CELL_ALIVE
			STA	cell, X

			; Update buffer
			LDA	#1
			STA	buffer, X

			; Increment its neighbours
			INC	cell - 1, X
			INC	cell + 1, X
			INC	cell - CELLS_WIDTH, X
			INC	cell - CELLS_WIDTH - 1, X
			INC	cell - CELLS_WIDTH + 1, X
			INC	cell + CELLS_WIDTH, X
			INC	cell + CELLS_WIDTH - 1, X
			INC	cell + CELLS_WIDTH + 1, X
		ENDIF
	ENDIF

	BRA continue


continue:

.endmacro

;; Process a Game of Life Frame.
;;
;; INPUT: long of data in in `rlepos`
;;
ROUTINE ProcessFrame
	REP	#$30
.I16
.A16
	; Copy cells to prevCells
	LDA	#.sizeof(cells) - 1
	LDX	#.loword(cells)
	LDY	#.loword(prevCells)
	MVN	#.bankbyte(prevCells), #.bankbyte(cells)

; MVN changes DB to .bankbyte(cells)

	SEP	#$20
.A8

	LDX	#0
	REPEAT
		; I have lots of free space, So I'll unroll the loop.
		;
		; Unfortunately for each line unrolled, 2790 bytes is added to the ROM.
		; Also UNROLL must be divisible by 28.
		;
		; Testing using `cpu_usage` bogos revealed the following spedups:
		;
		; Reference bogo:        $0A3C
		;   no unrolling    ($00 $026F) best case        ($02 $04E2) worst case
		;   1 line  - 17.1% ($00 $042E) best case, 5.7 % ($02 $06A1) worst case
		;   2 lines - 18.1% ($00 $0448) best case, 6.0 % ($02 $06BB) worst case
		;   4 lines - 18.5% ($00 $0455) best case, 6.2 % ($02 $06CB) worst case
		;   7 lines - 18.8% ($00 $045B) best case, 6.2 % ($02 $06CD) worst case
		UNROLL = 7

		.repeat UNROLL, H
			.repeat GAME_WIDTH, V
				delta .set H * CELLS_WIDTH + V + 1 ; +1 to remove left padding
				_ProcessFrame_UnrollLoop cells + delta, prevCells + delta, tilemapBuffer + delta
			.endrepeat
		.endrepeat

		REP	#$20
.A16
		TXA
		ADD	#UNROLL * CELLS_WIDTH
		TAX
		SEP	#$20
.A8

		CPX #.sizeof(cells)
	UNTIL_EQ

	LDA	#1
	STA	updateTilemap

	RTS


;; Loads the game specified by `gameNumber`.
;;
;; REQUIRES 8 bit A, 16 bit Index.
;;
;; GAME FORMAT
;;
;;	word - number of frames
;;	byte - starting X Position
;;	byte - starting Y
;;
;;	repeat:
;;		'O': New Cell
;;		EOL: New Line (Cursor will be shifted X position cells)
;;
;;	byte 0 - End of FILE
;;
;; NOTE:
;;	The cell format cannot wrap lines automatically,
;;	it must be done manually
.A8
.I16
ROUTINE LoadGame
.scope

startX   = tmp


	; Clear the CellMap
	FOR_X #0, INC, #GAME_WIDTH
		STZ	cellsPaddingBefore, X
	NEXT
	FOR_X #0, INC, #.sizeof(cells) + GAME_WIDTH
		STZ	cells, X
	NEXT

	; Clear the Tilemap
	FOR_X #0, INC, #.sizeof(tilemapBuffer)
		STZ	tilemapBuffer, X
	NEXT

	; Set padding of buffer to blank tile

	LDX #0
	REPEAT
		LDA #PADDING_TILE
		STA tilemapBuffer, X
		INC
		STA tilemapBuffer + CELLS_WIDTH - 1, X

		REP #$20
.A16
		TXA
		ADD	#32
		TAX

		SEP #$20
.A8

		CPX #.sizeof(::tilemapBuffer)
	UNTIL_EQ


	; Check to see if game is a random one
	LDA	gameNumber
	CMP	#NUM_GAMES
	IF_GE
		JMP	GenerateRandomGame
	ENDIF

	; Get the address of the game
	REP	#$30
.A16
	AND	#$00FF
	ASL
	TAX

	LDY	GamesTable, X


	; Byte 0 + 1 = Number of frames
	; Byte 2     = Start X position
	; Byte 3     = Starting Y Position
	;
	; startX = Starting X + 1
	; position = Starting Y * 32 + startX

	; Index X = Cells Position
	; Index Y = game file position

	LDA	0, Y
	STA	framesLeft

	INY
	INY

	LDA	0, Y
	AND	#$00FF
	INC
	STA	startX

	LDA	0, Y
	AND	#$FF00
	LSR		; 128
	LSR		; 64
	LSR		; 32
	ADD	startX
	TAX

	INY

	SEP #$20
.A8

	REPEAT
		INY
		LDA	0, Y
		IF_ZERO
			BREAK
		ENDIF

		CMP	#EOL
		IF_EQ
			; pos = (pos & ~$1F) + 32 + startX
			REP	#$30
.A16
			TXA
			AND	#.loword(~$1F)
			ADD	#32
			ADC	startX
			TAX

			SEP	#$20
.A8
		ELSE
			CMP	#'O'
			IF_EQ
				; +1 to remove left padding
				; Set cell alive
				LDA	cells, X
				ORA	#CELL_ALIVE
				STA	cells, X

				; Show in buffer
				LDA	#1
				STA	tilemapBuffer, X

				; Increment its neighbours
				INC	cells - 1, X
				INC	cells + 1, X
				INC	cells - CELLS_WIDTH, X
				INC	cells - CELLS_WIDTH - 1, X
				INC	cells - CELLS_WIDTH + 1, X
				INC	cells + CELLS_WIDTH, X
				INC	cells + CELLS_WIDTH - 1, X
				INC	cells + CELLS_WIDTH + 1, X
			ENDIF

			INX
		ENDIF

		CPX #CELLS_WIDTH * GAME_HEIGHT
	UNTIL_GE

	LDA	#1
	STA	updateTilemap

	RTS
.endscope


;; Generates a random game in the centre of the field
;;
;; REQUIRES: 8 bit A, 16 bit Index
;;
;; ASSUMES: The board is clear
.A8
.I16
ROUTINE GenerateRandomGame
.scope
X_POS = (CELLS_WIDTH - RANDOM_WIDTH) / 2
Y_POS = (GAME_HEIGHT - RANDOM_HEIGHT) / 2
MAP_START = (Y_POS * CELLS_WIDTH) + X_POS
MAP_END = MAP_START + RANDOM_HEIGHT * CELLS_WIDTH

RowCounter = tmp

	LDX	#MAP_START
	REPEAT
		LDA	#RANDOM_WIDTH
		STA	RowCounter

		REPEAT
			PHX
				JSR	Random__Rnd
			PLX

			CMP	#RANDOM_LIVE_RNG
			IF_LT
				; +1 to remove left padding
				; Set cell alive
				LDA	cells, X
				ORA	#CELL_ALIVE
				STA	cells, X

				; Show in buffer
				LDA	#1
				STA	tilemapBuffer, X

				; Increment its neighbours
				INC	cells - 1, X
				INC	cells + 1, X
				INC	cells - CELLS_WIDTH, X
				INC	cells - CELLS_WIDTH - 1, X
				INC	cells - CELLS_WIDTH + 1, X
				INC	cells + CELLS_WIDTH, X
				INC	cells + CELLS_WIDTH - 1, X
				INC	cells + CELLS_WIDTH + 1, X
			ENDIF

			INX
			DEC	RowCounter
		UNTIL_ZERO

		REP	#$31
.A16
		; c clear
		TXA
		ADC	#CELLS_WIDTH - RANDOM_WIDTH
		TAX

		SEP	#$20
.A8
		CPX	#MAP_END
	UNTIL_GE

	LDY	#RANDOM_FRAMES
	STY	framesLeft

	LDA	#1
	STA	updateTilemap

	RTS
.endscope


;; VBlank Handler
;;
;; Copies `tilemapBuffer` to VRAM when `updateTilemap` is set
.export VBlank
VBlank:
	; Save state
	REP #$30
	PHA
	PHB
	PHD
	PHX
	PHY

	SEP #$20
.A8
.I16
	; Reset NMI Flag.
	LDA	RDNMI

	CpuUsage_NMI

	; If updateTilemap is set then load buffer into VRAM
	; Remember that tilemapBuffer only stores the low byte of the tilemap
	LDA updateTilemap
	IF_NOT_ZERO
		TransferToVramLocationDataLow	tilemapBuffer, BG1_MAP
	ENDIF


	; Wait for end of auto-joy
	; (Needed for Random__AddJoypadEntropy)
	LDA	#HVJOY_AUTOJOY
	REPEAT
		BIT	HVJOY
	UNTIL_ZERO


	; Load State
	REP	#$30
	PLY
	PLX
	PLD
	PLB
	PLA

	RTI



;; Sets up the screen base addresses and mode.
;;
;; Mode 0, BG1 enabled, BG1 tilepos set by BG1_Tilemap and BG1_Tiles
.A8
.I16
ROUTINE SetupPPU
	LDA	#INIDISP_FORCE
	STA	INIDISP

	LDA	#BGMODE_MODE0
	STA	BGMODE

	Screen_SetVramBaseAndSize

	LDA	#TM_BG1
	STA	TM

	RTS



;; Copies the characters from Tiles to VRAM at word address  BG1_Tiles
;;
;; REQUIRES 8 bit A, 16 bit Index, Forced Blank
.A8
.I16
ROUTINE LoadTiles
	TransferToVramLocation Tiles, BG1_TILES
	TransferToCgramLocation Palette, 0

	RTS

; -----------------------------------------------------------------------------

.rodata

PADDING_TILE = 2

.export Tiles
Tiles:
	; Dead Cell
	.byte $00, $FF, $00, $81, $00, $81, $00, $81
	.byte $00, $81, $00, $81, $00, $81, $00, $FF

	; Alive Cell
	.byte $00, $81, $7E, $00, $7E, $00, $7E, $00
	.byte $7E, $00, $7E, $00, $7E, $00, $00, $81

	; Padding Left
	.byte $00, $01, $00, $01, $00, $01, $00, $01
	.byte $00, $01, $00, $01, $00, $01, $00, $01

	; Padding Right
	.byte $00, $80, $00, $80, $00, $80, $00, $80
	.byte $00, $80, $00, $80, $00, $80, $00, $80
Tiles_End:

Palette:
	.word	$0000, $7FFF, $4DEF
Palette_End:

.proc Games

::NUM_GAMES = (EndGamesTable - GamesTable) / 2

::GamesTable:
	.addr Life
	.addr Glider
	.addr Multum_in_parvo
EndGamesTable:



Life:
	.word 300
	.byte 5, 10
	.byte ".O....OOO.OOOO.OOOO.", EOL
	.byte ".O.....O..O....O....", EOL
	.byte ".O.....O..O....O....", EOL
	.byte ".O.....O..OOO..OOOO.", EOL
	.byte ".O.....O..O....O....", EOL
	.byte ".O.....O..O....O....", EOL
	.byte ".O.....O..O....O....", EOL
	.byte ".OOOO.OOO.O....OOOO.", EOL
	.byte 0

Glider:
	.word 120
	.byte 0, 0
	.byte ".O.", EOL
	.byte "..O", EOL
	.byte "OOO", EOL
	.byte 0


;Name: Multum in parvo
;Author: Charles Corderman
;A methuselah with lifespan 3933.
;www.conwaylife.com/wiki/index.php?title=Multum_in_parvo
Multum_in_parvo:
	.word 600
	.byte 12, 11
	.byte "...OOO", EOL
	.byte "..O..O", EOL
	.byte ".O", EOL
	.byte "O", EOL
	.byte 0


BestCase:
	.word 1
	.byte 0,0
	.byte 0


WorstCase:
	.word 2
	.byte 0, 0
	.repeat 28
		.res 30, 'O'
		.byte EOL
	.endrepeat
	.byte 0

.endproc


