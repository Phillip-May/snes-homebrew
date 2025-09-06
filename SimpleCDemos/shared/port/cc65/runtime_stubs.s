; Runtime stub definitions for missing cc65 symbols
; This file provides definitions for symbols that cc65 runtime expects

.export __STACKSIZE__
.export __STACKSTART__
.export __CONSTRUCTOR_COUNT__
.export __CONSTRUCTOR_TABLE__
.export __DESTRUCTOR_COUNT__
.export __DESTRUCTOR_TABLE__

.segment "DATA"

; Stack-related symbols
__STACKSIZE__:
    .word $0800
__STACKSTART__:
    .word $0FFF

; Constructor/destructor symbols (empty for SNES)
__CONSTRUCTOR_COUNT__:
    .word $0000
__CONSTRUCTOR_TABLE__:
    .word $0000  ; Empty table - single word
__DESTRUCTOR_COUNT__:
    .word $0000
__DESTRUCTOR_TABLE__:
    .word $0000  ; Empty table - single word
