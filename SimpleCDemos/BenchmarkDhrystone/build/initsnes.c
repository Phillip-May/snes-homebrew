#include "snes_regs_xc.h"
#include "initsnes.h"

// Pointer splitting union for TCC816 compiler
typedef union {
    const unsigned char *ptr;
    struct {
        uint16_t low16;   // low 16 bits for A1T registers
        uint8_t  high8;   // high 8 bits for A1B registers
    } parts;
    uint32_t full;
} PtrSplit;

#ifdef __VBCC__
// VBCC816 stub implementations for malloc functions
void* farMalloc(uint32_t size) {
    (void)size; // Suppress unused parameter warning
    return NULL;
}

void free(void* ptr) {
    (void)ptr; // Suppress unused parameter warning
}

#endif

#ifdef __CALYPSI__
// Calypsi farmalloc is just malloc (calypsi malloc is automatically far)
void* farMalloc(uint32_t size) {
    return malloc(size);
}
#endif

#ifdef __mos__
// LLVM-MOS farmalloc stub (LLVM-MOS only has near malloc)
void* farMalloc(uint32_t size) {
    (void)size; // Suppress unused parameter warning
    return NULL; // Return NULL since far malloc is not available
}
#endif

#ifdef __JCC__
// JCC816 stub implementations for malloc functions
void* farMalloc(uint32_t size) {
    (void)size; // Suppress unused parameter warning
    return NULL;
}

void* nearMalloc(uint32_t size) {
    (void)size; // Suppress unused parameter warning
    return NULL;
}

void free(void* ptr) {
    (void)ptr; // Suppress unused parameter warning
}

#endif

#ifdef __CC65__
// CC65 farmalloc implementation using malloc
void* farMalloc(uint32_t size) {
    return malloc(size);
}
#endif



//Based on the init snes macro by peter Lemons.
//Takes 8 bit unsigned integer RomSpeed, 0 means Slow ROM, 1 means Fast ROM
//I also added some other hardware related functions because they are hardware
//Related and needlessly big.
//I am aware a switch is not ideal here, you could just add a constant to the
//registers offset but I chose to use a switch anyway in the hope it's
//optimised out.
void initSNES(uint8_t ROMSPEED){

	int i;
	volatile uint16_t cCONSTZERO = 0;
	volatile uint16_t* pCONSTZERO = &cCONSTZERO;
	REG_MEMSEL = ROMSPEED;  // Access Cycle Designation (Slow ROM / Fast ROM)

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
	

	// Clear CGRAM
	for(i = 0; i < 256; i++) {
		LoadCGRam((const unsigned char*)pCONSTZERO, i, sizeof(cCONSTZERO));
	}

	// Clear VRAM using existing ClearVram function in chunks
	// Clear VRAM in 32KB chunks (0x8000 bytes each) to avoid DMA size issues
	for(i = 0; i < 2; i++) {
		ClearVram((const unsigned char*)pCONSTZERO, i * 0x4000, 0x8000);
	}
	
#ifdef __TCC816__
	// Initialize TCC816 interrupt handling
	init_tcc816_interrupts();
#endif
}

//======================================
// LoadCGRam - Load Palette Data To CGRAM
//======================================
//  SRC: 24-Bit Address Of Source Data
// DEST: 8-Bit CGRAM Word Destination Address (Color # To Start On) * 2
// SIZE: Size Of Data (# Of Colours To Copy) in sets of two
// CHAN: DMA Channel To Transfer Data (0..7) This takes an int to prevent a 
// compiler warning.
FUNCTIONATR void LoadCGRam(const unsigned char *pSource, uint16_t pCGRAMDestination, uint16_t cSize){
#ifdef __TCC816__
	PtrSplit src;
	REG_CGADD = pCGRAMDestination;
	src.ptr = pSource;
	
	// Always use channel 0
	REG_DMAP0 = 0x00;
	REG_BBAD0 = 0x22;
	REG_A1T0 = src.parts.low16;
	REG_A1B0 = src.parts.high8;
	REG_DAS0L = (uint8_t)(cSize & 0xFF);
	REG_DAS0H = (uint8_t)((cSize >> 8) & 0xFF);
	REG_MDMAEN = 0x01;
#else
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);
	REG_CGADD = pCGRAMDestination;
	
	// Always use channel 0
	REG_DMAP0 = 0x00;
	REG_BBAD0 = 0x22;
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;
	REG_DAS0L = (uint8_t)(cSize & 0xFF);
	REG_DAS0H = (uint8_t)((cSize >> 8) & 0xFF);
	REG_MDMAEN = 0x01;
#endif
}

//==================================
// LoadVRAM - Load GFX Data To VRAM
//==================================
//  SRC: 24-Bit Address Of Source Data
// DEST: 16-Bit VRAM Destination (WORD Address)
// SIZE: Size Of Data (BYTE Size)
// CHAN: DMA Channel To Transfer Data (0..7)
//Load Data into Vram, using DMA channel 0
FUNCTIONATR void LoadVram(const unsigned char *pSource, uint16_t pVRAMDestination,
			 uint16_t cSize){
#ifdef __TCC816__
	PtrSplit src;
	REG_VMAIN = 0x80;
	REG_VMADD = (pVRAMDestination >> 1);
	src.ptr = pSource;
	
	// Always use channel 0
	REG_DMAP0 = 0x01;
	REG_BBAD0 = 0x18;
	REG_A1T0 = src.parts.low16;
	REG_A1B0 = src.parts.high8;
	REG_DAS0 = cSize;
	REG_MDMAEN = 0x01;
#else
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	REG_VMAIN = 0x80;
	REG_VMADD = (pVRAMDestination >> 1);
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	// Always use channel 0
	REG_DMAP0 = 0x01;
	REG_BBAD0 = 0x18;
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;
	REG_DAS0 = cSize;
	REG_MDMAEN = 0x01;
#endif
}	

//=============================================
// LoadLOVRAM - Load GFX Data To VRAM Lo Bytes
//=============================================
//  SRCTILES: 24-Bit Address Of Source Tile Data
//      DEST: 16-Bit VRAM Destination (WORD Address)
// SIZETILES: Size Of Tile Data (BYTE Size)
//      CHAN: DMA Channel To Transfer Data (0..7)
FUNCTIONATR void LoadLoVram(const unsigned char *pSource, uint16_t pVRAMDestination,
			 uint16_t cSize){
#ifdef __TCC816__
	PtrSplit src;
	REG_VMAIN = 0x00;
	REG_VMADD = (pVRAMDestination >> 1);
	src.ptr = pSource;
	
	// Always use channel 0
	REG_DMAP0 = 0x00;
	REG_BBAD0 = 0x18;
	REG_A1T0 = src.parts.low16;
	REG_A1B0 = src.parts.high8;
	REG_DAS0 = cSize;
	REG_MDMAEN = 0x01;
#else
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	REG_VMAIN = 0x00;
	REG_VMADD = (pVRAMDestination >> 1);	
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	// Always use channel 0
	REG_DMAP0 = 0x00;
	REG_BBAD0 = 0x18;
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;
	REG_DAS0 = cSize;
	REG_MDMAEN = 0x01;
#endif
}

FUNCTIONATR void LoadHiVram(const unsigned char *pSource, uint16_t pVRAMDestination,
			 uint16_t cSize){
#ifdef __TCC816__
	PtrSplit src;
	REG_VMAIN = 0x80;
	REG_VMADD = (pVRAMDestination >> 1);
	src.ptr = pSource;
	
	// Always use channel 0
	REG_DMAP0 = 0x00;
	REG_BBAD0 = 0x19;
	REG_A1T0 = src.parts.low16;
	REG_A1B0 = src.parts.high8;
	REG_DAS0L = (uint8_t)(cSize & 0xFF);
	REG_DAS0H = (uint8_t)((cSize >> 8) & 0xFF);
	REG_MDMAEN = 0x01;
#else
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	REG_VMAIN = 0x80;
	REG_VMADD = (pVRAMDestination >> 1);
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	// Always use channel 0
	REG_DMAP0 = 0x00;
	REG_BBAD0 = 0x19;
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;
	REG_DAS0L = (uint8_t)(cSize & 0xFF);
	REG_DAS0H = (uint8_t)((cSize >> 8) & 0xFF);
	REG_MDMAEN = 0x01;
#endif
}



//===================================
// ClearVRAM - Clear VRAM Fixed Word 
//===================================
//  SRC: 24-Bit Address Of Source Data
// DEST: 16-Bit VRAM Destination (WORD Address)
// SIZE: Size Of Data (BYTE Size)
// CHAN: DMA Channel To Transfer Data (0..7)
FUNCTIONATR void ClearVram(const unsigned char *pSource, uint16_t pVRAMDestination,
			 uint16_t cSize){
#ifdef __TCC816__
	PtrSplit src;
	uint16_t regWrite3;

	regWrite3 = (pVRAMDestination >> 1);
	//Transfer LoByte
	REG_VMAIN = 0x00;
	REG_VMADD = regWrite3;
	src.ptr = pSource;
	
	// Always use channel 0
	REG_DMAP0 = 0x08;
	REG_BBAD0 = 0x18;
	REG_A1T0 = src.parts.low16;
	REG_A1B0 = src.parts.high8;
	REG_DAS0 = cSize;
	REG_MDMAEN = 0x01;
	
	//Transfer HiByte
	REG_VMAIN = 0x80;
	REG_VMADD = regWrite3;
	src.ptr = pSource + 1;
	
	REG_BBAD0 = 0x19;
	REG_A1T0 = src.parts.low16;
	REG_A1B0 = src.parts.high8;
	REG_DAS0 = cSize;
	REG_MDMAEN = 0x01;
#else
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	uint16_t regWrite3;

	regWrite3 = (pVRAMDestination >> 1);
	//Transfer LoByte
	REG_VMAIN = 0x00;
	REG_VMADD = regWrite3;
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	// Always use channel 0
	REG_DMAP0 = 0x08;
	REG_BBAD0 = 0x18;
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;
	REG_DAS0 = cSize;
	REG_MDMAEN = 0x01;
	
	//Transfer HiByte
	REG_VMAIN = 0x80;
	REG_VMADD = regWrite3;
	regWrite1 = (uint16_t) ((uint32_t)pSource+1);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	REG_BBAD0 = 0x19;
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;
	REG_DAS0 = cSize;
	REG_MDMAEN = 0x01;
#endif
}

//===================================
// LoadOAMCopy - Clear VRAM Fixed Word 
//===================================
//  SRC: 24-Bit Address Of Source Data
// DEST: 16-Bit OAM Destination (WORD Address)
// SIZE: Size Of Data (BYTE Size)
// CHAN: DMA Channel To Transfer Data (0..7)
FUNCTIONATR void LoadOAMCopy(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize) {
#ifdef __TCC816__
	PtrSplit src;
	src.ptr = pSource;
	
	// Always use channel 0
	REG_OAMADD = pVRAMDestination;
	REG_DAS0 = cSize;
	REG_DMAP0 = 0x02;
	REG_BBAD0 = 0x04; //0x2104
	REG_A1T0 = src.parts.low16;
	REG_A1B0 = src.parts.high8;
	REG_MDMAEN = 0x01;
#else
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	// Always use channel 0
	REG_OAMADD = pVRAMDestination;
	REG_DAS0 = cSize;
	REG_DMAP0 = 0x02;
	REG_BBAD0 = 0x04; //0x2104
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;
	REG_MDMAEN = 0x01;
#endif
}


void initOAMCopy(unsigned char *pSource){
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
}

// Cross-compiler interrupt handler implementations
// Users MUST define snesXC_irq() and snesXC_nmi() in their code
// If they don't, the linker will fail with undefined symbols
// This ensures users are explicit about their interrupt handling

// Compiler-specific wrapper functions for assembly vectors
// All interrupts (emulation and native mode) use the same user functions
#ifdef __WDC816CC__
void far snesXC_cop_wrapper(void) {
	snesXC_cop();
}

void far snesXC_brk_wrapper(void) {
	snesXC_brk();
}

void far snesXC_abort_wrapper(void) {
	snesXC_abort();
}

void far snesXC_nmi_wrapper(void) {
	snesXC_nmi();
}

void far snesXC_irq_wrapper(void) {
	snesXC_irq();
}
#else
void snesXC_cop_wrapper(void) {
	snesXC_cop();
}

void snesXC_brk_wrapper(void) {
	snesXC_brk();
}

void snesXC_abort_wrapper(void) {
	snesXC_abort();
}

void snesXC_nmi_wrapper(void) {
	snesXC_nmi();
}

void snesXC_irq_wrapper(void) {
	snesXC_irq();
}
#endif

// VBCC-specific interrupt handlers
#ifdef __VBCC__
__near __interrupt void __irq_cop(void) {
	snesXC_cop();
}

__near __interrupt void __irq_brk(void) {
	snesXC_brk();
}

__near __interrupt void __irq_vblank(void) {
	snesXC_nmi();
}

__near __interrupt void __irq_ext(void) {
	snesXC_irq();
}

__near __interrupt void __irq_cop6502(void) {
	snesXC_cop();
}

__near __interrupt void __irq_nmi6502(void) {
	snesXC_nmi();
}

__near __interrupt void __irq_ext6502(void) {
	snesXC_irq();
}
#endif

// Calypsi-specific interrupt handlers
#ifdef __CALYPSI__
__attribute__((section("CODE_IN_BANK0"), interrupt(0xFFE4)))
void snesXC_cop_handler(void) {
	snesXC_cop();
}

__attribute__((section("CODE_IN_BANK0"), interrupt(0xFFE6)))
void snesXC_brk_handler(void) {
	snesXC_brk();
}

__attribute__((section("CODE_IN_BANK0"), interrupt(0xFFE8)))
void snesXC_abort_handler(void) {
	snesXC_abort();
}

__attribute__((section("CODE_IN_BANK0"), interrupt(0xFFEA)))
void snesXC_nmi_handler(void) {
	snesXC_nmi();
}

__attribute__((section("CODE_IN_BANK0"), interrupt(0xFFEE)))
void snesXC_irq_handler(void) {
	snesXC_irq();
}
#endif