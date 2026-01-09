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
    0xda, 0xc1, 0xea, 0xc3, 0xe3, 0xd5, 0xc2, 0xc2, 0xda, 0xd3, 0xc1, 0x0d, 0xcb, 0xdc, 0xc3, 0xe3,
    0xc9, 0xc1, 0xc1, 0xc4, 0xc6, 0xc1, 0xcd, 0xd8, 0xec, 0xc1, 0xc7, 0xc0, 0xc0, 0xc1, 0xc1, 0xd8,
    0xc9, 0xcb, 0xf3, 0xc0, 0xc6, 0xc1, 0xd3, 0xd8, 0xc9, 0x20, 0xe1, 0xc0, 0xc0, 0xf4, 0xc1, 0xc1,
    0xcc, 0x03, 0xf7, 0x09, 0x11, 0x1c, 0xd1, 0xc1, 0xca, 0xc6, 0xdc, 0x03, 0xee, 0xc1, 0xfd, 0xc1,
    0xd9, 0xc0, 0x00, 0xf8, 0xee, 0xc1, 0xc1, 0xf4, 0xc1, 0xc7, 0xc0, 0xf8, 0xc1, 0xcd, 0xc1, 0xc1,
    0xc1, 0xc4, 0xc0, 0xc0, 0xcb, 0xc1, 0xe1, 0xc1, 0xc1, 0x1c, 0x1d, 0xc0, 0xc0, 0xc0, 0xcb, 0xc1,
    0xc1, 0x08, 0xc1, 0xc4, 0xc0, 0x11, 0xc0, 0xcb, 0x09, 0x0d, 0xc4, 0x00, 0x1d, 0x0b, 0xc0, 0xc6,
    0x09, 0x25, 0xc0, 0xcd, 0xc0, 0xc0, 0xcb, 0x12, 0xf8, 0xc5, 0x26, 0x00, 0xf4, 0xc7, 0x82, 0x18,
    0xd1, 0xc7, 0xd1, 0x09, 0xcf, 0xde, 0xf3, 0xc0, 0xc0, 0xc1, 0xd4, 0xcd
};

#define TILEMAP_LEVEL8_COMPRESSED_SIZE 140
#define TILEMAP_LEVEL8_COUNT 256

// Background palette data for layer 'level8' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level8[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0c, 0x27, 0x17 },
    // Background Palette 2
    { 0x0d, 0x1a, 0x17, 0x27 },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL8_COUNT 3

// Sprite palette data for layer 'level8' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level8[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
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
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    9, 0, 27, 28, 29, 26,
    10, 0, 0, 26, 0, 26,
    11, 0, 30, 31, 0, 0,
    12, 0, 32, 33, 0, 0,
    13, 2, 34, 35, 36, 37,
    14, 2, 0, 38, 39, 0,
    15, 2, 39, 0, 0, 38,
    18, 0, 46, 47, 48, 49,
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

#define OBJECT_SPRITE_LEVEL8_COUNT 24

// Object palette data for layer 'level8' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level8[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL8_COUNT 3

// Player start location for layer 'level8'
#define SPAWN_X_LEVEL8 1
#define SPAWN_Y_LEVEL8 14

#endif // TILEMAP_LEVEL8_NES_H
