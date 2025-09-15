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

// Minimize memory usage
#define LUAI_MAXCCALLS 200

// Set memory optimization level for SNES
#define LUA_OPTIMIZE_MEMORY 0

// Platform-specific includes
#include "type.h"

#endif // #ifndef __PLATFORM_CONF_H__
