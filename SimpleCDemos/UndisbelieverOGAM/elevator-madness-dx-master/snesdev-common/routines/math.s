;; Math Routines

.include "routines/math.h"
.include "includes/import_export.inc"
.include "includes/structure.inc"
.include "includes/synthetic.inc"
.include "includes/registers.inc"

.setcpu "65816"

MODULE Math

.segment "SHADOW"
	BYTE	mathTmp1
	BYTE	mathTmp2
	BYTE	mathTmp3
	BYTE	mathTmp4

	DWORD	factor32
	DWORD	product32
	SAME_VARIABLE product16, product32

	DWORD	dividend32
	DWORD	divisor32
	DWORD	remainder32
	; major optimisation in Division routines
	; Allows me to ASL both dividend and result together
	SAME_VARIABLE result32, dividend32

.code

.include "routines/math/multiplication.asm"
.include "routines/math/division.asm"
.include "routines/math/db_unknown.asm"

ENDMODULE

