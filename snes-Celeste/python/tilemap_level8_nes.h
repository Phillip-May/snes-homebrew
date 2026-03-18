// NES tilemap data for layer 'level8'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL8_NES_H
#define TILEMAP_LEVEL8_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL8_WIDTH 16
#define TILEMAP_LEVEL8_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level8'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level8_compressed[] = {
    0xd8, 0xc1, 0xeb, 0xc3, 0xdf, 0xd4, 0xc2, 0xc2, 0xd8, 0xd2, 0xc1, 0x0d, 0xcb, 0xde, 0xc3, 0xdf,
    0xc8, 0xc1, 0xc1, 0xc4, 0xc6, 0xc1, 0xcd, 0xd7, 0xea, 0xc1, 0xc7, 0xc0, 0xc0, 0xc1, 0xc1, 0xd7,
    0xc8, 0xcb, 0xf2, 0xc0, 0xc6, 0xc1, 0xd2, 0xd7, 0xc8, 0x22, 0xe1, 0xc0, 0xc0, 0xf3, 0xc1, 0xc1,
    0xcc, 0x03, 0xf5, 0x09, 0x11, 0x1c, 0xd3, 0xc1, 0xc9, 0xc6, 0xde, 0x03, 0xee, 0xc1, 0x0c, 0xc5,
    0xc1, 0xd5, 0xc0, 0xf7, 0xee, 0xc1, 0xc1, 0xf3, 0xc1, 0xc7, 0xc0, 0xf7, 0xc1, 0xcd, 0xc1, 0xc1,
    0xc1, 0xc4, 0xc0, 0xc0, 0xcb, 0xc1, 0xe1, 0xc1, 0xc1, 0x1c, 0x1d, 0xc0, 0xc0, 0xc0, 0xcb, 0xc1,
    0xc1, 0x08, 0xc1, 0xc4, 0xc0, 0x11, 0xc0, 0xcb, 0x09, 0x0d, 0xc4, 0x00, 0x1d, 0x0b, 0xc0, 0xc6,
    0x09, 0x2e, 0xc0, 0xcd, 0xc0, 0xc0, 0xcb, 0x12, 0xf7, 0xc5, 0x2f, 0x00, 0xf3, 0xc7, 0x82, 0x18,
    0xd3, 0xc7, 0xd3, 0x09, 0xce, 0xdc, 0xf2, 0xc0, 0xc0, 0xc1, 0xd5, 0xcd
};

#define TILEMAP_LEVEL8_COMPRESSED_SIZE 140
#define TILEMAP_LEVEL8_COUNT 256

// Background palette data for layer 'level8' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level8[4][4] = {
    // Background Palette 0
    { 0x0d, 0x37, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x37, 0x00, 0x10 },
    // Background Palette 2
    { 0x0d, 0x0c, 0x28, 0x17 },
    // Background Palette 3
    { 0x0d, 0x37, 0x1b, 0x1a }
};

#define PALETTE_BACKGROUND_LEVEL8_COUNT 4

// Sprite palette data for layer 'level8' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level8[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x15, 0x1b, 0x36 },
    // Sprite Palette 1
    { 0x0d, 0x31, 0x28, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x15, 0x18, 0x37 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x36, 0x1b }
};

#define PALETTE_SPRITE_LEVEL8_COUNT 4

// Object data for layer 'level8'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level8[] = {
    22, 6, 4,
    23, 14, 7,
    23, 14, 8,
    18, 12, 12,
    23, 12, 13,
    23, 6, 14,
    23, 7, 14,
    23, 8, 14,
};

#define OBJECT_LEVEL8_COUNT 8

// Object sprite data for layer 'level8'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level8[] = {
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
    18, 1, 56, 57, 58, 59,
    22, 0, 68, 69, 70, 71,
    23, 1, 72, 73, 74, 75,
    29, 2, 94, 95, 96, 97,
    30, 2, 98, 99, 100, 101,
    31, 2, 102, 103, 104, 105,
    45, 1, 134, 135, 136, 137,
    46, 1, 0, 0, 138, 139,
    47, 1, 0, 0, 140, 141,
    64, 1, 110, 183, 184, 113,
};

#define OBJECT_SPRITE_LEVEL8_COUNT 24

// Object palette data for layer 'level8' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level8[3][4] = {
    // Object Palette 0
    { 0x0d, 0x15, 0x1b, 0x36 },
    // Object Palette 1
    { 0x0d, 0x31, 0x28, 0x00 },
    // Object Palette 2
    { 0x0d, 0x15, 0x18, 0x37 }
};

#define PALETTE_OBJECT_LEVEL8_COUNT 3

// Player start location for layer 'level8'
#define SPAWN_X_LEVEL8 1
#define SPAWN_Y_LEVEL8 14

#endif // TILEMAP_LEVEL8_NES_H
