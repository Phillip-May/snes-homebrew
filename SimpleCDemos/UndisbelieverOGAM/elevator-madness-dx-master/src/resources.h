.ifndef ::_RESOURCES_H_
::_RESOURCES_H_ = 1

.include "includes/import_export.inc"

.include "resources/metasprites/elevator.metasprite.h"

;; Resources
IMPORT_BINARY backgroundMap
IMPORT_BINARY backgroundTiles
IMPORT_BINARY backgroundPalette

IMPORT_BINARY splashMap
IMPORT_BINARY splashTiles
IMPORT_BINARY splashPalette

IMPORT_BINARY controlsMap
IMPORT_BINARY controlsTiles
IMPORT_BINARY controlsPalette

IMPORT_BINARY interactiveTiles
IMPORT_BINARY interactivePalette

IMPORT_BINARY strikeAnimationsTiles
IMPORT_BINARY strikeAnimationsPalette

IMPORT_BINARY playerTiles
IMPORT_BINARY playerPalette

IMPORT_BINARY businessManTiles
IMPORT_BINARY businessManPalette


.endif ; ::_RESOURCES_H_

; vim: set ft=asm:

