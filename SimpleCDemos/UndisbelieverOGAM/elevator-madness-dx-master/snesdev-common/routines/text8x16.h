;;
;; 8x16 tiled Text
;; ==============
;;
;;

.ifndef ::_TEXT8X16_H_
::_TEXT8X8_16_ = 1

.include "includes/import_export.inc"
.include "routines/text.h"


IMPORT_MODULE Text8x16
	STRUCT	Interface, TextInterface

	ROUTINE PrintChar
ENDMODULE

.endif ; ::_TEXT_H_

; vim: ft=asm:

