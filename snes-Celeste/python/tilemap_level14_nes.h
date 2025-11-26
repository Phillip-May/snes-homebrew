// NES tilemap data for layer 'level14'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL14_NES_H
#define TILEMAP_LEVEL14_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL14_WIDTH 16
#define TILEMAP_LEVEL14_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level14' (GIDs, one byte per entry)
const unsigned char tilemap_level14[] = {
    21, 9, 8, 9, 23, 0, 0, 0, 0, 0, 9, 9, 2, 5, 5, 5,
    21, 9, 0, 12, 10, 17, 14, 0, 0, 0, 0, 13, 12, 9, 9, 9,
    21, 9, 16, 9, 10, 11, 10, 27, 27, 27, 26, 20, 1, 9, 9, 11,
    3, 20, 20, 1, 26, 20, 1, 26, 20, 1, 7, 4, 21, 9, 9, 9,
    3, 3, 4, 21, 2, 5, 6, 2, 5, 6, 7, 3, 21, 10, 9, 12,
    3, 4, 3, 21, 10, 34, 34, 34, 34, 34, 7, 3, 21, 9, 9, 9,
    3, 3, 3, 3, 1, 0, 0, 0, 0, 0, 2, 5, 6, 9, 11, 9,
    3, 3, 3, 3, 21, 23, 24, 0, 0, 0, 10, 0, 0, 13, 9, 9,
    3, 3, 4, 3, 21, 12, 8, 0, 0, 0, 0, 0, 0, 24, 31, 9,
    3, 3, 3, 3, 21, 9, 0, 0, 0, 0, 0, 0, 0, 13, 26, 20,
    5, 5, 5, 5, 6, 8, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3,
    34, 34, 34, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9,
    0, 0, 0, 0, 0, 0, 0, 0, 16, 23, 0, 0, 0, 0, 13, 12,
    1, 0, 0, 0, 0, 0, 0, 0, 9, 8, 0, 0, 0, 0, 0, 9,
    21, 0, 0, 0, 30, 0, 0, 16, 9, 0, 0, 0, 0, 0, 0, 9
};

#define TILEMAP_LEVEL14_COUNT 256

// Background palette data for layer 'level14' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level14[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL14_COUNT 2

// Sprite palette data for layer 'level14' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level14[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL14_COUNT 4

// Object data for layer 'level14'
const unsigned char object_level14[] = {
    26, 2, 1,
    23, 10, 8,
    23, 10, 9,
    18, 4, 14,
};

#define OBJECT_LEVEL14_COUNT 4

// Object sprite data for layer 'level14'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level14[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    18, 2, 46, 47, 48, 49,
    23, 0, 63, 64, 65, 66,
    26, 1, 75, 76, 77, 78,
};

#define OBJECT_SPRITE_LEVEL14_COUNT 10

// Object palette data for layer 'level14' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level14[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL14_COUNT 3

// Player start location for layer 'level14'
#define SPAWN_X_LEVEL14 0
#define SPAWN_Y_LEVEL14 13

#endif // TILEMAP_LEVEL14_NES_H
