// NES tilemap data for layer 'level28'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL28_NES_H
#define TILEMAP_LEVEL28_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL28_WIDTH 16
#define TILEMAP_LEVEL28_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level28'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level28_compressed[] = {
    0xc3, 0xc3, 0xdf, 0xd8, 0xc7, 0xcf, 0xcc, 0xc2, 0xc4, 0xd3, 0xd7, 0x16, 0x0c, 0xc4, 0xcf, 0x07,
    0xc3, 0x05, 0xd9, 0x14, 0x17, 0xd7, 0xd8, 0xc0, 0xcf, 0x21, 0x09, 0x0d, 0xcb, 0xcd, 0xc1, 0xcc,
    0xdc, 0xd0, 0x2d, 0x23, 0xc4, 0xf7, 0xc9, 0x0b, 0x08, 0xfe, 0xc8, 0xd0, 0xc0, 0xc0, 0xc5, 0xc4,
    0xc0, 0xcc, 0xc8, 0xd0, 0xc0, 0xc0, 0xc5, 0xca, 0xca, 0xcc, 0xea, 0xd0, 0xc0, 0xc0, 0xc5, 0x13,
    0xd9, 0x14, 0xc3, 0xc8, 0xd0, 0xe2, 0xc0, 0x00, 0x08, 0xd6, 0xd6, 0x2b, 0xef, 0xec, 0xcf, 0x26,
    0xd0, 0xc0, 0xc0, 0xc0, 0xcf, 0xe9, 0x2c, 0x18, 0xf9, 0xd0, 0xc0, 0xc0, 0xc0, 0xcf, 0xe9, 0xca,
    0x22, 0x21, 0xd0, 0xe2, 0xca, 0x22, 0xd5, 0x22, 0xcc, 0xce, 0x15, 0x06, 0xd0, 0x00, 0xce, 0xdc,
    0xcd, 0x13, 0xc3, 0xc3, 0x06, 0xd0, 0xc0, 0xc2, 0xc8, 0xc5, 0xc7, 0x11, 0xe0, 0xe7, 0xc0, 0x11,
    0xd1, 0xc8, 0xc6, 0xc1, 0xcd, 0xc4, 0xc0, 0xc5, 0xc2, 0xc8, 0x18, 0x18, 0xcd, 0xc7, 0xc0, 0xc0,
    0xda
};

#define TILEMAP_LEVEL28_COMPRESSED_SIZE 145
#define TILEMAP_LEVEL28_COUNT 256

// Background palette data for layer 'level28' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level28[4][4] = {
    // Background Palette 0
    { 0x0d, 0x37, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x37, 0x00, 0x10 },
    // Background Palette 2
    { 0x0d, 0x0c, 0x28, 0x17 },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL28_COUNT 3

// Sprite palette data for layer 'level28' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level28[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x15, 0x28, 0x1b },
    // Sprite Palette 1
    { 0x0d, 0x37, 0x15, 0x10 },
    // Sprite Palette 2
    { 0x0d, 0x28, 0x27, 0x17 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL28_COUNT 4

// Object data for layer 'level28'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level28[] = {
    20, 8, 2,
    8, 14, 3,
    23, 10, 9,
    22, 13, 13,
    23, 4, 15,
    23, 5, 15,
};

#define OBJECT_LEVEL28_COUNT 6

// Object sprite data for layer 'level28'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level28[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    8, 2, 23, 24, 25, 26,
    9, 2, 27, 28, 29, 26,
    10, 0, 0, 30, 0, 30,
    11, 1, 31, 32, 0, 0,
    12, 1, 33, 34, 0, 0,
    13, 1, 35, 36, 37, 38,
    14, 1, 0, 39, 40, 0,
    15, 1, 40, 0, 0, 39,
    20, 0, 51, 52, 53, 54,
    22, 1, 57, 58, 59, 60,
    23, 1, 61, 62, 63, 64,
    29, 1, 83, 84, 85, 86,
    30, 1, 87, 88, 89, 90,
    31, 1, 91, 92, 93, 94,
    45, 1, 123, 124, 125, 126,
    46, 1, 0, 0, 127, 128,
    47, 1, 0, 0, 129, 16,
    64, 1, 99, 170, 171, 102,
};

#define OBJECT_SPRITE_LEVEL28_COUNT 25

// Object palette data for layer 'level28' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level28[3][4] = {
    // Object Palette 0
    { 0x0d, 0x15, 0x28, 0x1b },
    // Object Palette 1
    { 0x0d, 0x37, 0x15, 0x10 },
    // Object Palette 2
    { 0x0d, 0x28, 0x27, 0x17 }
};

#define PALETTE_OBJECT_LEVEL28_COUNT 3

// Player start location for layer 'level28'
#define SPAWN_X_LEVEL28 1
#define SPAWN_Y_LEVEL28 1

#endif // TILEMAP_LEVEL28_NES_H
