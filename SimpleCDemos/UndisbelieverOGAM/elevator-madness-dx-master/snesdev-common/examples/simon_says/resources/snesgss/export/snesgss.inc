.global SnesGss__Module
.global SnesGss__ModulePart2
.global SnesGss__MusicTable
.exportzp SnesGss__MusicTable_Count = 0

.proc SnesGssData

.segment SNESGSS_BANK1

LABEL SnesGss__Module

	.incbin "spc700.bin"
LABEL SnesGss__ModulePart2


LABEL SnesGss__MusicTable


.endproc

