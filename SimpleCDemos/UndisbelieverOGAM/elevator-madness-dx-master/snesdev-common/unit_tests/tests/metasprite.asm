
.include "routines/metasprite.h"

; prevent compilation error
.export _MetaSprite_VBlank__Called = 1

.A8
.I16
PAGE_ROUTINE MetaSprite_1
	Text_SetColor	4
	Text_PrintStringLn "Meta Sprite Tests"
	Text_NewLine

	MetaSprite_Init

	JSR	Test_MetaSprite_ProcessSprite
	JSR	Test_MetaSprite_ProcessMeta_Single
	JSR	Test_MetaSprite_ProcessMeta_NegativeX
	JSR	Test_MetaSprite_ProcessMeta_OutOfBounds
	JSR	Test_MetaSprite_ProcessMeta_MultiSize
	JSR	Test_MetaSprite_EmptyLoop

	RTS


.macro TestOamBuffer expected, expectedEnd
	LDX	#0
	REPEAT
		LDA	MetaSprite__oamBuffer, X
		CMP	f:expected, X
		IF_NE
			Fail
			RTS
		ENDIF
		INX
		CPX	#expectedEnd - expected
	UNTIL_EQ
.endmacro

.macro TestOamBuffer2 expected, expectedEnd
	LDX	#0
	REPEAT
		LDA	MetaSprite__oamBuffer2, X
		CMP	f:expected, X
		IF_NE
			Fail
			RTS
		ENDIF
		INX
		CPX	#expectedEnd - expected
	UNTIL_EQ
.endmacro


ROUTINE Test_MetaSprite_EmptyLoop
	Text_SetColor	0

	Text_PrintString "Empty Loop Test (cleanup) "

	JSR	MetaSprite__InitLoop
	JSR	MetaSprite__FinalizeLoop

	LDX	#0
	LDY	#0
	REPEAT
		LDA	MetaSprite__oamBuffer + OamFormat::yPos, X
		CMP	#240
		IF_NE
			INY
		ENDIF

		.repeat 4
			INX
		.endrepeat
		CPX	#128*4
	UNTIL_EQ

	CPY	#0
	IF_EQ
		Pass
	ELSE
		Fail
	ENDIF

	RTS



ROUTINE Test_MetaSprite_ProcessSprite
	Text_SetColor	0
	Text_PrintString "ProcessSprite "

	JSR	MetaSprite__InitLoop

	LDX	#.loword(-5)
	STX	MetaSprite__xPos
	LDX	#128
	STX	MetaSprite__yPos
	LDY	#$FEED
	STY	MetaSprite__charAttr
	LDA	#0
	STA	MetaSprite__size

	JSR	MetaSprite__ProcessSprite

	JSR	MetaSprite__FinalizeLoop


	LDA	MetaSprite__oamBuffer + OamFormat::xPos
	CMP	#.lobyte(-5)
	IF_NE
		Fail
		RTS
	ENDIF

	LDA	MetaSprite__oamBuffer + OamFormat::yPos
	CMP	#128
	IF_NE
		Fail
		RTS
	ENDIF

	LDA	MetaSprite__oamBuffer + OamFormat::char
	CMP	#$ED
	IF_NE
		Fail
		RTS
	ENDIF

	LDA	MetaSprite__oamBuffer + OamFormat::attr
	CMP	#$FE
	IF_NE
		Fail
		RTS
	ENDIF

	LDA	MetaSprite__oamBuffer2
	AND	#%00000011
	CMP	#%00000001
	IF_NE
		Fail
		RTS
	ENDIF

	Pass

	RTS




ROUTINE Test_MetaSprite_ProcessMeta_Single
	Text_SetColor	0
	Text_PrintString "ProcessMeta (single) "

	JSR	MetaSprite__InitLoop

	LDX	#128
	STX	MetaSprite__xPos
	LDX	#128
	STX	MetaSprite__yPos
	LDX	#05 + 2 << OAM_CHARATTR_PALETTE_SHIFT
	STX	MetaSprite__charAttr

	LDX	#.loword(ExampleMetaSprite)
	JSR	MetaSprite__ProcessMetaSprite

	JSR	MetaSprite__FinalizeLoop

	TestOamBuffer ProcessMeta_Single_Expected, ProcessMeta_Single_ExpectedEnd
	TestOamBuffer2 ProcessMeta_Single_Expected2, ProcessMeta_Single_Expected2End

	Pass

	RTS


ROUTINE Test_MetaSprite_ProcessMeta_NegativeX
	Text_SetColor	0
	Text_PrintString "ProcessMeta (negative X) "

	JSR	MetaSprite__InitLoop

	LDX	#0
	STX	MetaSprite__xPos
	LDX	#128
	STX	MetaSprite__yPos
	LDX	#05 + 2 << OAM_CHARATTR_PALETTE_SHIFT
	STX	MetaSprite__charAttr

	LDX	#.loword(ExampleMetaSprite)
	JSR	MetaSprite__ProcessMetaSprite

	JSR	MetaSprite__FinalizeLoop

	TestOamBuffer ProcessMeta_NegativeX_Expected, ProcessMeta_NegativeX_ExpectedEnd
	TestOamBuffer2 ProcessMeta_NegativeX_Expected2, ProcessMeta_NegativeX_Expected2End

	Pass

	RTS

; Tests all 4 boundaries with 3 meta-sprites
ROUTINE Test_MetaSprite_ProcessMeta_OutOfBounds
	Text_SetColor	0
	Text_PrintString "ProcessMeta (bounds) "

	JSR	MetaSprite__InitLoop

	LDX	#.loword(-6)
	STX	MetaSprite__xPos
	LDX	#.loword(-6)
	STX	MetaSprite__yPos
	LDX	#05 + 2 << OAM_CHARATTR_PALETTE_SHIFT
	STX	MetaSprite__charAttr

	LDX	#.loword(ExampleMetaSprite)
	JSR	MetaSprite__ProcessMetaSprite

	LDX	#250
	STX	MetaSprite__xPos
	LDX	#235
	STX	MetaSprite__yPos
	LDX	#05 + 2 << OAM_CHARATTR_PALETTE_SHIFT
	STX	MetaSprite__charAttr

	LDX	#.loword(ExampleMetaSprite)
	JSR	MetaSprite__ProcessMetaSprite

	LDX	#128
	STX	MetaSprite__xPos
	LDX	#128
	STX	MetaSprite__yPos
	LDX	#05 + 4 << OAM_CHARATTR_PALETTE_SHIFT
	STX	MetaSprite__charAttr

	LDX	#.loword(ExampleMetaSprite)
	JSR	MetaSprite__ProcessMetaSprite

	JSR	MetaSprite__FinalizeLoop

	TestOamBuffer ProcessMeta_OutOfBounds_Expected, ProcessMeta_OutOfBounds_ExpectedEnd
	TestOamBuffer2 ProcessMeta_OutOfBounds_Expected2, ProcessMeta_OutOfBounds_Expected2End

	Pass

	RTS


; Tests all 2 different sized meta sprites
ROUTINE Test_MetaSprite_ProcessMeta_MultiSize
	Text_SetColor	0
	Text_PrintString "ProcessMeta (size) "

	JSR	MetaSprite__InitLoop

	LDX	#100
	STX	MetaSprite__xPos
	LDX	#100
	STX	MetaSprite__yPos
	LDX	#256 + 3 << OAM_CHARATTR_PALETTE_SHIFT
	STX	MetaSprite__charAttr

	LDX	#.loword(MultiSizeMetaSprite)
	JSR	MetaSprite__ProcessMetaSprite

	LDX	#128
	STX	MetaSprite__xPos
	LDX	#128
	STX	MetaSprite__yPos
	LDX	#5 + 2 << OAM_CHARATTR_PALETTE_SHIFT
	STX	MetaSprite__charAttr

	LDX	#.loword(ExampleMetaSprite)
	JSR	MetaSprite__ProcessMetaSprite

	JSR	MetaSprite__FinalizeLoop

	TestOamBuffer ProcessMeta_MultiSize_Expected, ProcessMeta_MultiSize_ExpectedEnd
	TestOamBuffer2 ProcessMeta_MultiSize_Expected2, ProcessMeta_MultiSize_Expected2End

	Pass

	RTS


.segment "BANK1"

MetaSpriteLayoutBank := .bankbyte(*)

; This example draws 4 OAM objects in the 4 cardinals around the center point.
ExampleMetaSprite:
	.byte 4
		; up
		.byte .lobyte(0)
		.byte .lobyte(-10)
		.word 0
		.byte 0

		; down
		.byte .lobyte(0)
		.byte .lobyte(10)
		.word 0 | OAM_CHARATTR_V_FLIP_FLAG
		.byte 0

		; left
		.byte .lobyte(-10)
		.byte .lobyte(0)
		.word 1
		.byte 0

		; right
		.byte .lobyte(10)
		.byte .lobyte(0)
		.word 1 | OAM_CHARATTR_H_FLIP_FLAG
		.byte 0


; This example draws 2 OAM objects, an 8x8 and a 16x16
MultiSizeMetaSprite:
	.byte 2
		.byte .lobyte(0)
		.byte .lobyte(0)
		.word 0
		.byte 1

		.byte .lobyte(0)
		.byte .lobyte(-16)
		.word 1
		.byte 0


ProcessMeta_MultiSize_Expected:
	; multi sized meta sprite: 100, 100, start 256, palette 3
	.byte	100, 100, 0, 3 << OAM_ATTR_PALETTE_SHIFT | 1
	.byte	100,  84, 1, 3 << OAM_ATTR_PALETTE_SHIFT | 1

	; meta sprite: 128, 128, start 5, palette 2
	.byte	128, 118, 5, 2 << OAM_ATTR_PALETTE_SHIFT
	.byte	128, 138, 5, 2 << OAM_ATTR_PALETTE_SHIFT | OAM_ATTR_V_FLIP_FLAG
	.byte	118, 128, 6, 2 << OAM_ATTR_PALETTE_SHIFT
	.byte	138, 128, 6, 2 << OAM_ATTR_PALETTE_SHIFT | OAM_ATTR_H_FLIP_FLAG
ProcessMeta_MultiSize_ExpectedEnd:
ProcessMeta_MultiSize_Expected2:
	.byte	%00000010
	.byte	%00000000
ProcessMeta_MultiSize_Expected2End:


ProcessMeta_Single_Expected:
	; meta sprite: 128, 128, start 5, palette 2
	.byte	128, 118, 5, 2 << OAM_ATTR_PALETTE_SHIFT
	.byte	128, 138, 5, 2 << OAM_ATTR_PALETTE_SHIFT | OAM_ATTR_V_FLIP_FLAG
	.byte	118, 128, 6, 2 << OAM_ATTR_PALETTE_SHIFT
	.byte	138, 128, 6, 2 << OAM_ATTR_PALETTE_SHIFT | OAM_ATTR_H_FLIP_FLAG
ProcessMeta_Single_ExpectedEnd:
ProcessMeta_Single_Expected2:
	.byte	0
ProcessMeta_Single_Expected2End:

ProcessMeta_NegativeX_Expected:
	; meta sprite: 0, 128, start 5, palette 2
	.byte	0, 118, 5, 2 << OAM_ATTR_PALETTE_SHIFT
	.byte	0, 138, 5, 2 << OAM_ATTR_PALETTE_SHIFT | OAM_ATTR_V_FLIP_FLAG
	.byte	.lobyte(-10), 128, 6, 2 << OAM_ATTR_PALETTE_SHIFT
	.byte	10, 128, 6, 2 << OAM_ATTR_PALETTE_SHIFT | OAM_ATTR_H_FLIP_FLAG
ProcessMeta_NegativeX_ExpectedEnd:
ProcessMeta_NegativeX_Expected2:
	.byte	%00010000
ProcessMeta_NegativeX_Expected2End:

ProcessMeta_OutOfBounds_Expected:
	; meta sprite 0: -6, -6, start 5, palette 2
	; out of bounds
	.byte	.lobyte(-6), 4, 5, 2 << OAM_ATTR_PALETTE_SHIFT | OAM_ATTR_V_FLIP_FLAG
	; out of bounds
	.byte	4, .lobyte(-6), 6, 2 << OAM_ATTR_PALETTE_SHIFT | OAM_ATTR_H_FLIP_FLAG

	; meta sprite 1: 250, 235, start 5, palette 2
	.byte	250, 225, 5, 2 << OAM_ATTR_PALETTE_SHIFT
	; out of bounds
	.byte	240, 235, 6, 2 << OAM_ATTR_PALETTE_SHIFT
	; out of bounds

	; meta sprite 2: 128, 128, start 5, palette 4
	.byte	128, 118, 5, 4 << OAM_ATTR_PALETTE_SHIFT
	.byte	128, 138, 5, 4 << OAM_ATTR_PALETTE_SHIFT | OAM_ATTR_V_FLIP_FLAG
	.byte	118, 128, 6, 4 << OAM_ATTR_PALETTE_SHIFT
	.byte	138, 128, 6, 4 << OAM_ATTR_PALETTE_SHIFT | OAM_ATTR_H_FLIP_FLAG
ProcessMeta_OutOfBounds_ExpectedEnd:
ProcessMeta_OutOfBounds_Expected2:
	.byte	%00000001
	.byte	%00000000
ProcessMeta_OutOfBounds_Expected2End:

.code

