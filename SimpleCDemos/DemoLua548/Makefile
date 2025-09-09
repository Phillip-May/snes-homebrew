# SNES Lua Demo Makefile
# Uses shared build configuration system

# Project-specific settings
PROJECT_NAME = DemoELua
BUILD_DIR = build

# Enable floating point support for WDC816CC
# WDC816CC now supports floating point with both math and standard libraries
USE_FLOATING_POINT = 1

# Paths to shared build system
SHARED_BUILD_DIR = ../shared/build
SHARED_SRC_DIR = ../shared/src
SHARED_PORT_DIR = ../shared/port

# Lua source files - minimal set for SNES
LUA_SRC_DIR = lua-5.4.8
LUA_CORE_SOURCES = $(LUA_SRC_DIR)/lapi.c $(LUA_SRC_DIR)/lcode.c $(LUA_SRC_DIR)/lctype.c \
                   $(LUA_SRC_DIR)/ldebug.c $(LUA_SRC_DIR)/ldo.c $(LUA_SRC_DIR)/ldump.c \
                   $(LUA_SRC_DIR)/lfunc.c $(LUA_SRC_DIR)/lgc.c $(LUA_SRC_DIR)/llex.c \
                   $(LUA_SRC_DIR)/lmem.c $(LUA_SRC_DIR)/lobject.c $(LUA_SRC_DIR)/lopcodes.c \
                   $(LUA_SRC_DIR)/lparser.c $(LUA_SRC_DIR)/lstate.c $(LUA_SRC_DIR)/lstring.c \
                   $(LUA_SRC_DIR)/ltable.c $(LUA_SRC_DIR)/ltm.c $(LUA_SRC_DIR)/lundump.c \
                   $(LUA_SRC_DIR)/lvm.c $(LUA_SRC_DIR)/lzio.c

LUA_LIB_SOURCES = $(LUA_SRC_DIR)/lauxlib.c $(LUA_SRC_DIR)/lbaselib.c $(LUA_SRC_DIR)/lcorolib.c \
                  $(LUA_SRC_DIR)/ldblib.c $(LUA_SRC_DIR)/liolib.c $(LUA_SRC_DIR)/lmathlib.c \
                  $(LUA_SRC_DIR)/loadlib.c $(LUA_SRC_DIR)/loslib.c $(LUA_SRC_DIR)/lstrlib.c \
                  $(LUA_SRC_DIR)/ltablib.c $(LUA_SRC_DIR)/lutf8lib.c

# Essential Lua files only (for initial testing)
LUA_ESSENTIAL_SOURCES = $(LUA_SRC_DIR)/lapi.c $(LUA_SRC_DIR)/lcode.c $(LUA_SRC_DIR)/ldebug.c \
                        $(LUA_SRC_DIR)/ldo.c $(LUA_SRC_DIR)/lfunc.c $(LUA_SRC_DIR)/lgc.c \
                        $(LUA_SRC_DIR)/llex.c $(LUA_SRC_DIR)/lmem.c $(LUA_SRC_DIR)/lobject.c \
                        $(LUA_SRC_DIR)/lopcodes.c $(LUA_SRC_DIR)/lparser.c $(LUA_SRC_DIR)/lstate.c \
                        $(LUA_SRC_DIR)/lstring.c $(LUA_SRC_DIR)/ltable.c $(LUA_SRC_DIR)/ltm.c \
                        $(LUA_SRC_DIR)/lundump.c $(LUA_SRC_DIR)/lvm.c $(LUA_SRC_DIR)/lzio.c \
                        $(LUA_SRC_DIR)/lauxlib.c $(LUA_SRC_DIR)/lbaselib.c $(LUA_SRC_DIR)/lmathlib.c \
                        $(LUA_SRC_DIR)/lstrlib.c $(LUA_SRC_DIR)/ltablib.c

# Project-specific source files
PROJECT_SOURCES = mainBankZero.c lua_snes.c $(LUA_CORE_SOURCES) $(LUA_LIB_SOURCES)

# Include shared build system
include $(SHARED_BUILD_DIR)/shared-config.mk

# Override C_SOURCES for this project - use real Lua integration
C_SOURCES = mainBankZero.c lua_snes_real.c $(LUA_ESSENTIAL_SOURCES) $(SHARED_SRC_DIR)/initsnes.c
