// CHR data stored in PRG-ROM for CHR-RAM initialization.
// Compiled without LTO so bank section attributes are respected.
#define __NES__

// Game CHR data (8KB) - stored in PRG bank 5
__attribute__((section(".prg_rom_5")))
const unsigned char game_chr_data[8192] = {
#include "../../python/sprite_chr_combined.inc"
};

// Title CHR data (8KB) - stored in PRG bank 3
__attribute__((section(".prg_rom_3")))
const unsigned char title_chr_data[8192] = {
#include "../../python/title_chr.inc"
};
