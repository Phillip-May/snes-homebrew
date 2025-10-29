; SNES Startup Code for LLVM-MOS
; Simple startup that clears RAM and calls main

.section .text.startup,"ax",@progbits

.global _start
.extern main

_start:
    ; Disable interrupts during initialization
    sei
    
    ; Set up stack pointer to end of RAM
    ldx #$ff
    txs
    
    ; Clear all RAM from $0200 to $1FFF
    lda #$00
    ldx #$00
    
clear_ram_loop:
    sta $0200,x
    sta $0300,x
    sta $0400,x
    sta $0500,x
    sta $0600,x
    sta $0700,x
    sta $0800,x
    sta $0900,x
    sta $0A00,x
    sta $0B00,x
    sta $0C00,x
    sta $0D00,x
    sta $0E00,x
    sta $0F00,x
    sta $1000,x
    sta $1100,x
    sta $1200,x
    sta $1300,x
    sta $1400,x
    sta $1500,x
    sta $1600,x
    sta $1700,x
    sta $1800,x
    sta $1900,x
    sta $1A00,x
    sta $1B00,x
    sta $1C00,x
    sta $1D00,x
    sta $1E00,x
    sta $1F00,x
    inx
    bne clear_ram_loop
    
    ; Call main function
    jsr main
    
    ; If main returns, halt
halt:
    jmp halt
