# SNES C Compiler Demo Makefile

# Build directory
BUILD_DIR = build

# Compiler selection (wdc816cc, vbcc816, calypsi, llvm-mos, cc65, jcc816, or tcc816)
COMPILER ?= 

# Check if compiler is specified (skip for help, clean, and convenience targets)
ifneq ($(filter help clean wdc vbcc calypsi llvm-mos cc65 jcc816 tcc816,$(MAKECMDGOALS)),)
# Skip compiler check for help, clean, and convenience targets
else
ifeq ($(COMPILER),)
$(error Please specify a compiler. Usage: make COMPILER=wdc816cc, make COMPILER=vbcc816, make COMPILER=calypsi, make COMPILER=llvm-mos, make COMPILER=cc65, make COMPILER=jcc816, or make COMPILER=tcc816 (case-insensitive))
endif
endif

# WDC816CC Configuration
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),wdc816cc)
	CC = wdc816cc
	AS = wdc816as
	LD = wdcln
	CCFLAGS = -WL -SM -MK -MT -ML -WP -MU -MV -SI -SP -D__WDC816CC__=1
	ASFLAGS = 
	LDFLAGS = -HB -ML -B -E -T -C018000,008000 $(BUILD_DIR)/mainBankZero.obj $(BUILD_DIR)/vectors.obj -C028000,010000 $(BUILD_DIR)/kernel.obj $(BUILD_DIR)/initsnes.obj -D7E2000,18000 -Lcl
	INCLUDES = -I "C:\wdc\Tools\include" -I "C:\Users\Admin\Documents\snes-homebrew\SimpleCDemos\shared\src" -I "lib"
	OUTPUT_EXT = .bin
	POST_LINK = @powershell -Command "if (Test-Path '$(BUILD_DIR)/mainBankZero.bin') { Copy-Item '$(BUILD_DIR)/mainBankZero.bin' '$(BUILD_DIR)/mainBankZero_wdc816cc.smc' }"
endif

# VBCC65816 Configuration
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),vbcc65816)
	CC = "C:\vbcc65816\vbcc65816\vbcc65816_win\vbcc\bin\vc"
	AS = "C:\vbcc65816\vbcc65816\vbcc65816_win\vbcc\bin\vc"
	LD = "C:\vbcc65816\vbcc65816\vbcc65816_win\vbcc\bin\vc"
	CCFLAGS = +snes-hi -lm -maxoptpasses=300 -O4 -inline-depth=1000 -unroll-all -fp-associative -force-statics -range-opt -I"../shared/src" -I"lib"
	ASFLAGS = 
	LDFLAGS = +snes-hi -lm -maxoptpasses=300 -O4 -inline-depth=1000 -unroll-all -fp-associative -force-statics -range-opt
	INCLUDES = 
	OUTPUT_EXT = .smc
	POST_LINK = 
	VBCC_PATH = C:\vbcc65816\vbcc65816\vbcc65816_win\vbcc
endif

# Calypsi Configuration
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),calypsi)
	CC = "C:\calypsi-65816-5.11\bin\cc65816"
	AS = "C:\calypsi-65816-5.11\bin\cc65816"
	LD = "C:\calypsi-65816-5.11\bin\ln65816"
	CCFLAGS = --core=65816 -O2 --speed --code-model=large --data-model=large --list-file=$(BUILD_DIR)/calypsi.lst -D__CALYPSI__=1
	ASFLAGS = 
	LDFLAGS = --raw-multiple-memories --rom-code
	INCLUDES = -I"../shared/src" -I"lib"
	OUTPUT_EXT = .smc
	POST_LINK = @C:\Python310\python.exe ../shared/port/calypsi/ConvertIntelHex.py $(BUILD_DIR)/calypsi.hex $(BUILD_DIR)/mainBankZero_calypsi.smc
	CALYPSI_PATH = C:\calypsi-65816-5.11
endif

# LLVM-Mos Configuration
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),llvm-mos)
	CC = mos-common-clang
	AS = mos-common-clang
	LD = mos-common-clang
	CCFLAGS = -I../shared/src -T ../shared/port/llvm-mos/linker.ld -O3 -D__mos__=1 -ffast-math -funroll-loops -finline-functions -fomit-frame-pointer -fno-stack-protector
	ASFLAGS = 
	LDFLAGS = -lexit-loop -linit-stack
	INCLUDES = 
	OUTPUT_EXT = .smc
	POST_LINK = 
	LLVM_MOS_PATH = C:\llvm-mos
endif

# CC65 Configuration
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),cc65)
	CC = C:\cc65-2.19\bin\cc65
	AS = C:\cc65-2.19\bin\ca65
	LD = C:\cc65-2.19\bin\ld65
	CCFLAGS = -t none -O -I../shared/src -D__CC65__=1
	ASFLAGS = -t none
	LDFLAGS = -C ../shared/port/cc65/snes.cfg -o -m $(BUILD_DIR)/mainBankZero_cc65.map --no-smc
	INCLUDES = 
	OUTPUT_EXT = .smc
	POST_LINK = 
	CC65_PATH = C:\cc65-2.19
endif

# JCC816 Configuration
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),jcc816)
	CC = python ..\shared\port\jcc816\compile.py
	AS = python ..\shared\port\jcc816\compile.py
	LD = python ..\shared\port\jcc816\compile.py
	CCFLAGS = -l example=../shared/port/jcc816/exampleHeader.xml -O 0 -D 2 -V 2 -r build
	ASFLAGS = 
	LDFLAGS = 
	INCLUDES = 
	OUTPUT_EXT = .sfc
	POST_LINK = 
	JCC816_PATH = C:\Users\Admin\Documents\JCC816
endif

# TCC816 (pvsneslib) Configuration
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),tcc816)
	CC = python ..\shared\port\tcc816\compile.py
	AS = python ..\shared\port\tcc816\compile.py
	LD = python ..\shared\port\tcc816\compile.py
	CCFLAGS = -c -I../shared/src -I.
	ASFLAGS = 
	LDFLAGS = 
	INCLUDES = 
	OUTPUT_EXT = .obj
	POST_LINK = 
	TCC816_PATH = C:\pvsneslib
endif

# Source files (compiler-specific)
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),wdc816cc)
	C_SOURCES = mainBankZero.c ../shared/port/wdc816cc/lorom/kernel.c ../shared/src/initsnes.c
	ASM_SOURCES = ../shared/port/wdc816cc/lorom/vectors.asm
	OBJECTS = $(BUILD_DIR)/mainBankZero.obj $(BUILD_DIR)/kernel.obj $(BUILD_DIR)/initsnes.obj $(BUILD_DIR)/vectors.obj
	vpath %.c ../shared/port/wdc816cc/lorom ../shared/src
	vpath %.asm ../shared/port/wdc816cc/lorom
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),vbcc65816)
	C_SOURCES = mainBankZero.c ../shared/src/initsnes.c
	ASM_SOURCES = 
	OBJECTS = 
	vpath %.c ../shared/src
	vpath %.asm 
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),calypsi)
	C_SOURCES = mainBankZero.c ../shared/src/initsnes.c
	ASM_SOURCES = 
	OBJECTS = $(BUILD_DIR)/mainBankZero.o $(BUILD_DIR)/initsnes.o
	vpath %.c ../shared/src
	vpath %.asm 
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),llvm-mos)
	C_SOURCES = mainBankZero.c ../shared/src/initsnes.c ../shared/port/llvm-mos/putchar_stub.c
	ASM_SOURCES = 
	OBJECTS = 
	vpath %.c ../shared/src ../shared/port/llvm-mos
	vpath %.asm 
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),cc65)
	C_SOURCES = mainBankZero.c ../shared/src/initsnes.c ../shared/port/cc65/putchar_stub.c
	ASM_SOURCES = ../shared/port/cc65/snes_header.s ../shared/port/cc65/runtime_stubs.s
	OBJECTS = $(BUILD_DIR)/mainBankZero.o $(BUILD_DIR)/initsnes.o $(BUILD_DIR)/putchar_stub.o $(BUILD_DIR)/snes_header.o $(BUILD_DIR)/runtime_stubs.o
	vpath %.c ../shared/src ../shared/port/cc65
	vpath %.asm ../shared/port/cc65
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),jcc816)
	C_SOURCES = mainBankZero.c ../shared/src/initsnes.c
	ASM_SOURCES = 
	OBJECTS = 
	vpath %.c ../shared/src
	vpath %.asm 
endif

ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),tcc816)
	C_SOURCES = mainBankZero.c
	ASM_SOURCES = 
	OBJECTS = $(BUILD_DIR)/mainBankZero.o
	vpath %.c .
	vpath %.asm 
endif

# Default target
all: $(BUILD_DIR)
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),wdc816cc)
	@$(MAKE) $(OBJECTS)
	$(LD) $(LDFLAGS)
	$(POST_LINK)
else
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),calypsi)
	@$(MAKE) $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) ../shared/port/calypsi/linker-large-large.scm clib-lc-ld.a --list-file=$(BUILD_DIR)/calypsi.lst --cross-reference --output-format=intel-hex -o $(BUILD_DIR)/calypsi.hex
	$(POST_LINK)
else
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),llvm-mos)
	..\shared\port\llvm-mos\compile.bat $(CC) $(CCFLAGS) $(LDFLAGS) -o $(BUILD_DIR)\mainBankZero_llvm-mos$(OUTPUT_EXT) $(C_SOURCES)
	$(POST_LINK)
else
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),cc65)
	@$(MAKE) $(OBJECTS)
	$(LD) -C ../shared/port/cc65/snes.cfg -o $(BUILD_DIR)/mainBankZero_cc65$(OUTPUT_EXT) -m $(BUILD_DIR)/mainBankZero_cc65.map $(OBJECTS) C:\cc65-2.19\lib\none.lib
	$(POST_LINK)
else
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),jcc816)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(C_SOURCES)
	$(POST_LINK)
else
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),tcc816)
	$(CC) $(CCFLAGS) $(INCLUDES) mainBankZero.c
	$(POST_LINK)
else
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),vbcc65816)
	@echo "Compiling with VBCC65816..."
	@echo "\"C:\vbcc65816\vbcc65816\vbcc65816_win\vbcc\bin\vc\" $(CCFLAGS) mainBankZero.c ../shared/src/initsnes.c -o $(BUILD_DIR)/mainBankZero_vbcc65816$(OUTPUT_EXT)"
	@../shared/port/vbcc816/vc.bat $(CCFLAGS) mainBankZero.c ../shared/src/initsnes.c -o $(BUILD_DIR)/mainBankZero_vbcc65816$(OUTPUT_EXT)
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
	@mkdir $(BUILD_DIR) 2>nul || echo Build directory created

# WDC816CC specific rules
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),wdc816cc)
# Compile C sources (pattern rule)
$(BUILD_DIR)/%.obj: %.c
	@mkdir $(BUILD_DIR) 2>nul || echo Build directory exists
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ $<

# Assemble ASM sources (pattern rule)
$(BUILD_DIR)/%.obj: %.asm
	@mkdir $(BUILD_DIR) 2>nul || echo Build directory exists
	$(AS) $(ASFLAGS) -o $@ $<
endif

# Calypsi specific rules
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),calypsi)
# Compile C sources (pattern rule)
$(BUILD_DIR)/%.o: %.c
	@mkdir $(BUILD_DIR) 2>nul || echo Build directory exists
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ $<
endif

# TCC816 specific rules
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),tcc816)
# Compile C sources (pattern rule) - don't pass -o for ROM creation
$(BUILD_DIR)/%.o: %.c
	@mkdir $(BUILD_DIR) 2>nul || echo Build directory exists
	$(CC) $(CCFLAGS) $(INCLUDES) $<
endif

# CC65 specific rules
ifeq ($(shell echo $(COMPILER) | tr A-Z a-z),cc65)
# Compile C sources (pattern rule)
$(BUILD_DIR)/%.o: %.c
	@mkdir $(BUILD_DIR) 2>nul || echo Build directory exists
	$(CC) $(CCFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$*.s $<
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/$*.s

# Assemble ASM sources (pattern rule)
$(BUILD_DIR)/%.o: %.s
	@mkdir $(BUILD_DIR) 2>nul || echo Build directory exists
	$(AS) $(ASFLAGS) -o $@ $<

# Specific rule for snes_header.s
$(BUILD_DIR)/snes_header.o: ../shared/port/cc65/snes_header.s
	@mkdir $(BUILD_DIR) 2>nul || echo Build directory exists
	$(AS) $(ASFLAGS) -o $@ $<

# Specific rule for runtime_stubs.s
$(BUILD_DIR)/runtime_stubs.o: ../shared/port/cc65/runtime_stubs.s
	@mkdir $(BUILD_DIR) 2>nul || echo Build directory exists
	$(AS) $(ASFLAGS) -o $@ $<
endif

# Clean target
clean:
	@powershell -Command "if (Test-Path '$(BUILD_DIR)') { Remove-Item -Recurse -Force '$(BUILD_DIR)' }"
	@powershell -Command "if (Test-Path '*.obj') { Remove-Item '*.obj' }"
	@powershell -Command "if (Test-Path '*.bin') { Remove-Item '*.bin' }"
	@powershell -Command "if (Test-Path '*.bnk') { Remove-Item '*.bnk' }"
	@powershell -Command "if (Test-Path '*.map') { Remove-Item '*.map' }"
	@powershell -Command "if (Test-Path '*.smc') { Remove-Item '*.smc' }"
	@powershell -Command "if (Test-Path 'PROG.LINK') { Remove-Item 'PROG.LINK' }"
	@echo Clean complete!

# Convenience targets
wdc: clean
	@$(MAKE) COMPILER=wdc816cc

vbcc65816: clean
	@$(MAKE) COMPILER=vbcc65816

calypsi: clean
	@$(MAKE) COMPILER=calypsi

llvm-mos: clean
	@$(MAKE) COMPILER=llvm-mos

cc65: clean
	@$(MAKE) COMPILER=cc65

jcc816: clean
	@$(MAKE) COMPILER=jcc816

tcc816: clean
	@$(MAKE) COMPILER=tcc816

# Show current compiler
info:
	@echo Current compiler: $(COMPILER)
	@echo C sources: $(C_SOURCES)
	@echo Objects: $(OBJECTS)

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
	@echo "  make wdc                - Build with WDC816CC (convenience)"
	@echo "  make vbcc65816          - Build with VBCC65816 (convenience)"
	@echo "  make calypsi            - Build with Calypsi (convenience)"
	@echo "  make llvm-mos           - Build with LLVM-Mos (convenience)"
	@echo "  make cc65               - Build with CC65 (convenience)"
	@echo "  make jcc816             - Build with JCC816 (convenience)"
	@echo "  make tcc816             - Build with TCC816 (convenience)"
	@echo "  make clean              - Clean build artifacts"
	@echo "  make info               - Show current compiler info"
	@echo "  make help               - Show this help message"

# Phony targets
.PHONY: all clean wdc vbcc65816 calypsi llvm-mos cc65 jcc816 info help
