/* 6502 simulator linker script.
 *
 * Produces a 64KiB memory image file usable with the provided simulator.
 */

/* Provide imaginary (zero page) registers. */
__rc0 = 0x00;
INCLUDE imag-regs.ld
ASSERT(__rc31 == 0x001f, "Inconsistent zero page map.")

MEMORY {
    zp : ORIGIN = __rc31 + 1, LENGTH = 0x100 - (__rc31 + 1)
    ram (rw) : ORIGIN = 0x0200, LENGTH = 0x1C00
    rom_bank_0 (rx) : ORIGIN = 0x8000,  LENGTH = 0x8000
    rom_bank_1 (rx) : ORIGIN = 0x10000, LENGTH = 0x8000
    rom_bank_2 (rx) : ORIGIN = 0x18000, LENGTH = 0x8000
}

REGION_ALIAS("c_readonly", rom_bank_0)
REGION_ALIAS("c_readonly_bank1", rom_bank_1)

REGION_ALIAS("c_writeable", ram)

/* Set initial soft stack address to just above zero page */
__stack = 0x0100;

SECTIONS {
  INCLUDE c.ld

  .rom_bank_0_fixed : { *(.rom_bank_0_fixed .rom_bank_0_fixed.*) } >rom_bank_0
  .rom_bank_1_fixed : { *(.rom_bank_1_fixed .rom_bank_1_fixed.*) } >rom_bank_1
  .rom_bank_2_fixed : { *(.rom_bank_2_fixed .rom_bank_2_fixed.*) } >rom_bank_2
  
  
  .vector 0xfffc : {
    SHORT(_start)      /* Entrypoint */
]  } >rom_bank_0

  .text_bank1 : {
    *(.text.01 .text.01.*)
    *(.rodata.01 .rodata.01.*)
  } >rom_bank_1

  .text_bank2 : {
    *(.text.02 .text.02.*)
    *(.rodata.02 .rodata.02.*)
  } >rom_bank_2
   
}

OUTPUT_FORMAT {
 FULL(rom_bank_0)
 FULL(rom_bank_1)
 FULL(rom_bank_2)

}

