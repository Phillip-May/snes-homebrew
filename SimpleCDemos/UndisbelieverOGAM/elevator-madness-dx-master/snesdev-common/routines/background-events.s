
.include "background-events.h"
.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"

MODULE BackgroundEvents

BACKGROUND_EVENT_STRUCT BackgroundEventStruct
END_BACKGROUND_EVENT_STRUCT

.segment "SHADOW"
	BYTE	events, N_BACKGROUND_EVENTS * BACKGROUND_EVENT_SIZE

.code

ROUTINE Init
	PHP
	REP	#$20
.A16
	.repeat N_BACKGROUND_EVENTS, i
		STZ	events + i * BACKGROUND_EVENT_SIZE + BackgroundEventStruct::functionPtr
	.endrepeat

	PLP
	RTS


.A16
.I16
ROUTINE NewEvent
	.assert N_BACKGROUND_EVENTS <= 5, error, "Too many events, rewrite code"

	TXA

	; must not modify A
	.repeat N_BACKGROUND_EVENTS, i
		LDX	events + i * BACKGROUND_EVENT_SIZE + BackgroundEventStruct::functionPtr
		IF_ZERO
			LDX	#events + i * BACKGROUND_EVENT_SIZE
			BRA	_EmptyEventFound
		ENDIF
	.endrepeat

	; no empty events found
	CLC
	RTS

_EmptyEventFound:
	STA	a:BackgroundEventStruct::functionPtr, X
	SEC
	RTS



.A16
.I16
ROUTINE Process
	.assert N_BACKGROUND_EVENTS <= 5, error, "Too many events, rewrite code"

	.repeat N_BACKGROUND_EVENTS, i
		LDA	events + i * BACKGROUND_EVENT_SIZE + BackgroundEventStruct::functionPtr
		IF_NOT_ZERO
			LDA	#events + i * BACKGROUND_EVENT_SIZE
			TCD
			TAX
			JSR	(0, X)
			IF_C_CLEAR
				STZ	events + i * BACKGROUND_EVENT_SIZE + BackgroundEventStruct::functionPtr
			ENDIF
		ENDIF
	.endrepeat
	RTS

ENDMODULE

