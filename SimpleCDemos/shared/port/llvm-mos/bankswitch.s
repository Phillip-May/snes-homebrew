; Bank switching functions for LLVM-MOS
; These functions are duplicated across all code banks at link time
; 
; Calling convention notes for llvm-mos:
; - Parameters are typically passed on the stack
; - Stack grows downward (high to low addresses)
; - Return address is pushed first (low byte, then high byte)
; - Parameters follow the return address
; - Return values: 8-bit in A, 16-bit in A (low) + X (high)
; 
; Note: 65816 CPU features are enabled via -mcpu=mosw65816 compiler flag

.section .text,"ax",@progbits

; ========== CPU INITIALIZATION ==========
; Initialize 65816 to native mode and set all registers to 8-bit mode
; C declaration: void asm_init65816(void);
.global asm_init65816
asm_init65816:
    clc             ; Clear carry flag
    xce             ; Exchange carry and emulation bit (switch to native mode)
    sep #$30        ; Set accumulator (M flag) and index registers (X flag) to 8-bit mode
    rts

; ========== CODE BANK SWITCHING ==========
; These functions switch the Program Bank Register (PBR) using JML
; NOTE: These functions are in a fixed section that gets duplicated across all code banks
; NOTE: Each function JMLs to its continuation point in the target bank
; NOTE: The continuation point is at the same offset in all banks (duplicated)
; NOTE: JML format: JML $BBAAAA where BB=bank, AAAA=address
;
; How it works:
; 1. Function is duplicated to all banks at the same offset
; 2. Function JMLs to its continuation point in the target bank (same offset)
; 3. Continuation point just does RTS, returning to caller (now in target bank)

; Continuation point - just returns (duplicated across all banks at same offset)
; This label will be at the same 16-bit address in all banks
asm_switch_continue:
    rts             ; Return to caller (we're now in the target bank)

; 24-bit addresses for JML (constructed at assembly time)
; These store the full 24-bit address: bank byte + 16-bit address
asm_switch_addr_0:
    .word asm_switch_continue  ; Low and high bytes of address
    .byte $00                   ; Bank byte for bank 0

asm_switch_addr_1:
    .word asm_switch_continue  ; Low and high bytes of address
    .byte $01                   ; Bank byte for bank 1

asm_switch_addr_2:
    .word asm_switch_continue  ; Low and high bytes of address
    .byte $02                   ; Bank byte for bank 2

asm_switch_addr_3:
    .word asm_switch_continue  ; Low and high bytes of address
    .byte $03                   ; Bank byte for bank 3

; Switch to code bank 0 (Program Bank Register)
; void asm_switch_to_code_bank_0(void);
; Uses JML to switch PBR to bank 0 and jump to continuation point
.global asm_switch_to_code_bank_0
asm_switch_to_code_bank_0:
    ; JML to continuation point in bank 0 using indirect addressing
    ; JML (addr) uses the 24-bit address stored at addr
    jml (asm_switch_addr_0)

; Switch to code bank 1 (Program Bank Register)
; void asm_switch_to_code_bank_1(void);
.global asm_switch_to_code_bank_1
asm_switch_to_code_bank_1:
    ; JML to continuation point in bank 1
    jml (asm_switch_addr_1)

; Switch to code bank 2 (Program Bank Register)
; void asm_switch_to_code_bank_2(void);
.global asm_switch_to_code_bank_2
asm_switch_to_code_bank_2:
    ; JML to continuation point in bank 2
    jml (asm_switch_addr_2)

; Switch to code bank 3 (Program Bank Register)
; void asm_switch_to_code_bank_3(void);
.global asm_switch_to_code_bank_3
asm_switch_to_code_bank_3:
    ; JML to continuation point in bank 3
    jml (asm_switch_addr_3)

; ========== ROM CONST DATA BANK SWITCHING ==========
; These functions switch the Data Bank Register (DBR) for ROM const data access
; They use PLB which is correct for data bank switching

; Switch to const data bank 0 (Data Bank Register)
; void asm_switch_to_const_bank_0(void);
.global asm_switch_to_const_bank_0
asm_switch_to_const_bank_0:
    pha             ; Save A register
    lda #$00        ; Load bank 0
    pha             ; Push bank to stack
    plb             ; Pull into data bank register (sets DBR to $00)
    pla             ; Restore A register
    rts

; Switch to const data bank 1 (Data Bank Register)
; void asm_switch_to_const_bank_1(void);
.global asm_switch_to_const_bank_1
asm_switch_to_const_bank_1:
    pha             ; Save A register
    lda #$01        ; Load bank 1
    pha             ; Push bank to stack
    plb             ; Pull into data bank register (sets DBR to $01)
    pla             ; Restore A register
    rts

; Switch to const data bank 2 (Data Bank Register)
; void asm_switch_to_const_bank_2(void);
.global asm_switch_to_const_bank_2
asm_switch_to_const_bank_2:
    pha             ; Save A register
    lda #$02        ; Load bank 2
    pha             ; Push bank to stack
    plb             ; Pull into data bank register (sets DBR to $02)
    pla             ; Restore A register
    rts

; Switch to const data bank 3 (Data Bank Register)
; void asm_switch_to_const_bank_3(void);
.global asm_switch_to_const_bank_3
asm_switch_to_const_bank_3:
    pha             ; Save A register
    lda #$03        ; Load bank 3
    pha             ; Push bank to stack
    plb             ; Pull into data bank register (sets DBR to $03)
    pla             ; Restore A register
    rts

; Get current data bank (Data Bank Register) - optional utility
; uint8_t asm_get_data_bank(void);
.global asm_get_data_bank
asm_get_data_bank:
    phb             ; Push current DBR
    pla             ; Pull into A (now contains DBR value)
    rts

; ========== RAM BANKING FUNCTIONS ==========
; These functions switch the Data Bank Register (DBR) for RAM access
; They are already working and will be duplicated across all code banks by the linker

; Set data bank register (DBR) to $00 for RAM access
; void asm_setDB00(void);
.global asm_setDB00
asm_setDB00:
    lda #$00        ; Load $00 into accumulator
    pha             ; Push accumulator (1 byte) onto stack
    plb             ; Pull into data bank register (sets DBR to $00)
    rts

; Set data bank register (DBR) to $7E for WRAM access
; void asm_setDB7E(void);
.global asm_setDB7E
asm_setDB7E:
    lda #$7E        ; Load $7E into accumulator
    pha             ; Push accumulator (1 byte) onto stack
    plb             ; Pull into data bank register (sets DBR to $7E)
    rts
