// NES tilemap data for layer 'level3'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL3_NES_H
#define TILEMAP_LEVEL3_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL3_WIDTH 16
#define TILEMAP_LEVEL3_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level3'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
const unsigned char tilemap_level3_compressed[] = {
    0xc2, 0xc2, 0xc2, 0xdf, 0xcd, 0xc1, 0xf8, 0xc2, 0xd2, 0xc2, 0xc9, 0xc6, 0xc1, 0x08, 0xe1, 0xfd,
    0xc2, 0xc2, 0xd5, 0xf1, 0x00, 0xc4, 0xc6, 0xee, 0xc2, 0xe7, 0xc2, 0xe4, 0xc0, 0xc0, 0xc0, 0xc0,
    0xf8, 0xf1, 0xf8, 0x06, 0x0e, 0x0f, 0xc0, 0xc0, 0xc0, 0xc0, 0x02, 0xc0, 0x00, 0x1d, 0xe5, 0x01,
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xf5, 0xc9, 0xc8, 0xc8, 0xc0, 0xc0, 0x20, 0xc0, 0xd1, 0xdc,
    0x05, 0xdb, 0x13, 0x16, 0x17, 0x18, 0xc0, 0x21, 0xc0, 0xc5, 0xc1, 0xc1, 0x0a, 0x0a, 0xc1, 0xd9,
    0xe5, 0xc0, 0xd7, 0xd3, 0x0b, 0xc4, 0xc6, 0xcd, 0xd8, 0x03, 0xc0, 0xc6, 0xc1, 0xc7, 0xc0, 0xc5,
    0xe2, 0xcc, 0xc0, 0xc0, 0xc6, 0x09, 0xc8, 0xc8, 0xc1, 0xd8, 0x04, 0xc0, 0xc0, 0xc0, 0x09, 0x12,
    0xd0, 0x16, 0x08, 0xe9, 0xcc, 0xc0, 0x0f, 0xc0, 0xc6, 0xd8, 0x15, 0xc4, 0xc0, 0xcc, 0xd0, 0x01,
    0xc0, 0xc0, 0x0d, 0xeb, 0xc0, 0xc0, 0xcc, 0xd2, 0xe4, 0x00, 0x1e, 0xc0, 0xeb, 0xc0, 0x00, 0x1a,
    0xc2
};

#define TILEMAP_LEVEL3_COMPRESSED_SIZE 145
#define TILEMAP_LEVEL3_COUNT 256

// Background palette data for layer 'level3' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level3[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x1a, 0x17, 0x27 },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL3_COUNT 3

// Sprite palette data for layer 'level3' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level3[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL3_COUNT 4

// Object data for layer 'level3'
const unsigned char object_level3[] = {
    26, 1, 6,
    18, 5, 14,
    18, 13, 14,
};

#define OBJECT_LEVEL3_COUNT 3

// Object sprite data for layer 'level3'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level3[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    18, 2, 46, 47, 48, 49,
    26, 1, 75, 76, 77, 78,
};

#define OBJECT_SPRITE_LEVEL3_COUNT 9

// Object palette data for layer 'level3' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level3[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL3_COUNT 3

// Player start location for layer 'level3'
#define SPAWN_X_LEVEL3 1
#define SPAWN_Y_LEVEL3 13

#endif // TILEMAP_LEVEL3_NES_H
