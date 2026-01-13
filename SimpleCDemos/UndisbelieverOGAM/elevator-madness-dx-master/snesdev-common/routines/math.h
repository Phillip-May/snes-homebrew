
.ifndef ::_MATH_H_
::_MATH_H_ = 1

.setcpu "65816"
.include "includes/import_export.inc"

IMPORT_MODULE Math

	DWORD	factor32
	DWORD	product32
	WORD	product16	; uses the same memory address as product32

	DWORD	dividend32
	DWORD	divisor32
	DWORD	remainder32
	SAME_VARIABLE result32, dividend32

;; Multiplication
;; ==============
	;; Multiply an 8 bit unsigned integer by an 8 bit unsigned integer
	;;
	;; REQUIRE: 8 bit A, DB Shadow
	;; MODIFIES: Y
	;;
	;; INPUT:
	;;	Y: unsigned integer (only low 8 bits are used)
	;;	X: unsigned integer (only low 8 bit are used)
	;;
	;; OUTPUT:
	;;	Y: result (8 or 16 bits depending on Index size)
	ROUTINE Multiply_U8Y_U8X_UY
	ROUTINE Multiply_U8Y_U8X_UY_DB

	;; Multiply a 16 bit integer by an 8 bit unsigned integer
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, DB Shadow
	;; MODIFIES: A, Y, X
	;;
	;; INPUT:
	;;	Y: 16 bit integer
	;;	A: 8 bit unsigned integer
	;;
	;; OUTPUT:
	;;	Y: 16 bit product
	;;	product16: 16 bit product
	;;	product32: 16 bit product
	;;	product32: 32 bit unsigned product (if inputs are unsigned)
	ROUTINE Multiply_S16Y_U8A_S16Y
	ROUTINE Multiply_U16Y_U8A_U16Y
	ROUTINE Multiply_U16Y_U8A_U32
	ROUTINE Multiply_S16Y_U8A_S16Y_DB
	ROUTINE Multiply_U16Y_U8A_U16Y_DB
	ROUTINE Multiply_U16Y_U8A_U32_DB

	;; Multiply two 16 bit integers.
	;;
	;; The signs and lengths of the inputs and outputs are in the parameters.
	;;
	;; REQUIRE: 16 bit Index, DB Shadow
	;; MODIFIES: A, X, Y
	;;
	;; INPUT:
	;;	Y: 16 bit factor
	;;	X: 16 bit factor
	;;
	;; OUTPUT:
	;;	Y: 16 bit product
	;;	product16: 16 bit product
	ROUTINE Multiply_U16Y_U16X_U16Y
	ROUTINE Multiply_U16Y_S16X_16Y
	ROUTINE Multiply_S16Y_U16X_16Y
	ROUTINE Multiply_S16Y_S16X_S16Y
	ROUTINE Multiply_U16Y_U16X_U16Y_DB
	ROUTINE Multiply_U16Y_S16X_16Y_DB
	ROUTINE Multiply_S16Y_U16X_16Y_DB
	ROUTINE Multiply_S16Y_S16X_S16Y_DB

	;; Multiply two 16 bit integers resulting in a 32 integer.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, DB Shadow
	;; MODIFIES: A, X, Y
	;;
	;; INPUT:
	;;	Y: signed 16 bit factor
	;;	X: signed 16 bit factor
	;;
	;; OUTPUT:
	;;	XY: 32 bit signed product
	;;	product32: 32 bit signed product
	ROUTINE Multiply_U16Y_U16X_U32XY
	ROUTINE Multiply_U16Y_S16X_32XY
	ROUTINE Multiply_S16Y_U16X_32XY
	ROUTINE Multiply_U16Y_U16X_U32XY_DB
	ROUTINE Multiply_U16Y_S16X_32XY_DB
	ROUTINE Multiply_S16Y_U16X_32XY_DB

	;; Multiply two signed 16 bit integers resulting in a 32 signed integer.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, DB Shadow
	;; MODIFIES: A, X, Y
	;;
	;; INPUT:
	;;	Y: signed 16 bit factor
	;;	X: signed 16 bit factor
	;;
	;; OUTPUT:
	;;	XY: 32 bit signed product
	;;	product32: 32 bit signed product
	ROUTINE Multiply_S16Y_S16X_S32XY
	ROUTINE Multiply_S16Y_S16X_S32XY_DB

	;; Multiply a 32 bit integer by an 8 bit unsigned integer
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, DB Shadow
	;; MODIFIES: A, X, Y
	;;
	;; INPUT:
	;;	XY: 32 bit factor (Y loword)
	;;	A: 8 bit unsigned factor
	;;
	;; OUTPUT:
	;;	XY: 32 bit product
	;;	product32: 32 bit product
	;;	c: 33rd bit of result
	ROUTINE Multiply_U32XY_U8A_U32XY
	ROUTINE Multiply_S32XY_U8A_S32XY
	ROUTINE Multiply_U32XY_U8A_U32XY_DB
	ROUTINE Multiply_S32XY_U8A_S32XY_DB

	;; Multiply a 32 bit integer by a 16 bit unsigned integer.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, DB Shadow
	;;
	;; INPUT:
	;;	factor32: 32 bit factor
	;;	Y: 16 bit unsigned factor
	;;
	;; OUTPUT:
	;;	XY: 32 bit product
	;;	product32: 32 bit product
	ROUTINE Multiply_U32_U16Y_U32XY
	ROUTINE Multiply_S32_U16Y_S32XY
	ROUTINE Multiply_U32_U16Y_U32XY_DB
	ROUTINE Multiply_S32_U16Y_S32XY_DB

	;; Multiply a 32 bit integer by a 16 bit signed integer.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, DB Shadow
	;;
	;; INPUT:
	;;	factor32: 32 bit factor
	;;	Y: 16 bit signed factor
	;;
	;; OUTPUT:
	;;	XY: 32 bit product
	;;	product32: 32 bit product
	ROUTINE Multiply_U32_S16Y_32XY
	ROUTINE Multiply_S32_S16Y_S32XY
	ROUTINE Multiply_U32_S16Y_32XY_DB
	ROUTINE Multiply_S32_S16Y_S32XY_DB

	;; Multiply a 32 bit integer by another 32 bit integer.
	;;
	;; REQUIRE: 8 bit A, 16 bit Index, DB Shadow
	;;
	;; INPUT:
	;;	factor32: 32 bit factor
	;;	XY: 32 bit factor (Y = loword)
	;;
	;; OUTPUT:
	;;	XY: 32 bit product
	;;	product32: 32 bit product
	ROUTINE Multiply_U32_U32XY_U32XY
	ROUTINE Multiply_U32_S32XY_32XY
	ROUTINE Multiply_S32_U32XY_32XY
	ROUTINE Multiply_S32_S32XY_S32XY
	ROUTINE Multiply_U32_U32XY_U32XY_DB
	ROUTINE Multiply_U32_S32XY_32XY_DB
	ROUTINE Multiply_S32_U32XY_32XY_DB
	ROUTINE Multiply_S32_S32XY_S32XY_DB


;; Division
;; ========

	;; Optimised Unsigned 16 / 16 bit Integer Division.
	;;
	;; Inspiration: <http://codebase64.org/doku.php?id=base:16bit_division_16-bit_result>
	;;
	;; REQUIRES: 16 bit Index
	;;
	;; INPUT:
	;;	Y: 16 bit unsigned Dividend
	;;	X: 16 bit unsigned Divisor
	;;
	;; OUTPUT:
	;;	Y: 16 bit unsigned Result
	;;	X: 16 bit unsigned Remainder
	;;
	;;
	;; Uses SNES Registers if Y < 256.
	;;
	;;
	;; Uses 4 bytes of temporary variables.
	;;   * uint16 divisor
	;;   * uint16 counter
	ROUTINE Divide_U16Y_U16X
	ROUTINE Divide_U16Y_U16X_DB

	;; Signed 16 bit / unsigned 16 bit Integer Division.
	;;
	;; Inspiration: <http://codebase64.org/doku.php?id=base:16bit_division_16-bit_result>
	;;
	;; REQUIRES: 16 bit Index
	;;
	;; INPUT:
	;;	Y: 16 bit signed Dividend
	;;	X: 16 bit unsigned Divisor
	;;
	;; OUTPUT:
	;;	Y: 16 bit signed Result
	;;	X: 16 bit unsigned Remainder (Always positive, Euclidean division)
	;;
	;;
	;; Uses SNES Registers if Y < 256 and Y >= 0.
	;;
	;;
	;; Uses 4 bytes of temporary variables.
	;;   * uint16 divisor
	;;   * uint16 counter
	ROUTINE Divide_S16Y_U16X
	ROUTINE Divide_S16Y_U16X_DB

	;; Unsigned 16 bit by signed 16 bit Integer Division.
	;;
	;; Inspiration: <http://codebase64.org/doku.php?id=base:16bit_division_16-bit_result>
	;;
	;; REQUIRES: 16 bit Index
	;;
	;; INPUT:
	;;	Y: 16 bit unsigned Dividend
	;;	X: 16 bit signed Divisor
	;;
	;; OUTPUT:
	;;	Y: 16 bit signed Result
	;;	X: 16 bit unsigned Remainder (Always positive, Euclidean division)
	;;
	;;
	;; Uses SNES Registers if Y < 256 and Y >= 0.
	;;
	;;
	;; Uses 4 bytes of temporary variables.
	;;   * uint16 divisor
	;;   * uint16 counter
	ROUTINE Divide_U16Y_S16X
	ROUTINE Divide_U16Y_S16X_DB

	;; Signed 16 / 16 bit Integer Division.
	;;
	;; Inspiration: <http://codebase64.org/doku.php?id=base:16bit_division_16-bit_result>
	;;
	;; REQUIRES: 16 bit Index
	;;
	;; INPUT:
	;;	Y: 16 bit signed Dividend
	;;	X: 16 bit signed Divisor
	;;
	;; OUTPUT:
	;;	Y: 16 bit signed Result
	;;	X: 16 bit unsigned Remainder (Always positive, Euclidean division)
	;;
	;;
	;; Uses SNES Registers if Y < 256 and Y >= 0.
	;;
	;;
	;; Uses 4 bytes of temporary variables.
	;;   * uint16 divisor
	;;   * uint16 counter
	ROUTINE Divide_S16Y_S16X
	ROUTINE Divide_S16Y_S16X_DB

	;; Unsigned 16 / 8 bit Integer Division
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT:
	;;	Y: 16 bit unsigned Dividend
	;;	A: 8 bit unsigned Divisor
	;;
	;; OUTPUT:
	;;	Y: 16 bit unsigned Result
	;;	X: 16 bit unsigned Remainder
	ROUTINE Divide_U16Y_U8A
	ROUTINE Divide_U16Y_U8A_DB

	;; Unsigned 32 / 32 bit Integer Division
	;;
	;; Inspiration: <http://codebase64.org/doku.php?id=base:16bit_division_16-bit_result>
	;;
	;; INPUT:
	;;	dividend32: uint32
	;;	divisor32: uint32
	;;
	;; OUTPUT:
	;;	result32: uint32
	;;      remainder32: uint32
	;;
	;; NOTES:
	;;	`result32` and `dividend32` share the same memory location
	ROUTINE Divide_U32_U32
	ROUTINE Divide_U32_U32_DB

	;; Signed 32 / 32 bit Integer Division
	;;
	;; Inspiration: <http://codebase64.org/doku.php?id=base:16bit_division_16-bit_result>
	;;
	;; INPUT:
	;;	dividend32: sint32
	;;	divisor32: sint32
	;;
	;; OUTPUT:
	;;	result32: sint32
	;;      remainder32: uint32 (Always positive, Euclidean division)
	;;
	;; NOTES:
	;;	`result32` and `dividend32` share the same memory location
	ROUTINE Divide_S32_S32
	ROUTINE Divide_S32_S32_DB

	;; Unsigned 32 / 8 bit Integer Division
	;;
	;; INPUT:
	;;	dividend32: uint32 dividend
	;;	A : 8 bit divisor
	;;
	;; OUTPUT:
	;;	result32: uint32 result
	;;      A: uint8 remainder
	;;
	;; NOTES:
	;;	`result32` and `dividend32` share the same memory location
	ROUTINE Divide_U32_U8A
	ROUTINE Divide_U32_U8A_DB
ENDMODULE

.endif ; ::_MATH_H_

; vim: ft=asm:

