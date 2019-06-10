#include "include\SNES.h"
#include <STDINT.H>

//Based on the init snes macro by peter Lemons.
//Takes 8 bit unsigned integer RomSpeed, 0 means Slow ROM, 1 means Fast ROM
//I also added some other hardware related functions because they are hardware
//Related and needlessly big.
//I am aware a switch is not ideal here, you could just add a constant to the
//registers offset but I chose to use a switch anyway in the hope it's
//optimised out.
void initSNES(uint8_t RomSpeed){

	int i;
	int cCONSTZERO = 0;
	int* pCONSTZERO = &cCONSTZERO;
	REG_MEMSEL = RomSpeed;  // Access Cycle Designation (Slow ROM / Fast ROM)

	REG_INIDISP = 0x8F;// Display Control 1: Brightness & Screen Enable Register ($2100)

	REG_OBSEL = 0x00;   // Object Size & Object Base ($2101)
	REG_OAMADDL = 0x00; // OAM Address (Lower 8-Bit) ($2102)
	REG_OAMADDH = 0x00; // OAM Address (Upper 1-Bit) & Priority Rotation ($2103)
	REG_BGMODE = 0x00;  // BG Mode & BG Character Size: Set Graphics Mode 0 ($2105)
	REG_MOSAIC = 0x00;  // Mosaic Size & Mosaic Enable: No Planes, No Mosiac ($2106)
	REG_BG1SC = 0x00;   // BG1 Screen Base & Screen Size: BG1 Map VRAM Location = $0000 ($2107)
	REG_BG2SC = 0x00;   // BG2 Screen Base & Screen Size: BG2 Map VRAM Location = $0000 ($2108)
	REG_BG3SC = 0x00;   // BG3 Screen Base & Screen Size: BG3 Map VRAM Location = $0000 ($2109)
	REG_BG4SC = 0x00;   // BG4 Screen Base & Screen Size: BG4 Map VRAM Location = $0000 ($210A)
	REG_BG12NBA = 0x00; // BG1/BG2 Character Data Area Designation: BG1/BG2 Tile Data Location = $0000 ($210B)
	REG_BG34NBA = 0x00; // BG3/BG4 Character Data Area Designation: BG3/BG4 Tile Data Location = $0000 ($210C)
	REG_BG1HOFS = 0x00; // BG1 Horizontal Scroll (X) / M7HOFS: BG1 Horizontal Scroll 1st Write = 0 (Lower 8-Bit) ($210D)
	REG_BG1HOFS = 0x00; // BG1 Horizontal Scroll (X) / M7HOFS: BG1 Horizontal Scroll 2nd Write = 0 (Upper 3-Bit) ($210D)
	REG_BG1VOFS = 0x00; // BG1 Vertical   Scroll (Y) / M7VOFS: BG1 Vertical   Scroll 1st Write = 0 (Lower 8-Bit) ($210E)
	REG_BG1VOFS = 0x00; // BG1 Vertical   Scroll (Y) / M7VOFS: BG1 Vertical   Scroll 2nd Write = 0 (Upper 3-Bit) ($210E)
	REG_BG2HOFS = 0x00; // BG2 Horizontal Scroll (X): BG2 Horizontal Scroll 1st Write = 0 (Lower 8-Bit) ($210F)
	REG_BG2HOFS = 0x00; // BG2 Horizontal Scroll (X): BG2 Horizontal Scroll 2nd Write = 0 (Upper 3-Bit) ($210F)
	REG_BG2VOFS = 0x00; // BG2 Vertical   Scroll (Y): BG2 Vertical   Scroll 1st Write = 0 (Lower 8-Bit) ($2110)
	REG_BG2VOFS = 0x00; // BG2 Vertical   Scroll (Y): BG2 Vertical   Scroll 2nd Write = 0 (Upper 3-Bit) ($2110)
	REG_BG3HOFS = 0x00; // BG3 Horizontal Scroll (X): BG3 Horizontal Scroll 1st Write = 0 (Lower 8-Bit) ($2111)
	REG_BG3HOFS = 0x00; // BG3 Horizontal Scroll (X): BG3 Horizontal Scroll 2nd Write = 0 (Upper 3-Bit) ($2111)
	REG_BG3VOFS = 0x00; // BG3 Vertical   Scroll (Y): BG3 Vertical   Scroll 1st Write = 0 (Lower 8-Bit) ($2112)
	REG_BG3VOFS = 0x00; // BG3 Vertical   Scroll (Y): BG3 Vertical   Scroll 2nd Write = 0 (Upper 3-Bit) ($2112)
	REG_BG4HOFS = 0x00; // BG4 Horizontal Scroll (X): BG4 Horizontal Scroll 1st Write = 0 (Lower 8-Bit) ($2113)
	REG_BG4HOFS = 0x00; // BG4 Horizontal Scroll (X): BG4 Horizontal Scroll 2nd Write = 0 (Upper 3-Bit) ($2113)
	REG_BG4VOFS = 0x00; // BG4 Vertical   Scroll (Y): BG4 Vertical   Scroll 1st Write = 0 (Lower 8-Bit) ($2114)
	REG_BG4VOFS = 0x00; // BG4 Vertical   Scroll (Y): BG4 Vertical   Scroll 2nd Write = 0 (Upper 3-Bit) ($2114)
	//
	REG_M7A = 0x00; // Mode7 Rot/Scale A (COSINE A) & Maths 16-Bit Operand: 1st Write = 0 (Lower 8-Bit) ($211B)
	REG_M7A = 0x01; // Mode7 Rot/Scale A (COSINE A) & Maths 16-Bit Operand: 2nd Write = 1 (Upper 8-Bit) ($211B)
	REG_M7B = 0x00; // Mode7 Rot/Scale B (SINE A)   & Maths  8-bit Operand: 1st Write = 0 (Lower 8-Bit) ($211C)
	REG_M7B = 0x00; // Mode7 Rot/Scale B (SINE A)   & Maths  8-bit Operand: 2nd Write = 0 (Upper 8-Bit) ($211C)
	REG_M7C = 0x00; // Mode7 Rot/Scale C (SINE B): 1st Write = 0 (Lower 8-Bit) ($211D)
	REG_M7C = 0x00; // Mode7 Rot/Scale C (SINE B): 2nd Write = 0 (Upper 8-Bit) ($211D)
	REG_M7D = 0x00; // Mode7 Rot/Scale D (COSINE B): 1st Write = 0 (Lower 8-Bit) ($211E)
	REG_M7D = 0x01;// Mode7 Rot/Scale D (COSINE B): 2nd Write = 1 (Upper 8-Bit) ($211E)
	REG_M7X = 0x00; // Mode7 Rot/Scale Center Coordinate X: 1st Write = 0 (Lower 8-Bit) ($211F)
	REG_M7X = 0x00; // Mode7 Rot/Scale Center Coordinate X: 2nd Write = 0 (Upper 8-Bit) ($211F)
	REG_M7Y = 0x00; // Mode7 Rot/Scale Center Coordinate Y: 1st Write = 0 (Lower 8-Bit) ($2120)
	REG_M7Y = 0x00; // Mode7 Rot/Scale Center Coordinate Y: 2nd Write = 0 (Upper 8-Bit) ($2120)	
	
	REG_W12SEL = 0x00;  // Window BG1/BG2  Mask Settings = 0 ($2123)
	REG_W34SEL = 0x00;  // Window BG3/BG4  Mask Settings = 0 ($2124)
	REG_WOBJSEL = 0x00; // Window OBJ/MATH Mask Settings = 0 ($2125)
	REG_WH0 = 0x00;     // Window 1 Left  Position (X1) = 0 ($2126)
	REG_WH1 = 0x00;     // Window 1 Right Position (X2) = 0 ($2127)
	REG_WH2 = 0x00;     // Window 2 Left  Position (X1) = 0 ($2128)
	REG_WH3 = 0x00;     // Window 2 Right Position (X2) = 0 ($2129)
	REG_WBGLOG = 0x00;  // Window 1/2 Mask Logic (BG1..BG4) = 0 ($212A)
	REG_WOBJLOG = 0x00; // Window 1/2 Mask Logic (OBJ/MATH) = 0 ($212B)
	REG_TM = 0x00;      // Main Screen Designation = 0 ($212C)
	REG_TS = 0x00;      // Sub  Screen Designation = 0 ($212D)
	REG_TMW = 0x00;     // Window Area Main Screen Disable = 0 ($212E)
	REG_TSW = 0x00;     // Window Area Sub  Screen Disable = 0 ($212F)
	
	REG_CGWSEL = 0x30;  // Color Math Control Register A = $30 ($2130)
	REG_CGADSUB = 0x00; // Color Math Control Register B = 0 ($2131)	
	
	REG_COLDATA = 0xE0; // Color Math Sub Screen Backdrop Color = $E0 ($2132)
	REG_SETINI = 0x00; // Display Control 2 = 0 ($2133)
	
	REG_JOYWR = 0x00; // Joypad Output = 0 ($4016)
	
	REG_NMITIMEN = 0x00; // Interrupt Enable & Joypad Request: Reset VBlank, Interrupt, Joypad ($4200)
	
	REG_WRIO = 0xFF; // Programmable I/O Port (Open-Collector Output) = $FF ($4201)
	
	REG_WRMPYA = 0x00; // Set Unsigned  8-Bit Multiplicand = 0 ($4202)
    REG_WRMPYB = 0x00; // Set Unsigned  8-Bit Multiplier & Start Multiplication = 0 ($4203)
    REG_WRDIVL = 0x00; // Set Unsigned 16-Bit Dividend (Lower 8-Bit) = 0 ($4204)
    REG_WRDIVH = 0x00; // Set Unsigned 16-Bit Dividend (Upper 8-Bit) = 0 ($4205)
    REG_WRDIVB = 0x00; // Set Unsigned  8-Bit Divisor & Start Division = 0 ($4206)
    REG_HTIMEL = 0x00; // H-Count Timer Setting (Lower 8-Bit) = 0 ($4207)
    REG_HTIMEH = 0x00; // H-Count Timer Setting (Upper 1-Bit) = 0 ($4208)
    REG_VTIMEL = 0x00; // V-Count Timer Setting (Lower 8-Bit) = 0 ($4209)
    REG_VTIMEH = 0x00; // V-Count Timer Setting (Upper 1-Bit) = 0 ($420A)
    REG_MDMAEN = 0x00; // Select General Purpose DMA Channels & Start Transfer = 0 ($420B)
    REG_HDMAEN = 0x00; // Select H-Blank DMA (H-DMA) Channels = 0 ($420C)	
	
    //Clear OAM
	for(i = 0; i < 0x80; i++){
		REG_OAMDATA = 0xE0;
		REG_OAMDATA = 0xE0;
		REG_OAMDATA = 0x00;
		REG_OAMDATA = 0x00;
	}
	
	for(i = 0; i < 20; i++){
		REG_OAMDATA = 0x00;
	}
	
	// Clear WRAM
	//REG_WMADDL = 0x00;// WRAM Address (Lower  8-Bit): Transfer To $7E:0000 ($2181)
	//REG_WMADDM = 0x00;// WRAM Address (Lower  8-Bit): Transfer To $7E:0000 ($2181)
	//REG_WMADDH = 0x00; // WRAM Address (Upper  1-Bit): Select 1st WRAM Bank = $7E ($2183)

	//REG_DMAP0 = 0x8008;// DMA0 DMA/HDMA Parameters ($4300)
	//REG_DAS0L = 0x00;// DMA0 DMA Count / Indirect HDMA Address: Transfer 64KB ($4305)
	//Clear CGRAM with DMA0
	REG_CGADD = 0x0000;
	REG_DMAP0 = 0x00;
	REG_BBAD0 = 0x22;
	REG_A1T0 = 0x0000;
	REG_A1B0 = 0x00;
	REG_DAS0 = 0x01FF;
	REG_MDMAEN = 0x01;	
	
	REG_WMADD = 0x0000;
	REG_WMADDH = 0x00;
	REG_DMAP0 = 0x08;
	
	REG_A1T0 = 0xFF00;

	REG_A1B0 = 0x00;
	
  	REG_DAS0L = 0x00;// DMA0 DMA Count / Indirect HDMA Address: Transfer 64KB ($4305)
	
	REG_MDMAEN = 0x01;// Select General Purpose DMA Channels & Start Transfer ($420B)
    asm{
		NOP
	} //delay, the c compiler is probably doing this on its own though
	REG_MDMAEN = 0x01; // Select General Purpose DMA Channels & Start Transfer: $2181..$2183 & $4305 Wrap Appropriately ($420B)	

	// VRAM
	REG_VMAIN = 0x80;
	REG_VMADD = 0x0000;
	REG_DAS0L = 0x00;
	REG_DAS0H = 0x00;
	
	REG_DMAP0 = 0x09;
	REG_BBAD0 = 0x18;
	
	REG_MDMAEN = 0x01;
	
    // CGRAM
	REG_CGADD = 0x00;
	REG_DAS0L = 0x00;
	REG_DAS0H = 0x02;
	REG_DMAP0 = 0x08;
	REG_BBAD0 = 0x22;
	REG_MDMAEN = 0x01;
}

//======================================
// LoadCGRam - Load Palette Data To CGRAM
//======================================
//  SRC: 24-Bit Address Of Source Data
// DEST: 8-Bit CGRAM Word Destination Address (Color # To Start On) * 2
// SIZE: Size Of Data (# Of Colours To Copy) in sets of two
// CHAN: DMA Channel To Transfer Data (0..7) This takes an int to prevent a 
// compiler warning.
int LoadCGRam(const unsigned char *pSource, uint16_t pCGRAMDestination, uint16_t cSize,
				int cChannel){
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);
	REG_CGADD = pCGRAMDestination;
	
	switch(cChannel){
		case 0:
			REG_DMAP0 = 0x00;
			REG_BBAD0 = 0x22;
			REG_A1T0 = regWrite1;
			REG_A1B0 = regWrite2;
			REG_DAS0 = cSize;
			REG_MDMAEN = 0x01;
			break;
		case 1:
			REG_DMAP1 = 0x00;
			REG_BBAD1 = 0x22;
			REG_A1T1 = regWrite1;
			REG_A1B1 = regWrite2;
			REG_DAS1 = cSize;
			REG_MDMAEN = 0x02;
			break;
		case 2:
			REG_DMAP2 = 0x00;
			REG_BBAD2 = 0x22;
			REG_A1T2 = regWrite1;
			REG_A1B2 = regWrite2;
			REG_DAS2 = cSize;
			REG_MDMAEN = 0x04;
			break;
		case 3:
			REG_DMAP3 = 0x00;
			REG_BBAD3 = 0x22;
			REG_A1T3 = regWrite1;
			REG_A1B3 = regWrite2;
			REG_DAS3 = cSize;
			REG_MDMAEN = 0x08;
			break;
		case 4:
			REG_DMAP4 = 0x00;
			REG_BBAD4 = 0x22;
			REG_A1T4 = regWrite1;
			REG_A1B4 = regWrite2;
			REG_DAS4 = cSize;
			REG_MDMAEN = 0x10;
			break;
		case 5:
			REG_DMAP5 = 0x00;
			REG_BBAD5 = 0x22;
			REG_A1T5 = regWrite1;
			REG_A1B5 = regWrite2;
			REG_DAS5 = cSize;
			REG_MDMAEN = 0x20;
			break;
		case 6:
			REG_DMAP6 = 0x00;
			REG_BBAD6 = 0x22;
			REG_A1T6 = regWrite1;
			REG_A1B6 = regWrite2;
			REG_DAS6 = cSize;
			REG_MDMAEN = 0x40;
			break;
		case 7:
			REG_DMAP7 = 0x00;
			REG_BBAD7 = 0x22;
			REG_A1T7 = regWrite1;
			REG_A1B7 = regWrite2;
			REG_DAS7 = cSize;
			REG_MDMAEN = 0x80;
			break;
		default:
			//Insert some compiler/run-time warning if possible
			return -1;
			break;
	}
	
	return 0;
}

//==================================
// LoadVRAM - Load GFX Data To VRAM
//==================================
//  SRC: 24-Bit Address Of Source Data
// DEST: 16-Bit VRAM Destination (WORD Address)
// SIZE: Size Of Data (BYTE Size)
// CHAN: DMA Channel To Transfer Data (0..7)
//Load Data into Vram, using DMA channel 0
int LoadVram(const unsigned char *pSource, uint16_t pVRAMDestination,
			 uint16_t cSize, int cChannel){
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	REG_VMAIN = 0x80;
	REG_VMADD = (pVRAMDestination >> 1);
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	switch(cChannel){
		case 0:
			REG_DMAP0 = 0x01;
			REG_BBAD0 = 0x18;
			REG_A1T0 = regWrite1;
			REG_A1B0 = regWrite2;
			REG_DAS0 = cSize;
			REG_MDMAEN = 0x01;			
			break;
		case 1:
			REG_DMAP1 = 0x01;
			REG_BBAD1 = 0x18;
			REG_A1T1 = regWrite1;
			REG_A1B1 = regWrite2;
			REG_DAS1 = cSize;
			REG_MDMAEN = 0x02;			
			break;
		case 2:
			REG_DMAP2 = 0x01;
			REG_BBAD2 = 0x18;
			REG_A1T2 = regWrite1;
			REG_A1B2 = regWrite2;
			REG_DAS2 = cSize;
			REG_MDMAEN = 0x04;			
			break;
		case 3:
			REG_DMAP3 = 0x01;
			REG_BBAD3 = 0x18;
			REG_A1T3 = regWrite1;
			REG_A1B3 = regWrite2;
			REG_DAS3 = cSize;
			REG_MDMAEN = 0x08;			
			break;
		case 4:
			REG_DMAP4 = 0x01;
			REG_BBAD4 = 0x18;
			REG_A1T4 = regWrite1;
			REG_A1B4 = regWrite2;
			REG_DAS4 = cSize;
			REG_MDMAEN = 0x10;			
			break;
		case 5:
			REG_DMAP5 = 0x01;
			REG_BBAD5 = 0x18;
			REG_A1T5 = regWrite1;
			REG_A1B5 = regWrite2;
			REG_DAS5 = cSize;
			REG_MDMAEN = 0x20;			
			break;
		case 6:
			REG_DMAP6 = 0x01;
			REG_BBAD6 = 0x18;
			REG_A1T6 = regWrite1;
			REG_A1B6 = regWrite2;
			REG_DAS6 = cSize;
			REG_MDMAEN = 0x40;			
			break;
		case 7:
			REG_DMAP7 = 0x01;
			REG_BBAD7 = 0x18;
			REG_A1T7 = regWrite1;
			REG_A1B7 = regWrite2;
			REG_DAS7 = cSize;
			REG_MDMAEN = 0x80;			
			break;
		default:
			//Insert some compiler/run-time warning if possible
			return -1;
			break;
	}		
	
	return 0;
}	

//=============================================
// LoadLOVRAM - Load GFX Data To VRAM Lo Bytes
//=============================================
//  SRCTILES: 24-Bit Address Of Source Tile Data
//      DEST: 16-Bit VRAM Destination (WORD Address)
// SIZETILES: Size Of Tile Data (BYTE Size)
//      CHAN: DMA Channel To Transfer Data (0..7)
int LoadLoVram(uint32_t pSource, uint16_t pVRAMDestination,
			 uint16_t cSize, int cChannel){
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	REG_VMAIN = 0x00;
	REG_VMADD = (pVRAMDestination >> 1);
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	REG_DMAP7 = 0x00;
	REG_BBAD7 = 0x18;
	REG_A1T7 = regWrite1;
	REG_A1B7 = regWrite2;
	REG_DAS7 = cSize;
	REG_MDMAEN = 0x80;
	return 0;
}

//===================================
// ClearVRAM - Clear VRAM Fixed Word 
//===================================
//  SRC: 24-Bit Address Of Source Data
// DEST: 16-Bit VRAM Destination (WORD Address)
// SIZE: Size Of Data (BYTE Size)
// CHAN: DMA Channel To Transfer Data (0..7)
int ClearVram(const unsigned char *pSource, uint16_t pVRAMDestination,
			 uint16_t cSize, int cChannel){
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	uint16_t regWrite3;

	regWrite3 = (pVRAMDestination >> 1);
	//Transfer LoByte
	REG_VMAIN = 0x00;
	REG_VMADD = regWrite3;
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	REG_DMAP6 = 0x08;
	REG_BBAD6 = 0x18;
	REG_A1T6 = regWrite1;
	REG_A1B6 = regWrite2;
	REG_DAS6 = cSize;
	REG_MDMAEN = 0x40;
	
	//Transfer HiByte
	REG_VMAIN = 0x80;
	REG_VMADD = regWrite3;
	regWrite1 = (uint16_t) ((uint32_t)pSource+1);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	REG_BBAD6 = 0x19;
	REG_A1T6 = regWrite1;
	REG_A1B6 = regWrite2;
	REG_DAS6 = cSize;
	REG_MDMAEN = 0x40;
	
	return 0;
}

//===================================
// LoadOAMCopy - Clear VRAM Fixed Word 
//===================================
//  SRC: 24-Bit Address Of Source Data
// DEST: 16-Bit OAM Destination (WORD Address)
// SIZE: Size Of Data (BYTE Size)
// CHAN: DMA Channel To Transfer Data (0..7)
int LoadOAMCopy(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize, int cChannel){
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	REG_OAMADD = pVRAMDestination;
	REG_DAS0 = cSize;
	REG_DMAP0 = 0x00;
	REG_BBAD0 = 0x04;
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;	
	
	REG_MDMAEN = 0x01;
	return 0;
}

int initOAMCopy(unsigned char *pSource){
	uint16_t i;
	for (i = 0; i < 128; i++){
		*pSource = 0x01;
		pSource++;
		*pSource = 0x00;
		pSource++;
		*pSource = 0x00;
		pSource++;
		*pSource = 0x00;
		pSource++;
	}
	for(i = 0; i < 32; i++){
		*pSource = 0x55;
		pSource++;
	}
	
	return 0;
}

//SPC700 stuff
int SPCWaitBoot() {
	uint8_t reg1 = 0x00;
	uint8_t reg2 = 0x00;
	
	while ( (reg1 != 0xAA) && (reg1 != 0xBB) ) {
		reg1 = REG_APUIO0BYTE;
		reg2 = REG_APUIO1BYTE;
	}
	return 0;
}

int TransferBlockSPC (unsigned char *srcAddr, uint16_t SPCDestAddr, uint16_t size) {
	uint8_t dummyRead;
	uint8_t dummyRead2;
	uint16_t byteWrite;
	uint8_t *localPointer;
	//The counter to assure each byte was transfered correctly
	//This lower 8 bits as a counter while the upper 16 bits are the index
	uint16_t byteWriteIndex = 0;
	
	localPointer = (uint8_t *)srcAddr;
	
	REG_APUIO2WORD = SPCDestAddr;
	dummyRead = REG_APUIO0BYTE;
	dummyRead += 0x22;
	//Special case
	if (dummyRead == 0) {
		dummyRead++;
	}
	REG_APUIO1BYTE = dummyRead;
	REG_APUIO0BYTE = dummyRead;
	
	//Wait for Acknowledgement
	do {
		dummyRead2 = REG_APUIO0BYTE;
	} while (dummyRead2 != dummyRead);
	
	do {		
		byteWrite = *localPointer;
		localPointer++;
		//SPCLoadByte
		REG_APUIO1BYTE = byteWrite;
		REG_APUIO0BYTE = byteWriteIndex;
		//Wait for Acknowledgement
		do {
			dummyRead2 = REG_APUIO0BYTE;
		} while (dummyRead2 != ((uint8_t)byteWriteIndex) );
		byteWriteIndex++;
	} while (byteWriteIndex != size);	
	
	return 0;
}

int SPCExecute(uint16_t startAddr) {
	uint16_t dummyX;
	uint8_t dummyA;
	uint8_t dummyRead;
	
	dummyX = startAddr;
	REG_APUIO2WORD = dummyX;
	REG_APUIO1BYTE = 0;
	dummyA = REG_APUIO0BYTE;
	dummyA += 0x22;
	REG_APUIO0BYTE = dummyA;
	//Wait for Acknowledgement
	do {
		dummyRead = REG_APUIO0BYTE;
	} while (dummyA != dummyRead);
	
	return 0;
}













