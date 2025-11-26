// NES tilemap data for layer 'level13'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL13_NES_H
#define TILEMAP_LEVEL13_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL13_WIDTH 16
#define TILEMAP_LEVEL13_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level13' (GIDs, one byte per entry)
const unsigned char tilemap_level13[] = {
    // Row 0
    3,
    3,
    5,
    5,
    5,
    5,
    6,
    2,
    5,
    5,
    5,
    5,
    6,
    9,
    8,
    0,
    // Row 1
    3,
    21,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    13,
    11,
    9,
    9,
    12,
    17,
    0,
    // Row 2
    3,
    21,
    0,
    0,
    0,
    0,
    27,
    27,
    0,
    0,
    0,
    8,
    13,
    9,
    8,
    0,
    // Row 3
    3,
    21,
    17,
    0,
    0,
    0,
    26,
    1,
    0,
    0,
    0,
    0,
    0,
    26,
    20,
    20,
    // Row 4
    3,
    21,
    9,
    0,
    0,
    16,
    7,
    21,
    0,
    0,
    0,
    0,
    0,
    7,
    3,
    3,
    // Row 5
    5,
    6,
    9,
    17,
    0,
    9,
    7,
    21,
    27,
    27,
    27,
    27,
    27,
    7,
    3,
    3,
    // Row 6
    8,
    0,
    13,
    9,
    12,
    9,
    2,
    5,
    19,
    19,
    19,
    19,
    20,
    3,
    4,
    3,
    // Row 7
    0,
    0,
    0,
    9,
    9,
    9,
    8,
    0,
    0,
    0,
    0,
    0,
    2,
    5,
    3,
    3,
    // Row 8
    17,
    0,
    16,
    9,
    8,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    9,
    2,
    5,
    // Row 9
    9,
    9,
    9,
    11,
    0,
    0,
    0,
    0,
    0,
    27,
    0,
    0,
    13,
    12,
    9,
    8,
    // Row 10
    9,
    12,
    9,
    8,
    0,
    0,
    0,
    0,
    36,
    10,
    35,
    0,
    31,
    9,
    9,
    0,
    // Row 11
    9,
    0,
    13,
    0,
    0,
    27,
    0,
    0,
    0,
    34,
    0,
    13,
    10,
    11,
    8,
    32,
    // Row 12
    8,
    0,
    0,
    0,
    36,
    10,
    35,
    17,
    0,
    0,
    0,
    0,
    8,
    0,
    0,
    37,
    // Row 13
    0,
    47,
    0,
    0,
    0,
    13,
    12,
    8,
    0,
    27,
    0,
    0,
    0,
    0,
    0,
    26,
    // Row 14
    20,
    1,
    35,
    0,
    0,
    0,
    9,
    17,
    36,
    30,
    0,
    0,
    0,
    0,
    0,
    7,
    // Row 15
    3,
    21,
    35,
    0,
    0,
    16,
    9,
    11,
    36,
    28,
    0,
    0,
    0,
    26,
    20,
    3
};

#define TILEMAP_LEVEL13_COUNT 256

// Background palette data for layer 'level13' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
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
const unsigned char palette_sprite_level13[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL13_COUNT 4

// Collision data for layer 'level13'
const unsigned char collision_level13[] = {
    // Row 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    // Row 1
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 2
    1, 1, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 3
    1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
    // Row 4
    1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
    // Row 5
    1, 1, 0, 0, 0, 0, 1, 1, 4, 4, 4, 4, 4, 1, 1, 1,
    // Row 6
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    // Row 7
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    // Row 8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    // Row 9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0,
    // Row 10
    0, 0, 0, 0, 0, 0, 0, 0, 32, 1, 16, 0, 0, 0, 0, 0,
    // Row 11
    0, 0, 0, 0, 0, 4, 0, 0, 0, 8, 0, 0, 1, 0, 0, 0,
    // Row 12
    0, 0, 0, 0, 32, 1, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 1,
    // Row 14
    1, 1, 16, 0, 0, 0, 0, 0, 32, 1, 0, 0, 0, 0, 0, 1,
    // Row 15
    1, 1, 16, 0, 0, 0, 0, 0, 32, 1, 0, 0, 0, 1, 1, 1
};

#define COLLISION_LEVEL13_COUNT 256

// Object data for layer 'level13'
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
    20, 0, 52, 53, 54, 55,
    22, 1, 59, 60, 61, 62,
};

#define OBJECT_SPRITE_LEVEL13_COUNT 10

// Object palette data for layer 'level13' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level13[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL13_COUNT 3

// Player start location for layer 'level13'
#define SPAWN_X_LEVEL13 0
#define SPAWN_Y_LEVEL13 13

#endif // TILEMAP_LEVEL13_NES_H
