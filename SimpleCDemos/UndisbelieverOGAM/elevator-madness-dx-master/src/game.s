; Elevator Madness DX game loop.

.include "game.h"
.include "npcs.h"
.include "player.h"
.include "elevators.h"
.include "controler.h"

.include "routines/metasprite.h"
.include "routines/screen.h"
.include "routines/block.h"
.include "routines/math.h"
.include "routines/random.h"


.segment "SHADOW"
	WORD	interactiveBgBuffer, 32*32
	BYTE	updateBgBufferOnZero

.code


MODULE Game

.segment "SHADOW"
	BYTE	strikes
	WORD	score

	ADDR	strikeAntimationRoutinePtr

	WORD	strikeTimeout


.code

.A8
.I16
ROUTINE PlayGame
	; score = 0
	; strikes = 0
	;
	; SetupScreen()
	;
	; repeat
	;	InitForLife()
	;	Show Player
	;	Screen__FadeIn()
	;	GameLoop()
	;	StrikeAnimation()
	;
	;	strikes++
	;	DrawStrikes()
	;
	;	if Strikes == N_STRIKES
	;		break
	;	else
	;		Screen__FadeOut()
	;
	; Screen__SlowFadeOut()

	LDY	#0
	STY	score
	STZ	strikes

	JSR	SetupScreen

	REPEAT
		JSR	InitForLife

		; Show player
		JSR	MetaSprite__InitLoop
		JSR	Player__DrawSprites
		JSR	MetaSprite__FinalizeLoop

		JSR	Screen__FadeIn

		JSR	GameLoop

		JSR	StrikeAnimation

		INC	strikes
		JSR	DrawStrikes

		LDA	strikes
		CMP	#N_STRIKES
		BEQ	BREAK_LABEL

			JSR	Screen__FadeOut
	FOREVER

	LDA	#GAME_OVER_FADEOUT_SPEED
	JSR	Screen__SlowFadeOut

	RTS


.A8
.I16
ROUTINE InitForLife
	LDX	#0
	STX	strikeAntimationRoutinePtr

	MemClear	interactiveBgBuffer, .sizeof(interactiveBgBuffer)

	JSR	DrawScore
	JSR	DrawStrikes

	JSR	Player__Init
	JSR	Elevators__Init
	JSR	Npcs__Init

	STZ	updateBgBufferOnZero

	RTS



.A8
.I16
ROUTINE GameLoop
	REPEAT
		JSR	Screen__WaitFrame
		JSR	Random__AddJoypadEntropy

		JSR	MetaSprite__InitLoop

		JSR	Player__Process
		JSR	Elevators__Process
		JSR	Npcs__Process

		JSR	Player__DrawSprites
		JSR	Npcs__DrawSprites

		JSR	DrawScore

		STZ	updateBgBufferOnZero

		JSR	MetaSprite__FinalizeLoop

		LDX	strikeAntimationRoutinePtr
	UNTIL_NOT_ZERO

	RTS


.A8
.I16
ROUTINE StrikeAnimation
	; strikeTimeout = STRIKE_MIN_FRAMES
	; repeat
	;	StrikeAnimationFrame()
	;	strikeTimeout--
	; until strikeTimeout == 0
	;
	; strikeTimeout = STRIKE_TIMEOUT
	; repeat
	;	StrikeAnimationFrame()
	;	strikeTimeout--
	; until strikeTimeout == 0 || (Controler__pressed & JOY_BUTTONS) != 0

	LDY	#STRIKE_MIN_FRAMES
	REPEAT
		STY	strikeTimeout
		JSR	StrikeAnimationFrame

		LDY	strikeTimeout
		DEY
	UNTIL_ZERO

	LDY	#STRIKE_TIMEOUT
	REPEAT
		STY	strikeTimeout
		JSR	StrikeAnimationFrame

		REP	#$30
.A16
		LDA	Controler__pressed
		AND	#JOY_BUTTONS
		SEP	#$20
.A8
		BNE	BREAK_LABEL

		LDY	strikeTimeout
		DEY
	UNTIL_ZERO

	RTS


.A8
.I16
ROUTINE StrikeAnimationFrame
	; WaitFrame()
	; MetaSprite__InitLoop()
	;
	; *strikeAntimationRoutinePtr()
	; Player__DrawSprites()
	; Npcs__DrawSprites()
	;
	; MetaSprite__FinalizeLoop()

	JSR	Screen__WaitFrame

	JSR	MetaSprite__InitLoop

	PEA	_PlayGame_Return_ - 1
	JMP	(strikeAntimationRoutinePtr)
_PlayGame_Return_:

	JSR	Player__DrawSprites
	JSR	Npcs__DrawSprites

	STZ	updateBgBufferOnZero
	JMP	MetaSprite__FinalizeLoop


;; Displays the score on the foreground
.A8
.I16
ROUTINE DrawScore
	; y = score
	; for i = 0 to 4
	; 	y, x = y / 10, y % 10
	;	interactiveBgBuffer[SCORE_TILE_COLUMN * 32 * 2 + (SCORE_TILE_ROW + 3 - i)] = y + SCORE_DIGIT_DELTA | (SCORE_TILE_ATTR << 8)

	LDY	score

	.repeat 4, i
		LDA	#10
		JSR	Math__Divide_U16Y_U8A

		TXA
		ADD	#SCORE_DIGIT_DELTA
		STA	interactiveBgBuffer + SCORE_TILE_COLUMN * 32 * 2 + (SCORE_TILE_ROW + 3 - i) * 2

		LDA	#SCORE_TILE_ATTR
		STA	interactiveBgBuffer + SCORE_TILE_COLUMN * 32 * 2 + (SCORE_TILE_ROW + 3 - i) * 2 + 1
	.endrepeat

	RTS


;; Draws the Strike signs on the interactiveBgBuffer
.A8
.I16
ROUTINE DrawStrikes
	REP	#$30
.A16

	LDA	strikes
	AND	#$00FF
	ASL
	ASL
	ASL
	TAX

	FOR_Y	#0, INC2, #8
		LDA	StrikeTableTop, X
		STA	interactiveBgBuffer + STRIKES_TILE_COLUMN * 64 + STRIKES_TILE_ROW * 2, Y

		LDA	StrikeTableBottom, X
		STA	interactiveBgBuffer + (STRIKES_TILE_COLUMN + 1) * 64 + STRIKES_TILE_ROW * 2, Y
		INX
		INX
	NEXT

	SEP	#$20
.A8

	RTS



;; Sets up the Screen Registers and loads tiles, maps and palette to PPU
.A8
.I16
ROUTINE SetupScreen
	LDA	#INIDISP_FORCE
	STA	INIDISP

	LDA	#BGMODE_MODE2
	STA	BGMODE

	LDX	#0
	STX	BG1HOFS
	STX	BG1HOFS
	STX	BG2HOFS
	STX	BG2HOFS

	Screen_SetVramBaseAndSize GAME

	TransferToVramLocation	backgroundMap, GAME_BG1_MAP
	TransferToVramLocation	backgroundTiles, GAME_BG1_TILES
	TransferToCgramLocation	backgroundPalette, 0
	TransferToVramLocation	interactiveTiles, GAME_BG2_TILES
	TransferToCgramLocation	interactivePalette, 7 * 16

	TransferToVramLocation	playerTiles, GAME_OAM_TILES
	TransferToCgramLocation playerPalette, 8 * 16, 32

	TransferToVramLocation	strikeAnimationsTiles, GAME_OAM_TILES + 32 * 16
	TransferToCgramLocation strikeAnimationsPalette, 9 * 16, 32

	TransferToVramLocation businessManTiles, GAME_OAM_TILES + (512 - 32) * 16
	TransferToCgramLocation businessManPalette, 128 + 7 * 16

	LDA	#TM_BG1 | TM_BG2 | TM_BG3 | TM_OBJ
	STA	TM

	RTS


.rodata


LABEL	StrikeTableTop

; 0 Strikes
	.word	0
	.word	0
	.word	0
	.word	0

; 1 Strike
	.word	STRIKES_FULL_TOP_TILE | STRIKES_TILE_CHARATTR
	.word	0
	.word	0
	.word	0

; 2 Strikes
	.word	STRIKES_FULL_TOP_TILE | STRIKES_TILE_CHARATTR
	.word	STRIKES_HALF_TOP_TILE | STRIKES_TILE_CHARATTR
	.word	STRIKES_HALF_TOP_TILE | STRIKES_TILE_CHARATTR ^ TILEMAP_H_FLIP_FLAG
	.word	0

; 3 Strikes
	.word	STRIKES_FULL_TOP_TILE | STRIKES_TILE_CHARATTR
	.word	STRIKES_HALF_TOP_TILE | STRIKES_TILE_CHARATTR
	.word	STRIKES_HALF_TOP_TILE | STRIKES_TILE_CHARATTR ^ TILEMAP_H_FLIP_FLAG
	.word	STRIKES_FULL_TOP_TILE | STRIKES_TILE_CHARATTR



LABEL	StrikeTableBottom

; 0 Strikes
	.word	0
	.word	0
	.word	0
	.word	0

; 1 Strike
	.word	STRIKES_FULL_BOTTOM_TILE | STRIKES_TILE_CHARATTR
	.word	0
	.word	0
	.word	0

; 2 Strikes
	.word	STRIKES_FULL_BOTTOM_TILE | STRIKES_TILE_CHARATTR
	.word	STRIKES_HALF_BOTTOM_TILE | STRIKES_TILE_CHARATTR
	.word	STRIKES_HALF_BOTTOM_TILE | STRIKES_TILE_CHARATTR ^ TILEMAP_H_FLIP_FLAG
	.word	0

; 3 Strikes
	.word	STRIKES_FULL_BOTTOM_TILE | STRIKES_TILE_CHARATTR
	.word	STRIKES_HALF_BOTTOM_TILE | STRIKES_TILE_CHARATTR
	.word	STRIKES_HALF_BOTTOM_TILE | STRIKES_TILE_CHARATTR ^ TILEMAP_H_FLIP_FLAG
	.word	STRIKES_FULL_BOTTOM_TILE | STRIKES_TILE_CHARATTR

ENDMODULE

