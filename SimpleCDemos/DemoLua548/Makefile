# DemoLua548 (eLua 0.9, integer-only, Calypsi huge/HiROM)
SUPPORTED_COMPILERS = calypsi
ROM_TYPE = huge
ROM_MAPPING = HiROM
USE_FLOATING_POINT = 0

# eLua 0.9 source files for SNES
LUA_SRC_DIR = elua-0.9/src/lua
PLATFORM_SRC_DIR = elua-0.9/src/platform/snes
LUA_ESSENTIAL_SOURCES = $(LUA_SRC_DIR)/lstate.c $(LUA_SRC_DIR)/lauxlib.c \
                        $(LUA_SRC_DIR)/lvm.c $(LUA_SRC_DIR)/lstring.c \
                        $(LUA_SRC_DIR)/lobject.c $(LUA_SRC_DIR)/lfunc.c \
                        $(LUA_SRC_DIR)/ltable.c $(LUA_SRC_DIR)/ltm.c \
                        $(LUA_SRC_DIR)/ldo.c $(LUA_SRC_DIR)/ldebug.c \
                        $(LUA_SRC_DIR)/lgc.c $(LUA_SRC_DIR)/lparser.c \
                        $(LUA_SRC_DIR)/lmem.c $(LUA_SRC_DIR)/lcode.c \
                        $(LUA_SRC_DIR)/lopcodes.c $(LUA_SRC_DIR)/llex.c \
                        $(LUA_SRC_DIR)/lundump.c $(LUA_SRC_DIR)/ldump.c $(LUA_SRC_DIR)/lzio.c \
                        $(LUA_SRC_DIR)/lapi.c $(LUA_SRC_DIR)/lbaselib.c \
                        $(LUA_SRC_DIR)/ldblib.c $(LUA_SRC_DIR)/liolib.c $(LUA_SRC_DIR)/lmathlib.c \
                        $(LUA_SRC_DIR)/loslib.c $(LUA_SRC_DIR)/lstrlib.c $(LUA_SRC_DIR)/ltablib.c \
                        $(LUA_SRC_DIR)/loadlib.c $(LUA_SRC_DIR)/linit.c \
                        $(LUA_SRC_DIR)/lrotable.c \
                        snes_stubs.c

include ../shared/build/shared-config.mk

C_SOURCES = mainBankZero.c snes_memory_manager.c $(LUA_ESSENTIAL_SOURCES) $(SHARED_SRC_DIR)/initsnes.c
INCLUDES += -I"elua-0.9/inc" -I"elua-0.9/inc/snes" -I"elua-0.9/src/lua" -I"elua-0.9/inc/newlib"
CCFLAGS += -DLUA_NUMBER_INTEGRAL -DNO_FLOAT -DNO_DOUBLE -D__STRICT_ANSI__ -DLUA_ANSI

ifeq ($(ROM_TYPE),huge)
	STDLIB =
endif

LUA_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(notdir $(LUA_ESSENTIAL_SOURCES)))))
OBJECTS = $(BUILD_DIR)/mainBankZero.o $(BUILD_DIR)/snes_memory_manager.o $(LUA_OBJECTS) $(BUILD_DIR)/initsnes.o

vpath %.c $(LUA_SRC_DIR) $(PLATFORM_SRC_DIR) elua-0.9/src $(SHARED_SRC_DIR) .
