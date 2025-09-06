arch snes.cpu
//The temrinal module runs on Mode 1
//8x8 tiles
//32x32 tile background
//Lowest priority

macro mCALLPRINTTERM24BITS(sz24BIT) {
  //Context save 
  php
  //Setup
  sep #0x20              // Set 8-Bit Accumulator
  rep #0x10              // 16-bit x,y
  lda.b #{sz24BIT} >> 16 //Give bank via data bank register
  pha                    //Push onto stack
  plb                    //Give upper bytes of pointer
  ldx.w #{sz24BIT}       //Give address via x reigster
  rep #0x20
  lda.w #{sz24BIT}
  jsl fStrlen24
  //Accumulator contains size
  jsl fPrintString24
  plp
}