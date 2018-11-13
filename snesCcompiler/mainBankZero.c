#define MY_REGISTER (*(volatile uint16_t*)0x12345678u)

#include <FLOAT.H>
#include <INTTYPES.H>
#include <STDINT.H>
#include <STDIO.H>
#include <STRING.H>

#include "lib/snes.h"

void far main(void){
	volatile int variable;
	char testString[20];
	INIDISPBITS temp;
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
	VMADD = 0x1234;
	
	
	while(1){
	}
}

void far IRQHandler(void){
	
}
