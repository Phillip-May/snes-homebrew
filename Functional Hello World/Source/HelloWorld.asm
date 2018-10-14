// SNES "Hello, World!" Text Printing demo by krom (Peter Lemon):
// 1. DMA Loads Palette Data To CGRAM
// 2. DMA Loads 1BPP Character Tile Data To VRAM (Converts to 2BPP Tiles)
// 3. DMA Clears VRAM Map To A Space " " Character
// 4. DMA Prints Text Characters To Lo Bytes Of Map
arch snes.cpu
output "../HelloWorld.sfc", create

macro seek(variable offset) {
  origin ((offset & $7F0000) >> 1) | (offset & $7FFF)
  base offset
}

macro updateDataBank(bank) {
  sep #0x20 // Set 8-Bit Accumulator
  lda #{bank}
  pha
  plb  
}

//Removed terminit

//Includes
//If a bank is not entirefilled with zero's, snes9x ignores it.
seek($008000); fill $8000 // Fill Upto $7FFF (Bank 0) With Zero Bytes
seek($018000); fill $8000 // Fill Upto $7FFF (Bank 1) With Zero Bytes
seek($028000); fill $8000 // Fill Upto $7FFF (Bank 2) With Zero Bytes
include "../LIB/SNES.INC"        // Include SNES Definitions
include "../LIB/SNES_HEADER.ASM" // Include Header & Vector Table
include "../LIB/SNES_GFX.INC"    // Include Graphics Macros
include "TerminalMacros.ASM"

seek($008000); Start:
  //Initialisation
  SNES_INIT(SLOWROM)   // Run SNES Initialisation Routine
  TERMINIT()
  // Print Text  
  mCALLPRINTTERM24BITS(BANK3TEXT)
  //mCALLPRINTTERM16BITS(TEMP)
  //mCALLPRINTTERM24BITS(BANK3TEXTPART2)

Loop:
  jmp Loop

HELLOWORLD:
  db "Hello, World!{}" // Hello World Text
  db 0x00            //Null Byte

BGCHR:
  include "Font8x8.asm" // Include BG 1BPP 8x8 Tile Font Character Data (1016 Bytes)
BGPAL:
  dw $0000, $7FFF // Black / White Palette (4 Bytes)
BGCLEAR:
  dw $0020 // BG Clear Character Space " " Fixed value upper byte is tile properties

seek($018000)
include "TerninalFunctions.ASM"

seek($028000)
BANK3TEXT:
db "Text in bank 3.[]012346578901234"
//Line feed, I believe in linux line endings
db 0x0D
db "Bank 3 part 2"
db 0x0D
db "Bank 3 part 3"
db 0x00

BANK3TEXTPART2:
db "Break the worlds shell, for the revolution of the world!!"
db 0x00 //Null byte