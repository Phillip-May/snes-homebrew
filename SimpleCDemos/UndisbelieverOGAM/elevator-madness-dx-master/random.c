#include "random.h"

// Random module state
uint32_t Random_seed = 0x1de589c0; // Initial seed from main.s

// Previous joypad state for entropy
static uint16_t prevJoypadState = 0;

void Random_AddJoypadEntropy(void) {
    uint16_t currentJoypad = Controller_current;
    
    // If joypad state changed, run Rnd twice for more entropy
    if (currentJoypad != prevJoypadState) {
        prevJoypadState = currentJoypad;
        Random_Rnd();
    }
    
    // Always run Rnd at least once
    Random_Rnd();
}

uint8_t Random_Rnd(void) {
    // LCG: seed = seed * 0x010101 + 0x31415927
    // This is equivalent to:
    // seed = seed + (seed << 8) + (seed << 16) + 0x31415927
    
    // Add lower bytes with carry propagation
    uint32_t temp = Random_seed;
    uint8_t carry = 0;
    
    // Add seed bytes with carry
    temp += (Random_seed << 8);
    carry = (temp < Random_seed) ? 1 : 0;
    
    temp += (Random_seed << 16);
    carry += (temp < (Random_seed << 8)) ? 1 : 0;
    
    // Add constant
    temp += 0x31415927UL;
    carry += (temp < 0x31415927UL) ? 1 : 0;
    
    Random_seed = temp;
    
    // Return bits 16-23 (byte 2 of seed)
    return (uint8_t)(Random_seed >> 16);
}

uint8_t Random_Rnd_4(void) {
    Random_Rnd();
    return (uint8_t)(Random_seed & 0x03);
}

uint8_t Random_Rnd_3(void) {
    Random_Rnd();
    uint8_t result = (uint8_t)(Random_seed & 0x03);
    
    if (result == 0) {
        // Get higher bits
        result = (uint8_t)((Random_seed >> 2) & 0x03);
        if (result == 0) {
            result = 2;
        }
    }
    
    return result - 1; // Returns 0-2
}

uint8_t Random_Rnd_2(void) {
    Random_Rnd();
    // Return bit 16 (MSB of byte 2)
    return (uint8_t)((Random_seed >> 16) & 0x01);
}

uint8_t Random_Rnd_U8A(uint8_t max) {
    if (max == 0) return 0;
    
    // Use SNES hardware division for efficiency
    REG_WRDIVL = (uint8_t)(Random_seed >> 16);
    REG_WRDIVH = (uint8_t)(Random_seed >> 24);
    REG_WRDIVB = max;
    
    // Run Rnd to update seed
    Random_Rnd();
    
    // Return remainder
    return REG_RDMPYL;
}

uint16_t Random_Rnd_U16Y(uint16_t max) {
    if (max == 0) return 0;
    
    // Use standard C division
    Random_Rnd();
    uint32_t dividend = (Random_seed >> 16) | ((Random_seed >> 24) << 16);
    return (uint16_t)(dividend % max);
}

uint16_t Random_Rnd_U16X_U16Y(uint16_t min, uint16_t max) {
    if (max <= min) return min;
    
    uint16_t range = max - min;
    Random_Rnd();
    uint32_t dividend = (Random_seed >> 16) | ((Random_seed >> 24) << 16);
    uint16_t result = (uint16_t)(dividend % range);
    return result + min;
}
