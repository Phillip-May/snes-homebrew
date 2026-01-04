// EXAMPLE: Modified level header file with banking attributes
// This shows how to modify tilemap_level1_nes.h (and other level headers)

// NES tilemap data for layer 'level1'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL1_NES_H
#define TILEMAP_LEVEL1_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL1_WIDTH 16
#define TILEMAP_LEVEL1_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level1'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
// ADD THIS ATTRIBUTE: Place in bank 1 (levels 1-10)
__attribute__((section(".prg_rom_1")))
const unsigned char tilemap_level1_compressed[] = {
    0x01, 0xf9, 0xd2, 0xc2, 0xc3, 0xc3, 0xdf, 0xc0, 0xcc, 0x01, 0xdc, 0xc3, 0x06, 0xc4, 0xc5, 0xc4,
    // ... rest of data ...
};

#define TILEMAP_LEVEL1_COMPRESSED_SIZE 148
#define TILEMAP_LEVEL1_COUNT 256

// Background palette data for layer 'level1' (NES 6-bit format)
// ADD THIS ATTRIBUTE: Place in bank 1
__attribute__((section(".prg_rom_1")))
const unsigned char palette_background_level1[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // ... rest of palettes ...
};

#define PALETTE_BACKGROUND_LEVEL1_COUNT 2

// Sprite palette data for layer 'level1' (NES 6-bit format)
// ADD THIS ATTRIBUTE: Place in bank 1
__attribute__((section(".prg_rom_1")))
const unsigned char palette_sprite_level1[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // ... rest of palettes ...
};

#define PALETTE_SPRITE_LEVEL1_COUNT 4

// Object data for layer 'level1'
// ADD THIS ATTRIBUTE: Place in bank 1
__attribute__((section(".prg_rom_1")))
const unsigned char object_level1[] = {
    64, 1, 4,
};

#define OBJECT_LEVEL1_COUNT 1

#endif // TILEMAP_LEVEL1_NES_H

/*
BANK ASSIGNMENT GUIDE:

For levels 1-10:   Use __attribute__((section(".prg_rom_1")))
For levels 11-20:  Use __attribute__((section(".prg_rom_2")))
For levels 21-31:  Use __attribute__((section(".prg_rom_3")))

You need to add the attribute to ALL arrays in each level header:
- tilemap_levelN_compressed
- palette_background_levelN
- palette_sprite_levelN  
- object_levelN
- object_sprite_levelN (if present)

The level_data array in nes.c should NOT have section attributes (it stays in fixed bank).
*/
