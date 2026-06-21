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
    bank_7e_ram (rw) : ORIGIN = 0x7E2000, LENGTH = 0xE000
    
    /* SNES Header - 64 bytes at 0x7FC0 (LoROM header location) */
    snes_header (r) : ORIGIN = 0x7FC0, LENGTH = 0x40
    
    /* Fixed code section - 8KB, will be duplicated to all banks */
    rom_bank_0_fixed (rx) : ORIGIN = 0xE000, LENGTH = 0x2000  /* 8KB fixed code */
        
    /* Bank-specific code regions - ~24KB each, placed after the 8KB fixed section */
    /* Leave space for vectors at 0xFFE4-0xFFFF (28 bytes) */
    code_bank_0_only (rx) : ORIGIN = 0xA000, LENGTH = 0x6000  /* ~24KB for bank 0 specific code (ends before vectors) */
    code_bank_1_only (rx) : ORIGIN = 0x01A000, LENGTH = 0x6000  /* ~24KB for bank 1 specific code */
    code_bank_2_only (rx) : ORIGIN = 0x02A000, LENGTH = 0x6000  /* ~24KB for bank 2 specific code */
    code_bank_3_only (rx) : ORIGIN = 0x03A000, LENGTH = 0x6000  /* ~24KB for bank 3 specific code */
    
    /* Data Banks - Separate from code banks */
    data_bank_1 (r) : ORIGIN = 0x048000, LENGTH = 0x8000  /* Data bank 1 */
    data_bank_2 (r) : ORIGIN = 0x058000, LENGTH = 0x8000  /* Data bank 2 */
    data_bank_3 (r) : ORIGIN = 0x068000, LENGTH = 0x8000  /* Data bank 3 */
}

REGION_ALIAS("c_readonly", rom_bank_0_fixed)
REGION_ALIAS("c_writeable", ram)

/* Set initial soft stack address to just above zero page */
__stack = 0x0200;

/* Set entry point to our startup code */
ENTRY(_reset)

SECTIONS {
  /* Reset entry (startup.s) must be in the fixed bank so its link address
   * equals its physical ROM address for the 16-bit reset vector. */
  .text.startup : { KEEP(*(.text.startup)) } >rom_bank_0_fixed
  .text.vectors : { *(.text.vectors) } >rom_bank_0_fixed
  
  /* Define bank-specific rodata sections FIRST, before c.ld */
  /* In GNU ld, more specific section names take precedence, so .rodata.bankX will match */
  /* before c.ld's general .rodata pattern. This ensures bank-specific data goes to data banks */
  .rodata.bank0 : { *(.rodata.bank0 .rodata.bank0.*) } >rom_bank_0_fixed
  .rodata.bank1 : { *(.rodata.bank1 .rodata.bank1.*) } >data_bank_1
  .rodata.bank2 : { *(.rodata.bank2 .rodata.bank2.*) } >data_bank_2
  .rodata.bank3 : { *(.rodata.bank3 .rodata.bank3.*) } >data_bank_3
  
  /* Include c.ld which will define default .text and .rodata sections */
  /* c.ld's .rodata might use *(.rodata .rodata.*) which could match bank-specific sections */
  /* We override it below to be more restrictive */
  INCLUDE c.ld
  
  /* Override .rodata to only match exact .rodata sections (not .rodata.*) */
  /* This ensures bank-specific .rodata.bankX sections go to their designated data banks */
  /* Bank-specific sections are defined above and will have already matched their input sections */
  .rodata : {
    *(.rodata)
    /* Explicitly exclude .rodata.* patterns - they are handled by bank-specific sections above */
  } >rom_bank_0_fixed
  
  /* Explicit code bank sections (only used with section attributes) */
  .text.bank0 : { *(.text.bank0 .text.bank0.*) } >code_bank_0_only
  .text.bank1 : { *(.text.bank1 .text.bank1.*) } >code_bank_1_only
  .text.bank2 : { *(.text.bank2 .text.bank2.*) } >code_bank_2_only
  .text.bank3 : { *(.text.bank3 .text.bank3.*) } >code_bank_3_only
  
  /* Ensure data initialization is properly handled */
  .data : {
    *(.data .data.*)
    . = ALIGN(2);
  } >ram AT >rom_bank_0_fixed
  
  .bss : {
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(2);
  } >ram
  
  /* Bank 7E RAM section for data that should be in bank 7E */
  .bank_7e_data : {
    *(.bank_7e_data .bank_7e_data.*)
    . = ALIGN(2);
  } >bank_7e_ram
  
  .bank_7e_bss : {
    *(.bank_7e_bss .bank_7e_bss.*)
    . = ALIGN(2);
  } >bank_7e_ram
  
  /* SNES ROM Header - 64 bytes at 0x7FC0 (LoROM header location) */
  /* This ensures the ROM is detected as LoROM by emulators */
  .snes_header 0xFFC0 : {
    /* Title: 21 bytes (padded with spaces) */
    BYTE(0x4C) BYTE(0x4C) BYTE(0x56) BYTE(0x4D) BYTE(0x2D)  /* "LLVM-" */
    BYTE(0x4D) BYTE(0x4F) BYTE(0x53) BYTE(0x20) BYTE(0x53)  /* "MOS S" */
    BYTE(0x4E) BYTE(0x45) BYTE(0x53) BYTE(0x58) BYTE(0x43)  /* "NESXC" */
    BYTE(0x20) BYTE(0x20) BYTE(0x20) BYTE(0x20) BYTE(0x20)  /* "     " */
    BYTE(0x20)  /* " " */
    
    /* Map mode: 0x20 = LoROM + Fast ROM (bit 7=0: Fast ROM, bit 6=0: LoROM) */
    BYTE(0x20)
    
    /* Cartridge type: 0x00 = ROM only */
    BYTE(0x00)
    
    /* ROM size: 0x07 = 128KB (4 banks of 32KB), adjust as needed */
    /* 0x07 = 128KB, 0x08 = 256KB, 0x09 = 512KB, etc. */
    BYTE(0x07)
    
    /* SRAM size: 0x00 = No SRAM */
    BYTE(0x00)
    
    /* Country: 0x01 = USA/Canada */
    BYTE(0x01)
    
    /* Licensee: 0x33 = Extended licensee code */
    BYTE(0x33)
    
    /* Version: 0x00 = Version 1.00 */
    BYTE(0x00)
    
    /* Complement check: 0x0000 (can be calculated by checksum tool) */
    SHORT(0x0000)
    
    /* Checksum: 0x0000 (can be calculated by checksum tool) */
    SHORT(0x0000)
  } >rom_bank_0_fixed
  
  /* SNES Interrupt Vectors - Native Mode */
  /* Vectors are at fixed addresses at the end of bank 0 (0xFFE4-0xFFFF) */
  /* These are only in bank 0, not duplicated */
  .vectors_native 0xFFE4 : {
    SHORT(COP)        /* COP vector */
    SHORT(BRK)        /* BRK vector */
    SHORT(ABORT)      /* ABORT vector */
    SHORT(NMI)        /* NMI vector */
    SHORT(DIRQ)       /* Reserved vector */
    SHORT(IRQ)        /* IRQ vector */
    /* 4 bytes reserved */
    LONG(0x00000000)
  } >rom_bank_0_fixed
  
  /* SNES Interrupt Vectors - Emulation Mode */
  .vectors_emulation 0xFFF4 : {
    SHORT(COP)        /* COP vector */
    SHORT(DIRQ)       /* Reserved vector */
    SHORT(ABORT)      /* ABORT vector */
    SHORT(NMI)        /* NMI vector */
    SHORT(_reset)     /* RESET vector - clears RAM (startup.s) then calls main */
    SHORT(IRQ)        /* IRQ vector */
  } >rom_bank_0_fixed
}

OUTPUT_FORMAT { 
 /* Bank 0: 8KB fixed (duplicated) + 24KB bank-specific = 32KB */
 /* Header will be placed at 0x7FC0 by section address */
 FULL(code_bank_0_only)
 FULL(rom_bank_0_fixed)
 
 /* Bank 1: 8KB fixed (duplicated) + 24KB bank-specific = 32KB */
 FULL(code_bank_1_only)
 FULL(rom_bank_0_fixed)
 
 /* Bank 2: 8KB fixed (duplicated) + 24KB bank-specific = 32KB */
 FULL(code_bank_2_only)
 FULL(rom_bank_0_fixed)
 
 /* Bank 3: 8KB fixed (duplicated) + 24KB bank-specific = 32KB */
 FULL(code_bank_3_only)
 FULL(rom_bank_0_fixed)
 
 /* Data banks */
 FULL(data_bank_1)
 FULL(data_bank_2)
 FULL(data_bank_3)
}

