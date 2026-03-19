; FamiStudio Sound Engine compiled from CA65 source, placed in bank 7
; ZP at $E0-$E7 and BSS at $0740-$07BC are hardcoded in the binary
; No .bss/.zp.bss reservations needed — the engine manages its own memory
; Binary built by: ca65 + ld65 from famistudio_config.s + famistudio_ca65.s
; Symbols extracted from labels file

.section .prg_rom_7,"a"

.globl famistudio_engine_start
famistudio_engine_start:
.incbin "src/assets/famistudio_engine.bin"

; Public API symbols (offsets from binary start at VMA 0x8000)
.globl famistudio_init
.set famistudio_init, famistudio_engine_start + 0x0000

.globl famistudio_music_stop
.set famistudio_music_stop, famistudio_engine_start + 0x0058

.globl famistudio_music_play
.set famistudio_music_play, famistudio_engine_start + 0x00B0

.globl famistudio_music_pause
.set famistudio_music_pause, famistudio_engine_start + 0x011D

.globl famistudio_update
.set famistudio_update, famistudio_engine_start + 0x015B
