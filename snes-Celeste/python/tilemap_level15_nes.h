// NES tilemap data for layer 'level15'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL15_NES_H
#define TILEMAP_LEVEL15_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL15_WIDTH 16
#define TILEMAP_LEVEL15_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level15'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level15_compressed[] = {
    0xc2, 0xe7, 0xee, 0xc7, 0xfa, 0xd2, 0xc2, 0xd2, 0xc2, 0xda, 0xc1, 0xc4, 0xf0, 0xe7, 0xc3, 0xc3,
    0xc2, 0xda, 0xd3, 0xc0, 0xf0, 0xed, 0xc6, 0x0d, 0x0c, 0xc3, 0xee, 0xc1, 0xc0, 0x2a, 0xfe, 0xd0,
    0xc0, 0x09, 0xcf, 0xf7, 0x08, 0xf3, 0xc0, 0xca, 0xc0, 0xc0, 0xfc, 0xc9, 0xc0, 0xcb, 0xd1, 0xc4,
    0xc0, 0xc0, 0x00, 0xd5, 0x16, 0xc8, 0x20, 0x13, 0x14, 0x17, 0xc8, 0xc8, 0xc0, 0xfc, 0xe7, 0xdb,
    0x14, 0xcf, 0xcf, 0x15, 0x14, 0x17, 0xd4, 0x00, 0xdf, 0xe2, 0x08, 0xf0, 0xd5, 0xc9, 0xc1, 0xc7,
    0xc5, 0xca, 0xc0, 0xf0, 0xc2, 0xec, 0xc1, 0xc4, 0xc5, 0xc7, 0xc0, 0x2a, 0xdc, 0xe3, 0x16, 0xd3,
    0x00, 0xf8, 0xca, 0xc0, 0xc0, 0xd6, 0x28, 0xfe, 0xe1, 0x1c, 0xc0, 0xc7, 0xc0, 0xc0, 0xc0, 0xe6,
    0x28, 0xcb, 0xc4, 0xc5, 0x1c, 0x1d, 0x24, 0xc0, 0xc0, 0xc0, 0x00, 0xf4, 0xc0, 0xc5, 0xe1, 0xf3,
    0x00, 0x84, 0x18, 0xc1, 0xd4, 0xc5, 0xd3, 0xc1, 0xd4, 0xc0, 0x1d, 0xf4, 0xd3, 0xc0
};

#define TILEMAP_LEVEL15_COMPRESSED_SIZE 142
#define TILEMAP_LEVEL15_COUNT 256

// Background palette data for layer 'level15' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level15[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0c, 0x27, 0x17 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL15_COUNT 2

// Sprite palette data for layer 'level15' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level15[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL15_COUNT 4

// Object data for layer 'level15'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level15[] = {
    20, 7, 5,
    8, 3, 9,
    18, 14, 9,
    23, 14, 10,
    22, 2, 11,
    23, 6, 14,
    23, 7, 14,
    23, 8, 14,
    23, 9, 14,
    23, 10, 14,
};

#define OBJECT_LEVEL15_COUNT 10

// Object sprite data for layer 'level15'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level15[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    8, 0, 23, 24, 25, 26,
    9, 0, 27, 28, 29, 26,
    10, 0, 0, 26, 0, 26,
    11, 0, 30, 31, 0, 0,
    12, 0, 32, 33, 0, 0,
    13, 2, 34, 35, 36, 37,
    14, 2, 0, 38, 39, 0,
    15, 2, 39, 0, 0, 38,
    18, 0, 46, 47, 48, 49,
    20, 0, 50, 51, 52, 53,
    22, 0, 56, 57, 58, 59,
    23, 1, 60, 61, 62, 63,
    29, 0, 81, 82, 83, 84,
    30, 0, 85, 86, 87, 88,
    31, 0, 89, 90, 91, 92,
    45, 0, 121, 122, 123, 124,
    46, 0, 0, 0, 123, 125,
    47, 0, 0, 0, 126, 16,
    64, 1, 97, 167, 168, 100,
};

#define OBJECT_SPRITE_LEVEL15_COUNT 26

// Object palette data for layer 'level15' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level15[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL15_COUNT 3

// Player start location for layer 'level15'
#define SPAWN_X_LEVEL15 6
#define SPAWN_Y_LEVEL15 13

#endif // TILEMAP_LEVEL15_NES_H
