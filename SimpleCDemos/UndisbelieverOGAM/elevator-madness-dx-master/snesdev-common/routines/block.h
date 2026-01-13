;; SNES Memory Block

.ifndef ::__BLOCK_H_
::__BLOCK_H_ = 1

.setcpu "65816"
.include "includes/import_export.inc"

IMPORT_MODULE Block
	;; Clears a section of memory to 0
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	;; NOTE: This routine uses DMA channel 0 to reset the block.
	;; INPUT: A = bank
	;;	: X = memory address
	;;	: Y = size
	ROUTINE	MemClear

	;; Reset a block of memory to 0
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	;;
	;; NOTE: This routine uses DMA channel 0 to reset the block.
	;;
	;; size can be omitted if:
	;;	* the label {ptr}End exists denoting the end of the data
	;;	* the label {ptr}__size exists.
	.macro MemClear ptr, size
		_Block_Addr_Size_Helper ptr, size, Block__MemClear
	.endmacro

	;; Copies a block of memory in ROM to WMDATA register
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, WMADD set
	;; NOTE: This routine uses DMA channel 0 to reset the block.
	;; INPUT: A = bank
	;;	: X = memory address
	;;	: Y = size
	ROUTINE	CopyToWmdata

	;; Copies a block of memory in ROM to WMDATA register
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, WMADD set
	;; NOTE: This routine uses DMA channel 0 to reset the block.
	;;
	;; size can be omitted if:
	;;	* the label {source}End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro MemCopy source, destination, size
		.assert .not(((.bankbyte(source) & $7F < $40 .and .loword(source) < $2000) .or (.bankbyte(source) = $7E) .or (.bankbyte(source) = $7F))), error, "source must not be in RAM"

		.assert ((.bankbyte(destination) & $7F < $40 .and .loword(destination) < $2000) .or (.bankbyte(destination) = $7E) .or (.bankbyte(destination) = $7F)), error, "destination must be in RAM"

		LDX	#.loword(destination)
		LDA	#.bankbyte(destination)
		STX	WMADD
		STA	WMADD + 2

		_Block_Addr_Size_Helper source, size, Block__CopyToWmdata
	.endmacro

	;; Clears a block of VRAM.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;; NOTE: This routine uses DMA channel 0 to reset the block.
	;; INPUT
	;;	: X = VRAM word location
	;;	: Y = size
	ROUTINE ClearVramLocation

	;; Clears a block of VRAM at a given location.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	.macro ClearVramLocation vramWordLocation, size
		LDX	#vramWordLocation
		LDY	#size
		JSR	Block__ClearVramLocation
	.endmacro

	;; Transfer a block to memory to VRAM.
	;;
	;; The VRAM Address is not set by this routine. It must be set set before use.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;; NOTE: This routine uses DMA channel 0 to reset the block.
	;; INPUT: A = bank
	;;	: X = memory address
	;;	: Y = size
	ROUTINE TransferToVram

	;; Transfer a block of memory to VRAM.
	;; The VRAM Address is not set by this routine. It must be set set before use.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;;
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	;;
	;; size can be omitted if:
	;;	* the label {source}_End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro TransferToVram source, size
		_Block_Addr_Size_Helper source, size, Block__TransferToVram
	.endmacro

	;; Transfer a block of memory to VRAM at a given location.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;;
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	;;
	;; size can be omitted if:
	;;	* the label {source}_End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro TransferToVramLocation source, vramWordAddress, size
		LDY	#vramWordAddress
		STY	VMADD

		_Block_Addr_Size_Helper source, size, Block__TransferToVram
	.endmacro


	;; Transfer a block to memory to VRAM, sending it only to VMDATAL.
	;;
	;; The VRAM Address is not set by this routine. It must be set set before use.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;; NOTE: This routine uses DMA channel 0 to reset the block.
	;; INPUT: A = bank
	;;	: X = memory address
	;;	: Y = size
	ROUTINE TransferToVramDataLow

	;; Transfer a block of memory to VRAM, sending it only to VMDATAL.
	;; The VRAM Address is not set by this routine. It must be set set before use.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;;
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	;;
	;; size can be omitted if:
	;;	* the label {source}_End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro TransferToVramDataLow source, size
		_Block_Addr_Size_Helper source, size, Block__TransferToVramDataLow
	.endmacro

	;; Transfer a block of memory to VRAM, sending it only to VMDATAL, at a given word address.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;;
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	;;
	;; size can be omitted if:
	;;	* the label {source}_End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro TransferToVramLocationDataLow source, vramWordAddress, size
		LDY	#vramWordAddress
		STY	VMADD

		_Block_Addr_Size_Helper source, size, Block__TransferToVramDataLow
	.endmacro

	;; Transfer a block to memory to VRAM, sending it only to VMDATAH.
	;;
	;; The VRAM Address is not set by this routine. It must be set set before use.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;; NOTE: This routine uses DMA channel 0 to reset the block.
	;; INPUT: A = bank
	;;	: X = memory address
	;;	: Y = size
	ROUTINE TransferToVramDataHigh

	;; Transfer a block of memory to VRAM, sending it only to VMDATAH.
	;; The VRAM Address is not set by this routine. It must be set set before use.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;;
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	;;
	;; size can be omitted if:
	;;	* the label {source}_End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro TransferToVramDataHigh source, size
		_Block_Addr_Size_Helper source, size, Block__TransferToVramDataHigh
	.endmacro

	;; Transfer a block of memory to VRAM, sending it only to VMDATAH, at a specified address.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;;
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	;;
	;; size can be omitted if:
	;;	* the label {source}_End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro TransferToVramLocationDataHigh source, vramWordAddress, size
		LDY	#vramWordAddress
		STY	VMADD

		_Block_Addr_Size_Helper source, size, Block__TransferToVramDataHigh
	.endmacro


	;; Transfer a block to memory to CGRAM.
	;; The CGRAM Address is not set by this routine. It must be set set before use.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;; NOTE: This routine uses DMA channel 0 to reset the block.
	;; INPUT: A = bank
	;;	: X = memory address
	;;	: Y = size
	ROUTINE TransferToCgram

	;; Transfer a block of memory to CGRAM.
	;; The VRAM Address is not set by this routine. It must be set set before use.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;;
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	;;
	;; size can be omitted if:
	;;	* the label {source}_End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro TransferToCgram source, size
		_Block_Addr_Size_Helper source, size, Block__TransferToCgram
	.endmacro

	;; Transfer a block of memory to CGRAM at a given location.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;;
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	;;
	;; size can be omitted if:
	;;	* the label {source}_End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro TransferToCgramLocation source, color, size
		.if color = 0
			STZ	CGADD
		.else
			LDA	#color
			STA	CGADD
		.endif

		_Block_Addr_Size_Helper source, size, Block__TransferToCgram
	.endmacro



	;; Transfer a block to memory to OAM.
	;; The CGRAM Address is not set by this routine. It must be set set before use.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;; NOTE: This routine uses DMA channel 0 to reset the block.
	;; INPUT: A = bank
	;;	: X = memory address
	;;	: Y = size
	ROUTINE TransferToOam

	;; Transfer a block of memory to OAM.
	;; The VRAM Address is not set by this routine. It must be set set before use.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;;
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	;;
	;; size can be omitted if:
	;;	* the label {source}_End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro TransferToOam source, size
		_Block_Addr_Size_Helper source, size, Block__TransferToOam
	.endmacro

	;; Transfer a block of memory to OAM at a given location.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, V-Blank or Force Blank
	;;
	;; NOTE: This routine uses DMA channel 0 to transfer the data.
	;;
	;; size can be omitted if:
	;;	* the label {source}_End exists denoting the end of the data
	;;	* the label {source}__size exists.
	.macro TransferToOamLocation source, oamAddress, size
		LDY	#oamAddress
		STY	OAMADD

		_Block_Addr_Size_Helper source, size, Block__TransferToOam
	.endmacro


	;; Calculates a checksum of the given data block.
	;;
	;; It is recommended that the checksum be used to verify the integrity
	;; of the Save RAM.
	;;
	;; This routine doesn't conform to any standards and is made to be faster
	;; than CRC16, but better than a straight addition checksum.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; Input: A data bank
	;;        X block address
	;;        Y block size (if 0, then 65536 the bytes will be read)
	;;
	;; Output: Y the 16 bit checksum
	ROUTINE Checksum

	;; Calculates the CRC-16-CCITT (little endian) of a data block stored in Y
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; size can be omitted if:
	;;	* the label {addr}_End exists denoting the end of the data
	;;	* the label {addr}__size exists.
	.macro	Block_Checksum addr, size
		_Block_Addr_Size_Helper addr, size, Block__Checksum
	.endmacro


	.macro _Block_Addr_Size_Helper addr, size, routine
		.ifblank size
			.ifdef .ident(.sprintf("%s__size", .string(addr)))
				_Block_Addr_Size_Helper addr, .ident(.sprintf("%s__size", .string(addr))), routine
			.else
				_Block_Addr_Size_Helper addr, (.ident(.sprintf("%s_End", .string(addr))) - addr), routine
			.endif
		.else
			LDA	#.bankbyte(addr)
			LDX	#.loword(addr)
			LDY	#size

			JSR	routine
		.endif
	.endmacro

ENDMODULE


.endif ; __BLOCK_H_

; vim: set ft=asm:

