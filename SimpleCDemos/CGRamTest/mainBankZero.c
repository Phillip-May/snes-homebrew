//LoROM mememory map
#include <stdint.h>
#include "snes_regs_xc.h"
#include <stdint.h>

typedef struct CallInfo {
  void* base;  /* base for this function */
  void* func;  /* function index in the stack */
  void*	top;  /* top for this function */
  const int32_t *savedpc;
  int nresults;  /* expected number of results from this function */
  int tailcalls;  /* number of tail calls lost under this entry */
} CallInfo;




void main(void) {
	REG_CGDATA = 0x00;
	REG_CGDATA = 0x1F;
	REG_CGADD = 0x00;
	//Method 1 - Direct register access
	REG_INIDISP = 0x0F; // Set brightness to 15, screen enabled
	(*(volatile uint32_t*volatile)0x7FF000ul) = sizeof(CallInfo);
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
