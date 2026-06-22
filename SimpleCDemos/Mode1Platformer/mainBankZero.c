//LoROM memory map
//
// Thin entry point only. The platformer itself lives in mapengine.c, which is
// compiled at reduced optimization for vbcc (its -O4 miscompiles the
// fixed-point/streaming math); keeping this file separate lets the rest of the
// build stay at -O4. See mapengine.c / mapengine.h and the Makefile.

#include <stdint.h>

#include "initsnes.h"
#include "mapengine.h"

void main(void){
	initSNES(SLOWROM);
	engineInit();
	while (1) engineFrame();
}

// Cross-compiler interrupt handler implementations
void snesXC_cop(void) {
}

void snesXC_brk(void) {
}

void snesXC_abort(void) {
}

void snesXC_nmi(void) {
}
