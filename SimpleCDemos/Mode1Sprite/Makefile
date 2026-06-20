# Mode1Sprite
ASSETS = build/assets/school.inc build/assets/biker.inc
include ../shared/build/shared-config.mk

build/assets/school.inc: school.png
	@mkdir -p build/assets
	$(SNESCONV) bg $< -o $@ --symbol school

build/assets/biker.inc: biker.png
	@mkdir -p build/assets
	$(SNESCONV) sprite $< -o $@ --symbol biker
