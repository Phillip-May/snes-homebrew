;; structure.inc
;;
;; Macros to handle structure.
;;
;; Inspired by:
;; 	* [Adventures in NESDEV - HL macros technical](http://mynesdev.blogspot.ca/2012/10/hl-macros-technical.html)
;;	* [Wilson Mines Company - Structure Macros](http://wilsonminesco.com/StructureMacros/index.html)
;;
;; I first tried using the HL macros for 65816 code, but they kept mixing
;; 8/16 bit state and failing to generate correct code and thus tried to
;; look for something simpler.
;; I was looking at the [6502 forum](http://forum.6502.org/) and found
;; [Wilson Mines Company's Structure macros](http://wilsonminesco.com/StructureMacros/index.html).
;;
;; After understanding how the structure macros are made, I crated my own.
;;
;; These macros allow for status flag checking to be used in conditional code.
;; The actual branch that is used is the opposite of what is indicated by
;; macro name (it improves readability).
;;
;; The following structures are supported:
;;        * IF_CONDITION ... [ ELSE ... ] ENDIF
;;        * IFL_CONDITION ... [ ELSEL ... ] ENDIF
;;	  * IF_BIT arg ... [ ELSE_BIT arg ] ... [ ELSE ... ] ENDIF
;;	  * IF_NOT_BIT arg ... [ ELSE_BIT arg ] ... [ ELSE ... ] ENDIF
;;        * REPEAT ... FOREVER
;;        * REPEAT ... UNTIL_CONDITION
;;        * REPEAT ... WHILE_CONDITION ... WEND
;;        * FOR_XY start, direction, end ... NEXT
;;        * CONTINUE for all loops
;;        * BREAK for all loops
;;
;; These macros provide the following functionality:
;;
;;   * An 8 level stack. Up to 8 IFs and REPEATs can be nested in one code.
;;     A .fatal will be emitted if the stack is overrun or underrun.
;;
;;   * A fatal will be emitted if a ENDIF / ELSE / UNTIL / NEXT
;;	 is used without the matching IF / REPEAT / FOR on the stack.
;;
;;   * Long branches will be used on REPEAT / FOR if necessary.
;;
;;   * BREAK / CONTINUE for loops (can be used inside IFs).
;;	Also BREAK_LABEL and CONTINUE_LABEL for direct access
;;
;;   * Generates the minimum code needed as all testing is done outside the
;;     structure macros (usually with CMP, CPX, XPY, AND, EOR, BIT, etc).
;;
;; Caveats:
;;
;;   * If a NEXT / UNTIL is missing, no error will be produced.
;;     There is no way for ca65 to automatically call a macro at end of file.
;;
;;   * Decrementing FOR loops will only exit if equal.
;;
;;   * Incrementing FOR loops will only exit if greater than or equal.


.ifndef ::_STRUCTURE_INC_
::_STRUCTURE_INC_ = 1

.include "includes/synthetic.inc"


.define __STRUCTURE_TYPE_IF	 1
.define __STRUCTURE_TYPE_IF_BIT	 2
.define __STRUCTURE_TYPE_REPEAT  3
.define __STRUCTURE_TYPE_WHILE   4
.define __STRUCTURE_TYPE_FOR     5


::__STRUCTURE_STACK_TYPE_12 .set 0
::__STRUCTURE_STACK_TYPE_11 .set 0
::__STRUCTURE_STACK_TYPE_10 .set 0
::__STRUCTURE_STACK_TYPE_09 .set 0
::__STRUCTURE_STACK_TYPE_08 .set 0
::__STRUCTURE_STACK_TYPE_07 .set 0
::__STRUCTURE_STACK_TYPE_06 .set 0
::__STRUCTURE_STACK_TYPE_05 .set 0
::__STRUCTURE_STACK_TYPE_04 .set 0
::__STRUCTURE_STACK_TYPE_03 .set 0
::__STRUCTURE_STACK_TYPE_02 .set 0
::__STRUCTURE_STACK_TYPE_01 .set 0

::__STRUCTURE_STACK_LOOP_12 .set 0
::__STRUCTURE_STACK_LOOP_11 .set 0
::__STRUCTURE_STACK_LOOP_10 .set 0
::__STRUCTURE_STACK_LOOP_09 .set 0
::__STRUCTURE_STACK_LOOP_08 .set 0
::__STRUCTURE_STACK_LOOP_07 .set 0
::__STRUCTURE_STACK_LOOP_06 .set 0
::__STRUCTURE_STACK_LOOP_05 .set 0
::__STRUCTURE_STACK_LOOP_04 .set 0
::__STRUCTURE_STACK_LOOP_03 .set 0
::__STRUCTURE_STACK_LOOP_02 .set 0
::__STRUCTURE_STACK_LOOP_01 .set 0

::__STRUCTURE_STACK_ID_12 .set 0
::__STRUCTURE_STACK_ID_11 .set 0
::__STRUCTURE_STACK_ID_10 .set 0
::__STRUCTURE_STACK_ID_09 .set 0
::__STRUCTURE_STACK_ID_08 .set 0
::__STRUCTURE_STACK_ID_07 .set 0
::__STRUCTURE_STACK_ID_06 .set 0
::__STRUCTURE_STACK_ID_05 .set 0
::__STRUCTURE_STACK_ID_04 .set 0
::__STRUCTURE_STACK_ID_03 .set 0
::__STRUCTURE_STACK_ID_02 .set 0
::__STRUCTURE_STACK_ID_01 .set 0

::__STRUCTURE_STACK_BIT_01 .set 0
::__STRUCTURE_STACK_BIT_02 .set 0

::__STRUCTURE_STACK_PREV  .set 0

::__STRUCTURE_STACK_COUNT .set 0


; need to setup new stack


.define __STRUCTURE_STACK_LABEL .ident(.sprintf("__STRUCTURE_%04X", ::__STRUCTURE_STACK_ID_01))
.define __STRUCTURE_STACK_PREV_LABEL .ident(.sprintf("__STRUCTURE_%04X", ::__STRUCTURE_STACK_PREV))
.define __STRUCTURE_END_IF_BIT_LABEL .ident(.sprintf("__STRUCTURE_IF_BIT_%04X", ::__STRUCTURE_STACK_BIT_01))
.define BREAK_LABEL .ident(.sprintf("__STRUCTURE_BREAK_%04X", ::__STRUCTURE_STACK_LOOP_01))
.define CONTINUE_LABEL .ident(.sprintf("__STRUCTURE_CONTINUE_%04X", ::__STRUCTURE_STACK_LOOP_01))


; creates a new label name and adds it to the if stack
.macro __STRUCTURE_STACK_PUSH_NEW type
	::__STRUCTURE_STACK_COUNT .set ::__STRUCTURE_STACK_COUNT + 1

	.if ::__STRUCTURE_STACK_TYPE_12 <> 0
		.fatal "Too many structures"
	.endif

	; Push to loop stack only if it is a looping structure
	.if type = __STRUCTURE_TYPE_REPEAT .or type = __STRUCTURE_TYPE_FOR
		::__STRUCTURE_STACK_LOOP_12 .set ::__STRUCTURE_STACK_LOOP_11
		::__STRUCTURE_STACK_LOOP_11 .set ::__STRUCTURE_STACK_LOOP_10
		::__STRUCTURE_STACK_LOOP_10 .set ::__STRUCTURE_STACK_LOOP_09
		::__STRUCTURE_STACK_LOOP_09 .set ::__STRUCTURE_STACK_LOOP_08
		::__STRUCTURE_STACK_LOOP_08 .set ::__STRUCTURE_STACK_LOOP_07
		::__STRUCTURE_STACK_LOOP_07 .set ::__STRUCTURE_STACK_LOOP_06
		::__STRUCTURE_STACK_LOOP_06 .set ::__STRUCTURE_STACK_LOOP_05
		::__STRUCTURE_STACK_LOOP_05 .set ::__STRUCTURE_STACK_LOOP_04
		::__STRUCTURE_STACK_LOOP_04 .set ::__STRUCTURE_STACK_LOOP_03
		::__STRUCTURE_STACK_LOOP_03 .set ::__STRUCTURE_STACK_LOOP_02
		::__STRUCTURE_STACK_LOOP_02 .set ::__STRUCTURE_STACK_LOOP_01
		::__STRUCTURE_STACK_LOOP_01 .set ::__STRUCTURE_STACK_COUNT 
	.endif

	; Stack for the IF_BIT code
	.if type = __STRUCTURE_TYPE_IF_BIT
		.if ::__STRUCTURE_STACK_BIT_02 <> 0
			.fatal "Too many structures"
		.endif
		::__STRUCTURE_STACK_BIT_02 .set ::__STRUCTURE_STACK_BIT_01
		::__STRUCTURE_STACK_BIT_01 .set __STRUCTURE_STACK_COUNT
	.endif

	::__STRUCTURE_STACK_TYPE_12 .set ::__STRUCTURE_STACK_TYPE_11
	::__STRUCTURE_STACK_TYPE_11 .set ::__STRUCTURE_STACK_TYPE_10
	::__STRUCTURE_STACK_TYPE_10 .set ::__STRUCTURE_STACK_TYPE_09
	::__STRUCTURE_STACK_TYPE_09 .set ::__STRUCTURE_STACK_TYPE_08
	::__STRUCTURE_STACK_TYPE_08 .set ::__STRUCTURE_STACK_TYPE_07
	::__STRUCTURE_STACK_TYPE_07 .set ::__STRUCTURE_STACK_TYPE_06
	::__STRUCTURE_STACK_TYPE_06 .set ::__STRUCTURE_STACK_TYPE_05
	::__STRUCTURE_STACK_TYPE_05 .set ::__STRUCTURE_STACK_TYPE_04
	::__STRUCTURE_STACK_TYPE_04 .set ::__STRUCTURE_STACK_TYPE_03
	::__STRUCTURE_STACK_TYPE_03 .set ::__STRUCTURE_STACK_TYPE_02
	::__STRUCTURE_STACK_TYPE_02 .set ::__STRUCTURE_STACK_TYPE_01
	::__STRUCTURE_STACK_TYPE_01 .set type

	::__STRUCTURE_STACK_ID_12 .set ::__STRUCTURE_STACK_ID_11
	::__STRUCTURE_STACK_ID_11 .set ::__STRUCTURE_STACK_ID_10
	::__STRUCTURE_STACK_ID_10 .set ::__STRUCTURE_STACK_ID_09
	::__STRUCTURE_STACK_ID_09 .set ::__STRUCTURE_STACK_ID_08
	::__STRUCTURE_STACK_ID_08 .set ::__STRUCTURE_STACK_ID_07
	::__STRUCTURE_STACK_ID_07 .set ::__STRUCTURE_STACK_ID_06
	::__STRUCTURE_STACK_ID_06 .set ::__STRUCTURE_STACK_ID_05
	::__STRUCTURE_STACK_ID_05 .set ::__STRUCTURE_STACK_ID_04
	::__STRUCTURE_STACK_ID_04 .set ::__STRUCTURE_STACK_ID_03
	::__STRUCTURE_STACK_ID_03 .set ::__STRUCTURE_STACK_ID_02
	::__STRUCTURE_STACK_ID_02 .set ::__STRUCTURE_STACK_ID_01
	::__STRUCTURE_STACK_ID_01 .set ::__STRUCTURE_STACK_COUNT
.endmacro

.macro __STRUCTURE_STACK_NEW_ID
	::__STRUCTURE_STACK_PREV  .set ::__STRUCTURE_STACK_ID_01

	::__STRUCTURE_STACK_COUNT .set ::__STRUCTURE_STACK_COUNT + 1
	::__STRUCTURE_STACK_ID_01 .set ::__STRUCTURE_STACK_COUNT
.endmacro

;; Ensure current structure is of type
.macro __STRUCTURE_STACK_CHECK type
	.if type <> ::__STRUCTURE_STACK_TYPE_01
		.if ::__STRUCTURE_STACK_TYPE_01 = __STRUCTURE_TYPE_IF_BIT .and type = __STRUCTURE_TYPE_IF
			; ::HACK let IF_BIT be treated as IF::
		.elseif type = __STRUCTURE_TYPE_IF
			.fatal "Missing IF"
		.elseif type = __STRUCTURE_TYPE_IF_BIT
			.fatal "Missing IF_BIT or IF_NOT_BIT"
		.elseif type = __STRUCTURE_TYPE_REPEAT
			.fatal "Missing REPEAT"
		.elseif type = __STRUCTURE_TYPE_FOR
			.fatal "Missing FOR_X or FOR_Y"
		.else
			.fatal "Missing statement of unkown type"
		.endif
	.endif
.endmacro

; emits the current name as a label and removes it from the stack
; also checks to ensure this is the proper type
.macro __STRUCTURE_STACK_POP_CHECK type
	__STRUCTURE_STACK_CHECK type

	; Pop from loop stack only if it is a looping structure
	.if type = __STRUCTURE_TYPE_REPEAT .or type = __STRUCTURE_TYPE_WHILE .or type = __STRUCTURE_TYPE_FOR
		::__STRUCTURE_STACK_LOOP_01 .set ::__STRUCTURE_STACK_LOOP_02
		::__STRUCTURE_STACK_LOOP_02 .set ::__STRUCTURE_STACK_LOOP_03
		::__STRUCTURE_STACK_LOOP_03 .set ::__STRUCTURE_STACK_LOOP_04
		::__STRUCTURE_STACK_LOOP_04 .set ::__STRUCTURE_STACK_LOOP_05
		::__STRUCTURE_STACK_LOOP_05 .set ::__STRUCTURE_STACK_LOOP_06
		::__STRUCTURE_STACK_LOOP_06 .set ::__STRUCTURE_STACK_LOOP_07
		::__STRUCTURE_STACK_LOOP_07 .set ::__STRUCTURE_STACK_LOOP_08
		::__STRUCTURE_STACK_LOOP_08 .set ::__STRUCTURE_STACK_LOOP_09
		::__STRUCTURE_STACK_LOOP_09 .set ::__STRUCTURE_STACK_LOOP_10
		::__STRUCTURE_STACK_LOOP_10 .set ::__STRUCTURE_STACK_LOOP_11
		::__STRUCTURE_STACK_LOOP_11 .set ::__STRUCTURE_STACK_LOOP_12
		::__STRUCTURE_STACK_LOOP_12 .set 0
	.endif

	; Stack for the IF_BIT code
	.if ::__STRUCTURE_STACK_TYPE_01 = __STRUCTURE_TYPE_IF_BIT
		::__STRUCTURE_STACK_BIT_01 .set ::__STRUCTURE_STACK_BIT_02
		::__STRUCTURE_STACK_BIT_02 .set 0
	.endif

	; ELSE statements need this after pop&push.
	; Save it so it can be used after the branch
	::__STRUCTURE_STACK_PREV  .set ::__STRUCTURE_STACK_ID_01

	::__STRUCTURE_STACK_TYPE_01 .set ::__STRUCTURE_STACK_TYPE_02
	::__STRUCTURE_STACK_TYPE_02 .set ::__STRUCTURE_STACK_TYPE_03
	::__STRUCTURE_STACK_TYPE_03 .set ::__STRUCTURE_STACK_TYPE_04
	::__STRUCTURE_STACK_TYPE_04 .set ::__STRUCTURE_STACK_TYPE_05
	::__STRUCTURE_STACK_TYPE_05 .set ::__STRUCTURE_STACK_TYPE_06
	::__STRUCTURE_STACK_TYPE_06 .set ::__STRUCTURE_STACK_TYPE_07
	::__STRUCTURE_STACK_TYPE_07 .set ::__STRUCTURE_STACK_TYPE_08
	::__STRUCTURE_STACK_TYPE_08 .set ::__STRUCTURE_STACK_TYPE_09
	::__STRUCTURE_STACK_TYPE_09 .set ::__STRUCTURE_STACK_TYPE_10
	::__STRUCTURE_STACK_TYPE_10 .set ::__STRUCTURE_STACK_TYPE_11
	::__STRUCTURE_STACK_TYPE_11 .set ::__STRUCTURE_STACK_TYPE_12
	::__STRUCTURE_STACK_TYPE_12 .set 0

	::__STRUCTURE_STACK_ID_01 .set ::__STRUCTURE_STACK_ID_02
	::__STRUCTURE_STACK_ID_02 .set ::__STRUCTURE_STACK_ID_03
	::__STRUCTURE_STACK_ID_03 .set ::__STRUCTURE_STACK_ID_04
	::__STRUCTURE_STACK_ID_04 .set ::__STRUCTURE_STACK_ID_05
	::__STRUCTURE_STACK_ID_05 .set ::__STRUCTURE_STACK_ID_06
	::__STRUCTURE_STACK_ID_06 .set ::__STRUCTURE_STACK_ID_07
	::__STRUCTURE_STACK_ID_07 .set ::__STRUCTURE_STACK_ID_08
	::__STRUCTURE_STACK_ID_08 .set ::__STRUCTURE_STACK_ID_09
	::__STRUCTURE_STACK_ID_09 .set ::__STRUCTURE_STACK_ID_10
	::__STRUCTURE_STACK_ID_10 .set ::__STRUCTURE_STACK_ID_11
	::__STRUCTURE_STACK_ID_11 .set ::__STRUCTURE_STACK_ID_12
	::__STRUCTURE_STACK_ID_12 .set 0
.endmacro


;; IF
;;; Tests A bits, branches if A & param == 0
;;; Index is optional
.macro IF_BIT param, index
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF_BIT

	.ifblank index
		BIT	param
	.else
		BIT	param, index
	.endif

	BEQ __STRUCTURE_STACK_LABEL
.endmacro


;;; Tests A bits, branches if A & param != 0
;;; Index is optional
.macro IF_NOT_BIT param, index
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF_BIT

	.ifblank index
		BIT	param
	.else
		BIT	param, index
	.endif

	BNE __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests z flag
.macro IF_ZERO
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BNE __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests z flag
.macro IF_NOT_ZERO
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BEQ __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests n Flag
.macro IF_PLUS
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BMI __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests n Flag
.macro IF_MINUS
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BPL __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests c Flag
.macro IF_C_CLEAR
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BCS __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests c Flag
.macro IF_C_SET
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BCC __STRUCTURE_STACK_LABEL
.endmacro


;;; Tests z Flag
.macro IF_Z_CLEAR
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BEQ __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests z Flag
.macro IF_Z_SET
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BNE __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests v Flag
.macro IF_V_CLEAR
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BVS __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests v Flag
.macro IF_V_SET
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BVC __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests n Flag
.macro IF_N_CLEAR
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BMI __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests n Flag
.macro IF_N_SET
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BPL __STRUCTURE_STACK_LABEL
.endmacro


;;; Equals after CMP
.macro IF_EQ
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF
	BNE __STRUCTURE_STACK_LABEL
.endmacro

;;; Not equals after CMP
.macro IF_NE
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF
	BEQ __STRUCTURE_STACK_LABEL
.endmacro

;;; Unsigned Greater than or equal after CMP
.macro IF_GE
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BLT __STRUCTURE_STACK_LABEL
.endmacro

;;; Unsigned Greater than after CMP
.macro IF_GT
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BLE __STRUCTURE_STACK_LABEL
.endmacro

;;; Unsigned Less than after CMP
.macro IF_LT
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BGE __STRUCTURE_STACK_LABEL
.endmacro

;;; Unsigned Less than or equal after CMP
.macro IF_LE
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	BGT __STRUCTURE_STACK_LABEL
.endmacro



;; IF Long Branch
;;; Tests z flag
.macro IFL_ZERO
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JNE __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests z flag
.macro IFL_NOT_ZERO
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JEQ __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests n Flag
.macro IFL_PLUS
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JMI __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests n Flag
.macro IFL_MINUS
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JPL __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests c Flag
.macro IFL_C_CLEAR
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JCS __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests c Flag
.macro IFL_C_SET
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JCC __STRUCTURE_STACK_LABEL
.endmacro


;;; Tests z Flag
.macro IFL_Z_CLEAR
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JEQ __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests z Flag
.macro IFL_Z_SET
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JNE __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests v Flag
.macro IFL_V_CLEAR
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JVS __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests v Flag
.macro IFL_V_SET
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JVC __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests n Flag
.macro IFL_N_CLEAR
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JMI __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests n Flag
.macro IFL_N_SET
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JPL __STRUCTURE_STACK_LABEL
.endmacro


;;; Equals after CMP
.macro IFL_EQ
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF
	JNE __STRUCTURE_STACK_LABEL
.endmacro

;;; Not equals after CMP
.macro IFL_NE
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF
	JEQ __STRUCTURE_STACK_LABEL
.endmacro

;;; Unsigned Greater than or equal after CMP
.macro IFL_GE
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JLT __STRUCTURE_STACK_LABEL
.endmacro

;;; Unsigned Greater than after CMP
.macro IFL_GT
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JLE __STRUCTURE_STACK_LABEL
.endmacro

;;; Unsigned Less than after CMP
.macro IFL_LT
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JGE __STRUCTURE_STACK_LABEL
.endmacro

;;; Unsigned Less than or equal after CMP
.macro IFL_LE
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_IF

	JGT __STRUCTURE_STACK_LABEL
.endmacro



;; Else
.macro ELSE
	__STRUCTURE_STACK_CHECK __STRUCTURE_TYPE_IF
	__STRUCTURE_STACK_NEW_ID

	BRA __STRUCTURE_STACK_LABEL
__STRUCTURE_STACK_PREV_LABEL:
.endmacro

;; Else Long Branch
.macro ELSEL
	__STRUCTURE_STACK_CHECK __STRUCTURE_TYPE_IF
	__STRUCTURE_STACK_NEW_ID

	JRA __STRUCTURE_STACK_LABEL

__STRUCTURE_STACK_PREV_LABEL:
.endmacro


;; Else BIT test, branch if A & param == 0
;; Index is optional
.macro ELSE_BIT param, index
	__STRUCTURE_STACK_CHECK __STRUCTURE_TYPE_IF

	__STRUCTURE_STACK_NEW_ID

	BRA	__STRUCTURE_END_IF_BIT_LABEL

__STRUCTURE_STACK_PREV_LABEL:

	.ifblank index
		BIT	param
	.else
		BIT	param, index
	.endif

	BEQ __STRUCTURE_STACK_LABEL
.endmacro

;; Else not BIT test, branch if A & param != 0
;; Index is optional
.macro ELSE_NOT_BIT param, index
	__STRUCTURE_STACK_CHECK __STRUCTURE_TYPE_IF

	__STRUCTURE_STACK_NEW_ID

	BRA	__STRUCTURE_END_IF_BIT_LABEL

__STRUCTURE_STACK_PREV_LABEL:

	.ifblank index
		BIT	param
	.else
		BIT	param, index
	.endif

	BNE __STRUCTURE_STACK_LABEL
.endmacro


;; End if
.macro ENDIF
	.if ::__STRUCTURE_STACK_TYPE_01 = __STRUCTURE_TYPE_IF_BIT
__STRUCTURE_END_IF_BIT_LABEL:
	.endif

__STRUCTURE_STACK_LABEL:

	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_IF
.endmacro





;; REPEAT structure, expects a FOREVER or a UNTIL_*
.macro REPEAT
	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_REPEAT

__STRUCTURE_STACK_LABEL:
CONTINUE_LABEL:
.endmacro

;; Loops a REPEAT forever
.macro FOREVER
	JRA __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;; UNTIL
;;; Tests z Flag
.macro UNTIL_ZERO
	JNE __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Tests z Flag
.macro UNTIL_NOT_ZERO
	JEQ __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Tests n Flag
.macro UNTIL_PLUS
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT

BREAK_LABEL:
	JMI __STRUCTURE_STACK_LABEL
.endmacro

;;; Tests n Flag
.macro UNTIL_MINUS
	JPL __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Tests c Flag
.macro UNTIL_C_CLEAR
	JCS __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Tests c Flag
.macro UNTIL_C_SET
	JCC __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro


;;; Tests z Flag
.macro UNTIL_Z_CLEAR
	JEQ __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Tests z Flag
.macro UNTIL_Z_SET
	JNE __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Tests v Flag
.macro UNTIL_V_CLEAR
	JVS __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Tests v Flag
.macro UNTIL_V_SET
	JVC __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Tests n Flag
.macro UNTIL_N_CLEAR
	JMI __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Tests n Flag
.macro UNTIL_N_SET
	JPL __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro


;;; Equals after CMP
.macro UNTIL_EQ
	JNE __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Not equals after CMP
.macro UNTIL_NE
	JEQ __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Unsigned Greater than or equal after CMP
.macro UNTIL_GE
	JLT __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Unsigned Greater than after CMP
.macro UNTIL_GT
	JLE __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Unsigned Less than after CMP
.macro UNTIL_LT
	JGE __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro

;;; Unsigned Less than or equal after CMP
.macro UNTIL_LE
	JGT __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_REPEAT
.endmacro


.macro __WHILE_HELPER
	.if ::__STRUCTURE_STACK_TYPE_01 <> __STRUCTURE_TYPE_REPEAT
		.fatal "Missing REPEAT"
	.endif

	::__STRUCTURE_STACK_TYPE_01 .set __STRUCTURE_TYPE_WHILE
.endmacro

;; WHILE

;;; Tests z flag
.macro WHILE_ZERO
	__WHILE_HELPER

	BNE BREAK_LABEL
.endmacro

;;; Tests z flag
.macro WHILE_NOT_ZERO
	__WHILE_HELPER

	BEQ BREAK_LABEL
.endmacro

;;; Tests n Flag
.macro WHILE_PLUS
	__WHILE_HELPER

	BMI BREAK_LABEL
.endmacro

;;; Tests n Flag
.macro WHILE_MINUS
	__WHILE_HELPER

	BPL BREAK_LABEL
.endmacro

;;; Tests c Flag
.macro WHILE_C_CLEAR
	__WHILE_HELPER

	BCS BREAK_LABEL
.endmacro

;;; Tests c Flag
.macro WHILE_C_SET
	__WHILE_HELPER

	BCC BREAK_LABEL
.endmacro


;;; Tests z Flag
.macro WHILE_Z_CLEAR
	__WHILE_HELPER

	BEQ BREAK_LABEL
.endmacro

;;; Tests z Flag
.macro WHILE_Z_SET
	__WHILE_HELPER

	BNE BREAK_LABEL
.endmacro

;;; Tests v Flag
.macro WHILE_V_CLEAR
	__WHILE_HELPER

	BVS BREAK_LABEL
.endmacro

;;; Tests v Flag
.macro WHILE_V_SET
	__WHILE_HELPER

	BVC BREAK_LABEL
.endmacro

;;; Tests n Flag
.macro WHILE_N_CLEAR
	__WHILE_HELPER

	BMI BREAK_LABEL
.endmacro

;;; Tests n Flag
.macro WHILE_N_SET
	__WHILE_HELPER

	BPL BREAK_LABEL
.endmacro


;;; Equals after CMP
.macro WHILE_EQ
	__WHILE_HELPER
	BNE BREAK_LABEL
.endmacro

;;; Not equals after CMP
.macro WHILE_NE
	__WHILE_HELPER
	BEQ BREAK_LABEL
.endmacro

;;; Unsigned Greater than or equal after CMP
.macro WHILE_GE
	__WHILE_HELPER

	BLT BREAK_LABEL
.endmacro

;;; Unsigned Greater than after CMP
.macro WHILE_GT
	__WHILE_HELPER

	BLE BREAK_LABEL
.endmacro

;;; Unsigned Less than after CMP
.macro WHILE_LT
	__WHILE_HELPER

	BGE BREAK_LABEL
.endmacro

;;; Unsigned Less than or equal after CMP
.macro WHILE_LE
	__WHILE_HELPER

	BGT BREAK_LABEL
.endmacro




;; End REPEAT ... WHILE loop
;;
;; Loops back to the REPEAT statement
.macro WEND
	JRA __STRUCTURE_STACK_LABEL

BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_WHILE
.endmacro




;; For Loops
;;
;; Usage:
;;
;;	FOR_X #0, INC, #6
;;		<code>
;;	NEXT
;;
;;	FOR_Y count, DEC, #0
;;		<code>
;;	NEXT

::__FOR_TYPE      .set 0
::__FOR_EXIT      .set 0

.define __FOR_TYPE_INX  101
.define __FOR_TYPE_DEX  111
.define __FOR_TYPE_INX2 102
.define __FOR_TYPE_DEX2 112
.define __FOR_TYPE_INY  201
.define __FOR_TYPE_DEY  211
.define __FOR_TYPE_INY2 202
.define __FOR_TYPE_DEY2 212


.macro FOR_X start, direction, exit
	.if ::__FOR_TYPE <> 0
		.fatal "Only 1 FOR loop at a time"
	.endif

	.if .xmatch(.left(1, {exit}), #)
		::__FOR_EXIT .set .mid(1, .tcount({exit}) - 1, {exit})
	.else
		.fatal .sprintf("FOR_X: exit must be constant")
	.endif

	.if .xmatch(direction, INC)
		::__FOR_TYPE .set __FOR_TYPE_INX
	.elseif .xmatch(direction, DEC)
		::__FOR_TYPE .set __FOR_TYPE_DEX
	.elseif .xmatch(direction, INC2)
		::__FOR_TYPE .set __FOR_TYPE_INX2
	.elseif .xmatch(direction, DEC2)
		::__FOR_TYPE .set __FOR_TYPE_DEX2
	.else
		.fatal "Unkown direction (DEC, INC, DEC2, INC2 are valid)"
	.endif

	.ifnblank start
		LDX start
	.endif

	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_FOR

__STRUCTURE_STACK_LABEL:
.endmacro

.macro FOR_Y start, direction, exit
	.if ::__FOR_TYPE <> 0
		.fatal "Only 1 FOR loop at a time"
	.endif

	.if .xmatch(.left(1, {exit}), #)
		::__FOR_EXIT .set .mid(1, .tcount({exit}) - 1, {exit})
	.else
		.fatal .sprintf("FOR_Y: exit must be constant")
	.endif

	.if .xmatch(direction, INC)
		::__FOR_TYPE .set __FOR_TYPE_INY
	.elseif .xmatch(direction, DEC)
		::__FOR_TYPE .set __FOR_TYPE_DEY
	.elseif .xmatch(direction, INC2)
		::__FOR_TYPE .set __FOR_TYPE_INY2
	.elseif .xmatch(direction, DEC2)
		::__FOR_TYPE .set __FOR_TYPE_DEY2
	.else
		.fatal "Unkown direction (DEC, INC, DEC2, INC2 are valid)"
	.endif

	.ifnblank start
		LDY start
	.endif

	__STRUCTURE_STACK_PUSH_NEW __STRUCTURE_TYPE_FOR

__STRUCTURE_STACK_LABEL:
.endmacro

.macro NEXT
CONTINUE_LABEL:

	.if ::__FOR_TYPE = __FOR_TYPE_DEX
		DEX
		.if ::__FOR_EXIT = 0
			JNE __STRUCTURE_STACK_LABEL
		.else
			CPX	#::__FOR_EXIT
			JGT __STRUCTURE_STACK_LABEL
		.endif 
	.elseif ::__FOR_TYPE = __FOR_TYPE_INX
		INX
		.if ::__FOR_EXIT = 0
			JNE __STRUCTURE_STACK_LABEL
		.else
			CPX	#::__FOR_EXIT
			JLT __STRUCTURE_STACK_LABEL
		.endif 
	.elseif ::__FOR_TYPE = __FOR_TYPE_DEX2
		DEX
		DEX
		.if ::__FOR_EXIT = 0
			JNE __STRUCTURE_STACK_LABEL
		.else
			CPX	#::__FOR_EXIT
			JGT __STRUCTURE_STACK_LABEL
		.endif 
	.elseif ::__FOR_TYPE = __FOR_TYPE_INX2
		INX
		INX
		.if ::__FOR_EXIT = 0
			JNE __STRUCTURE_STACK_LABEL
		.else
			CPX	#::__FOR_EXIT
			JLT __STRUCTURE_STACK_LABEL
		.endif 
	.elseif ::__FOR_TYPE = __FOR_TYPE_DEY
		DEY
		.if ::__FOR_EXIT = 0
			JNE __STRUCTURE_STACK_LABEL
		.else
			CPY	#::__FOR_EXIT
			JGT __STRUCTURE_STACK_LABEL
		.endif 
	.elseif ::__FOR_TYPE = __FOR_TYPE_INY
		INY
		.if ::__FOR_EXIT = 0
			JNE __STRUCTURE_STACK_LABEL
		.else
			CPY	#::__FOR_EXIT
			JLT __STRUCTURE_STACK_LABEL
		.endif 
	.elseif ::__FOR_TYPE = __FOR_TYPE_DEY2
		DEY
		DEY
		.if ::__FOR_EXIT = 0
			JNE __STRUCTURE_STACK_LABEL
		.else
			CPY	#::__FOR_EXIT
			JGT __STRUCTURE_STACK_LABEL
		.endif 
	.elseif ::__FOR_TYPE = __FOR_TYPE_INY2
		INY
		INY
		.if ::__FOR_EXIT = 0
			JNE __STRUCTURE_STACK_LABEL
		.else
			CPY	#::__FOR_EXIT
			JLT __STRUCTURE_STACK_LABEL
		.endif 
	.else
		.fatal "Unknown FOR type"
	.endif


BREAK_LABEL:
	__STRUCTURE_STACK_POP_CHECK __STRUCTURE_TYPE_FOR

	; Free the FOR variables
	::__FOR_TYPE .set 0
	::__FOR_EXIT .set 0

.endmacro


;; Break a loop
.macro BREAK
	JRA BREAK_LABEL
.endmacro

;; Continue a loop
.macro CONTINUE
	JRA CONTINUE_LABEL
.endmacro

.endif ; ::_STRUCTURE_INC_

; vim: ft=asm:

