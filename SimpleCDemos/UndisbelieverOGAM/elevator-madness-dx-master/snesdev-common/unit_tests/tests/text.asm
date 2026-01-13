; Test numbers are printed correctly.


PAGE_ROUTINE Text_1
	Text_SetColor	4
	Text_PrintStringLn "Signed Printing Page"

	Text_SetColor	0
	Text_NewLine

	Text_PrintString " S8A   Minus 33     = "
	LDA	#.lobyte(-33)
	JSR	Text__PrintDecimal_S8A

	Text_NewLine

	Text_PrintString " S16Y  Minus 1      = "
	LDY	#.loword(-1)
	JSR	Text__PrintDecimal_S16Y

	Text_NewLine

	Text_PrintString " S32XY Minus 123456 = "
	LDXY	#-123456
	JSR	Text__PrintDecimal_S32XY

	Text_NewLine
	Text_NewLine

	Text_PrintString " S8A   Plus 33      = "
	LDA	#.lobyte(33)
	JSR	Text__PrintDecimal_S8A

	Text_NewLine

	Text_PrintString " S16Y  Plus 1       = "
	LDA	#6
	LDY	#.loword(1)
	JSR	Text__PrintDecimalPadded_S16Y

	Text_NewLine

	Text_PrintString " S32XY Plus 123456  = "
	LDA	#8
	LDXY	#123456
	JSR	Text__PrintDecimalPadded_S32XY

	Text_NewLine

	Text_PrintString " S16Y  Minus 1      = "
	LDA	#6
	LDY	#.loword(-1)
	JSR	Text__PrintDecimalPadded_S16Y

	Text_NewLine

	Text_PrintString " S32XY Minus 123456 = "
	LDA	#8
	LDXY	#-123456
	JSR	Text__PrintDecimalPadded_S32XY

	RTS



