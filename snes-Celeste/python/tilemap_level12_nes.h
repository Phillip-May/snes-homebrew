// NES tilemap data for layer 'level12'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL12_NES_H
#define TILEMAP_LEVEL12_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL12_WIDTH 16
#define TILEMAP_LEVEL12_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level12' (GIDs, one byte per entry)
const unsigned char tilemap_level12[] = {
    // Row 0
    3,
    3,
    3,
    6,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    38,
    // Row 1
    4,
    3,
    21,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    38,
    // Row 2
    3,
    3,
    21,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    38,
    // Row 3
    5,
    5,
    6,
    23,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    39,
    0,
    // Row 4
    20,
    1,
    9,
    9,
    0,
    0,
    32,
    0,
    0,
    32,
    0,
    0,
    0,
    39,
    40,
    40,
    // Row 5
    3,
    21,
    12,
    9,
    23,
    0,
    37,
    0,
    0,
    37,
    39,
    41,
    41,
    40,
    42,
    42,
    // Row 6
    3,
    6,
    9,
    9,
    9,
    17,
    26,
    20,
    20,
    1,
    38,
    40,
    40,
    43,
    0,
    0,
    // Row 7
    21,
    13,
    9,
    9,
    11,
    9,
    7,
    3,
    3,
    21,
    44,
    42,
    43,
    0,
    0,
    0,
    // Row 8
    21,
    0,
    8,
    0,
    13,
    9,
    7,
    4,
    3,
    3,
    1,
    0,
    0,
    0,
    0,
    0,
    // Row 9
    21,
    0,
    0,
    0,
    0,
    26,
    3,
    3,
    3,
    3,
    21,
    0,
    0,
    0,
    0,
    0,
    // Row 10
    6,
    0,
    0,
    0,
    0,
    2,
    5,
    3,
    3,
    3,
    6,
    0,
    0,
    0,
    0,
    0,
    // Row 11
    0,
    0,
    0,
    0,
    39,
    41,
    45,
    7,
    3,
    21,
    9,
    17,
    0,
    0,
    0,
    0,
    // Row 12
    0,
    0,
    0,
    0,
    38,
    40,
    46,
    7,
    4,
    21,
    9,
    9,
    0,
    0,
    0,
    0,
    // Row 13
    0,
    47,
    0,
    0,
    38,
    40,
    46,
    7,
    3,
    21,
    9,
    11,
    23,
    16,
    17,
    0,
    // Row 14
    0,
    26,
    20,
    1,
    38,
    40,
    46,
    7,
    3,
    6,
    9,
    9,
    9,
    12,
    8,
    0,
    // Row 15
    0,
    7,
    3,
    21,
    38,
    40,
    46,
    7,
    21,
    9,
    9,
    9,
    9,
    9,
    9,
    17
};

#define TILEMAP_LEVEL12_COUNT 256

// Background palette data for layer 'level12' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level12[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1a, 0x17, 0x27 },
    // Background Palette 2
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL12_COUNT 3

// Sprite palette data for layer 'level12' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level12[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL12_COUNT 4

// Collision data for layer 'level12'
const unsigned char collision_level12[] = {
    // Row 0
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    // Row 1
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    // Row 2
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    // Row 3
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    // Row 4
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    // Row 5
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    // Row 6
    1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
    // Row 7
    1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    // Row 8
    1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    // Row 9
    1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    // Row 10
    1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    // Row 11
    0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    // Row 12
    0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    // Row 13
    0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    // Row 14
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    // Row 15
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0
};

#define COLLISION_LEVEL12_COUNT 256

// Object data for layer 'level12'
const unsigned char object_level12[] = {
    70, 7, 4,
    71, 8, 4,
    86, 7, 5,
    87, 8, 5,
    28, 12, 9,
};

#define OBJECT_LEVEL12_COUNT 5

// Object sprite data for layer 'level12'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level12[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    28, 1, 81, 82, 83, 84,
    70, 1, 0, 0, 188, 189,
    71, 1, 0, 0, 190, 191,
    86, 1, 210, 211, 212, 106,
    87, 1, 213, 214, 106, 215,
};

#define OBJECT_SPRITE_LEVEL12_COUNT 12

// Object palette data for layer 'level12' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level12[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL12_COUNT 3

// Player start location for layer 'level12'
#define SPAWN_X_LEVEL12 2
#define SPAWN_Y_LEVEL12 13

#endif // TILEMAP_LEVEL12_NES_H
