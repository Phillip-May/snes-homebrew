/*
 * Elevator Madness DX - Main Entry Point
 * Converted from assembly to C for snesXC system
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "snes_regs_xc.h"
#include "initsnes.h"

int main(void) {
    // Initialize SNES
    initSNES(SLOWROM);
    
    REG_NMITIMEN = 0x81; // VBlank enable (bit 7) + Auto-joypad (bit 0)
    
        
    // Main game loop
    for (;;) {

    }
    
    return 0;
}


// VBlank interrupt handler
void snesXC_nmi(void) {
}

// COP interrupt handler
void snesXC_cop(void) {
}

// BRK interrupt handler
void snesXC_brk(void) {
    // BRK handler
}

// ABORT interrupt handler
void snesXC_abort(void) {
    // ABORT handler
}
