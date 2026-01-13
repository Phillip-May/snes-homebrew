;; Routine that tests the Math Routines for correctness.

.setcpu "65816"

.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"
.include "includes/static_random.inc"

.include "routines/block.h"
.include "routines/reset-snes.h"
.include "routines/text.h"
.include "routines/screen.h"
.include "routines/text8x8.h"
.include "routines/text8x16.h"

BG1_MAP			= $0400
BG1_TILES		= $1000


.segment "SHADOW"
	BYTE	noErrors		; If true then there are no errors.
	WORD	passNumber
	ADDR	pageNumber

.code

.A8
.I16

.include "macros.inc"
.include "tests/text.asm"
.include "tests/math.asm"
.include "tests/metasprite.asm"

BuildPageTable

.code
ROUTINE Main
	REP	#$10        ; X/Y 16-bit
	SEP	#$20        ; A 8-bit
.A8
.I16

	JSR	SetupPPU

	; Copies the palette to CGRAM
	TransferToCgramLocation FontBoldTransparentPalette, 0

	Text_LoadFont Font8x16BoldTransparent, BG1_TILES, BG1_MAP

	Text_SelectWindow 0
	Text_SetStringBasic
	Text_SetInterface Text8x16__Interface, 0
	Text_SetupWindow 1, 1, 30, 26, Text__WINDOW_NO_BORDER

	LDA	#$0F
	STA	INIDISP

	LDA	#1
	STA	noErrors

	LDX	#0
	STX	passNumber

	REPEAT
		LDX	#0
		STX	pageNumber

		REPEAT
			LDX	pageNumber
			JSR	(.loword(PageTable), X)

			JSR	NewPage

			LDX	pageNumber
			INX
			INX
			STX	pageNumber

			CPX	#PageTableEnd - PageTable
		UNTIL_GE

		INC16	passNumber

		LDA	noErrors
		IF_NOT_ZERO
			Text_SetColor	2

			Text_SetCursor	5, 12
			Text_PrintString "All Tests Completed"
			Text_SetCursor	10, 14
			Text_PrintString "No Errors"
			JSR	NewPage
		ENDIF
	FOREVER




;; For the first pass just go through all the pages as fast as possible.
;;
;; Waits for a button-press, then waits until all buttons are released.
;;
;; REQUIRES: 8 bit A, 16 bit Index
.I16
ROUTINE NewPage
	; If on first pass and there are no errors.
	LDA	noErrors
	IF_NOT_ZERO
		LDX	passNumber
		IF_ZERO
			WAI
			JMP	Text__ClearWindow
		ENDIF
	ENDIF

	; Wait until button pressed
	REPEAT
		WAI
		LDY	JOY1
	UNTIL_NOT_ZERO

	; Wait until button released
	REPEAT
		WAI
		LDY	JOY1
	UNTIL_ZERO

	JMP	Text__ClearWindow



;; Blank Handlers
LABEL IrqHandler
LABEL CopHandler
	RTI


LABEL VBlank
	; Save state
	REP	#$30
	PHA
	PHX
	PHY

	SEP	#$20
.A8
.I16

	; Reset NMI Flag.
	LDA	RDNMI

	Text_VBlank
	Screen_VBlank

	; restore state
	REP	#$30
	PLY
	PLX
	PLA
	RTI



;; Sets up the screen base addresses and mode.
;;
;; Mode 0, BG1 enabled, BG1 tilepos set by BG1_Tilemap and BG1_Tiles, and VBlank enabled
.A8
.I16
ROUTINE SetupPPU
	LDA	#INIDISP_FORCE
	STA	INIDISP

	LDA	#BGMODE_MODE0
	STA	BGMODE

	LDA	#(BG1_MAP / BGXSC_BASE_WALIGN) << 2
	STA	BG1SC

	LDA	#BG1_TILES / BG12NBA_BASE_WALIGN
	STA	BG12NBA

	LDA	#TM_BG1
	STA	TM

	LDA	#NMITIMEN_VBLANK_FLAG | NMITIMEN_AUTOJOY_FLAG
	STA	NMITIMEN

	RTS





.rodata

Font8x8BoldTransparent:
	.incbin "../resources/tiles2bpp/font8x8-bold-transparent.2bpp"
Font8x8BoldTransparent_End:

Font8x16BoldTransparent:
	.incbin "../resources/tiles2bpp/font8x16-bold-transparent.2bpp"
Font8x16BoldTransparent_End:

FontBoldTransparentPalette:                ; ANSI Colors
	.word	$7FFF, $0000, $4e73, $6b3a ; Black   (0)
	.word	$7FFF, $001F, $4e73, $6b3a ; Red     (1)
	.word	$7FFF, $02E0, $4e73, $6b3a ; Green   (2)
	.word	$7FFF, $02FF, $4e73, $6b3a ; Yellow  (3)
	.word	$7FFF, $7C00, $4e73, $6b3a ; Blue    (4)
	.word	$7FFF, $3C0F, $4e73, $6b3a ; Magenta (5)
	.word	$7FFF, $3DE0, $4e73, $6b3a ; Cyan    (6)
	.word	$7FFF, $3DEF, $4e73, $6b3a ; Gray    (7)
FontBoldTransparentPalette_End:


; Just to get it to compile

LABEL SnesGss__Module
LABEL SnesGss__ModulePart2
LABEL SnesGss__MusicTable
.exportzp SnesGss__MusicTable_Count = 0

LABEL PalettesTable
LABEL VramTable

