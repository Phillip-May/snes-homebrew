#define MY_REGISTER (*(volatile uint32_t*)0x7E2004u)
//LoROM mememory map

#include <FLOAT.H>
#include <INTTYPES.H>
#include <STDINT.H>
#include <STDIO.H>
#include <STRING.H>
#include <MALLOC.H>

#include "lib/snes.h"
char testString[40] = "Initial string in heap";
void main(void){
	//Example
	INIDISPBITS temp;
	int size = 4; /*one extra for ‘\0’*/
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
	
	CGDATA = 0x00;
	CGDATA = 0x1F;
	CGADD = 0x00;
	//3 ways of writting to a register
	sprintf(testString,"D compiler text");
	testString[0] = 'C';
	
	//Method of one
	temp.SCREENDISABLE = 0;
	temp.BRIGHTNESS = 0x0F;
	INIDISPbits = temp;
	//Method 2
	(*(uint8_t*)0x002100) = 0x0F;
	//Method 3
	INIDISP = 0x0F;
	while(1){
	}
}

void far IRQHandler(void){
	
}
