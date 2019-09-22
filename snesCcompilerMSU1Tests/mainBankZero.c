#pragma section CODE=BANK1
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

extern far unsigned char SNESFONT_bin[];
extern far unsigned char END_SNESFONT_bin[];

extern far unsigned char SPC700Code[];
extern far unsigned char END_SPC700Code[];

extern far unsigned char TestInsert[];

uint32_t currentLine;

void fe4Hack() {
	//Grab current line from x register
	if (currentLine == 0x600930ul) {
				MSU_VOLUME = 0xFF;
				MSU_TRACK = 0;
				MSU_CONTROL = 1;		
	}
	else if (currentLine == 0x600982ul) {
				MSU_VOLUME = 0xFF;
				MSU_TRACK = 0;
				MSU_CONTROL = 1;		
	}
	/*
	switch (currentLine) {
			case 0x600930 :
				MSU_VOLUME = 0xFF;
				MSU_TRACK = 0;
				MSU_CONTROL = 1;
				break;
			case 0x600982 :
				MSU_VOLUME = 0xFF;
				MSU_TRACK = 0;
				MSU_CONTROL = 1;
				break;
			default:
				break;
	}*/
	
}



void main(void){
	//Variables
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers
	uint32_t regTest;
	unsigned char testChar;
	char szMSU1Check[10];
	const char szMSU1CheckRef[10] = "S-MSU1";
	char displayString1[40] = "1";
	char displayString2[40] = "2";
	char displayString3[40] = "3";	
	uint8_t msuVolume = 0xFF;
	uint16_t msuTrack = 0;
	uint8_t msuStatus = 0;
	uint8_t lastInputLo;
	uint8_t lastInputHi;
	
	
		
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);		
	termM0Init();
	
	REG_INIDISP = 0x0F;

	//regTest = END_SNESFONT_bin-SNESFONT_bin;
	regTest = (uint32_t)SNESFONT_bin;
	
	do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
	} while( (regRead1 > 0));
	termM0PrintStringXY("Waiting for SPC700 to boot....",0,1);
	SPCWaitBoot();
	termM0PrintStringXY("SPC700 booted",0,2);
	TransferBlockSPC(SPC700Code,0x0200,(uint16_t) ((uint32_t)END_SPC700Code)-((uint32_t)SPC700Code));
	do{ //Wait for Vblank
		regRead1 = REG_RDNMI;
	} while( (regRead1 > 0));
	termM0PrintStringXY("Program transfered",0,3);
	//SPCExecute(0x0200);
	do{ //Wait for Vblank
		regRead1 = REG_RDNMI;
	} while( (regRead1 > 0));
	termM0PrintStringXY("Program started",0,4);
	
	//Enable input
	//This was not in the example but was in the programmers manual
	REG_JOYWR = 0x00;
	//Enalbe joypad and VBlank interrupts
	REG_NMITIMEN = 0x01;
	
	szMSU1Check[0] = MSU_ID;
	szMSU1Check[1] = *((&MSU_ID)+1);
	szMSU1Check[2] = *((&MSU_ID)+2);
	szMSU1Check[3] = *((&MSU_ID)+3);
	szMSU1Check[4] = *((&MSU_ID)+4);
	szMSU1Check[5] = *((&MSU_ID)+5);
	szMSU1Check[6] = *((&MSU_ID)+6);
	szMSU1Check[7] = 0;
	
	//Display if the expansion chip was detected
	if (strcmp(szMSU1CheckRef,szMSU1Check) != 0)
	{
		termM0PrintStringXY("S-MSU1 not detected",0,5);
	}
	else {
		termM0PrintStringXY("s-MSU1 Detected",0,5);
	}
	
	MSU_VOLUME = msuVolume;
	MSU_TRACK = msuTrack;
	MSU_CONTROL = 1;
	msuStatus = MSU_STATUS;
		
	for (;;) {
		sprintf(displayString1,"MSU Volume: %d   ",msuVolume);
		sprintf(displayString2,"MSU Track:  %d   ",msuTrack);
		sprintf(displayString3,"MSU Status: 0x%2x   ",msuStatus);
				
		do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
		} while( (regRead1 > 0));
		termM0PrintStringXY(displayString1,0,10);
		termM0PrintStringXY(displayString2,0,11);
		termM0PrintStringXY(displayString3,0,12);
		
		//Higan specific feature!
		do{ //Wait for RegisterReadFlag
			regRead1 = REG_HVBJOY;
		} while( ( (regRead1 & 0x01) != 0));
		
		lastInputLo = REG_JOY1L;
		lastInputHi = REG_JOY1H;	
		
		//Up
		if (lastInputHi & 0x08){
			msuVolume++;
			MSU_VOLUME = msuVolume;
		}//Down
		else if (lastInputHi & 0x04) {
			msuVolume--;
			MSU_VOLUME = msuVolume;
		}//Left
		if (lastInputHi & 0x02) {
			msuTrack--;
			MSU_TRACK = msuTrack;
		}//Right
		else if (lastInputHi & 0x01) {
			msuTrack++;
			MSU_TRACK = msuTrack;
		}//Idle
		if (lastInputLo & 0x80) {
			MSU_CONTROL = 1;
		}
		else if (lastInputLo & 0x40) {
			MSU_CONTROL = 0;
		}
		else if (lastInputLo & 0x20) {
			MSU_TRACK = msuTrack;
			MSU_CONTROL = 1;
		}
			
		
	}
	
}

void far IRQHandler(void){
	
}

int termM0Init(void){	
	const unsigned char BGPAL[] = {0x00,0x00,0xFF,0x7F};
	const unsigned char BGCLEAR[] = {0x20, 0x00};
	uint16_t transferSize;
	transferSize = ((uint32_t)&END_SNESFONT_bin)-((uint32_t)&SNESFONT_bin);
	transferSize = 1016;
	LoadCGRam(BGPAL, 0x00, 4, 0); // Load BG Palette Data
	LoadLoVram((char *)&SNESFONT_bin, 0x0000,(uint16_t) ((uint32_t)END_SNESFONT_bin)-((uint32_t)SNESFONT_bin), 7);
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