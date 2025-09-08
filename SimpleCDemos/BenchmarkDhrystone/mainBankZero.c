#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "snes_regs_xc.h"
#include "include/imagedata.h"
#include "initsnes.h"

/* External declarations for dhry.c functions */
extern int dhrystone_main(int number_of_runs);

/* Function declarations */
int termM0Init(void);
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY);

/* External functions required by dhry.c */
uint32_t snesTimeFrames(void);
void snesPrintResults(uint32_t loops, uint32_t time_used);

/* SNES timing system */
volatile uint32_t frameCounter = 0;

/* Dhrystone benchmark variables (integer math - 16-bit compatible) */
static uint16_t dhrystones_per_second = 0;
static uint32_t microseconds_per_dhrystone = 0; /* Keep 32-bit for overflow safety */

/* Integer to string conversion function (handles both 16-bit and 32-bit) */
void intToString(uint32_t value, char* buffer) {
    int32_t i = 0;
    int32_t k = 0;
    uint32_t temp;
    char int_buffer[12];
    int j = 0;
    
    if (value == 0) {
        buffer[i++] = '0';
    } else {
        temp = value;
        while (temp > 0) {
            int_buffer[j++] = '0' + (temp % 10);
            temp /= 10;
        }
        
        /* Reverse the string */
        for (k = j - 1; k >= 0; k--) {
            buffer[i++] = int_buffer[k];
        }
    }
    
    /* Null terminate */
    buffer[i] = '\0';
}

/* 16-bit specific version for dhrystones_per_second */
void intToString16(uint16_t value, char* buffer) {
    intToString((uint32_t)value, buffer);
}

#ifdef __TCC816__
/* Custom string functions for TCC816 compatibility */
void my_strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void my_strcat(char* dest, const char* src) {
    while (*dest) {
        dest++;
    }
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

int my_strlen(const char* str) {
    int len = 0;
    while (*str++) {
        len++;
    }
    return len;
}

/* Define macros to use custom functions for TCC816 */
#define strcpy my_strcpy
#define strcat my_strcat
#define strlen my_strlen
#endif

/* SNES-compatible timing function (replaces dtime()) - integer version */
uint32_t snesTimeFrames(void) {
    return frameCounter; /* Return frame count directly */
}

/* SNES-compatible result printing function */
void snesPrintResults(uint32_t loops, uint32_t time_used) {
    char displayBuffer[80];
    char intStr[20];
    int32_t temp_dhrystones;
    
    /* Calculate results using integer math (16-bit compatible) */
    /* dhrystones_per_second = (loops * 60) / time_used */
    /* Use 32-bit intermediate to avoid overflow, then cast to 16-bit */
    temp_dhrystones = ((uint32_t)loops * 60);
    if (time_used > 0) {
        temp_dhrystones = temp_dhrystones / time_used;
    }
    dhrystones_per_second = (uint16_t)temp_dhrystones;
    
    /* microseconds_per_dhrystone = (time_used * 1000000) / (loops * 60) */
    /* Keep as 32-bit to handle large numbers safely */
    microseconds_per_dhrystone = ((uint32_t)time_used * 1000000) / ((uint32_t)loops * 60);
    
    /* Display results */
    termM0PrintStringXY("=====================",(uint8_t)0,(uint8_t)5);
    if (time_used == 0) {
        strcpy(displayBuffer, "Optimized out entire test");
        termM0PrintStringXY(displayBuffer, (uint8_t)0, (uint8_t)6);
        return;
    }
    
    /* Dhrystones per second (16-bit value) */
    strcpy(displayBuffer, "Dhrystones/sec: ");
    intToString16(dhrystones_per_second, intStr);
    strcat(displayBuffer, intStr);
    while (strlen(displayBuffer) < 32) {
        strcat(displayBuffer, " ");
    }
    termM0PrintStringXY(displayBuffer, (uint8_t)0, (uint8_t)6);
    
    /* Microseconds per Dhrystone */
    strcpy(displayBuffer, "us/Dhrystone: ");
    intToString(microseconds_per_dhrystone, intStr);
    strcat(displayBuffer, intStr);
    while (strlen(displayBuffer) < 32) {
        strcat(displayBuffer, " ");
    }
    termM0PrintStringXY(displayBuffer, (uint8_t)0, (uint8_t)7);
    
    /* Time used in frames */
    strcpy(displayBuffer, "Frames: ");
    intToString(time_used, intStr);
    strcat(displayBuffer, intStr);
    while (strlen(displayBuffer) < 32) {
        strcat(displayBuffer, " ");
    }
    termM0PrintStringXY(displayBuffer, (uint8_t)0, (uint8_t)8);
    
    /* Number of runs */
    strcpy(displayBuffer, "Runs: ");
    intToString(loops, intStr);
    strcat(displayBuffer, intStr);
    while (strlen(displayBuffer) < 32) {
        strcat(displayBuffer, " ");
    }
    termM0PrintStringXY(displayBuffer, (uint8_t)0, (uint8_t)9);
    
    /* Test descriptions */
    termM0PrintStringXY("Higher=Better",(uint8_t)0,(uint8_t)12);
}

int main(void) {
    int8_t regRead1; /* Variable for storing hardware registers */
    int i;
    
    /* Clear problematic memory area that C runtime might be using */
    volatile uint8_t *mem = (volatile uint8_t *)0x0200;
    for (i = 0; i < 0x1C00; i++) {
        mem[i] = 0;
    }
    
    /* Initialization */
    initSNES(SLOWROM);
    
 
    
    termM0Init();
    termM0PrintStringXY("Dhrystone Test for SNES in C",0,1);
    termM0PrintStringXY("Compiler:",0,2);
    termM0PrintStringXY(SNES_XC_COMPILER_NAME,20,2);
    REG_INIDISP = 0x0F;
    REG_NMITIMEN = 0x81; /* Enable NMI for frame counting */
    
    /* Initialize static variables to zero */
    dhrystones_per_second = 0;
    microseconds_per_dhrystone = 0;
    
    /* Run the benchmark */
    termM0PrintStringXY("Running benchmark...",0,3);
    do { /* Wait for Vblank */
        regRead1 = REG_RDNMI;
    } while(regRead1 > 0);
    dhrystone_main(30000);
    
    /* Main loop - just display results */
    while(1) {
        do { /* Wait for Vblank */
            regRead1 = REG_RDNMI;
        } while(regRead1 > 0);
        termM0PrintStringXY("TEST COMPLETE           ",0,15);      
    }
    return 0;
}

/* Cross-compiler interrupt handlers, must be present */
/* Note: These are called from assembly interrupt handlers, so they should NOT have */
/* the interrupt attribute as register saving/restoring is handled in assembly */
void snesXC_cop(void) {
}

void snesXC_brk(void) {
}

void snesXC_abort(void) {
}

void snesXC_nmi(void) {
    frameCounter++;
}

void snesXC_irq(void) {
}

int termM0Init(void){	
	const static unsigned char BGPAL[] = {0x00,0x00,0xFF,0x7F, 0x00, 0x00, 0x00, 0x00};
	const static unsigned char BGCLEAR[] = {0x20, 0x00};
	LoadCGRam(BGPAL, 0x00, sizeof(BGPAL)); /* Load BG Palette Data */
	/*TODO Fix init snes and make it actually clear VRAM */
	LoadLoVram(SNESFONT_bin, 0x0000, sizeof(SNESFONT_bin));
    ClearVram(BGCLEAR, 0xF800, 0x400); /* Clear VRAM Map To Fixed Tile Word */
	
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

/*Takes an input and maps prints to an xy on the tile */
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY){
	int8_t regRead1;		
	/* Wait for 2 frames */
	do {
		regRead1 = REG_RDNMI;
	} while(regRead1 > 0);
	do {
		regRead1 = REG_RDNMI;
	} while(regRead1 > 0);
	LoadLoVram((unsigned char *)szInput, 0xF800+inpX+inpY*0x40, strlen(szInput)); /* Load Text To VRAM Lo Bytes */
	/*Wait for 1 more frames */
	do {
		regRead1 = REG_RDNMI;
	} while(regRead1 > 0);
	return 0;
}
