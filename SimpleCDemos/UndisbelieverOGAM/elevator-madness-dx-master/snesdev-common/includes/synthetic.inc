;; Synthetic Instructions.
;;
;; Inspiration: <http:;forums.nesdev.com/viewtopic.php?p=87209#p87209>

.ifndef ::_SYNTHETIC_INC_
::_SYNTHETIC_INC_ = 1

;; Add to accumulator
;;
;; CLC : ADC
.macro ADD param, index
	.ifblank index
		CLC
		ADC	param
	.else
		CLC
		ADC	param, index
	.endif
.endmacro

;; Subtract from accumulator
;;
;; SEC : SBC
.macro SUB param, index
	.ifblank index
		SEC
		SBC	param
	.else
		SEC
		SBC	param, index
	.endif
.endmacro

;; Reverse Subtract
;;
;; REQUIRES: 8 bit A
.macro RSB8 param, index
	.assert .asize = 8, error, "Require 8 bit A"

	.ifblank index
		EOR	#$FF
		SEC
		ADC	param
	.else
		EOR	#$FF
		SEC
		ADC	param, index
	.endif
.endmacro

.macro RSB16 param, index
	.assert .asize = 16, error, "Require 16 bit A"

	.ifblank index
		EOR	#$FFFF
		SEC
		ADC	param
	.else
		EOR	#$FFFF
		SEC
		ADC	param, index
	.endif
.endmacro

;; Reverse Subtract with carry
;;
;; REQUIRE: 8 bit A
.macro RSC8 param, index
	.assert .asize = 8, error, "Require 8 bit A"

	.ifblank index
		EOR	#$FF
		SEC
		ADC	param
	.else
		EOR	#$FF
		SEC
		ADC	param, index
	.endif
.endmacro

.macro RSC16 param, index
	.assert .asize = 16, error, "Require 16 bit A"

	.ifblank index
		EOR	#$FFFF
		SEC
		ADC	param
	.else
		EOR	#$FFFF
		SEC
		ADC	param, index
	.endif
.endmacro

;; 8 bit Arithmetic Shift Right
;;
;; REQUIRE: 8 bit A
.macro ASR8
	.assert .asize = 8, error, "Require 8 bit A"

	CMP	#$80		; carry set to the sign bit
	ROR
.endmacro

;; 16 bit Arithmetic Shift Right
;;
;; REQUIRE: 16 bit A
.macro ASR16
	.assert .asize = 16, error, "Require 16 bit A"

	CMP	#$8000		; carry set to the sign bit
	ROR
.endmacro


;; 8/16 bit rotate right
.macro ROTATEL
	ASL A
	ADC	#0
.endmacro


;; 8/16 bit rotate right
.macro ROTATER
	PHA
	LSR A
	PLA
	ROR
.endmacro

;; Negate 8 bit A
;;
;; REQUIRE: 8 bit A
.macro NEG8
	.assert .asize = 8, error, "Require 8 bit A"

	EOR	#$FF
	INC
.endmacro

;; Negate 16 bit A
;;
;; REQUIRE: 16 bit A
.macro NEG16
	.assert .asize = 16, error, "Require 16 bit A"

    EOR	#$FFFF
	INC
.endmacro

;; Negates a 32 bit variable.
;;
;; var can be XY
;;
;; REQUIRE: 16 bit A
.macro NEG32 var, index
	.assert .asize = 16, error, "Require 16 bit A"

	.ifblank index
		.if .xmatch({var}, XY)
			TYA
			EOR	#$FFFF
			CLC
			ADC	#1
			TAY
			TXA
			EOR	#$FFFF
			ADD	#0
			TAX
		.else
			LDA	var
			EOR	#$FFFF
			CLC
			ADC	#1
			STA	var
			LDA	2 + (var)
			EOR	#$FFFF
			ADC	#0
			STA	2 + (var)
		.endif
	.else
		.if .xmatch({var}, XY)
			.error "invalid addressing mode"
		.else
			LDA	var, X
			EOR	#$FFFF
			CLC
			ADC	#1
			STA	var, X
			LDA	2 + (var), index
			EOR	#$FFFF
			ADC	#0
			STA	2 + (var), index
		.endif
	.endif
.endmacro


;; Load an absolute signed 8 bit variable into A.
;;
;; Parameter can be X, Y, blank (A), or a variable.
;;
;; Please note that doing a `IF_MINUS : NEG16 : ENDIF` would be faster
;; than `LDABS16 A` if you know the N flag matches the accumulator's
;; MSB.
;;
;; REQUIRES: 8 bit A
.macro LDABS8 var, index
	.assert .asize = 8, error, "Require 8 bit A"

	.local skip

	.ifblank index
		.if .xmatch({var}, X)
			TXA
		.elseif .xmatch({var}, Y)
			TYA
		.elseif .blank(var) .or .xmatch({var}, A)
			; Sets N flag depending on MSB of A
			CMP	#0
		.else
			LDA	var
		.endif
	.else
		LDA	var, index
	.endif

	BPL	skip
		NEG8
skip:
.endmacro


;; Load an absolute signed 16 bit variable into A.
;;
;; Parameter can be X, Y, blank (A), or a variable.
;;
;; Please note that doing a `IF_MINUS : NEG16 : ENDIF` would be faster
;; than `LDABS16 A` if you know the N flag matches the accumulator's
;; MSB.
;;
;; REQUIRES: 16 bit A
.macro LDABS16 var, index
	.assert .asize = 16, error, "Require 16 bit A"

	.local skip

	.ifblank index
		.if .xmatch({var}, X)
			TXA
		.elseif .xmatch({var}, Y)
			TYA
		.elseif .blank(var) .or .xmatch({var}, A)
			; Sets N flag depending on MSB of A
			CMP	#0
		.else
			LDA	var
		.endif
	.else
		LDA	var, index
	.endif

	BPL	skip
		NEG16
skip:
.endmacro

;; 16 bit increment on 8 bit Accumulator
;;
;; REQUIRES: 8 bit A
.macro INC16 param, index
	.assert .asize = 8, error, "Require 8 bit A"

	.local skip
	.ifblank index
		INC	param
		BNE	skip
		INC	param + 1
	.else
		INC	param, index
		BNE	skip
		INC	param + 1, index
	.endif
skip:
.endmacro

;; 32 bit increment
;;
;; param can be XY
;;
;; REQUIRES: 16 bit A
.macro INC32 param, index
	.assert .asize = 16, error, "Require 16 bit A"

	.local skip
	.ifblank index
		.if .xmatch({param}, XY)
			INY
			BNE	skip
			INX
		.else
			INC	param
			BNE	skip
			INC	param + 1
		skip:
		.endif
	.else
		.if .xmatch({param}, XY)
			.error "invalid addressing mode"
		.else
			INC	param, index
			BNE	skip
			INC	param + 1, index
		skip:
		.endif
	.endif
.endmacro


;; 32 bit decrement
;;
;; REQUIRES: 16 bit A
.macro DEC32 param, index
	.assert .asize = 16, error, "Require 16 bit A"

	.local skip
	.ifblank index
		.if .xmatch({param}, XY)
			CPY	#0
			BNE	skip
			DEX
		skip:
			DEY
		.else
			LDA	param
			BNE	skip
			DEC	param + 2
		skip:
			DEC	param
		.endif
	.else
		.if .xmatch({param}, XY)
			.error "invalid addressing mode"
		.else
			LDA	param
			BNE	skip
			DEC	param + 2, index
		skip:
			DEC	param, index
		.endif
	.endif
.endmacro

;; Loads the 32 bit value into XY
;;
;; OUTPUT: X/Y the 32 bit value to load (Y = loword)
;;
;; REQUIRES: 16 bit Index
.macro LDXY param, index
	.ifblank index
		.if (.match (.left (1, {param}), #))
			; immediate mode
			LDY	#.loword(.right (.tcount ({param})-1, {param}))
			LDX	#.hiword(.right (.tcount ({param})-1, {param}))
		.else
			; absolute or zero page
			LDY	param
			LDX	2 + (param)
		.endif
	.else
		.if (.match (.left (1, {param}), #))
			.error "invalid addressing mode"
		.else
			; absolute or zero page
			LDY	param, index
			LDX	2 + (param), index
		.endif
	.endif
.endmacro

;; Stores the 32 bit XY into a variable
;;
;; INPUT: X/Y the 32 bit value to load (Y = loword)
;;
;; REQUIRES: 16 bit Index
.macro STXY param, index
	.ifblank index
		; absolute or zero page
		STY	param
		STX	2 + (param)
	.else
		; absolute or zero page
		STY	param, index
		STX	2 + (param), index
	.endif
.endmacro


;; Pushes the 32 bit XY into the Stack (loword first)
;;
;; INPUT: X/Y the 32 bit value to load (Y = loword)
;;
;; REQUIRES: 16 bit Index
.macro PHXY
	PHX
	PHY
.endmacro


;; Pulls the 32 bit XY from the Stack
;;
;; OUTPUT: X/Y the 32 bit value to load (Y = loword)
;;
;; REQUIRES: 16 bit Index
.macro PLXY
	PLY
	PLX
.endmacro

;
; Branching
; ---------
;

;; Branch less than after CMP
.macro BLT target
	BCC	target
.endmacro

;; Branch greater than or equal after CMP
.macro BGE target
	BCS	target
.endmacro

;; Branch greater than after CMP
.macro BGT target
	.local here

	BEQ	here
	BCS	target
here:
.endmacro

;; Branch less than or equal after CMP
.macro BLE target
	BCC	target
	BEQ	target
.endmacro


;
; Long branches
; -------------

;; Long branch always
;; Uses branch is possible (save 1 byte, same cycles)
.macro JRA target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BRA	target
	.else
		JMP	target
	.endif
.endmacro

;; Long branch if z set
.macro JEQ target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BEQ	target
	.else
		.local skip
		BNE	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch if z clear
.macro JNE target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BNE	target
	.else
		.local skip
		BEQ	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch if c clear
.macro JCC target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BCC	target
	.else
		.local skip
		BCS	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch if c set
.macro JCS target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BCS	target
	.else
		.local skip
		BCC	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch if n set 
.macro JMI target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BMI	target
	.else
		.local skip
		BPL	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch if n clear
.macro JPL target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BPL	target
	.else
		.local skip
		BMI	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch if v set 
.macro JVS target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BVS	target
	.else
		.local skip
		BVC	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch if v clear
.macro JVC target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BVC	target
	.else
		.local skip
		BVS	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch less than after CMP
.macro JLT target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BLT	target
	.else
		.local skip
		BGE	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch greater than or equal after CMP
.macro JGE target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BGE	target
	.else
		.local skip
		BLT	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch greater than after CMP
.macro JGT target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BGT	target
	.else
		.local skip
		BLE	skip
		JMP	target
skip:
	.endif
.endmacro

;; Long branch less than or equal after CMP
.macro JLE target
	.if .def(target) .and ((*+2)-(target) <= 127)
		BLE	target
	.else
		.local skip
		BGT	skip
		JMP	target
skip:
	.endif
.endmacro

.endif ; ::_SYNTHETIC_INC_

; vim: ft=asm:

