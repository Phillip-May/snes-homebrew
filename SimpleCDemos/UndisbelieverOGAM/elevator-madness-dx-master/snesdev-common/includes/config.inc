;; config.inc
;;
;; Handles configurable variables that can be overridden with a project's
;; `config.h` file.
;;

.ifndef ::_CONFIG_INC_
::_CONFIG_INC_ = 1

.include "config.h"

;; Creates a .define called FPS with a value of either 60 or 50
;; depending on the state of the REGION flag

.if .xmatch(REGION, NTSC)
	.define FPS 60
.elseif .xmatch(REGION, PAL)
	.define FPS 50
.else
	.error "Unkown Region"
.endif

;; Handles a configurable variable.
;;
;; If the symbol is not already defined, then it will be created by the `default`
;; value. If the `default` value does not exist then it will error out with an
;; error message.
;;
;; PARAM: name - the name of the .define
;;        default - can be blank
;;
;; NOTE: Due to the way ca65 tokenizes this macro cannot test if define exists,
;;       it tests by type instead. Thus default must not be a label.
.macro CONFIG name, default
	.ifndef name
		.ifblank default
			.error .sprintf("Require %s config variable", .string(name))
		.else
			name = default
		.endif
	.endif
.endmacro

;; Handles a configurable .define.
;;
;; If the system detects that the .define does not exist (ie, it resolve to something
;; that is a symbol) it will either:
;;	* Creates a .define macro with a value of `default` (if it doesn't exist)
;;	* Error out, asking for the .define name.
;;
;; NOTE: Due to the way ca65 tokenizes this macro cannot test if define exists,
;;       it tests by type instead. Thus default must not be a label.
;;
;; PARAM: name - the name of the .define
;;        default (MUST not be a label) - can be blank
.macro CONFIG_DEFINE name, default
	.if .match(name, __UNKOWN_LABEL__)
		.ifblank default
			.error .sprintf("Require %s config .define ", .string(name))
		.else
			.define name default
		.endif
	.endif
.endmacro

.endif

; vim: ft=asm:

