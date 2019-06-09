#pragma section CONST=BANK0
#pragma section CODE=BANK0

typedef	unsigned char byte;
//LoROM mememory map
#include <FLOAT.H>
#include <INTTYPES.H>
#include <STDINT.H>
#include <MALLOC.H>
#include <STRING.H>
#include <STDIO.h>

#include "include\mainBankZero.h"
#include "include\SNES.h"
#include "include\initsnes.h"
#include "include\imagedata.h"


const char testString[40] = "Initial string in ROM";
const char testString2[40] = "\nInitial string in ROM 2";
//char testStringRam[40] = "Initial string in User Data";
char testStringRam[40] = "Error";
char testStringRam2[40] = "Error2";
char testStringRam3[40] = "Error3";

void main(void){
	//Variables
	static int something = 5;
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers
	uint8_t lastInputLo;
	uint8_t lastInputHi;
	uint8_t xOffset = 10;
	
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);
	
	termM0Init();
	//termM0PrintStringXY("Test",0,0);
	REG_INIDISP = 0x0F;
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
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY) {
	LoadLoVram(szInput, 0xF944, strlen(szInput), 0); // Load Text To VRAM Lo Bytes
	return 0;
}	 



void far IRQHandler(void){
	
}


void far CrashHandler(void){
	
}












