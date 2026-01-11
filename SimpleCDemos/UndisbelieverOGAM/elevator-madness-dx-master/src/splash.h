.ifndef ::_SPLASH_H_
::_SPLASH_H_ = 1

.setcpu "65816"

; Common includes
.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"

.include "resources.h"

;; VRAM Map
;; WORD ADDRESSES
SPLASH_BG1_MAP		= $0000
SPLASH_BG1_TILES	= $5000

SPLASH_BG1_SIZE	= BGXSC_SIZE_32X32

SPLASH_TIMEOUT		=  5 * 60
CONTROLS_TIMEOUT	= 10 * 60


.global	interactiveBgBuffer
.global updateBgBufferOnZero

IMPORT_MODULE Splash

	;; Shows the Splash screen on the display.
	;;
	;; Also shows the controls.
	;;
	;; Mainly used to seed the random number generator.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	ROUTINE SplashScreen

ENDMODULE

.endif ; ::_SPLASH_H_

; vim: set ft=asm:

