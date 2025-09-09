#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "snes_regs_xc.h"
#include "include/imagedata.h"
#include "initsnes.h"
/* Lua functions are in lua_standalone.c */
#include "test_scripts.h"

/* External declarations for dhry.c functions */
extern int dhrystone_main(int number_of_runs);

/* Lua function declarations */
typedef struct lua_State lua_State;
int lua_snes_init(void);
int lua_snes_dostring(lua_State *L, const char *script);
int lua_snes_dofile(lua_State *L, const char *filename);
void lua_snes_close(void);
lua_State* lua_snes_getstate(void);

/* Lua constants */
#define LUA_OK 0

/* Function to demonstrate individual Lua operations with results */
void demonstrate_lua_operation(lua_State *L, const char* operation, const char* description, int line) {
    char result_buffer[64];
    
    /* Display the operation description */
    termM0PrintStringXY_scroll(description, 0, line);
    
    /* Execute the Lua operation */
    if (lua_snes_dostring(L, operation) == LUA_OK) {
        /* Show success message */
        strcpy(result_buffer, "  -> Success");
        termM0PrintStringXY_scroll(result_buffer, 0, line + 1);
    } else {
        /* Show failure message */
        strcpy(result_buffer, "  -> Failed");
        termM0PrintStringXY_scroll(result_buffer, 0, line + 1);
    }
}

/* Function declarations */
int termM0Init(void);
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY);
void scroll_text_up(void);
void clear_screen(void);
int termM0PrintStringXY_scroll(char *szInput, uint8_t inpX, uint8_t inpY);

/* External functions required by dhry.c */
uint32_t snesTimeFrames(void);
void snesPrintResults(uint32_t loops, uint32_t time_used);

/* SNES timing system */
volatile uint32_t frameCounter = 0;

/* Text scrolling system */
static int current_scroll_line = 0;
static int max_display_lines = 20;  /* Maximum lines visible on screen */
static int total_lines = 0;         /* Total lines printed */
static int current_line = 0;        /* Current line being printed on */

/* Memory usage tracking */
static uint32_t stack_start = 0;
static uint32_t heap_start = 0;

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

/* Clear the entire screen */
void clear_screen(void) {
    const static unsigned char BGCLEAR[] = {0x20, 0x00};
    ClearVram(BGCLEAR, 0xF800, 0x400); /* Clear VRAM Map To Fixed Tile Word */
    current_scroll_line = 0;
    total_lines = 0;
    current_line = 0;
}

/* Clear entire screen and reset to top */
void scroll_text_up(void) {
    int i;
    char space_line[33];  /* 32 spaces + null terminator */
    
    /* Create a string of 32 spaces */
    for (i = 0; i < 32; i++) {
        space_line[i] = ' ';
    }
    space_line[32] = '\0';
    
    /* Clear entire screen with spaces using termM0PrintStringXY */
    for (i = 0; i < max_display_lines; i++) {
        termM0PrintStringXY(space_line, 0, i);
    }
    
    /* Reset scroll position to top */
    current_scroll_line = 0;
    current_line = 0;
}

/* Print string with automatic scrolling */
int termM0PrintStringXY_scroll(char *szInput, uint8_t inpX, uint8_t inpY) {
    int8_t regRead1;
    int display_line;
    
    /* Use current_line for display position */
    display_line = current_line;
    
    /* If the line would be off-screen, clear screen and reset to top */
    if (display_line >= max_display_lines) {
        scroll_text_up();
        display_line = 0;  /* Always start from the top of the screen */
    }
    
    /* Wait for VBlank */
    do {
        regRead1 = REG_RDNMI;
    } while(regRead1 > 0);
    do {
        regRead1 = REG_RDNMI;
    } while(regRead1 > 0);
    
    /* Print the text at the calculated position */
    LoadLoVram((unsigned char *)szInput, 0xF800 + inpX + display_line * 0x40, strlen(szInput));
    
    /* Wait for VBlank again */
    do {
        regRead1 = REG_RDNMI;
    } while(regRead1 > 0);
    
    /* Update current line for next print */
    current_line++;
    
    total_lines++;
    return 0;
}

/* Calculate and display memory usage */
void display_memory_usage(void) {
    char mem_buffer[64];
    uint32_t stack_usage, heap_usage, total_usage;
    uint32_t stack_ptr, heap_ptr;
    
    /* Get current stack pointer (approximate) - use a simple approach */
    stack_ptr = (uint32_t)&stack_ptr;  /* Use local variable address as stack approximation */
    
    /* Calculate stack usage */
    if (stack_ptr < stack_start) {
        stack_usage = stack_start - stack_ptr;
    } else {
        stack_usage = 0;
    }
    
    /* Calculate heap usage (approximate) */
    heap_ptr = (uint32_t)&heap_start;
    if (heap_ptr > heap_start) {
        heap_usage = heap_ptr - heap_start;
    } else {
        heap_usage = 0;
    }
    
    total_usage = stack_usage + heap_usage;
    
    /* Display memory usage */
    sprintf(mem_buffer, "Stack: %lu bytes", (unsigned long)stack_usage);
    termM0PrintStringXY_scroll(mem_buffer, 0, current_line);
    
    sprintf(mem_buffer, "Heap: %lu bytes", (unsigned long)heap_usage);
    termM0PrintStringXY_scroll(mem_buffer, 0, current_line);
    
    sprintf(mem_buffer, "Total: %lu bytes", (unsigned long)total_usage);
    termM0PrintStringXY_scroll(mem_buffer, 0, current_line);
}

int main(void) {
    int8_t regRead1; /* Variable for storing hardware registers */
    int i;
    lua_State *L;
    
    /* Clear problematic memory area that C runtime might be using */
    volatile uint8_t *mem = (volatile uint8_t *)0x0200;
    for (i = 0; i < 0x1C00; i++) {
        mem[i] = 0;
    }
    
    /* Initialize memory tracking */
    stack_start = (uint32_t)&stack_start;  /* Use local variable address as stack approximation */
    heap_start = (uint32_t)&heap_start;
    
    /* Initialization */
    initSNES(SLOWROM);
    
    termM0Init();
    clear_screen();  /* Clear screen and initialize scrolling */
    termM0PrintStringXY_scroll("Lua 5.4.8 Demo for SNES",0,1);
    termM0PrintStringXY_scroll("Compiler:",0,2);
    termM0PrintStringXY_scroll(SNES_XC_COMPILER_NAME,20,2);
    REG_INIDISP = 0x0F;
    REG_NMITIMEN = 0x81; /* Enable NMI for frame counting */
    
    /* Initialize Lua */
    termM0PrintStringXY_scroll("Initializing Lua...",0,3);
    if (lua_snes_init() == 0) {
        termM0PrintStringXY_scroll("Lua init failed!",0,4);
        while(1) {
            do { /* Wait for Vblank */
                regRead1 = REG_RDNMI;
            } while(regRead1 > 0);
        }
    }
    L = lua_snes_getstate();
    
    /* Run Lua test scripts with step-by-step results */
    termM0PrintStringXY_scroll("Lua 5.4.8 Demo Results:",0,4);
    
    /* Test 1: Basic arithmetic - show step by step */
    termM0PrintStringXY_scroll("1. Arithmetic Operations:",0,5);
    demonstrate_lua_operation(L, "result1 = snes.add(5, 3)", "  snes.add(5, 3) -> 8", 6);
    demonstrate_lua_operation(L, "result2 = snes.multiply(4, 6)", "  snes.multiply(4, 6) -> 24", 8);
    demonstrate_lua_operation(L, "sum = result1 + result2", "  sum = 8 + 24 -> 32", 10);
    
    /* Test 2: String operations */
    termM0PrintStringXY_scroll("2. String Operations:",0,12);
    demonstrate_lua_operation(L, "message = 'Hello from Lua!'", "  message = 'Hello from Lua!'", 13);
    demonstrate_lua_operation(L, "length = string.len(message)", "  string.len(message) -> 18", 15);
    
    /* Test 3: Table operations */
    termM0PrintStringXY_scroll("3. Table Operations:",0,17);
    demonstrate_lua_operation(L, "t = {x = 10, y = 20}", "  t = {x = 10, y = 20}", 18);
    demonstrate_lua_operation(L, "sum = t.x + t.y", "  t.x + t.y -> 30", 20);
    demonstrate_lua_operation(L, "t.z = 30; total = t.x + t.y + t.z", "  total = 10 + 20 + 30 -> 60", 22);
    
    /* Test 4: Function definition and call */
    termM0PrintStringXY_scroll("4. Function Definitions:",0,24);
    demonstrate_lua_operation(L, "function square(n) return n * n end", "  function square(n) defined", 25);
    demonstrate_lua_operation(L, "result = square(4)", "  square(4) -> 16", 27);
    demonstrate_lua_operation(L, "function fib(n) if n<=1 then return n else return fib(n-1)+fib(n-2) end end", "  function fib(n) defined", 29);
    demonstrate_lua_operation(L, "result2 = fib(6)", "  fib(6) -> 8", 31);
    
    /* Test 5: Control structures */
    termM0PrintStringXY_scroll("5. Control Structures:",0,33);
    demonstrate_lua_operation(L, "sum = 0; for i = 1, 10 do sum = sum + i end", "  sum 1 to 10 -> 55", 34);
    demonstrate_lua_operation(L, "if sum > 50 then msg = 'Sum > 50' else msg = 'Sum <= 50' end", "  if sum > 50 -> 'Sum > 50'", 36);
    
    /* Test 6: Math library */
    termM0PrintStringXY_scroll("6. Math Library:",0,38);
    demonstrate_lua_operation(L, "pi = math.pi", "  math.pi -> 3.14159", 39);
    demonstrate_lua_operation(L, "sqrt_val = math.sqrt(16)", "  math.sqrt(16) -> 4", 41);
    demonstrate_lua_operation(L, "floor_val = math.floor(3.7)", "  math.floor(3.7) -> 3", 43);
    
    /* Test 7: Performance benchmark */
    termM0PrintStringXY_scroll("7. Performance Benchmark:",0,current_line);
    termM0PrintStringXY_scroll("Running object creation test...",0,current_line);
    
    /* Test io.write first */
    termM0PrintStringXY_scroll("Testing io.write...",0,current_line);
    lua_snes_dostring(L, "io.write('TEST: io.write is working!\\n')");
    
    /* Run the benchmark from file */
    termM0PrintStringXY_scroll("Loading benchmark.lua...",0,current_line);
    if (lua_snes_dofile(L, "benchmark.lua") == LUA_OK) {
        termM0PrintStringXY_scroll("  Benchmark completed successfully",0,current_line);
    } else {
        termM0PrintStringXY_scroll("  Benchmark failed to run",0,current_line);
    }
    
    termM0PrintStringXY_scroll("All Lua operations complete!",0,current_line);
    
    /* Display memory usage after Lua tests */
    termM0PrintStringXY_scroll("Memory Usage:",0,current_line);
    display_memory_usage();
    
    /* Main loop - display results */
    while(1) {
        do { /* Wait for Vblank */
            regRead1 = REG_RDNMI;
        } while(regRead1 > 0);
        termM0PrintStringXY_scroll("LUA DEMO RUNNING        ",0,current_line);
        
        /* Display memory usage every frame */
        display_memory_usage();
    }
    
    /* Clean up Lua (never reached due to infinite loop) */
    lua_snes_close();
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
