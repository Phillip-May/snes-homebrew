// NES tilemap data for layer 'level2'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL2_NES_H
#define TILEMAP_LEVEL2_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL2_WIDTH 16
#define TILEMAP_LEVEL2_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level2'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level2_compressed[] = {
    0xc8, 0xcc, 0xc8, 0xde, 0xc3, 0xc8, 0xc1, 0xd7, 0xc8, 0xde, 0xdd, 0xc1, 0xc5, 0xe9, 0x0d, 0xe0,
    0xcc, 0x03, 0x01, 0x0a, 0xe0, 0x08, 0xc4, 0xd7, 0xe4, 0x11, 0x0c, 0xfe, 0xc8, 0xc1, 0xc4, 0xc0,
    0x0d, 0x07, 0xf1, 0xc6, 0xcc, 0xea, 0xc4, 0xc0, 0xc0, 0x00, 0x21, 0xc0, 0xc0, 0xcc, 0xdd, 0xc0,
    0xc0, 0xe3, 0x00, 0x23, 0xc0, 0x00, 0x24, 0xe9, 0x00, 0x25, 0xd3, 0xd5, 0x26, 0xc0, 0xc0, 0xc0,
    0x1f, 0xc2, 0x01, 0x0a, 0x0a, 0x0b, 0xc4, 0x21, 0xd5, 0xc0, 0x00, 0x1d, 0xfe, 0xc2, 0xdc, 0xdb,
    0x00, 0x23, 0x09, 0x1d, 0xd5, 0xf3, 0xde, 0xc2, 0xea, 0xc1, 0x1c, 0x0a, 0xc1, 0xcd, 0xc1, 0xe5,
    0xd4, 0xc8, 0xc6, 0x09, 0x26, 0xc4, 0xcb, 0x09, 0x13, 0xdf, 0xc3, 0xc8, 0xd3, 0x09, 0x21, 0xc0,
    0xc0, 0xcb, 0xd7, 0xc5, 0x02, 0x16, 0x11, 0x0c, 0x08, 0x21, 0xc0, 0xc0, 0xc0, 0x0d, 0x02, 0xc6,
    0x0c, 0x23, 0xc9, 0x00, 0x21, 0xca, 0xe3, 0xc0, 0xda, 0xc0, 0xcb, 0xf2, 0x28, 0x07, 0xce, 0x01,
    0xc0, 0xf6, 0x09, 0xce, 0xce, 0xce, 0x15, 0xc2, 0xea, 0x1c, 0x1d, 0xf3, 0x09
};

#define TILEMAP_LEVEL2_COMPRESSED_SIZE 157
#define TILEMAP_LEVEL2_COUNT 256

// Background palette data for layer 'level2' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level2[4][4] = {
    // Background Palette 0
    { 0x0d, 0x37, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x37, 0x00, 0x10 },
    // Background Palette 2
    { 0x0d, 0x37, 0x1b, 0x1a },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL2_COUNT 3

// Sprite palette data for layer 'level2' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level2[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x15, 0x36, 0x1b },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x18, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x31, 0x28, 0x00 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x36, 0x1b }
};

#define PALETTE_SPRITE_LEVEL2_COUNT 4

// Object data for layer 'level2'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level2[] = {};

#define OBJECT_LEVEL2_COUNT 0

// Object sprite data for layer 'level2'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level2[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 5, 6, 7, 8,
    3, 3, 9, 10, 11, 12,
    4, 3, 1, 2, 13, 14,
    5, 3, 15, 16, 17, 18,
    6, 3, 19, 20, 21, 22,
    7, 3, 23, 24, 25, 26,
    9, 1, 31, 32, 33, 30,
    10, 1, 0, 30, 0, 30,
    11, 2, 34, 35, 0, 0,
    12, 2, 36, 37, 0, 0,
    13, 2, 38, 39, 40, 41,
    14, 2, 42, 43, 44, 45,
    15, 2, 46, 47, 48, 49,
    23, 1, 73, 74, 75, 76,
    29, 0, 95, 96, 97, 98,
    30, 0, 99, 100, 101, 102,
    31, 0, 103, 104, 105, 106,
    45, 2, 135, 136, 137, 138,
    46, 2, 0, 0, 139, 140,
    47, 2, 0, 0, 141, 142,
    64, 1, 111, 184, 185, 114,
};

#define OBJECT_SPRITE_LEVEL2_COUNT 22

// Object palette data for layer 'level2' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level2[3][4] = {
    // Object Palette 0
    { 0x0d, 0x15, 0x36, 0x1b },
    // Object Palette 1
    { 0x0d, 0x15, 0x18, 0x0c },
    // Object Palette 2
    { 0x0d, 0x31, 0x28, 0x00 }
};

#define PALETTE_OBJECT_LEVEL2_COUNT 3

// Player start location for layer 'level2'
#define SPAWN_X_LEVEL2 1
#define SPAWN_Y_LEVEL2 14

#endif // TILEMAP_LEVEL2_NES_H
