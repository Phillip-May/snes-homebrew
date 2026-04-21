; SNES far-call dispatch stubs for LLVM-MOS / 65816.
; Allows bank-0 C code to execute selected banked functions.

.section rom_fixed,"ax",@progbits

.global snes_prg_bank_switch_pc

; C-callable from mirrored fixed code. JML changes the 65816 program bank,
; then RTS returns to the caller's mirrored fixed address in the target bank.
snes_prg_bank_switch_pc:
    cmp #$01
    beq snes_prg_bank_switch_pc_bank1
    cmp #$02
    beq snes_prg_bank_switch_pc_bank2
    cmp #$03
    beq snes_prg_bank_switch_pc_bank3
    cmp #$04
    beq snes_prg_bank_switch_pc_bank4
    cmp #$05
    beq snes_prg_bank_switch_pc_bank5
    cmp #$06
    beq snes_prg_bank_switch_pc_bank6
    cmp #$07
    beq snes_prg_bank_switch_pc_bank7
snes_prg_bank_switch_pc_bank0:
    .byte 0x5C
    .word snes_prg_bank_switch_pc_return
    .byte 0x00
snes_prg_bank_switch_pc_bank1:
    .byte 0x5C
    .word snes_prg_bank_switch_pc_return
    .byte 0x01
snes_prg_bank_switch_pc_bank2:
    .byte 0x5C
    .word snes_prg_bank_switch_pc_return
    .byte 0x02
snes_prg_bank_switch_pc_bank3:
    .byte 0x5C
    .word snes_prg_bank_switch_pc_return
    .byte 0x03
snes_prg_bank_switch_pc_bank4:
    .byte 0x5C
    .word snes_prg_bank_switch_pc_return
    .byte 0x04
snes_prg_bank_switch_pc_bank5:
    .byte 0x5C
    .word snes_prg_bank_switch_pc_return
    .byte 0x05
snes_prg_bank_switch_pc_bank6:
    .byte 0x5C
    .word snes_prg_bank_switch_pc_return
    .byte 0x06
snes_prg_bank_switch_pc_bank7:
    .byte 0x5C
    .word snes_prg_bank_switch_pc_return
    .byte 0x07
snes_prg_bank_switch_pc_return:
    rts
