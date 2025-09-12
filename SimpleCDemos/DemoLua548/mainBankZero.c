#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <stdarg.h>

/* Assert stub for SNES - just ignore failed assertions in release builds */
void _Stub_assert(const char *filename, int linenum) {
    /* In a real implementation, you might want to print an error message
     * or halt execution, but for SNES homebrew we'll just ignore it */
    (void)filename;
    (void)linenum;
}

/* File system stubs for SNES - I/O not supported */
int _Stub_remove(const char *pathname) {
    /* File operations not supported on SNES */
    (void)pathname;
    return -1;  /* Indicate failure */
}

int _Stub_rename(const char *oldpath, const char *newpath) {
    /* File operations not supported on SNES */
    (void)oldpath;
    (void)newpath;
    return -1;  /* Indicate failure */
}

char **_Stub_environ(void) {
    /* Environment variables not supported on SNES */
    return NULL;
}

int system(const char *command) {
    /* System commands not supported on SNES */
    (void)command;
    return -1;
}

#include "snes_regs_xc.h"
#include "include/imagedata.h"
#include "initsnes.h"
/* Use eLua 0.9 implementation for SNES */
#include "elua-0.9/src/lua/lua.h"
#include "elua-0.9/src/lua/lstate.h"
#include "elua-0.9/src/lua/lauxlib.h"
#include "elua-0.9/src/lua/lualib.h"
#include "snes_memory_manager.h"

// Define eLua platform configuration
#define PLATFORM_SNES
#define BUILD_TERM
#define BUILD_ELUA
#include "snes_memory_manager.h"

/* Function declarations */
int termM0Init(void);
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY);
void scroll_text_up(void);
void clear_screen(void);
int termM0PrintStringXY_scroll(char *szInput, uint8_t inpX, uint8_t inpY);
void write_debug_info(const char* info);
void write_debug_char(char c, uint8_t line);


/* Lua function declarations */
// lua_State typedef is already defined in lua.h


/* Function declarations */

/* Lua constants */
#define LUA_OK 0

/* Define missing symbols for SNES */
const char luai_ctype_[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

/* Lua state for SNES */
static lua_State *L = NULL;

/* Text scrolling system - declare current_line early */
extern int current_line;

/* Debug variables for luaM_growaux_ */

/* Lua panic handler to catch errors instead of calling abort() */
int lua_panic_handler(lua_State *L) {
    termM0PrintStringXY_scroll("LUA PANIC!", 0, current_line);
    termM0PrintStringXY_scroll("Error caught by panic handler", 0, current_line);
    return 0;  /* Return 0 to indicate error was handled */
}

/* Custom memory allocator for SNES */
void* snes_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
    (void)ud;  /* Not used in this implementation */
    
    /* Debug: show ALL allocator calls to track the hang */
    static int call_count = 0;
    call_count++;
    
    /* Always show call count for debugging */
    char count_msg[16];
    count_msg[0] = 'C';
    count_msg[1] = 'a';
    count_msg[2] = 'l';
    count_msg[3] = 'l';
    count_msg[4] = ' ';
    count_msg[5] = '0' + (call_count / 10);
    count_msg[6] = '0' + (call_count % 10);
    count_msg[7] = '\0';
    termM0PrintStringXY_scroll(count_msg, 0, current_line);
    
    /* Show allocation info for debugging */
    if (call_count <= 20) {
        char debug_msg[32];
        debug_msg[0] = 'A';
        debug_msg[1] = 'l';
        debug_msg[2] = 'l'; 
        debug_msg[3] = 'o';
        debug_msg[4] = 'c';
        debug_msg[5] = ' ';
        debug_msg[6] = '0' + (call_count % 10);
        debug_msg[7] = '\0';
        termM0PrintStringXY_scroll(debug_msg, 0, current_line);
        
        /* Show allocation size for debugging */
        if (nsize > 0) {
            char size_msg[16];
            size_msg[0] = 'S';
            size_msg[1] = 'z';
            size_msg[2] = ':';
            size_msg[3] = '0' + (nsize % 10);
            size_msg[4] = '\0';
            termM0PrintStringXY_scroll(size_msg, 0, current_line);
        }
    }
    
    /* Check for stack corruption - verify current_line is reasonable */
    if (current_line > 50) {
        termM0PrintStringXY_scroll("STACK CORRUPT!", 0, current_line);
        return NULL;
    }
    
    /* Check heap state before allocation */
    if (call_count == 3) {
        termM0PrintStringXY_scroll("HEAP CHECK", 0, current_line);
        if (!g_mem_manager.pool3_available) {
            termM0PrintStringXY_scroll("HEAP CORRUPT!", 0, current_line);
            return NULL;
        }
    }
    
    if (nsize == 0) {
        /* Free memory */
        if (ptr != NULL) {
            snes_free(ptr);
        }
        return NULL;
    }
    
    if (ptr == NULL) {
        /* Allocate new memory */
        termM0PrintStringXY_scroll("MALLOC", 0, current_line);
        void* result = snes_malloc(nsize);
        if (result == NULL) {
            termM0PrintStringXY_scroll("MALLOC FAIL!", 0, current_line);
        } else {
            termM0PrintStringXY_scroll("MALLOC OK", 0, current_line);
            /* Verify the allocated memory is valid - check SA-1 SRAM range */
            if (result < (void*)0x400000 || result > (void*)0x500000) {
                termM0PrintStringXY_scroll("BAD PTR!", 0, current_line);
                /* Show a simple indicator of where the pointer is */
                unsigned long ptr_val = (unsigned long)(uintptr_t)result;
                if (ptr_val < 0x400000) {
                    termM0PrintStringXY_scroll("LOW MEM", 0, current_line);
                } else if (ptr_val < 0x7E0000) {
                    termM0PrintStringXY_scroll("MID MEM", 0, current_line);
                } else if (ptr_val < 0x800000) {
                    termM0PrintStringXY_scroll("WRAM", 0, current_line);
                } else {
                    termM0PrintStringXY_scroll("HIGH MEM", 0, current_line);
                }
            }
        }
        return result;
    } else {
        /* Reallocate existing memory */
        termM0PrintStringXY_scroll("REALLOC", 0, current_line);
        void* result = snes_realloc(ptr, osize, nsize);
        if (result == NULL) {
            termM0PrintStringXY_scroll("REALLOC FAIL!", 0, current_line);
        } else {
            termM0PrintStringXY_scroll("REALLOC OK", 0, current_line);
            /* Verify the reallocated memory is valid - check SA-1 SRAM range */
            if (result < (void*)0x400000 || result > (void*)0x500000) {
                termM0PrintStringXY_scroll("BAD PTR!", 0, current_line);
                /* Show a simple indicator of where the pointer is */
                unsigned long ptr_val = (unsigned long)(uintptr_t)result;
                if (ptr_val < 0x400000) {
                    termM0PrintStringXY_scroll("LOW MEM", 0, current_line);
                } else if (ptr_val < 0x7E0000) {
                    termM0PrintStringXY_scroll("MID MEM", 0, current_line);
                } else if (ptr_val < 0x800000) {
                    termM0PrintStringXY_scroll("WRAM", 0, current_line);
                } else {
                    termM0PrintStringXY_scroll("HIGH MEM", 0, current_line);
                }
            }
        }
        return result;
    }
}

/* Initialize Lua for SNES */
int lua_snes_init(void) {
    /* Initialize eLua 0.9 for SNES */
    termM0PrintStringXY_scroll("Using eLua 0.9", 0, current_line);
    
    /* Check memory manager before creating Lua state */
    if (!g_mem_manager.pool3_available) {
        termM0PrintStringXY_scroll("No memory available!", 0, current_line);
        return 0;
    }
    
    /* Create Lua state with custom allocator */
    termM0PrintStringXY_scroll("Creating Lua state...", 0, current_line);
    termM0PrintStringXY_scroll("Calling lua_newstate...", 0, current_line);
    
    /* Try to create Lua state - this might hang */
    termM0PrintStringXY_scroll("About to call lua_newstate...", 0, current_line);
    
    /* The hang is likely in lua_newstate initialization */
    /* Let's try a different approach - create a minimal state */
    termM0PrintStringXY_scroll("Trying minimal approach...", 0, current_line);
    
    /* Try to create the basic state structure manually */
    /* Add a timeout mechanism to prevent infinite hanging */
    volatile int timeout_counter = 0;
    
    /* Try to create Lua state with debug tracing */
    termM0PrintStringXY_scroll("Calling lua_newstate...", 0, current_line);
    L = lua_newstate(snes_lua_alloc, NULL);
    
    /* If we reach here, lua_newstate completed */
    termM0PrintStringXY_scroll("lua_newstate completed!", 0, current_line);
    
    /* Set panic handler */
    lua_atpanic(L, lua_panic_handler);
    
    /* Print Lua version - use simple string to avoid sprintf issues */
    termM0PrintStringXY_scroll("Lua 5.1.4", 0, current_line);
    
    /* Skip standard libraries for now - they may require full state initialization */
    termM0PrintStringXY_scroll("Skipping std libs...", 0, current_line);
    termM0PrintStringXY_scroll("eLua ready", 0, current_line);
    
    return 1;
}

/* Execute Lua string */
int lua_snes_dostring(lua_State *L, const char *script) {
    int status = luaL_loadstring(L, script);
    if (status == LUA_OK) {
        status = lua_pcall(L, 0, LUA_MULTRET, 0);  /* Allow multiple return values */
    }
    return status;
}

/* Execute Lua file */
int lua_snes_dofile(lua_State *L, const char *filename) {
    /* For now, just execute the benchmark script directly */
    if (strcmp(filename, "benchmark.lua") == 0) {
        const char *benchmark_script = 
            "-- Simple benchmark to avoid opcode limit\n"
            "local start = 0\n"
            "local sum = 0\n"
            "for i = 1, 1000 do\n"
            "  sum = sum + i\n"
            "end\n"
            "print('Sum 1 to 1000: ' .. sum)\n"
            "print('Benchmark complete!')\n";
        
        return lua_snes_dostring(L, benchmark_script);
    }
    return LUA_ERRFILE;
}

/* Close Lua state */
void lua_snes_close(void) {
    if (L != NULL) {
        lua_close(L);
        L = NULL;
    }
}

/* Get Lua state */
lua_State* lua_snes_getstate(void) {
    termM0PrintStringXY_scroll("Getting Lua state...", 0, current_line);
    return L;
}

/* Minimal stubs for SNES build */
size_t _Stub_write(int fd, const void *buf, size_t count) { return count; }
size_t _Stub_read(int fd, void *buf, size_t count) { return 0; }
int _Stub_close(int fd) { return 0; }
long _Stub_lseek(int fd, long offset, int whence) { return 0; }
int _Stub_open(const char *pathname, int flags, ...) { return -1; }


#undef lua_writestring
#undef lua_writeline
#undef lua_writestringerror

void lua_writestring(const char *str, size_t len) { (void)str; (void)len; }
void lua_writeline(void) { }
void lua_writestringerror(const char *str, const char *param) { (void)str; (void)param; }

/* Minimal Lua operation demonstration */
void demonstrate_lua_operation(lua_State *L, const char* operation, const char* description, int line) {
    /* Display the operation description */
    termM0PrintStringXY_scroll((char*)description, 0, line);
    
    /* Execute the Lua operation */
    termM0PrintStringXY_scroll("Executing...", 0, line + 1);
    int status = lua_snes_dostring(L, operation);
    
    /* Show status */
    if (status == LUA_OK) {
        termM0PrintStringXY_scroll("Status: OK", 0, line + 2);
    } else {
        termM0PrintStringXY_scroll("Status: FAIL", 0, line + 2);
    }
    
    /* Check stack size */
    int stack_size = lua_gettop(L);
    char stack_msg[32];
    stack_msg[0] = 'S';
    stack_msg[1] = ':';
    if (stack_size < 10) {
        stack_msg[2] = '0' + stack_size;
        stack_msg[3] = '\0';
    } else {
        stack_msg[2] = '1';
        stack_msg[3] = '0';
        stack_msg[4] = '+';
        stack_msg[5] = '\0';
    }
    termM0PrintStringXY_scroll(stack_msg, 0, line + 3);
    
    if (status == LUA_OK) {
        /* Check if there's a result on the stack to display */
        if (lua_gettop(L) > 0) {
            /* Get the result and display it */
            if (lua_isnumber(L, -1)) {
                double result = lua_tonumber(L, -1);
                termM0PrintStringXY_scroll("Type: number", 0, line + 4);
                
                /* Simple number display without sprintf */
                char result_str[32];
                result_str[0] = 'R';
                result_str[1] = ':';
                if (result < 10) {
                    result_str[2] = '0' + (int)result;
                    result_str[3] = '\0';
                } else if (result < 100) {
                    result_str[2] = '0' + ((int)result / 10);
                    result_str[3] = '0' + ((int)result % 10);
                    result_str[4] = '\0';
                } else {
                    result_str[2] = '1';
                    result_str[3] = '0';
                    result_str[4] = '0';
                    result_str[5] = '+';
                    result_str[6] = '\0';
                }
                termM0PrintStringXY_scroll(result_str, 0, line + 5);
            } else if (lua_isstring(L, -1)) {
                termM0PrintStringXY_scroll("Type: string", 0, line + 4);
                const char* result = lua_tostring(L, -1);
                termM0PrintStringXY_scroll("String result", 0, line + 5);
            } else {
                termM0PrintStringXY_scroll("Type: other", 0, line + 4);
                termM0PrintStringXY_scroll("-> OK", 0, line + 5);
            }
            /* Pop the result from the stack */
            lua_pop(L, 1);
        } else {
            termM0PrintStringXY_scroll("No result", 0, line + 4);
            termM0PrintStringXY_scroll("-> OK", 0, line + 5);
        }
    } else {
        termM0PrintStringXY_scroll("-> FAIL", 0, line + 4);
        
        /* Show error message if available */
        if (lua_isstring(L, -1)) {
            const char* error_msg = lua_tostring(L, -1);
            termM0PrintStringXY_scroll("Error:", 0, line + 5);
            termM0PrintStringXY_scroll((char*)error_msg, 0, line + 6);
        } else {
            termM0PrintStringXY_scroll("Unknown error", 0, line + 5);
        }
        
        /* Pop the error message from the stack */
        lua_pop(L, 1);
    }
}

/* SNES timing system */
volatile uint32_t frameCounter = 0;

/* Text scrolling system */
static int current_scroll_line = 0;
static int max_display_lines = 20;  /* Maximum lines visible on screen */
static int total_lines = 0;         /* Total lines printed */
int current_line = 0;        /* Current line being printed on */

/* Debug output for Mesen 2 Lua script */
static char debug_buffer[256] = {0};
static int debug_buffer_pos = 0;

/* Debug register for Lua script monitoring */
/* Use very end of WRAM for debug registers - safe area */
#define DEBUG_CHAR_ADDR   0x7FFF00
#define DEBUG_LINE_ADDR   0x7FFF01
#define DEBUG_READY_ADDR  0x7FFF02

/* Function to write a single character to debug register */
void write_debug_char(char c, uint8_t line) {
    /* Write the character data */
    *(volatile uint8_t*)DEBUG_CHAR_ADDR = (uint8_t)c;
    *(volatile uint8_t*)DEBUG_LINE_ADDR = line;
    *(volatile uint8_t*)DEBUG_READY_ADDR = 1;  /* Signal that new data is available */
    
    /* Longer delay to give Lua script time to process */
    volatile int delay = 0;
    while (delay < 5000) {
        delay++;
    }
}

/* Function to write debug info to a specific memory location */
void write_debug_info(const char* info) {
    /* Write to a specific memory location that Mesen 2 can easily monitor */
    /* We'll use a location in SNES RAM that's easy to find */
    volatile char* debug_addr = (volatile char*)0x7E1000;
    
    /* Simple string copy to debug location */
    int i = 0;
    while (info[i] != '\0' && i < 255) {
        debug_addr[i] = info[i];
        i++;
    }
    debug_addr[i] = '\0';
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


/* Clear the entire screen */
void clear_screen(void) {
    static const unsigned char BGCLEAR[] = {0x20, 0x00};
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
    
    /* Write each character to debug register for Lua script monitoring */
    int i = 0;
    while (szInput[i] != '\0') {
        write_debug_char(szInput[i], display_line);
        i++;
    }
    /* Send null terminator to signal end of string for Lua script */
    write_debug_char('\0', display_line);
    
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


int main(void) {
    int8_t regRead1; /* Variable for storing hardware registers */
    
    /* Initialization */
    initSNES(SLOWROM);
    
    /* Initialize SA1 BW-RAM mapping for F0:0000-FF:FFFF */
    initSA1();
    
    /* Initialize custom memory manager */
    termM0PrintStringXY_scroll("Initializing memory manager...", 0, current_line);
    snes_memory_init();
    
    /* Check if memory manager is working */
    if (g_mem_manager.pool3_available) {
        termM0PrintStringXY_scroll("SA-1 SRAM: OK", 0, current_line);
    } else {
        termM0PrintStringXY_scroll("SA-1 SRAM: FAIL", 0, current_line);
    }
    
    termM0PrintStringXY_scroll("Memory manager ready", 0, current_line);
    
    /* Test memory allocation */
    void* test_ptr = snes_malloc(1024);
    if (test_ptr != NULL) {
        termM0PrintStringXY_scroll("Test alloc: OK", 0, current_line);
        snes_free(test_ptr);
    } else {
        termM0PrintStringXY_scroll("Test alloc: FAIL", 0, current_line);
    }
    
    termM0Init();
    clear_screen();  /* Clear screen and initialize scrolling */
    
    /* Display header first */
    termM0PrintStringXY_scroll("eLua 0.9 Demo for SNES",0,1);
    termM0PrintStringXY_scroll("Compiler:",0,2);
    termM0PrintStringXY_scroll(SNES_XC_COMPILER_NAME,20,2);
    REG_INIDISP = 0x0F;
    REG_NMITIMEN = 0x01; /* Enable NMI for frame counting */
    
    current_line = 3;  // Start Lua tests after header
    
    /* Initialize Lua */
    termM0PrintStringXY_scroll("Initializing Lua...",0,current_line);
    termM0PrintStringXY_scroll("About to call lua_snes_init...",0,current_line);
    
    /* Add debug before the call */
    termM0PrintStringXY_scroll("Calling lua_snes_init now",0,current_line);
    
    if (lua_snes_init() == 0) {
        termM0PrintStringXY_scroll("Lua init failed!",0,4);
        while(1) {
            do { /* Wait for Vblank */
                regRead1 = REG_RDNMI;
            } while(regRead1 > 0);
        }
    }
    
    /* Add debug after the call */
    termM0PrintStringXY_scroll("lua_snes_init completed",0,current_line);
    termM0PrintStringXY_scroll("About to get Lua state...",0,current_line);
    
    L = lua_snes_getstate();
    termM0PrintStringXY_scroll("Got Lua state from getter",0,current_line);
    termM0PrintStringXY_scroll("About to start Lua tests...",0,current_line);
    
    /* Run basic Lua test - just test core functionality */
    termM0PrintStringXY_scroll("eLua 0.9 Basic Test:",0,4);
    
    /* Test 1: Basic arithmetic - use return to get result on stack */
    termM0PrintStringXY_scroll("1. Basic Arithmetic:",0,5);
    demonstrate_lua_operation(L, "return 5 + 3", "5 + 3 = ?", 6);
    
    /* Test 2: String operations - use return to get result on stack */
    termM0PrintStringXY_scroll("2. String Operations:",0,8);
    demonstrate_lua_operation(L, "return 'Hello Lua!'", "String test", 9);
    
    /* Test 3: Table operations - use return to get result on stack */
    termM0PrintStringXY_scroll("3. Table Operations:",0,11);
    demonstrate_lua_operation(L, "t = {x = 10, y = 20}; return t.x + t.y", "Table sum", 12);
    
    /* Test 4: Function definition and call - use return to get result on stack */
    termM0PrintStringXY_scroll("4. Function Definitions:",0,15);
    demonstrate_lua_operation(L, "function square(n) return n * n end; return square(4)", "Square function", 16);
    
    /* Test 5: Control structures - use return to get result on stack */
    termM0PrintStringXY_scroll("5. Control Structures:",0,19);
    demonstrate_lua_operation(L, "sum = 0; for i = 1, 10 do sum = sum + i end; return sum", "For loop sum", 20);
    
    /* Test 6: Global variable access - test that assignments work */
    termM0PrintStringXY_scroll("6. Global Variables:",0,22);
    demonstrate_lua_operation(L, "result = 5 + 3", "Assign result = 5+3", 23);
    
    /* Now fetch the global variable to show it was set */
    termM0PrintStringXY_scroll("Fetching global 'result':",0,24);
    lua_getglobal(L, "result");
    if (lua_isnumber(L, -1)) {
        int result_value = (int)lua_tonumber(L, -1);
        char result_msg[32];
        result_msg[0] = 'G';
        result_msg[1] = 'l';
        result_msg[2] = 'o';
        result_msg[3] = 'b';
        result_msg[4] = 'a';
        result_msg[5] = 'l';
        result_msg[6] = ':';
        result_msg[7] = ' ';
        result_msg[8] = '0' + result_value;
        result_msg[9] = '\0';
        termM0PrintStringXY_scroll(result_msg, 0, 25);
    } else {
        termM0PrintStringXY_scroll("Global not found!", 0, 25);
    }
    lua_pop(L, 1);  /* Remove the value from stack */
    
    termM0PrintStringXY_scroll("Lua tests complete!",0,current_line);
    
    /* Main loop - display results */
    while(1) {
        do { /* Wait for Vblank */
            regRead1 = REG_RDNMI;
        } while(regRead1 > 0);
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
	static const unsigned char BGPAL[] = {0x00,0x00,0xFF,0x7F, 0x00, 0x00, 0x00, 0x00};
	static const unsigned char BGCLEAR[] = {0x20, 0x00};
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
