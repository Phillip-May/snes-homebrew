#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include "snes_regs_xc.h"
#include "controller.h"

// Random module state
extern uint32_t Random_seed;

// Add entropy from joypad state (call once per frame)
void Random_AddJoypadEntropy(void);

// Generate random byte (returns bits 16-23 of seed)
uint8_t Random_Rnd(void);

// Generate random number 0-3
uint8_t Random_Rnd_4(void);

// Generate random number 0-2 (skewed toward 1)
uint8_t Random_Rnd_3(void);

// Generate random number 0-1
uint8_t Random_Rnd_2(void);

// Generate random number 0 to (max-1)
uint8_t Random_Rnd_U8A(uint8_t max);

// Generate random number 0 to (max-1), 16-bit
uint16_t Random_Rnd_U16Y(uint16_t max);

// Generate random number between min and max-1
uint16_t Random_Rnd_U16X_U16Y(uint16_t min, uint16_t max);

#endif // RANDOM_H
