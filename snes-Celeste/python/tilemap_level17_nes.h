// NES tilemap data for layer 'level17'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL17_NES_H
#define TILEMAP_LEVEL17_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL17_WIDTH 16
#define TILEMAP_LEVEL17_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level17'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
const unsigned char tilemap_level17_compressed[] = {
    0xc0, 0xc0, 0xc0, 0xc0, 0xd8, 0xc2, 0xc2, 0xd5, 0xc0, 0xc0, 0xc0, 0x18, 0xf3, 0xfd, 0xc2, 0xc2,
    0x03, 0xc0, 0xc0, 0xc0, 0x0d, 0xe2, 0xdc, 0xc2, 0xd5, 0x03, 0xc0, 0xc0, 0xc0, 0xc0, 0xcb, 0xd8,
    0xc2, 0xd2, 0xc0, 0xc0, 0xc0, 0x00, 0x11, 0xd7, 0xd8, 0xc2, 0xc2, 0xc0, 0xc0, 0xc0, 0xc6, 0xcd,
    0xd8, 0xd5, 0xc2, 0xc0, 0xc0, 0xc0, 0xc0, 0xc1, 0xea, 0xc3, 0xc3, 0xc0, 0xdd, 0xc8, 0x1b, 0xd7,
    0xe1, 0xd6, 0xd6, 0xe8, 0x00, 0x1f, 0xe5, 0xde, 0xcd, 0xc1, 0xd4, 0xc0, 0xc0, 0x00, 0x0b, 0xdc,
    0xc2, 0x14, 0xdb, 0x13, 0x16, 0xc0, 0xc0, 0xc0, 0xcb, 0xea, 0x05, 0x15, 0xd6, 0xd6, 0xc0, 0xc0,
    0xc0, 0xc5, 0x0c, 0x08, 0xf7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc6, 0xc7, 0x24, 0x1d, 0xc0, 0xc0,
    0x10, 0x1e, 0xc0, 0xc0, 0xc0, 0xc7, 0xc5, 0xc0, 0xc6, 0x09, 0xfe, 0xc0, 0x00, 0xe5, 0x01, 0xcf,
    0x0c, 0xd4, 0x00, 0x1f, 0x0c, 0x1c, 0xd4, 0x1a, 0x01, 0xcc, 0xed, 0xf3, 0x09, 0x1e, 0xc1, 0x0b,
    0x1c, 0xc7
};

#define TILEMAP_LEVEL17_COMPRESSED_SIZE 146
#define TILEMAP_LEVEL17_COUNT 256

// Background palette data for layer 'level17' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level17[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL17_COUNT 1

// Sprite palette data for layer 'level17' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level17[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL17_COUNT 4

// Object data for layer 'level17'
const unsigned char object_level17[] = {
    18, 13, 11,
    64, 0, 14,
    18, 9, 14,
};

#define OBJECT_LEVEL17_COUNT 3

// Object sprite data for layer 'level17'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level17[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    18, 2, 46, 47, 48, 49,
    64, 2, 170, 171, 172, 106,
};

#define OBJECT_SPRITE_LEVEL17_COUNT 9

// Object palette data for layer 'level17' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level17[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL17_COUNT 3

// Player start location for layer 'level17'
#define SPAWN_X_LEVEL17 2
#define SPAWN_Y_LEVEL17 13

#endif // TILEMAP_LEVEL17_NES_H
