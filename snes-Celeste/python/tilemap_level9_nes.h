// NES tilemap data for layer 'level9'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL9_NES_H
#define TILEMAP_LEVEL9_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL9_WIDTH 16
#define TILEMAP_LEVEL9_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level9'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level9_compressed[] = {
    0xe6, 0xf1, 0xc0, 0x07, 0xc3, 0xc3, 0xc3, 0xdf, 0xc8, 0xc1, 0xc0, 0x00, 0x21, 0x10, 0xcb, 0xc1,
    0xc1, 0xcc, 0xee, 0xd5, 0xc0, 0x23, 0xc0, 0x0d, 0xd2, 0xc6, 0xcc, 0xc1, 0x0b, 0xc4, 0xc0, 0xc0,
    0xc5, 0xdb, 0xf4, 0x04, 0xc1, 0xc7, 0xc0, 0xc0, 0xc6, 0xc1, 0xc7, 0xcc, 0xcd, 0xc7, 0xc0, 0xc0,
    0xc0, 0xda, 0xc4, 0xcc, 0xc1, 0xf2, 0x1d, 0xc0, 0x00, 0x1d, 0xf3, 0x0c, 0xc0, 0xde, 0xc1, 0xc1,
    0x0b, 0xc4, 0xc0, 0xcb, 0xf2, 0xc0, 0xc1, 0xc4, 0xc6, 0xc0, 0xc0, 0xf6, 0x0d, 0xc4, 0xda, 0x09,
    0x11, 0xc0, 0xc0, 0xc0, 0xc0, 0xc7, 0xc0, 0xc1, 0x0c, 0xc4, 0xc0, 0xc0, 0xc0, 0x11, 0x0b, 0xc4,
    0xc5, 0x0c, 0xc4, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0xc0, 0xc9, 0xc0, 0xc0, 0xc0, 0xca, 0xe3, 0xc0,
    0xc0, 0x0d, 0x11, 0xca, 0x0a, 0xca, 0x22, 0xe5, 0x01, 0xc0, 0xc0, 0xc0, 0x0d, 0xce, 0xce, 0xdc,
    0xfe, 0x16, 0xca, 0x22, 0x0a, 0x0a, 0xca, 0xd1, 0xc2, 0xc8, 0xcc, 0x16, 0xe5, 0xce, 0xce, 0x15
};

#define TILEMAP_LEVEL9_COMPRESSED_SIZE 144
#define TILEMAP_LEVEL9_COUNT 256

// Background palette data for layer 'level9' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level9[4][4] = {
    // Background Palette 0
    { 0x0d, 0x37, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x37, 0x00, 0x10 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL9_COUNT 2

// Sprite palette data for layer 'level9' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level9[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x15, 0x36, 0x1b },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x18, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x31, 0x28, 0x00 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x36, 0x1b }
};

#define PALETTE_SPRITE_LEVEL9_COUNT 4

// Object data for layer 'level9'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level9[] = {
    64, 7, 1,
    22, 4, 4,
    22, 8, 4,
    18, 12, 13,
    18, 13, 13,
};

#define OBJECT_LEVEL9_COUNT 5

// Object sprite data for layer 'level9'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level9[] = {
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
    18, 2, 56, 57, 58, 59,
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

#define OBJECT_SPRITE_LEVEL9_COUNT 24

// Object palette data for layer 'level9' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level9[3][4] = {
    // Object Palette 0
    { 0x0d, 0x15, 0x36, 0x1b },
    // Object Palette 1
    { 0x0d, 0x15, 0x18, 0x0c },
    // Object Palette 2
    { 0x0d, 0x31, 0x28, 0x00 }
};

#define PALETTE_OBJECT_LEVEL9_COUNT 3

// Player start location for layer 'level9'
#define SPAWN_X_LEVEL9 2
#define SPAWN_Y_LEVEL9 12

#endif // TILEMAP_LEVEL9_NES_H
