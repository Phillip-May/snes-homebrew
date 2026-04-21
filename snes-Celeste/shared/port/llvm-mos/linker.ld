/* SNES LoROM linker script for LLVM-MOS
 *
 * Multi-bank ROM for SNES LoROM mapping.
 * Target CPU: 65816 (use -mcpu=mosw65816 compiler flag)
 */

/* Provide imaginary (zero page) registers. */
__rc0 = 0x00;
INCLUDE imag-regs.ld
ASSERT(__rc31 == 0x001f, "Inconsistent zero page map.")

MEMORY {
    zp : ORIGIN = __rc31 + 1, LENGTH = 0x100 - (__rc31 + 1)
    ram (rw) : ORIGIN = 0x0200, LENGTH = 0x1E00
    /* Executable PRG banks are split like an NES-style PRG window:
     * $8000-$AFFF is bank-local; $B000-$FFDF is fixed code mirrored into
     * every executable bank by OUTPUT_FORMAT below. */
    rom_bank_0 (rx)  : ORIGIN = 0x008000,  LENGTH = 0x3000
    rom_fixed_mirror (rx) : ORIGIN = 0x00B000, LENGTH = 0x4FC0
    rom_bank_1 (rx)  : ORIGIN = 0x018000,  LENGTH = 0x3000
    rom_bank_2 (rx)  : ORIGIN = 0x028000,  LENGTH = 0x3000
    rom_bank_3 (rx)  : ORIGIN = 0x038000,  LENGTH = 0x3000
    rom_bank_4 (rx)  : ORIGIN = 0x048000,  LENGTH = 0x3000
    rom_bank_5 (rx)  : ORIGIN = 0x058000,  LENGTH = 0x3000
    rom_bank_6 (rx)  : ORIGIN = 0x068000,  LENGTH = 0x3000
    rom_bank_7 (rx)  : ORIGIN = 0x078000,  LENGTH = 0x3000

    /* Large SNES-only data banks. These are not executable PRG banks; they
     * are addressed by DMA/DBR through explicit SNES data-bank constants. */
    rom_data_bank_1 (rx) : ORIGIN = 0x088000, LENGTH = 0x7FE0
    rom_data_bank_2 (rx) : ORIGIN = 0x098000, LENGTH = 0x7FE0
    rom_data_bank_3 (rx) : ORIGIN = 0x0A8000, LENGTH = 0x7FE0
    rom_data_bank_4 (rx) : ORIGIN = 0x0B8000, LENGTH = 0x7FE0
    rom_data_bank_5 (rx) : ORIGIN = 0x0C8000, LENGTH = 0x7FE0
    rom_data_bank_6 (rx) : ORIGIN = 0x0D8000, LENGTH = 0x7FE0
    rom_pad_bank_7 (rx)  : ORIGIN = 0x0E8000, LENGTH = 0x7FE0
    rom_pad_bank_8 (rx)  : ORIGIN = 0x0F8000, LENGTH = 0x7FE0
    rom_snes_header (rx) : ORIGIN = 0x00FFC0, LENGTH = 0x0020
    rom_header_pad (rx)  : ORIGIN = 0x01FFC0, LENGTH = 0x0020
    rom_bank_0_fixed : ORIGIN = 0x00FFE0,  LENGTH = 0x0020
}

REGION_ALIAS("c_writeable", ram)
/* Default C/runtime code is fixed-window code mirrored into executable banks. */
REGION_ALIAS("c_readonly", rom_fixed_mirror)

/* Set initial soft stack address to just above zero page */
__stack = 0x0200;

/* Set entry point to startup code */
ENTRY(_start)

SECTIONS {
  INCLUDE c.ld

  /* Custom sections for ROM bank placement - these come AFTER c.ld sections */  

  /* Generic fixed mirror section. This is emitted into $B000-$FFDF in every
   * executable bank by OUTPUT_FORMAT, so near JSRs to fixed helpers work while
   * code is running in any executable PRG bank. */
  rom_fixed : { *(rom_fixed rom_fixed.*) } >rom_fixed_mirror
  rom_fixed_rodata : { *(rom_fixed_rodata rom_fixed_rodata.*) } >rom_fixed_mirror

  /* Code section for rom_bank_0_fixed - placed before vectors */
  .text.rom_bank_0_fixed : { *(.text.rom_bank_0_fixed .text.rom_bank_0_fixed.*) } >rom_bank_0_fixed

  /* Read-only data section for rom_bank_0_fixed */
  .rodata.rom_bank_0_fixed : { *(.rodata.rom_bank_0_fixed .rodata.rom_bank_0_fixed.*) } >rom_bank_0_fixed

  /* Generic section for rom_bank_0_fixed (allows __attribute__((section("rom_bank_0_fixed")))) */
  rom_bank_0_fixed : { *(rom_bank_0_fixed rom_bank_0_fixed.*) } >rom_bank_0_fixed


  /* SNES LoROM header at $00:FFC0. Emit it only for bank 0; repeated headers
   * at $01:FFC0 make emulators prefer the wrong HiROM header candidate. */
  .snes_header 0xFFC0 : {
    BYTE(0x53) BYTE(0x4E) BYTE(0x45) BYTE(0x53) BYTE(0x20) BYTE(0x43) BYTE(0x45)
    BYTE(0x4C) BYTE(0x45) BYTE(0x53) BYTE(0x54) BYTE(0x45) BYTE(0x20) BYTE(0x20)
    BYTE(0x20) BYTE(0x20) BYTE(0x20) BYTE(0x20) BYTE(0x20) BYTE(0x20) BYTE(0x20)
    BYTE(0x20)        /* LoROM, slow ROM */
    BYTE(0x00)        /* ROM only */
    BYTE(0x09)        /* 512 KiB */
    BYTE(0x00)        /* no SRAM */
    BYTE(0x01)        /* NTSC */
    BYTE(0x33)        /* extended licensee */
    BYTE(0x00)        /* version */
    SHORT(0x0000)     /* checksum complement, filled by post tools later */
    SHORT(0x0000)     /* checksum, filled by post tools later */
  } >rom_snes_header

  .snes_header_pad : { *(rom_header_pad rom_header_pad.*) } >rom_header_pad

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
  } >rom_bank_0_fixed

  /* SNES Interrupt Vectors - Emulation Mode */
  .vectors_emulation 0xFFF4 : {
    SHORT(COP)        /* COP vector */
    SHORT(DIRQ)       /* Reserved vector */
    SHORT(ABORT)      /* ABORT vector */
    SHORT(NMI)        /* NMI vector */
    SHORT(_start)     /* RESET vector - points to program entry point */
    SHORT(IRQ)        /* IRQ vector */
  } >rom_bank_0_fixed

  /* Custom sections for ROM bank placement */
  /* Code sections for specific banks */
  .text.rom_bank_0 : { *(.text.rom_bank_0 .text.rom_bank_0.*) } >rom_bank_0
  .text.rom_bank_1 : { *(.text.rom_bank_1 .text.rom_bank_1.*) } >rom_bank_1
  .text.rom_bank_2 : { *(.text.rom_bank_2 .text.rom_bank_2.*) } >rom_bank_2
  .text.rom_bank_3 : { *(.text.rom_bank_3 .text.rom_bank_3.*) } >rom_bank_3
  .text.rom_bank_4 : { *(.text.rom_bank_4 .text.rom_bank_4.*) } >rom_bank_4
  .text.rom_bank_5 : { *(.text.rom_bank_5 .text.rom_bank_5.*) } >rom_bank_5
  .text.rom_bank_6 : { *(.text.rom_bank_6 .text.rom_bank_6.*) } >rom_bank_6
  .text.rom_bank_7 : { *(.text.rom_bank_7 .text.rom_bank_7.*) } >rom_bank_7

  /* Constant/read-only data sections for specific banks */
  .rodata.rom_bank_0 : { *(.rodata.rom_bank_0 .rodata.rom_bank_0.*) } >rom_bank_0
  .rodata.rom_bank_1 : { *(.rodata.rom_bank_1 .rodata.rom_bank_1.*) } >rom_bank_1
  .rodata.rom_bank_2 : { *(.rodata.rom_bank_2 .rodata.rom_bank_2.*) } >rom_bank_2
  .rodata.rom_bank_3 : { *(.rodata.rom_bank_3 .rodata.rom_bank_3.*) } >rom_bank_3
  .rodata.rom_bank_4 : { *(.rodata.rom_bank_4 .rodata.rom_bank_4.*) } >rom_bank_4
  .rodata.rom_bank_5 : { *(.rodata.rom_bank_5 .rodata.rom_bank_5.*) } >rom_bank_5
  .rodata.rom_bank_6 : { *(.rodata.rom_bank_6 .rodata.rom_bank_6.*) } >rom_bank_6
  .rodata.rom_bank_7 : { *(.rodata.rom_bank_7 .rodata.rom_bank_7.*) } >rom_bank_7

  /* Generic sections that can be used directly with rom_bank_X names */
  /* These allow using __attribute__((section("rom_bank_0"))) directly */
  rom_bank_0 : { *(rom_bank_0 rom_bank_0.*) } >rom_bank_0
  rom_bank_0_rodata : { *(rom_bank_0_rodata rom_bank_0_rodata.*) } >rom_bank_0
  rom_bank_1 : { *(rom_bank_1 rom_bank_1.*) } >rom_bank_1
  rom_bank_1_rodata : { *(rom_bank_1_rodata rom_bank_1_rodata.*) } >rom_bank_1
  rom_bank_2 : { *(rom_bank_2 rom_bank_2.*) } >rom_bank_2
  rom_bank_2_rodata : { *(rom_bank_2_rodata rom_bank_2_rodata.*) } >rom_bank_2
  rom_bank_3 : { *(rom_bank_3 rom_bank_3.*) } >rom_bank_3
  rom_bank_3_rodata : { *(rom_bank_3_rodata rom_bank_3_rodata.*) } >rom_bank_3
  rom_bank_4 : { *(rom_bank_4 rom_bank_4.*) } >rom_bank_4
  rom_bank_4_rodata : { *(rom_bank_4_rodata rom_bank_4_rodata.*) } >rom_bank_4
  rom_bank_5 : { *(rom_bank_5 rom_bank_5.*) } >rom_bank_5
  rom_bank_5_rodata : { *(rom_bank_5_rodata rom_bank_5_rodata.*) } >rom_bank_5
  rom_bank_6 : { *(rom_bank_6 rom_bank_6.*) } >rom_bank_6
  rom_bank_6_rodata : { *(rom_bank_6_rodata rom_bank_6_rodata.*) } >rom_bank_6
  rom_bank_7 : { *(rom_bank_7 rom_bank_7.*) } >rom_bank_7
  rom_bank_7_rodata : { *(rom_bank_7_rodata rom_bank_7_rodata.*) } >rom_bank_7

  /* Large SNES-only data banks used by DMA/DBR, kept separate from executable
   * PRG banks so PORT_FUNC_BANKX can mirror NES-style banked code safely. */
  rom_data_bank_1 : { *(rom_data_bank_1 rom_data_bank_1.*) } >rom_data_bank_1
  rom_data_bank_1_rodata : { *(rom_data_bank_1_rodata rom_data_bank_1_rodata.*) } >rom_data_bank_1
  rom_data_bank_2 : { *(rom_data_bank_2 rom_data_bank_2.*) } >rom_data_bank_2
  rom_data_bank_2_rodata : { *(rom_data_bank_2_rodata rom_data_bank_2_rodata.*) } >rom_data_bank_2
  rom_data_bank_3 : { *(rom_data_bank_3 rom_data_bank_3.*) } >rom_data_bank_3
  rom_data_bank_3_rodata : { *(rom_data_bank_3_rodata rom_data_bank_3_rodata.*) } >rom_data_bank_3
  rom_data_bank_4 : { *(rom_data_bank_4 rom_data_bank_4.*) } >rom_data_bank_4
  rom_data_bank_4_rodata : { *(rom_data_bank_4_rodata rom_data_bank_4_rodata.*) } >rom_data_bank_4
  rom_data_bank_5 : { *(rom_data_bank_5 rom_data_bank_5.*) } >rom_data_bank_5
  rom_data_bank_5_rodata : { *(rom_data_bank_5_rodata rom_data_bank_5_rodata.*) } >rom_data_bank_5
  rom_data_bank_6 : { *(rom_data_bank_6 rom_data_bank_6.*) } >rom_data_bank_6
  rom_data_bank_6_rodata : { *(rom_data_bank_6_rodata rom_data_bank_6_rodata.*) } >rom_data_bank_6
  rom_pad_bank_7 : { *(rom_pad_bank_7 rom_pad_bank_7.*) } >rom_pad_bank_7
  rom_pad_bank_8 : { *(rom_pad_bank_8 rom_pad_bank_8.*) } >rom_pad_bank_8
}

OUTPUT_FORMAT {
 /* Executable PRG banks 0-7: local window + mirrored fixed window + header/pad + vectors. */
 FULL(rom_bank_0)
 FULL(rom_fixed_mirror)
 FULL(rom_snes_header)
 FULL(rom_bank_0_fixed)
 FULL(rom_bank_1)
 FULL(rom_fixed_mirror)
 FULL(rom_header_pad)
 FULL(rom_bank_0_fixed)
 FULL(rom_bank_2)
 FULL(rom_fixed_mirror)
 FULL(rom_header_pad)
 FULL(rom_bank_0_fixed)
 FULL(rom_bank_3)
 FULL(rom_fixed_mirror)
 FULL(rom_header_pad)
 FULL(rom_bank_0_fixed)
 FULL(rom_bank_4)
 FULL(rom_fixed_mirror)
 FULL(rom_header_pad)
 FULL(rom_bank_0_fixed)
 FULL(rom_bank_5)
 FULL(rom_fixed_mirror)
 FULL(rom_header_pad)
 FULL(rom_bank_0_fixed)
 FULL(rom_bank_6)
 FULL(rom_fixed_mirror)
 FULL(rom_header_pad)
 FULL(rom_bank_0_fixed)
 FULL(rom_bank_7)
 FULL(rom_fixed_mirror)
 FULL(rom_header_pad)
 FULL(rom_bank_0_fixed)

 /* High data banks used by SNES DMA/DBR copies. */
 FULL(rom_data_bank_1)
 FULL(rom_bank_0_fixed)
 FULL(rom_data_bank_2)
 FULL(rom_bank_0_fixed)
 FULL(rom_data_bank_3)
 FULL(rom_bank_0_fixed)
 FULL(rom_data_bank_4)
 FULL(rom_bank_0_fixed)
 FULL(rom_data_bank_5)
 FULL(rom_bank_0_fixed)
 FULL(rom_data_bank_6)
 FULL(rom_bank_0_fixed)
 FULL(rom_pad_bank_7)
 FULL(rom_bank_0_fixed)
 FULL(rom_pad_bank_8)
 FULL(rom_bank_0_fixed)
}
