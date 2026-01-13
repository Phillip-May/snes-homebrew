
.include "includes/import_export.inc"
.include "includes/structure.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "routines/metasprite.h"

.setcpu "65816"

MODULE MetaSprite

.segment "SHADOW"
	BYTE	updateOam

	STRUCT	oamBuffer, OamFormat, 128
	BYTE	oamBuffer2, 128 / 4

	WORD	oamBufferPos
	WORD	prevOamBufferPos

	WORD	oamBuffer2Pos
	BYTE	oamBuffer2Temp

	BYTE	nObjects

	WORD	xPos
	WORD	yPos
	WORD	charAttr
	SAME_VARIABLE size, nObjects


.assert oamBuffer + .sizeof(oamBuffer) = oamBuffer2, lderror, "Bad oamBuffer -> oamBuffer2 flow"

; Ensure Init and VBlank macros are used
.forceimport _MetaSprite_Init__Called:zp
.forceimport _MetaSprite_VBlank__Called:zp


.code



.A8
.I16
ROUTINE InitLoop
	; reset oamBuffer2 to 0
	;
	; oamBufferPos = 0
	; oamBuffer2Pos = 0
	; oamBuffer2Temp = 0x80

	LDX	#0
	STX	oamBufferPos
	STX	oamBuffer2Pos

	LDA	#$80
	STA	oamBuffer2Temp

	RTS



.A8
.I16
; Y unused
ROUTINE	FinalizeLoop
	; if oamBuffer2Temp != $80
	;   if oamBuffer2Pos < sizeof(oamBuffer2)
	;     finalize oamBuffer2Temp
	;     oamBuffer2[oamBuffer2Pos] = oamBuffer2Temp
	;
	; for x = oamBuffer2Pos + 1 to sizeof(oamBuffer2):
	;	oamBuffer2[x] = 0
	;
	; if oamBufferPos < sizeof(oamBuffer):
	;   repeat
	;     oamBuffer[x].ypos = 240
	;     x += 4
	;   until x >= prevOamBufferPos
	;
	;   prevOamBufferPos = oamBufferPos
	; else:
	;   prevOamBufferPos = sizeof(oamBuffer)
	;

	LDA	oamBuffer2Temp
	CMP #$80
	IF_NE
		LDX	oamBuffer2Pos
		CPX #.sizeof(oamBuffer2)
		IF_LT
			REPEAT
				LSR
				LSR
			UNTIL_C_SET

			STA	oamBuffer2, X
		ENDIF
	ENDIF


	; This would would be unnecessary if sprites are only 8x8 and/or 16x16 in size
	.if SPRITE_SIZE > 16
		LDX	oamBuffer2Pos
		INX
		CPX #,sizeof(oamBuffer2)
		IF_LT
			REPEAT
				STZ	oamBuffer2, X
				INX
				CPX	#.sizeof(oamBuffer2)
			UNTIL_GE
		ENDIF
	.endif


	; Set the `yPos` of unused sprites to 240 (offscreen)
	;
	; The sprites after `prevOamBufferPos` are already offscreen, there is no need to set all unused sprites offscreen.
	LDX	oamBufferPos
	CPX #.sizeof(oamBuffer)
	IF_LT
		LDA	#240

		REPEAT
			STA	oamBuffer + OamFormat::yPos, X
			INX
			INX
			INX
			INX

			CPX	prevOamBufferPos
		UNTIL_GE

		LDX	oamBufferPos
		STX	prevOamBufferPos
	ELSE
		LDX #.sizeof(oamBuffer)
		STX prevOamBufferPos
	ENDIF


	LDA #1
	STA	updateOam

	RTS



.A8
.I16
ROUTINE ProcessSprite
	; if xPos < -SPRITE_SIZE + 1 || xPos >= 0x100
	;   return
	;
	; if yPos < -SPRITE_SIZE + 1 || yPos >= 240
	;   return
	;
	; if oamBufferPos >= sizeof(oamBuffer)
	;	return
	;
	; oamBuffer[oamBufferPos].xPos = xPos
	; oamBuffer[oamBufferPos].xPos = yPos
	; oamBuffer[oamBufferPos].charAttr = charAttr
	; oamBufferPos++ // 4 bytes
	;
	; ROR(oamBuffer2Temp, bit9(xPos))
	; ROR(oamBuffer2Temp, lowbit(size))
	; if oamBuffer2Temp underflowed
	;	oamBuffer2[oamBuffer2Pos] = oamBuffer2Temp
	;	oamBuffer2Pos++
	;	oamBuffer2Temp = 0x80

	LDX	xPos
	CPX	#.loword(-SPRITE_SIZE + 1)
	IF_LT
		CPX	#$0100
		BCS	_ProcessSprite_Return
	ENDIF

	LDY	yPos
	CPY	#.loword(-SPRITE_SIZE + 1)
	IF_LT
		CPY	#240
		BCS	_ProcessSprite_Return
	ENDIF

	TXA

	LDX	oamBufferPos
	CPX	#.sizeof(oamBuffer)
	BGE	_ProcessSprite_Return

	; X = lobyte(xPos)
	STA	oamBuffer + OamFormat::xPos, X

	TYA
	STA	oamBuffer + OamFormat::yPos, X

	; ANNOY: NO `STY addr,X`
	REP	#$31		; also clear carry
.A16
	LDA	charAttr
	STA	oamBuffer + OamFormat::char, X

	TXA
	ADC	#4
	STA	oamBufferPos

	SEP	#$20
.A8

	LDA	xPos + 1			; don't modify xPos
	LSR
	ROR	oamBuffer2Temp
	LDA	size				; don't modify size
	LSR
	ROR	oamBuffer2Temp
	IF_C_SET
		; populate oamBuffer2
		LDY	oamBuffer2Pos
		STA	oamBuffer2, Y
		INC	oamBuffer2Pos

		LDA	#$80
		STA	oamBuffer2Temp
	ENDIF

_ProcessSprite_Return:
	RTS



; To fix range error in Sprite Loop
; 6 bytes too short
.A8
.I16
_ProcessMetaSprite_BreakLoop:
	; Y is index into oamBuffer
	STY	oamBufferPos

	RTS


.A8
.I16
ROUTINE ProcessMetaSprite_Y
	STY	charAttr

	.assert * = ProcessMetaSprite, lderror, "Bad Flow"

.A8
.I16
ROUTINE	ProcessMetaSprite
MetaSpriteObjects := MetaSpriteLayoutBank << 16 + 1

	; n = layout->nObjects
	;
	; foreach layout->objects as l
	;	if oamBufferPos >= sizeof(oamBuffer)
	;		break;
	;
	;	displayYPos = yPos + l->yPos
	;	if displayYPos < -15 or displayYPos >= 240
	;		continue
	;	oamBuffer[oamBufferPos]->yPos = lowbyte(displayYPos)
	;
	;	displayXPos = xPos + l->xPos
	;	if displayXPos < -15 or displayXPos >= 256
	;		continue
	;	oamBuffer[oamBufferPos]->xPos = lowbyte(displayXPos)
	;	ROR(oamBuffer2Temp, highbit(displayXPos))
	;
	;	oamBuffer[oamBufferPos]->charAttr = layout->chatAttr + l->charAttr
	;
	;	oamBufferPos++ // 4 bytes
	;
	;	ROR(oamBuffer2Temp, lowbit(l->size))
	;	if oamBuffer2Temp underflowed
	;		oamBuffer2[oamBuffer2Pos] = oamBuffer2Temp
	;		oamBuffer2Pos++
	;		oamBuffer2Temp = 0x80
	;

	; The parameters of this function are not zeropage addresses because:
    ;   1. In order to effectively store them in a struct I need a 3rd index
    ;      register or modify DP (which I would have to then push/pop)
	;   2. xPos, yPos have to be calculated relative to the screen anyway
	;      which are probably thrown away, thus not worth the memory.
	;   3. Future modules (dynamic tiles, animated sprites) would
	;      need more bytes and thus are better handled by the caller.

	LDA	f:MetaSpriteLayoutBank << 16, X
	STA	nObjects


	LDY	oamBufferPos

	REPEAT
.A8
		; Y is index into oamBuffer
		CPY	#.sizeof(oamBuffer)
		BGE	_ProcessMetaSprite_BreakLoop


		; yPos
		LDA	f:MetaSpriteObjects + MetaSpriteObjectFormat::yPos, X
		REP	#$21		; also clear Carry, not set by ORA or AND
.A16
		IF_MINUS
			ORA	#$FF00
		ELSE
			AND	#$00FF
		ENDIF

		; unsigned comparison of a signed word variable
		ADC	yPos
		CMP	#.loword(-SPRITE_SIZE + 1)
		IF_LT
			CMP	#240
			BCS	SkipObject
		ENDIF

		SEP	#$20
.A8
		STA	oamBuffer + OamFormat::yPos, Y

		; xPos
		LDA	f:MetaSpriteObjects + MetaSpriteObjectFormat::xPos, X
		REP	#$21		; also clear Carry, not set by ORA or AND
.A16
		IF_MINUS
			ORA	#$FF00
		ELSE
			AND	#$00FF
		ENDIF

		; unsigned comparison of a signed word variable
		ADC	xPos
		CMP	#.loword(-SPRITE_SIZE + 1)
		IF_LT
			CMP	#$0100
			BCS	SkipObject
		ENDIF

		SEP	#$20
.A8
		STA	oamBuffer + OamFormat::xPos, Y

		; xpos bit 9
		XBA
		LSR
		ROR	oamBuffer2Temp


		; charAttr
		REP	#$31		; also clear carry
.A16

		LDA	charAttr
		ADC	f:MetaSpriteObjects + MetaSpriteObjectFormat::charAttr, X
		STA	oamBuffer + OamFormat::char, Y


		; increment oamBufferPos
		TYA
		ADD	#4
		TAY


		; size
		SEP	#$20
.A8
		LDA	f:MetaSpriteObjects + MetaSpriteObjectFormat::size, X
		LSR
		ROR	oamBuffer2Temp
		IF_C_SET
			PHY

			; populate oamBuffer2
			LDY	oamBuffer2Pos
			LDA	oamBuffer2Temp
			STA	oamBuffer2, Y

			INC	oamBuffer2Pos

			LDA	#$80
			STA	oamBuffer2Temp

			PLY
		ENDIF


SkipObject:
; A unknown
		SEP	#$20
.A8

		.assert .sizeof(MetaSpriteObjectFormat) <= 6, error, "Inefficient size, use ADC"
		.repeat .sizeof(MetaSpriteObjectFormat)
			INX
		.endrepeat

		DEC	nObjects
	UNTIL_ZERO

	STY	oamBufferPos

	RTS

ENDMODULE

