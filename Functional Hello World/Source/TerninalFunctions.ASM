//Constants
scope TermPrivate {
  //Tile map start location
  constant BGTILEMAPLINE1($7800) //First line is partially cutoff
  constant BGSCROLLRESETLOW(0xFF)//This fixes it

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

fTERMINIT24BITS:
  jsl fCLEARTERM24BITS      //Clears Vram tile map and waits for NMI
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

  lda.b #(TermPrivate.BGTILEMAPLINE1 >> (8+1)) + 0b00 //Upper byte shifted by 1
  sta REG_BG1SC     // $2108: BG1 32x32, BG1 Map Address = $3F (VRAM Address / $400)
  ldx.w #(TermPrivate.BGTILEMAPLINE1 >> 1)
  stx.b TermPrivate.wCursorPos  //Also save starting cursor position, using direct page

  lda.b #%00000000  // BBBBAAAA: A = BG1 Tile Address, B = BG2 Tile Address
  sta.w REG_BG12NBA // $210B: BG1 Tile Address = $0 (VRAM Address / $1000)

  lda.b #%00000001 // Enable BG1
  sta.w REG_TM // $212C: BG1 To Main Screen Designation
  
  //Turn the screen on
  //Set screen to max brightness
  lda.b #0x0F // 8-Bit A
  sta.w REG_INIDISP // $2100: Turn Screen To Brightness Level
rtl


//Requires direct page be set to page for terminal variables
fCLEARTERM24BITS:
  sep #0x20              // Set 8-Bit Accumulator
  rep #0x10              // 16-bit x,y
  ldx.w #TermPrivate.wCursorPos >> 8 << 8 //Direct page bank to variable bank
  phx                    //Direct page for static variables
  pld                    //Direct page will use bank variable page  
  //Set direct page
  ldx.w #(TermPrivate.BGTILEMAPLINE1 >> 1)
  stx.b TermPrivate.wCursorPos  //Also save starting cursor position, using direct page
  lda.b #0x00
  sta.b TermPrivate.bScroll

  stz.w REG_BG1HOFS // Store Zero To BG1 Horizontal Scroll Pos Low Byte
  stz.w REG_BG1HOFS // Store Zero To BG1 Horizontal Scroll Pos High Byte
  lda.b #TermPrivate.BGSCROLLRESETLOW
  sta.w REG_BG1VOFS // Store Zero To BG1 Vertical Scroll Pos Low Byte
  sta.b TermPrivate.bScrollPosition
  stz.w REG_BG1VOFS // Store Zero To BG1 Vertical Pos High Byte
  //Wait for Vblank (NMI)
  WaitHVB()

  rep #0x10              // 16-bit x,y
  ClearVRAM(TermPrivate.BGCLEAR2, $7800, $0400, 0) // Clear VRAM Map To Fixed Tile Word
rtl


//Passes 24 bit address to string
//Lower 16 bits in x register
//Upper 8 bits in data bank register
//Direct page to terminal variables
//8 bit accumulator
//Text can't cross bank bonaries
//Uses DMA0 for VRAM access
fPRINTTERM24BITS:
  //Setup stack (Link)  
  phx 		  //Original 16 bit address
  ldx #0x0000
  phx 		  //Variable String length

  //Get length of string, data bank is already correct
  rep #0x20   //16 bit accumulator
  lda 3,s 	  //Load 16 bit address
  tax
  dex 		  //Start at first character
  sep #0x20	  //8-bit accumulator
  -
    inx
    lda 0x0000,x
    cmp #0x0D //Line feed
    bne	++     //If line feed
      //Call print string with x value up to current
      //Change original address to address after after
      //print. Context save
      phx
      pha
      php
      //Calculate current size
      rep #0x20    // Set 16-Bit Accumulator
      txa 		   //New x, Original x is current value on stack
      sbc 7,s      //Subtract to get size
      //Size of string in bytes is now in accumulator
      tay       //y holds size temporarilly          
      lda 7,s   //Grab 16 bit pointer
      tax       //Store it in x
      tya 		//accumulator holds size
      //Databank and directpage are still correct
      jsr TermPrivate.fPRINTTERMSIZE24BITS
      //Linebreak code     
      lda.b TermPrivate.wCursorPos
	    bit #0x000F
	    beq + //Equal means zero flag

      lda.b TermPrivate.wCursorPos
      lsr; lsr; lsr; lsr; lsr;
      inc  //Increment to the next line
      asl; asl; asl; asl; asl
      //lda.b bScrollPosition
      //sbc #0x08
      //sta.b bScrollPosition
      +
      sta.b TermPrivate.wCursorPos
      //Update 16 bit address used to determine size
      lda 3,s 	   //Grab new start position from context save
      inc 		   //Special character means do not display
      sta 7,s //Update start position
      //Context restore
      plp
      pla
      plx
    +
    cmp #0x00
    bne - //If not null byte keep reading
  //Final print
  //Number of bytes in string is original new x - orignal x  
  rep #0x20 // Set 16-Bit Accumulator
  txa //New x  //Original x is current value on stack
  sbc 3,s //One value forward because it's a 16 bit number
  //Size of string in bytes is now in accumulator
  sta 1,s
  //Function call for print string
  //Data bank and direct page are already set
  //Size is already in 16 bit accumulator
  tay
  lda 3,s   	 	 //Grab 16 bit pointer
  tax 				 //Store it in the x register
  tya 				 //Restore 16 bit size to accumulator
  jsr TermPrivate.fPRINTTERMSIZE24BITS
  //Restore (Unlink) stack
  rep #0x10          // 16-bit x,y
  plx
  plx
rtl
  
//Private function
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
  fPRINTTERMSIZE24BITS:
  //Set up stack (Link)
  phx       //16 bit original wcurpos
  phx       //16 bit variable
  phx       //16 bit variable
  phx				//16-bit pointer
  pha				//16 bit size
  sep #0x10         //8-bit x,y
  rep #0x20			//16-bit accumulator
  lda.b wCursorPos
  sta 9,s

  //Move accumulator number of bytes to VRAM tilemap (screen base)
  lda 1,s
  cmp #0x0000    //Make sure transfer is not 0 bytes
  bne +     //Skip to the end if zero
  
  lda.b wCursorPos
  //Undo the scroll
  brl ENDPRINTTERMSIZE24
  +

  //If past the limit set the scroll flag
  lda 1,s          // Grab size
  adc wCursorPos
  cmp #0x3F80
  //Screen needs to start scrolling the first time wCursorPos passes 0x3f80
  bmi + //If (cursor pos > 0x3F80)
    lda #0x0001
    ora.b bScroll   //or so only flag is set
    sta.b bScroll
    //db 0x42, 0x00
    //lda.b bScrollPosition
    //sbc #0x08
    //sta.b bScrollPosition
  +
  
  //Make write head loop
  lda.b wCursorPos
  adc 1,s
  cmp #0x3FFF + 1
  //Need to reset write head to the top of the tile map past 0x3FFF
  bmi + //If (cursor pos > 0x3FFF)
    //Wait for VBlank
    php
    sep #0x20          //8 bit accumulator
    WaitHVB()  
    rep #0x20
    rep #0x10              // 16-bit x,y
    ldx.w #(TermPrivate.BGTILEMAPLINE1 >> 1)
    stx.b TermPrivate.wCursorPos  //Also save starting cursor position, using direct page
    //Clear the first two lines
    rep #0x10              // 16-bit x,y
    sep #0x20
    WaitHVB()
    //lda.b bScrollPosition
    //adc #0x08
    //sta.b bScrollPosition
    ClearVRAM(TermPrivate.BGCLEAR2, $7800, $0080, 0) // Clear VRAM Map To Fixed Tile Word
    sep #0x10
    plp        
  +

  ldx #0x00  
  lda #0x0001
  and bScroll   //and returns non zero it means flag is set
  beq ++         //Skip if scroll is not set
    lda 1,s          // Grab size
    adc.b wCursorPos   //Add to old possition
    //Acumulator contains new write head location
    //Calculate if over the limit for one line
    -                   //This calculates the line number of the current write head
      inx
      sbc #0x0020       //Subtract 32 for 32 characters per line
      bpl -             //If not negative
    lda #0x0000
    txa
    sta 5,s
    ldx #0x00
    lda.b wCursorPos    //Repeat the same calculation with old write head
    -                   //This calculates the line number of the current write head
      inx
      sbc #0x0020       //Subtract 32 for 32 characters per line
      bpl -             //If not negative
    //New minus old for lines to scroll
    lda #0x0000
    txa
    sta 7,s
    lda 5,s
    sec
    sbc 7,s
    //Make sure New is bigger than old
    bpl +
    //If that fails scroll size //32 with minimum of 1
    db 0x42, 0x00
    and #0x00FF
    //lsr; lsr; lsr; lsr; lsr;
    //cmp #0x0001
    //bne + //If a non zero value
    //lda #0x0000
    +
  +
  //accumulator now contains the number of lines to scroll
  asl
  asl
  asl
  adc.b bScrollPosition
  and #0x00FF
  sta.b bScrollPosition //New value for bScrollYRegister

  //used to be here
  
  //Clear the line in front of what is being printed
  rep #0x10              // 16-bit x,y
  sep #0x20
  WaitHVB()
  stz.w REG_VMAIN    // Set Increment VRAM Address After Accessing Lo Byte ($2115: Video Port Control)
  rep #0x20
  lda.b wCursorPos
  adc #0x0020  
  //ldx.w #$7800 >> 1 // Set VRAM Destination
  sta.w REG_VMADDL   // $2116: VRAM
  sep #0x20

  lda.b #$08           // Set DMA Mode (Write Byte, Fixed Source)
  sta.w REG_DMAP0 // $43X0: DMA Control
  lda.b #$18           // Set Destination Register ($2118: VRAM Write)
  sta.w REG_BBAD0 // $43X1: DMA Destination
  ldx.w #TermPrivate.BGCLEAR2         // Set Source Offset (Lo Byte)
  stx.w REG_A1T0L // $43X2: DMA Source
  lda.b #TermPrivate.BGCLEAR2 >> 16   // Set Source Bank
  sta.w REG_A1B0  // $43X4: Source Bank
  ldx.w #0x0060       // Set Size In Bytes To DMA Transfer
  stx.w REG_DAS0L // $43X5: DMA Transfer Size/HDMA

  lda.b #$01 << 0 // Start DMA Transfer On Channel
  sta.w REG_MDMAEN     // $420B: DMA Enable

  // Transfer Hi Byte
  lda.b #$80         // Set Increment VRAM Address After Accessing Hi Byte
  sta.w REG_VMAIN    // $2115: Video Port Control
  rep #0x20
  lda.b wCursorPos
  adc #0x0020   
  //ldx.w #$7800 >> 1 // Set VRAM Destination
  sta.w REG_VMADDL   // $2116: VRAM
  sep #0x20

  lda.b #$19           // Set Destination Register ($2119: VRAM Write)
  sta.w REG_BBAD0 // $43X1: DMA Destination
  ldx.w #(TermPrivate.BGCLEAR2 + 1)   // Set Source Offset (Hi Byte)
  stx.w REG_A1T0L // $43X2: DMA Source
  ldx.w #0x0060        // Set Size In Bytes To DMA Transfer
  stx.w REG_DAS0L // $43X5: DMA Transfer Size/HDMA

  lda.b #$01 << 0 // Start DMA Transfer On Channel
  WaitHVB()
  sta.w REG_MDMAEN     // $420B: DMA Enable
  sep #0x10
  rep #0x20

  //Only Lo bytes as Hi bytes are tile properties
  //DMA setup stuff
  stz.w REG_VMAIN    // Set Increment VRAM Address After Accessing Lo Byte ($2115: Video Port Control)
  lda.b wCursorPos	 // Direct page + variable offset, cursor position is current destination
  sta.w REG_VMADDL   // $2116: VRAM Set VRAM Destination

  stz.w REG_DMAP0    // Set DMA Mode (Write Byte, Increment Source) ($43X0: DMA Control)
  ldx.b #$18         // Set Destination Register ($2118: VRAM Write)
  stx.w REG_BBAD0    // $43X1: DMA Destination
  lda   1,s		     // Set Size In Bytes To DMA Transfer
  sta.w REG_DAS0L    // $43X5: DMA Transfer Size/HDMA
  lda   3,s    		 // Set Source Offset
  sta.w REG_A1T0L    // $43X2: DMA Source 
  phb				 // Set Source Bank
  plx 				 // Source Bank is current
  stx REG_A1B0       // $43X4: Source Bank

  //Wait for VBlank
  sep #0x20          //8 bit accumulator
  WaitHVB()

  //Update scroll register
  lda.b bScrollPosition
  sta.w REG_BG1VOFS // Store accumulator To BG1 Vertical Scroll Pos Low Byte
  stz.w REG_BG1VOFS // Store Zero To BG1 Vertical Pos High Byte

  ldx.b #$01 << 0    // Start DMA Transfer On Channel 0
  stx.w REG_MDMAEN   // $420B: DMA Enable
  //ldx.b #$01 << 1    // Start DMA Transfer On Channel 1
  //stx.w REG_MDMAEN   // $420B: DMA Enable
  //Increment cursor position variable, x register is current size
  rep #0x20			 //16 bit accumulator
  lda 1,s 			 //Load size
  adc.b wCursorPos	 //Add to old possition
  //dec                //Im not setting carry before subbing somewhere
  sta.b wCursorPos
  //Restore stack (Unlink)
  ENDPRINTTERMSIZE24:
  pla
  pla
  pla
  pla
  pla
  rts
}
