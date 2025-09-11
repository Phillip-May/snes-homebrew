# SNES Lua Demo Makefile
# Uses shared build configuration system

# Project-specific settings
PROJECT_NAME = DemoLua548
BUILD_DIR = build
ROM_TYPE = huge
ROM_MAPPING = HiROM

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
                        $(LUA_SRC_DIR)/lopcodes.c $(LUA_SRC_DIR)/lparser.c \
                        $(LUA_SRC_DIR)/lstring.c $(LUA_SRC_DIR)/ltable.c $(LUA_SRC_DIR)/ltm.c \
                        $(LUA_SRC_DIR)/lundump.c $(LUA_SRC_DIR)/lvm.c $(LUA_SRC_DIR)/lzio.c \
                        $(LUA_SRC_DIR)/lauxlib.c $(LUA_SRC_DIR)/lbaselib.c $(LUA_SRC_DIR)/lmathlib.c \
                        $(LUA_SRC_DIR)/lstrlib.c $(LUA_SRC_DIR)/ltablib.c

# Problematic files that need to be compiled without huge attribute
LUA_PROBLEMATIC_SOURCES = $(LUA_SRC_DIR)/lstate.c

# Include shared build system
include $(SHARED_BUILD_DIR)/shared-config.mk

# Override C_SOURCES for this project - use real Lua integration with Calypsi
# This must come after the include to override the shared config
C_SOURCES = mainBankZero.c snes_memory_manager.c $(LUA_ESSENTIAL_SOURCES) $(SHARED_SRC_DIR)/initsnes.c

# Override OBJECTS to include all Lua object files
LUA_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(notdir $(LUA_ESSENTIAL_SOURCES)))))
LUA_PROBLEMATIC_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(notdir $(LUA_PROBLEMATIC_SOURCES)))))
OBJECTS = $(BUILD_DIR)/mainBankZero.o $(BUILD_DIR)/snes_memory_manager.o $(LUA_OBJECTS) $(LUA_PROBLEMATIC_OBJECTS) $(BUILD_DIR)/initsnes.o

# Override vpath to include Lua source directory
vpath %.c $(LUA_SRC_DIR) $(SHARED_SRC_DIR) .

# Custom build rule for problematic files (compile without huge attribute)
$(BUILD_DIR)/lstate.o: $(LUA_SRC_DIR)/lstate.c
	@mkdir $(BUILD_DIR) 2>nul || echo Build directory exists
	"C:\calypsi-65816-5.11\bin\cc65816" --core=65816 -O2 --speed --code-model=large --data-model=large --list-file=$(BUILD_DIR)/calypsi.lst -D__CALYPSI__=1 -I"$(SHARED_SRC_DIR)" -I"lib" -I"include" -o $@ $<
