; Elevator Madness DX Splash Screen.
;
; Displays the Splash screen,
;
; Primary used to prime the Random Number Generator

.include "splash.h"

.include "includes/registers.inc"
.include "routines/screen.h"
.include "routines/random.h"
.include "routines/block.h"

.include "controler.h"

.code


MODULE Splash

.segment "SHADOW"
	BYTE	screenBrightness
	WORD	timer


.code

.A8
.I16
ROUTINE SplashScreen
	; SetupScreen()
	; Load Tiles and map
	;
	; repeat
	;	force blank
	;	load splash screen
	;	a = FadeInOutScreen(SPLASH_TIMEOUT)
	;
	;	IF a == 0
	;		return
	;
	;	force blank
	;	load controls screen
	;	a = FadeInOutScreen(CONTROLS_TIMEOUT)

	JSR	SetupScreen

	REPEAT
		LDA	#INIDISP_FORCE
		STA	INIDISP
		TransferToVramLocation	splashMap, SPLASH_BG1_MAP
		TransferToVramLocation	splashTiles, SPLASH_BG1_TILES
		TransferToCgramLocation	splashPalette, 0

		LDX	#SPLASH_TIMEOUT
		JSR	FadeInOutScreen

		IF_ZERO
			RTS
		ENDIF


		LDA	#INIDISP_FORCE
		STA	INIDISP
		TransferToVramLocation	controlsMap, SPLASH_BG1_MAP
		TransferToVramLocation	controlsTiles, SPLASH_BG1_TILES
		TransferToCgramLocation	controlsPalette, 0

		LDX	#CONTROLS_TIMEOUT
		JSR	FadeInOutScreen
	FOREVER


; IN: X = frames to timeout
; OUT: z set if user pressed button, z clear if user doesn't
.A8
.I16
ROUTINE FadeInOutScreen
	; timer = X
	;
	; FadeIn()
	; repeat
	;	Screen__WaitFrame()
	;
	;	timer--
	;	if timer == 0
	;		FadeOut()
	;		return 1
	;		break
	;
	; until Controler__current & JOY_BUTTONS | JOY_START | JOY_SELECT != 0
	;
	; FadeOut()
	;
	; return 0

	STX	timer

	JSR	Screen__FadeIn

	REPEAT
		JSR	Screen__WaitFrame
		JSR	Random__AddJoypadEntropy

		LDX	timer
		DEX
		IF_ZERO
			JSR	Screen__FadeOut
			LDA	#1
			RTS
		ENDIF

		STX	timer

		REP	#$30
.A16
		LDA	Controler__current
		AND	#JOY_BUTTONS | JOY_START | JOY_SELECT

		SEP	#$20
.A8
	UNTIL_NOT_ZERO

	JSR	Screen__FadeOut

	LDA	#0
	RTS


; Sets up the game screen
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

	Screen_SetVramBaseAndSize SPLASH

	LDA	#TM_BG1
	STA	TM

	RTS

ENDMODULE

