//LoROM memory map
// Test project for banking system
#include <stdint.h>
#include "snes_regs_xc.h"

// Test configuration defines
#define TEST_ARRAY_SIZE_00 0xC00
#define TEST_ARRAY_SIZE_7E 0xC00
#define TEST_PATTERN_BASE_00 0x00
#define TEST_PATTERN_BASE_7E 0x80

// Use the new banking system's section attribute macros
// BANK_7E_BSS is already defined in snes_regs_xc.h for backward compatibility
// but we can also use the new macros directly if needed

volatile uint8_t testValue = 0x00;

volatile uint8_t testArrayFixedRAM[TEST_ARRAY_SIZE_00] = {0x00};
BANK7E_DATA volatile uint8_t testArray_bank7E[TEST_ARRAY_SIZE_7E] = {0x00};

// Test const data in different ROM data banks
// Bank 0 const data (default, can be explicit with BANK0_CONST)
volatile const uint8_t testDataBank0[16] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

// Bank 1 const data
BANK1_CONST volatile const uint8_t testDataBank1[30000] = {
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

// Bank 2 const data
BANK2_CONST volatile const uint8_t testDataBank2[30000] = {
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F
};

// Bank 3 const data
BANK3_CONST volatile const uint8_t testDataBank3[30000] = {
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
};

// Test functions in different code banks
BANK1_FUNC void test_function_bank1(void) {
	// Function in code bank 1
	// Do something simple to verify it's being called
	testValue = 0x01;  // Set test value to indicate bank 1 was called
}

BANK2_FUNC void test_function_bank2(void) {
	// Function in code bank 2
	testValue = 0x02;  // Set test value to indicate bank 2 was called
}

BANK3_FUNC void test_function_bank3(void) {
	// Function in code bank 3
	testValue = 0x03;  // Set test value to indicate bank 3 was called
}

void main(void) {
	// Initialize 65816 to native mode (now from bankswitch.s)
	asm_init65816();
	
	// ========== ROM BANKING TESTS ==========
	
	// ========== CODE BANK SWITCHING TESTS ==========
	// Test code bank switching and calling functions in different banks
	// Note: Actual PBR switching happens when calling functions in other banks
	
	// Test calling function in bank 1
	SET_CODE_BANK_1();  // Switch to code bank 1 (sets DBR to $01)
	test_function_bank1();  // Call function in bank 1
	// Verify function was called (testValue should be 0x01)
	
	// Test calling function in bank 2
	SET_CODE_BANK_2();  // Switch to code bank 2 (sets DBR to $02)
	test_function_bank2();  // Call function in bank 2
	// Verify function was called (testValue should be 0x02)
	
	// Test calling function in bank 3
	SET_CODE_BANK_3();  // Switch to code bank 3 (sets DBR to $03)
	test_function_bank3();  // Call function in bank 3
	// Verify function was called (testValue should be 0x03)
	
	// Switch back to code bank 0
	SET_CODE_BANK_0();  // Switch back to code bank 0 (sets DBR to $00)

	// ========== ROM CONST DATA BANK SWITCHING TESTS ==========
	// Test ROM const data bank switching (for accessing const data in other banks)
	
	// Test accessing const data in bank 0
	SET_CONST_BANK_0();  // Switch to ROM const data bank 0
	testValue = 0;
	for (uint16_t i = 0; i < 16; i++) {
		if (testDataBank0[i] != (uint8_t)(i & 0xFF)) {
			testValue = 0xFF; // Error flag
			break;
		}
	}
	
	// Test accessing const data in bank 1
	SET_CONST_BANK_1();  // Switch to ROM const data bank 1
	for (uint16_t i = 0; i < 16; i++) {
		if (testDataBank1[i] != (uint8_t)((i + 0x10) & 0xFF)) {
			testValue = 0xFF; // Error flag
			break;
		}
	}
	
	// Test accessing const data in bank 2
	SET_CONST_BANK_2();  // Switch to ROM const data bank 2
	for (uint16_t i = 0; i < 16; i++) {
		if (testDataBank2[i] != (uint8_t)((i + 0x20) & 0xFF)) {
			testValue = 0xFF; // Error flag
			break;
		}
	}
	
	// Test accessing const data in bank 3
	SET_CONST_BANK_3();  // Switch to ROM const data bank 3
	for (uint16_t i = 0; i < 16; i++) {
		if (testDataBank3[i] != (uint8_t)((i + 0x30) & 0xFF)) {
			testValue = 0xFF; // Error flag
			break;
		}
	}
	
	SET_CONST_BANK_0();  // Switch back to ROM const data bank 0

	// Get current data bank (optional utility)
	uint8_t currentDataBank = asm_get_data_bank();
	
	// ========== RAM BANKING TESTS ==========
	// Test writing/reading to testArrayFixedRAM (bank $00)
	SET_DB_00();  // Switch to RAM bank $00
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
	SET_DB_7E();  // Switch to WRAM bank $7E
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

	// ========== SNES INITIALIZATION ==========
	SET_DB_00();  // Switch back to RAM bank $00 for register access
	REG_CGDATA = 0x00;
	REG_CGDATA = 0x1F;
	REG_CGADD = 0x00;
	REG_INIDISP = 0x0F; // Set brightness to 15, screen enabled
	SET_DB_7E();  // Switch to WRAM bank $7E
	
	while(1){
		// Main loop
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

