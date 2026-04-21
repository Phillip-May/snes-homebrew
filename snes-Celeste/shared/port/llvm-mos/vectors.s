; SNES Interrupt Vector Handlers for LLVM-MOS (6502 compatible)
; Based on vectors.asm reference from wdc816cc

.section rom_bank_0,"ax",@progbits

; External C function references
.extern snesXC_cop
.extern snesXC_brk  
.extern snesXC_abort
.extern snesXC_irq
.extern GLOBAL_FrameCount
.extern _start

; COP (Co-Processor) interrupt handler
.global COP
COP:
    pha
    txa
    pha
    tya
    pha
    php
    ; Save the full LLVM-MOS imaginary-register window (__rc0..__rc31).
    lda $00
    pha
    lda $01
    pha
    lda $02
    pha
    lda $03
    pha
    lda $04
    pha
    lda $05
    pha
    lda $06
    pha
    lda $07
    pha
    jsr snesXC_cop
    ; Restore zero page registers
    pla
    sta $07
    pla
    sta $06
    pla
    sta $05
    pla
    sta $04
    pla
    sta $03
    pla
    sta $02
    pla
    sta $01
    pla
    sta $00
    plp
    pla
    tay
    pla
    tax
    pla
    rti

; BRK (Break) interrupt handler  
.global BRK
BRK:
    pha
    txa
    pha
    tya
    pha
    php
    ; Save zero page registers that LLVM-MOS might use
    lda $00
    pha
    lda $01
    pha
    lda $02
    pha
    lda $03
    pha
    lda $04
    pha
    lda $05
    pha
    lda $06
    pha
    lda $07
    pha
    jsr snesXC_brk
    ; Restore zero page registers
    pla
    sta $07
    pla
    sta $06
    pla
    sta $05
    pla
    sta $04
    pla
    sta $03
    pla
    sta $02
    pla
    sta $01
    pla
    sta $00
    plp
    pla
    tay
    pla
    tax
    pla
    rti

; ABORT interrupt handler
.global ABORT
ABORT:
    pha
    txa
    pha
    tya
    pha
    php
    ; Save zero page registers that LLVM-MOS might use
    lda $00
    pha
    lda $01
    pha
    lda $02
    pha
    lda $03
    pha
    lda $04
    pha
    lda $05
    pha
    lda $06
    pha
    lda $07
    pha
    jsr snesXC_abort
    ; Restore zero page registers
    pla
    sta $07
    pla
    sta $06
    pla
    sta $05
    pla
    sta $04
    pla
    sta $03
    pla
    sta $02
    pla
    sta $01
    pla
    sta $00
    plp
    pla
    tay
    pla
    tax
    pla
    rti

; NMI (Non-Maskable Interrupt) handler.
; Keep this tiny: acknowledge NMI and bump the global frame counter.
.global NMI
NMI:
    rep #$30            ; save full A/X/Y regardless of caller widths
    pha
    phx
    phy
    sep #$20
    lda $4210           ; Read NMI status to acknowledge
    rep #$20
    lda GLOBAL_FrameCount
    inc a
    sta GLOBAL_FrameCount
    rep #$30
    ply
    plx
    pla
    rti

; IRQ (Interrupt Request) handler
.global IRQ
IRQ:
    pha
    txa
    pha
    tya
    pha
    php
    ; Save zero page registers that LLVM-MOS might use
    lda $00
    pha
    lda $01
    pha
    lda $02
    pha
    lda $03
    pha
    lda $04
    pha
    lda $05
    pha
    lda $06
    pha
    lda $07
    pha
    ; Acknowledge IRQ interrupt
    lda $4211       ; Read IRQ status to acknowledge
    jsr snesXC_irq
    ; Restore zero page registers
    pla
    sta $07
    pla
    sta $06
    pla
    sta $05
    pla
    sta $04
    pla
    sta $03
    pla
    sta $02
    pla
    sta $01
    pla
    sta $00
    plp
    pla
    tay
    pla
    tax
    pla
    rti

; Dummy IRQ handler (for reserved vectors)
.global DIRQ
DIRQ:
    rti
