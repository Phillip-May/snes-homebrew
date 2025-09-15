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
// Note: BUILD_TERM and BUILD_ELUA are disabled in platform_conf.h for SNES
#include "snes_memory_manager.h"

/* Function declarations */
int termM0Init(void);
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY);
void scroll_text_up(void);

/* Test function for Lua */
static int lua_test_function(lua_State *L) {
    lua_pushnumber(L, 42);
    return 1;
}
void clear_screen(void);
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
    /* Silent panic handler - just return 0 to indicate error was handled */
    return 0;
}

/* Custom memory allocator for SNES */
void* snes_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
    (void)ud;  /* Not used in this implementation */
    
    if (nsize == 0) {
        /* Free memory */
        if (ptr != NULL) {
            snes_free(ptr);
        }
        return NULL;
    }
    
    if (ptr == NULL) {
        /* Allocate new memory */
        return snes_malloc(nsize);
    } else {
        /* Reallocate existing memory */
        return snes_realloc(ptr, osize, nsize);
    }
}

/* Initialize Lua for SNES */
int lua_snes_init(void) {
    /* Check memory manager before creating Lua state */
    if (!g_mem_manager.pool3_available) {
        return 0;
    }
    
    /* Create Lua state with custom allocator */
    L = lua_newstate(snes_lua_alloc, NULL);
    if (L == NULL) {
        return 0;
    }
    
    /* Set panic handler */
    lua_atpanic(L, lua_panic_handler);
    
    /* Load only essential libraries to reduce output flood */
    luaL_openlibs(L);
    
    /* Disable print function to stop Lua from printing to console */
    lua_pushnil(L);
    lua_setglobal(L, "print");
    
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
    return L;
}

/* Minimal stubs for SNES build */
size_t _Stub_write(int fd, const void *buf, size_t count) { return count; }
size_t _Stub_read(int fd, void *buf, size_t count) { return 0; }
int _Stub_close(int fd) { return 0; }
long _Stub_lseek(int fd, long offset, int whence) { return 0; }
int _Stub_open(const char *pathname, int flags, ...) { return -1; }


/* Disable Lua's internal print functions to reduce console flood */
#undef lua_writestring
#undef lua_writeline
#undef lua_writestringerror

#define lua_writestring(s,l) ((void)0)
#define lua_writeline() ((void)0)
#define lua_writestringerror(s,p) ((void)0)


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


/* Auto-scrolling print function that manages line counter and screen clearing */
int auto_print_string(char *szInput) {
    int8_t regRead1;
    int display_line;
    
    /* Use current_line for display position */
    display_line = current_line;
    
    /* If the line would be off-screen, clear screen and reset to top */
    if (display_line >= max_display_lines) {
        scroll_text_up();
        display_line = 0;  /* Always start from the top of the screen */
        current_line = 0;  /* Reset current_line after clearing */
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
    LoadLoVram((unsigned char *)szInput, 0xF800 + display_line * 0x40, strlen(szInput));
    
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
    snes_memory_init();
    
    /* Check memory manager status */
    if (!g_mem_manager.pool3_available) {
        /* Memory manager not available - just hang silently */
        while(1) {
            do { /* Wait for Vblank */
                regRead1 = REG_RDNMI;
            } while(regRead1 > 0);
        }
    }
    
    termM0Init();    
    clear_screen();  /* Clear screen and initialize scrolling */
    
    /* Print memory pool information */
    if (g_mem_manager.pool3_available) {
        char pool_range[64];
        sprintf(pool_range, "Pool3: %p-%p", 
                g_mem_manager.pool3_start, g_mem_manager.pool3_end);
        auto_print_string(pool_range);
        
        char pool_size[64];
        unsigned long pool_size_bytes = (unsigned long)((uintptr_t)g_mem_manager.pool3_end - (uintptr_t)g_mem_manager.pool3_start);
        unsigned long pool_size_kb = pool_size_bytes / 1024;
        sprintf(pool_size, "Size: %lu KB", pool_size_kb);
        auto_print_string(pool_size);
        
        char current_info[64];
        sprintf(current_info, "Current: %p", g_mem_manager.pool3_current);
        auto_print_string(current_info);
    } else {
        auto_print_string("Pool3: Not available");
    }
    
    REG_INIDISP = 0x0F;
    REG_NMITIMEN = 0x01; /* Enable NMI for frame counting */
    
    /* Initialize Lua silently */
    if (lua_snes_init() == 0) {
        /* Lua init failed - just hang silently */
        while(1) {
            do { /* Wait for Vblank */
                regRead1 = REG_RDNMI;
            } while(regRead1 > 0);
        }
    }
    
    
    /* Wait a few frames to let the clear take effect */
    for (int i = 0; i < 10; i++) {
        do {
            regRead1 = REG_RDNMI;
        } while(regRead1 > 0);
    }
    
    L = lua_snes_getstate();
    
    /* Check Lua state */
    if (L == NULL) {
        /* Lua state is NULL - just hang silently */
        while(1) {
            do { /* Wait for Vblank */
                regRead1 = REG_RDNMI;
            } while(regRead1 > 0);
        }
    }
    
    /* Test 1: Simple arithmetic */
    lua_pushnumber(L, 42);
    if (lua_isnumber(L, -1)) {
        double num = lua_tonumber(L, -1);
        if (num == 42.0) {
            auto_print_string("Test 1: Direct push - OK");
        } else {
            auto_print_string("Test 1: Direct push - FAIL");
        }
        lua_pop(L, 1);
    } else {
        auto_print_string("Test 1: Direct push - FAIL");
    }
    
    /* Test 2: Simple script execution */
    const char* test_script = "return 6*7";
    char script_str[64];
    sprintf(script_str, "Test 2: Script = '%s'", test_script);
    auto_print_string(script_str);
    
    int load_status = luaL_loadstring(L, test_script);
    
    if (load_status == LUA_OK) {
        int call_result = lua_pcall(L, 0, 1, 0);
        if (call_result == LUA_OK) {
            if (lua_isnumber(L, -1)) {
                double result = lua_tonumber(L, -1);
                char result_str[64];
                sprintf(result_str, "Test 2: Script result = %.0f", result);
                auto_print_string(result_str);
                
                if (result == 42.0) {
                    auto_print_string("Test 2: Script execution - OK");
                } else {
                    auto_print_string("Test 2: Script execution - FAIL");
                }
            } else {
                auto_print_string("Test 2: Script execution - FAIL");
            }
            lua_pop(L, 1);
        } else {
            auto_print_string("Test 2: Script execution - FAIL");
        }
    } else {
        auto_print_string("Test 2: Script loading - FAIL");
    }
    
    /* Test 2.5: Check stack status */
    int stack_size = lua_gettop(L);
    if (stack_size == 0) {
        /* Stack is clean - good */
    } else {
        /* Stack not clean - clear it */
        lua_settop(L, 0);
    }
    
    /* Test 3: Custom function */
    lua_pushcfunction(L, lua_test_function);
    int call_result = lua_pcall(L, 0, 1, 0);
    if (call_result == LUA_OK) {
        double result = lua_tonumber(L, -1);
        if (result == 42.0) {
            auto_print_string("Test 3: Custom function - OK");
        } else {
            auto_print_string("Test 3: Custom function - FAIL");
        }
        lua_pop(L, 1);
    } else {
        auto_print_string("Test 3: Custom function - FAIL");
    }
    
    /* Ensure stack is clean */
    lua_settop(L, 0);
    
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
/ /   T e s t   c o m m e n t 
 
 