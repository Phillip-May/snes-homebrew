; iNES header for NES ROM
; Horizontal mirroring: bit 0 of byte 6 = 0
; Vertical mirroring: bit 0 of byte 6 = 1

.section .header, "a"
.byte 'N', 'E', 'S', $1A   ; iNES file signature
.byte 2                    ; Number of 16 KB PRG-ROM banks (32KB total)
.byte 1                    ; Number of 8 KB CHR-ROM banks
.byte $00                  ; Control byte 1: horizontal mirroring (bit 0 = 0), mapper 0 (NROM)
.byte $00                  ; Control byte 2: mapper number upper nibble (0 for NROM)
.byte $00, $00, $00, $00, $00, $00, $00, $00  ; Reserved bytes, set to 0
