#include "include\SNES.h"

//Based on the init snes macro by peter Lemons.
//Takes 8 bit unsigned integer RomSpeed, 0 means Slow ROM, 1 means Fast ROM
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
	
	
	asm{
		ldx #$0000
		stx $2181
		stz $2183
		ldx $8008
		stx $4300
		BYTE $42, $00
	}
	

	asm{
		ldx #$FF00 //This should not be hardcoded but I cant get it to work othwerise
		stx $4302
	}
	REG_A1B0 = 0x00;
	
  	REG_DAS0L = 0x00;// DMA0 DMA Count / Indirect HDMA Address: Transfer 64KB ($4305)
	
	REG_MDMAEN = 0x01;// Select General Purpose DMA Channels & Start Transfer ($420B)
    asm{
		NOP
	} //delay, the c compiler is probably doing this on its own though
	REG_MDMAEN = 0x01; // Select General Purpose DMA Channels & Start Transfer: $2181..$2183 & $4305 Wrap Appropriately ($420B)	

	// VRAM
	REG_VMAIN = 0x80;
	REG_VMADDL = 0x00;
	REG_VMADDH = 0x00;
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