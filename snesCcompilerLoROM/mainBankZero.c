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

int initOAMCopy(unsigned char *pSource){
	uint16_t i;
	for (i = 0; i < 128; i++){
		*pSource = 0x01;
		pSource++;
		*pSource = 0x00;
		pSource++;
		*pSource = 0x00;
		pSource++;
		*pSource = 0x00;
		pSource++;
	}
	for(i = 0; i < 32; i++){
		*pSource = 0x55;
		pSource++;
	}
	
	return 0;
}

int LoadOAMCopy(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize, int cChannel){
	uint16_t regWrite1; //Variable for storing hardware registers
	uint8_t  regWrite2; //Variable for storing hardware registers				 
	regWrite1 = (uint16_t) ((uint32_t)pSource);
	regWrite2 = (uint8_t) (((uint32_t)pSource)>> 16);	
	
	REG_OAMADD = pVRAMDestination;
	REG_DAS0 = cSize;
	REG_DMAP0 = 0x00;
	REG_BBAD0 = 0x04;
	REG_A1T0 = regWrite1;
	REG_A1B0 = regWrite2;	
	
	REG_MDMAEN = 0x01;
	return 0;
}

const char testString[40] = "Initial string in ROM";
const char testString2[40] = "\nInitial string in ROM 2";
//char testStringRam[40] = "Initial string in User Data";
char testStringRam[40] = "Error";
void main(void){
	//Variables
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
	
	//LoadVram(school_bin, 0x0000, (uint16_t) school_bin_size, 7);
	//LoadCGRam(school_pal, 0x10, sizeof(school_pal), something);
	
	//Simple recreation of input test
	//ButtonTest();
	//Mode0Text();
	/*
	termM0Init();
	termM0PrintString("Part 1\nLine 2\nLine 3\nLine 4 is very long");
	termM0PrintString("\rMore text\nLine 5\n");
	termM0PrintString("\nLine 6\r");
	termM0PrintString("12346578901234657890123465789012\n");
	termM0PrintString("\nABC");
	termM0PrintString("\n123");
	termM0PrintString(testString);
	termM0PrintString(testString2);
	*/
	LoadCGRam(biker_clr, 0x80, sizeof(biker_clr), 0); // Load BG Palette Data
	LoadVram(biker_pic, 0x0000, sizeof(biker_pic), 7);
	
	//This seems correct to me but the compiler still spits out warnings.
	pOAMCopy = (union uOAMCopy *) farcalloc(1,sizeof(union uOAMCopy));
	initOAMCopy(pOAMCopy->Bytes);
	
	pOAMCopy->Names.OBJ000X = 112;
	pOAMCopy->Names.OBJ000Y = 96;
	pOAMCopy->Names.CHARNUM000 = 0;
	pOAMCopy->Names.PROPERTIES000 = 0x70;
	pOAMCopy->Names.OAMTABLE2BYTE00 = 0x54;
	REG_OBJSEL = 0xA0;
	REG_TM = 0x10;

	LoadOAMCopy(pOAMCopy->Bytes,0x0000,sizeof(union uOAMCopy),0);
	
	REG_INIDISP = 0x0F;
	while(1){
		do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
		} while( (regRead1 > 0));
		//termM0PrintStringXY(testStringRam);
		//sprintf(testStringRam,"Value: %06d\0",counter);
		counter++;
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

