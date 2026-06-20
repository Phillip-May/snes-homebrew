# Mode1HDMA
ASSETS = build/assets/bg.inc
include ../shared/build/shared-config.mk

build/assets/bg.inc: bg_001.png
	@mkdir -p build/assets
	$(SNESCONV) bg $< -o $@ --symbol bg --scanline-palettes
