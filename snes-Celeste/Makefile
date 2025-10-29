# SNES Celeste Project Makefile
# Uses shared build configuration system

# Project-specific settings
PROJECT_NAME = snes-Celeste
BUILD_DIR = build

# Paths to shared build system
SHARED_BUILD_DIR = shared/build
SHARED_SRC_DIR = shared/src
SHARED_PORT_DIR = shared/port

# Project-specific source files
# Source files are in src/ directory
PROJECT_SRC_DIR = src

# Common libfixmath library files
LIBFIXMATH_SOURCES = $(PROJECT_SRC_DIR)/libfixmath/fix16.c
LIBFIXMATH_INCLUDES = -I"$(PROJECT_SRC_DIR)/libfixmath"

# Include shared build system
include $(SHARED_BUILD_DIR)/shared-config.mk

# Override source file paths to point to src/ directory
# This must be done after the include because shared-config.mk conditionally sets these variables
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),wdc816cc)
	PROJECT_C_FILES = $(wildcard $(PROJECT_SRC_DIR)/*.c)
	PROJECT_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .obj,$(basename $(notdir $(PROJECT_C_FILES)))))
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_PORT_DIR)/wdc816cc/lorom/kernel.c $(SHARED_SRC_DIR)/initsnes.c
	OBJECTS = $(PROJECT_OBJECTS) $(BUILD_DIR)/kernel.obj $(BUILD_DIR)/initsnes.obj $(BUILD_DIR)/vectors.obj
	INCLUDES = -I "C:\wdc\Tools\include" -I "$(SHARED_SRC_DIR)" -I "$(PROJECT_SRC_DIR)" -I "include"
	vpath %.c $(PROJECT_SRC_DIR) $(SHARED_PORT_DIR)/wdc816cc/lorom $(SHARED_SRC_DIR) .
	vpath %.h $(PROJECT_SRC_DIR) .
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),vbcc65816)
	PROJECT_C_FILES = $(wildcard $(PROJECT_SRC_DIR)/*.c)
	PROJECT_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(notdir $(PROJECT_C_FILES)))))
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c $(LIBFIXMATH_SOURCES)
	OBJECTS = $(PROJECT_OBJECTS) $(BUILD_DIR)/initsnes.o $(BUILD_DIR)/fix16.o
	# Override CCFLAGS to remove -I"lib" and add project src directory
	CCFLAGS = +snes-hi -lm -maxoptpasses=300 -O4 -inline-depth=1000 -unroll-all -fp-associative -force-statics -range-opt -I"$(SHARED_SRC_DIR)" -I"$(PROJECT_SRC_DIR)" $(LIBFIXMATH_INCLUDES) -I"include" -I"elua-0.9/inc" -I"elua-0.9/inc/snes" -I"elua-0.9/src/lua" -I"elua-0.9/inc/newlib" -D__VBCC__=1 -DLUA_CROSS_COMPILER -D__VBCC65816__ -c
	vpath %.c $(PROJECT_SRC_DIR) $(PROJECT_SRC_DIR)/libfixmath $(SHARED_SRC_DIR) .
	vpath %.h $(PROJECT_SRC_DIR) .
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),calypsi)
	PROJECT_C_FILES = $(wildcard $(PROJECT_SRC_DIR)/*.c)
	PROJECT_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(notdir $(PROJECT_C_FILES)))))
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c $(LIBFIXMATH_SOURCES)
	OBJECTS = $(PROJECT_OBJECTS) $(BUILD_DIR)/initsnes.o $(BUILD_DIR)/fix16.o
	INCLUDES = -I"$(SHARED_SRC_DIR)" -I"$(PROJECT_SRC_DIR)" $(LIBFIXMATH_INCLUDES) -I"include"
	vpath %.c $(PROJECT_SRC_DIR) $(PROJECT_SRC_DIR)/libfixmath $(SHARED_SRC_DIR) .
	vpath %.h $(PROJECT_SRC_DIR) .
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),llvm-mos)
	PROJECT_C_FILES = $(wildcard $(PROJECT_SRC_DIR)/*.c)
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c $(SHARED_PORT_DIR)/llvm-mos/putchar_stub.c $(LIBFIXMATH_SOURCES)
	CCFLAGS += -I$(PROJECT_SRC_DIR) $(LIBFIXMATH_INCLUDES)
	vpath %.c $(PROJECT_SRC_DIR) $(PROJECT_SRC_DIR)/libfixmath $(SHARED_SRC_DIR) $(SHARED_PORT_DIR)/llvm-mos .
	vpath %.h $(PROJECT_SRC_DIR) .
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),cc65)
	PROJECT_C_FILES = $(wildcard $(PROJECT_SRC_DIR)/*.c)
	PROJECT_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(notdir $(PROJECT_C_FILES)))))
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c $(SHARED_PORT_DIR)/cc65/putchar_stub.c $(LIBFIXMATH_SOURCES)
	OBJECTS = $(PROJECT_OBJECTS) $(BUILD_DIR)/initsnes.o $(BUILD_DIR)/putchar_stub.o $(BUILD_DIR)/snes_header.o $(BUILD_DIR)/runtime_stubs.o $(BUILD_DIR)/fix16.o
	CCFLAGS += -I$(PROJECT_SRC_DIR) $(LIBFIXMATH_INCLUDES)
	vpath %.c $(PROJECT_SRC_DIR) $(PROJECT_SRC_DIR)/libfixmath $(SHARED_SRC_DIR) $(SHARED_PORT_DIR)/cc65 .
	vpath %.h $(PROJECT_SRC_DIR) .
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),jcc816)
	PROJECT_C_FILES = $(wildcard $(PROJECT_SRC_DIR)/*.c)
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c $(LIBFIXMATH_SOURCES)
	CCFLAGS += -I$(PROJECT_SRC_DIR) $(LIBFIXMATH_INCLUDES)
	vpath %.c $(PROJECT_SRC_DIR) $(PROJECT_SRC_DIR)/libfixmath $(SHARED_SRC_DIR) .
	vpath %.h $(PROJECT_SRC_DIR) .
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),tcc816)
	PROJECT_C_FILES = $(wildcard $(PROJECT_SRC_DIR)/*.c)
	PROJECT_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(notdir $(PROJECT_C_FILES)))))
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c $(LIBFIXMATH_SOURCES)
	OBJECTS = $(PROJECT_OBJECTS) $(BUILD_DIR)/fix16.o
	CCFLAGS += -I$(PROJECT_SRC_DIR) $(LIBFIXMATH_INCLUDES)
	vpath %.c $(PROJECT_SRC_DIR) $(PROJECT_SRC_DIR)/libfixmath $(SHARED_SRC_DIR) .
	vpath %.h $(PROJECT_SRC_DIR) .
endif

