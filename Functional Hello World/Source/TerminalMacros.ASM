arch snes.cpu
//Tile map start location
constant BGTILEMAPLINE1($7840) //First line is partially cutoff

//The temrinal module runs on Mode 1
//8x8 tiles
//32x32 tile background
//Lowest priority

macro TERMINIT() {
  LoadPAL(BGPAL, $00, 4, 0) // Load BG Palette Data
  LoadLOVRAM(BGCHR, $0000, $3F8, 0) // Load 1BPP Tiles To VRAM Lo Bytes (Converts To 2BPP Tiles)  
  ClearVRAM(BGCLEAR, $F800, $400, 0) // Clear VRAM Map To Fixed Tile Word
  // Setup Video
  lda.b #%00001000 // DCBAPMMM: M = Mode, P = Priority, ABCD = BG1,2,3,4 Tile Size
  sta.w REG_BGMODE // $2105: BG Mode 0, Priority 1, BG1 8x8 Tiles

  // Setup BG1 4 Color Background
  lda.b #%01111100  // AAAAAASS: S = BG Map Size, A = BG Map Address
  db 0x42, 0x00
  ldx.w #wCursorPos >> 8 << 8 //Direct page bank to variables bank
  phx
  pld          //Direct page will use bank variable page

  lda.b #(BGTILEMAPLINE1 >> (8+1)) + 0b00 //Upper byte shifted by 1
  sta REG_BG1SC     // $2108: BG1 32x32, BG1 Map Address = $3F (VRAM Address / $400)
  ldx.w #(BGTILEMAPLINE1 >> 1)
  stx.b wCursorPos  //Also save starting cursor position, using direct page

  lda.b #%00000000  // BBBBAAAA: A = BG1 Tile Address, B = BG2 Tile Address
  sta.w REG_BG12NBA // $210B: BG1 Tile Address = $0 (VRAM Address / $1000)

  lda.b #%00000001 // Enable BG1
  sta.w REG_TM // $212C: BG1 To Main Screen Designation

  stz.w REG_BG1HOFS // Store Zero To BG1 Horizontal Scroll Pos Low Byte
  stz.w REG_BG1HOFS // Store Zero To BG1 Horizontal Scroll Pos High Byte
  stz.w REG_BG1VOFS // Store Zero To BG1 Vertical Scroll Pos Low Byte
  stz.w REG_BG1VOFS // Store Zero To BG1 Vertical Pos High Byte

  //Turn the screen on
  //Wait for Vblank (NMI)
  clc
  -
    bit.w REG_RDNMI // $4210: Read NMI Flag Register
    bpl - // Wait For NMI Flag
  //Set screen to max brightness
  lda.b #0x0F // 8-Bit A
  sta.w REG_INIDISP // $2100: Turn Screen To Brightness Level
}

macro mCALLPRINTTERM24BITS(sz24BIT) {
  //Context save 
  php
  sep #0x20 // Set 8-Bit Accumulator
  rep #0x10 // 16-bit x,y
  ldx.w #{sz24BIT} //Give address via x reigster
  lda.b #{sz24BIT} >> 16 //Give bank via accumulator
  jsl fPRINTTERM24BITS
  plp
}