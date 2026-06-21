# BouncingSprites
# cc65 is excluded: it produces a ROM but nothing initializes at runtime
# (graphics never DMA in; VRAM/CGRAM/OAM stay at power-on values). This is a
# cc65 issue, not a bank overflow. Build it with ALLOW_UNSUPPORTED=1 to dig in.
SUPPORTED_COMPILERS = calypsi llvm-mos vbcc65816 tcc816
ASSETS = build/assets/.stamp
include ../shared/build/shared-config.mk

build/assets/.stamp: convert.py sprites.png
	@mkdir -p build/assets
	$(PYTHON) convert.py
	@touch $@
