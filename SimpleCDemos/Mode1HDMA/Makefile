# Mode1HDMA
ASSETS = build/assets/.stamp
include ../shared/build/shared-config.mk

build/assets/.stamp: convert.py bg_001.png
	@mkdir -p build/assets
	$(PYTHON) convert.py
	@touch $@
