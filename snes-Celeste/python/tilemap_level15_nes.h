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
    0xc2, 0xe6, 0xee, 0xc7, 0xfa, 0xd1, 0xc2, 0xd1, 0xc2, 0xd8, 0xc1, 0xc4, 0xef, 0xe6, 0xc3, 0xc3,
    0xc2, 0xd8, 0xd2, 0xc0, 0xef, 0xec, 0xc6, 0x0d, 0x0c, 0xc3, 0xee, 0xc1, 0xc0, 0x2d, 0xfd, 0xd0,
    0xc0, 0x09, 0xce, 0xf5, 0x08, 0xf2, 0xc0, 0xc9, 0xc0, 0xc0, 0xfc, 0xc8, 0xc0, 0xcb, 0xd3, 0xc4,
    0xc0, 0xc0, 0x00, 0xd4, 0x16, 0xca, 0x22, 0x13, 0x14, 0x17, 0xca, 0xca, 0xc0, 0xfc, 0xe6, 0xd9,
    0x14, 0xce, 0xce, 0x15, 0x14, 0x17, 0xd5, 0x00, 0xdd, 0xe0, 0x08, 0xef, 0xd4, 0xc8, 0xc1, 0xc7,
    0xc5, 0xc9, 0xc0, 0xef, 0xc2, 0xea, 0xc1, 0xc4, 0xc5, 0xc7, 0xc0, 0x2d, 0xde, 0xdf, 0x16, 0xd2,
    0xff, 0xc5, 0xc4, 0xc0, 0xe8, 0xd6, 0xfd, 0xe1, 0x1c, 0xc0, 0xc7, 0xc0, 0xc0, 0xc0, 0xe8, 0x2b,
    0xcb, 0xc4, 0xc5, 0x1c, 0x1d, 0x27, 0xc0, 0xc0, 0xc0, 0x00, 0xf3, 0xc0, 0xc5, 0xe1, 0xf2, 0xff,
    0x83, 0x18, 0xc1, 0xd5, 0xc5, 0xd2, 0xc1, 0xd5, 0xc0, 0x1d, 0xf3, 0xd2, 0xc0
};

#define TILEMAP_LEVEL15_COMPRESSED_SIZE 141
#define TILEMAP_LEVEL15_COUNT 256

// Background palette data for layer 'level15' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level15[4][4] = {
    // Background Palette 0
    { 0x0d, 0x37, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x37, 0x00, 0x10 },
    // Background Palette 2
    { 0x0d, 0x0c, 0x28, 0x17 },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL15_COUNT 3

// Sprite palette data for layer 'level15' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level15[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x15, 0x36, 0x1b },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x18, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x31, 0x28, 0x00 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x36, 0x1b }
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
    2, 3, 5, 6, 7, 8,
    3, 3, 9, 10, 11, 12,
    4, 3, 1, 2, 13, 14,
    5, 3, 15, 16, 17, 18,
    6, 3, 19, 20, 21, 22,
    7, 3, 23, 24, 25, 26,
    8, 1, 27, 28, 29, 30,
    9, 1, 31, 32, 33, 30,
    10, 1, 0, 30, 0, 30,
    11, 2, 34, 35, 0, 0,
    12, 2, 36, 37, 0, 0,
    13, 2, 38, 39, 40, 41,
    14, 2, 42, 43, 44, 45,
    15, 2, 46, 47, 48, 49,
    18, 2, 56, 57, 58, 59,
    20, 1, 62, 63, 64, 65,
    22, 0, 69, 70, 71, 72,
    23, 1, 73, 74, 75, 76,
    29, 0, 95, 96, 97, 98,
    30, 0, 99, 100, 101, 102,
    31, 0, 103, 104, 105, 106,
    45, 2, 135, 136, 137, 138,
    46, 2, 0, 0, 139, 140,
    47, 2, 0, 0, 141, 142,
    64, 1, 111, 184, 185, 114,
};

#define OBJECT_SPRITE_LEVEL15_COUNT 26

// Object palette data for layer 'level15' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level15[3][4] = {
    // Object Palette 0
    { 0x0d, 0x15, 0x36, 0x1b },
    // Object Palette 1
    { 0x0d, 0x15, 0x18, 0x0c },
    // Object Palette 2
    { 0x0d, 0x31, 0x28, 0x00 }
};

#define PALETTE_OBJECT_LEVEL15_COUNT 3

// Player start location for layer 'level15'
#define SPAWN_X_LEVEL15 6
#define SPAWN_Y_LEVEL15 13

#endif // TILEMAP_LEVEL15_NES_H
