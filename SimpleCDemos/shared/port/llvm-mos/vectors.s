; SNES Interrupt Vector Handlers for LLVM-MOS (6502 compatible)
; Based on vectors.asm reference from wdc816cc

.section .text.vectors,"ax",@progbits

; External C function references
.extern snesXC_cop
.extern snesXC_brk  
.extern snesXC_abort
.extern snesXC_nmi
.extern snesXC_irq
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
    ; Save current Program Bank Register (PBR) - interrupts can occur in any bank
    ; NOTE: PBR restoration is complex (requires JML) and not fully implemented here
    ; Interrupt handlers must be in bank 0, and interrupts should ideally occur when in bank 0
    phk             ; Push current PBR to stack (saved for potential future use)
    ; Call user handler (must be in bank 0)
    jsr snesXC_cop
    ; NOTE: PBR is not restored here - restoration would require JML which is incompatible with normal return
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
    ; NOTE: PBR was saved with PHK but cannot be easily restored here
    ; (would require JML which is a jump, incompatible with normal return)
    ; Pull the saved PBR value from stack (discard it for now)
    pla             ; Discard saved PBR value
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
    ; Save current Program Bank Register (PBR) - interrupts can occur in any bank
    ; NOTE: PBR restoration is complex (requires JML) and not fully implemented here
    ; Interrupt handlers must be in bank 0, and interrupts should ideally occur when in bank 0
    phk             ; Push current PBR to stack (saved for potential future use)
    ; Call user handler (must be in bank 0)
    jsr snesXC_brk
    ; NOTE: PBR is not restored here - restoration would require JML which is incompatible with normal return
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
    ; NOTE: PBR was saved with PHK but cannot be easily restored here
    ; (would require JML which is a jump, incompatible with normal return)
    ; Pull the saved PBR value from stack (discard it for now)
    pla             ; Discard saved PBR value
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
    ; Save current Program Bank Register (PBR) - interrupts can occur in any bank
    ; NOTE: PBR restoration is complex (requires JML) and not fully implemented here
    ; Interrupt handlers must be in bank 0, and interrupts should ideally occur when in bank 0
    phk             ; Push current PBR to stack (saved for potential future use)
    ; Call user handler (must be in bank 0)
    jsr snesXC_abort
    ; NOTE: PBR is not restored here - restoration would require JML which is incompatible with normal return
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
    ; NOTE: PBR was saved with PHK but cannot be easily restored here
    ; (would require JML which is a jump, incompatible with normal return)
    ; Pull the saved PBR value from stack (discard it for now)
    pla             ; Discard saved PBR value
    plp
    pla
    tay
    pla
    tax
    pla
    rti

; NMI (Non-Maskable Interrupt) handler
.global NMI
NMI:
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
    ; Save current Program Bank Register (PBR) - interrupts can occur in any bank
    ; NOTE: PBR restoration is complex (requires JML) and not fully implemented here
    ; Interrupt handlers must be in bank 0, and interrupts should ideally occur when in bank 0
    phk             ; Push current PBR to stack (saved for potential future use)
    ; Acknowledge NMI interrupt
    lda $4210       ; Read NMI status to acknowledge
    ; Call user handler (must be in bank 0)
    jsr snesXC_nmi
    ; NOTE: PBR is not restored here - restoration would require JML which is incompatible with normal return
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
    ; Save current Program Bank Register (PBR) - interrupts can occur in any bank
    ; NOTE: PBR restoration is complex (requires JML) and not fully implemented here
    ; Interrupt handlers must be in bank 0, and interrupts should ideally occur when in bank 0
    phk             ; Push current PBR to stack (saved for potential future use)
    ; Acknowledge IRQ interrupt
    lda $4211       ; Read IRQ status to acknowledge
    ; Call user handler (must be in bank 0)
    jsr snesXC_irq
    ; NOTE: PBR is not restored here - restoration would require JML which is incompatible with normal return
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
    ; NOTE: PBR was saved with PHK but cannot be easily restored here
    ; (would require JML which is a jump, incompatible with normal return)
    ; Pull the saved PBR value from stack (discard it for now)
    pla             ; Discard saved PBR value
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
