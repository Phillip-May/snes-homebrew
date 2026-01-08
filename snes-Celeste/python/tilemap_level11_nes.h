// NES tilemap data for layer 'level11'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL11_NES_H
#define TILEMAP_LEVEL11_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL11_WIDTH 16
#define TILEMAP_LEVEL11_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level11'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level11_compressed[] = {
    0xe7, 0xc3, 0xc3, 0xc3, 0xc9, 0xc1, 0xd8, 0xd5, 0xda, 0xc0, 0xd1, 0xc1, 0xfd, 0xc1, 0x0c, 0xcc,
    0x03, 0xee, 0xc8, 0x20, 0xc1, 0xc7, 0xc5, 0xc6, 0xf9, 0x03, 0x0c, 0x0b, 0xe5, 0xf6, 0x0c, 0xc4,
    0x11, 0x0b, 0xc4, 0x0d, 0xcc, 0x09, 0x13, 0xc2, 0x06, 0x08, 0xe9, 0xc0, 0xe9, 0xc8, 0xcc, 0x08,
    0xf0, 0xda, 0xd4, 0xc0, 0xc0, 0x2a, 0xe5, 0xc2, 0xce, 0xeb, 0xc4, 0xc0, 0xc0, 0xce, 0xf9, 0xd5,
    0xce, 0xfd, 0xc0, 0xc0, 0xc0, 0xc6, 0xea, 0xe3, 0xc0, 0x0d, 0xc4, 0xdd, 0xe0, 0xc0, 0xc5, 0xc1,
    0x02, 0xc8, 0xc8, 0xc8, 0x1f, 0x17, 0xc0, 0xc0, 0xcb, 0xfc, 0x15, 0xdb, 0xdb, 0x14, 0xf1, 0xc0,
    0xc0, 0xc5, 0xd9, 0xee, 0xd3, 0xca, 0xc0, 0xc0, 0xc0, 0x00, 0xe2, 0xc1, 0xca, 0xc6, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc1, 0xca, 0xc0, 0xc0, 0xc0, 0xc0, 0xc6, 0x1c, 0xf3, 0xc4, 0xc0, 0xc0, 0xc0, 0x18,
    0xf7, 0xc0, 0xc1, 0x0c, 0x12, 0x11, 0xff, 0xf7, 0xc0, 0xc0, 0xc0, 0xd7, 0xc1, 0xc1
};

#define TILEMAP_LEVEL11_COMPRESSED_SIZE 142
#define TILEMAP_LEVEL11_COUNT 256

// Background palette data for layer 'level11' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level11[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0c, 0x27, 0x17 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL11_COUNT 2

// Sprite palette data for layer 'level11' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level11[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL11_COUNT 4

// Object data for layer 'level11'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level11[] = {
    22, 4, 1,
    11, 15, 9,
    23, 6, 14,
    23, 7, 14,
    23, 1, 15,
    23, 2, 15,
};

#define OBJECT_LEVEL11_COUNT 6

// Object sprite data for layer 'level11'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level11[] = {
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
    22, 0, 56, 57, 58, 59,
    23, 1, 60, 61, 62, 63,
    64, 1, 97, 167, 168, 100,
};

#define OBJECT_SPRITE_LEVEL11_COUNT 17

// Object palette data for layer 'level11' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level11[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL11_COUNT 3

// Player start location for layer 'level11'
#define SPAWN_X_LEVEL11 1
#define SPAWN_Y_LEVEL11 14

#endif // TILEMAP_LEVEL11_NES_H
