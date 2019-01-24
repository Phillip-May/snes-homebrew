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
const char testString2[40] = "\nInitial string in ROM 2";
//char testStringRam[40] = "Initial string in User Data";
char testStringRam[40] = "Error";
void main(void){
	//Variables
	uint8_t lastInputLo;
	uint8_t lastInputHi;
	uint16_t spriteX = 0x00;
	uint8_t spriteY = 0x00;	
	uint16_t BGTileLocation = 0xF800;
	unsigned char BGTile[] = {0x20, 0x00};
	uint16_t BGTileValue = 0x0020;
	int i = 0;
	static int something = 5;
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers
	uint8_t *test_heap2;
	uint32_t counter = 40000;
	union uOAMCopy *pOAMCopy;
	char *stringa1;
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);
	
	LoadVram(school_bin, 0x2000, (uint16_t) school_bin_size, 7);
	LoadCGRam(school_pal, 0x00, sizeof(school_pal), something);
	LoadCGRam(biker_clr, 0x80, sizeof(biker_clr), 0); // Load BG Palette Data
	LoadVram(biker_pic, 0x0000, sizeof(biker_pic), 7);
	
	//ClearVram(BGCLEAR, 0xF800, 0x400, 0); // Clear VRAM Map To Fixed Tile Word
	//Initialise BG1's tilemap to incrementing tiles indexes bitmap
	LoadVram(school_tilemap, BGTileLocation, sizeof(school_tilemap), 7);	
	
	//This seems correct to me but the compiler still spits out warnings.
	pOAMCopy = (union uOAMCopy *) farcalloc(1,sizeof(union uOAMCopy));
	initOAMCopy(pOAMCopy->Bytes);
	
	pOAMCopy->Names.OBJ000X = 112;
	pOAMCopy->Names.OBJ000Y = 96;
	pOAMCopy->Names.CHARNUM000 = 0;
	pOAMCopy->Names.PROPERTIES000 = 0x70;
	pOAMCopy->Names.OAMTABLE2BYTE00 = 0x54;
	
	
	REG_OBJSEL = 0xA0;
	REG_TM = 0x11;
	
	REG_BGMODE  = 0x09;
	//Tilemap offset
	REG_BG1SC  = 0xFC;
	REG_BG12NBA = 0x01;
	
	REG_BG1HOFS = 0x00;
	REG_BG1HOFS = 0x00;
	REG_BG1VOFS = 0x00;
	REG_BG1VOFS = 0x00;	
		
	REG_NMITIMEN = 0x01;
	REG_INIDISP = 0x0F;
	while(1){
		do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
		} while( (regRead1 > 0));
		lastInputLo = REG_JOY1L;
		lastInputHi = REG_JOY1H;
		LoadOAMCopy(pOAMCopy->Bytes,0x0000,sizeof(union uOAMCopy),0);
		//Up
		if (lastInputHi & 0x08){
			spriteY--;
			spriteY--;
			spriteY--;
			spriteY--;
		}//Down
		else if (lastInputHi & 0x04) {
			spriteY++;
			spriteY++;
			spriteY++;
			spriteY++;
		}//Left
		if (lastInputHi & 0x02) {
			spriteX--;
			spriteX--;
			spriteX--;
			spriteX--;
		}//Right
		else if (lastInputHi & 0x01) {
			spriteX++;
			spriteX++;
			spriteX++;
			spriteX++;
		}//Idle
		
		pOAMCopy->Names.OBJ000X = spriteX;
		pOAMCopy->Names.OAMTABLE2BYTE00 = (spriteX & 0x0100) >> 8;
		pOAMCopy->Names.OBJ000Y = spriteY;
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
int termM0PrintString(const char *szInput){
	static uint16_t curWritePos = 0xF800;
	uint8_t *test_heap;
	uint8_t curOffset = 0;
	int i;
	test_heap = (uint8_t*)farmalloc((uint32_t)(strlen(szInput)+1));
	//strcpy(test_heap,szInput);
	for (i = 0; i < (strlen(szInput)+1); i++){
		if( (szInput[i] == '\n') ) {
			curOffset = i;
			if (strlen(test_heap) != 0){
				LoadLoVram(test_heap, curWritePos, strlen(test_heap), 0); // Load Text To VRAM Lo Bytes
				curWritePos = curWritePos + (strlen(test_heap) << 1);
			}
			curOffset = i + 1;
			//Change edgecase behaviour
			/*if ( (curWritePos & 0x003F) != 0x0000 ){
			//	curWritePos = (curWritePos & 0xFFC0) + 0x40;	
			}*/
			curWritePos = (curWritePos & 0xFFC0) + 0x40;
		}
		else if( (szInput[i] == '\r') ) {
			if (strlen(test_heap) != 0){
				LoadLoVram(test_heap, curWritePos, strlen(test_heap), 0); // Load Text To VRAM Lo Bytes
			}
			curOffset = i + 1;
			curWritePos = (curWritePos & 0xFFC0);
		}
		else if( (szInput[i] == '\0') ) {
			test_heap[i-curOffset] = '\0';
			if (strlen(test_heap) != 0){
				LoadLoVram(test_heap, curWritePos, strlen(test_heap), 0); // Load Text To VRAM Lo Bytes
				curWritePos = curWritePos + (strlen(test_heap) << 1);
			}
			curOffset = i + 1;
		}
		test_heap[i-curOffset] = szInput[i];
		test_heap[i-curOffset+1] = '\0';
	}
	farfree(test_heap);
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
		if ( (regRead1 != 0) || (regRead2 != 0)){
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
