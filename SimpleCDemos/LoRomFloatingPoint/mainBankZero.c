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
//char testStringRam[40] = "Initial string in User Data";
char testStringRam[40] = "Error";
void main(void){
	//Variables
	static int something = 5;
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers
	uint8_t *test_heap;
	uint32_t counter = 40000;
	float floatingCounter = 0.0;
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);
	
	//LoadVram(school_bin, 0x0000, (uint16_t) school_bin_size, 7);
	//LoadCGRam(school_pal, 0x10, sizeof(school_pal), something);
	
	//Simple recreation of input test
	//ButtonTest();
	//Mode0Text();
	termM0Init();
	termM0PrintStringXY("Test",0,0);
	REG_INIDISP = 0x0F;
	while(1){
		do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
		} while( (regRead1 > 0));
		termM0PrintStringXY(testStringRam,0,0);
		sprintf(testStringRam,"Value: %03f\0",floatingCounter);
		//counter++;
		floatingCounter = floatingCounter + 1.1;
	}
}

void far IRQHandler(void){
	
}

int termM0Init(void){	
	const unsigned char BGPAL[] = {0x00,0x00,0xFF,0x7F};
	const unsigned char BGCLEAR[] = {0x20, 0x00};
	LoadCGRam(BGPAL, 0x00, 4, 0); // Load BG Palette Data
	LoadLoVram(SNESFONT_bin, 0x0000, sizeof(SNESFONT_bin), 7);
    ClearVram(BGCLEAR, 0xF800, 0x400, 0); // Clear VRAM Map To Fixed Tile Word
	
	REG_BGMODE  = 0x08;
	REG_BG1SC  = 0xFC;
	REG_BG12NBA = 0x00;
	REG_TM = 0x01;
	
	REG_BG1HOFS = 0x00;
	REG_BG1HOFS = 0x00;
	REG_BG1VOFS = 0x00;
	REG_BG1VOFS = 0x00;	
	return 0;
}
//Takes an input and maps prints to an xy on the tile
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY){
	LoadLoVram(szInput, 0xF944, strlen(szInput), 0); // Load Text To VRAM Lo Bytes
	return 0;
}	 

//A little program that demonstrates reading inputs and waiting for VBlank
//Requires snes to be initialised
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

//A little program that demonstrates text display using mode 0
//Requires snes to be initialised
void Mode0Text(){
	int8_t regRead1;
	const unsigned char BGPAL[] = {0x00,0x00,0xFF,0x7F};
	const unsigned char BGCLEAR[] = {0x20, 0x00};
	const unsigned char HELLOWORLD[] = "Hello World!";

	
	REG_INIDISP = 0x0F;
	do{ //Wait for Vblank
		regRead1 = REG_RDNMI;
	} while( (regRead1 > 0));	
	LoadCGRam(BGPAL, 0x00, 4, 0); // Load BG Palette Data
	LoadLoVram(SNESFONT_bin, 0x0000, sizeof(SNESFONT_bin), 7);
    ClearVram(BGCLEAR, 0xF800, 0x400, 0); // Clear VRAM Map To Fixed Tile Word
	
	LoadLoVram(HELLOWORLD, 0xF944, sizeof(HELLOWORLD)-1, 0); // Load Text To VRAM Lo Bytes
	
	REG_BGMODE  = 0x08;
	REG_BG1SC  = 0xFC;
	REG_BG12NBA = 0x00;
	REG_TM = 0x01;
	
	REG_BG1HOFS = 0x00;
	REG_BG1HOFS = 0x00;
	REG_BG1VOFS = 0x00;
	REG_BG1VOFS = 0x00;
	
	while(1){
		//REG_INIDISP = 0x0F;
	}
}

