;; Resource Loader

.include "resourceloader.h"
.include "includes/import_export.inc"
.include "includes/structure.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"


MODULE ResourceLoader

.segment "ZEROPAGE"
	FARADDR	dataPtr

.segment "SHADOW"


.code


.A8
.I16
ROUTINE LoadPalette_16Y
	REP	#$30
.A16
	TYA
	BRA	_LoadPalette__16A

.A8
.I16
ROUTINE LoadPalette_8A
	REP	#$30
.A16

	AND	#$00FF
_LoadPalette__16A:

	ASL
	ASL
	TAX

	LDA	f:PalettesTable + PaletteTableFormat::palettePtr, X
	STA	A1T0

	LDA	f:PalettesTable + PaletteTableFormat::nColors, X
	AND	#$00FF
	ASL
	STA	DAS0

	SEP	#$20
.A8
	LDA	f:PalettesTable + PaletteTableFormat::palettePtr + 2, X
	STA	A1B0

	LDX	#DMAP_DIRECTION_TO_PPU | DMAP_TRANSFER_WRITE_TWICE | (.lobyte(CGDATA) << 8)
	STX	DMAP0

	LDA	#MDMAEN_DMA0
	STA	MDMAEN

	RTS



;
; Tiles4bpp Resource to VRAM
; ==========================
;

.A8
.I16
ROUTINE LoadVram_16Y
	LDA	#VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_1
	STA	VMAIN

	REP	#$30
.A16
	TYA
	BRA	_LoadVram__16A

.A8
.I16
ROUTINE LoadVram_8A
	SEP	#$10
.I8
	LDY	#VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_1
	STY	VMAIN

	REP	#$30
.A16
.I16

	AND	#$00FF
_LoadVram__16A:

	STA	dataPtr
	ASL
	ADD	dataPtr
	TAX

	LDA	f:VramTable + 1, X
	STA	dataPtr + 1
	LDA	f:VramTable, X
	STA	dataPtr
	TAY

	LDA	[dataPtr]
	ASL
	AND	#LoadVram_FormatTable_MASK
	TAX

	INY
	STY	dataPtr

	JMP	(.loword(LoadVram_FormatTable), X)


.rodata
;; Loads the data (which is 4bpp tiles) into VRAM.
;; INPUT:
;;	dataPtr - the source (byte AFTER format byte)
;;	Y - source addr (minus bank, byte AFTER format)
;;	VMAIN = VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_1
;;	16 bit A, 16 bit Index
;;	DB access registers
;; RETURN:
;;	8 bit A, 16 bit Index
.A8
.I16
LoadVram_FormatTable:
	.addr	LoadVram_Format_UnCompressed
LoadVram_FormatTable_MASK = $00
.code


.A16
.I16
ROUTINE	LoadVram_Format_UnCompressed
	LDA	[dataPtr]
	STA	DAS0

	INY
	INY
	STY	A1T0

	SEP	#$20
.A8
	LDA	dataPtr + 2
	STA	A1B0

	LDX	#DMAP_DIRECTION_TO_PPU | DMAP_TRANSFER_2REGS | (.lobyte(VMDATA) << 8)
	STX	DMAP0

	LDA	#MDMAEN_DMA0
	STA	MDMAEN
	RTS


;
; Data to WRAM
; ============
;

.A8
.I16
ROUTINE LoadDataToWram7E
	STA	dataPtr + 2
	STX	dataPtr

	LDA	[dataPtr]

	INX
	STX	dataPtr

	REP	#$30
.A16
	ASL
	AND	#LoadDataToWram7E_FormatTable_MASK
	TAX

	SEP	#$20
.A8
	JMP	(.loword(LoadDataToWram7E_FormatTable), X)



.A8
.I16
ROUTINE LoadDataToWram7F
	STA	dataPtr + 2
	STX	dataPtr

	LDA	[dataPtr]

	INX
	STX	dataPtr

	REP	#$30
.A16
	ASL
	ASL
	AND	#LoadDataToWram7F_FormatTable_MASK
	TAX

	JMP	(.loword(LoadDataToWram7F_FormatTable), X)


.rodata
;; Copies the data block to an address in bank $7E
;; INPUT:
;;	dataPtr - the source
;;	Y - the destination within WRAM7F bank
;; REQUIRES: 16 bit A, 16 bit Index.
;; RETURNS: 8 bit A, 16 bit Index
.A8
.I16
LoadDataToWram7E_FormatTable:
	.addr	DataToWram7E_Format_UnCompressed
LoadDataToWram7E_FormatTable_MASK = $00


;; Copies the data block to an address in bank $7F
;; INPUT:
;;	dataPtr - the source
;;	Y - the destination within WRAM7F bank
;; REQUIRES: 16 bit A, 16 bit Index.
;; RETURNS: 8 bit A, 16 bit Index
.A8
.I16
LoadDataToWram7F_FormatTable:
	.addr	DataToWram7F_Format_UnCompressed
LoadDataToWram7F_FormatTable_MASK = $00
.code



.A16
.I16
ROUTINE	DataToWram7F_Format_UnCompressed
	SEP	#$20
.A8
	LDA	#1
	STA	WMADDH
	BRA	_DataToWram_Format_UnCompressed_AfterSetBank


.A16
.I16
ROUTINE	DataToWram7E_Format_UnCompressed
	; MemCopy(dataPtr + 2, Y, *dataPtr)

	SEP	#$20
.A8
	STZ	WMADDH

_DataToWram_Format_UnCompressed_AfterSetBank:
	STY	WMADDL

	LDX	#DMAP_DIRECTION_TO_PPU | DMAP_TRANSFER_1REG | (.lobyte(WMDATA) << 8)
	STX	DMAP0

	LDX	dataPtr
	INX
	INX
	STX	A1T0
	LDA	dataPtr + 2
	STA	A1B0

	REP	#$30
.A16
	LDA	[dataPtr]
	STA	DAS0

	SEP	#$20
.A8
	LDA	#MDMAEN_DMA0
	STA	MDMAEN

	RTS

ENDMODULE

