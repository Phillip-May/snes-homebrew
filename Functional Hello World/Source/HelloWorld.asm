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
include "../LIB/SNES_INPUT.INC"
include "TerminalMacros.ASM"

seek($008000); Start:
  //Initialisation
  SNES_INIT(SLOWROM)   // Run SNES Initialisation Routine
  TERMINIT()
  // Print Text
  mCALLPRINTTERM24BITS(BANK3TEXT)

db 0x42, 0x00
lda.b #$01
sta.w REG_NMITIMEN // Enable Joypad NMI Reading Interrupt
ldx.w #$0000 // Reset BG X Position
ldy.w #$0000 // Reset BG Y Position

Loop:
  WaitNMI() // Wait For Vertical Blank
InputLoop: 
  WaitNMI() // Wait For Vertical Blank

  Up:
    ReadJOY({JOY_UP}) // Test Joypad UP Button
    beq Down          // IF (UP ! Pressed) Branch Down
    BGScroll(REG_BG1VOFS, de, y) // Decrement BG1 Vertical Scroll Position

  Down:
    ReadJOY({JOY_DOWN}) // Test DOWN Button
    beq Left            // IF (DOWN ! Pressed) Branch Down
    BGScroll(REG_BG1VOFS, in, y) // Increment BG1 Vertical Scroll Position

  Left:
    ReadJOY({JOY_LEFT}) // Test LEFT Button
    beq Right           // IF (LEFT ! Pressed) Branch Down
    BGScroll(REG_BG1HOFS, de, x) // Decrement BG1 Horizontal Scroll Position

  Right:
    ReadJOY({JOY_RIGHT}) // Test RIGHT Button
    beq Finish           // IF (RIGHT ! Pressed) Branch Down
    BGScroll(REG_BG1HOFS, in, x) // Increment BG1 Horizontal Scroll Position

  Finish:
    jmp InputLoop
  jmp Loop

HELLOWORLD:
  db "Hello, World!{}" // Hello World Text
  db 0x0D
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
db "Bank 3 part 1"
db 0x0D
db "Bank 3 part 2"
db 0x0D
db "Bank 3 part 3"
db 0x0D
db "Bank 3 part 4"
db 0x0D
db "Bank 3 part 5"
db 0x0D
db "Bank 3 part 6"
db 0x0D
db "Bank 3 part 7"
db 0x0D
db "Bank 3 part 9"
db 0x0D
db "Bank 3 part 10"
db 0x0D
db "Bank 3 part 11"
db 0x0D
db "Bank 3 part 12"
db 0x0D
db "Bank 3 part 13"
db 0x0D
db "Bank 3 part 14"
db 0x0D
db "Bank 3 part 15"
db 0x0D
db "Bank 3 part 16"
db 0x0D
db "Bank 3 part 17"
db 0x0D
db "Bank 3 part 18"
db 0x0D
db "Bank 3 part 19"
db 0x0D
db "Bank 3 part 20"
db 0x0D
db "Bank 3 part 21"
db 0x0D
db "Bank 3 part 22"
db 0x0D
db "Bank 3 part 23"
db 0x0D
db "Bank 3 part 24"
db 0x0D
db "Bank 3 part 25"
db 0x0D
db "Bank 3 part 26"
db 0x0D
db "Bank 3 part 27"
db 0x0D
db "Bank 3 part 28"
db 0x0D
db "Bank 3 part 29"
db 0x0D
db "Bank 3 part 30"
db 0x0D
db "Bank 3 part 31"
db 0x0D
db "Bank 3 part 32"
db 0x0D
db "Bank 3 part 33"
db 0x00

BANK3TEXTPART2:
db "Break the worlds shell, for the revolution of the world!!"
db 0x0D
db 0x00 //Null byte