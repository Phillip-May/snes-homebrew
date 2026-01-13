;;
;; Builds the SNES ROM headers
;;
;;
;; In order to use this file the following must be defined by
;; the `config.h`
;;
;;  * VERSION  - from 0 to 255
;;  * REGION   - NTSC or PAL
;;  * ROM_NAME - a string of the ROM name.

.include "includes/config.inc"

; Allow memmap to declare these symbols.
.import __ROMHEADER_EXPANSION_RAM
.import __ROMHEADER_MAP_MODE
.import __ROMHEADER_CART_TYPE
.import __ROMHEADER_ROM_SIZE
.import __ROMHEADER_SRAM_SIZE

.global ResetHandler
.global CopHandler
.global VBlank
.global IrqHandler
.global EmptyHandler

; 65816 processor
.setcpu "65816"

.if .xmatch(REGION, NTSC)
	__ROMHEADER_DESTINATION = $01 ; USA
.elseif .xmatch(REGION, PAL)
	__ROMHEADER_DESTINATION = $02 ; Europe
.else
	.error "Unkown Region"
.endif

.if VERSION > $FF .or VERSION < 0
	.error "Version must be a single byte"
.endif


.segment "ROMHEADER"
	.byte "  "					; $FFB0 2 Digit maker code (blank = unlicensed)
	.byte "    "					; $FFB2 4 Character game ID (black = unlicensed)

	.byte $00, $00, $00, $00, $00, $00, $00		; $FFB6 Fixed Value (7 bytes)

	.byte .lobyte(__ROMHEADER_EXPANSION_RAM)	; $FFBD Expansion RAM Size
	.byte $00					; $FFBE Special Version
	.byte $00					; $FFBF Cartridge Type Sub-Number

	.byte .sprintf("%21s", ROM_NAME)		; $FFC0 ROM Name (21 Characters)

	.byte .lobyte(__ROMHEADER_MAP_MODE)		; $FFD5 Map Mode
	.byte .lobyte(__ROMHEADER_CART_TYPE)		; $FFD6 Cartridge Type
	.byte .lobyte(__ROMHEADER_ROM_SIZE)		; $FFD7 ROM Size
	.byte .lobyte(__ROMHEADER_SRAM_SIZE)		; $FFD8 RAM Size
	.byte .lobyte(__ROMHEADER_DESTINATION)		; $FFD9 Destination code
	.byte $33					; $FFDA Fixed value
	.byte VERSION					; $FFDB ROM Version

	.word $AAAA					; $FFDC - Checksum compliment
	.word $5555					; $FFDE - Checksum


;; Define the Interrupt Vectors.
;;
;; `EmptyHandler` just links to an `RTI` and doesn't do anything.
;; If you are not using any of these handlers (like `CopHandler`)
;; Then just define it to `EmptyHandler`
;;
;; The Emulation mode vectors are empty as I don't code in it.
.segment "VECTORS"
	.addr 0					; $FFE0
	.addr 0					; $FFE2
	.addr CopHandler			; $FFE4 - Native COP
	.addr EmptyHandler			; $FFE6 - Native BRK
	.addr EmptyHandler			; $FFE8 - Native ABORT
	.addr VBlank				; $FFEA - Native NMI
	.addr 0					; $FFEC
	.addr IrqHandler			; $FFEE - Native IRQ

	.addr 0					; $FFF0
	.addr 0					; $FFF2
	.addr EmptyHandler			; $FFF4 - Emu COP
	.addr 0					; $FFF6
	.addr EmptyHandler			; $FFF8 - Emu ABORT
	.addr EmptyHandler			; $FFFA - Emu NMI
	.addr ResetHandler			; $FFFC - Emu RESET
	.addr 0					; $FFFE - Emu IRQ

.code

EmptyHandler:
	RTI

; vim: ft=asm:

