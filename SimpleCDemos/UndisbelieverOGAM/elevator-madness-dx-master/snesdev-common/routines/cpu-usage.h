
;; CPU Usage calculator.
;;
;; This module allows for an estimation of the CPU usage.
;;
;; It uses a bogo counter that is incremented on idle (waiting
;; for next frame) and comparing it to a reference we can
;; calculate the CPU usage.
;;
;; The formula to calculate the CPU usage is:
;;
;;       1 - (CurrentBogo) / (ReferenceBogo * (MissedFrames + 1))
;;
;; It also keeps track of missed frames, see the `missedFrames` counter.
;;
;; In order to use this module, the following must occur.
;;
;;  * `CPU_Usage__NMI` must active on every VBlank Interrupt.
;;
;;  * `CPU_Usage__Calc_Idle` must be called once.
;;
;;  * `CPU_Usage__Wait_Frame` must be called instead of `WAI`
;;

.ifndef ::_CPU_USAGE_H_
::__CPU_USAGE_H_ = 1

.setcpu "65816"
.include "includes/import_export.inc"

IMPORT_MODULE CpuUsage

	;; Number of frames that were missed in-between `Wait_Frame`s
	UINT8 missedFrames

	;; Number of VBlanks passed since last `WaitFrame` or `WaitLimited`
	;; (byte)
	UINT8 vBlankCounter

	;; Reference Bogo taken from an empty frame, used
	;; for calculations
	UINT16 referenceBogo

	;; Current Bogo for previous frame
	UINT16 currentBogo


	;; Calculate `ReferenceBogo`
	;;
	;; REQUIRES: 8 bit A, 16 bit Index, DB Access Shadow RAM
	;;
	;; This routine will disable Interrupts and must be called:
	;;   * straight after Initialisation.
	;;   * After FASTROM is set (if using FASTROM)
	;;   * When VBlank is not doing anything
	ROUTINE CalcReference


	;; Wait until the next frame, calculating the number of bogos to the
	;; start of the next frame.
	;;
	;; REQUIRES: DB Access Shadow RAM
	;;
	;; This routine calculates:
	;;    * `FramesMised`
	;;    * `CurrentBogo`
	;;
	;; This routine saves the CPU state.
	ROUTINE WaitFrame


	;; Wait until a given number of frames has passed since the last
	;; `CPU_Usage__Wait_Frame` or `CPU_Usage__Wait_Limited` call.
	;;
	;; REQUIRES: 8 bit A, DB Access Shadow RAM
	;;
	;; INPUT: A = number of frames to wait
	;;
	;; MODIFIES: Decrements A by one
	;;
	;; This routine calculates:
	;;    * `FramesMised`
	;;    * `CurrentBogo`
	;;
	;; This routine saves the CPU state.
	ROUTINE WaitLimited


	;; Sets Frame Increment Counter
	;;
	;; REQUIRE 8 bit A
	.macro CpuUsage_NMI
		INC ::CpuUsage__vBlankCounter
	.endmacro

ENDMODULE

.endif ; __CPU_USAGE_H_

; vim: ft=asm:

