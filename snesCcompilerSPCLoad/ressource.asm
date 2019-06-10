SnesGraphics1 SECTION

	XDEF ~~TestInsert
~~TestInsert:
	INSERT ressource/Insert.bin
	
	XDEF  ~~SNESFONT_bin
~~SNESFONT_bin:
	INSERT ressource/SNESFONT_bin.bin
	XDEF  ~~END_SNESFONT_bin
~~END_SNESFONT_bin:

SPCROM SECTION
	XDEF  ~~SPC700Code
~~SPC700Code:
	INSERT ressource/ItaloTest.spc
	XDEF  ~~END_SPC700Code
~~END_SPC700Code:	