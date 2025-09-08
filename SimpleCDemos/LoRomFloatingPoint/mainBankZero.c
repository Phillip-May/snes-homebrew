//LoROM mememory map
// 0x0000-0x1FFF: RAM
// 0x2000-0x7FFF: RAM (mirror)
// 0x8000-0xFFFF: ROM
// 0x10000-0x1FFFF: ROM (mirror)

// Test mode configuration
// Uncomment the next line to enable test mode (skip actual benchmark)
// Mostly to test the output formating without running the full 40+ min test

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

// LLVM-MOS and VBCC don't have the same math library support as WDC816CC
// Provide simple implementations for missing functions
#if defined(__llvm__) || defined(__VBCC__)
// Simple approximations for math functions when using LLVM-MOS
static float simple_sin(float x) {
    // Simple sine approximation using Taylor series
    float x2 = x * x;
    float x3 = x2 * x;
    float x5 = x3 * x2;
    float x7 = x5 * x2;
    return x - x3/6.0f + x5/120.0f - x7/5040.0f;
}

static float simple_cos(float x) {
    // Simple cosine approximation using Taylor series
    float x2 = x * x;
    float x4 = x2 * x2;
    float x6 = x4 * x2;
    return 1.0f - x2/2.0f + x4/24.0f - x6/720.0f;
}

static float simple_atan(float x) {
    // Simple arctangent approximation
    if (x > 1.0f) return 1.570796f - x/(1.0f + x*x);
    if (x < -1.0f) return -1.570796f - x/(1.0f + x*x);
    return x - x*x*x/3.0f + x*x*x*x*x/5.0f - x*x*x*x*x*x*x/7.0f;
}

static float simple_sqrt(float x) {
    // Simple square root using Newton's method
    if (x < 0.0f) return 0.0f;
    if (x == 0.0f) return 0.0f;
    float guess = x * 0.5f;
    for (int i = 0; i < 10; i++) {
        guess = (guess + x/guess) * 0.5f;
    }
    return guess;
}

static float simple_exp(float x) {
    // Simple exponential approximation
    float result = 1.0f;
    float term = 1.0f;
    for (int i = 1; i < 10; i++) {
        term *= x / i;
        result += term;
    }
    return result;
}

static float simple_log(float x) {
    // Simple natural logarithm approximation
    if (x <= 0.0f) return -1000.0f;
    if (x == 1.0f) return 0.0f;
    float result = 0.0f;
    float y = (x - 1.0f) / (x + 1.0f);
    float y2 = y * y;
    float term = y;
    for (int i = 0; i < 20; i++) {
        result += term / (2*i + 1);
        term *= y2;
    }
    return 2.0f * result;
}

// Define macros to use our simple implementations
// Only define if not already defined by the compiler
#ifndef sin
#define sin simple_sin
#endif
#ifndef cos
#define cos simple_cos
#endif
#ifndef atan
#define atan simple_atan
#endif
#ifndef sqrt
#define sqrt simple_sqrt
#endif
#ifndef exp
#define exp simple_exp
#endif
#ifndef log
#define log simple_log
#endif
#endif
#include "snes_regs_xc.h"
#include "include/imagedata.h"
#include "initsnes.h"

// Function declarations
int termM0Init(void);
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY);
void ButtonTest(void);
void Mode0Text(void);
void floatToString(float value, char* buffer, int32_t precision);

// Whetstone benchmark functions
void whetstones(long xtra, long x100, int calibrate);
void pa(float e[4], float t, float t2);
void po(float e1[4], long j, long k, long l);
void p3(float *x, float *y, float *z, float t, float t1, float t2);
void pout(char title[22], float ops, int type, float checknum, float time, int calibrate, int section);

// SNES timing system
volatile uint32_t frameCounter = 0;
volatile uint32_t startFrame = 0;
volatile uint32_t endFrame = 0;

// Whetstone benchmark variables
static float loop_time[9] = {0.0f};
static float TimeUsed;
static float mwips;
static float Check;


// Custom float to string conversion function, avoids sprintf
void floatToString(float value, char* buffer, int32_t precision) {
    int32_t i;
    int32_t k;
    int32_t integer_part;
    int32_t decimal_part;
    int32_t temp;
    int32_t is_negative;
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

// SNES-compatible timing function (replaces dtime())
float snesTime(void) {
    return (float)frameCounter / 60.0f; // 60 FPS on SNES
}

int main(void) {
    int8_t regRead1; // Variable for storing hardware registers
    char displayBuffer[80];
    char floatStr[20];
    char resultLine[80];
    char perfStr[15];
    int section;
    int i, j;
    int perfLen;
    int spacesNeeded;
    long xtra = 1;
    long x100 = 100;
    int calibrate = 0;
    
    // Clear problematic memory area that C runtime might be using
    volatile uint8_t *mem = (volatile uint8_t *)0x0200;
    for (i = 0; i < 0x1C00; i++) {
        mem[i] = 0;
    }
    
    // Initialization
    initSNES(SLOWROM);
    
    termM0Init();
    termM0PrintStringXY("Whetstone Test for SNES in C",0,1);
    termM0PrintStringXY("Compiler:",0,2);
    termM0PrintStringXY(SNES_XC_COMPILER_NAME,20,2);
    REG_INIDISP = 0x0F;
    REG_NMITIMEN = 0x81; // Enable NMI for frame counting
    
    // Initialize static variables to zero
    for (i = 0; i < 9; i++) {
        loop_time[i] = 0.0f;
    }
    TimeUsed = 0.0f;
    mwips = 0.0f;
    Check = 0.0f;
    
    // Simplified: Run once with fixed parameters
    termM0PrintStringXY("Running benchmark...",0,3);
    
    // Run the benchmark once
    TimeUsed = 0;
    startFrame = frameCounter;
    whetstones(xtra, x100, calibrate);
    endFrame = frameCounter;
    TimeUsed = (float)(endFrame - startFrame) / 60.0f;
    
    // Calculate MWIPS with division by zero check
    if (TimeUsed > 0.0f && xtra > 0 && x100 > 0) {
        mwips = (float)(xtra) * (float)(x100) / (10.0f * TimeUsed);
    } else {
        mwips = 0.0f;
    }
    
    // Summary lines
    termM0PrintStringXY("=====================",(uint8_t)0,(uint8_t)15);
    strcpy(displayBuffer, "MWIPS: ");
    floatToString(mwips, floatStr, 7);
    strcat(displayBuffer, floatStr);
    while (strlen(displayBuffer) < 32) {
        strcat(displayBuffer, " ");
    }
    termM0PrintStringXY(displayBuffer, (uint8_t)0, (uint8_t)16);
    
    // Total time
    strcpy(displayBuffer, "Time: ");
    floatToString(TimeUsed, floatStr, 2);
    strcat(displayBuffer, floatStr);
    strcat(displayBuffer, "s");
    while (strlen(displayBuffer) < 32) {
        strcat(displayBuffer, " ");
    }
    termM0PrintStringXY(displayBuffer, (uint8_t)0, (uint8_t)17);
    
    // Test descriptions
    termM0PrintStringXY("Test Descriptions:",(uint8_t)0,(uint8_t)18);
    termM0PrintStringXY("N1-2: Float ops  N3: If/else",(uint8_t)0,(uint8_t)19);
    termM0PrintStringXY("N4: Integer     N5: Trig",(uint8_t)0,(uint8_t)20);
    termM0PrintStringXY("N6: Calls       N7: Arrays",(uint8_t)0,(uint8_t)21);
    termM0PrintStringXY("N8: Math funcs  Higher=Better",(uint8_t)0,(uint8_t)22);
    
    // Main loop - just display results
    while(1) {
        do { // Wait for Vblank
            regRead1 = REG_RDNMI;
        } while(regRead1 > 0);
        termM0PrintStringXY("TEST COMPLETE           ",0,25);      
    }
    return 0;
}

// Cross-compiler interrupt handlers, must be present
// Note: These are called from assembly interrupt handlers, so they should NOT have
// the interrupt attribute as register saving/restoring is handled in assembly
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
	LoadCGRam(BGPAL, 0x00, sizeof(BGPAL), 0); // Load BG Palette Data
	//TODO Fix init snes and make it actually clear VRAM
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
	int8_t regRead1;		
	// Wait for 2 frames
	do {
		regRead1 = REG_RDNMI;
	} while(regRead1 > 0);
	do {
		regRead1 = REG_RDNMI;
	} while(regRead1 > 0);
	LoadLoVram((unsigned char *)szInput, 0xF800+inpX+inpY*0x40, strlen(szInput), 0); // Load Text To VRAM Lo Bytes
	//Wait for 1 more frames
	do {
		regRead1 = REG_RDNMI;
	} while(regRead1 > 0);
	return 0;
}	 

// Individual Whetstone test functions
void test1_array_elements(long xtra, long x100, int calibrate) {
    int32_t n1, i, ix, n1mult;
    float e1[4], timea, timeb;
    float t = 0.49999975f;
    float t0 = 0.49999975f;
    
    n1 = 12*x100;
    n1mult = 10;
    
    e1[0] = 1.0f;
    e1[1] = -1.0f;
    e1[2] = -1.0f;
    e1[3] = -1.0f;
    timea = snesTime();
    {
        for (ix=0; ix<xtra; ix++) {
            for(i=0; i<n1*n1mult; i++) {
                e1[0] = (e1[0] + e1[1] + e1[2] - e1[3]) * t;
                e1[1] = (e1[0] + e1[1] - e1[2] + e1[3]) * t;
                e1[2] = (e1[0] - e1[1] + e1[2] + e1[3]) * t;
                e1[3] = (-e1[0] + e1[1] + e1[2] + e1[3]) * t;
            }
            t = 1.0f - t;
        }
        t = t0;
    }
    
    if (n1mult != 0) {
        timeb = (snesTime()-timea)/(float)(n1mult);
    } else {
        timeb = 0.0f;
    }
    pout("N1 float\0",(float)(n1*16)*(float)(xtra), 1, e1[3], timeb, calibrate, 1);
}

void test2_array_parameter(long xtra, long x100, int calibrate) {
    int32_t n2, i, ix;
    float e1[4], timea, timeb;
    float t = 0.49999975f;
    float t0 = 0.49999975f;
    float t2 = 2.0f;
    
    n2 = 14*x100;
    
    e1[0] = 1.0f;
    e1[1] = -1.0f;
    e1[2] = -1.0f;
    e1[3] = -1.0f;
    
    timea = snesTime();
    {
        for (ix=0; ix<xtra; ix++) {
            for(i=0; i<n2; i++) {
                pa(e1,t,t2);
            }
            t = 1.0f - t;
        }
        t = t0;
    }
    timeb = snesTime()-timea;
    pout("N2 float\0",(float)(n2*96)*(float)(xtra), 1, e1[3], timeb, calibrate, 2);
}

void test3_conditional_jumps(long xtra, long x100, int calibrate) {
    int32_t n3, i, ix, j;
    float timea, timeb;
    
    n3 = 345*x100;
    j = 1;
    timea = snesTime();
    {
        for (ix=0; ix<xtra; ix++) {
            for(i=0; i<n3; i++) {
                if(j==1)       j = 2;
                else           j = 3;
                if(j>2)        j = 0;
                else           j = 1;
                if(j<1)        j = 1;
                else           j = 0;
            }
        }
    }
    timeb = snesTime()-timea;
    pout("N3 if/else\0",(float)(n3*3)*(float)(xtra), 2, (float)(j), timeb, calibrate, 3);
}

void test4_integer_arithmetic(long xtra, long x100, int calibrate) {
    int32_t n4, i, ix, j, k, l;
    float e1[4], timea, timeb, x;
    
    n4 = 210*x100;
    j = 1;
    k = 2;
    l = 3;
    timea = snesTime();
    {
        for (ix=0; ix<xtra; ix++) {
            for(i=0; i<n4; i++) {
                j = j *(k-j)*(l-k);
                k = l * k - (l-j) * k;
                l = (l-k) * (k+j);
                e1[l-2] = j + k + l;
                e1[k-2] = j * k * l;
            }
        }
    }
    timeb = snesTime()-timea;
    x = e1[0]+e1[1];
    pout("N4 fixed\0",(float)(n4*15)*(float)(xtra), 2, x, timeb, calibrate, 4);
}

void test5_trig_functions(long xtra, long x100, int calibrate) {
    int32_t n5, i, ix;
    float x, y, timea, timeb;
    float t = 0.49999975f;
    float t0 = 0.49999975f;
    float t2 = 2.0f;
    
    n5 = 32*x100;
    x = 0.5f;
    y = 0.5f;
    timea = snesTime();
    {
        for (ix=0; ix<xtra; ix++) {
            for(i=1; i<n5; i++) {
                x = t*atan(t2*sin(x)*cos(x)/(cos(x+y)+cos(x-y)-1.0f));
                y = t*atan(t2*sin(y)*cos(y)/(cos(x+y)+cos(x-y)-1.0f));
            }
            t = 1.0f - t;
        }
        t = t0;
    }
    timeb = snesTime()-timea;
    pout("N5 trig\0",(float)(n5*26)*(float)(xtra), 2, y, timeb, calibrate, 5);
}

void test6_procedure_calls(long xtra, long x100, int calibrate) {
    int32_t n6, i, ix;
    float x, y, z, timea, timeb;
    float t = 0.49999975f;
    float t1 = 0.50000025f;
    float t2 = 2.0f;
    
    n6 = 899*x100;
    x = 1.0f;
    y = 1.0f;
    z = 1.0f;
    timea = snesTime();
    {
        for (ix=0; ix<xtra; ix++) {
            for(i=0; i<n6; i++) {
                p3(&x,&y,&z,t,t1,t2);
            }
        }
    }
    timeb = snesTime()-timea;
    pout("N6 calls\0",(float)(n6*6)*(float)(xtra), 1, z, timeb, calibrate, 6);
}

void test7_array_references(long xtra, long x100, int calibrate) {
    int32_t n7, i, ix, j, k, l;
    float e1[4], timea, timeb;
    
    n7 = 616*x100;
    j = 0;
    k = 1;
    l = 2;
    e1[0] = 1.0f;
    e1[1] = 2.0f;
    e1[2] = 3.0f;
    timea = snesTime();
    {
        for (ix=0; ix<xtra; ix++) {
            for(i=0;i<n7;i++) {
                po(e1,j,k,l);
            }
        }
    }
    timeb = snesTime()-timea;
    pout("N7 arrays\0",(float)(n7*3)*(float)(xtra), 2, e1[2], timeb, calibrate, 7);
}

void test8_standard_functions(long xtra, long x100, int calibrate) {
    int32_t n8, i, ix;
    float x, timea, timeb;
    float t1 = 0.50000025f;
    
    n8 = 93*x100;
    x = 0.75f;
    timea = snesTime();
    {
        for (ix=0; ix<xtra; ix++) {
            for(i=0; i<n8; i++) {
                x = sqrt(exp(log(x)/t1));
            }
        }
    }
    timeb = snesTime()-timea;
    pout("N8 math\0",(float)(n8*4)*(float)(xtra), 2, x, timeb, calibrate, 8);
}


void whetstones(long xtra, long x100, int calibrate) {
    Check = 0.0f;
    
    // Display table header first
    termM0PrintStringXY("Whetstone Results",(uint8_t)0,(uint8_t)3);
    termM0PrintStringXY("==================",(uint8_t)0,(uint8_t)4);
    termM0PrintStringXY("Format: Test Perf Time",(uint8_t)0,(uint8_t)5);
    termM0PrintStringXY("MF=MFLOPS MO=MOPS",(uint8_t)0,(uint8_t)6);
    
    // Run tests one by one - results are displayed automatically in pout()
    test1_array_elements(xtra, x100, calibrate);
    
    test2_array_parameter(xtra, x100, calibrate);
    
    test3_conditional_jumps(xtra, x100, calibrate);
    
    test4_integer_arithmetic(xtra, x100, calibrate);
    test5_trig_functions(xtra, x100, calibrate);
    test6_procedure_calls(xtra, x100, calibrate);
    test7_array_references(xtra, x100, calibrate);
    test8_standard_functions(xtra, x100, calibrate);
}

void pa(float e[4], float t, float t2) {
     long j;
     for(j=0;j<6;j++)
        {
           e[0] = (e[0]+e[1]+e[2]-e[3])*t;
           e[1] = (e[0]+e[1]-e[2]+e[3])*t;
           e[2] = (e[0]-e[1]+e[2]+e[3])*t;
           // Check for division by zero
           if (t2 != 0.0f) {
               e[3] = (-e[0]+e[1]+e[2]+e[3])/t2;
           } else {
               e[3] = 0.0f; // Set to zero if division by zero
           }
        }

     return;
}

void po(float e1[4], long j, long k, long l) {
     e1[j] = e1[k];
     e1[k] = e1[l];
     e1[l] = e1[j];
     return;
}

void p3(float *x, float *y, float *z, float t, float t1, float t2) {
     *x = *y;
     *y = *z;
     *x = t * (*x + *y);
     *y = t1 * (*x + *y);
     // Check for division by zero
     if (t2 != 0.0f) {
         *z = (*x + *y)/t2;
     } else {
         *z = 0.0f; // Set to zero if division by zero
     }
     return;
}

void pout(char title[18], float ops, int type, float checknum,
          float time, int calibrate, int section) {
    float mops, mflops;
    char resultLine[40];
    char floatStr[20];
    char perfStr[15];
    int i;
    int perfLen;
    int spacesNeeded;

    Check = Check + checknum;
    loop_time[section] = time;
    TimeUsed = TimeUsed + time;
    
    // Calculate performance metrics
    if (type == 1) {
        if (time > 0) {
            mflops = ops/(1000000L*time);
        } else {
            mflops = 0;
        }
    } else {
        if (time > 0) {
            mops = ops/(1000000L*time);
        } else {
            mops = 0;
        }
    }

    // Format: "N1 Float: 12.3456789MF 0.12s" (32 chars)
    // Hardcoded test names to save memory
    switch(section) {
        case 1: strcpy(resultLine, "N1 Float"); break;
        case 2: strcpy(resultLine, "N2 Float"); break;
        case 3: strcpy(resultLine, "N3 IfElse"); break;
        case 4: strcpy(resultLine, "N4 Int"); break;
        case 5: strcpy(resultLine, "N5 Trig"); break;
        case 6: strcpy(resultLine, "N6 Float"); break;
        case 7: strcpy(resultLine, "N7 Assign"); break;
        case 8: strcpy(resultLine, "N8 Param"); break;
        default: strcpy(resultLine, "Unknown"); break;
    }
    strcat(resultLine, ": ");
    
    // Pad test name to fixed width (8 chars + ": " = 10 chars total)
    while (strlen(resultLine) < 11) {
        strcat(resultLine, " ");
    }
    
    // Add performance with 7 digits precision (14 chars total)
    if (type == 1) {
        // This is a floating point test - show MFLOPS
        floatToString(mflops, floatStr, 7);
        strcpy(perfStr, floatStr);
        strcat(perfStr, "MF");
    } else {
        // This is an integer test - show MOPS
        floatToString(mops, floatStr, 7);
        strcpy(perfStr, floatStr);
        strcat(perfStr, "MO");
    }
    
    // Right-align performance in 14-char field
    perfLen = strlen(perfStr);
    spacesNeeded = 11 - perfLen;
    for (i = 0; i < spacesNeeded; i++) {
        strcat(resultLine, " ");
    }
    strcat(resultLine, perfStr);
    
    // Add time (10 chars total - moved 2 chars right)
    strcat(resultLine, "  ");  // Add 2 spaces for better spacing
    floatToString(loop_time[section], floatStr, 2);
    strcat(resultLine, floatStr);
    strcat(resultLine, "s");
    
    // Pad to 32 chars if needed
    while (strlen(resultLine) < 32) {
        strcat(resultLine, " ");
    }
    
    termM0PrintStringXY(resultLine, (uint8_t)0, (uint8_t)(section + 6));
    return;
}	 