#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>

/* Assert stub for SNES - just ignore failed assertions in release builds */
void _Stub_assert(const char *filename, int linenum) {
    /* In a real implementation, you might want to print an error message
     * or halt execution, but for SNES homebrew we'll just ignore it */
    (void)filename;
    (void)linenum;
}

#include "snes_regs_xc.h"
#include "include/imagedata.h"
#include "initsnes.h"
/* SNES Lua config is now integrated into luaconf.h */
#include "lua-5.4.8/lua.h"
#include "lua-5.4.8/lauxlib.h"
#include "snes_memory_manager.h"

/* Function declarations */
int termM0Init(void);
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY);
void scroll_text_up(void);
void clear_screen(void);
int termM0PrintStringXY_scroll(char *szInput, uint8_t inpX, uint8_t inpY);
void test_memory_manager(void);


/* Lua function declarations */
// lua_State typedef is already defined in lua.h


/* Function declarations */
void* lua_alloc_debug(void* ud, void* ptr, size_t osize, size_t nsize);

/* Lua allocator function using only pool3 (SA-1 SRAM) */
void* lua_alloc_debug(void *ud, void *ptr, size_t osize, size_t nsize) {
    static int alloc_count = 0;
    char debug_buffer[128];
    void *result;
    extern int current_line;  // Declare external reference to current_line
    
    (void)ud;  // unused parameter
    alloc_count++;
    
    /* Log ALL allocation attempts for debugging */
    if (nsize == 0) {
        sprintf(debug_buffer, "Lua #%d: FREE %u", alloc_count, (unsigned int)osize);
        termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    } else if (ptr == NULL) {
        sprintf(debug_buffer, "Lua #%d: MALLOC %u", alloc_count, (unsigned int)nsize);
        termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    } else {
        sprintf(debug_buffer, "Lua #%d: REALLOC %u->%u", alloc_count, (unsigned int)osize, (unsigned int)nsize);
        termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    }
    
    /* Check if pool3 is available */
    if (!g_mem_manager.pool3_available) {
        sprintf(debug_buffer, "Lua #%d: Pool3 not available!", alloc_count);
        termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
        return NULL;
    }
    
    if (nsize == 0) {
        if (ptr != NULL) {
            snes_free(ptr);
        }
        return NULL;
    }
    
    /* Use pool3-specific allocation */
    if (ptr == NULL) {
        result = snes_malloc_pool3(nsize);
    } else {
        result = snes_realloc_pool3(ptr, osize, nsize);
    }
    
    /* Log ALL allocation results for debugging */
    if (result != NULL) {
        sprintf(debug_buffer, "Lua #%d: OK", alloc_count);
        termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    } else {
        sprintf(debug_buffer, "Lua #%d: FAILED!", alloc_count);
        termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
        /* Show available pool3 memory when allocation fails */
        uint32_t pool3_free = g_mem_manager.pool3_end - g_mem_manager.pool3_current;
        sprintf(debug_buffer, "Pool3 free: %u bytes", (unsigned int)pool3_free);
        termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    }
    
    return result;
}

/* Lua constants */
#define LUA_OK 0

/* Define missing symbols for SNES */
const char luai_ctype_[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

/* Lua state for SNES */
static lua_State *L = NULL;

/* Text scrolling system - declare current_line early */
extern int current_line;

/* Debug variables for luaM_growaux_ */

/* Define luaL_openlibs for minimal SNES Lua */
void luaL_openlibs(lua_State *L) {
    /* Skip all standard libraries for now - just create a minimal Lua state */
    /* This avoids any filesystem access issues */
    /* We'll add our own minimal functions instead of loading standard libraries */
    /* This avoids any filesystem access that might cause stp instructions */
}

/* SNES-specific Lua functions - ultra minimal versions */
static int snes_print(lua_State *L) {
    /* Don't use luaL_checkstring - just get the string directly */
    if (lua_gettop(L) >= 1 && lua_isstring(L, 1)) {
        const char *str = lua_tostring(L, 1);
        if (str != NULL) {
            termM0PrintStringXY_scroll((char*)str, 0, current_line);
        }
    }
    return 0;
}

static int snes_math_add(lua_State *L) {
    /* Don't use luaL_checknumber - just get numbers directly */
    if (lua_gettop(L) >= 2 && lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
        lua_Number a = lua_tonumber(L, 1);
        lua_Number b = lua_tonumber(L, 2);
        lua_pushnumber(L, a + b);
        return 1;
    }
    lua_pushnumber(L, 0);
    return 1;
}

static int snes_math_multiply(lua_State *L) {
    /* Don't use luaL_checknumber - just get numbers directly */
    if (lua_gettop(L) >= 2 && lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
        lua_Number a = lua_tonumber(L, 1);
        lua_Number b = lua_tonumber(L, 2);
        lua_pushnumber(L, a * b);
        return 1;
    }
    lua_pushnumber(L, 0);
    return 1;
}

/* SNES-specific io.write function */
static int snes_io_write(lua_State *L) {
    int n = lua_gettop(L);  /* number of arguments */
    int i;
    for (i = 1; i <= n; i++) {
        const char *s;
        size_t l;
        s = lua_tolstring(L, i, &l);  /* convert to string */
        if (s == NULL)
            return luaL_error(L, "'tostring' must return a string to 'print'");
        
        /* Print the string */
        termM0PrintStringXY_scroll((char*)s, 0, current_line);
    }
    return 0;
}

/* SNES-specific io.flush function */
static int snes_io_flush(lua_State *L) {
    /* On SNES, we don't need to flush anything */
    (void)L;
    return 0;
}

/* SNES-specific os.clock function */
static int snes_os_clock(lua_State *L) {
    /* Simple counter for SNES - just return a basic time value */
    static int counter = 0;
    counter++;
    lua_pushnumber(L, (lua_Number)counter);
    return 1;
}

/* SNES-specific collectgarbage function */
static int snes_collectgarbage(lua_State *L) {
    const char *opt = luaL_optstring(L, 1, "collect");
    
    if (strcmp(opt, "count") == 0) {
        /* Return simulated heap usage */
        lua_pushnumber(L, 1024.0); /* 1KB simulated */
    } else if (strcmp(opt, "collect") == 0) {
        /* Simulate garbage collection */
        lua_pushnumber(L, 0); /* No objects collected */
    } else {
        lua_pushnumber(L, 0); /* Unknown option */
    }
    return 1;
}

/* Initialize Lua for SNES */
int lua_snes_init(void) {
    char debug_buffer[128];
    
    /* Check memory manager status */
    sprintf(debug_buffer, "Pool1: start=%p end=%p", g_mem_manager.pool1_start, g_mem_manager.pool1_end);
    termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    sprintf(debug_buffer, "Pool2: start=%p end=%p", g_mem_manager.pool2_start, g_mem_manager.pool2_end);
    termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    
    /* Check SA-1 SRAM availability */
    if (g_mem_manager.pool3_available) {
        sprintf(debug_buffer, "Pool3: start=%p end=%p", g_mem_manager.pool3_start, g_mem_manager.pool3_end);
        termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    } else {
        termM0PrintStringXY_scroll("SA-1 SRAM: Not available", 0, current_line);
        sprintf(debug_buffer, "Pool3: start=%p end=%p", g_mem_manager.pool3_start, g_mem_manager.pool3_end);
        termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
        /* Test SA-1 SRAM directly */
        int test_result = snes_test_sa1_sram();
        sprintf(debug_buffer, "SA-1 test result: %d", test_result);
        termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    }
    
    /* Print key debugging info */
    sprintf(debug_buffer, "INT_MAX: %ld", (long)INT_MAX);
    termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    
    /* Test INT_MAX directly */
    sprintf(debug_buffer, "INT_MAX test: %d", (int)INT_MAX);
    termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    
    /* Test SIZE_MAX directly */
    sprintf(debug_buffer, "SIZE_MAX: %u", (unsigned int)SIZE_MAX);
    termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    
    /* Test size_t max value */
    sprintf(debug_buffer, "SIZE_T_MAX: %u", (unsigned int)(~(size_t)0));
    termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
    
    /* Test memory manager directly */
    void *test_ptr = snes_malloc(1024);
    if (test_ptr != NULL) {
        termM0PrintStringXY_scroll("Test malloc: OK", 0, current_line);
        snes_free(test_ptr);
        termM0PrintStringXY_scroll("Test free: OK", 0, current_line);
    } else {
        termM0PrintStringXY_scroll("Test malloc: FAILED!", 0, current_line);
    }
    
    /* Test large allocation that Lua might need */
    void *test_large = snes_malloc(65000);
    if (test_large != NULL) {
        termM0PrintStringXY_scroll("Test large: OK", 0, current_line);
        snes_free(test_large);
        termM0PrintStringXY_scroll("Test large free: OK", 0, current_line);
    } else {
        termM0PrintStringXY_scroll("Test large: FAILED!", 0, current_line);
    }
    
    /* Check if memory manager is properly initialized */
    if (g_mem_manager.pool1_start == NULL || g_mem_manager.pool2_start == NULL) {
        termM0PrintStringXY_scroll("ERROR: Memory manager not init!", 0, current_line);
        return 0;
    }
    
    /* Create Lua state with custom allocator */
    termM0PrintStringXY_scroll("Creating Lua state...", 0, current_line);
    
    L = lua_newstate(lua_alloc_debug, NULL);
    
    if (L == NULL) {
        termM0PrintStringXY_scroll("ERROR: lua_newstate() failed!", 0, current_line);
        return 0; /* Failed to create state */
    }
    
    termM0PrintStringXY_scroll("Lua state created OK", 0, current_line);
    
    /* Skip standard libraries for now to avoid opcode issues */
    termM0PrintStringXY_scroll("Skipping libraries...", 0, current_line);
    
    termM0PrintStringXY_scroll("Lua init complete!", 0, current_line);
    return 1; /* Success */
}

/* Execute Lua string */
int lua_snes_dostring(lua_State *L, const char *script) {
    termM0PrintStringXY_scroll("Loading Lua script...", 0, current_line);
    
    int status = luaL_loadstring(L, script);
    if (status == LUA_OK) {
        termM0PrintStringXY_scroll("Script loaded OK", 0, current_line);
        termM0PrintStringXY_scroll("Calling lua_pcall...", 0, current_line);
        status = lua_pcall(L, 0, 0, 0);
        if (status != LUA_OK) {
            termM0PrintStringXY_scroll("lua_pcall failed!", 0, current_line);
        }
    } else {
        termM0PrintStringXY_scroll("luaL_loadstring failed!", 0, current_line);
        /* Print Lua error details */
        char debug_buffer[64];
        const char *error_msg = lua_tostring(L, -1);
        if (error_msg != NULL) {
            strncpy(debug_buffer, error_msg, 30);
            debug_buffer[30] = '\0';
            termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
        } else {
            sprintf(debug_buffer, "Error code: %d", status);
            termM0PrintStringXY_scroll(debug_buffer, 0, current_line);
        }
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

/* Calypsi stubs for SNES build */
/* These provide the missing functions that Lua requires */

/* Stub for write function - used by Lua's I/O operations */
size_t _Stub_write(int fd, const void *buf, size_t count) {
    /* For SNES, we don't have a real file system */
    /* Just return success to satisfy the linker */
    return count;
}

/* Stub for read function - used by Lua's I/O operations */
size_t _Stub_read(int fd, void *buf, size_t count) {
    /* For SNES, we don't have a real file system */
    /* Just return 0 (EOF) to satisfy the linker */
    return 0;
}

/* Stub for close function - used by Lua's I/O operations */
int _Stub_close(int fd) {
    /* For SNES, we don't have a real file system */
    /* Just return success to satisfy the linker */
    return 0;
}

/* Stub for lseek function - used by Lua's I/O operations */
long _Stub_lseek(int fd, long offset, int whence) {
    /* For SNES, we don't have a real file system */
    /* Just return success to satisfy the linker */
    return 0;
}

/* Stub for open function - used by Lua's I/O operations */
int _Stub_open(const char *pathname, int flags, ...) {
    /* For SNES, we don't have a real file system */
    /* Just return -1 (error) to satisfy the linker */
    return -1;
}

/* SNES-compatible implementations of Lua's output functions */
/* These replace the default stdout/stderr implementations */

/* Undefine Lua macros to define our own functions */
#undef lua_writestring
#undef lua_writeline
#undef lua_writestringerror

void lua_writestring(const char *str, size_t len) {
    /* For SNES, we'll just ignore output to stdout */
    /* In a real implementation, you might want to buffer this or send to a debug port */
    (void)str;  /* Suppress unused parameter warning */
    (void)len;  /* Suppress unused parameter warning */
}

void lua_writeline(void) {
    /* For SNES, we'll just ignore newline output */
    /* In a real implementation, you might want to handle this */
}

void lua_writestringerror(const char *str, const char *param) {
    /* For SNES, we'll just ignore error output */
    /* In a real implementation, you might want to handle this */
    (void)str;   /* Suppress unused parameter warning */
    (void)param; /* Suppress unused parameter warning */
}

/* Function to demonstrate individual Lua operations with results */
void demonstrate_lua_operation(lua_State *L, const char* operation, const char* description, int line) {
    char result_buffer[128];
    char error_buffer[128];
    
    /* Display the operation description */
    termM0PrintStringXY_scroll((char*)description, 0, line);
    
    /* Execute the Lua operation */
    int status = lua_snes_dostring(L, operation);
    if (status == LUA_OK) {
        /* Show success message */
        strcpy(result_buffer, "-> Success");
        termM0PrintStringXY_scroll(result_buffer, 0, line + 1);
    } else {
        /* Show failure message with Lua error details */
        strcpy(result_buffer, "-> Failed (");
        
        /* Get the error message from Lua stack */
        const char *error_msg = lua_tostring(L, -1);
        if (error_msg != NULL) {
            /* Truncate error message to fit in buffer */
            strncpy(error_buffer, error_msg, 20);
            error_buffer[20] = '\0';
            strcat(result_buffer, error_buffer);
        } else {
            strcat(result_buffer, "Unknown error");
        }
        strcat(result_buffer, ")");
        
        termM0PrintStringXY_scroll(result_buffer, 0, line + 1);
        
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

/* Test memory manager with large allocations */
void test_memory_manager(void) {
    char test_buffer[64];
    
    termM0PrintStringXY_scroll("=== Memory Manager Tests ===", 0, current_line);
    
    /* Print sizeof information for debugging */
    termM0PrintStringXY_scroll("sizeof() debugging in test:", 0, current_line);
    sprintf(test_buffer, "char:%u int:%u long:%u", (unsigned int)sizeof(char), (unsigned int)sizeof(int), (unsigned int)sizeof(long));
    termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    sprintf(test_buffer, "size_t:%u ptr:%u mgr:%u", (unsigned int)sizeof(size_t), (unsigned int)sizeof(void*), (unsigned int)sizeof(snes_memory_manager_t));
    termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    
    /* Display pool information */
    termM0PrintStringXY_scroll("Memory Pools:", 0, current_line);
    sprintf(test_buffer, "Pool1: %u bytes", (unsigned int)POOL1_SIZE);
    termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    sprintf(test_buffer, "Pool2: %u bytes", (unsigned int)POOL2_SIZE);
    termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    
    /* Test 1: Allocate 60KB block (should go to pool1) */
    termM0PrintStringXY_scroll("Test 1: Allocating 60KB...", 0, current_line);
    void *block1 = snes_malloc(60000);
    if (block1 != NULL) {
        strcpy(test_buffer, "60KB allocation: SUCCESS");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    } else {
        strcpy(test_buffer, "60KB allocation: FAILED");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    }
    
    /* Test 2: Allocate 30KB block (should go to pool2) */
    termM0PrintStringXY_scroll("Test 2: Allocating 30KB...", 0, current_line);
    void *block2 = snes_malloc(30000);
    if (block2 != NULL) {
        strcpy(test_buffer, "30KB allocation: SUCCESS");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    } else {
        strcpy(test_buffer, "30KB allocation: FAILED");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    }
    
    /* Test 3: Try to allocate another 10KB block (should fail) */
    termM0PrintStringXY_scroll("Test 3: Allocating 10KB...", 0, current_line);
    void *block3 = snes_malloc(10000);
    if (block3 != NULL) {
        strcpy(test_buffer, "10KB allocation: SUCCESS (UNEXPECTED!)");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    } else {
        strcpy(test_buffer, "10KB allocation: FAILED (expected)");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    }
    
    /* Test 4: Free blocks and try again */
    termM0PrintStringXY_scroll("Test 4: Freeing blocks...", 0, current_line);
    if (block1 != NULL) {
        snes_free(block1);
        strcpy(test_buffer, "Freed 60KB block");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    }
    if (block2 != NULL) {
        snes_free(block2);
        strcpy(test_buffer, "Freed 30KB block");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    }

    if (block3 != NULL) {
        snes_free(block3);
        strcpy(test_buffer, "Freed 10KB block");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    }
    
    /* Test 5: Try allocation after freeing */
    termM0PrintStringXY_scroll("Test 5: Allocating 50KB...", 0, current_line);
    void *block4 = snes_malloc(50000);
    if (block4 != NULL) {
        strcpy(test_buffer, "50KB allocation: SUCCESS");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    } else {
        strcpy(test_buffer, "50KB allocation: FAILED");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    }
    
    if (block4 != NULL) {
        snes_free(block4);
        strcpy(test_buffer, "Freed 50KB block");
        termM0PrintStringXY_scroll(test_buffer, 0, current_line);
    }
    termM0PrintStringXY_scroll("Memory Manager Tests Complete!", 0, current_line);
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


int main(void) {
    int8_t regRead1; /* Variable for storing hardware registers */
    lua_State *L;
    
    /* Initialization */
    initSNES(SLOWROM);
    
    /* Initialize SA1 BW-RAM mapping for F0:0000-FF:FFFF */
    initSA1();
    
    /* Initialize custom memory manager */
    snes_memory_init();
    
    termM0Init();
    clear_screen();  /* Clear screen and initialize scrolling */
    
    /* Display header first */
    termM0PrintStringXY_scroll("Lua 5.4.8 Demo for SNES",0,1);
    termM0PrintStringXY_scroll("Compiler:",0,2);
    termM0PrintStringXY_scroll(SNES_XC_COMPILER_NAME,20,2);
    REG_INIDISP = 0x0F;
    REG_NMITIMEN = 0x01; /* Enable NMI for frame counting */
    
    current_line = 3;  // Start Lua tests after header
    
    /* Initialize Lua */
    termM0PrintStringXY_scroll("Initializing Lua...",0,current_line);
    if (lua_snes_init() == 0) {
        termM0PrintStringXY_scroll("Lua init failed!",0,4);
        while(1) {
            do { /* Wait for Vblank */
                regRead1 = REG_RDNMI;
            } while(regRead1 > 0);
        }
    }
    L = lua_snes_getstate();
    
    /* Run basic Lua test - just test core functionality */
    termM0PrintStringXY_scroll("Lua 5.4.8 Basic Test:",0,4);
    
    /* Test 1: Basic arithmetic */
    termM0PrintStringXY_scroll("1. Basic Arithmetic:",0,5);
    termM0PrintStringXY_scroll("About to test: 5+3",0,6);
    demonstrate_lua_operation(L, "result = 5 + 3", "result = 5 + 3", 7);
    
    /* Test 2: String operations */
    termM0PrintStringXY_scroll("2. String Operations:",0,8);
    demonstrate_lua_operation(L, "message = 'Hello Lua!'", "message = 'Hello Lua!'", 9);
    
    /* Test 3: Table operations */
    termM0PrintStringXY_scroll("3. Table Operations:",0,11);
    demonstrate_lua_operation(L, "t = {x = 10, y = 20}", "t = {x = 10, y = 20}", 12);
    demonstrate_lua_operation(L, "sum = t.x + t.y", "sum = t.x + t.y", 13);
    
    /* Test 4: Function definition */
    termM0PrintStringXY_scroll("4. Function Definitions:",0,15);
    demonstrate_lua_operation(L, "function square(n) return n * n end", "function square(n) defined", 16);
    demonstrate_lua_operation(L, "result = square(4)", "result = square(4)", 17);
    
    /* Test 5: Control structures */
    termM0PrintStringXY_scroll("5. Control Structures:",0,19);
    demonstrate_lua_operation(L, "sum = 0; for i = 1, 10 do sum = sum + i end", "for loop: sum 1 to 10", 20);
    
    termM0PrintStringXY_scroll("Lua tests complete!",0,current_line);
    
    /* Main loop - display results */
    while(1) {
        do { /* Wait for Vblank */
            regRead1 = REG_RDNMI;
        } while(regRead1 > 0);
        termM0PrintStringXY_scroll("LUA DEMO RUNNING        ",0,current_line);
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
