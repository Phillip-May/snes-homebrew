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

extern uint32_t SNESFONT_bin;
extern uint32_t END_SNESFONT_bin;

void main(void){
	//Variables
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers
	uint32_t regTest;
	
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);	
	termM0Init();
	
	REG_INIDISP = 0x0F;
	
	regTest = (uint32_t) SNESFONT_bin;
	
	do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
		} while( (regRead1 > 0));
	termM0PrintStringXY("Waiting for SPC700 to boot....",0,1);
	SPCWaitBoot();
	termM0PrintStringXY("SPC700 booted",0,2);
	
	
	for (;;) {
		do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
		} while( (regRead1 > 0));
	}
	
}

void far IRQHandler(void){
	
}

int termM0Init(void){	
	const unsigned char BGPAL[] = {0x00,0x00,0xFF,0x7F};
	const unsigned char BGCLEAR[] = {0x20, 0x00};
	LoadCGRam(BGPAL, 0x00, 4, 0); // Load BG Palette Data
	LoadLoVram((char *)SNESFONT_bin, 0x0000,((uint16_t)SNESFONT_bin)-((uint16_t)END_SNESFONT_bin), 7);
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
int termM0PrintStringXY(char *szInput, int inpX, int inpY){
	int8_t regRead1;
	do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
	} while( (regRead1 > 0));
	LoadLoVram(szInput, 0xF800+inpX+inpY*0x40, strlen(szInput), 0); // Load Text To VRAM Lo Bytes
	return 0;
}