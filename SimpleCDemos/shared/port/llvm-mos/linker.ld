/* SNES LoROM linker script for LLVM-MOS
 *
 * Produces a 32KiB ROM image for SNES LoROM mapping.
 * Based on vectors.asm reference from wdc816cc.
 */

/* Provide imaginary (zero page) registers. */
__rc0 = 0x00;
INCLUDE imag-regs.ld
ASSERT(__rc31 == 0x001f, "Inconsistent zero page map.")

MEMORY {
    zp : ORIGIN = __rc31 + 1, LENGTH = 0x100 - (__rc31 + 1)
    ram (rw) : ORIGIN = 0x0200, LENGTH = 0x1C00
    rom_bank_0 (rx) : ORIGIN = 0x8000,  LENGTH = 0x8000
}

REGION_ALIAS("c_readonly", rom_bank_0)
REGION_ALIAS("c_writeable", ram)

/* Set initial soft stack address to just above zero page */
__stack = 0x0200;

/* Set entry point to our startup code */
ENTRY(_start)

SECTIONS {
  INCLUDE c.ld

  /* Ensure our startup code is first */
  .text.startup : { *(.text.startup) } >rom_bank_0

  .rom_bank_0_fixed : { *(.rom_bank_0_fixed .rom_bank_0_fixed.*) } >rom_bank_0
  
  /* Ensure data initialization is properly handled */
  .data : {
    *(.data .data.*)
    . = ALIGN(2);
  } >ram AT >rom_bank_0
  
  .bss : {
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(2);
  } >ram  
  
  /* SNES Interrupt Vectors - Native Mode */
  .vectors_native 0xFFE4 : {
    SHORT(COP)        /* COP vector */
    SHORT(BRK)        /* BRK vector */
    SHORT(ABORT)      /* ABORT vector */
    SHORT(NMI)        /* NMI vector */
    SHORT(DIRQ)       /* Reserved vector */
    SHORT(IRQ)        /* IRQ vector */
    /* 4 bytes reserved */
    LONG(0x00000000)
  } >rom_bank_0
  
  /* SNES Interrupt Vectors - Emulation Mode */
  .vectors_emulation 0xFFF4 : {
    SHORT(COP)        /* COP vector */
    SHORT(DIRQ)       /* Reserved vector */
    SHORT(ABORT)      /* ABORT vector */
    SHORT(NMI)        /* NMI vector */
    SHORT(_start)     /* RESET vector - points to program entry point */
    SHORT(IRQ)        /* IRQ vector */
  } >rom_bank_0
}

OUTPUT_FORMAT {
 FULL(rom_bank_0)
}

