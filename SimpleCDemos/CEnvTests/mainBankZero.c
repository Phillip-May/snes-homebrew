//LoROM mememory map
#include <stdint.h>
#include "snes_regs_xc.h"

// Test configuration defines
#define TEST_ARRAY_SIZE_00 0xC00
#define TEST_ARRAY_SIZE_7E 0xC00
#define TEST_PATTERN_BASE_00 0x00
#define TEST_PATTERN_BASE_7E 0x80

// Section attribute defines
#define BANK_7E_BSS __attribute__((section(".bank_7e_bss")))

// Declare assembly functions
extern void asm_init65816(void);
extern void asm_setDB00(void);
extern void asm_setDB7E(void);

volatile uint8_t testValue = 0x00;

volatile uint8_t testArrayFixedRAM[TEST_ARRAY_SIZE_00] = {0x00};
BANK_7E_BSS volatile uint8_t testArray_bank7E[TEST_ARRAY_SIZE_7E] = {0x00};

void main(void) {
	asm_init65816();	
	// Test writing/reading to testArrayFixedRAM (bank $00)
	asm_setDB00();
	for (uint16_t i = 0; i < TEST_ARRAY_SIZE_00; i++) {
		testArrayFixedRAM[i] = (uint8_t)((i + TEST_PATTERN_BASE_00) & 0xFF); // Write pattern: 0x00, 0x01, 0x02, ...
	}
	
	// Verify reads from testArrayFixedRAM
	testValue = 0;
	for (uint16_t i = 0; i < TEST_ARRAY_SIZE_00; i++) {
		if (testArrayFixedRAM[i] != (uint8_t)((i + TEST_PATTERN_BASE_00) & 0xFF)) {
			testValue = 0xFF; // Error flag
			break;
		}
	}
	
	// Test writing/reading to testArray_bank7E (bank $7E)
	asm_setDB7E();
	for (uint16_t i = 0; i < TEST_ARRAY_SIZE_7E; i++) {
		testArray_bank7E[i] = (uint8_t)((i + TEST_PATTERN_BASE_7E) & 0xFF); // Write pattern: 0x80, 0x81, 0x82, ...
	}
	
	// Verify reads from testArray_bank7E
	for (uint16_t i = 0; i < TEST_ARRAY_SIZE_7E; i++) {
		if (testArray_bank7E[i] != (uint8_t)((i + TEST_PATTERN_BASE_7E) & 0xFF)) {
			testValue = 0xFF; // Error flag
			break;
		}
	}

	asm_setDB00();
	REG_CGDATA = 0x00;
	REG_CGDATA = 0x1F;
	REG_CGADD = 0x00;
	//Method 1 - Direct register access
	REG_INIDISP = 0x0F; // Set brightness to 15, screen enabled
	asm_setDB7E();
	
	while(1){
	}
}

// Cross-compiler interrupt handlers, must be present
void snesXC_cop(void) {
}

void snesXC_brk(void) {
}

void snesXC_abort(void) {
}

void snesXC_nmi(void) {
}

