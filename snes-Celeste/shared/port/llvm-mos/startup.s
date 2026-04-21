; SNES early startup for LLVM-MOS.
; Runs before the CRT init chain so interrupts can safely return from banked
; gameplay code.

.section .init.050,"ax",@progbits

; Reset starts the 65816 in emulation mode. In emulation mode an interrupt
; does not preserve the program bank, so an NMI taken during banked gameplay
; returns to bank 0 and hangs. Switch to native mode before the CRT runs.
__snes_native_init:
    sei
    clc
    xce
    cld
    rep #$30
    .byte 0xA9, 0xFF, 0x1F   ; lda #$1FFF
    tcs
    .byte 0xA9, 0x00, 0x00   ; lda #$0000
    tcd
    phk
    plb
    sep #$30
