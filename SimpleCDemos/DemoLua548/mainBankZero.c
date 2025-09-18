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
int auto_print_string(char *szInput);
void clear_screen(void);
void write_debug_info(const char* info);
void write_debug_char(char c, uint8_t line);

/* Test function for Lua */
static int lua_test_function(lua_State *L) {
    lua_pushnumber(L, 42);
    return 1;
}

/* Forward declarations for timing variables */
extern volatile uint32_t frameCounter;
static uint32_t startFrameCount = 0;

/* Custom SNES os.clock function - uses frame counter for timing */
static int snes_os_clock(lua_State *L) {
    /* Convert frame count to seconds (assuming 60 FPS) */
    double seconds = (double)(frameCounter - startFrameCount) / 60.0;
    lua_pushnumber(L, seconds);
    return 1;
}

// MSU1 functions
static int msu1_is_present(void);
static int msu1_load_lua_file(lua_State *L, const char* filename);
static int msu1_read_data(uint32_t offset, uint8_t* buffer, uint32_t size);

/* Custom SNES print function for Lua */
/* Ultra-minimal SNES print function for Lua - optimized for extremely low memory usage */
static int snes_lua_print(lua_State *L) {
    int n = lua_gettop(L);  /* number of arguments */
    int i;
    
    /* Process each argument individually to minimize memory usage */
    for (i = 1; i <= n; i++) {
        if (lua_isnumber(L, i)) {
            /* Handle numbers directly without tostring conversion */
            lua_Number num = lua_tonumber(L, i);
            char num_str[16];  /* Small buffer for numbers only */
            snprintf(num_str, sizeof(num_str), "%ld", (long)num);
            auto_print_string(num_str);
        } else if (lua_isstring(L, i)) {
            /* Handle strings directly */
            const char* str = lua_tostring(L, i);
            if (str) {
                auto_print_string(str);
            }
        } else if (lua_isboolean(L, i)) {
            /* Handle booleans */
            auto_print_string(lua_toboolean(L, i) ? "true" : "false");
        } else if (lua_isnil(L, i)) {
            /* Handle nil */
            auto_print_string("nil");
        } else {
            /* For other types, just print a generic message */
            auto_print_string("[object]");
        }
        
        /* Add space between arguments (except last) */
        if (i < n) {
            auto_print_string(" ");
        }
    }
    
    return 0;
}

/* MSU1 function implementations */
static int msu1_is_present(void) {
    /* Check if MSU1 is present by reading the identification string */
    /* According to the spec, $2002-$2007 should contain "S-MSU1" */
    const char* expected_id = "S-MSU1";
    volatile uint8_t* ident_ptr = (volatile uint8_t*)0x2002;
    int i;

    for (i = 0; i < 6; i++) {
        if (ident_ptr[i] != (uint8_t)expected_id[i]) {
            return 0;  /* MSU1 not present */
        }
    }
    return 1;  /* MSU1 is present */
}

static int msu1_read_data(uint32_t offset, uint8_t* buffer, uint32_t size) {
    uint32_t i;
    
    /* Set the data offset */
    MSU1_OFFSET = offset;
    
    /* Wait for data port to be ready */
    while (MSU1_STATUS & MSU1_STATUS_DATA_BUSY) {
        /* Wait for data port to be ready */
    }
    
    /* Read data byte by byte */
    for (i = 0; i < size; i++) {
        /* Wait for data port to be ready */
        while (MSU1_STATUS & MSU1_STATUS_DATA_BUSY) {
            /* Wait for data port to be ready */
        }
        
        buffer[i] = MSU1_DATA;
    }
    
    return 1;  /* Success */
}

static int msu1_load_lua_file(lua_State *L, const char* filename) {
    static char file_buffer[2048];  /* Static buffer for entire file */
    uint32_t offset = 0;
    uint32_t buffer_pos = 0;
    uint32_t max_bytes = sizeof(file_buffer) - 1;  /* Leave room for null terminator */
    
    auto_print_string("Reading entire Lua file from MSU1 into buffer...");
    
    /* Read entire file into buffer first */
    while (buffer_pos < max_bytes && offset < max_bytes) {
        uint8_t byte;
        
        /* Read single byte from MSU1 */
        if (!msu1_read_data(offset, &byte, 1)) {
            auto_print_string("Failed to read data from MSU1");
            return 0;  /* Failed to read data */
        }
        
        /* Check for null terminator (end of file) */
        if (byte == 0) {
            auto_print_string("End of file reached");
            break;
        }
        
        /* Store byte in file buffer */
        file_buffer[buffer_pos] = byte;
        buffer_pos++;
        offset++;
    }
    
    /* Null-terminate the buffer */
    file_buffer[buffer_pos] = '\0';
    
    /* Report what we read */
    char file_info[64];
    snprintf(file_info, sizeof(file_info), "Read %lu bytes from MSU1", (unsigned long)buffer_pos);
    auto_print_string(file_info);
    
    /* Show the file content for debugging */
    auto_print_string("File content:");
    auto_print_string(file_buffer);
    
    /* PHASE 1: Compile the Lua script to bytecode */
    auto_print_string("PHASE 1: Compiling Lua script...");
    lua_gc(L, LUA_GCCOLLECT, 0);  /* GC before compilation */
    
    uint32_t mem_before_compile = snes_get_free_memory();
    uint32_t largest_before_compile = snes_get_largest_free_block();
    
    int compile_result = luaL_loadstring(L, file_buffer);
    
    if (compile_result != 0) {
        /* Get compilation error message from Lua stack */
        const char* error_msg = lua_tostring(L, -1);
        if (error_msg) {
            char error_info[128];
            snprintf(error_info, sizeof(error_info), "Compilation error: %.80s", error_msg);
            auto_print_string(error_info);
        }
        auto_print_string("Failed to compile Lua file from MSU1");
        lua_pop(L, 1);  /* Remove error message from stack */
        return 0;
    }
    
    /* CRITICAL DEBUG POINT: Compilation complete, execution not started */
    uint32_t mem_after_compile = snes_get_free_memory();
    uint32_t largest_after_compile = snes_get_largest_free_block();
    
    char compile_debug[128];
    snprintf(compile_debug, sizeof(compile_debug), "COMPILATION COMPLETE! Memory: %lu->%lu, Largest: %lu->%lu", 
             (unsigned long)mem_before_compile, (unsigned long)mem_after_compile,
             (unsigned long)largest_before_compile, (unsigned long)largest_after_compile);
    auto_print_string(compile_debug);
    auto_print_string("PHASE 2: About to execute bytecode...");
    
    /* PHASE 2: Execute the compiled bytecode */
    lua_gc(L, LUA_GCCOLLECT, 0);  /* GC before execution */
    
    uint32_t mem_before_exec = snes_get_free_memory();
    int exec_result = lua_pcall(L, 0, 0, 0);  /* Execute with 0 args, 0 returns, no error handler */
    uint32_t mem_after_exec = snes_get_free_memory();
    
    if (exec_result != 0) {
        /* Get execution error message from Lua stack */
        const char* error_msg = lua_tostring(L, -1);
        if (error_msg) {
            char error_info[128];
            snprintf(error_info, sizeof(error_info), "Execution error: %.80s", error_msg);
            auto_print_string(error_info);
        }
        auto_print_string("Failed to execute Lua file from MSU1");
        lua_pop(L, 1);  /* Remove error message from stack */
        return 0;
    }
    
    char exec_debug[64];
    snprintf(exec_debug, sizeof(exec_debug), "EXECUTION COMPLETE! Memory: %lu->%lu", 
             (unsigned long)mem_before_exec, (unsigned long)mem_after_exec);
    auto_print_string(exec_debug);
    auto_print_string("MSU1 Lua script executed successfully!");
    
    /* Check memory after execution */
    uint32_t free_after = snes_get_free_memory();
    char mem_after[64];
    snprintf(mem_after, sizeof(mem_after), "Free memory after execution: %lu bytes", (unsigned long)free_after);
    auto_print_string(mem_after);
    
    /* Force garbage collection */
    lua_gc(L, LUA_GCCOLLECT, 0);
    
    char success_msg[64];
    snprintf(success_msg, sizeof(success_msg), "MSU1 Lua file executed successfully! (%lu bytes)", (unsigned long)buffer_pos);
    auto_print_string(success_msg);
    return 1;  /* Success */
}

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


/* Custom memory allocator for SNES with debugging - NO size_t, explicit uint32_t only */
void* snes_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
    /* COMPLETELY ELIMINATE size_t usage - cast immediately to uint32_t */
    uint32_t old_size = (uint32_t)osize;
    uint32_t new_size = (uint32_t)nsize;
    (void)ud;  /* Not used in this implementation */
    
    /* Debug: Log ALL allocation requests to see what's happening */
    if (new_size > 1024) {  /* Log allocations > 1KB */
        char alloc_debug[80];
        snprintf(alloc_debug, sizeof(alloc_debug), "ALLOC: %lu bytes requested (size_t=%lu, ptr=%p)", 
                 (unsigned long)new_size, (unsigned long)nsize, ptr);
        auto_print_string(alloc_debug);
    }
    
    /* CRITICAL: Check for size_t truncation by comparing original vs cast */
    if ((size_t)new_size != nsize) {
        char truncation_debug[80];
        snprintf(truncation_debug, sizeof(truncation_debug), "TRUNCATION: size_t %lu became %lu", 
                 (unsigned long)nsize, (unsigned long)new_size);
        auto_print_string(truncation_debug);
    }
    
    if (new_size == 0) {
        /* Free memory */
        if (ptr != NULL) {
            snes_free(ptr);
        }
        return NULL;
    }
    
    if (ptr == NULL) {
        /* Allocate new memory using explicit 32-bit size */
        void* result = snes_malloc(new_size);
        if (result == NULL && new_size > 1024) {
            /* Log failed allocations */
            char fail_debug[64];
            snprintf(fail_debug, sizeof(fail_debug), "FAILED: %lu bytes", (unsigned long)new_size);
            auto_print_string(fail_debug);
        } else if (result != NULL && new_size > 1024) {
            /* Log successful allocations with address */
            char success_debug[80];
            snprintf(success_debug, sizeof(success_debug), "SUCCESS: %lu bytes at %p", (unsigned long)new_size, result);
            auto_print_string(success_debug);
        }
        return result;
    } else {
        /* Reallocate existing memory using explicit 32-bit sizes */
        return snes_realloc(ptr, old_size, new_size);
    }
}

/* Note: snes_memory_reset() removed - not safe with active Lua state */

/* Global pre-allocated compilation buffer for Lua */
static void* lua_compilation_buffer = NULL;

/* Initialize Lua for SNES */
int lua_snes_init(void) {
    /* Check memory manager before creating Lua state */
    if (!g_mem_manager.pool3_available) {
        return 0;
    }
    
    /* Strategy: Allocate almost ALL remaining memory to force consolidation later */
    auto_print_string("Pre-allocating large buffer to prevent fragmentation...");
    
    /* Get current free memory */
    uint32_t current_free = snes_get_free_memory();
    uint32_t buffer_size = current_free - 32768;  /* Leave 32KB for Lua init, allocate the rest */
    
    lua_compilation_buffer = snes_malloc(buffer_size);
    if (lua_compilation_buffer == NULL) {
        auto_print_string("Failed to pre-allocate large buffer");
        return 0;
    } else {
        char buffer_info[64];
        snprintf(buffer_info, sizeof(buffer_info), "Pre-allocated %lu byte buffer", (unsigned long)buffer_size);
        auto_print_string(buffer_info);
    }
    
    /* Create Lua state with custom allocator */
    L = lua_newstate(snes_lua_alloc, NULL);
    if (L == NULL) {
        /* Free the pre-allocated buffer if Lua init fails */
        snes_free(lua_compilation_buffer);
        lua_compilation_buffer = NULL;
        return 0;
    }
    
    /* Set panic handler */
    lua_atpanic(L, lua_panic_handler);
    
    /* Load only essential libraries to reduce output flood */
    luaL_openlibs(L);
    
    /* Register custom SNES print function */
    lua_pushcfunction(L, snes_lua_print);
    lua_setglobal(L, "print");
    
    /* Register custom SNES os.clock function */
    lua_getglobal(L, "os");
    if (lua_isnil(L, -1)) {
        /* Create os table if it doesn't exist */
        lua_newtable(L);
        lua_setglobal(L, "os");
        lua_getglobal(L, "os");
    }
    lua_pushcfunction(L, snes_os_clock);
    lua_setfield(L, -2, "clock");
    lua_pop(L, 1);  /* Pop os table */
    
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
    /* Execute the benchmark script directly */
    if (strcmp(filename, "benchmark.lua") == 0) {
        const char *benchmark_script = 
            "-- Ultra-simple benchmark - no collectgarbage calls\n"
            "print('=== Ultra-Simple Benchmark ===')\n"
            "\n"
            "local start = os.clock()\n"
            "print('Start time: ' .. start .. ' seconds')\n"
            "\n"
            "print('Creating simple objects...')\n"
            "local objects = {}\n"
            "for i = 1, 5 do\n"
            "    objects[i] = {i, tostring(i)}\n"
            "end\n"
            "\n"
            "print('Objects created successfully')\n"
            "\n"
            "print('Freeing objects...')\n"
            "for i = 1, 5 do\n"
            "    objects[i] = nil\n"
            "end\n"
            "\n"
            "print('Objects freed successfully')\n"
            "\n"
            "local finish = os.clock()\n"
            "print('End time: ' .. finish .. ' seconds')\n"
            "print('Elapsed time: ' .. (finish - start) .. ' seconds')\n"
            "print('=== Benchmark Complete ===')\n";
        
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
        sprintf(pool_size, "Size: %lu bytes (%lu KB)", pool_size_bytes, pool_size_kb);
        auto_print_string(pool_size);
        
        char current_info[64];
        sprintf(current_info, "Current: %p", g_mem_manager.pool3_current);
        auto_print_string(current_info);
        
        /* Debug: Show expected vs actual */
        if (pool_size_bytes != 131072) {  /* 128KB = 131072 bytes */
            char debug_info[128];
            sprintf(debug_info, "WARNING: Expected 131072 bytes, got %lu", pool_size_bytes);
            auto_print_string(debug_info);
        } else {
            auto_print_string("Pool size detection: CORRECT (128KB)");
        }
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
    
    /* Check memory state right after Lua init */
    auto_print_string("=== Memory State After Lua Init ===");
    char mem_info[128];
    snprintf(mem_info, sizeof(mem_info), "Mem after init: %p-%p, current: %p", 
             g_mem_manager.pool3_start, g_mem_manager.pool3_end, g_mem_manager.pool3_current);
    auto_print_string(mem_info);
    snprintf(mem_info, sizeof(mem_info), "Total allocated: %lu bytes", g_mem_manager.total_allocated);
    auto_print_string(mem_info);
    uint32_t free_mem = snes_get_free_memory();
    snprintf(mem_info, sizeof(mem_info), "Free memory: %lu bytes", free_mem);
    auto_print_string(mem_info);
    
    /* Debug: Check if free list is working */
    if (free_mem == 0) {
        auto_print_string("ERROR: Free memory is 0 - free list issue!");
        
        /* Debug: Check if free list head is NULL */
        if (free_list_head == NULL) {
            auto_print_string("ERROR: free_list_head is NULL!");
        } else {
            auto_print_string("ERROR: free_list_head exists but size is 0!");
        }
    } else {
        snprintf(mem_info, sizeof(mem_info), "Free list working: %lu bytes available", free_mem);
        auto_print_string(mem_info);
    }
    
    /* Initialize timing for os.clock */
    startFrameCount = frameCounter;
    
    
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
    
    /* Prepare for MSU1 test - maximize available memory */
    auto_print_string("=== Preparing for MSU1 test ===");
        lua_settop(L, 0);
    lua_gc(L, LUA_GCCOLLECT, 0);
    
    /* Report available memory */
    uint32_t free_mem_msu1 = snes_get_free_memory();
    char mem_info2[64];
    snprintf(mem_info2, sizeof(mem_info2), "Available memory: %lu bytes", (unsigned long)free_mem_msu1);
    auto_print_string(mem_info2);
    
    /* Ensure stack is clean before MSU1 test */
    lua_settop(L, 0);
    
    /* Test MSU1 Lua file loading */
    auto_print_string("=== Testing MSU1 Lua File Loading ===");
    
    auto_print_string("=== eLua SNES Demo Ready ===");
    
    if (msu1_is_present()) {
        auto_print_string("MSU1 detected - loading Lua file...");
        
        /* Clean start for MSU1 test - maximize available memory */
        lua_settop(L, 0);  /* Clear Lua stack */
        lua_gc(L, LUA_GCCOLLECT, 0);  /* Force garbage collection */
        
        /* Free the pre-allocated compilation buffer to make 70KB available */
        if (lua_compilation_buffer != NULL) {
            auto_print_string("Freeing pre-allocated 70KB compilation buffer...");
            uint32_t largest_before_free = snes_get_largest_free_block();
            snes_free(lua_compilation_buffer);
            lua_compilation_buffer = NULL;
            uint32_t largest_after_free = snes_get_largest_free_block();
            
            char buffer_info[128];
            snprintf(buffer_info, sizeof(buffer_info), "Buffer freed: %lu -> %lu bytes largest block", 
                     (unsigned long)largest_before_free, (unsigned long)largest_after_free);
            auto_print_string(buffer_info);
        }
        
        /* Force coalescing of adjacent free blocks */
        auto_print_string("Forcing memory coalescing...");
        uint32_t largest_before_coalesce = snes_get_largest_free_block();
        snes_force_coalesce();
        uint32_t largest_after_coalesce = snes_get_largest_free_block();
        
        char coalesce_info[128];
        snprintf(coalesce_info, sizeof(coalesce_info), "Coalescing: %lu -> %lu bytes largest block", 
                 (unsigned long)largest_before_coalesce, (unsigned long)largest_after_coalesce);
        auto_print_string(coalesce_info);
        
        if (msu1_load_lua_file(L, "test.lua")) {
            auto_print_string("MSU1 Lua file loaded and executed successfully!");
        } else {
            auto_print_string("Failed to load or execute Lua file from MSU1");
        }
    } else {
        auto_print_string("MSU1 not present - skipping Lua file loading test");
    }
    
    auto_print_string("=== All Tests Complete ===");
    
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

