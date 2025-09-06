.segment "HEADER"
.import _main
.import zerobss

; --- Game title (21 bytes, padded with spaces) ---
.asciiz "SNESDEMO"
.res 21 - 9, $20

; --- Header values ---
.byte $20    ; Map mode (LoROM, fast ROM)
.byte $00    ; Cartridge type (ROM only)
.byte $09    ; ROM size (2^9 = 512 KB, adjust as needed)
.byte $00    ; SRAM size (none)
.byte $01    ; Country (NTSC)
.byte $33    ; Licensee (extended)
.byte $00    ; Version

.word $0000  ; Complement check (fix later with checksum tool)
.word $0000  ; Checksum

; --- Interrupt vectors (65816 native mode) ---
.word $0000  ; Unused
.word $0000  ; Unused
.word $0000  ; COP (co-processor)
.word $0000  ; BRK (break)
.word $0000  ; ABORT
.word $0000  ; NMI (non-maskable interrupt)
.word $0000  ; RESET (not used - CPU starts in 6502 mode)
.word $0000  ; IRQ (interrupt request)

; --- Interrupt vectors (6502 emulation mode) ---
.word $0000  ; Unused
.word $0000  ; Unused
.word $0000  ; COP (co-processor)
.word $0000  ; BRK (break)
.word $0000  ; ABORT
.word $0000  ; NMI (non-maskable interrupt)
.word custom_start  ; RESET (start vector) - points to custom startup routine
.word $0000  ; IRQ (interrupt request)

; Custom startup routine that bypasses the problematic initlib
.segment "STARTUP"
custom_start:   
    ; Note: DATA segment initialization is handled automatically by cc65
    ; The DATA segment is already in RAM at the correct address
    ; Clear BSS (uninitialized data)
    JSR zerobss
    
    ; Jump to main function
    JMP _main

