// NES tilemap data for layer 'level12'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL12_NES_H
#define TILEMAP_LEVEL12_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL12_WIDTH 16
#define TILEMAP_LEVEL12_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level12'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level12_compressed[] = {
    0xc2, 0x03, 0xf1, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x30, 0xd4, 0xe4, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0x00, 0x30, 0xc2, 0xe4, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x30, 0xc3, 0x06, 0x1c, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0x31, 0x00, 0xdc, 0xc1, 0xc0, 0x29, 0xc0, 0x29, 0xc0, 0x00, 0x31, 0x32, 0x32,
    0xc8, 0xd2, 0x1c, 0x00, 0x2a, 0xc0, 0x2a, 0x31, 0x33, 0x33, 0x32, 0x34, 0x34, 0x03, 0xee, 0xc1,
    0x12, 0xe5, 0xdc, 0x30, 0x32, 0x32, 0x35, 0xc0, 0x16, 0xcb, 0xe1, 0xd7, 0xc2, 0x16, 0x36, 0x34,
    0x35, 0xc0, 0x00, 0xe4, 0xc4, 0xcb, 0xfe, 0xc2, 0x01, 0xc0, 0xc0, 0x00, 0xe4, 0xc0, 0x00, 0x1f,
    0xc2, 0xc2, 0xe4, 0xc0, 0xc0, 0xf1, 0xc0, 0xf0, 0xdf, 0xc2, 0xf1, 0xc0, 0xc0, 0xc0, 0xc0, 0x31,
    0x33, 0x37, 0xcc, 0xd8, 0xd5, 0xc0, 0xc0, 0xc0, 0x00, 0x30, 0x32, 0x38, 0xfe, 0xd8, 0xc7, 0xc0,
    0xc0, 0x0f, 0xc0, 0x30, 0x32, 0x38, 0xcc, 0xd8, 0x0b, 0x1c, 0x11, 0xd5, 0x00, 0xe5, 0x01, 0x30,
    0x32, 0x38, 0xcc, 0xee, 0xc1, 0x0c, 0xc4, 0xf4, 0xc8, 0x30, 0x32, 0x38, 0xe9, 0xc1, 0xc1, 0xc1,
    0x12
};

#define TILEMAP_LEVEL12_COMPRESSED_SIZE 161
#define TILEMAP_LEVEL12_COUNT 256

// Background palette data for layer 'level12' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level12[4][4] = {
    // Background Palette 0
    { 0x0d, 0x37, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x37, 0x1b, 0x1a },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL12_COUNT 2

// Sprite palette data for layer 'level12' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level12[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x15, 0x1b, 0x36 },
    // Sprite Palette 1
    { 0x0d, 0x31, 0x28, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x15, 0x18, 0x37 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x36, 0x1b }
};

#define PALETTE_SPRITE_LEVEL12_COUNT 4

// Object data for layer 'level12'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level12[] = {
    70, 7, 4,
    71, 8, 4,
    86, 7, 5,
    87, 8, 5,
    28, 12, 9,
};

#define OBJECT_LEVEL12_COUNT 5

// Object sprite data for layer 'level12'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level12[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 5, 6, 7, 8,
    3, 3, 9, 10, 11, 12,
    4, 3, 1, 2, 13, 14,
    5, 3, 15, 16, 17, 18,
    6, 3, 19, 20, 21, 22,
    7, 3, 23, 24, 25, 26,
    9, 2, 31, 32, 33, 30,
    10, 2, 0, 30, 0, 30,
    11, 1, 34, 35, 0, 0,
    12, 1, 36, 37, 0, 0,
    13, 1, 38, 39, 40, 41,
    14, 1, 42, 43, 44, 45,
    15, 1, 46, 47, 48, 49,
    23, 1, 72, 73, 74, 75,
    28, 0, 90, 91, 92, 93,
    29, 2, 94, 95, 96, 97,
    30, 2, 98, 99, 100, 101,
    31, 2, 102, 103, 104, 105,
    45, 1, 134, 135, 136, 137,
    46, 1, 0, 0, 138, 139,
    47, 1, 0, 0, 140, 141,
    64, 1, 110, 183, 184, 113,
    70, 1, 0, 0, 200, 201,
    71, 1, 0, 0, 202, 203,
    86, 1, 221, 222, 223, 125,
    87, 1, 224, 225, 125, 226,
};

#define OBJECT_SPRITE_LEVEL12_COUNT 27

// Object palette data for layer 'level12' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level12[3][4] = {
    // Object Palette 0
    { 0x0d, 0x15, 0x1b, 0x36 },
    // Object Palette 1
    { 0x0d, 0x31, 0x28, 0x00 },
    // Object Palette 2
    { 0x0d, 0x15, 0x18, 0x37 }
};

#define PALETTE_OBJECT_LEVEL12_COUNT 3

// Player start location for layer 'level12'
#define SPAWN_X_LEVEL12 2
#define SPAWN_Y_LEVEL12 13

#endif // TILEMAP_LEVEL12_NES_H
