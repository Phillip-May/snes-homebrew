; Interrupt Handlers for Elevator Madness DX

.include "game.h"
.include "controler.h"
.include "routines/block.h"
.include "routines/screen.h"
.include "routines/random.h"

;; Blank Handlers
ROUTINE IrqHandler
	RTI

ROUTINE CopHandler
	RTI

ROUTINE VBlank
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

	Screen_VBlank
	MetaSprite_VBlank

	LDA	updateBgBufferOnZero
	IF_ZERO
		TransferToVramLocation interactiveBgBuffer, GAME_BG2_MAP, 32*32*2

		; A not Zero
		STA updateBgBufferOnZero
	ENDIF

	JSR	Controler__Update

	; Load State
	REP	#$30
	PLY
	PLX
	PLD
	PLB
	PLA

	RTI

