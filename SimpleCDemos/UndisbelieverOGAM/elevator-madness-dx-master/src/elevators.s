; Elevators
;
; This code uses the DP register to select which elevator is processed.

.include "game.h"
.include "elevators.h"
.include "npcs.h"

MODULE Elevators

.struct ElevatorStruct
	;; Current state of the elevator
	state		.word

	;; The location of the table for this elevator
	tileTable	.word

	;; The current floor the elevator is on
	;; 16 bit because the Draw Functions use them.
	floor		.word

	;; The position of the rope for the elevator.
	;; This is bit masked, it is allowed to over/under flow.
	ropePos		.byte

	;; Frame counter for animations/states
	counter		.byte

	;; The NPC that is currently in the elevator
	occupant	.addr
.endstruct


.struct ElevatorTilePositionTable
	doorsPos		.res N_FLOORS * 2
	lightsPos		.res N_FLOORS * 2
	switchesPos		.res N_FLOORS * 2

	ropePos			.addr
	ropeTileOffset		.word

	switchOnTile		.word
	switchOffTile		.word
.endstruct


.segment "SHADOW"
	STRUCT	leftElevator, ElevatorStruct
	STRUCT	rightElevator, ElevatorStruct

	WORD	tmp
	WORD	tmp2


.rodata
LABEL ElevatorStateTable
	.addr ContinueDoorClosed
	.addr ContinueDoorOpening
	.addr ContinueDoorOpen
	.addr ContinueDoorClosing
	.addr ContinueMovingUp
	.addr ContinueMovingDown
	.addr ContinueCrashed
	.addr ContinueNpcEntering
	.addr ContinueNpcLeaving

.code

.A8
.I16
ROUTINE Init
	PHD

	; Setup left Elevator
	LDA	#.hibyte(leftElevator)
	XBA
	LDA	#.lobyte(leftElevator)
	TCD

	LDX	#ELEVATOR_DOOR_CLOSED
	STX	ElevatorStruct::state

	LDX	#.loword(LeftElevatorTilePositionTable)
	STX	ElevatorStruct::tileTable

	LDY	#0
	STY	ElevatorStruct::floor
	STY	ElevatorStruct::occupant

	JSR	_Init_Draw

	; Setup Right Elevator
	LDA	#.hibyte(rightElevator)
	XBA
	LDA	#.lobyte(rightElevator)
	TCD

	LDX	#ELEVATOR_DOOR_CLOSED
	STX	ElevatorStruct::state

	LDX	#.loword(RightElevatorTilePositionTable)
	STX	ElevatorStruct::tileTable

	LDY	#0
	STY	ElevatorStruct::floor
	STY	ElevatorStruct::occupant

	JSR	_Init_Draw

	PLD
	RTS


; Draws all doors closed, switches off and only the bottom floor's lights on.
; DP = elevator
ROUTINE _Init_Draw
	; for f = 0 to N_FLOORS
	;	elevator.floor = f
	;	elevator.drawDoor(0)
	;	elevator.drawLightOff()
	;	elevator.drawSwitchOff()
	;	elevator.clearArrows()
	; elevator.drawLightOn()
	; elevator.drawRope()

	LDA	#0
	REPEAT
		STA	ElevatorStruct::floor

		LDA	#0
		JSR	DrawDoor
		JSR	DrawLightOff
		JSR	DrawSwitchOff

		LDA	ElevatorStruct::floor
		INC
		CMP	#N_FLOORS
	UNTIL_GE

	JSR	DrawLightOn
	JMP	DrawRope



.A8
.I16
ROUTINE Process
	; stateTable[leftElevator.state](leftElevator)
	; stateTable[rightElevator.state](rightElevator)

	PHD

	LDA	#.hibyte(leftElevator)
	XBA
	LDA	#.lobyte(leftElevator)

	TCD

	LDX	ElevatorStruct::state
	JSR	(.loword(ElevatorStateTable), X)


	LDA	#.hibyte(rightElevator)
	XBA
	LDA	#.lobyte(rightElevator)
	TCD

	LDX	ElevatorStruct::state
	JSR	(.loword(ElevatorStateTable), X)

	PLD
	RTS



; INPUT:
;	C: elevator (clear = left, set = right)
;	A: player's floor (0-3)
; OUTPUT: C set if elevator door is opening/closing, otherwise false.
.A8
.I16
ROUTINE PlayerPressDoor
	; if c set
	;	elevator = leftElevator
	; else
	;	elevator = rightElevator
	;
	; if elevator->floor == player_floor
	;	DP = elevator
	;	if elevator->state == ELEVATOR_DOOR_CLOSED
	;		SetDoorOpening()
	;		r = true
	;	else if elevator->state == ELEVATOR_DOOR_OPEN
	;		SetDoorClosed()
	;		r = true
	;	else
	;		r = false
	;	return r
	;
	; return false
	IF_C_CLEAR
		LDX	#leftElevator
	ELSE
		LDX	#rightElevator
	ENDIF

	CMP	a:ElevatorStruct::floor, X
	IF_EQ
		PHD

		REP	#$30
		TXA
		SEP	#$20
		TCD

		LDY	ElevatorStruct::state
		CPY	#ELEVATOR_DOOR_CLOSED
		IF_EQ
			JSR	SetDoorOpening
			SEC
		ELSE
			CPY	#ELEVATOR_DOOR_OPEN
		IF_EQ
			JSR	SetDoorClosing
			SEC
		ELSE
			CLC
		ENDIF
		ENDIF

		PLD
		RTS
	ENDIF

	CLC
	RTS



; INPUT:
;	C: elevator (clear = left, set = right)
; OUTPUT: C set if elevator is now moving, otherwise false.
.A8
.I16
ROUTINE PlayerPressUp
	; if c set
	;	elevator = leftElevator
	; else
	;	elevator = rightElevator
	;
	; if elevator.state = ELEVATOR_DOOR_CLOSED
	;	DP = elevator
	;	elevator.SetMovingUp()

	IF_C_CLEAR
		LDX	#leftElevator
	ELSE
		LDX	#rightElevator
	ENDIF

	LDA	a:ElevatorStruct::state, X
	CMP	#ELEVATOR_DOOR_CLOSED
	IF_EQ
		PHD

		REP	#$30
		TXA
		SEP	#$20
		TCD

		JSR	SetMovingUp

		PLD
		SEC
		RTS
	ELSE
		CLC
	ENDIF

	RTS


; INPUT:
;	C: elevator (clear = left, set = right)
; OUTPUT: C set if elevator is now moving, otherwise false.
.A8
.I16
ROUTINE PlayerPressDown
	; if c clear
	;	elevator = leftElevator
	; else
	;	elevator = rightElevator
	;
	; if elevator.state = ELEVATOR_DOOR_CLOSED
	;	DP = elevator
	;	elevator.SetMovingDown()

	IF_C_CLEAR
		LDX	#leftElevator
	ELSE
		LDX	#rightElevator
	ENDIF

	LDA	a:ElevatorStruct::state, X
	CMP	#ELEVATOR_DOOR_CLOSED
	IF_EQ
		PHD

		REP	#$30
		TXA
		SEP	#$20
		TCD

		JSR	SetMovingDown

		PLD
		SEC
		RTS
	ELSE
		CLC
	ENDIF

	RTS


; DP: The NPC
;  A: NPC floor
;  C: elevator (clear = left, set = right)
;
; Return C set if NPC is now entering elevator
.A8
.I16
ROUTINE	NpcEnterElevator
	; if c clear
	;	x = leftElevator
	; else
	;	x = rightElevator
	;
	; if x->occupannt == 0 and x->state = ELEVATOR_DOOR_OPEN && elevator->floor = floor
	;	x->occupant = DP
	;	x->state = ELECATOR_NPC_ENTERING
	;	return true
	;
	; return false

	IF_C_CLEAR
		LDX	#leftElevator
	ELSE
		LDX	#rightElevator
	ENDIF

	LDY	a:ElevatorStruct::occupant, X
	IF_ZERO
		LDY	a:ElevatorStruct::state, X
		CPY	#ELEVATOR_DOOR_OPEN
		IF_EQ
			CMP a:ElevatorStruct::floor, X
			IF_EQ
				REP	#$30
.A16
				TDC
				STA	a:ElevatorStruct::occupant, X

				LDA	#ELEVATOR_NPC_ENTERING
				STA	a:ElevatorStruct::state, X

				SEP	#$21 ; Also sets carry
.A8
				RTS
			ENDIF
		ENDIF
	ENDIF

	CLC
	RTS


; DP: The NPC
;  C: elevator (clear = left, set = right)
.A8
.I16
ROUTINE	NpcInsideElevator
	IF_C_CLEAR
		LDX	#leftElevator
	ELSE
		LDX	#rightElevator
	ENDIF

	LDA	#ELEVATOR_DOOR_OPEN
	STA	a:ElevatorStruct::state, X

	RTS


; DP = selected elevator
.A8
.I16
ROUTINE SetDoorClosed
	; state = ELEVATOR_DOOR_CLOSED
	; elevator.drawDoor(DOOR_CLOSED_FRAME)
	; play(DOOR_CLOSED_SOUND)

	LDX	#ELEVATOR_DOOR_CLOSED
	STX	ElevatorStruct::state

	LDA	#DOOR_CLOSED_FRAME
	JSR	DrawDoor

	;; ::SOUND door closed::

	.assert * = ContinueDoorClosed, lderror, "Bad Flow"

; DP = selected elevator
.A8
.I16
ROUTINE ContinueDoorClosed
	RTS



; DP = selected elevator
.A8
.I16
ROUTINE SetDoorOpening
	; elevator.state = ELEVATOR_DOOR_OPENING
	; elevator.counter = 0
	; play(DOOR_OPENING_SOUND)
	; elevator.DrawSwitchOn()
	;
	; if elevator.occupant != 0
	;	Npc__OccupiedElevatorDoorOpening(elevator.occupant, elevator.floor)

	LDY	#ELEVATOR_DOOR_OPENING
	STY	ElevatorStruct::state

	STZ	ElevatorStruct::counter

	;; ::SOUND door opening::

	LDX	ElevatorStruct::occupant
	IF_NOT_ZERO
		LDA	ElevatorStruct::floor
		JSR	Npcs__OccupiedElevatorDoorOpening
	ENDIF

	JMP	DrawSwitchOn


; DP = selected elevator
.A8
.I16
ROUTINE ContinueDoorOpening
	; if counter > N_DOOR_FRAMES
	;	SetDoorOpen()
	; else
	;	counter++
	;	DrawDoor(counter)

	LDA	ElevatorStruct::counter
	CMP	#N_DOOR_FRAMES
	BGE	SetDoorOpen

	JSR	DrawDoor

	INC	ElevatorStruct::counter

	RTS



; DP = selected elevator
.A8
.I16
ROUTINE SetDoorOpen
	; state = ELEVATOR_DOOR_OPEN
	; elevator.drawDoor(DOOR_OPEN_FRAME)
	; play(DOOR_OPEN_SOUND)
	;
	; if elevator.occupant != 0
	;	Npc__OccupiedElevatorOpen(elevator.occupant, elevator.floor)
	;	elevator.occupant = 0

	LDX	#ELEVATOR_DOOR_OPEN
	STX	ElevatorStruct::state

	LDA	#DOOR_OPEN_FRAME
	JSR	DrawDoor

	;; ::SOUND door open::

	LDX	ElevatorStruct::occupant
	IF_NOT_ZERO
		LDA	ElevatorStruct::floor
		JSR	Npcs__OccupiedElevatorOpen
		LDX	#0
		STX	ElevatorStruct::occupant
	ENDIF

	.assert * = ContinueDoorOpen, lderror, "Bad Flow"


; DP = selected elevator
.A8
.I16
ROUTINE ContinueDoorOpen
	RTS



; DP = selected elevator
.A8
.I16
ROUTINE SetDoorClosing
	; elevator.state = ELEVATOR_DOOR_CLOSING
	; elevator.counter = N_DOOR_FRAMES - 1
	; elevator.DrawSwitchOff()
	; play(DOOR_CLOSING_SOUND)

	LDY	#ELEVATOR_DOOR_CLOSING
	STY	ElevatorStruct::state

	LDA	#N_DOOR_FRAMES - 1
	STA	ElevatorStruct::counter

	JSR	DrawSwitchOff

	;; ::SOUND door closing::

	RTS



; DP = selected elevator
.A8
.I16
ROUTINE ContinueDoorClosing
	; if counter == 0
	;	SetDoorClosed
	; else
	;	counter--
	;	DrawDoor(counter)

	LDA	ElevatorStruct::counter
	BEQ	SetDoorClosed

	DEC	ElevatorStruct::counter

	JSR	DrawDoor

	RTS



; DP = selected elevator
.A8
.I16
ROUTINE SetMovingUp
	; elevator.state = ELEVATOR_MOVING_UP
	; elevator.counter = N_ELEVATOR_MOVING_FRAMES
	; elevator.DrawLightOff()
	; play(SOUND_ELEVATOR_MOVING)

	LDY	#ELEVATOR_MOVING_UP
	STY	ElevatorStruct::state

	LDA	#N_ELEVATOR_MOVING_FRAMES
	STA	ElevatorStruct::counter

	JSR	DrawLightOff

	;; ::SOUND elevator moving::

	RTS



; DP = selected elevator
.A8
.I16
ROUTINE ContinueMovingUp
	; if --counter == 0
	;	elevator.floor--
	;	if elevator.floor < 0
	;		SetCrashed()
	;	else
	;		play(SOUND_ELEVATOR_DING)
	;		elevator.drawLightOn()
	;	elevator.SetDoorClosed
	; else
	;	elevator.ropePos++
	;	DrawRope()

	DEC	ElevatorStruct::counter
	IF_ZERO
		DEC	ElevatorStruct::floor
		BMI	SetCrashed

		; ::SOUND elevator ding::
		JSR	DrawLightOn
		JMP	SetDoorClosed
	ENDIF

	INC	ElevatorStruct::ropePos
	JMP	DrawRope




; DP = selected elevator
.A8
.I16
ROUTINE SetMovingDown
	; elevator.state = ELEVATOR_MOVING_DOWN
	; elevator.counter = N_ELEVATOR_MOVING_FRAMES
	; elevator.DrawLightOff()
	; play(SOUND_ELEVATOR_MOVING)

	LDY	#ELEVATOR_MOVING_DOWN
	STY	ElevatorStruct::state

	LDA	#N_ELEVATOR_MOVING_FRAMES
	STA	ElevatorStruct::counter

	JSR	DrawLightOff

	;; ::SOUND elevator moving::

	RTS


; DP = selected elevator
.A8
.I16
ROUTINE ContinueMovingDown
	; if --counter == 0
	;	if elevator.floor >= N_FLOORS - 1
	;		SetCrashed()
	;	else
	;		elevator.floor++
	;		play(SOUND_ELEVATOR_DING)
	;		elevator.drawLightOn()
	;	elevator.SetDoorClosed
	; else
	;	elevator.ropePos--
	;	DrawRope()

	DEC	ElevatorStruct::counter
	IF_ZERO
		LDA	ElevatorStruct::floor
		CMP	#N_FLOORS - 1
		BGE	SetCrashed

		INC	ElevatorStruct::floor
		; ::SOUND elevator ding::
		JSR	DrawLightOn
		JMP	SetDoorClosed
	ENDIF

	DEC	ElevatorStruct::ropePos
	JMP	DrawRope



; DP = selected elevator
.A8
.I16
ROUTINE SetCrashed
	; elevator.state = ELEVATOR_CRASHED
	; play(SOUND_ELEVATOR_CRASHED)
	;
	; strikeAnimationRopeTile = Memory[tileTable + ElevatorTilePositionTable::ropePos]
	; Game__strikeAntimationRoutinePtr = StrikeAnimationRopeBreaking


	LDY	#ELEVATOR_CRASHED
	STY	ElevatorStruct::state

	;; ::SOUND elevator moving::

	LDY	ElevatorStruct::tileTable
	LDX	a:ElevatorTilePositionTable::ropePos, Y
	STX	strikeAnimationRopeTile

	LDX	#.loword(StrikeAnimationRopeBreaking)
	STX	Game__strikeAntimationRoutinePtr

	RTS




; DP = selected elevator
; Blank routines
.A8
.I16
ROUTINE ContinueCrashed
ROUTINE ContinueNpcEntering
ROUTINE ContinueNpcLeaving
	RTS



.segment "SHADOW"
	;; Location of rope tile to remove.
	;; Used by StrikeAnimationRopeBreaking
	ADDR	strikeAnimationRopeTile

.code

;; Animates the Rope breaking when elevator goes over/under
;; Called once per frame
.A8
.I16
ROUTINE StrikeAnimationRopeBreaking
	; if strikeAnimationRopeTile < 28 * 64
	; 	interactiveBgBuffer[strikeAnimationRopeTile] = 0
	; 	strikeAnimationRopeTile += 64

	STZ	updateBgBufferOnZero

	REP	#$30
.A16

	LDX	strikeAnimationRopeTile
	CPX	#28 * 64
	IF_LT
		STZ	interactiveBgBuffer, X

		TXA
		ADD	#64
		STA	strikeAnimationRopeTile
	ENDIF

	SEP	#$20
	RTS



;; Draws the elevator light to the ForegroundBuffer
;;
;; INPUT:
;;  	X = tile position (top left of elevator light)
;; MODIFIES: A, X, Y
.A8
.I16
ROUTINE	DrawLightOn
	; (X) tilePos = Memory[ElevatorTilePositionTable::lightsPos + tileTable + floor * 2]
	; interactiveBgBuffer[tilePos] = ELEVATOR_LIGHT_LEFT_TILE
	; interactiveBgBuffer[tilePos + 2] = ELEVATOR_LIGHT_RIGHT_TILE

	REP	#$30
.A16
.I16
	LDA	ElevatorStruct::floor
	ASL
	ADD	ElevatorStruct::tileTable
	TAY
	LDX	a:ElevatorTilePositionTable::lightsPos, Y

	LDA	#ELEVATOR_LIGHT_LEFT_TILE
	STA	interactiveBgBuffer + 0, X
	LDA	#ELEVATOR_LIGHT_RIGHT_TILE
	STA	interactiveBgBuffer + 2, X

	SEP	#$20
.A8

	RTS


;; Turns off the elevator light on the ForegroundBuffer
;;
;; INPUT:
;;	DP = start of ElevatorStruct
;; MODIFIES: A, X, Y
.A8
.I16
ROUTINE	DrawLightOff
	; (X) tilePos = Memory[ElevatorTilePositionTable::lightsPos + tileTable + floor * 2]
	; interactiveBgBuffer[tilePos] = 0
	; interactiveBgBuffer[tilePos + 2] = 0

	REP	#$30
.A16
.I16
	LDA	ElevatorStruct::floor
	ASL
	ADD	ElevatorStruct::tileTable
	TAY
	LDX	a:ElevatorTilePositionTable::lightsPos, Y

	STZ	interactiveBgBuffer + 0, X
	STZ	interactiveBgBuffer + 2, X

	SEP	#$20
.A8

	RTS


;; Draws the depressed elevator switch to the ForegroundBuffer
;;
;; INPUT:
;;	DP = start of ElevatorStruct
;; MODIFIES: A, X, Y
.A8
.I16
ROUTINE	DrawSwitchOn
	; (X) tilePos = Memory[ElevatorTilePositionTable::switch + tileTable + floor * 2]
	; (A) tile = Memory[ElevatorTilePositionTable::switchOnTile + tileTable]
	; interactiveBgBuffer[tilePos] = tile

	REP	#$30
.A16
.I16
	LDA	ElevatorStruct::floor
	ASL
	ADD	ElevatorStruct::tileTable
	TAY
	LDX	a:ElevatorTilePositionTable::switchesPos, Y

	LDY	ElevatorStruct::tileTable
	LDA	a:ElevatorTilePositionTable::switchOnTile, Y

	STA	interactiveBgBuffer, X

	SEP	#$20
.A8

	RTS


;; Draws the elevator switch on the ForegroundBuffer
;;
;; INPUT:
;;	DP = start of ElevatorStruct
;; MODIFIES: A, X, Y
.A8
.I16
ROUTINE	DrawSwitchOff
	; (X) tilePos = Memory[ElevatorTilePositionTable::lightPos + tileTable + floor * 2]
	; (A) tile = Memory[ElevatorTilePositionTable::switchOffTile + tileTable]
	; interactiveBgBuffer[tilePos] = tile

	REP	#$30
.A16
.I16
	LDA	ElevatorStruct::floor
	ASL
	ADD	ElevatorStruct::tileTable
	TAY
	LDX	a:ElevatorTilePositionTable::switchesPos, Y

	LDY	ElevatorStruct::tileTable
	LDA	a:ElevatorTilePositionTable::switchOffTile, Y

	STA	interactiveBgBuffer, X

	SEP	#$20
.A8

	RTS



;; Draws the elevator door to the ForegroundBuffer
;;
;; INPUT:
;; 	A = Door Frame Number
;;	DP = start of ElevatorStruct
;; MODIFIES: A, X, Y
.A8
.I16
ROUTINE	DrawDoor
	; (Y) DoorFrameData = A * 8
	; (X) tilePos = Memory[tileTable + ElevatorTilePositionTable::doorsPos + floor * 2]
	; for row = 0 to ELEVATOR_DOOR_ROWS
	;	tile = DoorTiles[DoorFrameData][row]
	;	for columns = 0 to ELEVATOR_DOOR_HEIGHT
	;		interactiveBgBuffer[tilePos + column * 64 + row * 2] = tile

	REP	#$30
.A16
.I16
	AND	#$000F
	ASL
	ASL
	ASL
	STA	tmp

	LDA	ElevatorStruct::floor
	ASL
	ADD	ElevatorStruct::tileTable
	TAY
	LDX	a:ElevatorTilePositionTable::doorsPos, Y

	LDY	tmp

	.repeat ELEVATOR_DOOR_ROWS, row
		LDA	DoorTiles + row * 2, Y

		.repeat ELEVATOR_DOOR_HEIGHT, column
			STA	interactiveBgBuffer + column * 64 + row * 2, X
		.endrepeat
	.endrepeat

	SEP	#$20
.A8

	RTS



;; Draws the elevator's rope
;;
;; INPUT:
;;	DP = start of ElevatorStruct
;; MODIFIES: A, X, Y
.A8
.I16
ROUTINE	DrawRope
	; (X) tilePos = Memory[tileTable + ElevatorTilePositionTable::ropePos]
	; (A) tile    = (ropePos / ELEVATOR_ROPE_FRAME_DELAY & ROPE_MASK) + Memory[tileTable + ElevatorTilePositionTable::ropeTileOffset]
	; for columns = 0 to N_ROPE_COLUMNS
	;	interactiveBgBuffer[tilePos + columns * 64] = tile

	REP	#$30
.A16
.I16
	LDY	ElevatorStruct::tileTable
	LDX	a:ElevatorTilePositionTable::ropePos, Y

	.assert ELEVATOR_ROPE_FRAME_DELAY = 4, error, "Invalid Division"
	LDA	ElevatorStruct::ropePos
	LSR
	LSR
	AND	#ELEVATOR_ROPE_TILE_MASK
	ADD	a:ElevatorTilePositionTable::ropeTileOffset, Y

	.repeat ELEVATOR_ROPE_HEIGHT, column
		STA	interactiveBgBuffer + column * 64, X
	.endrepeat

	SEP	#$20
.A8

	RTS


.rodata

LABEL LeftElevatorTilePositionTable
	; Doors
	.repeat N_FLOORS, i
		.addr (ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * i + ELEVATOR_DOOR_COLUMN) * 64 + ELEVATOR_LEFT_DOOR_ROW * 2
	.endrepeat
	; Lights
	.repeat N_FLOORS, i
		.addr (ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * i + ELEVATOR_LIGHT_COLUMN) * 64 + ELEVATOR_LEFT_LIGHT_ROW * 2
	.endrepeat
	; Switches
	.repeat N_FLOORS, i
		.addr (ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * i + ELEVATOR_SWITCH_COLUMN) * 64 + ELEVATOR_LEFT_SWITCH_ROW * 2
	.endrepeat
	; Rope Position
	.addr ELEVATOR_LEFT_ROPE_ROW * 2
	; Rope Offset
	.word ELEVATOR_LEFT_ROPE_TILE_OFFSET
	; Tiles
	.word ELEVATOR_SWITCH_ON_TILE
	.word ELEVATOR_SWITCH_OFF_TILE



LABEL RightElevatorTilePositionTable
	; Doors
	.repeat N_FLOORS, i
		.addr (ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * i + ELEVATOR_DOOR_COLUMN) * 64 + ELEVATOR_RIGHT_DOOR_ROW * 2
	.endrepeat
	; Lights
	.repeat N_FLOORS, i
		.addr (ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * i + ELEVATOR_LIGHT_COLUMN) * 64 + ELEVATOR_RIGHT_LIGHT_ROW * 2
	.endrepeat
	; Switches
	.repeat N_FLOORS, i
		.addr (ELEVATOR_FLOOR_0_COLUMN + ELEVATOR_FLOOR_COLUMN_SPACING * i + ELEVATOR_SWITCH_COLUMN) * 64 + ELEVATOR_RIGHT_SWITCH_ROW * 2
	.endrepeat
	; Rope Position
	.addr ELEVATOR_RIGHT_ROPE_ROW * 2
	; Rope Offset
	.word ELEVATOR_RIGHT_ROPE_TILE_OFFSET
	; Tiles
	.word ELEVATOR_SWITCH_ON_TILE | TILEMAP_H_FLIP_FLAG
	.word ELEVATOR_SWITCH_OFF_TILE | TILEMAP_H_FLIP_FLAG


LABEL DoorTiles

	.repeat 8, i
		.word ELEVATOR_DOOR_TILE_OFFSET + 0 | ELEVATOR_TILE_CHARATTR
		.word ELEVATOR_DOOR_TILE_OFFSET + 1 + i | ELEVATOR_TILE_CHARATTR
		.word ELEVATOR_DOOR_TILE_OFFSET + 1 + i | ELEVATOR_TILE_CHARATTR | TILEMAP_H_FLIP_FLAG
		.word ELEVATOR_DOOR_TILE_OFFSET + 0 | ELEVATOR_TILE_CHARATTR | TILEMAP_H_FLIP_FLAG
	.endrepeat
	.repeat 8, i
		.word ELEVATOR_DOOR_TILE_OFFSET + 1 + i | ELEVATOR_TILE_CHARATTR
		.word ELEVATOR_DOOR_TILE_CLEAR  | ELEVATOR_TILE_CHARATTR
		.word ELEVATOR_DOOR_TILE_CLEAR  | ELEVATOR_TILE_CHARATTR | TILEMAP_H_FLIP_FLAG
		.word ELEVATOR_DOOR_TILE_OFFSET + 1 + i | ELEVATOR_TILE_CHARATTR | TILEMAP_H_FLIP_FLAG
	.endrepeat


ENDMODULE

