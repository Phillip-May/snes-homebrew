; NPCs
;
; This code uses the DP register to select which NPC is processed.

.include "game.h"
.include "npcs.h"
.include "elevators.h"

.include "routines/math.h"
.include "routines/block.h"
.include "routines/metasprite.h"
.include "routines/random.h"

MODULE Npcs

.struct NpcStruct
	;; Current state of the elevator
	state			.word

	;; The side of the screen the NPC is on.
	;; If 0, then on left, if $FF or 1 on right
	leftSideOnZero		.byte

	;; The current floor the NPC is on
	floor			.byte

	;; The current floor the NPC wants to go to
	targetFloor		.byte

	;; Position
	;; Signed 16.8 fractional integer
	xPos			.res 3
	yPos			.res 3

	;; Frame counter for animations
	;; word because it is used in 16 bit Mode.
	animationCounter	.byte

	;; The direction the NPC is facing
	;; If 0, then on left, if 1 on right
	facingLeftOnZero	.byte

	;; character's frame
	;; 0 = stand, 1 = walk, 2 = wait
	frame			.byte

	;; Current feelings
	;; (normal, stressed, angry)
	feelings		.byte

	;; Timeout to next feeling change
	feelingsTimeout		.word

	;; MetaSprite charAttr diff word
	spriteCharAttr		.word

	;; Location of the MetaSprite Table.
	spriteFrameTablePtr	.addr

	;; MetaSprite Frame Location
	spriteFramePtr		.word

	;; NPC directly in front of this one. 0 If at head of line
	nextNpcInLine		.addr

	;; Frame timer for enter/exit elevator state
	stateTimer		.byte
.endstruct


.segment "SHADOW"
	STRUCT	npcs, NpcStruct, N_NPCS

	;; A pointer to the next free NPC
	ADDR	nextFreeNpcPtr

	;; A frame countdown to the next NPC sprite
	SINT16	countdownToNextNpc

	;; Subtracted from each `npc.feelingsTimeout` every frame
	;; Increases every point, adds difficulty curve
	SINT16	feelingsSpeed


	;; Strike Animation variables

	;;; animation frame number (as a muliple of 2)
	ADDR	strike_frame
	;;; current frame number
	BYTE	strike_frameTimeout
	;;; The NPC the strike is for
	WORD	strike_npc
	;;; If 0 then hide NPCs arrow
	BYTE	strike_hideArrowsOnZero

.rodata
LABEL NpcStateTable
	.addr 0
	.addr ContinueWalkToElevator
	.addr ContinueEnterElevator
	.addr ContinueWaitDoorClose
	.addr ContinueLimbo
	.addr ContinueWaitDoorOpen
	.addr ContinueExitElevator
	.addr ContinueWalkOffscreen
.code

.A8
.I16
ROUTINE Init
	;; ::BUGFIX previous games NPCs still visible ::
	;; ::: Have ABSOLUTELY NO CLUE why the old code didn't work. ::
	;; ::: Just gave up after 20 minutes and clear all the memory associated with NPCs ::
	MemClear npcs

	LDX	#NPC_FEELINGS_SPEED
	STX	feelingsSpeed

	LDX	#1
	STX	countdownToNextNpc

	RTS


; inline routine that draws the NPC to the screen
; dp = npc
.macro Process_DrawNpc
.endmacro


.A8
.I16
ROUTINE DrawSprites
	; for dp in npcs
	;	if dp.state != NPC_DEAD && dp.state != NPC_LIMBO
	;		MetaSprite__ProcessMetaSprite_Y(npc.xpos, npc.ypos, npc.spriteCharAttr, npc.spriteFramePtr)

	PHD

	REP	#$30
.A16
	LDA	#npcs

	REPEAT
		TCD

		.assert NPC_DEAD = 0, error, "NPC_DEAD != 0"

		LDX	NpcStruct::state
		IF_NOT_ZERO
			LDX	NpcStruct::state
			CPX	#NPC_LIMBO
			IF_NE
				SEP	#$20
.A8
				LDY	NpcStruct::xPos + 1
				STY	MetaSprite__xPos

				LDY	NpcStruct::yPos + 1
				STY	MetaSprite__yPos

				LDY	NpcStruct::spriteCharAttr
				LDX	NpcStruct::spriteFramePtr

				JSR	MetaSprite__ProcessMetaSprite_Y

				REP	#$30
			ENDIF
.A16

		ENDIF

		TDC
		ADD	#.sizeof(NpcStruct)

		CMP	#npcs + N_NPCS * .sizeof(NpcStruct)
	UNTIL_GE

	SEP	#$20
.A8
	PLD
	RTS



.A8
.I16
ROUTINE Process
	; for dp in npcs
	;	if dp.state == NPC_DEAD
	;		nextFreeNpcPtr = dp
	;	else
	;		NpcStateTable[dp.state](dp)
	;
	; if --countdownToNextNpc == 0
	;	Spawn()
	;	contdownToNextNpc = random(MIN_SPAWN_COUNTDOWN, MAX_SPAWN_COUNTDOWN)

	PHD

	REP	#$30
.A16
.I16
	LDA	#npcs

	REPEAT
		TCD

		.assert NPC_DEAD = 0, error, "NPC_DEAD != 0"

		LDX	NpcStruct::state
		IF_ZERO
			STA	nextFreeNpcPtr
		ELSE
			SEP	#$20
.A8
			JSR	(.loword(NpcStateTable), X)

			REP	#$30
		ENDIF
.A16

		TDC
		ADD	#.sizeof(NpcStruct)

		CMP	#npcs + N_NPCS * .sizeof(NpcStruct)
	UNTIL_GE

.A16
	DEC	countdownToNextNpc
	SEP	#$20
.A8
	IF_ZERO
		; subtraction overflow
		JSR	Spawn

		LDX	#MIN_SPAWN_COUNTDOWN
		LDY	#MAX_SPAWN_COUNTDOWN
		JSR	Random__Rnd_U16X_U16Y

		STY	countdownToNextNpc
	ENDIF

	PLD
	RTS


; A = elevator floor
; X = npc
.A8
.I16
ROUTINE OccupiedElevatorDoorOpening
	; x->yPos = floor * ELEVATOR_FLOOR_COLUMN_SPACING * 8 + NPC_ELEVATOR_YPOS
	; x->state = NPC_WAIT_DOOR_OPEN

	PHX
	TAY
	LDX	#ELEVATOR_FLOOR_COLUMN_SPACING * 8
	JSR	Math__Multiply_U8Y_U8X_UY
	TYA
	ADD	#NPC_ELEVATOR_YPOS
	PLX
	STA	a:NpcStruct::yPos + 1, X

	LDA	#NPC_WAIT_DOOR_OPEN
	STA	a:NpcStruct::state, X

	RTS



; A = elevator floor
; X = npc
.A8
.I16
ROUTINE OccupiedElevatorOpen
	; npc = X
	; if npc.targetFloor == elevatorFloor
	;	SetExitElevator()
	; else
	;	SetWrongFloor()

	PHD
	PHA

	REP	#$30
.A16
	TXA
	TCD

	SEP	#$20
.A8

	PLA
	CMP	NpcStruct::targetFloor
	IF_EQ
		JSR	SetExitElevator
	ELSE
		JSR	SetWrongFloor
	ENDIF

	PLD
	RTS




; OUTPUT DP = new NPC
.A8
.I16
ROUTINE Spawn
	; if !nextFreeNpc
	;	return
	;
	; npc = nextFreeNpcPtr
	; nextFreeNpcPtr = 0
	;
	;
	; npc.floor = Random_Rnd(4)
	; npc.targetFloor = (npc.floor + Random_Rnd(3) + 1) AND 3
	;
	; npc.leftSideOnZero = Random_Rnd(2)
	; npc.facingLeftOnZero = npc.leftSideOnZero
	; if leftSideOnZero == 0
	;	npc.xPos = NPC_LEFT_XSTART
	; else
	;	npc.xPos = NPC_RIGHT_XSTART
	; npc.yPos = npc.floor * ELEVATOR_FLOOR_COLUMN_SPACING * 8 + NPC_YSTART
	; npc.animationCounter = NPC_MOVE_FRAME_DELAY
	; npc.frame = 0
	; npc.feelings = 0
	; npc.feelingsTimeout = NPC_FEELINGS_TIMEOUT
	;
	; npc.spriteCharAttr = businessManCharAttr
	; npc.spriteTablePtr = MetaSprite_businessMan
	; CalculateSpriteFramePtr()
	;
	; // determine the next sprite in line
	; y = 0 // best so far.
	; for x in npcs
	;	if x->state == NPC_WALK_TO_ELEVATOR && x->side == npc.side && x->floor == npc.floor
	;		if y == 0
	;			y = x
	;		else if leftSideOnZero == 0
	;			if x->xPos < y->xPos
	;				y = x
	;		else
	;			if x->xPos >= y->xPos
	;				y = x
	; npc.nextNpcInLine = y
	;
	; // set state last so it doesn't interfear with with search above.
	; npc.state = NPC_WALK_TO_ELEVATOR

	REP	#$30
.A16
	LDA	nextFreeNpcPtr
	IF_ZERO
		SEP	#$20
		RTS
	ENDIF

	TCD

	STZ	nextFreeNpcPtr

	SEP	#$20
.A8

	.assert N_FLOORS = 4, error, "Bad value"

	JSR	Random__Rnd_4
	STA	NpcStruct::floor

	JSR	Random__Rnd_3
	SEC
	ADC	NpcStruct::floor
	AND	#$03
	STA	NpcStruct::targetFloor

	JSR	Random__Rnd_2
	IF_NOT_ZERO
		LDA	#$FF
	ENDIF
	STA	NpcStruct::leftSideOnZero
	STA	NpcStruct::facingLeftOnZero

	LDA	NpcStruct::leftSideOnZero
	IF_ZERO
		LDY	#.loword(NPC_LEFT_XSTART)
	ELSE
		LDY	#.loword(NPC_RIGHT_XSTART)
	ENDIF
	STY	NpcStruct::xPos + 1
	STZ	NpcStruct::xPos

	LDX	#ELEVATOR_FLOOR_COLUMN_SPACING * 8
	LDY	NpcStruct::floor
	JSR	Math__Multiply_U8Y_U8X_UY
	TYA
	ADD	#NPC_YSTART

	STA	NpcStruct::yPos + 1
	STZ	NpcStruct::yPos + 2
	STZ	NpcStruct::yPos + 0

	LDA	#NPC_MOVE_FRAME_DELAY
	STA	NpcStruct::animationCounter

	STZ	NpcStruct::frame
	STZ	NpcStruct::feelings

	LDY	#NPC_FEELINGS_TIMEOUT
	STY	NpcStruct::feelingsTimeout

	LDY	Npcs_SpriteCharAttr
	STY	NpcStruct::spriteCharAttr
	LDX	Npcs_SpriteFrameTablePtr
	STX	NpcStruct::spriteFrameTablePtr

	JSR	CalculateSpriteFramePtr


	; determine next sprite in line
	LDY	#0
	LDX	#npcs
	REPEAT
.A8
		LDA	a:NpcStruct::state, x
		CMP	#NPC_WALK_TO_ELEVATOR
		IF_EQ
			LDA	a:NpcStruct::leftSideOnZero, X
			CMP	NpcStruct::leftSideOnZero
			IF_EQ
				LDA	a:NpcStruct::floor, X
				CMP	NpcStruct::floor
				IF_EQ
					CPY	#0
					IF_EQ
						TXY
					ELSE
						LDA	a:NpcStruct::leftSideOnZero, X
						REP	#$20
.A16
						IF_ZERO
							; on left
							LDA	a:NpcStruct::xPos + 1, X
							CMP	a:NpcStruct::xPos + 1, Y
							IF_MINUS
								TXY
							ENDIF
						ELSE
							; on right
							LDA	a:NpcStruct::xPos + 1, X
							CMP	a:NpcStruct::xPos + 1, Y
							IF_PLUS
								TXY
							ENDIF
						ENDIF
						SEP	#$20
.A8
					ENDIF
				ENDIF
			ENDIF
		ENDIF

		REP	#$30
.A16
		TXA
		ADD	#.sizeof(NpcStruct)
		TAX
		SEP	#$20
.A8
		CPX	#npcs + N_NPCS * .sizeof(NpcStruct)
	UNTIL_GE
	STY	NpcStruct::nextNpcInLine

	LDY	#NPC_WALK_TO_ELEVATOR
	STY	NpcStruct::state

	RTS




; DP = npc
.A8
.I16
ROUTINE ContinueWalkToElevator
	; if npc.leftSideOnZero == 0
	;	nextNpcInLine = npc.nextNpcInLine
	;	if nextNpcInLine != 0 && nextNpcInLine->state != NPC_WALK_TO_ELEVATOR
	;		// now at the front of line
	;		npc.nextNpcInLine = 0
	;	if npc.nextNpcInLine == 0 || npc.xPos + NPC_LINE_SPACING < nextNpcInLine.xPos
	;		// no NPC directly in front of us
	;		if npc.xPos < NPC_LEFT_WAIT_POS
	;			npc.xPos += NPC_WALK_SPEED
	;			ProcessWalkAnimation()
	;			return
	;		else
	;			c = Elevators__NpcEnterElevator(npc, npc.floor, npc.leftSideOnZero)
	;			if c
	;				SetEnterElevator()
	; else
	;	// on right side
	;	nextNpcInLine = npc.nextNpcInLine
	;	if nextNpcInLine != 0 && nextNpcInLine->state != NPC_WALK_TO_ELEVATOR
	;		// now at the front of line
	;		npc.nextNpcInLine = 0
	;	if npc.nextNpcInLine == 0 || npc.xPos - NPC_LINE_SPACING > nextNpcInLine.xPos
	;		// no NPC directly in front of us
	;		if npc.xPos > NPC_RIGHT_WAIT_POS
	;			npc.xPos -= NPC_WALK_SPEED
	;			ProcessWalkAnimation()
	;			return
	;		else
	;			c =  Elevators__NpcEnterElevator(npc, npc.floor, npc.leftSideOnZero)
	;			if c
	;				SetEnterElevator()
	;
	;	WaitingInLine()

	LDA	NpcStruct::leftSideOnZero
	IF_ZERO
		; Check if in line
		LDX	NpcStruct::nextNpcInLine
		BEQ	ContinueWalkToElevator_SkipLeftLineCheck

		; Check if in the NPC in front of us has left the line
		LDY	a:NpcStruct::state, X
		CPY	#NPC_WALK_TO_ELEVATOR
		IF_NE
			LDX	#0
			STX	NpcStruct::nextNpcInLine
			BRA	ContinueWalkToElevator_SkipLeftLineCheck
		ENDIF

		REP	#$30
.A16
		; remember xPos is a 16.8 fractional integer

		LDA	NpcStruct::xPos + 1
		ADD	#NPC_LINE_SPACING
		CMP	a:NpcStruct::xPos + 1, X
		IF_MINUS
ContinueWalkToElevator_SkipLeftLineCheck:
			REP	#$30
.A16
			; There is no other NPC directly in front of us.

			LDA	NpcStruct::xPos + 1
			CMP	#NPC_LEFT_WAIT_XPOS
			IF_MINUS
				LDA	NpcStruct::xPos
				ADD	#NPC_WALK_SPEED
				STA	NpcStruct::xPos

				SEP	#$20
.A8
				LDA	NpcStruct::xPos + 2
				ADC	#0
				STA	NpcStruct::xPos + 2

				JMP	ProcessWalkAnimation
			ENDIF
.A16
			SEP	#$20
			CLC
.A8
			LDA	NpcStruct::floor
			JSR	Elevators__NpcEnterElevator
			IF_C_SET
				JMP	SetEnterElevator
			ENDIF
		ENDIF

		SEP	#$20
.A8
	ELSE
		; on right side
		; Check if in line
		LDX	NpcStruct::nextNpcInLine
		BEQ	ContinueWalkToElevator_SkipRightLineCheck

		; Check if in the NPC in front of us has left the line
		LDY	a:NpcStruct::state, X
		CPY	#NPC_WALK_TO_ELEVATOR
		IF_NE
			LDX	#0
			STX	NpcStruct::nextNpcInLine
			BRA	ContinueWalkToElevator_SkipRightLineCheck
		ENDIF

		REP	#$30
.A16
		; remember xPos is a 16.8 fractional integer

		LDA	NpcStruct::xPos + 1
		SUB	#NPC_LINE_SPACING + 1
		CMP	a:NpcStruct::xPos + 1, X
		IF_PLUS
ContinueWalkToElevator_SkipRightLineCheck:
			REP	#$30
.A16
			; There is no other NPC directly in front of us.

			LDA	NpcStruct::xPos + 1
			CMP	#NPC_RIGHT_WAIT_XPOS + 1
			IF_PLUS
				LDA	NpcStruct::xPos
				SUB	#NPC_WALK_SPEED
				STA	NpcStruct::xPos

				SEP	#$20
.A8
				LDA	NpcStruct::xPos + 2
				SBC	#0
				STA	NpcStruct::xPos + 2

				JMP	ProcessWalkAnimation
			ENDIF
.A16
			SEP	#$21	; c set if on right
.A8
			LDA	NpcStruct::floor
			JSR	Elevators__NpcEnterElevator
			IF_C_SET
				JMP	SetEnterElevator
			ENDIF
		ENDIF

		SEP	#$20
.A8
	ENDIF

	.assert * = WaitingInLine, lderror, "Bad Flow"


; DP = npc
.A8
.I16
ROUTINE WaitingInLine
	; // waiting
	; if npc.feelings != NPC_FEELING_FIGHTING
	;	npc.feelingsTimeout -= feelingsSpeed
	; 	if npc.feelingsTimeout < 0
	;		npc.feelings++
	;		npc.feelingTimeout = NPC_FEELINGS_TIMEOUT
	;		CalculateSpriteFramePtr()
	; else
	;	if npc.nextNpcInLine
	;		SetFighting()

	JSR	ProcessWaitAnimation

	LDA	NpcStruct::feelings
	CMP	#NPC_FEELINGS_FIGHTING
	IF_NE
		REP	#$30
.A16
		LDA	NpcStruct::feelingsTimeout
		SUB	feelingsSpeed
		STA	NpcStruct::feelingsTimeout

		SEP	#$20
.A8
		IF_C_CLEAR
			; subtraction underflow
			INC	NpcStruct::feelings

			LDY	#NPC_FEELINGS_TIMEOUT
			STY	NpcStruct::feelingsTimeout

			JMP	CalculateSpriteFramePtr
		ENDIF
	ELSE
		LDX	NpcStruct::nextNpcInLine
		IF_NOT_ZERO
			JMP	SetFighting
		ENDIF
	ENDIF

	RTS



; DP = npc
.A8
.I16
ROUTINE SetEnterElevator
	; npc.state = NPC_EXIT_ELEVATOR
	; npc.stateTimer = NPC_ENTER_ELEVATOR_FRAMES
	; npc.DrawArrows()

	LDX	#NPC_ENTER_ELEVATOR
	STX	NpcStruct::state

	LDA	#NPC_ENTER_ELEVATOR_FRAMES
	STA	NpcStruct::stateTimer

	JMP	DrawArrows


; DP = npc
.A8
.I16
ROUTINE ContinueEnterElevator
	; if --npc.stateTimer == 0
	;	SetWaitDoorClose()
	; else
	;	if npc.leftSideOnZero == 0
	;		npc.xPos += NPC_ENTER_ELEVATOR_XSPEED
	;	else
	;		npc.xPos -= NPC_ENTER_ELEVATOR_XSPEED
	;	npc.yPos -= NPC_ENTER_ELEVATOR_YSPEED
	;	ProcessWalkAnimation()

	DEC	NpcStruct::stateTimer
	BEQ	SetWaitDoorClose

	LDA	NpcStruct::leftSideOnZero
	IF_ZERO
		; left

		REP	#$30
.A16

		LDA	NpcStruct::xPos
		ADD	#NPC_ENTER_ELEVATOR_XSPEED
		STA	NpcStruct::xPos

		SEP	#$20
.A8
		LDA	NpcStruct::xPos + 2
		ADC	#0
		STA	NpcStruct::xPos + 2
	ELSE
		; right

		REP	#$30
.A16

		LDA	NpcStruct::xPos
		SUB	#NPC_ENTER_ELEVATOR_XSPEED
		STA	NpcStruct::xPos

		SEP	#$20
.A8
		LDA	NpcStruct::xPos + 2
		SBC	#0
		STA	NpcStruct::xPos + 2
	ENDIF

	REP	#$30
.A16

	LDA	NpcStruct::yPos
	SUB	#NPC_ENTER_ELEVATOR_YSPEED
	STA	NpcStruct::yPos

	SEP	#$20
.A8
	LDA	NpcStruct::yPos + 2
	SBC	#0
	STA	NpcStruct::yPos + 2

	JMP	ProcessWalkAnimation



; DP = npc
.A8
.I16
ROUTINE SetWaitDoorClose
	; npc.state = NPC_WAIT_DOOR_CLOSE
	; set npc.spriteCharAttr Order to 2 (behind doors)
	; npc.facingLeftOnZero = !npc.leftSideOnZero
	; npc.frame = 0
	; npc.feelingsTimeout = 0xFFFF
	; CalculateSpriteFramePtr()
	;
	; Elevators__NpcInsideElevator(npc, npc.leftSideOnZero)

	LDX	#NPC_WAIT_DOOR_CLOSE
	STX	NpcStruct::state

	; Change Order of MetaSprite so it is behind elevator doors
	LDA	NpcStruct::spriteCharAttr + 1
	AND	#.lobyte(~OAM_ATTR_ORDER_MASK)
	ORA	#1 << OAM_ATTR_ORDER_SHIFT
	STA	NpcStruct::spriteCharAttr + 1

	LDA	NpcStruct::leftSideOnZero
	EOR	#$FF
	STA	NpcStruct::facingLeftOnZero

	STZ	NpcStruct::frame

	LDY	#$FFFF
	STY	NpcStruct::feelingsTimeout

	JSR	CalculateSpriteFramePtr

	LDA	NpcStruct::leftSideOnZero
	ASL
	JMP	Elevators__NpcInsideElevator


; DP = npc
.A8
.I16
ROUTINE	ContinueWaitDoorClose
	JMP	WaitingInLine



; DP = npc
.A8
.I16
ROUTINE ContinueWaitDoorOpen
ROUTINE	ContinueLimbo
	RTS



; A = floor
; DP = npc
.A8
.I16
ROUTINE SetExitElevator
	; x->state = NPC_EXIT_ELEVATOR
	; x->stateTimer = NPC_EXIT_ELEVATOR_FRAMES
	; x->animationCounter = 1
	; set x->spriteCharAttr Order to 3 (in front of everything)
	; npc.ClearArrows()

	LDA	#NPC_EXIT_ELEVATOR
	STA	NpcStruct::state

	LDA	#NPC_EXIT_ELEVATOR_FRAMES
	STA	NpcStruct::stateTimer

	LDA	#1
	STA	NpcStruct::animationCounter

	; Make sprite in front of everything.
	LDA	NpcStruct::spriteCharAttr + 1
	ORA	#3 << OAM_ATTR_ORDER_SHIFT
	STA	NpcStruct::spriteCharAttr + 1

	JMP	ClearArrows



; DP = npc
.A8
.I16
ROUTINE ContinueExitElevator
	; if --npc.stateTimer == 0
	;	SetWalkOffscreen()
	; else
	;	if npc.leftSideOnZero == 0
	;		npc.xPos -= NPC_EXIT_ELEVATOR_XSPEED
	;	else
	;		npc.xPos += NPC_EXIT_ELEVATOR_XSPEED
	;	npc.yPos += NPC_EXIT_ELEVATOR_YSPEED
	;	ProcessWalkAnimation()

	DEC	NpcStruct::stateTimer
	BEQ	SetWalkOffscreen

	LDA	NpcStruct::leftSideOnZero
	IF_ZERO
		REP	#$30
.A16
		LDA	NpcStruct::xPos
		SUB	#NPC_EXIT_ELEVATOR_XSPEED
		STA	NpcStruct::xPos

		SEP	#$20
.A8
		LDA	NpcStruct::xPos + 2
		SBC	#0
		STA	NpcStruct::xPos + 2
	ELSE
		REP	#$30
.A16
		LDA	NpcStruct::xPos
		ADD	#NPC_EXIT_ELEVATOR_XSPEED
		STA	NpcStruct::xPos

		SEP	#$20
.A8
		LDA	NpcStruct::xPos + 2
		ADC	#0
		STA	NpcStruct::xPos + 2
	ENDIF

	REP	#$30
.A16

	LDA	NpcStruct::yPos
	ADD	#NPC_EXIT_ELEVATOR_YSPEED
	STA	NpcStruct::yPos

	SEP	#$20
.A8
	LDA	NpcStruct::yPos + 2
	ADD	#0
	STA	NpcStruct::yPos + 2

	JMP	ProcessWalkAnimation



; DP = npc
.A8
.I16
ROUTINE	SetWalkOffscreen
	LDX	#NPC_WALK_OFFSCREEN
	STX	NpcStruct::state

	RTS


.A8
.I16
ROUTINE ContinueWalkOffscreen
	; if npc.leftSideOnZero == 0
	;	// on left side, walking right
	;	if npc.xPos >= NPC_LEFT_OFFSCREEN_XPOS
	;		npc.xPos -= NPC_WALK_OFFSCREEN_SPEED
	;		ProcessWalkAnimation()
	;		return
	;	else
	;		NpcOffscreen()
	; else
	;	// on right side, walking left
	;	if npc.xPos < NPC_RIGHT_OFFSCREEN_XPOS
	;		npc.xPos += NPC_WALK_OFFSCREEN_SPEED
	;		ProcessWalkAnimation()
	;		return
	;	else
	;		NpcOffscreen()

	LDA	NpcStruct::leftSideOnZero
	IF_ZERO
		REP	#$30
.A16
		LDA	NpcStruct::xPos + 1
		CMP	#.loword(NPC_LEFT_OFFSCREEN_XPOS)
		IF_PLUS
			LDA	NpcStruct::xPos
			SUB	#NPC_WALK_OFFSCREEN_SPEED
			STA	NpcStruct::xPos

			SEP	#$20
.A8
			LDA	NpcStruct::xPos + 2
			SBC	#0
			STA	NpcStruct::xPos + 2

			JMP	ProcessWalkAnimation
		ENDIF
.A16
		SEP	#$20
.A8
		BRA	NpcOffscreen
	ELSE
		REP	#$30
.A16
		LDA	NpcStruct::xPos + 1
		CMP	#.loword(NPC_RIGHT_OFFSCREEN_XPOS + 1)
		IF_MINUS
			LDA	NpcStruct::xPos
			ADD	#NPC_WALK_OFFSCREEN_SPEED
			STA	NpcStruct::xPos

			SEP	#$20
.A8
			LDA	NpcStruct::xPos + 2
			ADC	#0
			STA	NpcStruct::xPos + 2

			JMP	ProcessWalkAnimation
		ENDIF
.A16
		SEP	#$20
.A8
		BRA	NpcOffscreen
	ENDIF


; DP = npc
.A8
.I16
ROUTINE SetWrongFloor
	; strike_npc = npc
	; strike_frameTimeout = 1
	; strike_hideArrowsOnZero = 0
	; if npc.leftSideOnZero == 0
	;	strike_frame = Random__Rnd_2() == 0 ? MetaSprite_wrongFloor0_left : MetaSprite_wrongFloor1_left
	; else
	;	strike_frame = Random__Rnd_2() == 0 ? MetaSprite_wrongFloor0_right : MetaSprite_wrongFloor1_right
	;
	; Game__strikeAntimationRoutinePtr = StrikeAnimationWrongFloor

	TDC
	STA	strike_npc
	XBA
	STA	strike_npc + 1

	LDA	#1
	STA	strike_frameTimeout

	STZ	strike_hideArrowsOnZero

	LDA	NpcStruct::leftSideOnZero
	IF_ZERO
		JSR	Random__Rnd_2
		IF_ZERO
			LDX	#.loword(MetaSprite_wrongFloor0_left)
		ELSE
			LDX	#.loword(MetaSprite_wrongFloor1_left)
		ENDIF
	ELSE
		JSR	Random__Rnd_2
		IF_ZERO
			LDX	#.loword(MetaSprite_wrongFloor0_right)
		ELSE
			LDX	#.loword(MetaSprite_wrongFloor1_right)
		ENDIF
	ENDIF

	STX	strike_frame

	LDX	#.loword(StrikeAnimationWrongFloor)
	STX	Game__strikeAntimationRoutinePtr

	RTS



; DP = npc
.A8
.I16
ROUTINE SetFighting
	; // hide the two sprites
	; npc.nextNpcInLine->state = NPC_DEAD
	; npc.state = NPC_DEAD
	;
	; strike_npc = npc.nextNpcInLine
	; strike_frame = 0
	; strike_frameTimeout = FIGHTING_FRAME_DELAY
	;
	; Game__strikeAntimationRoutinePtr = StrikeAnimationFighting

	LDX	NpcStruct::nextNpcInLine
	STZ	a:NpcStruct::state, X
	STZ	NpcStruct::state

	STX	strike_npc

	LDY	#0
	STY	strike_frame

	LDA	#FIGHTING_FRAME_DELAY
	STA	strike_frameTimeout

	LDX	#.loword(StrikeAnimationFighting)
	STX	Game__strikeAntimationRoutinePtr

	RTS



; DP = npc
.A8
.I16
ROUTINE NpcOffscreen
	; npc.state = NPC_DEAD
	; game.score++
	; if feelingsSpeed < NPC_MAX_FEELINGS_SPEED
	;	feelingsSpeed++
	;
	; play(AWARD_POINT_SOUND)

	.assert NPC_DEAD = 0, error, "NPC_DEAD != 0"
	STZ	NpcStruct::state

	REP	#$30
.A16

	INC	Game__score

	LDA	feelingsSpeed
	CMP	#NPC_MAX_FEELINGS_SPEED
	IF_LT
		INC
		STA	feelingsSpeed
	ENDIF

	;; ::SOUND award point::

	RTS



;; Animates the NPCs wrong floor animation
;; Called once per frame
.A8
.I16
ROUTINE StrikeAnimationWrongFloor
	; strike_frameTimeout--
	; if strike_frameTimeout == 0
	;	strike_frameTimeout = WRONG_FLOOR_ARROW_DELAY
	;	npc = strike_npc
	;	if strike_hideArrowsOnZero == 0
	;		ClearArrows(npc)
	;		strike_hideArrowsOnZero = 1
	;	else
	;		DrawArrows(npc)
	;		strike_hideArrowsOnZero = 0
	;
	;	MetaSprite__ProcessMetaSprite_Y(strike_npc->xPos, strike_npc->yPos, FIGHTING_CHARATTR, strike_frame)

	DEC	strike_frameTimeout
	IF_ZERO
		LDA	#WRONG_FLOOR_ARROW_DELAY
		STA	strike_frameTimeout

		PHD
		LDA	strike_npc + 1
		XBA
		LDA	strike_npc
		TCD

		LDA	strike_hideArrowsOnZero
		IF_ZERO
			JSR	ClearArrows
			INC	strike_hideArrowsOnZero
		ELSE
			JSR	DrawArrows
			STZ	strike_hideArrowsOnZero
		ENDIF

		PLD
	ENDIF

	LDX	strike_npc
	LDA	a:NpcStruct::xPos + 1, X
	STA	MetaSprite__xPos

	LDA	a:NpcStruct::yPos + 1, X
	STA	MetaSprite__yPos

	LDY	#FIGHTING_CHARATTR
	LDX	strike_frame

	JMP	MetaSprite__ProcessMetaSprite_Y




;; Animates the NPC fighting animation.
;; Called once per frame
.A8
.I16
ROUTINE StrikeAnimationFighting
	; strike_frameTimeout--
	; if strike_frameTimeout == 0
	;	strike_frameTimeoute = FIGHTING_FRAME_DELAY
	;	if strike_frame >= (N_FIGHTING_FRAMES - 1) * 2
	;		strike_frame = 0
	;	else
	;		strike_frame += 2
	;
	;	MetaSprite__ProcessMetaSprite_Y(strike_npc->xPos, strike_npc->yPos, FIGHTING_CHARATTR, MetaSprite_fightingCloud[strike_frame])

	DEC	strike_frameTimeout
	IF_ZERO
		LDA	#FIGHTING_FRAME_DELAY
		STA	strike_frameTimeout

		LDY	strike_frame
		CPY	#(N_FIGHTING_FRAMES - 1) * 2
		IF_GE
			LDY	#0
		ELSE
			INY
			INY
		ENDIF

		STY	strike_frame
	ENDIF

	LDX	strike_npc
	LDA	a:NpcStruct::xPos + 1, X
	STA	MetaSprite__xPos

	LDA	a:NpcStruct::yPos + 1, X
	STA	MetaSprite__yPos

	LDY	#FIGHTING_CHARATTR

	REP	#$30
.A16

	LDX	strike_frame
	LDA	f:MetaSprite_fightingCloud, X
	TAX

	SEP	#$20
.A8

	JMP	MetaSprite__ProcessMetaSprite_Y



;; Processes the sprite's walk animation
;; DP = npc
.A8
.I16
ROUTINE ProcessWalkAnimation
	; if --npc.animationCounter == 0
	;	npc.animationCounter = NPC_MOVE_FRAME_DELAY
	;	npc.frame = npc.frame == NPC_FRAME_STAND ? NPC_FRAME_WALK : NPC_FRAME_STAND
	;	CalculateSpriteFramePtr()

	DEC	NpcStruct::animationCounter
	IF_ZERO
		LDA	#NPC_MOVE_FRAME_DELAY
		STA	NpcStruct::animationCounter

		LDA	NpcStruct::frame
		.assert NPC_FRAME_STAND = 0, error, "Bad variable"
		IF_ZERO
			LDA	#NPC_FRAME_WALK
		ELSE
			LDA	#NPC_FRAME_STAND
		ENDIF
		STA	NpcStruct::frame

		BRA	CalculateSpriteFramePtr
	ENDIF

	RTS



;; Processes the sprite's wait animation
;; DP = npc
.A8
.I16
ROUTINE ProcessWaitAnimation
	; if --npc.animationCounter == 0
	;	npc.animationCounter = NPC_WAIT_FRAME_DELAY
	;	npc.frame = npc.frame == NPC_FRAME_WAIT : NPC_FRAME_STAND ? NPC_FRAME_WAIT
	;	CalculateSpriteFramePtr()

	DEC	NpcStruct::animationCounter
	IF_ZERO
		LDA	#NPC_MOVE_FRAME_DELAY
		STA	NpcStruct::animationCounter

		LDA	NpcStruct::frame
		CMP	#NPC_FRAME_WAIT
		IF_EQ
			LDA	#NPC_FRAME_STAND
		ELSE
			LDA	#NPC_FRAME_WAIT
		ENDIF
		STA	NpcStruct::frame

		BRA	CalculateSpriteFramePtr
	ENDIF

	RTS


;; Calculates the value of `spriteFramePtr` depending on `frame`, `feelings`, `leftSideOnZero` and `spriteFrameTablePtr`
;; DP = npc
.A8
.I16
ROUTINE	CalculateSpriteFramePtr
	; x = npc.facingLeftOnZero == 0 ? 0 : 12
	; x = (start + npc.frame + npc.feelings * 3) * 2 + npc.spriteFrameTablePtr
	; npc.spriteFramePtr = MetaSpriteBank[spriteFrameTablePtr]

	LDA	NpcStruct::facingLeftOnZero
	IF_ZERO
		LDA	#0
	ELSE
		LDA	#12
	ENDIF

	; ::HACK frame and feelings < 4, will not overflow::

	ADD	NpcStruct::frame
	ADC	NpcStruct::feelings
	ADC	NpcStruct::feelings
	ADC	NpcStruct::feelings

	REP	#$30
.A16
	AND	#$00FF
	ASL
	ADD	NpcStruct::spriteFrameTablePtr
	TAX
	LDA	f:.bankbyte(MetaSpriteFrameTable_npcs) << 16, X
	STA	NpcStruct::spriteFramePtr

	SEP	#$20
.A8

	RTS



;; Draws The Arrows of the NPCs request to the Player
; dp = npc
.A8
.I16
ROUTINE DrawArrows
	; x = npc.floor * 2
	; y = (npc.floor * 16 + npc.target * 4) * 2
	; if npc.leftSideOnZero == 0
	;	x = ArrowsLocationsLeft[x]
	;	for i in 0 to 4
	;		interactiveBgBuffer[x - i] = ArrowsTilesLeft[y + i]
	; else
	;	x = ArrowsLocationsRight[x]
	;	for i in 0 to 4
	;		interactiveBgBuffer[x + i] = ArrowsTilesRight[y + i]

	LDA	#0
	XBA

	LDA	NpcStruct::floor
	ASL
	TAX
	ASL
	ADD	NpcStruct::targetFloor
	ASL
	ASL
	ASL
	TAY

_DrawArrows_ToTilemap:

	LDA	NpcStruct::leftSideOnZero
	IF_ZERO
		REP	#$20
.A16
		LDA	ArrowsLocationsLeft, X
		TAX
		.repeat 4, i
			LDA	ArrowsTilesLeft + 2 * i, Y
			STA	interactiveBgBuffer - 2 * i, X
		.endrepeat
	ELSE
		REP	#$20
.A16
		LDA	ArrowsLocationsRight, X
		TAX
		.repeat 4, i
			LDA	ArrowsTilesRight + 2 * i, Y
			STA	interactiveBgBuffer + 2 * i, X
		.endrepeat
	ENDIF

	SEP	#$20
.A8
	RTS


;; Clears The Arrows of the NPCs request to the Player
; dp = npc
.A8
.I16
ROUTINE ClearArrows
	; x = npc.floor * 2
	; y = (npc.floor * 16 + npc.floor * 4) * 2
	; drawArrows_ToTilemap(x, y)

	LDA	#0
	XBA

	LDA	NpcStruct::floor
	ASL
	TAX
	ASL
	ADD	NpcStruct::floor
	ASL
	ASL
	ASL
	TAY

	BRA	_DrawArrows_ToTilemap

.rodata


;; A list of NPC MetaSprites Frame Tables
LABEL Npcs_SpriteFrameTablePtr
	.addr MetaSprite_businessMan


;; The CharAttr values of the NPCs (matches `Npcs_SpriteFrameTablePtr`)
LABEL Npcs_SpriteCharAttr
	.word 7 << OAM_CHARATTR_PALETTE_SHIFT | 2 << OAM_CHARATTR_ORDER_SHIFT | (512 - 32)



LABEL ArrowsLocationsLeft
	.repeat N_FLOORS, i
		.addr (ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * i + ELEVATOR_ARROW_COLUMN) * 64 + (ELEVATOR_LEFT_ARROW_ROW * 2)
	.endrepeat

LABEL ArrowsLocationsRight
	.repeat N_FLOORS, i
		.addr (ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * i + ELEVATOR_ARROW_COLUMN) * 64 + (ELEVATOR_RIGHT_ARROW_ROW * 2)
	.endrepeat



.macro _ARROW_TILEMAP c, tile
	.repeat c
		.word tile
	.endrepeat
	.repeat 4 - c
		.word 0
	.endrepeat
.endmacro

LABEL ArrowsTilesLeft
	_ARROW_TILEMAP 0, 0
	_ARROW_TILEMAP 1, ARROW_LEFT_DOWN_TILE
	_ARROW_TILEMAP 2, ARROW_LEFT_DOWN_TILE
	_ARROW_TILEMAP 3, ARROW_LEFT_DOWN_TILE

	_ARROW_TILEMAP 1, ARROW_LEFT_UP_TILE
	_ARROW_TILEMAP 0, 0
	_ARROW_TILEMAP 1, ARROW_LEFT_DOWN_TILE
	_ARROW_TILEMAP 2, ARROW_LEFT_DOWN_TILE

	_ARROW_TILEMAP 2, ARROW_LEFT_UP_TILE
	_ARROW_TILEMAP 1, ARROW_LEFT_UP_TILE
	_ARROW_TILEMAP 0, 0
	_ARROW_TILEMAP 1, ARROW_LEFT_DOWN_TILE

	_ARROW_TILEMAP 3, ARROW_LEFT_UP_TILE
	_ARROW_TILEMAP 2, ARROW_LEFT_UP_TILE
	_ARROW_TILEMAP 1, ARROW_LEFT_UP_TILE
	_ARROW_TILEMAP 0, 0


LABEL ArrowsTilesRight
	_ARROW_TILEMAP 0, 0
	_ARROW_TILEMAP 1, ARROW_RIGHT_DOWN_TILE
	_ARROW_TILEMAP 2, ARROW_RIGHT_DOWN_TILE
	_ARROW_TILEMAP 3, ARROW_RIGHT_DOWN_TILE

	_ARROW_TILEMAP 1, ARROW_RIGHT_UP_TILE
	_ARROW_TILEMAP 0, 0
	_ARROW_TILEMAP 1, ARROW_RIGHT_DOWN_TILE
	_ARROW_TILEMAP 2, ARROW_RIGHT_DOWN_TILE

	_ARROW_TILEMAP 2, ARROW_RIGHT_UP_TILE
	_ARROW_TILEMAP 1, ARROW_RIGHT_UP_TILE
	_ARROW_TILEMAP 0, 0
	_ARROW_TILEMAP 1, ARROW_RIGHT_DOWN_TILE

	_ARROW_TILEMAP 3, ARROW_RIGHT_UP_TILE
	_ARROW_TILEMAP 2, ARROW_RIGHT_UP_TILE
	_ARROW_TILEMAP 1, ARROW_RIGHT_UP_TILE
	_ARROW_TILEMAP 0, 0


ENDMODULE

