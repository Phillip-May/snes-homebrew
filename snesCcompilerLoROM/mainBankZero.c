#define MY_REGISTER (*(volatile uint32_t*)0x7E2004u)
typedef	unsigned char byte;
//LoROM mememory map

#include <FLOAT.H>
#include <INTTYPES.H>
#include <STDINT.H>
#include <STDIO.H>
#include <STRING.H>
#include <MALLOC.H>

#include "include\SNES.h"
#include "include\initsnes.h"

char testString[40] = "Initial string in heap";
void main(void){
	//Variables
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);

	//Simple code to verify the program can do a thing
	REG_CGADD = 0x00;
	REG_CGDATA = 0x00;
	REG_CGDATA = 0x78;	
	
	REG_INIDISP = 0x0F;
	while(1){
	}
}

void far IRQHandler(void){
	
}
