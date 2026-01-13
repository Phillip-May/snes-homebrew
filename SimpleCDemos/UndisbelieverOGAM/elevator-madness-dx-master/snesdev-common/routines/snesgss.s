
.include "snesgss.h"
.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/structure.inc"
.include "includes/registers.inc"

MODULE SnesGss

SNESGSS_SPC_INIT_LOAD = $0200
SPC700_START_PC       = $0200

MUSIC_LOAD_ADDR_MODULE_OFFSET = 14
SONG_N_MUSIC_CHANNELS_OFFSET = 2


.zeropage
	;; The location of the data to load into SPC
	FARADDR	dataPtr


.segment "SHADOW"
	;; The starting 'command value' of the SFX rotating channel counter
	BYTE	sfxRCStart

	;; The high nibble represents the max channel to use in the SFX
	;; rotating channel counter command
	;;
	;; `DEFAULT_SFXRC_MAX_CHANNEL << 4 | SnesGssCommand::SFX_PLAY`
	BYTE	sfxRCOverflow

	;; the command byte to send to play a SFX in the first free channel
	;; equal to `(channels in song) << 4 | SnesGssCommand::SFX_PLAY`
	;; incremented every `PlaySfxRC` call
	BYTE	sfxRCCommand
	BYTE	sfxVolume
	BYTE	sfxParam
	BYTE	sfxPan

	BYTE	stereoFlag

	;; Address in SPC-700 of the address to load the music to
	ADDR	musicLoadAddress

.code


; DB access registers
; DP = 0
.A8
.I16
ROUTINE Init
	.assert SNESGSS_INIT_SFXRC_MAX_CHANNEL <= 0 & SNESGSS_INIT_SFXRC_MAX_CHANNEL <=8, error, "Invalid SNESGSS_INIT_SFXRC_MAX_CHANNEL"
	LDA	#SNESGSS_INIT_SFXRC_MAX_CHANNEL << 4 | SnesGssCommand::SFX_PLAY
	STA	sfxRCOverflow

	LDA	#SnesGssCommand::SFX_PLAY
	STA	sfxRCCommand
	STA	sfxRCStart

	LDA	#127
	STA	sfxVolume
	INC			; 128
	STA	sfxPan

	; stereoFlag will be set by `Stereo` routine if non-zero
	.if SNESGSS_INIT_STEREO = 0
		STZ	stereoFlag
	.endif

	LDX	#.loword(SnesGss__Module + 2)
	LDA	#.bankbyte(SnesGss__Module)
	STX	dataPtr
	STA	dataPtr + 2

	REP	#$30
.A16
	LDA	f:SnesGss__Module + MUSIC_LOAD_ADDR_MODULE_OFFSET
	STA	musicLoadAddress

	LDA	f:SnesGss__Module
	TAX

	SEP	#$20
.A8
	LDY	#SNESGSS_SPC_INIT_LOAD

	CPX	#1 << 15 - 2 + 1
	IF_LT
		JSR	LoadSpcData_KnownSize
	ELSE
		; Module covers 2 ROM banks
		LDX	#1 << 15 - 2
		JSR	LoadSpcData_KnownSize

		LDX	#SnesGssCommand::LOAD
		JSR	Command

		LDA	#.bankbyte(SnesGss__ModulePart2)
		LDX	#.loword(SnesGss__ModulePart2)
		STX	dataPtr
		STA	dataPtr + 2

		REP	#$20
.A16
		LDA	f:SnesGss__Module
		SUB	#1 << 15 - 2
		TAX

		SEP	#$20
.A8
		LDY	#SNESGSS_SPC_INIT_LOAD + (1 << 15 - 2)
		JSR	LoadSpcData_KnownSize
	ENDIF

	LDX	#SnesGssCommand::INITIALIZE
	JSR	Command

	.if SNESGSS_INIT_STEREO <> 0
		.assert * = Stereo, lderror, "Bad Flow"
	.else
		RTS
	.endif


; DB  anywhere
.A8
.I16
ROUTINE Stereo
	LDA	#1
	STA	f:stereoFlag

	LDX	#SnesGssCommand::STEREO
	LDY	#1
	BRA	Command


; DB  anywhere
.A8
.I16
ROUTINE Mono
	LDA	#0
	STA	f:stereoFlag

	LDX	#SnesGssCommand::STEREO
	LDY	#0
	BRA	Command


.A8
.I16
ROUTINE	PauseMusic
	LDX	#SnesGssCommand::MUSIC_PAUSE
	LDY	#1
	BRA	Command


.A8
.I16
ROUTINE	UnPauseMusic
	LDX	#SnesGssCommand::MUSIC_PAUSE
	LDY	#0
	BRA	Command


.A8
.I16
ROUTINE	StopMusic
	LDA	#SnesGssCommand::SFX_PLAY
	STA	sfxRCCommand
	STA	sfxRCStart

	LDX	#SnesGssCommand::MUSIC_STOP
	BRA	Command

.A8
.I16
ROUTINE StopAllSounds
	LDX	#SnesGssCommand::STOP_ALL_SOUNDS
	BRA	Command


; IN: A = sfx
.A8
.I16
ROUTINE PlaySfx7
	.assert sfxParam + 1 = sfxPan, error, "Bad Value"

	STA	sfxParam
	LDY	sfxParam

	LDA	sfxVolume
	XBA
	LDA	#SnesGssCommand::SFX_PLAY | (7 << 4)
	TAX

	BRA	Command


; IN: A = sfx
.A8
.I16
ROUTINE PlaySfx6
	.assert sfxParam + 1 = sfxPan, error, "Bad Value"

	STA	sfxParam
	LDY	sfxParam

	LDA	sfxVolume
	XBA
	LDA	#SnesGssCommand::SFX_PLAY | (6 << 4)
	TAX

	BRA	Command


; IN: A = sfx
.A8
.I16
ROUTINE PlaySfxFirst
	.assert sfxParam + 1 = sfxPan, error, "Bad Value"

	STA	sfxParam
	LDY	sfxParam

	LDA	sfxVolume
	XBA
	LDA	sfxRCStart
	TAX

	BRA	Command


; IN: A = sfx
.A8
.I16
ROUTINE PlaySfxRC
	; ::ANNOY cant SNESGSS select the channel for me::

	.assert sfxRCCommand + 1 = sfxVolume, error, "Bad Value"
	.assert sfxParam + 1 = sfxPan, error, "Bad Value"

	STA	sfxParam
	LDY	sfxParam

	; incrementing buffer first makes no difference and is faster.

	LDA	sfxRCCommand
	ADD	#1 << 4
	CMP	sfxRCOverflow
	IF_GE
		LDA	sfxRCStart

		; ::BUGFIX SNESGSS will interrupt a note if sent a SFX_PLAY command on channel 8::
		CMP	sfxRCOverflow
		BGE	_Return
	ENDIF
	STA	sfxRCCommand

	LDX	sfxRCCommand

	.assert * = Command, lderror, "Bad Flow"



; IN:	X = command
;	Y = parameter
; DB anywhere
.A8
.I16
ROUTINE Command
	; OK to use long addressing here as we have to wait for the
	; SPC driver to process the state first.

	; Faster to use long addressing then to set DP
	; (and save old version on stack)

	REPEAT
		LDA	f:$800000 + APUIO0
	UNTIL_ZERO

	REP	#$20
.A16
	; The reference implementation writes `APUIO1` before `APUIO0`.
	; Reading the SPC700 code of SNESGSS shows 13 instructions pass
	; since reading APUIO0 and APUIO1 so were fine.

	TYA
	STA	f:$800000 + APUIO2
	TXA
	STA	f:$800000 + APUIO0

	SEP	#$20
.A8

	; Wait for acknowledgement of request
	REPEAT
		LDA	f:$800000 + APUIO0
	UNTIL_NOT_ZERO

_Return:
	RTS



.A8
.I16
ROUTINE PlayMusic
	.assert SnesGss__MusicTable_Count < 256, lderror, "Too many songs"

	CMP	#.lobyte(SnesGss__MusicTable_Count)
	IF_GE
		RTS
	ENDIF

	; AX = SnesGss__MusicTable[A * 3]
	REP	#$30
.A16
	AND	#$00FF
	STA	dataPtr
	ASL
	ADC	dataPtr		; carry clear from AND
	TAX

	LDA	f:SnesGss__MusicTable, X
	TAY

	SEP	#$20
.A8
	LDA	f:SnesGss__MusicTable + 2, X
	TYX

	.assert * = PlayMusicFromData, error, "Bad Flow"

.A8
.I16
ROUTINE PlayMusicFromData
	STA	dataPtr + 2
	STX	dataPtr

	LDY	#SONG_N_MUSIC_CHANNELS_OFFSET
	LDA	[dataPtr], Y
	ASL
	ASL
	ASL
	ASL
	ORA	#SnesGssCommand::SFX_PLAY
	STA	sfxRCCommand
	STA	sfxRCStart

	SEI
	STZ	NMITIMEN

	LDX	#SnesGssCommand::MUSIC_STOP
	JSR	Command

	LDX	#SnesGssCommand::LOAD
	JSR	Command

	LDY	musicLoadAddress
	JSR	LoadSpcData_SizeHeader

	LDA	stereoFlag
	IF_NOT_ZERO
		LDX	#SnesGssCommand::STEREO
		LDY	#1
		JSR	Command
	ENDIF

	CLI
	LDX	#SnesGssCommand::MUSIC_PLAY
	BRA	Command



.A8
.I16
ROUTINE ResetSfxVolume
	LDA	#127
	STA	SnesGss__sfxVolume
	INC			; 128
	STA	SnesGss__sfxPan
	RTS



;; Loads a headered data block to the SPC-700
;;
;; The first two bytes of the data (not loaded to SPC) are
;; the size of the block.
;;
;; REQUIRES: 8 bit A, 16 bit Index, DB access registers, DP = 0
;; INPUT: dataPtr - location of data
;;	  Y - SPC-700 Destination Address
;; OUTPUT: Interrupts disabled
.A8
.I16
ROUTINE LoadSpcData_SizeHeader
	; ::KUDOS Alpha-II Productions ::
	; ::: http://wiki.superfamicom.org/snes/show/Transferring+Data+from+ROM+to+the+SNES+APU ::

	; Get size of block, first two bytes are size
	REP	#$30
.A16
	LDA	[dataPtr]
	LDX	dataPtr
	INX
	INX
	STX	dataPtr

	TAX

	.assert * = LoadSpcData_KnownSize, error, "Bad Flow"


;; Loads a data block of a known size to the SPC-700
;;
;; REQUIRES: 8 bit A, 16 bit Index, DB access registers, DP = 0
;; INPUT: dataPtr - location of data
;;	  Y - SPC-700 Destination Address
;;	  X - the size of the data
;; OUTPUT: Interrupts disabled
.A8
.I16
ROUTINE LoadSpcData_KnownSize
	REP	#$30
.A16

	; Wait until ready
	LDA	#$BBAA
	REPEAT
		CMP	APUIO0
	UNTIL_EQ

	SEP	#$20
.A8

	; Disable interrupts
	SEI
	STZ	NMITIMEN

	; A is non-zero
	STA	APUIO1

	; SPC-700 Destination Address
	STY	APUIO2

	LDA	#$CC
	STA	APUIO0

	REPEAT
		CMP	APUIO0
	UNTIL_EQ

	LDY	#0
	REPEAT
		LDA	[dataPtr], Y
		STA	APUIO1

		TYA
		STA	APUIO0

		REPEAT
			CMP	APUIO0
		UNTIL_ZERO

		INY
		DEX
	UNTIL_ZERO


	; Set SPC-700 PC
	LDX	#SPC700_START_PC
	STX	APUIO2

	STZ	APUIO1

	TYA
	INC
	INC
	STA	APUIO0



	; Confirm SNESGSS code is running
	REP	#$30
.A16
	REPEAT
		LDA	APUIO0
		ORA	APUIO2
	UNTIL_ZERO

	SEP	#$20
.A8

	CLI
	RTS



ENDMODULE

