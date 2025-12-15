// NES tilemap data for layer 'level30'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL30_NES_H
#define TILEMAP_LEVEL30_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL30_WIDTH 16
#define TILEMAP_LEVEL30_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level30'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
const unsigned char tilemap_level30_compressed[] = {
    0xd2, 0xc2, 0xdf, 0xc1, 0x08, 0xcc, 0xc3, 0xe3, 0xc2, 0xe7, 0x06, 0xd6, 0xe2, 0xee, 0x06, 0xd3,
    0xd8, 0xd5, 0x15, 0xd6, 0xc0, 0x0c, 0x1a, 0x01, 0xcf, 0xc0, 0x0b, 0xcc, 0xc9, 0xcf, 0xc0, 0x0d,
    0xfc, 0xcf, 0x00, 0xf3, 0xcc, 0xc9, 0xcf, 0xc0, 0xe6, 0xe8, 0xc6, 0x0c, 0x0d, 0xcc, 0xc9, 0xcf,
    0xc0, 0xc0, 0xc0, 0xc0, 0x08, 0xf0, 0x04, 0xec, 0xc8, 0xc8, 0xc0, 0xc0, 0xc0, 0xce, 0xf8, 0xc2,
    0x01, 0xe5, 0x01, 0xcf, 0xc0, 0xc0, 0xc0, 0x22, 0x07, 0xc3, 0x06, 0xdc, 0x06, 0xcf, 0xdd, 0xe0,
    0xc0, 0xf0, 0xd6, 0xd6, 0xd6, 0xdd, 0x1b, 0x1a, 0x01, 0xe0, 0xce, 0x07, 0xcf, 0xc0, 0xc0, 0x24,
    0xe5, 0xe7, 0x16, 0xcf, 0x24, 0x02, 0xcf, 0xc0, 0xc0, 0xf0, 0x03, 0x06, 0xd6, 0xc0, 0xe6, 0xc8,
    0xc8, 0xc0, 0x24, 0xfc, 0xe8, 0x10, 0xc0, 0xc0, 0xd0, 0xde, 0xcf, 0xe6, 0xe8, 0xf4, 0xc0, 0x00,
    0x1f, 0xd2, 0xc9, 0xcf, 0xc0, 0xd4, 0xc5, 0x10, 0xd1, 0x09, 0xc2, 0xec, 0xcf, 0xd1, 0xfb, 0xd7,
    0xe2, 0xd3
};

#define TILEMAP_LEVEL30_COMPRESSED_SIZE 146
#define TILEMAP_LEVEL30_COUNT 256

// Background palette data for layer 'level30' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level30[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL30_COUNT 1

// Sprite palette data for layer 'level30' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level30[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL30_COUNT 4

// Object data for layer 'level30'
const unsigned char object_level30[] = {
    20, 8, 1,
    8, 12, 2,
    22, 9, 6,
    23, 1, 11,
    22, 13, 12,
    22, 5, 14,
};

#define OBJECT_LEVEL30_COUNT 6

// Object sprite data for layer 'level30'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level30[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    8, 0, 23, 24, 25, 26,
    20, 0, 52, 53, 54, 55,
    22, 1, 59, 60, 61, 62,
    23, 0, 63, 64, 65, 66,
};

#define OBJECT_SPRITE_LEVEL30_COUNT 11

// Object palette data for layer 'level30' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level30[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL30_COUNT 3

// Player start location for layer 'level30'
#define SPAWN_X_LEVEL30 1
#define SPAWN_Y_LEVEL30 10

#endif // TILEMAP_LEVEL30_NES_H
