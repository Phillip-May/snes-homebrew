.ifndef ::__SNESGSS_H_
::__SNESGSS_H_ = 1

.setcpu "65816"
.include "includes/import_export.inc"
.include "includes/config.inc"

;; The maximum channel to use in the sfx rotating channel count.
CONFIG SNESGSS_INIT_SFXRC_MAX_CHANNEL, 8

;; Default state of the stereo/mono flag (0 = mono, 1 = stereo)
CONFIG SNESGSS_INIT_STEREO, 0


;; An implementation of the SNES GSS (Game Sound System) ABI.
;;
;; As the SNESGSS engine remember the state of the mono/stereo flag
;; when reset (ie, LOAD command) it is stored on the SNES side and
;; processed on every music load.
;;
;; As the SNESGSS engine itself does not cycle sound effects through the
;; various SPC-700 sound channels, this module does it instead.
;;
;; This is achieved though `SnesGss__PlaySfxRC`, which uses a rotating
;; channel count that is incremented on every call.
;; This allows a simple API to play sound effects through, but limits the
;; number of channels an effect can use to 1.
;; It is annoying that SNESGSS does not have this functionality built-in.
;;
;;
;; Currently it does not implement the streaming functionality of SNESGSS,
;; that may be implemented later.
IMPORT_MODULE SnesGss

	.enum SnesGssCommand
		NONE		= $00

		;; Initialize DSP registers
		INITIALIZE	= $01

		;; Load new music
		;; Should not be called manually
		LOAD		= $02

		;; Sets mono/stereo state of SNESGSS
		;; LOBYTE OF PARAM: non-zero for stereo, zero for mono
		STEREO		= $03

		;; Fades in/out the global volume to a given value with a given speed.
		;; LOBYTE OF PARAM: Volume (max 127)
		;; HIBYTE OF PARAM: Volume change speed (1..255 - slowest to fastest)
		GLOBAL_VOLUME	= $04

		;; Sets the volume of one or more channels
		;; LOBYTE OF PARAM: Volume (max 127)
		;; HIBYTE OF PARAM: Channel Mask
		CHANNEL_VOLUME	= $05

		;; Starts the currently loaded song
		;; Should not be called manually
		MUSIC_PLAY	= $06

		;; Stops the current music
		MUSIC_STOP	= $07

		;; Pauses or unpauses music
		;; LOBYTE OF PARAM: pause state (non-zero = paused, zero = paused)
		MUSIC_PAUSE	= $08

		;; Plays a sound effect 
		;; LOBYTE OF COMMAND: lonibble = $09, hinibble = starting channel
		;; HIBYTE OF COMMAND: volume
		;; LOBYTE OF PARAM:   effect number
		;; HIBYTE OF PARAM:   pan
		SFX_PLAY	= $09

		;; Stop all sounds
		STOP_ALL_SOUNDS	= $0A

		STREAM_START	= $0B
		STREAM_STOP	= $0C
		STREAM_END	= $0D
	.endenum


	;; Sets the maximum channel count to use in the sfx rotating channel count.
	;; This value is reset on Init
	;; REQUIRES: 8 bit A, DB access shadow
	.macro SnesGss_SetSfxRcMaxChannel max
		.global SnesGss__sfxRCOverflow

		LDA	#(max) << 4 | SnesGssCommand::SFX_PLAY
		STA	SnesGss__sfxRCOverflow
	.endmacro

	;; The volume to play the sound effect at
	;; Will be clipped to 127
	BYTE	sfxVolume
	;; The pan value of the sound effect to play
	BYTE	sfxPan

	;; The state of the stereo/mono flag in SNESGSS (0 = mono, 1 = stereo)
	;; It is referenced on every music load.
	BYTE	stereoFlag

	;; The first 32KiB of the spc700.bin file created by snesgss GUI
	.global SnesGss__Module
	;; The second half of the spc700.bin file (if file >= 32KiB)
	.global SnesGss__ModulePart2

	;; A table of 24bit addresses for each song in play
	.global	SnesGss__MusicTable
	;; Number of items in `MusicTable`. Must be below 256
	.globalzp SnesGss__MusicTable_Count


	;; Initializes SNESGSS, loads the module into the SPC-700
	;; REQUIRES: 8 bit A, 16 bit Index, DB access registers, DP = 0
	;;	     `SnesGss__Module` pointing to the location of the snesgss module.
	;;	     `SnesGss__ModulePart2` pointing to the location of the second half
	;;		of the module if module size > 32KiB in size
	;;
	;; This routine will disable interrupts
	ROUTINE Init

	;; Loads a song into SNESGSS from the music table and plays it.
	;;
	;; The table is called `SnesGss__MusicTable` and consists of
	;; of the far (24 bit) addresses of each song in the music table.
	;;
	;; This routine will set the SNESGSS stereo flag to the value of
	;; `stereoFlag` and will initialise the sound effect rotation count.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, DB access registers, DP = 0
	;;	     `SnesGss__MusicTable` - a table of 24 bit addresses for each song
	;;	     `SnesGss__MusicTable_Count` - the number of songs in the table
	;;
	;; INPUT: A - song number
	;;
	;; This routine will disable interrupts
	ROUTINE PlayMusic

	;; Loads a song into SNESGSS from a data block and plays it
	;;
	;; This routine will set the SNESGSS stereo flag to the value of
	;; `stereoFlag` and will initialise the sound effect rotation count.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, DB access registers, DP = 0
	;; INPUT: A - song number
	;;	  X - data address
	;;
	;; This routine will disable interrupts
	ROUTINE PlayMusicFromData

	;; Plays a sound effect on SPC-700 channel 7.
	;;
	;; Sound effects played by this routine MUST only involve 1 channel.
	;; Otherwise the engine will clip them
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, DB access shadow
	;; INPUT: A - sound effect number
	;;	  sfxVolume - the volume of the SFX
	;;	  sfxPan - the panning value of the SFX
	ROUTINE PlaySfx7

	;; Plays a sound effect on SPC-700 channel 6.
	;;
	;; Sound effects played by this routine MUST only involve 1 channel.
	;; Otherwise effects played here may override `PlaySfx7`
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, DB access shadow
	;; INPUT: A - sound effect number
	;;	  sfxVolume - the volume of the SFX
	;;	  sfxPan - the panning value of the SFX
	ROUTINE PlaySfx6

	;; Plays a sound effect using the first channel dedicated to SFX.
	;;
	;; Sound effects played by this routine can involve more than
	;; one channel.
	;;
	;; Effects played here will override ones activated with
	;; `PlaySfxRC`, `PlaySfx6` and `PlaySfx7`
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, DB access shadow
	;; INPUT: A - sound effect number
	;;	  sfxVolume - the volume of the SFX
	;;	  sfxPan - the panning value of the SFX
	ROUTINE PlaySfxFirst

	;; Plays a sound effect using a rotating channel counter so
	;; that they are not overridden.
	;;
	;; Sound effects played by this routine MUST only involve 1 channel.
	;;
	;; Effects played here will override `PlaySfx6` and `PlaySfx7`
	;; UNLESS the sfx max channel is set to 6 or 7 respectively.
	;;
	;; Effects played here will override `PlaySfxFirst`.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, DB access shadow
	;; INPUT: A - sound effect number
	;;	  sfxVolume - the volume of the SFX
	;;	  sfxPan - the panning value of the SFX
	ROUTINE PlaySfxRC

	;; Resets the sfxVolume and sfxPan values to their initial values
	;; REQUIRES: 8 bit Index, DB access shadow
	ROUTINE	ResetSfxVolume

	;; Sets mono sound.
	;;
	;; Will set `stereoFlag` to zero.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	ROUTINE	Mono

	;; Sets stereo sound
	;;
	;; Will set `stereoFlag` to 1.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	ROUTINE	Stereo

	;; Pauses the music
	;; REQUIRE: 8 bit A, 16 bit Index
	ROUTINE	PauseMusic

	;; Unpauses the music
	;; REQUIRE: 8 bit A, 16 bit Index
	ROUTINE	UnPauseMusic

	;; Stops the music
	;; REQUIRE: 8 bit A, 16 bit Index
	ROUTINE	StopMusic

	;; Stops the all sounds
	;; REQUIRE: 8 bit A, 16 bit Index
	ROUTINE StopAllSounds

	;; Fade In/Out the global volume.
	;; REQUIRE: 8 bit A, 16 bit Index
	;; INPUT: A - the volume (clipped to 127)
	;;	  Y - fade speed (1..255 (slowest to fastest)
	ROUTINE	GlobalVolume

	;; Set volume of audio channels
	;; REQUIRE: 8 bit A, 16 bit Index
	;; INPUT: A - the volume (clipped to 127)
	;;	  Y - the channels to set (bit mask for each channel 0 - 7)
	ROUTINE	ChannelVolume

	;; Sends a command to the SNES GSS module
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, DB anywhere
	;; 
	;; INPUT: X = command
	;;	  Y = parameter
	ROUTINE Command



	;; Plays a fixed sound effect using the command interface.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	;;
	;; PARAM: channel: (immediate) the minimum channel number, must be >= number channels in music < 8
	;;	  sfx: (immediate) the sound effect to play (>= 0 and <= 255)
	;;	  volume: (immediate) the volume of the sound effect to play (>= 0 and <= 127)
	;;	  pan: (immediate) the pan value of the sound effect to play (>= 0 and <= 255)
	.macro	SnesGss_Command_PlaySfx channel, sfx, volume, pan
		.assert (channel) >= 0 && (channel) <= 7, error, "Invalid channel (must be < 8)"
		.assert (sfx) >= 0 && (sfx) <= 255, error, "sfx must be >= 0 and <= 255"
		.assert (volume) >= 0 && (volume) <= 127, error, "volume must be >= 0 and <= 127"
		.assert (pan) >= 255 && (pan) <= 255, error, "pan must be >= 0 and <= 255" 

		LDX	#(SnesGssCommand::SFX_PLAY | ((channel) << 4) | ((volume) << 8)
		LDY	#(sfx) | ((pan) << 8)
		JSR_SnesGss_Command
	.endmacro
ENDMODULE

.endif ; __SNESGSS_H_

; vim: set ft=asm:

