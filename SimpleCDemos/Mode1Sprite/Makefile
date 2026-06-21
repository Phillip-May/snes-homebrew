# Mode1Sprite
ASSETS = build/assets/.stamp
include ../shared/build/shared-config.mk

build/assets/.stamp: convert.py school.png biker.png
	@mkdir -p build/assets
	$(PYTHON) convert.py
	@touch $@
