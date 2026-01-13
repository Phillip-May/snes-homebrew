; Assembly function stub for LLVM-MOS
; This file can be edited to add custom assembly functions
; 
; Calling convention notes for llvm-mos:
; - Parameters are typically passed on the stack
; - Stack grows downward (high to low addresses)
; - Return address is pushed first (low byte, then high byte)
; - Parameters follow the return address
; - Return values: 8-bit in A, 16-bit in A (low) + X (high)
; 
; Note: 65816 CPU features are enabled via -mcpu=mosw65816 compiler flag in Makefile

.section .text,"ax",@progbits

; Initialize 65816 to native mode and set all registers to 8-bit mode
; C declaration: void asm_init65816(void);
.global asm_init65816
asm_init65816:
    clc             ; Clear carry flag
    xce             ; Exchange carry and emulation bit (switch to native mode)
    sep #$30        ; Set accumulator (M flag) and index registers (X flag) to 8-bit mode
    rts

; Set data bank register (DBR) to $00
; C declaration: void asm_setDB00(void);
.global asm_setDB00
asm_setDB00:
    lda #$00        ; Load $00 into accumulator
    pha             ; Push accumulator (1 byte) onto stack
    plb             ; Pull into data bank register (sets DBR to $00)
    rts

; Set data bank register (DBR) to $7E
; C declaration: void asm_setDB7E(void);
.global asm_setDB7E
asm_setDB7E:
    lda #$7E        ; Load $7E into accumulator
    pha             ; Push accumulator (1 byte) onto stack
    plb             ; Pull into data bank register (sets DBR to $7E)
    rts
