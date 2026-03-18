// NES tilemap data for layer 'level26'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL26_NES_H
#define TILEMAP_LEVEL26_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL26_WIDTH 16
#define TILEMAP_LEVEL26_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level26'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level26_compressed[] = {
    0xd4, 0xc3, 0xc3, 0xc3, 0xc2, 0xc3, 0xdd, 0xc1, 0xdd, 0xf3, 0xc0, 0xc5, 0x07, 0x06, 0x2c, 0xcb,
    0xe0, 0xd2, 0xcd, 0xc4, 0xe2, 0x0d, 0x23, 0xd0, 0xda, 0x26, 0xc4, 0xe1, 0xe7, 0xcf, 0x26, 0x00,
    0xc4, 0xc0, 0x0c, 0x21, 0xc0, 0xc9, 0xc0, 0xcf, 0x21, 0xc0, 0xc0, 0xda, 0x21, 0xc0, 0x0c, 0xc0,
    0xc0, 0xf9, 0xc0, 0xc0, 0xc6, 0x21, 0x24, 0xc5, 0x12, 0x1d, 0x27, 0x12, 0xf9, 0xca, 0xca, 0xca,
    0x07, 0xce, 0xc1, 0xd2, 0x13, 0x14, 0x05, 0xd9, 0xd9, 0xd9, 0xc2, 0x04, 0xc4, 0xe7, 0x00, 0xe0,
    0xc1, 0xc9, 0xcf, 0xcc, 0x03, 0xc0, 0xc0, 0xc0, 0xcb, 0xd2, 0xf3, 0x1f, 0xc2, 0x03, 0xca, 0xca,
    0xe3, 0xc5, 0xc4, 0x08, 0xd7, 0xd1, 0xfb, 0xce, 0xdc, 0xca, 0x0a, 0xd0, 0xc6, 0xde, 0xc2, 0xc3,
    0xc3, 0x05, 0x14, 0x17, 0xed, 0xc0, 0xe8, 0x2b, 0xfa, 0xd5, 0x25, 0xc0, 0xcb, 0xc0, 0xc0, 0xc0,
    0xc5, 0x07, 0xce, 0x01, 0xd5, 0xe0, 0xd5, 0xc0, 0xc0, 0x11, 0x0b, 0xfa, 0xc8, 0x09, 0xda, 0xd2,
    0xc0, 0xc0, 0xf7, 0xc1
};

#define TILEMAP_LEVEL26_COMPRESSED_SIZE 148
#define TILEMAP_LEVEL26_COUNT 256

// Background palette data for layer 'level26' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level26[4][4] = {
    // Background Palette 0
    { 0x0d, 0x37, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x37, 0x00, 0x10 },
    // Background Palette 2
    { 0x0d, 0x37, 0x1b, 0x1a },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL26_COUNT 3

// Sprite palette data for layer 'level26' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level26[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x15, 0x28, 0x1b },
    // Sprite Palette 1
    { 0x0d, 0x37, 0x15, 0x10 },
    // Sprite Palette 2
    { 0x0d, 0x28, 0x27, 0x17 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL26_COUNT 4

// Object data for layer 'level26'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level26[] = {
    20, 15, 5,
    8, 12, 14,
};

#define OBJECT_LEVEL26_COUNT 2

// Object sprite data for layer 'level26'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level26[] = {
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
    23, 1, 61, 62, 63, 64,
    29, 1, 83, 84, 85, 86,
    30, 1, 87, 88, 89, 90,
    31, 1, 91, 92, 93, 94,
    45, 1, 123, 124, 125, 126,
    46, 1, 0, 0, 127, 128,
    47, 1, 0, 0, 129, 16,
    64, 1, 99, 170, 171, 102,
};

#define OBJECT_SPRITE_LEVEL26_COUNT 24

// Object palette data for layer 'level26' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level26[3][4] = {
    // Object Palette 0
    { 0x0d, 0x15, 0x28, 0x1b },
    // Object Palette 1
    { 0x0d, 0x37, 0x15, 0x10 },
    // Object Palette 2
    { 0x0d, 0x28, 0x27, 0x17 }
};

#define PALETTE_OBJECT_LEVEL26_COUNT 3

// Player start location for layer 'level26'
#define SPAWN_X_LEVEL26 1
#define SPAWN_Y_LEVEL26 13

#endif // TILEMAP_LEVEL26_NES_H
