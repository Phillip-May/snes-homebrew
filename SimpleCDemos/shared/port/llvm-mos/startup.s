; SNES reset entry for LLVM-MOS
;
; The "common" llvm-mos platform's crt0 _start does NOT zero .bss or copy
; .data -- it just calls main. Globals therefore start as whatever is in RAM,
; which is garbage on hardware / randomised-RAM emulators (e.g. an
; uninitialised animation counter giving a different palette every boot).
;
; This reset entry zeroes the low RAM holding .data/.bss/.noinit, then calls
; main. The linker (linker.ld) makes it the ENTRY and points the reset vector
; here. It must live in rom_bank_0_fixed so its link address equals its
; physical ROM address (the 16-bit reset vector can only reach that region).
; Code runs in 6502 emulation mode at reset, so the clear is an 8-bit loop.

.section .text.startup,"ax",@progbits

.global _reset
.extern main

_reset:
    sei
    ldx #$ff
    txs

    ; Zero $0200-$1FFF (the .data/.bss/.noinit region and scratch RAM).
    lda #$00
    sta $00            ; pointer low  (llvm-mos imaginary reg, reset before use)
    ldx #$02
    stx $01            ; pointer high -> start at page $02
    ldy #$00
clear_loop:
    sta ($00),y
    iny
    bne clear_loop
    inc $01
    ldx $01
    cpx #$20           ; stop once we pass page $1F ($2000)
    bne clear_loop

    jsr main
halt:
    jmp halt
