//LoROM mememory map
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "snes_regs_xc.h"
#include "include/imagedata.h"
#include "initsnes.h"

// Function declarations
int termM0Init(void);
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY);
void ButtonTest(void);
void Mode0Text(void);
void floatToString(float value, char* buffer, int precision);

const char testString[40] = "Initial string in ROM";

// Custom float to string conversion function
//VBCC's sprintf has trouble with floating point numbers
void floatToString(float value, char* buffer, int precision) {
    int i;
    int k;
    int integer_part;
    int decimal_part;
    int temp;
    int is_negative;
    float decimal_float;
	
    i = 0;
    k = 0;
    is_negative = 0;

    /* Handle negative numbers */
    if (value < 0) {
        is_negative = 1;
        value = -value;
        buffer[i++] = '-';
    }

    /* Extract integer part */
    integer_part = (int)value;

    /* Convert integer part to string */
    if (integer_part == 0) {
        buffer[i++] = '0';
    } else {
        int digits;
        char int_buffer[10];
        int j;
        digits = 0;
        temp = integer_part;
        while (temp > 0) {
            digits++;
            temp /= 10;
        }

        /* Convert integer part to string (reverse order) */
        j = 0;
        temp = integer_part;
        while (temp > 0) {
            int_buffer[j++] = '0' + (temp % 10);
            temp /= 10;
        }

        /* Reverse the integer part string */
        for (k = j - 1; k >= 0; k--) {
            buffer[i++] = int_buffer[k];
        }
    }

    /* Add decimal point */
    buffer[i++] = '.';

    /* Extract decimal part */
    decimal_float = value - (float)integer_part;
    for (k = 0; k < precision; k++) {
        decimal_float *= 10.0f;
        decimal_part = (int)decimal_float;
        buffer[i++] = '0' + (decimal_part % 10);
        decimal_float -= (float)decimal_part;
    }

    /* Null terminate */
    buffer[i] = '\0';
}

int main(void){
	int8_t regRead1; //Variable for storing hardware registers
	float floatingCounter = 0.0;
	char testStringRam[40] = "Error";
	char floatStr[20];
	//Initialization
	initSNES(SLOWROM);
		
	termM0Init();
	termM0PrintStringXY("Test",0,0);
	REG_INIDISP = 0x0F;
	REG_NMITIMEN = 0x01;
	while(1){
		do{ //Wait for Vblank
			regRead1 = REG_RDNMI;
		} while( regRead1 >  0);
		termM0PrintStringXY(testStringRam,0,0);
		// Use custom float conversion instead of sprintf
		// Avoids a weird issue with VBCC's sprintf
		floatToString(floatingCounter, floatStr, 7);
		strcpy(testStringRam, "Value: ");
		strcat(testStringRam, floatStr);

		floatingCounter = floatingCounter + 0.01f;
	}
	return 0;
}

// Cross-compiler interrupt handlers, must be present
void snesXC_cop(void) {
}

void snesXC_brk(void) {
}

void snesXC_abort(void) {
}

void snesXC_nmi(void) {
}

void snesXC_irq(void) {
}

int termM0Init(void){	
	const static unsigned char BGPAL[] = {0x00,0x00,0xFF,0x7F};
	const static unsigned char BGCLEAR[] = {0x20, 0x00};
	LoadCGRam(BGPAL, 0x00, sizeof(BGPAL), 0); // Load BG Palette Data
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
	LoadLoVram((unsigned char *)szInput, 0xF944, strlen(szInput), 0); // Load Text To VRAM Lo Bytes
	return 0;
}	 