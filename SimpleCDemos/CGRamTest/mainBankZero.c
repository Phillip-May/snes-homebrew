//LoROM mememory map
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "snes_regs_xc.h"

char testString[40] = "Initial string in heap";

void main(void){
	//Example
	int size = 4; /*one extra for '\0'*/
	uint8_t *test_heap;
	//Initialization
	//Initialize the stack
	size = 0x12;
	test_heap = (uint8_t*)farmalloc((uint32_t)1024);
	size = 0x34;
    test_heap[6] = 'A';
    test_heap[7] = 'A';
    test_heap[8] = 'A';	
    test_heap[9] = 'A';	
	test_heap = (uint8_t*)farmalloc((uint32_t)1024);
	size = 0x34;
    test_heap[6] = 'B';
    test_heap[7] = 'A';
    test_heap[8] = 'A';	
    test_heap[9] = 'B';	
	
	REG_CGDATA = 0x00;
	REG_CGDATA = 0x1F;
	REG_CGADD = 0x00;
	//3 ways of writting to a register
	sprintf(testString,"D compiler text");
	testString[0] = 'C';
	
	//Method 1 - Direct register access
	REG_INIDISP = 0x0F; // Set brightness to 15, screen enabled
	//Method 2 - Direct memory access
	(*(uint8_t*)0x002100) = 0x0F;
	//Method 3 - Using register macro
	REG_INIDISP = 0x0F;
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
