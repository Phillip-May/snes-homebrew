;; A simple Meta-Sprite Handler.
;;
;;
;;
;;
;; Meta Sprite Layout format
;; =========================
;;
;; 	.db number Of Objects
;;	// repeated for each object
;;		.byte	xPos (signed)
;;		.byte	yPos (signed)
;;		.word	charAttr
;;		.byte	size (only the lowest bit is used)
;;

.ifndef ::_METASPRITE_H_
::_METASPRITE_H_ := 1

.setcpu "65816"

.include "includes/import_export.inc"
.include "includes/registers.inc"

.ifndef SPRITE_SIZE
	SPRITE_SIZE = 16
.endif

.struct MetaSpriteObjectFormat
	xPos		.byte
	yPos		.byte
	charAttr	.word
	size		.byte
.endstruct

;; A label that points to the bank that the MetaSprite layouts reside in
.global MetaSpriteLayoutBank:zp


IMPORT_MODULE MetaSprite

	STRUCT	oamBuffer, OamFormat, 128
	BYTE	oamBuffer2, 128 / 4

	BYTE	updateOam

	WORD	oamBufferPos
	WORD	prevOamBufferPos

	WORD	oamBuffer2Pos
	BYTE	oamBuffer2Temp

	WORD	xPos
	WORD	yPos
	WORD	charAttr
	BYTE	size


	;; Initializes the MetaSprite module
	;; **Must** be used *before* `InitLoop`
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	.macro MetaSprite_Init
		.export _MetaSprite_Init__Called = 1

		; updateOam = 1
		; prevOamBufferPos = sizeof(oamBuffer)

		STZ	MetaSprite__updateOam

		LDX	#MetaSprite__oamBuffer__size
		STX	MetaSprite__prevOamBufferPos
	.endmacro


	;; Updates the OAM during VBlank
	;;
	;; Must be called during v-blank or f-blank
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	.macro MetaSprite_VBlank
		.export _MetaSprite_VBlank__Called = 1

		LDA	MetaSprite__updateOam
		IF_NOT_ZERO
			STZ	OAMADDL
			STZ	OAMADDH

			LDX	#DMAP_DIRECTION_TO_PPU | DMAP_TRANSFER_1REG | (.lobyte(OAMDATA) << 8)
			STX	DMAP0			; also sets BBAD0

			LDX	#128 * 4 + 128 / 4
			STX	DAS0

			LDX	#.loword(MetaSprite__oamBuffer)
			STX	A1T0
			STZ	A1B0
			.assert .bankbyte(MetaSprite__oamBuffer) = 0, lderror, "oamBuffer bank not 0"

			LDA	#MDMAEN_DMA0
			STA	MDMAEN

			STZ	MetaSprite__updateOam
		ENDIF
	.endmacro


	;; Initializes the OAM buffers.
	;;
	;; Call at the start of every display frame.
	;;
	;; REQUIRES: 8 bit A, 16 bit index, Shadow RAM accessible
	ROUTINE InitLoop

	;; Finalize the OAM buffer and prepares it for transfer.
	;;
	;; Call *after* the OAM buffer is populated by Process()
	;;
	;; REQUIRES: 8 bit A, 16 bit index, Shadow RAM accessible
	;;
	;; MODIFIES: A, X
	ROUTINE	FinalizeLoop

	;; Add a sprite to the OAM buffer.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, Shadow RAM accessible
	;;
	;; INPUT:
	;;	xPos      the sprite's X position on the screen (9 bit signed, const)
	;;	yPos      the sprite's Y position on the screen (const)
	;;	charAttr  the char & attr object bytes to use (const)
	;;	size      the size bit of the OAM high Table (const)
	;;
	;; MODIFIES: A, X, Y
	ROUTINE	ProcessSprite

	;; Add a meta-sprite to the OAM buffer.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, Shadow RAM accessible
	;;
	;; INPUT:
	;;	xPos      the sprite's X position on the screen (const)
	;;	yPos      the sprite's Y position on the screen (const)
	;;	charAttr  the char & attr object bytes to use (const)
	;;	X         location of the meta-sprite data in the DB `MetaSpriteLayoutBank`
	;;
	;; MODIFIES: A, X, Y
	ROUTINE	ProcessMetaSprite

	;; Add a meta-sprite to the OAM buffer. This version allows for the `charAttr` variable to be stored in Y.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, Shadow RAM accessible
	;;
	;; INPUT:
	;;	xPos      the sprite's X position on the screen (const)
	;;	yPos      the sprite's Y position on the screen (const)
	;;	Y         the char & attr object bytes to use
	;;	X         location of the meta-sprite data in the DB `MetaSpriteLayoutBank`
	;;
	;; MODIFIES: A, X, Y
	ROUTINE	ProcessMetaSprite_Y

ENDMODULE

.endif

; vim: ft=asm:

