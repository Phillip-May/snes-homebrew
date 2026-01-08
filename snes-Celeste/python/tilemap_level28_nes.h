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
    0xc3, 0xc3, 0xe3, 0xda, 0xc7, 0xce, 0xcc, 0xc2, 0xc4, 0xd1, 0xd8, 0x16, 0x0c, 0xc4, 0xce, 0x07,
    0xc3, 0x05, 0xdb, 0x14, 0x17, 0xd8, 0xda, 0xc0, 0xce, 0x21, 0x09, 0x0d, 0xcb, 0xcd, 0xc1, 0xcc,
    0xde, 0xd0, 0x2a, 0x22, 0xc4, 0xf4, 0xca, 0x0b, 0x08, 0xfe, 0xc9, 0xd0, 0xc0, 0xc0, 0xc5, 0xc4,
    0xc0, 0xcc, 0xc9, 0xd0, 0xc0, 0xc0, 0xc5, 0xc8, 0xc8, 0xcc, 0xec, 0xd0, 0xc0, 0xc0, 0xc5, 0x13,
    0xdb, 0x14, 0xc3, 0xc9, 0xd0, 0xdd, 0xc0, 0x00, 0x08, 0xd6, 0xd6, 0x28, 0xf0, 0xed, 0xce, 0x23,
    0xd0, 0xc0, 0xc0, 0xc0, 0xce, 0xeb, 0x29, 0x18, 0xf8, 0xd0, 0xc0, 0xc0, 0xc0, 0xce, 0xeb, 0xc8,
    0x20, 0x21, 0xd0, 0xdd, 0xc8, 0x20, 0xd4, 0x20, 0xcc, 0xcf, 0x15, 0x06, 0xd0, 0x00, 0xcf, 0xde,
    0xcd, 0x13, 0xc3, 0xc3, 0x06, 0xd0, 0xc0, 0xc2, 0xc9, 0xc5, 0xc7, 0x11, 0xe2, 0xe9, 0xc0, 0x11,
    0xd2, 0xc9, 0xc6, 0xc1, 0xcd, 0xc4, 0xc0, 0xc5, 0xc2, 0xc9, 0x18, 0x18, 0xcd, 0xc7, 0xc0, 0xc0,
    0xd7
};

#define TILEMAP_LEVEL28_COMPRESSED_SIZE 145
#define TILEMAP_LEVEL28_COUNT 256

// Background palette data for layer 'level28' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level28[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0c, 0x27, 0x17 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL28_COUNT 2

// Sprite palette data for layer 'level28' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level28[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
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
    8, 0, 23, 24, 25, 26,
    9, 0, 27, 28, 29, 26,
    10, 0, 0, 26, 0, 26,
    13, 2, 34, 35, 36, 37,
    14, 2, 0, 38, 39, 0,
    15, 2, 39, 0, 0, 38,
    20, 0, 50, 51, 52, 53,
    22, 0, 56, 57, 58, 59,
    23, 1, 60, 61, 62, 63,
    64, 1, 97, 167, 168, 100,
};

#define OBJECT_SPRITE_LEVEL28_COUNT 17

// Object palette data for layer 'level28' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level28[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL28_COUNT 3

// Player start location for layer 'level28'
#define SPAWN_X_LEVEL28 1
#define SPAWN_Y_LEVEL28 1

#endif // TILEMAP_LEVEL28_NES_H
