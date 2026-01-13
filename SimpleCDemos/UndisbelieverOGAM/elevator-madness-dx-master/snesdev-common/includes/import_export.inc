;; import_export.inc
;;
;; Simplifies the automation of importing and exporting variables
;; and functions between modules.
;;
;; To use this file, you need two files, a source and a header.
;;
;; The source file should start the module with
;;
;;	MODULE <name>
;;
;; And end with
;;
;;	END_MODULE
;;
;;
;; You can then define a variable, label or a routine with the following macros
;; 
;;	BYTE, WORD, ADDR, LONG, STRUCT
;;	UINT8, UINT16, SINT16, UINT32, SINT32
;;
;; Which will define a variable of a known size and (optional) count.
;;
;; It will also
;;
;;	* Export a variable called `MODULE_name`
;;	* Also creates:
;;		* `MODULE__name__type` which matches the `TYPE_*` enum symbols
;;		* `MODULE__name__size` containing the total size of the variable in bytes
;;		* `MODULE__name__elsize` containing the size of each element in bytes (if count is used)
;;		* `MODULE__name__count` containing the number of elements
;;
;; Any variables defined inside a `ZEROPAGE` ... `ENDZEROPAGE` block will be 
;; imported/exported within the zero page. 
;;
;; It is also possible to declare a label/routine with the following macros
;;
;;	LABEL, ROUTINE
;;
;;
;; Declaring a module interface using inside the header file is the same as
;; defining variables, routines and labels. Except you use the 
;;
;;	IMPORT_MODULE <name>
;;
;; instead.
;; This will cause use the same symbols as the source code, but it will be scoped into the module name.
;;
;; For example:
;;
;; Source:
;;	MODULE Test
;;	.segment "WRAM"
;;		WORD variable
;;		WORD local Variable
;;	.code
;;
;;	ROUTINE Routine
;;		RTS
;;
;;	ENDMODULE
;;
;; Header:
;;	IMPORT_MODULE Test
;;		WORD variable
;;		ROUTINE Routine
;;	ENDMODULE
;;
;; Will create the following symbols in all files that include the header
;;
;;	Test__variable
;;	Test__Routine
;;	Test__variable__size
;;	Test__variable__type
;;	Test__Routine__type
;;


.ifndef ::_IMPORT_EXPORT_H_
::_IMPORT_EXPORT_H_ = 1

.enum 
	TYPE_BYTE = 1
	TYPE_WORD
	TYPE_ADDR
	TYPE_FARADDR
	TYPE_DWORD
	TYPE_UINT8
	TYPE_SINT8
	TYPE_UINT16
	TYPE_SINT16
	TYPE_UINT32
	TYPE_SINT32
	TYPE_STRUCT
	TYPE_ROUTINE
	TYPE_LABEL
	TYPE_CONST
.endenum

.define __MODULE_TYPE_NONE   0
.define __MODULE_TYPE_EXPORT 1
.define __MODULE_TYPE_IMPORT 2

::__MODULE_TYPE .set 0


;; Defines a module.
;; Must be called at the start of a module file.
.macro MODULE name
	::__MODULE_TYPE .set __MODULE_TYPE_EXPORT
	::__MODULE_ZEROPAGE .set 0

	.define __MODULE_NAME .string(name)
.endmacro

.macro IMPORT_MODULE name
	::__MODULE_TYPE .set __MODULE_TYPE_IMPORT
	::__MODULE_ZEROPAGE .set 0

	.define __MODULE_NAME .string(name)
.endmacro

;; Cleans up the end of a module.
.macro ENDMODULE
	::__MODULE_TYPE .set __MODULE_TYPE_NONE
	.undefine __MODULE_NAME
.endmacro


.macro ZEROPAGE
	.zeropage
	::__MODULE_ZEROPAGE .set 1
.endmacro

.macro ENDZEROPAGE
	::__MODULE_ZEROPAGE .set 0
	.code
.endmacro



.macro _VARIABLE_HELPER name, type, size, count
	.if ::__MODULE_TYPE = __MODULE_TYPE_NONE
		.ident(.sprintf("%s__type", .string(name))) = type

		.ifblank count
			name:	.res size
			.ident(.sprintf("%s__size", .string(name))) = size
		.else
			name:	.res size * (count)
			.ident(.sprintf("%s__elsize", .string(name))) = size
			.ident(.sprintf("%s__size", .string(name))) = size * (count)
			.ident(.sprintf("%s__count", .string(name))) = count
		.endif

		.if ::__MODULE_ZEROPAGE
			.exportzp name
		.else
			.export name
		.endif

	.elseif ::__MODULE_TYPE = __MODULE_TYPE_EXPORT
		.ident(.sprintf("%s__type", .string(name))) = type

		.ifblank count
			name:	.res size
			.ident(.sprintf("%s__size", .string(name))) = size
		.else
			name:	.res size * (count)
			.ident(.sprintf("%s__elsize", .string(name))) = size
			.ident(.sprintf("%s__size", .string(name))) = size * (count)
			.ident(.sprintf("%s__count", .string(name))) = count
		.endif

		.if ::__MODULE_ZEROPAGE
			.exportzp .ident(.sprintf("%s__%s", __MODULE_NAME, .string(name))) := name
		.else
			.export .ident(.sprintf("%s__%s", __MODULE_NAME, .string(name))) := name
		.endif

	.elseif ::__MODULE_TYPE = __MODULE_TYPE_IMPORT
		.ident(.sprintf("%s__%s__type", __MODULE_NAME, .string(name))) = type

		.ifblank count
			.ident(.sprintf("%s__%s__size", __MODULE_NAME, .string(name))) = size
		.else
			.ident(.sprintf("%s__%s__elsize", __MODULE_NAME, .string(name))) = size
			.ident(.sprintf("%s__%s__size", __MODULE_NAME, .string(name))) = size * count
			.ident(.sprintf("%s__%s__count", __MODULE_NAME, .string(name))) = count
		.endif

		.if ::__MODULE_ZEROPAGE
			.globalzp .ident(.sprintf("%s__%s", __MODULE_NAME, .string(name)))
		.else
			.global .ident(.sprintf("%s__%s", __MODULE_NAME, .string(name)))
		.endif
	.endif
.endmacro


.macro _SYMBOL_HELPER name, type
	.if ::__MODULE_TYPE = __MODULE_TYPE_NONE
		.ifndef .ident(.sprintf("%s__type", .string(name)))
			.ident(.sprintf("%s__type", .string(name))) = type
		.elseif .ident(.sprintf("%s__type", .string(name))) <> type
			.fatal .sprintf("Error: %s type mismatch", .string(name))
		.endif
		.global name
		name:

	.elseif ::__MODULE_TYPE = __MODULE_TYPE_EXPORT
		.ident(.sprintf("%s__type", .string(name))) = type
		.export .ident(.sprintf("%s__%s", __MODULE_NAME, .string(name))) := name
		name:

	.elseif ::__MODULE_TYPE = __MODULE_TYPE_IMPORT
		.ident(.sprintf("%s__%s__type", __MODULE_NAME, .string(name))) = type
		.global .ident(.sprintf("%s__%s", __MODULE_NAME, .string(name)))

	.endif
.endmacro

.macro _IMPORT_SYMBOL_HELPER name, type
	.ifndef .ident(.sprintf("%s__type", .string(name)))
		.ident(.sprintf("%s__type", .string(name))) = type
	.elseif .ident(.sprintf("%s__type", .string(name))) <> type
		.fatal .sprintf("Error: %s type mismatch", .string(name))
	.endif

	.global .ident(.sprintf("%s", .string(name)))
.endmacro

.macro CONST name, value
	.if ::__MODULE_TYPE = __MODULE_TYPE_NONE
		.fatal "Require MODULE or IMPORT_MODULE"

	.elseif ::__MODULE_TYPE = __MODULE_TYPE_EXPORT
		.ident(.sprintf("%s__type", .string(name))) .set TYPE_CONST
		name = value

	.elseif ::__MODULE_TYPE = __MODULE_TYPE_IMPORT
		.ident(.sprintf("%s__%s__type", __MODULE_NAME, .string(name))) := TYPE_CONST
		.ident(.sprintf("%s__%s", __MODULE_NAME, .string(name))) = value
	.endif
.endmacro

;; Allows for two variables (with different name) to
;; contain the same memory location, size and type.
.macro SAME_VARIABLE name, dup
	.if ::__MODULE_TYPE = __MODULE_TYPE_NONE
		name := dup
		
		.ident(.sprintf("%s__type", .string(name))) = .ident(.sprintf("%s__type", .string(dup)))

		.if ::__MODULE_ZEROPAGE
			.exportzp name
		.else
			.export name
		.endif

	.elseif ::__MODULE_TYPE = __MODULE_TYPE_EXPORT
		name := dup
		
		.ident(.sprintf("%s__type", .string(name))) = .ident(.sprintf("%s__type", .string(dup)))

		.if ::__MODULE_ZEROPAGE
			.exportzp .ident(.sprintf("%s__%s", __MODULE_NAME, .string(name))) := name
		.else
			.export .ident(.sprintf("%s__%s", __MODULE_NAME, .string(name))) := name
		.endif

	.elseif ::__MODULE_TYPE = __MODULE_TYPE_IMPORT
		.ident(.sprintf("%s__%s__type", __MODULE_NAME, .string(name))) = .ident(.sprintf("%s__%s__type", __MODULE_NAME, .string(dup)))

		.if ::__MODULE_ZEROPAGE
			.globalzp .ident(.sprintf("%s__%s", __MODULE_NAME, .string(name))) 
		.else
			.global .ident(.sprintf("%s__%s", __MODULE_NAME, .string(name)))
		.endif

	.endif
.endmacro


;; Loads a binary file into a label.
;;
;; This Macro can be used in a header if the filename is missing.
;;
;; This macro also exports the label and creates the {label}End label used by the block module.
.macro INCLUDE_BINARY label, filename, start, size
	LABEL label

	.if ::__MODULE_TYPE <> __MODULE_TYPE_IMPORT .and (.not .blank(filename))
		.if .blank(start) .and .blank(size)
			.incbin filename
		.elseif .blank(size)
			.incbin filename, start
		.else
			.incbin filename, start, size
		.endif
	.endif

	LABEL .ident(.sprintf("%s_End", .string(label)))
.endmacro


;; Imports the binary file label that was included by `INCLUDE_BINARY`
;;
;; This Macro can be used in a header even if there is no module.
;;
;; This macro also imports the {label}End label.
.macro IMPORT_BINARY label
	IMPORT_LABEL label
	IMPORT_LABEL .ident(.sprintf("%s_End", .string(label)))
.endmacro


.macro STRUCT name, struct, count
	_VARIABLE_HELPER name, TYPE_STRUCT, .sizeof(struct), count
.endmacro

.macro BYTE name, count
	_VARIABLE_HELPER name, TYPE_BYTE, 1, count
.endmacro

.macro WORD name, count
	_VARIABLE_HELPER name, TYPE_WORD, 2, count
.endmacro

.macro ADDR name, count
	_VARIABLE_HELPER name, TYPE_ADDR, 2, count
.endmacro

.macro FARADDR name, count
	_VARIABLE_HELPER name, TYPE_FARADDR, 3, count
.endmacro

.macro DWORD name, count
	_VARIABLE_HELPER name, TYPE_DWORD, 4, count
.endmacro

.macro UINT8 name, count
	_VARIABLE_HELPER name, TYPE_UINT8, 1, count
.endmacro

.macro SINT8 name, count
	_VARIABLE_HELPER name, TYPE_SINT8, 1, count
.endmacro

.macro UINT16 name, count
	_VARIABLE_HELPER name, TYPE_UINT16, 2, count
.endmacro

.macro SINT16 name, count
	_VARIABLE_HELPER name, TYPE_SINT16, 2, count
.endmacro

.macro UINT32 name, count
	_VARIABLE_HELPER name, TYPE_UINT32, 4, count
.endmacro

.macro SINT32 name, count
	_VARIABLE_HELPER name, TYPE_SINT32, 4, count
.endmacro

.macro ROUTINE name
	_SYMBOL_HELPER name, TYPE_ROUTINE
.endmacro

.macro LABEL name
	_SYMBOL_HELPER name, TYPE_LABEL
.endmacro

.macro IMPORT_ROUTINE name
	_IMPORT_SYMBOL_HELPER name, TYPE_ROUTINE
.endmacro

.macro IMPORT_LABEL name
	_IMPORT_SYMBOL_HELPER name, TYPE_LABEL
.endmacro

.macro BREAKPOINT
	.global BREAKPOINT_
	BREAKPOINT_:
.endmacro


.endif ; ::_IMPORT_EXPORT_H_

; vim: ft=asm:

