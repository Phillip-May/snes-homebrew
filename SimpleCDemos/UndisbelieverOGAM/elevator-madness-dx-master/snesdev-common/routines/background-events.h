;; The BackgroundEvents module allows for a up to N_BACKGROUND_EVENTS
;; routines to be called once per frame on every frame of the game loop.
;;
;; In order to process the event state, {BACKGROUND_EVENT_SIZE - 2} bytes
;; of shadow RAM is allocated to the routine. This is accessed through
;; the X register after allocation (see `NewEvent`) and the direct page
;; instructions when the routine is called by the module (see `Process`).
;;
;; For example, to create a new event with a parameter of #60, simply:
;;
;;	BACKGROUND_EVENT_STRUCT
;;		counter		.word
;;	END_BACKGROUND_EVENT_STRUCT
;;
;;	.A16
;;	.I16
;;		LDX	#EventRoutine
;;		JSR	BackgroundEvents__NewEvent
;;		IF_C_SET
;;			LDA	#60
;;			STA	a:EventStruct::counter, X
;;		ENDIF
;;
;; And the variable *EventStruct::counter* will be available to
;; *EventRoutine* though the zero page register:
;;
;;	.A16
;;	.I16
;; 	EventRoutine:
;;		DEC	z:EventStruct::counter
;;		IF_ZERO
;;			; do something
;;
;;			CLC		; do not execute next frame
;;			RTS
;;		ENDIF
;;
;;		SEC			; continue next frame
;;		RTS
;;

.ifndef ::_BACKROUND_EVENTS_H_
::_BACKROUND_EVENTS_H_ = 1

.setcpu "65816"

; Common includes
.include "includes/import_export.inc"
.include "includes/registers.inc"
.include "includes/config.inc"

;; Number of events to run in the background of the game loop.
CONFIG N_BACKGROUND_EVENTS, 3
;; The maximum size of the events in bytes.
CONFIG BACKGROUND_EVENT_SIZE, 20

;; Sets up a struct containing the function pointer for the first item
;; Also ensures that the size of the structure is <= BACKGROUND_EVENT_SIZE
.macro BACKGROUND_EVENT_STRUCT name
	.define __BACKGROUND_EVENT_STRUCT_NAME name

	.struct name
		;; Function to execute, once per frame.
		;; REQUIRE: 16 bit A, 16 bit Index, DB = $7E
		;; CAN MODIFY: A, X, Y
		;; INPUT: DP - this struct to store routine state
		;; OUTPUT: C set if event continues in next frame
		;;	   if C clear then function will not execute in next frame.
		functionPtr		.addr
.endmacro

.macro END_BACKGROUND_EVENT_STRUCT
	.endstruct
	.assert .sizeof(__BACKGROUND_EVENT_STRUCT_NAME) <= ::BACKGROUND_EVENT_SIZE, error, .sprintf("ERROR: %s is too large (%d bytes max, %d bytes used)", .string(name), ::BACKGROUND_EVENT_SIZE, .sizeof(__BACKGROUND_EVENT_STRUCT_NAME))
	.undefine __BACKGROUND_EVENT_STRUCT_NAME
.endmacro


IMPORT_MODULE BackgroundEvents
	;; Turns off all of the background events.
	;; REQUIRES: DB access shadow RAM
	ROUTINE Init

	;; Creates and allocates a new background event space.
	;; REQUIRES: 16 bit A, 16 bit Index, DB access shadow RAM
	;; INPUT: X - Function Pointer
	;; OUTPUT: carry set if space allocated, carry clear if it didn't happen.
	;;	X - location of the BackgroundEventStruct allocated.
	ROUTINE	NewEvent

	;; Processes each event, one after another.
	;; REQUIRE: 16 bit A, 16 bit Index, DB = $7E
	;; MODIFIES: A, X, Y, DP
	ROUTINE Process
ENDMODULE

.endif ; ::_BACKROUND_EVENTS_H_

; vim: set ft=asm:

