
#ifndef __INT_SNES_XC_H
#define __INT_SNES_XC_H

#ifdef __TCC816__
// For TCC816, use stdint.h which it has
#include <stdint.h>
#else
// For other compilers, use inttypes.h
#include <inttypes.h>
#endif

typedef volatile int8_t *volatile vs8;
typedef volatile int16_t *volatile vs16;
typedef volatile int32_t *volatile vs32;
typedef volatile uint8_t *volatile vu8;
typedef volatile uint16_t *volatile vu16;
typedef volatile uint32_t *volatile vu32;

#endif // __INT_SNES_XC_H
