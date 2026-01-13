;
; Header for Reset SNES
;

.ifndef ::_RESET_H_
::_RESET_H_ := 1

.setcpu "65816"

.include "includes/import_export.inc"

;; Link to Reset::ResetSNES
IMPORT_ROUTINE ResetHandler

IMPORT_MODULE Reset

	;; Reset Handler
	;;
	;;
	;; Resets
	;;    * WRAM
	;;    * SNES Registers to a basic default (except mode 7 Matrix)
	;;    * VRAM
	;;    * OAM
	;;    * CGRAM
	ROUTINE ResetSNES


	;; Resets most of the Registers in the SNES to their recommended defaults.
	;;
	;; Defaults:
	;;     * Forced Screen Blank
	;;     * Mode 0
	;;     * OAM Size 8x8, 16x16, base address 0
	;;     * BG Base address 0, size = 8x8
	;;     * No Mosaic
	;;     * No BG Scrolling
	;;     * VRAM Increment on High Byte
	;;     * No Windows
	;;     * No Color Math
	;;     * No Backgrounds
	;;     * No HDMA
	;;     * ROM access to slow
	;;
	;; This routine does not set the following as any programmer need to set them anyway:
	;;     * the Mode 7 Matrix
	;;     * VRAM/CGRAM/OAM data address registers
	;;
	;; Also sets:
	;;  * Program Bank = Data Bank
	ROUTINE ResetRegisters


	;; Transfers 0x10000 0 bytes to VRAM
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; Uses DMA Channel 0 to do so.
	ROUTINE ClearVRAM


	;; Clears the Sprites off the screen.
	;;
	;; Sets:
	;;	* X position to -128 (can't use -256, as it would be counted in the scanlines)
	;;	* Y position to 240 (outside rendering area for 8x8 and 16x16 sprites)
	;;	* Character 0
	;;	* No Priority, no flips
	;;	* Small Size
	;;
	;; NOTICE: This sets both OAM tables.
	ROUTINE ClearOAM


	;; Clears the CGRAM
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	ROUTINE ClearCGRAM

ENDMODULE

.endif

; vim: ft=asm:

