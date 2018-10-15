//Constants
scope TermPrivate {
  //Tile map start location
  constant BGTILEMAPLINE1($7840) //First line is partially cutoff
  
  //Variables
  //Cursor current position (VRAM location)
  //Shifted//Upper byte shifted by 1 over 1
  constant wCursorPos($000105)
}

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
  phx				//16-bit pointer
  pha				//16 bit size
  sep #0x10         //8-bit x,y
  rep #0x20			//16-bit accumulator
  //Move x number of bytes to VRAM tilemap (screen base)
  cmp #0x0000		 //Make sure transfer is not 0 bytes
  beq +				 //Skip to the end if zero

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
  
  db 0x42, 0x00
  //Wait for VBlank
  sep #0x20
  WaitHVB()


  ldx.b #$01 << 0    // Start DMA Transfer On Channel
  stx.w REG_MDMAEN   // $420B: DMA Enable
  //Increment cursor position variable, x register is current size
  rep #0x20			 //16 bit accumulator
  lda 1,s 			 //Load size
  adc.b wCursorPos	 //Add to old possition
  dec 				 //Account for null byte
  sta.b wCursorPos
  //Restore stack (Unlink)
  +
  pla
  pla
  rts
}
