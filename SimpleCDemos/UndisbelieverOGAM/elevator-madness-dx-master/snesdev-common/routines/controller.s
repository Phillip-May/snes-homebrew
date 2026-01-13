
.include "controller.h"
.include "includes/import_export.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"
.include "includes/structure.inc"

;; Ensure Controller_VBlank is used
.forceimport _Controller_VBlank__Called:zp

MODULE Controller

.segment "SHADOW"
	WORD	pressed
	WORD	current

	;; The inversion of the controller status of of the previous frame
	WORD	invertedPrevious

	;; Frame counter used for UpdateRepeatingDPad routine
	BYTE	repeatCounter
.code

.A8
.I16
ROUTINE UpdateRepeatingDPad
	.assert CONTROLLER_REPEAT_DPAD_DELAY <= 255, error, "Bad Config"
	.assert CONTROLLER_REPEAT_DPAD_DELAY > CONTROLLER_REPEAT_DPAD_RATE, error, "Bad Config"

	LDA	current + 1
	IF_BIT	#JOYH_DPAD
		LDA	repeatCounter
		CMP	#CONTROLLER_REPEAT_DPAD_DELAY - CONTROLLER_REPEAT_DPAD_RATE - 1
		IF_GE
			CMP	#CONTROLLER_REPEAT_DPAD_DELAY
			IF_GE
				LDA	#JOYH_DPAD
				TSB	invertedPrevious + 1

				LDA	#CONTROLLER_REPEAT_DPAD_DELAY - CONTROLLER_REPEAT_DPAD_RATE
			ENDIF
		ENDIF

		INC
		STA	repeatCounter
	ELSE
		STZ	repeatCounter
	ENDIF

	RTS


ENDMODULE

