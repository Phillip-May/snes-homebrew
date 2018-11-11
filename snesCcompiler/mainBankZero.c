#define MY_REGISTER (*(volatile uint16_t*)0x12345678u)
typedef	unsigned char	byte;
typedef	unsigned short	word;


//#include <INTTYPES.H>

#include "lib/snes.h"

void main(void){
	
	//*(byte*)0x2122 = 0x55;
	//*(byte*)0x2122 = 0xBB;
	REG_CGDATA = 0x1F;
	//REG_CGDATAbits.TRISB0 = 1;
	REG_CGDATA = 0x00;
	*(byte*)0x2121 = 0x00;
	*(byte*)0x2100 = 0x0F;
	*(byte*) 0x2100 = 0x0f; // Plane 0 (bit one) enable register

	
	while(1){

	}
}

void far IRQHandler(void){
	
}

#define PortA (*(unsigned char *)0xfff002) 