// Minimal Lua configuration for SNES - disable everything possible

#ifndef __PLATFORM_CONF_H__
#define __PLATFORM_CONF_H__

// SNES-specific configuration
#define PLATFORM_SNES

// Disable ALL eLua features
#undef BUILD_RPC
#undef BUILD_MMCFS
#undef BUILD_ROMFS
#undef BUILD_SEMIFS
#undef BUILD_REMOTEFS
#undef BUILD_TERM
#undef BUILD_ELUA
#undef BUILD_ADC
#undef BUILD_CAN
#undef BUILD_CPU
#undef BUILD_I2C
#undef BUILD_NET
#undef BUILD_PD
#undef BUILD_PIO
#undef BUILD_PWM
#undef BUILD_SPI
#undef BUILD_TMR
#undef BUILD_UART

// Disable all optional Lua features
#undef LUA_USE_LINENOISE
#undef LUA_USE_READLINE

// Disable string table optimization that might cause hangs
#define LUAI_HASHLIMIT 5
#define MINSTRTABSIZE 32

// CRITICAL: Force memory limits BEFORE any Lua headers are included
// These must be defined early to override default luaconf.h values

// COMPILATION-FOCUSED limits - reduce compilation memory, keep execution reasonable
#undef LUAI_MAXCCALLS
#define LUAI_MAXCCALLS 50          // Reasonable for execution

#undef LUAI_MAXCALLS  
#define LUAI_MAXCALLS 1000         // Reasonable for execution

#undef LUAI_MAXCSTACK
#define LUAI_MAXCSTACK 200         // Reasonable for execution

#undef LUAI_MAXVARS
#define LUAI_MAXVARS 20            // Reasonable for execution

#undef LUAI_MAXUPVALUES
#define LUAI_MAXUPVALUES 20        // Reasonable for execution

#undef MAXSTACK
#define MAXSTACK 100               // Reasonable for execution

#undef LUA_MAXINPUT
#define LUA_MAXINPUT 128           // Reasonable for execution

// CRITICAL: Target compilation buffers specifically
#undef LUAL_BUFFERSIZE
#define LUAL_BUFFERSIZE 512        // Compilation buffer - keep reasonable

#undef LUA_MINBUFFER
#define LUA_MINBUFFER 256          // Compilation minimum buffer

// Reasonable string table sizes
#undef LUAI_HASHLIMIT
#define LUAI_HASHLIMIT 5

#undef MINSTRTABSIZE
#define MINSTRTABSIZE 32

// Enable integer-only arithmetic for better performance and memory usage
#define LUA_NUMBER_INTEGRAL         // Use integers instead of doubles for Lua numbers

// Disable floating-point support entirely
#define LUA_ANSI                    // Use strict ANSI C (no floating-point optimizations)
#define __STRICT_ANSI__             // Force strict ANSI compliance
#define NO_FLOAT                    // Disable floating-point operations
#define NO_DOUBLE                   // Disable double precision

// Manual optimization flags - enable one at a time for testing
#define LUA_USELIGHTFUNCTIONS 1
// Test: Individual optimizations (NOT LUA_OPTIMIZE_MEMORY = 1)
// #define LUA_OPTIMIZE_MEMORY 1
// #define LUA_META_ROTABLES

// Test 2: Manually override math library to return 0
// We'll modify lmathlib.c directly instead of using LUA_OPTIMIZE_MEMORY
#define TEST_MATH_RETURN_0 1

// Test 3: Manually override os library to return 0
#define TEST_OS_RETURN_0 1

// Test 4: Manually override table library to return 0
#define TEST_TABLE_RETURN_0 1

// Test 5: Manually override debug library to return 0
#define TEST_DEBUG_RETURN_0 1

// Test 8: Library return 0 approach WITHOUT read-only tables
// This will make ALL libraries return 0 but NOT use read-only tables
// #define LUA_OPTIMIZE_MEMORY 1

// Test 9: Enable LUA_META_ROTABLES from LUA_OPTIMIZE_MEMORY=2
// This enables metatable support for read-only tables
#define LUA_META_ROTABLES

// Test 10: Disable string metatable creation (LUA_OPTIMIZE_MEMORY=2 optimization)
// This disables createmetatable() function to save memory
#define DISABLE_STRING_METATABLE

// Test 11: Enable IO library optimizations (LUA_OPTIMIZE_MEMORY=2 optimization)
// This uses iolib_funcs[] instead of iolib[] and enables luaL_index function
#define ENABLE_IO_OPTIMIZATIONS

// Platform-specific includes
#include "type.h"
#include "lrodefs.h"

// Define platform libraries for read-only tables
#define LUA_PLATFORM_LIBS_ROM\
  _ROM( LUA_MATHLIBNAME, luaopen_math, math_map )\
  _ROM( LUA_OSLIBNAME, luaopen_os, syslib )\
  _ROM( LUA_TABLIBNAME, luaopen_table, tab_funcs )\
  _ROM( LUA_DBLIBNAME, luaopen_debug, dblib )\
  _ROM( LUA_COLIBNAME, luaopen_base, co_funcs )

#endif // #ifndef __PLATFORM_CONF_H__
