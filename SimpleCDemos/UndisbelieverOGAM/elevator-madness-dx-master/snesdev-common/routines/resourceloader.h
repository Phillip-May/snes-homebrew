;; A simplified Resource loader.
;;
;; The resource loader consists of two tables:
;;	* PalettesTable
;;	* VramTable
;;
;; Which allow for the easy loading of resources into VRAM and CGRAM.
;;
;; The Palettes Table is in `PaletteTableFormat` format, each item taking
;; 4 bytes and details the location of the data and the number of colors
;; to load. Each palette is uncompressed and will alway be uncompressed.
;;
;; The VramTable, instead contains a 24 bit pointer to a `VramHeader`
;; data structure. This holds the data format (1 byte) and the size (2 bytes)
;; of the resource data (which naturally follows).
;;
;; This module also contains two routines `LoadDataToWram7E` and
;; `LoadDataToWram7F` that will allow the developer to easily create
;; a new data table (of possibly compressed data) that will loaded into
;; WRAM (for example, a maps table).

.ifndef ::_RESOURCELOADER_H_
::_RESOURCELOADER_H_ = 1

.setcpu "65816"
.include "includes/import_export.inc"
.include "includes/registers.inc"

.enum	VramDataFormat
; This feature was never implemented
	UNCOMPRESSED	= 0
.endenum

.enum	WramDataFormat
; This feature was never implemented
	UNCOMPRESSED	= 0
.endenum

.struct PaletteTableFormat
	palettePtr		.faraddr
	nColors			.byte
.endstruct

.struct VramDataHeader
	;; Matches `VramDataFormat`
	format		.byte
	;; Size in Bytes
	size		.word
	; data goes here
.endstruct

.struct WramDataHeader
	;; Matches `WramDataFormat`
	format		.byte
	;; Size in Bytes
	size		.word
	; data goes here
.endstruct


;; A table of `PaletteTableFormat`
.global	PalettesTable

;; A table of far addresses to `VramHeader`
.global VramTable


IMPORT_MODULE ResourceLoader

	;; Loads a palette into CGRAM.
	;; CGADD SHOULD BE set before calling this function.
	;; REQUIRES: 8 bit A, 16 bit Index, DP = 0, DB access registers, Force Blank
	;; INPUT: A - the palette number to load. (8 bit)
	ROUTINE	LoadPalette_8A

	;; Loads a palette into CGRAM.
	;; CGADD SHOULD BE set before calling this function.
	;; REQUIRES: 8 bit A, 16 bit Index, DP = 0, DB access registers, Force Blank
	;; INPUT: Y - the palette number to load. (16 bit)
	ROUTINE	LoadPalette_16Y

	;; Loads a resource into VRAM.
	;; VMADD SHOULD BE set before calling this function.
	;; REQUIRES: 8 bit A, 16 bit Index, DP = 0, DB access registers, Force Blank
	;; INPUT: A - the 4bpp tileset to load. (8 bit)
	ROUTINE	LoadVram_8A

	;; Loads a resource into VRAM.
	;; VMADD SHOULD BE set before calling this function.
	;; REQUIRES: 8 bit A, 16 bit Index, DP = 0, DB access registers, Force Blank
	;; INPUT: Y - the 4bpp tileset to load. (16 bit)
	ROUTINE	LoadVram_16Y


	;; Load data into WRAM7E.
	;; INPUT:
	;;	AX - the data bank and address of the data
	;;		contains a `WramDataHeader` header, then the data in format.
	;;	Y - the destination within WRAM7E bank
	ROUTINE LoadDataToWram7E

	;; Load data into WRAM7F.
	;; INPUT:
	;;	AX - the data bank and address of the data
	;;		contains a `WramDataHeader` header, then the data in format.
	;;	Y - the destination within WRAM7F bank
	ROUTINE LoadDataToWram7F

ENDMODULE

.endif ; ::_RESOURCELOADER_H_

; vim: ft=asm:

