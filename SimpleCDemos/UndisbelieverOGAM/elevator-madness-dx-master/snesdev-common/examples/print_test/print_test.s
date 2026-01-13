; A test of `routines.text.s`

.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"

.include "routines/reset-snes.h"
.include "routines/text.h"
.include "routines/text8x8.h"
.include "routines/screen.h"

BG1_MAP		= $0400
BG1_TILES	= $1000
BG1_SIZE    = BGXSC_SIZE_32X32

.code

ROUTINE Main
	REP	#$10        ; X/Y 16-bit
	SEP	#$20        ; A 8-bit
.A8
.I16

	JSR	SetupPPU

	; Enable V-Blank
	LDA	#NMITIMEN_VBLANK_FLAG
	STA	NMITIMEN

	Text_LoadFont Font8x8Bold, BG1_TILES, BG1_MAP
	Text_SetInterface Text8x8__DoubleSpacingInterface, 1
	Text_SetStringBasic

	JSR	LoadPalette

	LDA	#$0F
	STA	INIDISP

	Text_SetupWindow 2, 2, 29, 25, Text__WINDOW_BORDER

	Text_SetColor 4
	Text_PrintStringLn "The quick brown fox jumped over the lazy dog."

	Text_SetStringWordWrapping

	Text_SetColor 5
	Text_PrintStringLn "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG."

	JSR	Text__NewLine

	Text_SetStringBasic

	Text_SetColor 4
	Text_PrintString StringFromLabel

	Text_SetStringWordWrapping

	Text_SetColor 5
	Text_PrintString StringFromLabel

	REPEAT
		WAI
	FOREVER

.rodata

StringFromLabel:
	.byte "123456789 123456789 123456789 "
	.byte "123456789012345678901234567890", EOL, 0 ; Word wrap test

.code

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

	; restore state
	REP	#$30
	PLY
	PLX
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



;; Copies the palette to CGRAM
;;
;; REQUIRES 8 bit A, 16 bit Index, Forced Blank
.A8
.I16
ROUTINE LoadPalette
	; Load white to color 1
	LDA	#0
	STA	CGADD

	LDA	#DMAP_DIRECTION_TO_PPU | DMAP_TRANSFER_1REG
	STA	DMAP0

	LDA	#.lobyte(CGDATA)
	STA	BBAD0

	LDX	#FontBoldPalette_End - FontBoldPalette
	STX	DAS0

	LDX	#.loword(FontBoldPalette)
	STX	A1T0
	LDA	#.bankbyte(FontBoldPalette)
	STA	A1B0

	LDA	#MDMAEN_DMA0
	STA	MDMAEN

	RTS



.rodata

Font8x8Bold:
	.res 16, 0
	.incbin "resources/tiles2bpp/font8x8-bold.2bpp"
Font8x8Bold_End:

FontBoldPalette:                          ; ANSI Colors
	.word	$001f, $7FFF, $0000, $5294 ; Black   (0)
	.word	$7FFF, $7FFF, $001F, $5294 ; Red     (1)
	.word	$7FFF, $7FFF, $02E0, $5294 ; Green   (2)
	.word	$7FFF, $7FFF, $02FF, $5294 ; Yellow  (3)
	.word	$7FFF, $7FFF, $7C00, $5294 ; Blue    (4)
	.word	$7FFF, $7FFF, $3C0F, $5294 ; Magenta (5)
	.word	$7FFF, $7FFF, $3DE0, $5294 ; Cyan    (6)
	.word	$7FFF, $7FFF, $3DEF, $5294 ; Gray    (7)
FontBoldPalette_End:

