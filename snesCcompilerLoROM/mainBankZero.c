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

const char testString[40] = "Initial string in ROM";
char testStringRam[40] = "Initial string in heap";
void main(void){
	//Variables
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers
	uint8_t *test_heap;
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);
	test_heap = (uint8_t*)farmalloc((uint32_t)1024);
	strcpy(testStringRam,testString);
	testStringRam[0] = 'C';
	
	
	LoadVram(school_bin, 0x0000, (uint16_t) school_bin_size, 0);
	LoadCGRam(school_pal, 0x00, school_pal_size, 0);
	REG_CGADD = 0x00;
	REG_CGDATA = 0xca;
	REG_CGDATA = 0x7e;		
	
	//Simple recreation of input test
	ButtonTest();
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

//==================================
// LoadVRAM - Load GFX Data To VRAM
//==================================
//  SRC: 24-Bit Address Of Source Data
// DEST: 16-Bit VRAM Destination (WORD Address)
// SIZE: Size Of Data (BYTE Size)
// CHAN: DMA Channel To Transfer Data (0..7)
//Load Data into Vram, using DMA channel 0
int LoadVram(const unsigned char *pSource, uint16_t pVRAMDestination,
			 uint16_t cSize, uint8_t cChannel){
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	
	REG_VMAIN = 0x80;
	REG_VMADDL = (pVRAMDestination >>　1);
	REG_DMAP0 = 0x01;
	REG_BBAD0 = 0x18;
	regWrite1 = (uint16_t) ((long)pSource);
	regWrite2 = (uint8_t) (((long)pSource)>> 16);
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;
	REG_DAS0 = cSize;
	
	REG_MDMAEN = 0x01;
	
	return 0;
}

//======================================
// LoadPAL - Load Palette Data To CGRAM
//======================================
//  SRC: 24-Bit Address Of Source Data
// DEST: 8-Bit CGRAM Word Destination Address (Color # To Start On) * 2
// SIZE: Size Of Data (# Of Colours To Copy) in sets of two
// CHAN: DMA Channel To Transfer Data (0..7)
int LoadCGRam(const unsigned char *pSource, uint16_t pCGRAMDestination, uint16_t cSize,
			 uint8_t cChannel){
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	
	REG_CGADD = pCGRAMDestination;
	
	REG_DMAP0 = 0x00;
	REG_BBAD0 = 0x22;
	regWrite1 = (uint16_t) ((long)pSource);
	regWrite2 = (uint8_t) (((long)pSource)>> 16);
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;
	REG_DAS0 = cSize;
	
	REG_MDMAEN = 0x01;
	
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
