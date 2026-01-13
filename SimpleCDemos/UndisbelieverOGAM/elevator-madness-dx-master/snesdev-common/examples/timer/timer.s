;; Displays the current time onto the screen
;; using two text windows:
;;
;;	* One in 8x8, cyan, displaying hexadecimal.
;;	* The other 8x16 font, black with decimal.
;;
;; This is an example of using multiple text windows.


.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"

.include "routines/math.h"
.include "routines/reset-snes.h"
.include "routines/text.h"
.include "routines/text8x8.h"
.include "routines/text8x16.h"
.include "routines/screen.h"

BG1_MAP			= $0400
BG1_TILES		= $1000
BG1_SIZE    = BGXSC_SIZE_32X32

LARGE_FONT_OFFSET	= 112

.segment "SHADOW"
	UINT32	frameCounter
	WORD	hours
	BYTE	minutes
	BYTE	seconds
	BYTE	fractionOfSeconds

.code
ROUTINE Main
	REP	#$10        ; X/Y 16-bit
	SEP	#$20        ; A 8-bit
.A8
.I16

	JSR	SetupPPU

	Text_LoadFont Font8x8BoldTransparent, BG1_TILES, BG1_MAP
	Text_LoadFont Font8x16BoldTransparent, BG1_TILES + LARGE_FONT_OFFSET * 8, BG1_MAP

	JSR	LoadPalette

	LDA	#$0F
	STA	INIDISP

	Text_SelectWindow 0
	Text_SetStringBasic
	Text_SetInterface Text8x8__SingleSpacingInterface, 0
	Text_SetColor 7
	Text_SetupWindow 16, 3, 28, 4, Text__WINDOW_BORDER
	Text_SetColor 6

	Text_SelectWindow 1
	Text_SetStringBasic
	Text_SetInterface Text8x16__Interface, LARGE_FONT_OFFSET
	Text_SetupWindow 3, 19, 28, 24, Text__WINDOW_BORDER

	REPEAT
		JSR	CalculateTime

		JSR	DisplayTimeHex
		JSR	DisplayTimeDecimal

		WAI
	FOREVER

;; Prints the time in hex to Text Window 0
;;
;; REQUIRES: 8 bit A, 16 bit Index
.A8
.I16
ROUTINE DisplayTimeHex
	Text_SelectWindow 0

	Text_SetCursor 5, 0

	Text_PrintHex frameCounter

	; no new line - text overflow

	Text_PrintHex hours
	Text_PrintChar ':'
	Text_PrintHex minutes
	Text_PrintChar ':'
	Text_PrintHex seconds
	Text_PrintChar ':'
	Text_PrintHex fractionOfSeconds

	RTS


;; Prints the time in hex to Text Window 1
;;
;; REQUIRES: 8 bit A, 16 bit Index
.A8
.I16
ROUTINE DisplayTimeDecimal
	Text_SelectWindow 1

	Text_SetCursor 8, 1

	Text_PrintDecimal frameCounter, 10

	Text_SetCursor 8, 3

	Text_PrintDecimal hours
	Text_PrintChar ':'
	Text_PrintDecimal minutes, 2
	Text_PrintChar ':'
	Text_PrintDecimal seconds, 2
	Text_PrintChar ':'
	Text_PrintDecimal fractionOfSeconds, 2

	RTS


;; Converts `frameCounter` into `hours`, `minutes`, `seconds`, `fractionOfSeconds`.
;;
;; REQUIRE: 8 bit A, 16 bit Index
.A8
.I16
ROUTINE CalculateTime
	LDX	frameCounter
	STX	Math__dividend32
	LDX	frameCounter + 2
	STX	Math__dividend32 + 2

	LDA	#FPS
	JSR	Math__Divide_U32_U8A

	STA	fractionOfSeconds ; A = remainder

	; result32 and dividend32 share memory location

	LDA	#60
	JSR	Math__Divide_U32_U8A

	STA	seconds

	; result32 and dividend32 share memory location
	LDA	#60
	JSR	Math__Divide_U32_U8A

	STA	minutes

	LDX	Math__result32
	STX	hours
	RTS



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

	INC	frameCounter
	IF_ZERO
		INC	frameCounter + 2
	ENDIF

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
;; Mode 0, BG1 enabled, BG1 tilepos set by BG1_Tilemap and BG1_Tiles, and VBlank enabled
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

	LDA	#NMITIMEN_VBLANK_FLAG
	STA	NMITIMEN

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

	LDX	#FontBoldTransparentPalette_End - FontBoldTransparentPalette
	STX	DAS0

	LDX	#.loword(FontBoldTransparentPalette)
	STX	A1T0
	LDA	#.bankbyte(FontBoldTransparentPalette)
	STA	A1B0

	LDA	#MDMAEN_DMA0
	STA	MDMAEN

	RTS



.rodata

Font8x8BoldTransparent:
	.incbin "resources/tiles2bpp/font8x8-bold-transparent.2bpp"
Font8x8BoldTransparent_End:

Font8x16BoldTransparent:
	.incbin "resources/tiles2bpp/font8x16-bold-transparent.2bpp"
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

