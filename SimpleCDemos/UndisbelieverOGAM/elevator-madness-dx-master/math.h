#ifndef MATH_H
#define MATH_H

#include <stdint.h>
#include "snes_regs_xc.h"

// Divide 16-bit unsigned by 8-bit unsigned
// Returns: quotient in return value, remainder in *remainder
uint16_t Math_Divide_U16Y_U8A(uint16_t dividend, uint8_t divisor, uint16_t *remainder);

// Multiply 8-bit unsigned by 8-bit unsigned, returns 16-bit result
uint16_t Math_Multiply_U8Y_U8X_UY(uint8_t a, uint8_t b);

#endif // MATH_H
