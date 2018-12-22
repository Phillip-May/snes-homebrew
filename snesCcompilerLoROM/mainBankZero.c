#define MY_REGISTER (*(volatile uint32_t*)0x7E2004u)
typedef	unsigned char byte;
//LoROM mememory map

#include <FLOAT.H>
#include <INTTYPES.H>
#include <STDINT.H>
#include <STDIO.H>
#include <STRING.H>
#include <MALLOC.H>

#include "include\mainBankZero.h"
#include "include\SNES.h"
#include "include\initsnes.h"
#include "include\imagedata.h"

//const char testString[40] = "Initial string in ROM";
//char testStringRam[40] = "Initial string in heap";
void main(void){
	//Variables
	static int something = 5;
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers
	uint8_t *test_heap;
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);
	//test_heap = (uint8_t*)farmalloc((uint32_t)1024);
	//strcpy(testStringRam,testString);
	//testStringRam[0] = 'C';
	something = something - 1;
	
	LoadVram(school_bin, 0x0000, (uint16_t) school_bin_size, 7);
	LoadCGRam(school_pal, 0x10, sizeof(school_pal), something);
	REG_CGADD = 0x00;
	REG_CGDATA = 0xca;
	REG_CGDATA = 0x7e;
	
	//Simple recreation of input test
	//ButtonTest();
	while(1){

	}
}

void far IRQHandler(void){
	
}

int termM0Init(void){
	
	return 0;
}
//Takes an input and maps prints to an xy on the tile
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY){
	
	return 0;
}	 

//A little program that demonstrates reading inputs and waiting for VBlank
void ButtonTest(void){
	//Variables
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers	
	
	REG_NMITIMEN = 0x01;
	REG_TM  = 0x00;	//Set main screen
	REG_INIDISP = 0x0F; //Turn screen on
	
	//Simple code to verify the program can do a thing
	REG_CGADD = 0x00;
	REG_CGDATA = 0x00;
	REG_CGDATA = 0x78;	
	
	//Simple recreation of input test
	while(1){
		do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
		} while( (regRead1 > 0));
		do{ //Wait for H/V blank flag and joypad busy flag
			regRead1 = REG_RDNMI;
		} while( (regRead1 > 0));
		
		regRead1 = REG_JOY1L;
		regRead2 = REG_JOY1H;
		if ( (REG_JOY1L != 0) || (REG_JOY1H != 0)){
			REG_CGADD = 0x00;
			REG_CGDATA = 0x1F;
			REG_CGDATA = 0x00;				
		}
		else{
			REG_CGADD = 0x00;
			REG_CGDATA = 0x00;
			REG_CGDATA = 0x78;				
		}
		
	}	
}
