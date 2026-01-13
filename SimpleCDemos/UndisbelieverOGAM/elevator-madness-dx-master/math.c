#include "math.h"

uint16_t Math_Divide_U16Y_U8A(uint16_t dividend, uint8_t divisor, uint16_t *remainder) {
    // Use SNES hardware division
    REG_WRDIVA = dividend;
    REG_WRDIVB = divisor;
    
    // Wait for hardware division to complete (16 cycles)
    // Use a small delay - compiler will optimize this
    volatile uint8_t delay = 0;
    (void)delay; // Prevent optimization
    
    // Read results
    uint16_t quotient = REG_RDDIV;
    if (remainder) {
        *remainder = REG_RDMPY;
    }
    
    return quotient;
}

uint16_t Math_Multiply_U8Y_U8X_UY(uint8_t a, uint8_t b) {
    // Use SNES hardware multiplication
    REG_WRMPYA = a;
    REG_WRMPYB = b;
    
    // Wait for hardware multiplication to complete (8 cycles)
    // Use a small delay - compiler will optimize this
    volatile uint8_t delay = 0;
    (void)delay; // Prevent optimization
    
    // Read 16-bit product
    return REG_RDMPY;
}
