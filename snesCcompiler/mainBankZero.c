#define MY_REGISTER (*(volatile uint16_t*)0x12345678u)
typedef	unsigned char	byte;
typedef	unsigned short	word;


#include <INTTYPES.H>
#include <STDINT.H>
#include <STDIO.H>

#include "lib/snes.h"

void main(void){
	volatile int variable;
	//char testString[100];
	//sprintf(testString,"C compiler text");
	//testString[0] = 'D';
	CGDATA = 0x00;
	CGDATA = 0x1F;
	CGADD = 0x00;
	INIDISP = 0x0F;

	
	while(1){
	}
}

void far IRQHandler(void){
	
}
