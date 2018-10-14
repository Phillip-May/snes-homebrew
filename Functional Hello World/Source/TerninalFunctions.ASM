//Passes 16 bit address to string in 16 x register
//Text can't cross bank bonaries
//Uses DMA0 for VRAM access
fRINTTERM16BITS:
  //Get length of string
  sep #0x20 // Set 8-Bit Accumulator
  phx //Original 16 bit address
  dex //Start at first character
  -
    inx
    lda 0x0000,x
    cmp #0x00
    bne - //If not null byte keep reading
  //Number of bytes in string is original new x - orignal x
  rep #0x20 // Set 16-Bit Accumulator
  txa //New x
  //Original x is current value on stack
  sbc 1,s //One value forward because it's a 16 bit number
  //Size of string is now in accumulator
  pha
  //Move x number of bytes to VRAM tilemap (screen base)
  //Only Lo bytes as Hi bytes are tile properties
  //DMA setup stuff
  sep #0x20			 // Set 8-bit accumulator
  stz.w REG_VMAIN    // Set Increment VRAM Address After Accessing Lo Byte ($2115: Video Port Control)
  ldx.w #BGTILEMAPLINE1>> 1 // Set VRAM Destination
  stx.w REG_VMADDL   // $2116: VRAM

  stz.w REG_DMAP0    // Set DMA Mode (Write Byte, Increment Source) ($43X0: DMA Control)
  lda.b #$18         // Set Destination Register ($2118: VRAM Write)
  sta.w REG_BBAD0    // $43X1: DMA Destination
  plx			     // Set Size In Bytes To DMA Transfer
  stx.w REG_DAS0L    // $43X5: DMA Transfer Size/HDMA
  plx       		 // Set Source Offset
  stx.w REG_A1T0L    // $43X2: DMA Source
  //Set Source Bank, Source Bank is current 
  phb
  pla
  sta.w REG_A1B0     // $43X4: Source Bank
  lda.b #$01 << 0    // Start DMA Transfer On Channel
  sta.w REG_MDMAEN   // $420B: DMA Enable
rts

//Passes 24 bit address to string
//Lower 16 bits in x register
//Upper 8 bits in accumulator
//Text can't cross bank bonaries
//Uses DMA0 for VRAM access
fRINTTERM24BITS:
  //Get length of string
  //Set data bank for memory fetches
  //to where the string is located.
  //Data bank was passed in accumulator
  pha
  plb
  phx //Original 16 bit address
  dex //Start at first character
  -
    inx
    lda 0x0000,x
    cmp #0x00
    bne - //If not null byte keep reading
  //Number of bytes in string is original new x - orignal x
  rep #0x20 // Set 16-Bit Accumulator
  txa //New x
  //Original x is current value on stack
  sbc 1,s //One value forward because it's a 16 bit number
  //Size of string is now in accumulator
  pha
  //Move x number of bytes to VRAM tilemap (screen base)
  //Only Lo bytes as Hi bytes are tile properties
  //DMA setup stuff
  sep #0x20			 // Set 8-bit accumulator
  stz.w REG_VMAIN    // Set Increment VRAM Address After Accessing Lo Byte ($2115: Video Port Control)
  ldx.w #BGTILEMAPLINE1>> 1 // Set VRAM Destination
  stx.w REG_VMADDL   // $2116: VRAM

  stz.w REG_DMAP0    // Set DMA Mode (Write Byte, Increment Source) ($43X0: DMA Control)
  lda.b #$18         // Set Destination Register ($2118: VRAM Write)
  sta.w REG_BBAD0    // $43X1: DMA Destination
  plx			     // Set Size In Bytes To DMA Transfer
  stx.w REG_DAS0L    // $43X5: DMA Transfer Size/HDMA
  plx       		 // Set Source Offset
  stx.w REG_A1T0L    // $43X2: DMA Source
  //Set Source Bank, Source Bank is current 
  phb
  pla
  sta.w REG_A1B0     // $43X4: Source Bank
  lda.b #$01 << 0    // Start DMA Transfer On Channel
  sta.w REG_MDMAEN   // $420B: DMA Enable
rtl