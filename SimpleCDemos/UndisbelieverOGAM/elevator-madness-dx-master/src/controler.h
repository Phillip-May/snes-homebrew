.ifndef ::_CONTROLER_H_
::_CONTROLER_H_ = 1

.setcpu "65816"

; Common includes
.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"


BUTTON_JUMP		= JOY_B
BUTTON_ELEVATOR_DOOR	= JOY_Y
BUTTON_ELEVATOR_UP	= JOY_UP | JOY_X
BUTTON_ELEVATOR_DOWN	= JOY_DOWN | JOY_A



IMPORT_MODULE Controler
	;; New buttons pressed on current frame.
	WORD	pressed

	;; The state of the current frame
	WORD	current

	;; Updates the control variables
	;; REQUIRE: 8 bit A, 16 bit Index, AUTOJOY enabled
	ROUTINE Update

ENDMODULE

.endif ; ::_CONTROLS_H_

; vim: set ft=asm:

