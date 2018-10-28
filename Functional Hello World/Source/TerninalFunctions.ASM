//Constants
scope TermPrivate {
  //Tile map start location
  constant BGTILEMAPLINE1($3C00) //First line is partially cutoff (7800 >> 1)
  constant BGSCROLLRESETLOW(0xFF)//This fixes it 07 would be one line down

  //Const data for dma ram clear
  BGCHR:
    include "Font8x8.asm" // Include BG 1BPP 8x8 Tile Font Character Data (1016 Bytes)
  BGPAL:
    dw $0000, $7FFF // Black / White Palette (4 Bytes)  
  BGCLEAR2:
    dw $0020 // BG Clear Character Space " " Fixed value upper byte is tile properties

  //Variables
  //Cursor current position (VRAM location)
  //Shifted//Upper byte shifted by 1 over 1
  constant wCursorPos($000105)
  constant bScroll($000107) //Boolean variable for if background needs to scroll or not.
  constant bScrollPosition($000109)  //Byte of scroll position

}


//No Requirements
//No Parameters
//Initialises the terminal
fTERMINIT24BITS:
  //Link
  pha
  phx
  phy
  phb
  php

  sep #0x20              // Set 8-Bit Accumulator
  rep #0x10              // 16-bit x,y
  WaitNMI()
  jsl fCLEARTERM24BITS      //Clears Vram tile map and waits for NMI
  //WaitHVB()
  LoadPAL(TermPrivate.BGPAL, $00, 4, 0) // Load BG Palette Data
  LoadLOVRAM(TermPrivate.BGCHR, $0000, $3F8, 0) // Load 1BPP Tiles To VRAM Lo Bytes (Converts To 2BPP Tiles)  
  // Setup Video
  lda.b #%00001000 // DCBAPMMM: M = Mode, P = Priority, ABCD = BG1,2,3,4 Tile Size
  sta.w REG_BGMODE // $2105: BG Mode 0, Priority 1, BG1 8x8 Tiles

  // Setup BG1 4 Color Background
  lda.b #%01111100  // AAAAAASS: S = BG Map Size, A = BG Map Address
  ldx.w #TermPrivate.wCursorPos >> 8 << 8 //Direct page bank to variables bank
  phx
  pld          //Direct page will use bank variable page

  lda.b #(TermPrivate.BGTILEMAPLINE1 >> (8)) + 0b00 //Upper byte shifted by 1
  sta REG_BG1SC     // $2108: BG1 32x32, BG1 Map Address = $3F (VRAM Address / $400)
  ldx.w #(TermPrivate.BGTILEMAPLINE1)
  stx.b TermPrivate.wCursorPos  //Also save starting cursor position, using direct page

  lda.b #%00000000  // BBBBAAAA: A = BG1 Tile Address, B = BG2 Tile Address
  sta.w REG_BG12NBA // $210B: BG1 Tile Address = $0 (VRAM Address / $1000)

  lda.b #%00000001 // Enable BG1
  sta.w REG_TM // $212C: BG1 To Main Screen Designation
  
  //Turn the screen on
  //Set screen to max brightness
  lda.b #0x0F // 8-Bit A
  sta.w REG_INIDISP // $2100: Turn Screen on to Brightness Level

  //Unlink
  plp  
  plb
  ply
  plx
  pla  
rtl


//Requires direct page be set to page for terminal variables
fCLEARTERM24BITS:
  //Link
  pha
  phx
  phy
  phb
  php

  rep #0x20              // Set 16-Bit Accumulator
  sep #0x10              // 8-bit x,y

  lda.w #TermPrivate.wCursorPos >> 8 << 8 //Direct page bank to variable bank
  pha                    //Direct page for static variables
  pld                    //Direct page will use bank variable page  
  //Set direct page
  lda.w #(TermPrivate.BGTILEMAPLINE1)
  sta.b TermPrivate.wCursorPos  //Also save starting cursor position, using direct page
  ldx.b #0x00
  stx.b TermPrivate.bScroll

  stz.w REG_BG1HOFS // Store Zero To BG1 Horizontal Scroll Pos Low Byte
  stz.w REG_BG1HOFS // Store Zero To BG1 Horizontal Scroll Pos High Byte
  ldx.b #TermPrivate.BGSCROLLRESETLOW
  stx.w REG_BG1VOFS // Store Zero To BG1 Vertical Scroll Pos Low Byte
  stx.b TermPrivate.bScrollPosition
  stz.w REG_BG1VOFS // Store Zero To BG1 Vertical Pos High Byte
  //Wait for Vblank (NMI)
  
  //Source address
  sep #0x10                // 8-bit x,y
  rep #0x20                // 16-bit accumulator
  ldx.b #(TermPrivate.BGCLEAR2 >> 16)
  phx
  plb
  lda.w #TermPrivate.BGCLEAR2//Character for space
  rep   #0x10                // 16-bit x,y
  ldx.w #0x7800              // Dest
  ldy.w #0x0400              // Size
  //db 0x42, 0x00
  jsl fClEARVRAM24BITS
  
  //Unlink
  plp  
  plb
  ply
  plx
  pla
rtl

//SRC 16 bit address in accumulator and 8 bit bank in db
//Dest 16 bit address of vram destiantion in x register
//Size 16 bit size in y register
//DMA channel 1
fClEARVRAM24BITS:
  //Link
  php
  pha                //Source address
  phx                //Destination
  phy                //Size

  //Wait Vblank
  sep #0x20              // Set 8-Bit Accumulator
  WaitHVB()    
  rep #0x20              // Set 16-Bit Accumulator

  rep #0x20              // 16-bit accumulator
  sep #0x10              // 8-bit x,y

  // Transfer Lo Byte
  stz.w REG_VMAIN    // Set Increment VRAM Address After Accessing Lo Byte ($2115: Video Port Control)
  //lda.w #{DEST} >> 1 // Set VRAM Destination
  lda   3,s
  lsr
  sta.w REG_VMADDL   // $2116: VRAM

  ldx.b #$08           // Set DMA Mode (Write Byte, Fixed Source)
  stx.w REG_DMAP1 // $43X0: DMA Control
  ldx.b #$18           // Set Destination Register ($2118: VRAM Write)
  stx.w REG_BBAD1 // $43X1: DMA Destination
  //lda.w #{SRC}         // Set Source Offset (Lo Byte)
  lda   5,s
  sta.w REG_A1T1L // $43X2: DMA Source
  //ldx.b #{SRC} >> 16   // Set Source Bank
  phb
  plx
  stx.w REG_A1B1  // $43X4: Source Bank
  //lda.w #{SIZE}        // Set Size In Bytes To DMA Transfer
  lda 1,s
  sta.w REG_DAS1L // $43X5: DMA Transfer Size/HDMA

  ldx.b #$01 << 1 // Start DMA Transfer On Channel
  stx.w REG_MDMAEN     // $420B: DMA Enable  

  // Transfer Hi Byte
  ldx.b #$80         // Set Increment VRAM Address After Accessing Hi Byte
  stx.w REG_VMAIN    // $2115: Video Port Control
  //lda.w #{DEST} >> 1 // Set VRAM Destination
  lda   3,s
  sta.w REG_VMADDL   // $2116: VRAM

  ldx.b #$19           // Set Destination Register ($2119: VRAM Write)
  stx.w REG_BBAD1 // $43X1: DMA Destination
  //lda.w #({SRC} + 1)   // Set Source Offset (Hi Byte)
  lda   5,s
  inc
  sta.w REG_A1T1L // $43X2: DMA Source
  //lda.w #{SIZE}        // Set Size In Bytes To DMA Transfer
  lda   1,s
  sta.w REG_DAS1L // $43X5: DMA Transfer Size/HDMA

  ldx.b     #%10000000   // Force VBlank by turning off the screen.
  stx.w     $2100
  ldx.b #$01 << 1 // Start DMA Transfer On Channel
  stx.w REG_MDMAEN     // $420B: DMA Enable  
  ldx.b     #%00001111   //End of forced Vblank
  stx.w     $2100

  rep #0x10              // 16-bit x,y
  //Unlink
  pla
  pla
  pla
  plp
rtl

//Passes 24 bit address to string
//Lower 16 bits in x register
//Upper 8 bits in data bank register
//Direct page to terminal variables
//Text can't cross bank bonaries
//Uses DMA0 for VRAM access
fPRINTTERM24BITS:
  //Link context save
  pha
  phx
  phy
  phb
  php
  //Variables link
  rep #0x20   //16 bit accumulator
  rep #0x10   //16 bit x,y register
  phx         //16-bit String pointer
  lda TermPrivate.wCursorPos
  pha         //Original Cursor position
  pha         //16 bit total size
  pha         //16 bit current segment size
  pha         //16 bit current subsegment size

  //Calculate total size
  lda 9,s    //String offset in db bank
  tax
  dex
  -
  inx
  lda 0x0000,x   //Grab charatcer at x offset in db bank
  and #0x00FF    //Only one byte at a time
  cmp #0x0000
  bne -          //If not null byte keep reading
  txa
  sec
  sbc 9,s        //Endoffset-Start offset
  sta 5,s        //Total size calculated

  //Seperate into segments
  lda 7,s         //Original wCursorPos
  clc
  adc 5,s         //Size
  cmp #0x4000 + 1
  bmi +           //If segment goes over the limit
  //Print segment of size accumulator
  lda 7,s         //Calculate space left
  sec
  sbc #0x4000 + 1
  eor #0xFFFF
  sta 1,s         //Amount of characters left that can be printed on this line
  
  lda 9,s
  tax
  lda 1,s
  jsr TermPrivate.fPRINTTERMSIZE16BITS
  //Now print the second half
  lda 1,s
  cmp #0x0000
  beq ENDPRINT24
  //Only if needed
  //db 0x42, 0x00  
  lda 9,s
  clc
  adc 1,s
  tax
  lda 5,s
  sec
  sbc 1,s
  jsr TermPrivate.fPRINTTERMSIZE16BITS
  bra ++
  +
  lda 9,s
  tax
  lda 5,s
  jsr TermPrivate.fPRINTTERMSIZE16BITS
  +  

  //CLear the line right below the newly printed line
  //SRC 16 bit address in accumulator and 8 bit bank in db
  //Dest 16 bit address of vram destiantion in x register
  //Size 16 bit size in y register
  //Set size to cover next line 
  lda.b TermPrivate.wCursorPos
  clc
  adc #0x0020     //Next Line
  cmp #0x3FFF + 1
  bmi +           //If past the tile map limit
  db 0x42, 0x00   
  lda.b TermPrivate.wCursorPos
  sec
  sbc.w #0x4001
  eor #0xFFFF
  //If size of zero clear nothing
  cmp #0x0000
  beq ENDPRINT24  
  tay             //Size
  bra ++
  +               //If not past the tile map limit
  lda.b TermPrivate.wCursorPos
  clc
  adc #0x0020     //Next Line
  and #0xFFF0  
  sec
  sbc.b TermPrivate.wCursorPos
  tay             //Size
  +               

  sep #0x10                // 8-bit x,y
  ldx.b #(TermPrivate.BGCLEAR2 >> 16)
  phx
  plb
  rep #0x10
  lda.b TermPrivate.wCursorPos
  asl  
  tax                        // Dest 
  lda.w #TermPrivate.BGCLEAR2//Character for space
  jsl fClEARVRAM24BITS

  ENDPRINT24:
  //Variable unlink
  pla
  pla
  pla
  pla
  pla
  //Unlink context restore
  plp  
  plb
  ply
  plx
  pla
rtl
  
//Private function
//Assumes that no print crosses right bondary
//Parameters
//Data bank of string location in databank register
//Directpage of terminal cursor page in directpage register
//16 bit pointer to string in x register
//16 bit size of string in accumulator
//Explanation
//Prints string of size x to terminal
//Does not wait for a null byte
//Requirements:
//16 bit accumulator
//16 bit x,y registers
//Uses DMA channel 0
scope TermPrivate {
  fPRINTTERMSIZE16BITS:
  //Link context save
  pha
  phx
  phy
  phb
  php  
  //Set up stack (Link)
  phx       //16 bit variable wCursor original
  phx       //16 bit variable
  phx       //16-bit pointer
  pha       //16 bit size of string
  phx       //16 bit string pointer
  lda.b wCursorPos
  sta 9,s
  
  //Reset Write head if over the limit
  cmp #0x3FFF + 1
  bmi +
  lda #TermPrivate.BGTILEMAPLINE1
  sta.b wCursorPos
  bra +
  nop
  +
  //Skip if transfer size is 0
  lda 3,s
  cmp #0x0000
  beq ENDPRINTTERMSIZE24 

  //Wait Vblank
  sep #0x20              // Set 8-Bit Accumulator
  WaitHVB()    
  rep #0x20              // Set 16-Bit Accumulator
  
  //Call Load LoVram
  lda 3,s
  tay
  lda.b TermPrivate.wCursorPos
  tax
  lda 1,s
  jsl fLOADVRAMLOW
  //Increment wCursor
  lda.b TermPrivate.wCursorPos
  clc
  adc 3,s
  sta TermPrivate.wCursorPos

  //Update scroll register
  //Check scrolling stuff   
  lda.b TermPrivate.bScroll
  and #0x0001
  cmp #0x0001
  bne +        //If flag is set
    //Only 28 lines on screen at a time
    lda.b TermPrivate.wCursorPos
    and #0x0FF0
    lsr;lsr;lsr;lsr
    lsr
    sec
    sbc #0x007B
    asl
    asl
    asl
    dec
    and #0x00FF
    sep #0x20   //8 bit accumulator
    sta.b TermPrivate.bScrollPosition
    rep #0x20   //16 bit accumulator
    bra ENDSCROLL24  
  +            //Else if flag is not set
  //Check to see if past the scroll point
  lda.b TermPrivate.wCursorPos
  clc
  adc 3,s
  cmp #0x3f80
  bmi +
    //Set the flag high
    lda.b TermPrivate.bScroll
    and #0xFFFE
    clc
    adc #0x0001
    sta TermPrivate.bScroll  
  +  
  ENDSCROLL24:
  sep #0x20
  lda.b TermPrivate.bScrollPosition
  sta.w REG_BG1VOFS
  stz.w REG_BG1VOFS // Store Zero To BG1 Vertical Pos High Byte
  rep #0x20
  //End of scrolling stuff

  //Restore stack (Unlink)
  ENDPRINTTERMSIZE24:
  pla
  pla
  pla
  pla
  pla
  //Unlink context restore
  plp  
  plb
  ply
  plx
  pla  
  rts
}

//LoadLOVRAM(HELLOWORLD, $F944, 13, 0) // Load Text To VRAM Lo Bytes
//Accumulator 16 bit pointer, db for upper 8 bits
//X register 16 bit VRAM destination
//Y register 16 bit size of data in bytes
//Kind of breaks with a size of 0
fLOADVRAMLOW:
  //Link context save
  pha
  phx
  phy
  phb
  php  
  //Set up stack (Link)
  pha     //16-bit pointer
  phx     //16- bit VRAM destination
  phy     //16-bit size in bytes

  stz.w REG_VMAIN    // Set Increment VRAM Address After Accessing Lo Byte ($2115: Video Port Control)
  //ldx.w #{DEST} >> 1 // Set VRAM Destination
  lda 3,s
  //lsr
  sta.w REG_VMADDL   // $2116: VRAM

  //stz.w REG_DMAP0    // Set DMA Mode (Write Byte, Increment Source) ($43X0: DMA Control)
  sep #0x20
  lda.b #$18              // Set Destination Register ($2118: VRAM Write)
  sta.w REG_BBAD0    // $43X1: DMA Destination
  rep #0x20
  //ldx.w #{SRCTILES}       // Set Source Offset
  lda 5,s
  sta.w REG_A1T0L    // $43X2: DMA Source
  //lda.b #{SRCTILES} >> 16 // Set Source Bank
  sep #0x20
  phb
  pla
  sta.w REG_A1B0     // $43X4: Source Bank
  rep #0x20
  //ldx.w #{SIZETILES}      // Set Size In Bytes To DMA Transfer
  lda 1,s
  sta.w REG_DAS0L    // $43X5: DMA Transfer Size/HDMA
  sep #0x20
  lda.b #$01 << 0 // Start DMA Transfer On Channel
  sta.w REG_MDMAEN     // $420B: DMA Enable
  rep #0x20

  //Unlink Variables
  plx
  plx
  plx
  //Unlink context restore
  plp  
  plb
  ply
  plx
  pla
  rtl