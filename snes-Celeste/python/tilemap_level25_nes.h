// NES tilemap data for layer 'level25'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL25_NES_H
#define TILEMAP_LEVEL25_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL25_WIDTH 16
#define TILEMAP_LEVEL25_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level25'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
const unsigned char tilemap_level25_compressed[] = {
    0xc2, 0xc2, 0xc9, 0x23, 0xd6, 0x22, 0xdc, 0xe3, 0xe4, 0xc2, 0xd2, 0xc9, 0xcf, 0xc0, 0xd6, 0x22,
    0xeb, 0xfa, 0xc2, 0xc2, 0xed, 0xc0, 0xc0, 0xc6, 0xeb, 0x17, 0xd2, 0xe7, 0xdf, 0xcf, 0xc0, 0xe0,
    0xd8, 0x15, 0x08, 0xc2, 0x15, 0xe5, 0x01, 0xcf, 0x10, 0x17, 0x1e, 0x23, 0x0d, 0xeb, 0x0c, 0xc2,
    0x15, 0xfd, 0xed, 0xc6, 0x0c, 0x1c, 0xcf, 0xeb, 0x08, 0xc3, 0x06, 0xdc, 0xed, 0xc0, 0x09, 0x1c,
    0xcf, 0xfc, 0xc5, 0xc0, 0xc6, 0x1c, 0x1b, 0x1e, 0xc6, 0x1c, 0xcf, 0x0b, 0x0b, 0x11, 0xc4, 0xe0,
    0xee, 0x13, 0xed, 0xee, 0xdb, 0xdb, 0x13, 0xce, 0x1e, 0xcf, 0x0d, 0x0c, 0x1c, 0xcf, 0xc0, 0xcb,
    0x11, 0x24, 0xce, 0x1c, 0xcf, 0xc5, 0x1c, 0xc8, 0xc8, 0xc5, 0x0c, 0x24, 0xce, 0x1d, 0xcf, 0xc6,
    0x02, 0xdb, 0x13, 0x16, 0xc5, 0x08, 0x24, 0xd4, 0xe8, 0xc0, 0xe6, 0xd6, 0xd6, 0xc6, 0xce, 0xca,
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xce, 0x0b, 0x17, 0x18, 0x10, 0xe5, 0x01, 0xc8, 0xc8, 0xe0,
    0xc0, 0x24, 0xcd, 0xc1, 0xcc, 0xf9, 0xd0, 0xde, 0xc0, 0xce
};

#define TILEMAP_LEVEL25_COMPRESSED_SIZE 154
#define TILEMAP_LEVEL25_COUNT 256

// Background palette data for layer 'level25' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level25[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL25_COUNT 1

// Sprite palette data for layer 'level25' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level25[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL25_COUNT 4

// Object data for layer 'level25'
const unsigned char object_level25[] = {
    18, 12, 5,
    18, 7, 6,
    23, 12, 6,
    8, 0, 10,
    20, 6, 13,
    18, 13, 14,
    23, 13, 15,
};

#define OBJECT_LEVEL25_COUNT 7

// Object sprite data for layer 'level25'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level25[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    8, 0, 23, 24, 25, 26,
    18, 2, 46, 47, 48, 49,
    20, 0, 50, 51, 52, 53,
    23, 0, 61, 62, 63, 64,
};

#define OBJECT_SPRITE_LEVEL25_COUNT 11

// Object palette data for layer 'level25' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level25[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL25_COUNT 3

// Player start location for layer 'level25'
#define SPAWN_X_LEVEL25 4
#define SPAWN_Y_LEVEL25 13

#endif // TILEMAP_LEVEL25_NES_H
