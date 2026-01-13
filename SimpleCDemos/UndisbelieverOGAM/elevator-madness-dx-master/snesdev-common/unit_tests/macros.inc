
.macro Pass
	PHA
	PHY
	PHX

	Text_SetColor	2
	Text_PrintStringLn "Pass"

	PLX
	PLY
	PLA
.endmacro

.macro Fail
	PHA
	PHY
	PHX

	Text_SetColor	1
	Text_PrintStringLn "Fail"

	STZ	noErrors

	PLX
	PLY
	PLA
.endmacro

; Sets color red if Y is wrong, otherwise green.
.macro Check_16Y expected
	PHA
	PHY
	PHX
		CPY	#.loword(expected)
		IF_NE
			Text_SetColor	1
			STZ	noErrors
		ELSE
			Text_SetColor	2
		ENDIF
	PLX
	PLY
	PLA
.endmacro

; Sets color red if X is wrong, otherwise green.
.macro Check_16X expected
	PHA
	PHY
	PHX
		CPX	#.loword(expected)
		IF_NE
			Text_SetColor	1
			STZ	noErrors
		ELSE
			Text_SetColor	2
		ENDIF
	PLX
	PLY
	PLA
.endmacro

; Sets color red if XY is wrong, otherwise green.
.macro Check_32XY expected
	PHA
	PHX
	PHY
		CPX	#.hiword(expected)
		IF_NE
			Text_SetColor	1
			STZ	noErrors
		ELSE
			CPY	#.loword(expected)
			IF_NE
				Text_SetColor	1
				STZ	noErrors
			ELSE
				Text_SetColor	2
			ENDIF
		ENDIF
	PLY
	PLX
	PLA
.endmacro

; Sets color red if A is wrong, otherwise green.
.macro Check_U8A expectedA
	PHA
	PHX
	PHY
		CMP	#expectedA
		IF_NE
			Text_SetColor	1
			STZ	noErrors
		ELSE
			Text_SetColor	2
		ENDIF
	PLY
	PLX
	PLA
.endmacro


; Use to build the page tables.
; Cannot uses `.condes` because it causes addressing size errors out in 65816 mode.

::N_PAGES .set 0

.macro PAGE_ROUTINE name
.code
	.ifnblank name
		ROUTINE name
	.endif

.ident(.sprintf("__Test_Page_%d", N_PAGES)):

	::N_PAGES .set ::N_PAGES + 1
.endmacro


.macro BuildPageTable
	.rodata

PageTable:
	.repeat N_PAGES, i
		.addr .ident(.sprintf("__Test_Page_%d", i))
	.endrepeat
PageTableEnd:

	.code
.endmacro

; vim: ft=asm:

