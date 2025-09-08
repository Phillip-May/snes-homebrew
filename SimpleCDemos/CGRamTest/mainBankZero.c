//LoROM mememory map
#include <stdint.h>
#include "snes_regs_xc.h"

void main(void) {
	REG_CGDATA = 0x00;
	REG_CGDATA = 0x1F;
	REG_CGADD = 0x00;
	//Method 1 - Direct register access
	REG_INIDISP = 0x0F; // Set brightness to 15, screen enabled
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

void snesXC_irq(void) {
}
