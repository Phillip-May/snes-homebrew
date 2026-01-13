;; A simple demo of a port of the game Simon.
;;
;; This demo incorporates:
;;	* A complex VBlank buffer to illuminating the lights
;;	* The Random Module
;;	* The Controller Module
;;	* The SNESGSS Module

.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"
.include "includes/config.inc"

.include "routines/screen.h"
.include "routines/block.h"
.include "routines/random.h"
.include "routines/math.h"
.include "routines/controller.h"
.include "routines/snesgss.h"

.include "resources/snesgss/export/snesgss.inc.h"


GAME_BGMODE		= BGMODE_MODE1
GAME_LAYERS		= TM_BG1
GAME_BG1_SIZE		= BGXSC_SIZE_32X32
GAME_BG1_MAP		= $0400
GAME_BG1_TILES		= $1000

TEXT_TILE_OFFSET	= (GameFieldTiles_End - GameFieldTiles) / 32
TEXT_TILEMAP_OFFSET	= TEXT_TILE_OFFSET  | (TEXT_PALETTE << TILEMAP_PALETTE_SHIFT)
TEXT_BOTTOM_OFFSET	= (TextTiles_End - TextTiles) / 2 / 32
TEXT_PALETTE		= 0
TEXT_WIDTH		= 6
TEXT_HEIGHT		= 4
TEXT_XPOS		= (32 - TEXT_WIDTH) / 2
TEXT_YPOS		= (28 - TEXT_HEIGHT) / 2

LIFE_ONE_TILE		= TEXT_TILE_OFFSET + 23
LIFE_TWO_TILE		= TEXT_TILE_OFFSET + 24
LIFE_THREE_TILE		= LIFE_ONE_TILE + TEXT_BOTTOM_OFFSET
LIFE_FOUR_TILE		= LIFE_TWO_TILE + TEXT_BOTTOM_OFFSET


BUTTON_PALETTE_START	= 4

PALETTE_X		= 4
PALETTE_A		= 5
PALETTE_B		= 6
PALETTE_Y		= 7

N_LIVES			= 4

.define	MAX_SCORE	 9999
.define	MAX_SCORE_BCD	$9999

.proc NTSC_FRAME_DELAYS
	LIGHT_ON	= 60 * 1 / 2
	LIGHT_OFF	= 60 / 10
	NEXT_ROUND	= 60
	WRONG_BUTTON	= 60 * 1
	PRESS_START_MSG	= 60 * 10
.endproc
.proc PAL_FRAME_DELAYS
	LIGHT_ON	= 50 * 1 / 2
	LIGHT_OFF	= 50 / 10
	NEXT_ROUND	= 50
	WRONG_BUTTON	= 50 * 1
	PRESS_START_MSG	= 50 * 10
.endproc

; -----------------------------------------------------------------------------

.segment "COPYRIGHT"
		;1234567890123456789012345678901
	.byte	"Simon Says                     ", 10
	.byte	"(c) 2015, The Undisbeliever    ", 10
	.byte	"MIT Licensed                   ", 10
	.byte	"One Game Per Month Challange   ", 10

; -----------------------------------------------------------------------------

.segment "SHADOW"
	WORD	tmp

	;; Player's Score
	;; 16 bit BCD
	WORD	score

	;; Number of lives left in play
	BYTE	lives

	;; Number of entries in the run
	ADDR	currentRunPos

	;; Position of the player in the current run
	ADDR	position

	;; A buffer of the CGRAM that is updated to PPU on VBlank
	WORD	buttonPaletteBuffer, 4 * 16
	;; Updates the CGRAM on VBlank if non-zero
	BYTE	updateButtonPaletteBuffer

	;; A small buffer to hold the text in.
	;; 6x4 tiles in size
	WORD	textBuffer, TEXT_WIDTH * TEXT_HEIGHT
	;; Updates the small buffer if non-zero
	BYTE	updateTextBuffer

	;; Frame delays, stored in RAM so that NTSC and PAL times
	;; exist on a single ROM.
	UINT16	frameDelay_lightOn
	UINT16	frameDelay_lightOff
	UINT16	frameDelay_nextRound
	UINT16	frameDelay_wrongButton
	UINT16	frameDelay_pressStartMessage


; Why not, I have a lot of RAM to play with
.segment "WRAM7F"
	BYTE	currentRun,	MAX_SCORE

; -----------------------------------------------------------------------------

.code

ROUTINE Main
	REP	#$10
	SEP	#$20
.A8
.I16
	JSR	Init
	JSR	Screen__FadeIn

	REPEAT
		JSR	WaitForStart
		JSR	PlayGame
	FOREVER



;; Initialize the game
.A8
.I16
ROUTINE Init
	JSR	SnesGss__Init

	LDA	STAT78
	IF_BIT	#STAT78_PAL_MASK
		.assert STAT78_PAL <> 0, error, "Bad Value"
		; PAL Timings
		LDX	#PAL_FRAME_DELAYS::LIGHT_ON
		STX	frameDelay_lightOn
		LDX	#PAL_FRAME_DELAYS::LIGHT_OFF
		STX	frameDelay_lightOff
		LDX	#PAL_FRAME_DELAYS::NEXT_ROUND
		STX	frameDelay_nextRound
		LDX	#PAL_FRAME_DELAYS::WRONG_BUTTON
		STX	frameDelay_wrongButton
		LDX	#PAL_FRAME_DELAYS::PRESS_START_MSG
		STX	frameDelay_pressStartMessage
	ELSE
		; NTSC Timings
		LDX	#NTSC_FRAME_DELAYS::LIGHT_ON
		STX	frameDelay_lightOn
		LDX	#NTSC_FRAME_DELAYS::LIGHT_OFF
		STX	frameDelay_lightOff
		LDX	#NTSC_FRAME_DELAYS::NEXT_ROUND
		STX	frameDelay_nextRound
		LDX	#NTSC_FRAME_DELAYS::WRONG_BUTTON
		STX	frameDelay_wrongButton
		LDX	#NTSC_FRAME_DELAYS::PRESS_START_MSG
		STX	frameDelay_pressStartMessage
	ENDIF

	MemClear textBuffer
	LDA	#1
	STA	updateTextBuffer

	JSR	ResetLights
	JSR	SetupScreen

	RTS



;; Wait for player to press start
;; Show the press start message after `frameDelay_pressStartMessage` frames
.A8
.I16
ROUTINE WaitForStart
	STZ	lives
	JSR	DisplayLivesLeft

	LDX	frameDelay_pressStartMessage
	REPEAT
		PHX
		JSR	WaitFrame
		PLX

		LDA	Controller__pressed + 1
		AND	#JOYH_START
		BNE	WaitForStart_StartPressed

		DEX
	UNTIL_MINUS

	MemCopy	PressStartTextBuffer, textBuffer
	LDA	#1
	STA	updateTextBuffer

	REPEAT
		JSR	WaitFrame

		LDA	Controller__pressed + 1
		AND	#JOYH_START
	UNTIL_NOT_ZERO

WaitForStart_StartPressed:

	MemClear textBuffer
	LDA	#1
	STA	updateTextBuffer

	RTS



;; Play game
.A8
.I16
ROUTINE PlayGame
	LDA	#N_LIVES
	STA	lives

	LDX	#0
	STX	currentRunPos
	STX	score

	JSR	DisplayScore

	JSR	GenerateNext

	REPEAT
		JSR	DisplayLivesLeft
		JSR	ResetLights

		LDA	frameDelay_lightOff
		JSR	Screen__WaitManyFrames

		REPEAT
			JSR	ShowPlayerRun

			JSR	PlayerRepeatRun
			BCC	BREAK_LABEL

			JSR	GenerateNext

			JSR	IncrementScore

			LDA	#SnesGssSfx::SCORE_POINT
			JSR	SnesGss__PlaySfxRC

			LDA	frameDelay_nextRound
			JSR	Screen__WaitManyFrames
		FOREVER

		LDA	frameDelay_wrongButton
		JSR	Screen__WaitManyFrames

		DEC	lives
	UNTIL_ZERO

	JSR	DisplayLivesLeft
	JSR	ResetLights

	LDA	#SnesGssSfx::GAME_OVER
	JSR	SnesGss__PlaySfxRC

	RTS

; -----------------------------------------------------------------------------

;; Shows the run to the player
.A8
.I16
ROUTINE ShowPlayerRun
	LDX	#0

	REPEAT
		STX	position
		LDA	f:currentRun, X

		PHA
		JSR	IlluminateLight

		PLA
		JSR	PlayButtonSound

		LDA	frameDelay_lightOn
		JSR	Screen__WaitManyFrames

		JSR	ResetLights

		LDA	frameDelay_lightOff
		JSR	Screen__WaitManyFrames

		LDX	position
		INX
		STX	position
		CPX	currentRunPos
	UNTIL_GE

	RTS



;; Player repeats the run.
;; RETURNS: C set if run successful, otherwise failed
.A8
.I16
ROUTINE PlayerRepeatRun
	LDX	#0
	STX	position

	JSR	WaitFrame

	REPEAT
		REP	#$20
.A16
		REPEAT
			LDA	Controller__pressed
			AND	#JOY_A | JOY_B | JOY_X | JOY_Y
		UNTIL_NOT_ZERO

		IF_BIT	#JOY_X
			LDA	#PALETTE_X
		ELSE_BIT #JOY_A
			LDA	#PALETTE_A
		ELSE_BIT #JOY_B
			LDA	#PALETTE_B
		ELSE
			LDA	#PALETTE_Y
		ENDIF

		SEP	#$20
.A8

		LDX	position
		CMP	f:currentRun, X
		IF_NE
			LDA	f:currentRun, X

			PHA
			JSR	IlluminateOtherLights

			PLA
			JSR	PlayWrongButtonSound

			CLC
			RTS
		ENDIF

		LDX	position
		CMP	f:currentRun, X

		JSR	PlayButtonSound

		; This wait is here in case currentRun[x - 1] == currentRun[x]
		; and the player is too quick.
		; Without it there is no dull frame and it doesn't look right.
		JSR	WaitFrame

		LDX	position
		LDA	f:currentRun, X
		JSR	IlluminateLight

		; Wait `frameDelay_lightOn` frames or until next button pressed
		REP	#$20
.A16
		LDA	frameDelay_lightOn
		STA	tmp

		REPEAT
			LDA	Controller__pressed
			AND	#JOY_A | JOY_B | JOY_X | JOY_Y
			BNE	BREAK_LABEL

			JSR	WaitFrame

			DEC	tmp
		UNTIL_ZERO

		SEP	#$20
.A8

		JSR	ResetLights

		LDX	position
		INX
		STX	position
		CPX	currentRunPos
	UNTIL_GE

	SEC
	RTS

; -----------------------------------------------------------------------------

;; Generate next item in sequence
.A8
.I16
ROUTINE GenerateNext
	JSR	Random__Rnd_4
	ADD	#BUTTON_PALETTE_START

	LDX	currentRunPos
	CPX	#MAX_SCORE
	IF_LT
		STA	f:currentRun, X
		INX
		STX	currentRunPos
	ENDIF

	RTS



;; Increments the score by 1 and displays it on screen
.A8
.I16
ROUTINE IncrementScore
	REP	#$20
.A16
	; score is BCD, cannot use INC
	SED
	LDA	score
	CMP	#MAX_SCORE_BCD
	IF_LT
		ADD	#1
		STA	score
	ENDIF
	CLD

	.assert * = DisplayScore, error, "Bad Flow"

; -----------------------------------------------------------------------------

;; Displays the score into the textBuffer
;; IN: score (BCD)
.A8
.I16
ROUTINE DisplayScore
	REP	#$30
.A16
	LDX	#(TEXT_WIDTH + (TEXT_WIDTH - 4) / 2) * 2
	LDY	#.sizeof(score) - 1

	REPEAT
		LDA	score, Y
		AND	#$00F0
		LSR
		LSR
		LSR
		LSR
		ADD	#TEXT_TILEMAP_OFFSET
		STA	textBuffer, X
		ADD	#TEXT_BOTTOM_OFFSET
		STA	textBuffer + TEXT_WIDTH * 2, X

		LDA	score, Y
		AND	#$000F
		ADD	#TEXT_TILEMAP_OFFSET
		STA	textBuffer + 2, X
		ADD	#TEXT_BOTTOM_OFFSET
		STA	textBuffer + TEXT_WIDTH * 2 + 2, X

		INX
		INX
		INX
		INX
		DEY
	UNTIL_MINUS

	SEP	#$20
.A8
	LDA	#1
	STA	updateTextBuffer

	RTS



;; Displays the number of lives left.
;; The lives positions are hard coded to the 4 corners of the text buffer.
.A8
.I16
ROUTINE DisplayLivesLeft
	.assert N_LIVES = 4, error, "The game is hard-coded for 4 lives"

	LDA	lives

	CMP	#1
	IF_GE
		LDX	#LIFE_ONE_TILE
	ELSE
		LDX	#0
	ENDIF
	STX	textBuffer

	CMP	#2
	IF_GE
		LDX	#LIFE_TWO_TILE
	ELSE
		LDX	#0
	ENDIF
	STX	textBuffer + TEXT_WIDTH * 2 - 2

	CMP	#3
	IF_GE
		LDX	#LIFE_THREE_TILE
	ELSE
		LDX	#0
	ENDIF
	STX	textBuffer + .sizeof(textBuffer) - 2

	CMP	#4
	IF_GE
		LDX	#LIFE_FOUR_TILE
	ELSE
		LDX	#0
	ENDIF
	STX	textBuffer + .sizeof(textBuffer) - TEXT_WIDTH * 2

	LDA	#1
	STA	updateTextBuffer

	RTS



;; Illuminates a given light
;; IN: A - the light to blink
.A8
.I16
ROUTINE IlluminateLight

	REP	#$20
.A16
	; X = (A - BUTTON_PALETTE_START & 4) * 32
	SUB	#BUTTON_PALETTE_START
	AND	#3
	ASL
	ASL
	ASL
	ASL
	ASL
	TAX

	FOR_Y #0, INC, #16
		LDA	GameFieldBrightPalette, X
		STA	buttonPaletteBuffer, X
		INX
		INX
	NEXT

	SEP	#$20
.A8
	LDA	#1
	STA	updateButtonPaletteBuffer

	RTS



;; Turns on all lights except the given one.
.A8
.I16
ROUTINE IlluminateOtherLights
	STA	tmp

	JSR	ResetLights

	LDA	#PALETTE_X
	CMP	tmp
	IF_NE
		JSR	IlluminateLight
	ENDIF

	LDA	#PALETTE_A
	CMP	tmp
	IF_NE
		JSR	IlluminateLight
	ENDIF

	LDA	#PALETTE_B
	CMP	tmp
	IF_NE
		JSR	IlluminateLight
	ENDIF

	LDA	#PALETTE_Y
	CMP	tmp
	IF_NE
		JSR	IlluminateLight
	ENDIF

	RTS



;; Turns off all of the lights
.A8
.I16
ROUTINE ResetLights
	MemCopy	GameFieldDullPalette, buttonPaletteBuffer

	LDA	#1
	STA	updateButtonPaletteBuffer

	RTS

; -----------------------------------------------------------------------------

;; Plays the button sound for the given button
;; IN: A - button.
.A8
.I16
ROUTINE PlayButtonSound
	SEP	#$30
.A8
.I8
	SUB	#BUTTON_PALETTE_START
	TAX

	REP	#$10
.I16
	LDA	ButtonSfxTable, X
	JMP	SnesGss__PlaySfxRC



;; Plays the wrong button sound for the given button
;; IN: A - button.
.A8
.I16
ROUTINE PlayWrongButtonSound
	SEP	#$30
.A8
.I8
	SUB	#BUTTON_PALETTE_START
	TAX

	REP	#$10
.I16
	LDA	WrongButtonSfxTable, X
	JMP	SnesGss__PlaySfxRC

; -----------------------------------------------------------------------------

;; Waits one frame and increments the RNG
.A8
.I16
ROUTINE WaitFrame
	JSR	Screen__WaitFrame
	JMP	Random__AddJoypadEntropy

; -----------------------------------------------------------------------------

;; Sets up the PPU and Graphics
.A8
.I16
ROUTINE SetupScreen
	LDA	#INIDISP_FORCE
	STA	INIDISP

	LDA	#GAME_BGMODE
	STA	BGMODE

	LDA	#GAME_LAYERS
	STA	TM

	Screen_SetVramBaseAndSize GAME

	TransferToVramLocation	GameFieldTiles,		GAME_BG1_TILES
	TransferToCgramLocation	GameFieldDullPalette,	BUTTON_PALETTE_START * 16

	; Transfer AFTER game field
	TransferToVram		TextTiles
	TransferToCgramLocation	TextPalette,		TEXT_PALETTE * 16


	; Load tile map to VRAM
	; ::BUGFIX pcx2snes does not include palette::
	; ::: Have to append palette to map manually::
	REP	#$20
.A16
	LDX	#GAME_BG1_MAP
	STX	VMADD

	FOR_X #0, INC2, #32*28*2
		LDA	GameFieldMap, X
		ORA	GameFieldMapPaletteBits, X
		STA	VMDATA
	NEXT

	SEP	#$20
.A8

	STZ	BG1HOFS
	STZ	BG1HOFS
	LDA	#$FF
	STA	BG1VOFS
	STA	BG1VOFS

	LDA	#NMITIMEN_VBLANK_FLAG | NMITIMEN_AUTOJOY_FLAG
	STA	NMITIMEN

	RTS

; -----------------------------------------------------------------------------

; Ignore interrupts
.import EmptyHandler
.export IrqHandler	= EmptyHandler
.export CopHandler	= EmptyHandler


;; VBlank Handler
.export VBlank
VBlank:
	; Save state
	REP #$30
.I16
.A16
	PHA
	PHB
	PHD
	PHX
	PHY

	PHK
	PLB

	LDA	#$2100
	TCD

	SEP #$20
.A8
.I16
	; Reset NMI Flag.
	LDA	RDNMI

	; Update the button's palette
	LDA	updateButtonPaletteBuffer
	IF_NOT_ZERO
		TransferToCgramLocation	buttonPaletteBuffer, BUTTON_PALETTE_START * 16

		STZ	updateButtonPaletteBuffer
	ENDIF

	; Update the tiny text buffer in center of display
	LDA	updateTextBuffer
	IF_NOT_ZERO
		.repeat	TEXT_HEIGHT, th
			TransferToVramLocation	textBuffer + th * TEXT_WIDTH * 2,	GAME_BG1_MAP + (TEXT_YPOS + th) * 32 + TEXT_XPOS,	TEXT_WIDTH * 2
		.endrepeat
		STZ	updateTextBuffer
	ENDIF

	Screen_VBlank
	Controller_VBlank

	; Load State
	REP	#$30
	PLY
	PLX
	PLD
	PLB
	PLA

	RTI

; -----------------------------------------------------------------------------

.define SNESGSS_BANK1 "BANK1"
	.include "resources/snesgss/export/snesgss.inc"

; -----------------------------------------------------------------------------

.rodata

	INCLUDE_BINARY GameFieldMap,		"resources/gamefield.map"
	INCLUDE_BINARY GameFieldTiles,		"resources/gamefield.4bpp"

	;; The Palette of the bright colors to display on screen
	INCLUDE_BINARY GameFieldBrightPalette,	"resources/gamefield.clr", 0, 4 * 16 * 2

	;; The Palette of the dull colors to display on screen
	INCLUDE_BINARY GameFieldDullPalette,	"resources/gamefield.clr", 4 * 16 * 2, 4 * 16 * 2

	INCLUDE_BINARY TextTiles,		"resources/text.4bpp"
	INCLUDE_BINARY TextPalette,		"resources/text.clr"


LABEL GameFieldMapPaletteBits
	.repeat 14, ty
		.repeat ty + 1
			.word PALETTE_Y << TILEMAP_PALETTE_SHIFT
		.endrepeat
		.repeat 30 - ty * 2
			.word PALETTE_X << TILEMAP_PALETTE_SHIFT
		.endrepeat
		.repeat ty + 1
			.word PALETTE_A << TILEMAP_PALETTE_SHIFT
		.endrepeat
	.endrepeat
	.repeat 14, ty
		.repeat 14 - ty
			.word PALETTE_Y << TILEMAP_PALETTE_SHIFT
		.endrepeat
		.repeat ty * 2 + 4
			.word PALETTE_B << TILEMAP_PALETTE_SHIFT
		.endrepeat
		.repeat 14 - ty
			.word PALETTE_A << TILEMAP_PALETTE_SHIFT
		.endrepeat
	.endrepeat


LABEL PressStartTextBuffer
	.repeat TEXT_WIDTH, tw
		.word	(TEXT_TILEMAP_OFFSET + 11 + tw)
	.endrepeat
	.repeat TEXT_WIDTH, tw
		.word	(TEXT_TILEMAP_OFFSET + TEXT_BOTTOM_OFFSET + 11 + tw)
	.endrepeat
	.repeat TEXT_WIDTH, tw
		.word	(TEXT_TILEMAP_OFFSET + 17 + tw)
	.endrepeat
	.repeat TEXT_WIDTH, tw
		.word	(TEXT_TILEMAP_OFFSET + TEXT_BOTTOM_OFFSET + 17 + tw)
	.endrepeat
PressStartTextBuffer_End:


;; Tables of sound effects to button display/press
LABEL ButtonSfxTable
	.byte	SnesGssSfx::BUTTON_X
	.byte	SnesGssSfx::BUTTON_A
	.byte	SnesGssSfx::BUTTON_B
	.byte	SnesGssSfx::BUTTON_Y


;; Tables of sound effects for each wrong button press
LABEL WrongButtonSfxTable
	.byte	SnesGssSfx::WRONG_X
	.byte	SnesGssSfx::WRONG_A
	.byte	SnesGssSfx::WRONG_B
	.byte	SnesGssSfx::WRONG_Y

