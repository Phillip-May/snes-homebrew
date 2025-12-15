// NES tilemap data for layer 'level4'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL4_NES_H
#define TILEMAP_LEVEL4_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL4_WIDTH 16
#define TILEMAP_LEVEL4_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level4'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
const unsigned char tilemap_level4_compressed[] = {
    0xc3, 0xc3, 0xf1, 0xc0, 0xdc, 0xc3, 0xc3, 0xc3, 0xc1, 0xca, 0xc0, 0xc0, 0xc1, 0xc4, 0xc0, 0xc0,
    0xcd, 0xc4, 0xd1, 0x17, 0x1f, 0xd3, 0xc0, 0xc0, 0xc0, 0xc4, 0xc0, 0x00, 0xe5, 0xf6, 0xc7, 0xc0,
    0xc0, 0xc0, 0xc0, 0x10, 0x10, 0xf8, 0x15, 0x0a, 0xd9, 0xc0, 0xc0, 0x00, 0x11, 0x18, 0xf3, 0xc1,
    0xeb, 0xca, 0xc0, 0xc0, 0xc0, 0xcd, 0xc1, 0xcd, 0xfc, 0xc7, 0xc0, 0xc0, 0xc0, 0xc1, 0xc1, 0xe2,
    0x1a, 0x01, 0xc4, 0xc0, 0xc0, 0xc0, 0xca, 0xc6, 0xc1, 0xeb, 0xc0, 0xc0, 0xc0, 0xc0, 0xe9, 0xc0,
    0xcd, 0xeb, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xcb, 0xeb, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xd1,
    0xc1, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0x19, 0xd1, 0xd3, 0x08, 0xe9, 0xc0, 0xc0, 0xc0, 0xc0, 0xd0,
    0xde, 0xc4, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc2, 0xc9, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc2,
    0xec, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0
};

#define TILEMAP_LEVEL4_COMPRESSED_SIZE 135
#define TILEMAP_LEVEL4_COUNT 256

// Background palette data for layer 'level4' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level4[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL4_COUNT 2

// Sprite palette data for layer 'level4' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level4[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL4_COUNT 4

// Object data for layer 'level4'
const unsigned char object_level4[] = {
    28, 2, 4,
    23, 12, 6,
    23, 13, 6,
    23, 11, 9,
    23, 12, 9,
    23, 14, 11,
    23, 15, 11,
    23, 12, 13,
    23, 13, 13,
    23, 8, 14,
    23, 9, 14,
    23, 4, 15,
    23, 5, 15,
};

#define OBJECT_LEVEL4_COUNT 13

// Object sprite data for layer 'level4'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level4[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    23, 0, 63, 64, 65, 66,
    28, 1, 81, 82, 83, 84,
};

#define OBJECT_SPRITE_LEVEL4_COUNT 9

// Object palette data for layer 'level4' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level4[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL4_COUNT 3

// Player start location for layer 'level4'
#define SPAWN_X_LEVEL4 1
#define SPAWN_Y_LEVEL4 12

#endif // TILEMAP_LEVEL4_NES_H
