# BenchmarkDhrystone
USE_FLOATING_POINT = 1
ASSETS = build/assets/imagedata.inc
include ../shared/build/shared-config.mk

build/assets/imagedata.inc: ../shared/assets/snesfont.png
	@mkdir -p build/assets
	$(SNESCONV) font $< -o $@ --symbol SNESFONT_bin
