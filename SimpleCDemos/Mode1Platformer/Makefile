# Mode1Platformer
# SUPPORTED_COMPILERS is set to the toolchains whose ROMs actually render this
# demo (a linked ROM that shows nothing is a fail). See README.md.
SUPPORTED_COMPILERS = calypsi vbcc65816
ASSETS = build/assets/.stamp
include ../shared/build/shared-config.mk

build/assets/.stamp: convert.py bg.png bg_col.png player.png walker.png
	@mkdir -p build/assets
	$(PYTHON) convert.py
	@touch $@
