#define MY_REGISTER (*(volatile uint32_t*)0x7E2004u)
typedef	unsigned char byte;
//LoROM mememory map


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mainBankZero.h"
#include "snes_regs_xc.h"
#include "initsnes.h"
#include "imagedata.h"

//Try not to put blocks of memory on stack
//Not every compiler will be smart enough to move it automatically
union uOAMCopy oamCopy;

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
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);
	
	LoadVram(school_bin, 0x2000, sizeof(school_bin));
	LoadCGRam(school_pal, 0x00, sizeof(school_pal));
	LoadCGRam(biker_clr, 0x80, sizeof(biker_clr)); // Load BG Palette Data
	LoadVram(biker_pic, 0x0000, sizeof(biker_pic));
	
	//ClearVram(BGCLEAR, 0xF800, 0x400, 0); // Clear VRAM Map To Fixed Tile Word
	//Initialise BG1's tilemap to incrementing tiles indexes bitmap
	LoadVram(school_tilemap, BGTileLocation, sizeof(school_tilemap));	
	//Initialize OAM copy structure
	initOAMCopy(oamCopy.Bytes);
	
	oamCopy.Names.OBJ000X = 112;
	oamCopy.Names.OBJ000Y = 96;
	oamCopy.Names.CHARNUM000 = 0;
	oamCopy.Names.PROPERTIES000 = 0x70;
	oamCopy.Names.OAMTABLE2BYTE00 = 0x54;
	
	
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
		
	REG_NMITIMEN = 0x01; //Joypad autoread enable
	REG_INIDISP = 0x0F;
	while(1){
		do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
		} while( (regRead1 > 0));
		LoadOAMCopy(oamCopy.Bytes,0x0000,sizeof(union uOAMCopy));
		do{ //Wait for joypad read ready
			regRead1 = REG_HVBJOY;
		} while( (regRead1 & 0x01) != 0);

		lastInputLo = REG_JOY1L;
		lastInputHi = REG_JOY1H;
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
		
		oamCopy.Names.OBJ000X = spriteX;
		oamCopy.Names.OAMTABLE2BYTE00 &= 0xFE;
		oamCopy.Names.OAMTABLE2BYTE00 |= (spriteX & 0x0100) >> 8;
		oamCopy.Names.OBJ000Y = spriteY;
	}
}

// Cross-compiler interrupt handler implementations
void snesXC_cop(void) {
    // COP (Coprocessor) interrupt handler
}

void snesXC_brk(void) {
    // BRK (Break) interrupt handler
}

void snesXC_abort(void) {
    // ABORT interrupt handler
}

void snesXC_nmi(void) {
    // NMI (Non-Maskable Interrupt) handler
}

void snesXC_irq(void) {
    // IRQ (Interrupt Request) handler
}
