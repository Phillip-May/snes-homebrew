# BenchmarkWhetstone
USE_FLOATING_POINT = 1
ASSETS = build/assets/.stamp
include ../shared/build/shared-config.mk

build/assets/.stamp: convert.py ../shared/assets/snesfont.png
	@mkdir -p build/assets
	$(PYTHON) convert.py
	@touch $@
