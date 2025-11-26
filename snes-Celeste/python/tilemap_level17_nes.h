// NES tilemap data for layer 'level17'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL17_NES_H
#define TILEMAP_LEVEL17_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL17_WIDTH 16
#define TILEMAP_LEVEL17_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level17' (GIDs, one byte per entry)
const unsigned char tilemap_level17[] = {
    // Row 0
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    9,
    7,
    3,
    3,
    3,
    3,
    4,
    3,
    // Row 1
    0,
    0,
    0,
    0,
    0,
    0,
    24,
    31,
    9,
    7,
    4,
    3,
    3,
    3,
    3,
    3,
    // Row 2
    0,
    0,
    0,
    0,
    0,
    0,
    13,
    11,
    9,
    2,
    5,
    3,
    3,
    4,
    3,
    3,
    // Row 3
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    13,
    9,
    9,
    7,
    3,
    3,
    3,
    4,
    // Row 4
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    17,
    16,
    9,
    9,
    7,
    3,
    3,
    3,
    3,
    // Row 5
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    13,
    9,
    12,
    9,
    7,
    4,
    3,
    3,
    3,
    // Row 6
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    9,
    9,
    9,
    2,
    5,
    5,
    5,
    5,
    // Row 7
    0,
    0,
    0,
    27,
    27,
    27,
    27,
    16,
    9,
    9,
    11,
    34,
    34,
    34,
    34,
    34,
    // Row 8
    0,
    0,
    31,
    26,
    20,
    20,
    1,
    9,
    12,
    9,
    9,
    17,
    0,
    0,
    0,
    0,
    // Row 9
    0,
    0,
    11,
    2,
    5,
    3,
    3,
    20,
    19,
    19,
    19,
    22,
    0,
    0,
    0,
    0,
    // Row 10
    0,
    0,
    13,
    9,
    9,
    2,
    5,
    21,
    34,
    34,
    34,
    34,
    0,
    0,
    0,
    0,
    // Row 11
    0,
    0,
    0,
    9,
    12,
    8,
    36,
    28,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 12
    0,
    0,
    0,
    13,
    9,
    0,
    36,
    29,
    0,
    0,
    0,
    0,
    16,
    30,
    0,
    0,
    // Row 13
    0,
    0,
    0,
    0,
    9,
    0,
    0,
    9,
    0,
    0,
    0,
    13,
    9,
    28,
    0,
    0,
    // Row 14
    0,
    0,
    26,
    20,
    1,
    35,
    0,
    12,
    17,
    0,
    0,
    31,
    12,
    28,
    17,
    0,
    // Row 15
    26,
    1,
    7,
    3,
    21,
    35,
    31,
    9,
    9,
    30,
    9,
    9,
    11,
    28,
    9,
    0
};

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

// Collision data for layer 'level17'
const unsigned char collision_level17[] = {
    // Row 0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    // Row 1
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    // Row 2
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    // Row 3
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    // Row 4
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    // Row 5
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    // Row 6
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    // Row 7
    0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 8, 8, 8, 8, 8,
    // Row 8
    0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 9
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    // Row 10
    0, 0, 0, 0, 0, 1, 1, 1, 8, 8, 8, 8, 0, 0, 0, 0,
    // Row 11
    0, 0, 0, 0, 0, 0, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 12
    0, 0, 0, 0, 0, 0, 32, 1, 0, 0, 0, 0, 0, 1, 0, 0,
    // Row 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    // Row 14
    0, 0, 1, 1, 1, 16, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    // Row 15
    1, 1, 1, 1, 1, 16, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0
};

#define COLLISION_LEVEL17_COUNT 256

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
