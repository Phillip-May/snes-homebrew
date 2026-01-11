
.include "resources.h"
.setcpu "65816"

.segment "BANK1"
	INCLUDE_BINARY backgroundMap,		"resources/images4bpp/background.map"
	INCLUDE_BINARY backgroundTiles,		"resources/images4bpp/background.4bpp"

	INCLUDE_BINARY splashMap,		"resources/images4bpp/splash.map"
	INCLUDE_BINARY splashTiles,		"resources/images4bpp/splash.4bpp"

	INCLUDE_BINARY controlsMap,		"resources/images4bpp/controls.map"
	INCLUDE_BINARY controlsTiles,		"resources/images4bpp/controls.4bpp"

	INCLUDE_BINARY interactiveTiles,	"resources/tiles4bpp/interactive-tiles.4bpp"

	INCLUDE_BINARY playerTiles,		"resources/metasprites/player.4bpp"
	INCLUDE_BINARY businessManTiles,	"resources/metasprites/business-man.4bpp"
	INCLUDE_BINARY strikeAnimationsTiles,	"resources/metasprites/strike-animations.4bpp"


.segment "BANK2"
	INCLUDE_BINARY backgroundPalette,	"resources/images4bpp/background.clr"
	INCLUDE_BINARY splashPalette,		"resources/images4bpp/splash.clr"
	INCLUDE_BINARY controlsPalette,		"resources/images4bpp/controls.clr"
	INCLUDE_BINARY interactivePalette,	"resources/tiles4bpp/interactive-tiles.clr"

	INCLUDE_BINARY playerPalette,		"resources/metasprites/player.clr"
	INCLUDE_BINARY businessManPalette,	"resources/metasprites/business-man.clr"
	INCLUDE_BINARY strikeAnimationsPalette,	"resources/metasprites/strike-animations.clr"


.segment "BANK2"
	.export MetaSpriteLayoutBank = .bankbyte(*)

	.include "resources/metasprites/elevator.metasprite-table.inc"
	.include "resources/metasprites/elevator.metasprite-data.inc"

