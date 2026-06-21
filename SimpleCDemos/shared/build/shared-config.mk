# Shared SNES C Compiler Build Configuration
# This file contains all compiler configurations, source definitions, and build rules

# Set default paths (can be overridden by individual projects)
SHARED_BUILD_DIR ?= ../shared/build
SHARED_SRC_DIR ?= ../shared/src
SHARED_PORT_DIR ?= ../shared/port
BUILD_DIR ?= build
PROJECT_NAME ?= $(notdir $(CURDIR))
COMPILERS := wdc816cc vbcc65816 calypsi llvm-mos cc65 jcc816 tcc816
SUPPORTED_COMPILERS ?= $(COMPILERS)

# Assets a project regenerates from source art at build time (see per-project
# Makefiles). Generated .inc files live under $(BUILD_DIR) and are #included by
# the project sources; empty for demos with no converted assets.
ASSETS ?=

# Lowercased compiler name, computed once (was re-forked sh+echo+tr 27 times).
COMPILER_LC := $(shell echo $(COMPILER) | tr A-Z a-z)

# =============================================================================
# TOOLCHAIN LOCATIONS
# =============================================================================
# All compiler/tool locations are configured in toolchains.mk (git-ignored).
# Copy toolchains.mk.example to toolchains.mk and edit the paths there - that
# file is the single place to point the build at your installed toolchains.
# The ?= values below are only a fallback for the conventional Windows locations.
ifeq ($(wildcard $(SHARED_BUILD_DIR)/toolchains.mk),)
$(warning toolchains.mk not found - using built-in default tool paths. Copy shared/build/toolchains.mk.example to toolchains.mk and edit it for your machine.)
else
include $(SHARED_BUILD_DIR)/toolchains.mk
endif

# Host tools
PYTHON          ?= python

# Compiler / toolchain install roots (set these in toolchains.mk).
# Use FORWARD slashes: GNU make runs recipes through sh.exe, which strips
# backslashes from unquoted paths. Forward slashes are accepted by Windows tools.
WDC_HOME        ?= C:/wdc/Tools
VBCC_HOME       ?= C:/vbcc65816/vbcc65816/vbcc65816_win/vbcc
CALYPSI_HOME    ?= C:/calypsi-65816-5.12
LLVM_MOS_HOME   ?= C:/llvm-mos
CC65_HOME       ?= C:/cc65-2.19
PVSNESLIB_HOME  ?= C:/pvsneslib
JCC816_HOME     ?= C:/path/to/JCC816

# Extra roots only needed by the jcc816 toolchain wrapper
JAVA_HOME       ?= C:/path/to/jdk/bin
JCC816_CC65_HOME ?= C:/path/to/cc65-snapshot/bin

# Export so the .bat / .py port wrappers (run as child processes) inherit them
export PYTHON WDC_HOME VBCC_HOME CALYPSI_HOME LLVM_MOS_HOME CC65_HOME
export PVSNESLIB_HOME JCC816_HOME JAVA_HOME JCC816_CC65_HOME

# Check if compiler is specified (skip for help, clean, and convenience targets)
ifneq ($(filter help clean info print-supported $(COMPILERS),$(MAKECMDGOALS)),)
# Skip compiler check for help, clean, and convenience targets
else
ifeq ($(COMPILER),)
$(error Please specify a compiler. Usage: make COMPILER=wdc816cc, make COMPILER=vbcc65816, make COMPILER=calypsi, make COMPILER=llvm-mos, make COMPILER=cc65, make COMPILER=jcc816, or make COMPILER=tcc816 (case-insensitive))
endif
ifeq ($(filter $(COMPILER_LC),$(SUPPORTED_COMPILERS)),)
ifneq ($(ALLOW_UNSUPPORTED),1)
$(error $(PROJECT_NAME) does not support COMPILER=$(COMPILER_LC). Supported: $(SUPPORTED_COMPILERS). Set ALLOW_UNSUPPORTED=1 to try anyway.)
endif
endif
endif

# =============================================================================
# COMPILER CONFIGURATIONS
# =============================================================================

# WDC816CC Configuration
ifeq ($(COMPILER_LC),wdc816cc)
	CC = "$(WDC_HOME)/bin/wdc816cc"
	AS = "$(WDC_HOME)/bin/wdc816as"
	LD = "$(WDC_HOME)/bin/wdcln"
	CCFLAGS = -WL -SM -MK -MT -ML -WP -MU -MV -SI -SP -D__WDC816CC__=1
	ASFLAGS = 
	# Use floating point math library if USE_FLOATING_POINT is set
	# WDC816CC requires both -Lml (math library) and -Lcl (standard library) for floating point
	ifeq ($(USE_FLOATING_POINT),1)
		LDFLAGS = -HB -ML -B -E -T -C018000,008000 $(PROJECT_OBJECTS) $(BUILD_DIR)/vectors.obj -C028000,010000 $(BUILD_DIR)/kernel.obj $(BUILD_DIR)/initsnes.obj -D7E2000,18000 -K048000,20000 -Lml -Lcl -O$(BUILD_DIR)/mainBankZero.bin
	else
		LDFLAGS = -HB -ML -B -E -T -C018000,008000 $(PROJECT_OBJECTS) $(BUILD_DIR)/vectors.obj -C028000,010000 $(BUILD_DIR)/kernel.obj $(BUILD_DIR)/initsnes.obj -D7E2000,18000 -K048000,20000 -Lcl -O$(BUILD_DIR)/mainBankZero.bin
	endif
	INCLUDES = -I "$(WDC_HOME)/include" -I "$(SHARED_SRC_DIR)" -I "lib" -I "include"
	OUTPUT_EXT = .bin
	POST_LINK = @powershell -Command "if (Test-Path '$(BUILD_DIR)/mainBankZero.bin') { Copy-Item '$(BUILD_DIR)/mainBankZero.bin' '$(BUILD_DIR)/mainBankZero_wdc816cc.smc' }"
	COMPILER_NAME = wdc816cc
endif

# VBCC65816 Configuration
ifeq ($(COMPILER_LC),vbcc65816)
	# Use the existing batch file approach but with direct calls
	CC = "$(SHARED_PORT_DIR)/vbcc816/vc_env.bat"
	AS = "$(SHARED_PORT_DIR)/vbcc816/vc_env.bat"
	LD = "$(SHARED_PORT_DIR)/vbcc816/vlink_direct.bat"
	
	# Compiler flags for incremental compilation
	CCFLAGS = +snes-hi -lm -maxoptpasses=300 -O4 -inline-depth=1000 -unroll-all -fp-associative -force-statics -range-opt -I"$(SHARED_SRC_DIR)" -I"lib" -I"include" -I"elua-0.9/inc" -I"elua-0.9/inc/snes" -I"elua-0.9/src/lua" -I"elua-0.9/inc/newlib" -D__VBCC__=1 -DLUA_CROSS_COMPILER -D__VBCC65816__ -c
	ASFLAGS = -816 -quiet -nowarn=62 -opt-branch -ldots -Fvobj
	LDFLAGS = +snes-hi -lm -maxoptpasses=300 -O4 -inline-depth=1000 -unroll-all -fp-associative -force-statics -range-opt -I"$(SHARED_SRC_DIR)" -I"lib" -I"include" -I"elua-0.9/inc" -I"elua-0.9/inc/snes" -I"elua-0.9/src/lua" -I"elua-0.9/inc/newlib" -D__VBCC__=1 -DLUA_CROSS_COMPILER -D__VBCC65816__
	INCLUDES = 
	OUTPUT_EXT = .smc
	POST_LINK = 
	COMPILER_NAME = vbcc65816
endif

# Calypsi Configuration
ifeq ($(COMPILER_LC),calypsi)
	CC = "$(CALYPSI_HOME)/bin/cc65816"
	AS = "$(CALYPSI_HOME)/bin/cc65816"
	LD = "$(CALYPSI_HOME)/bin/ln65816"
	# Check for huge model - requires --enable-huge-attribute with large data model
	ifeq ($(ROM_TYPE),huge)
		# huge model is pinned to -O0; higher opt levels have not been validated here.
		CCFLAGS += --core=65816 -O0 --code-model=large --data-model=huge --target=SNES --list-file=$(BUILD_DIR)/calypsi.lst -D__CALYPSI__=1
		STDLIB = $(CALYPSI_HOME)/lib-huge/clib-huge.a
		LDFLAGS = --raw-multiple-memories --rom-code --no-tree-shaking --no-copy-initialize huge
	else
		CCFLAGS += --core=65816 -O2 --speed --code-model=large --data-model=large --target=SNES --list-file=$(BUILD_DIR)/calypsi.lst -D__CALYPSI__=1
		STDLIB = $(CALYPSI_HOME)/lib/clib-lc-ld-snes.a
		LDFLAGS = --raw-multiple-memories --rom-code
	endif
	ASFLAGS =
	INCLUDES = -I"$(SHARED_SRC_DIR)" -I"lib" -I"include"
	OUTPUT_EXT = .smc
	# ROM type selection: huge, HiROM, or default LoROM
	ifeq ($(ROM_TYPE),huge)
		ifeq ($(ROM_MAPPING),HiROM)
			LINKER_SCRIPT = $(SHARED_PORT_DIR)/calypsi/linker-large-large-HiROM.scm
			POST_LINK = @$(PYTHON) $(SHARED_PORT_DIR)/calypsi/ConvertIntelHex_HiROM.py $(BUILD_DIR)/calypsi.hex $(BUILD_DIR)/mainBankZero_calypsi.smc
		else
			LINKER_SCRIPT = $(SHARED_PORT_DIR)/calypsi/linker-large-large-LoROM.scm
			POST_LINK = @$(PYTHON) $(SHARED_PORT_DIR)/calypsi/ConvertIntelHex_LoROM.py $(BUILD_DIR)/calypsi.hex $(BUILD_DIR)/mainBankZero_calypsi.smc
		endif
	else
		ifeq ($(ROM_TYPE),HiROM)
			LINKER_SCRIPT = $(SHARED_PORT_DIR)/calypsi/linker-large-large-HiROM.scm
			POST_LINK = @$(PYTHON) $(SHARED_PORT_DIR)/calypsi/ConvertIntelHex_HiROM.py $(BUILD_DIR)/calypsi.hex $(BUILD_DIR)/mainBankZero_calypsi.smc
		else
			LINKER_SCRIPT = $(SHARED_PORT_DIR)/calypsi/linker-large-large-LoROM.scm
			POST_LINK = @$(PYTHON) $(SHARED_PORT_DIR)/calypsi/ConvertIntelHex_LoROM.py $(BUILD_DIR)/calypsi.hex $(BUILD_DIR)/mainBankZero_calypsi.smc
		endif
	endif
	CALYPSI_PATH = $(CALYPSI_HOME)
	COMPILER_NAME = calypsi
endif

# LLVM-Mos Configuration
ifeq ($(COMPILER_LC),llvm-mos)
	CC = mos-common-clang
	AS = mos-common-clang
	LD = mos-common-clang
	CCFLAGS = -mcpu=mosw65816 -I$(SHARED_SRC_DIR) -Iinclude -T $(SHARED_PORT_DIR)/llvm-mos/linker.ld -Os -flto -fnonreentrant -ffast-math -funroll-loops -finline-functions -fomit-frame-pointer -fno-stack-protector -fdata-sections -ffunction-sections
	ASFLAGS = 
	LDFLAGS = -lexit-loop
	INCLUDES = 
	OUTPUT_EXT = .smc
	POST_LINK = 
	LLVM_MOS_PATH = $(LLVM_MOS_HOME)
	COMPILER_NAME = llvm-mos
	# Map file generation (can be overridden by project Makefiles)
	MAP_FILE = $(BUILD_DIR)/mainBankZero_llvm-mos.map
endif

# CC65 Configuration
ifeq ($(COMPILER_LC),cc65)
	CC = $(CC65_HOME)/bin/cc65
	AS = $(CC65_HOME)/bin/ca65
	LD = $(CC65_HOME)/bin/ld65
	CCFLAGS = -t none -O -I$(SHARED_SRC_DIR) -Iinclude -D__CC65__=1
	ASFLAGS = -t none
	LDFLAGS = -C $(SHARED_PORT_DIR)/cc65/snes.cfg -o -m $(BUILD_DIR)/mainBankZero_cc65.map --no-smc
	INCLUDES = 
	OUTPUT_EXT = .smc
	POST_LINK = 
	CC65_PATH = $(CC65_HOME)
	COMPILER_NAME = cc65
endif

# JCC816 Configuration
ifeq ($(COMPILER_LC),jcc816)
	CC = $(PYTHON) $(SHARED_PORT_DIR)/jcc816/compile.py
	AS = $(PYTHON) $(SHARED_PORT_DIR)/jcc816/compile.py
	LD = $(PYTHON) $(SHARED_PORT_DIR)/jcc816/compile.py
	CCFLAGS = -l example=$(SHARED_PORT_DIR)/jcc816/exampleHeader.xml -O 0 -D 2 -V 2 -r build
	ASFLAGS = 
	LDFLAGS = 
	INCLUDES = 
	OUTPUT_EXT = .sfc
	POST_LINK = 
	JCC816_PATH = $(JCC816_HOME)
	COMPILER_NAME = jcc816
endif

# TCC816 (pvsneslib) Configuration
ifeq ($(COMPILER_LC),tcc816)
	CC = $(PYTHON) $(SHARED_PORT_DIR)/tcc816/compile.py
	AS = $(PYTHON) $(SHARED_PORT_DIR)/tcc816/compile.py
	LD = $(PYTHON) $(SHARED_PORT_DIR)/tcc816/compile.py
	CCFLAGS = -c -I$(SHARED_SRC_DIR) -Iinclude -I.
	ASFLAGS = 
	LDFLAGS = 
	INCLUDES = 
	OUTPUT_EXT = .obj
	POST_LINK = 
	TCC816_PATH = $(PVSNESLIB_HOME)
	COMPILER_NAME = tcc816
endif

# =============================================================================
# SOURCE CONFIGURATIONS
# =============================================================================

# WDC816CC Source Configuration
ifeq ($(COMPILER_LC),wdc816cc)
	# Automatically include all C files in current directory
	PROJECT_C_FILES = $(wildcard *.c)
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_PORT_DIR)/wdc816cc/lorom/kernel.c $(SHARED_SRC_DIR)/initsnes.c
	ASM_SOURCES = $(SHARED_PORT_DIR)/wdc816cc/lorom/vectors.asm
	# Generate object file names from C sources
	PROJECT_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .obj,$(basename $(PROJECT_C_FILES))))
	OBJECTS = $(PROJECT_OBJECTS) $(BUILD_DIR)/kernel.obj $(BUILD_DIR)/initsnes.obj $(BUILD_DIR)/vectors.obj
	vpath %.c $(SHARED_PORT_DIR)/wdc816cc/lorom $(SHARED_SRC_DIR) .
	vpath %.asm $(SHARED_PORT_DIR)/wdc816cc/lorom
	vpath %.h .
endif

# VBCC65816 Source Configuration
ifeq ($(COMPILER_LC),vbcc65816)
	# Automatically include all C files in current directory
	PROJECT_C_FILES = $(wildcard *.c)
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c
	ASM_SOURCES = 
	# Generate object file names from C sources for incremental linking
	PROJECT_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(PROJECT_C_FILES))))
	OBJECTS = $(PROJECT_OBJECTS) $(BUILD_DIR)/initsnes.o
	vpath %.c $(SHARED_SRC_DIR) .
	vpath %.asm 
	vpath %.h .
endif

# Calypsi Source Configuration
ifeq ($(COMPILER_LC),calypsi)
	# Automatically include all C files in current directory
	PROJECT_C_FILES = $(wildcard *.c)
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c
	ASM_SOURCES = 
	# Generate object file names from C sources
	PROJECT_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(PROJECT_C_FILES))))
	OBJECTS = $(PROJECT_OBJECTS) $(BUILD_DIR)/initsnes.o
	vpath %.c $(SHARED_SRC_DIR) .
	vpath %.asm 
	vpath %.h .
endif

# LLVM-Mos Source Configuration
ifeq ($(COMPILER_LC),llvm-mos)
	# Automatically include all C files in current directory
	PROJECT_C_FILES = $(wildcard *.c)
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c $(SHARED_PORT_DIR)/llvm-mos/putchar_stub.c
	ASM_SOURCES = $(SHARED_PORT_DIR)/llvm-mos/vectors.s $(SHARED_PORT_DIR)/llvm-mos/startup.s $(SHARED_PORT_DIR)/llvm-mos/bankswitch.s
	OBJECTS = 
	vpath %.c $(SHARED_SRC_DIR) $(SHARED_PORT_DIR)/llvm-mos .
	vpath %.s $(SHARED_PORT_DIR)/llvm-mos
	vpath %.asm 
	vpath %.h .
endif

# CC65 Source Configuration
ifeq ($(COMPILER_LC),cc65)
	# Automatically include all C files in current directory
	PROJECT_C_FILES = $(wildcard *.c)
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c $(SHARED_PORT_DIR)/cc65/putchar_stub.c
	ASM_SOURCES = $(SHARED_PORT_DIR)/cc65/snes_header.s $(SHARED_PORT_DIR)/cc65/runtime_stubs.s
	# Generate object file names from C sources
	PROJECT_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(PROJECT_C_FILES))))
	OBJECTS = $(PROJECT_OBJECTS) $(BUILD_DIR)/initsnes.o $(BUILD_DIR)/putchar_stub.o $(BUILD_DIR)/snes_header.o $(BUILD_DIR)/runtime_stubs.o
	vpath %.c $(SHARED_SRC_DIR) $(SHARED_PORT_DIR)/cc65 .
	vpath %.asm $(SHARED_PORT_DIR)/cc65
	vpath %.h .
endif

# JCC816 Source Configuration
ifeq ($(COMPILER_LC),jcc816)
	# Automatically include all C files in current directory
	PROJECT_C_FILES = $(wildcard *.c)
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c
	ASM_SOURCES = 
	OBJECTS = 
	vpath %.c $(SHARED_SRC_DIR) .
	vpath %.asm 
	vpath %.h .
endif

# TCC816 Source Configuration
ifeq ($(COMPILER_LC),tcc816)
	# Automatically include all C files in current directory
	PROJECT_C_FILES = $(wildcard *.c)
	C_SOURCES = $(PROJECT_C_FILES) $(SHARED_SRC_DIR)/initsnes.c
	ASM_SOURCES = 
	# Generate object file names from C sources
	PROJECT_OBJECTS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(PROJECT_C_FILES))))
	OBJECTS = $(PROJECT_OBJECTS)
	vpath %.c $(SHARED_SRC_DIR) .
	vpath %.asm 
	vpath %.h .
endif

# =============================================================================
# BUILD RULES
# =============================================================================

# Default target
all: $(BUILD_DIR) $(ASSETS)
ifeq ($(COMPILER_LC),wdc816cc)
	@$(MAKE) $(OBJECTS)
	$(LD) $(LDFLAGS)
	$(POST_LINK)
else
ifeq ($(COMPILER_LC),calypsi)
	@$(MAKE) $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LINKER_SCRIPT) $(STDLIB) --list-file=$(BUILD_DIR)/calypsi.lst --cross-reference --output-format=intel-hex -o $(BUILD_DIR)/calypsi.hex
	$(POST_LINK)
else
ifeq ($(COMPILER_LC),llvm-mos)
	@echo "Compiling with LLVM-MOS..."
	@echo "$(CC) $(CCFLAGS) $(LDFLAGS) -o $(BUILD_DIR)/mainBankZero_llvm-mos$(OUTPUT_EXT) $(C_SOURCES) $(ASM_SOURCES)"
	@powershell -Command "& '$(SHARED_PORT_DIR)/llvm-mos/compile.bat' '$(CC)' $(CCFLAGS) $(LDFLAGS) -o '$(BUILD_DIR)/mainBankZero_llvm-mos$(OUTPUT_EXT)' $(C_SOURCES) $(ASM_SOURCES)"
	@echo "Compilation completed successfully"
	$(POST_LINK)
else
ifeq ($(COMPILER_LC),cc65)
	@$(MAKE) $(OBJECTS)
	$(LD) -C $(SHARED_PORT_DIR)/cc65/snes.cfg -o $(BUILD_DIR)/mainBankZero_cc65$(OUTPUT_EXT) -m $(BUILD_DIR)/mainBankZero_cc65.map $(OBJECTS) $(CC65_HOME)/lib/none.lib
	$(POST_LINK)
else
ifeq ($(COMPILER_LC),jcc816)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(C_SOURCES)
	$(POST_LINK)
else
ifeq ($(COMPILER_LC),tcc816)
	$(CC) $(CCFLAGS) $(INCLUDES) $(C_SOURCES)
	$(POST_LINK)
else
ifeq ($(COMPILER_LC),vbcc65816)
	@echo "Compiling with VBCC65816 (incremental compilation)..."
	@$(MAKE) $(OBJECTS)
	@echo "Linking object files with vbcc..."
	@echo "OBJECTS variable: $(OBJECTS)"
	@echo "Compiling and linking all sources together..."
	$(CC) $(LDFLAGS) $(C_SOURCES) -o $(BUILD_DIR)/mainBankZero_vbcc65816$(OUTPUT_EXT)
	@echo "Compilation completed successfully"
	$(POST_LINK)
else
	@echo "Unknown compiler: $(COMPILER)"
	@echo "Available compilers: wdc816cc, vbcc65816, calypsi, llvm-mos, cc65, jcc816, tcc816"
	@exit 1
endif
endif
endif
endif
endif
endif
endif

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# WDC816CC specific rules
ifeq ($(COMPILER_LC),wdc816cc)
# Compile C sources (pattern rule)
$(BUILD_DIR)/%.obj: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ $<

# Assemble ASM sources (pattern rule)
$(BUILD_DIR)/%.obj: %.asm
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) -o $@ $<
endif

# Calypsi specific rules
ifeq ($(COMPILER_LC),calypsi)
# Compile C sources (pattern rule)
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ $<
endif

# TCC816 specific rules
ifeq ($(COMPILER_LC),tcc816)
# Compile C sources (pattern rule) - don't pass -o for ROM creation
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(INCLUDES) $<
endif

# VBCC65816 specific rules
ifeq ($(COMPILER_LC),vbcc65816)
# Compile C sources (pattern rule) - compile to object files for incremental linking
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ $<

# Assemble ASM sources (pattern rule) - if needed
$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) -o $@ $<
endif

# CC65 specific rules
ifeq ($(COMPILER_LC),cc65)
# Compile C sources (pattern rule)
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$*.s $<
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/$*.s

# Assemble ASM sources (pattern rule)
$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) -o $@ $<

# Specific rule for snes_header.s
$(BUILD_DIR)/snes_header.o: $(SHARED_PORT_DIR)/cc65/snes_header.s
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) -o $@ $<

# Specific rule for runtime_stubs.s
$(BUILD_DIR)/runtime_stubs.o: $(SHARED_PORT_DIR)/cc65/runtime_stubs.s
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) -o $@ $<
endif

# Clean target
clean:
	@rm -rf $(BUILD_DIR) *.obj *.bin *.bnk *.map *.smc PROG.LINK
	@echo Clean complete!

# Convenience aliases: `make <compiler>` == `make clean && make COMPILER=<compiler>`
$(COMPILERS): clean
	@$(MAKE) COMPILER=$@

# Show current compiler
info:
	@echo Current compiler: $(COMPILER)
	@echo C sources: $(C_SOURCES)
	@echo Objects: $(OBJECTS)

print-supported:
	@echo $(SUPPORTED_COMPILERS)

# Help target
help:
	@echo Available targets:
	@echo "  make COMPILER=wdc816cc  - Build with WDC816CC compiler"
	@echo "  make COMPILER=vbcc65816 - Build with VBCC65816 compiler"
	@echo "  make COMPILER=calypsi   - Build with Calypsi compiler"
	@echo "  make COMPILER=llvm-mos  - Build with LLVM-Mos compiler"
	@echo "  make COMPILER=cc65      - Build with CC65 compiler"
	@echo "  make COMPILER=jcc816    - Build with JCC816 compiler"
	@echo "  make COMPILER=tcc816    - Build with TCC816 (pvsneslib) compiler"
	@echo "  make <compiler>         - Alias for 'make clean && make COMPILER=<compiler>'"
	@echo "  make clean              - Clean build artifacts"
	@echo "  make info               - Show current compiler info"
	@echo "  make help               - Show this help message"
	@echo ""
	@echo Calypsi ROM type options:
	@echo "  ROM_TYPE=HiROM          - Use HiROM mapping (default: LoROM)"
	@echo "  ROM_TYPE=huge           - Use huge data/code model with --enable-huge-attribute"
	@echo "  ROM_MAPPING=HiROM       - Use HiROM mapping when ROM_TYPE=huge (default: LoROM)"
	@echo ""
	@echo Examples:
	@echo "  make COMPILER=calypsi ROM_TYPE=HiROM"
	@echo "  make COMPILER=calypsi ROM_TYPE=huge"
	@echo "  make COMPILER=calypsi ROM_TYPE=huge ROM_MAPPING=HiROM"

# Phony targets
.PHONY: all clean info help print-supported $(COMPILERS)
