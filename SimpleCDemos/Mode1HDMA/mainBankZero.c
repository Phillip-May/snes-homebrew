#define MY_REGISTER (*(volatile uint32_t*)0x7FFF00u)
typedef	unsigned char byte;
//LoROM mememory map


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mainBankZero.h"
#include "snes_regs_xc.h"
#include "initsnes.h"
#include "BG18_mode1.h" // Now contains Mode 1 data

// 65816 Assembly Opcode Macros
#define OP_LDA_ABS      0xAD  // LDA absolute
#define OP_LDA_LONG     0xAF  // LDA long (4-byte addressing)
#define OP_STA_ABS      0x8D  // STA absolute  
#define OP_STA_LONG     0x8F  // STA long (4-byte addressing)
#define OP_STZ_ABS      0x9C  // STZ absolute
#define OP_LDA_IMM      0xA9  // LDA immediate
#define OP_REP          0xC2  // REP (reset processor status bits)
#define OP_SEP          0xE2  // SEP (set processor status bits)
#define OP_RTI          0x40  // RTI (return from interrupt)
#define OP_NOP          0xEA  // NOP (no operation)
#define OP_TXA          0x8A  // TXA (transfer X to A)

// Macros for writing to the assembly buffer
static unsigned char* irq_asm_ptr;
static int irq_asm_index;

#define WRITE_BYTE(value) do { \
    if (irq_asm_index < 32) { \
        irq_asm_ptr[irq_asm_index++] = (value); \
    } \
} while(0)
#define WRITE_WORD(value) do { \
    WRITE_BYTE((value) & 0xFF); \
    WRITE_BYTE(((value) >> 8) & 0xFF); \
} while(0)
#define WRITE_ADDR(addr) do { \
    WRITE_BYTE((addr) & 0xFF); \
    WRITE_BYTE(((addr) >> 8) & 0xFF); \
} while(0)
#define WRITE_LONG_ADDR(addr) do { \
    WRITE_BYTE((addr) & 0xFF); \
    WRITE_BYTE(((addr) >> 8) & 0xFF); \
    WRITE_BYTE(((addr) >> 16) & 0xFF); \
} while(0)



// Function to write the custom IRQ handler assembly code to the IRQ buffer for per-scanline palettes
void write_irq_assembly_buffer(void) {
	irq_asm_ptr = snesXC_getIRQ_ASM_Buffer();
	irq_asm_index = 0;
	/*Reference c code for per-scanline palettes:
	dummyRead = REG_TIMEUP;  // Acknowledge the IRQ by reading TIMEUP register
	REG_CGADD = 0;           // Set palette destination to start
	
	// Get current scanline palette address
	uint16_t palette_addr = get_scanline_palette_addr(current_scanline);
	REG_A1T0 = palette_addr;
	REG_DAS0 = 32;           // 16 colors * 2 bytes each = 32 bytes
	REG_MDMAEN = 0x01;       // Start DMA
	
	current_scanline++;
	if (current_scanline >= SCANLINE_COUNT) current_scanline = 0;
	*/
	
	// Assembly equivalent for per-scanline palette IRQ handler:
	// Due to 32-byte limit, we use a simplified approach with pre-calculated addresses
	
	// lda $4211       ; Read TIMEUP register to acknowledge IRQ
	WRITE_BYTE(OP_LDA_ABS);
	WRITE_ADDR(0x4211);
	
	// rep #$20        ; Set 16-bit A register
	WRITE_BYTE(OP_REP);
	WRITE_BYTE(0x20);
	
	// Load the calculated palette address for current scanline
	// Setup once per frame, and then all the palletes are aranged in a row
	
	// sta $4302       ; Store to A1T0 (DMA source address)
	//WRITE_BYTE(OP_STA_ABS);
	//WRITE_ADDR(0x4302);
	
	// lda #32         ; Load 32 for DMA size (16 colors * 2 bytes)
	WRITE_BYTE(OP_LDA_IMM);
	WRITE_WORD(32);
	
	// sta $4305       ; Store to DAS0 (DMA size)
	WRITE_BYTE(OP_STA_ABS);
	WRITE_ADDR(0x4305);
	
	// sep #$20        ; Set 8-bit A register
	WRITE_BYTE(OP_SEP);
	WRITE_BYTE(0x20);

	// lda #$90        ; Load 0x90 (NMI + H-IRQ enable)
	WRITE_BYTE(OP_LDA_IMM);
	WRITE_BYTE(0x90);
	
	// sta $4200       ; Store to NMITIMEN (re-enable NMI and H-IRQ for chain)
	WRITE_BYTE(OP_STA_ABS);
	WRITE_ADDR(0x4200);

	// lda #$01        ; Load 1 (prepare DMA enable value)
	WRITE_BYTE(OP_LDA_IMM);
	WRITE_BYTE(0x01);
	
	// stz $2121       ; Store 0 to CGADD (set palette destination)
	WRITE_BYTE(OP_STZ_ABS);
	WRITE_ADDR(0x2121);
		
	// sta $420B       ; Store to MDMAEN (start DMA)
	WRITE_BYTE(OP_STA_ABS);
	WRITE_ADDR(0x420B);
		
	// rti             ; Return from interrupt
	WRITE_BYTE(OP_RTI);
}

void main(void){
	//Variables
	int8_t regRead1; //Variable for storing hardware registers
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);
	
	
	// Set BG mode first (Mode 1: 4bpp BG1, 4bpp BG2)
	REG_BGMODE = 0x01; // Mode 1: 4bpp BG1, 4bpp BG2
	
	// Configure screen settings
	REG_SETINI = 0x00; // Mode 1 doesn't need high-res mode
	REG_M7SEL = 0x00;  // Not using Mode 7
	REG_MOSAIC = 0x00; // No mosaic effect
	
	// Enable BG1 on main screen and subscreen
	REG_TM = 0x01; // Enable BG1 on main screen
	REG_TS = 0x01; // Enable BG1 on subscreen (should match TM)
	
	// Configure color math and window settings - disable direct color
	REG_CGWSEL = 0x00; // Bit 0 = 0: Use palette for 4bpp BGs (disable direct color)
	REG_CGADSUB = 0x00; // Disable color math operations
	REG_COLDATA = 0x20; // Set fixed color to black (R=0, G=0, B=0)

	// Load initial palette to CGRAM (16 colors for Mode 1 BG1)
	// Use first scanline palette for initialization
	LoadCGRam((unsigned char *)&scanline_palettes[0][0], 0x0000, sizeof(scanline_palettes[0]));

	// Load tiles to VRAM (starting at 0x0000)
	// Set VRAM increment mode for 4bpp bitplane data (increment by 2 for 128 times)
	REG_VMAIN = 0x80; // Auto-increment, mode 80 (increment by 2 for 128 times for 4bpp bitplanes)
	LoadVram((unsigned char *)bg_tiles, 0x0000, sizeof(bg_tiles));

	// Load tilemap to VRAM (further down in VRAM for Mode 1)
	// Set VRAM increment mode for tilemap data (increment by 1 for 16-bit entries)
	REG_VMAIN = 0x00; // Auto-increment by 1 for tilemap data
	LoadVram((unsigned char *)&bg_tilemap, 0xE000, sizeof(bg_tilemap)); // Byte address 0xE000

	// Configure BG1 registers for Mode 1
	REG_BG1SC = 0x72; // Screen base address word 0x7000 (byte 0xE000), 32x32 tilemap
	REG_BG12NBA = 0x00; // BG1 tile base address 0x0000
		
	// Set H-timer to trigger at H-Blank (around H=274)
	// H-Blank begins at H=274, so we set HTIME to trigger slightly before
	// Note: H-timer values 0-340, V-timer values 0-261 (NTSC) or 0-311 (PAL)
	// Setup for IRQ to trigger once per scanline using H-IRQ
	// First disable all interrupts and HDMA
	REG_NMITIMEN = 0x00; // Disable NMI, H-IRQ, V-IRQ, and joypad auto-read
	REG_HDMAEN = 0x00;   // Disable HDMA
	
	// Set H-timer for H-IRQ to trigger once per scanline at a consistent point
	// Use a value well within the visible portion to avoid double triggers
	// SNES timing: 0-255 visible, 256-340 H-Blank, so use early visible area
	//Fine tuned a bit to account for overhead
	REG_HTIME = 256-58;   // Set to early in scanline to avoid retriggering issues  
	REG_VTIME = 0;        // V-timer, to synchronize with VBlank

	//Clear flags to ensure clean state
	REG_RDNMI = 0x00;    // Clear NMI flag
	REG_TIMEUP = 0x00;   // Clear IRQ flag  
	REG_HVBJOY = 0x00;   // Clear H/V-Blank and joypad flags

	//Setup DMA channel 0 for per-scanline palette system	
	LoadCGRam((unsigned char *)&scanline_palettes[0][0], 0x0000, sizeof(scanline_palettes[0]));
	
	//Write the irq handler to the irq extension buffer
	write_irq_assembly_buffer();
	
	// Enable both NMI (VBlank) and H-IRQ for per-scanline palette updates
	// NMI will reset scanline counter, H-IRQ will update palettes per scanline
	
	// Enable display with full brightness
	emitCLI();
	REG_NMITIMEN = 0x30 | 0x80; // Enable NMI (bit 7) and H-IRQ (bit 4+5)	
	REG_INIDISP = 0x0F; // Enable display, full brightness
	while(1){
		// Main loop
	}
}

// snesXC_irq removed - using assembly buffer for per-scanline palette updates

// Cross-compiler interrupt handler implementations
void snesXC_cop(void) {
    // COP (Coprocessor) interrupt handler
}

void snesXC_brk(void) {
    // BRK (Break) interrupt handler
}

void snesXC_abort(void) {
    // ABORT interrupt handler
}
volatile uint8_t dummyRead = 0;
void snesXC_nmi(void) {
    // NMI (Non-Maskable Interrupt) handler - VBlank only
    // Prepare IRQ assembly buffer for the first scanline of the next frame
	REG_NMITIMEN = 0x30 | 0x80; // Enable NMI (bit 7) and H-IRQ (bit 4+5)	
	LoadCGRam((unsigned char *)&scanline_palettes[0][0], 0x0000, sizeof(scanline_palettes[0]));
	write_irq_assembly_buffer();
	dummyRead = REG_TIMEUP;  // Acknowledge the IRQ by reading TIMEUP register
}




