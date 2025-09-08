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
	char displayString4[40] = "4";
	char displayString5[40] = "5";
	
	
	uint8_t DSPCommandCode = 0x00;
	uint16_t DSPparameter1 = 1;
	uint16_t DSPparameter2 = 6;
	uint16_t DSPresult = 0x11;	
	uint16_t  DSPStatus = 0xAA;
	

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
	DSPStatus = DSP1_SR_16;
	if ((DSPStatus & 0x8000) == 0)
	{
		termM0PrintStringXY("DSP1 not detected!",0,5);
	}
	else {
		termM0PrintStringXY("DSP1 detected!",0,5);
	}
	
	DSPStatus = DSP1_SR_8;
	DSP1_DR_8 = DSPCommandCode;
	DSP1_DR_16 = DSPparameter1;
	DSP1_DR_16 = DSPparameter2;
	
	for (;;) {
		sprintf(displayString1,"Parameter 1: 0x%4x    ",DSPparameter1);
		sprintf(displayString2,"Parameter 2: 0x%4x    ",DSPparameter2);
		sprintf(displayString3,"DSPCommandCode:  0x%2x    ",DSPCommandCode);
		sprintf(displayString4,"DSPStatus:  0x%4x    ",DSPStatus);
		DSPresult = DSP1_DR_16;
		sprintf(displayString5,"Result:     0x%4x    ",DSPresult);
			
		do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
		} while( (regRead1 > 0));
		termM0PrintStringXY(displayString1,0,10);
		termM0PrintStringXY(displayString2,0,11);
		termM0PrintStringXY(displayString3,0,12);
		termM0PrintStringXY(displayString4,0,13);
		termM0PrintStringXY(displayString5,0,14);
		
		//Higan specific feature!
		do{ //Wait for RegisterReadFlag
			regRead1 = REG_HVBJOY;
		} while( ( (regRead1 & 0x01) != 0));
		
		lastInputLo = REG_JOY1L;
		lastInputHi = REG_JOY1H;	
		
		//Up
		if (lastInputHi & 0x08){

		}//Down
		else if (lastInputHi & 0x04) {
		}//Left
		if (lastInputHi & 0x02) {
		}//Right
		else if (lastInputHi & 0x01) {
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