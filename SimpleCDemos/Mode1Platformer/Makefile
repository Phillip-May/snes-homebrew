# Mode1Platformer
# SUPPORTED_COMPILERS is set to the toolchains whose ROMs actually render this
# demo (a linked ROM that shows nothing is a fail). See README.md. tcc816 now
# fits the streamed ~40 KB tilemap via its auto-banking of .rodata.
SUPPORTED_COMPILERS = calypsi vbcc65816 tcc816
ASSETS = build/assets/.stamp
include ../shared/build/shared-config.mk

# The level (layout/collision/blockcells/spawns) is generated from the vendored
# level.tmx + the committed Kenney tile sheet by convert.py (via genart.py); the
# player/walker/block sprite strips are committed PNGs.
build/assets/.stamp: convert.py genart.py level.tmx kenney_tilemap_packed.png player.png walker.png blocks.png
	@mkdir -p build/assets
	$(PYTHON) convert.py
	@touch $@

# vbcc's -O4 is unusable for mapengine.c: it miscompiles the fixed-point math
# (the camera scroll computed to 0) AND its unused-object elimination strips the
# engine entry points from this separately-compiled object. -O3 with the same
# performance flags (-unroll-all, -inline-depth, -fp-associative) is correct and
# essentially -O4-speed, so build just this file that way and link its object;
# the rest of the demo keeps the shared -O4. Other toolchains compile it
# normally.
ifeq ($(COMPILER_LC),vbcc65816)
C_SOURCES = $(filter-out mapengine.c,$(PROJECT_C_FILES)) $(SHARED_SRC_DIR)/initsnes.c $(BUILD_DIR)/mapengine.o
all: $(BUILD_DIR)/mapengine.o
$(BUILD_DIR)/mapengine.o: mapengine.c mapengine.h
	@mkdir -p $(BUILD_DIR)
	$(CC) +snes-hi -lm -O3 -inline-depth=1000 -unroll-all -fp-associative -force-statics -I"$(SHARED_SRC_DIR)" -I"lib" -I"include" -D__VBCC__=1 -D__VBCC65816__ -c mapengine.c -o $@
endif
