// NES tilemap data for layer 'level13'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL13_NES_H
#define TILEMAP_LEVEL13_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL13_WIDTH 16
#define TILEMAP_LEVEL13_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level13'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_2")))
const unsigned char tilemap_level13_compressed[] = {
    0xc2, 0xc3, 0xc3, 0x06, 0xdc, 0xc3, 0xdf, 0xca, 0xfb, 0xe4, 0xc0, 0xc0, 0xc0, 0x0d, 0xe2, 0xcd,
    0xd4, 0xc9, 0xc0, 0xc0, 0xc8, 0xc0, 0x00, 0x08, 0xcb, 0xc4, 0xc9, 0xd4, 0xc0, 0x1d, 0x01, 0xc0,
    0xc0, 0x00, 0xe5, 0x14, 0xc9, 0xc7, 0xd1, 0xeb, 0xc0, 0xc0, 0xf5, 0xc2, 0xdf, 0xd9, 0xc5, 0xeb,
    0xc8, 0xc8, 0x1e, 0xcc, 0x03, 0xc4, 0xcb, 0xd3, 0xdc, 0xdb, 0xdb, 0x14, 0xd2, 0x03, 0xc0, 0xc5,
    0xc1, 0xc4, 0xc0, 0xc0, 0xdc, 0xc2, 0xd4, 0xd7, 0xc4, 0xc0, 0xc0, 0xc0, 0xc5, 0xdc, 0xc1, 0xe1,
    0xc0, 0xc0, 0xdd, 0xc0, 0x0d, 0xd3, 0x08, 0xcd, 0xca, 0xc0, 0xc0, 0x28, 0x0a, 0xd0, 0xf3, 0xc7,
    0xc7, 0xe9, 0xdd, 0xc0, 0xe6, 0xc6, 0x0a, 0x0b, 0x08, 0x23, 0xc4, 0xc0, 0x28, 0x0a, 0x27, 0xd4,
    0xc0, 0x00, 0xc4, 0x00, 0x2c, 0x00, 0x33, 0xc0, 0xc6, 0x0c, 0xc4, 0xe0, 0xc0, 0xc0, 0xe5, 0x01,
    0xd0, 0xc0, 0xd9, 0x28, 0x21, 0xc0, 0xc0, 0xf5, 0xc9, 0xd0, 0xd1, 0xe1, 0xf8, 0xc0, 0x00, 0xe5,
    0x03
};

#define TILEMAP_LEVEL13_COMPRESSED_SIZE 145
#define TILEMAP_LEVEL13_COUNT 256

// Background palette data for layer 'level13' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_2")))
const unsigned char palette_background_level13[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1a, 0x17, 0x27 },
    // Background Palette 2
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL13_COUNT 3

// Sprite palette data for layer 'level13' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_2")))
const unsigned char palette_sprite_level13[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL13_COUNT 4

// Object data for layer 'level13'
__attribute__((section(".prg_rom_2")))
const unsigned char object_level13[] = {
    22, 4, 1,
    22, 10, 4,
    22, 1, 7,
    8, 5, 14,
    20, 14, 14,
};

#define OBJECT_LEVEL13_COUNT 5

// Object sprite data for layer 'level13'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_2")))
const unsigned char object_sprite_level13[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    8, 0, 23, 24, 25, 26,
    20, 0, 50, 51, 52, 53,
    22, 0, 56, 57, 58, 59,
};

#define OBJECT_SPRITE_LEVEL13_COUNT 10

// Object palette data for layer 'level13' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level13[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL13_COUNT 3

// Player start location for layer 'level13'
#define SPAWN_X_LEVEL13 0
#define SPAWN_Y_LEVEL13 13

#endif // TILEMAP_LEVEL13_NES_H
