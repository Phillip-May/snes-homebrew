// NES tilemap data for layer 'level5'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL5_NES_H
#define TILEMAP_LEVEL5_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL5_WIDTH 16
#define TILEMAP_LEVEL5_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level5' (GIDs, one byte per entry)
const unsigned char tilemap_level5[] = {
    // Row 0
    5,
    6,
    0,
    0,
    0,
    7,
    5,
    5,
    5,
    6,
    2,
    5,
    5,
    5,
    3,
    3,
    // Row 1
    9,
    11,
    0,
    0,
    0,
    29,
    9,
    8,
    0,
    0,
    0,
    0,
    13,
    9,
    2,
    4,
    // Row 2
    12,
    9,
    17,
    25,
    0,
    16,
    9,
    0,
    0,
    0,
    0,
    0,
    0,
    9,
    0,
    7,
    // Row 3
    13,
    9,
    9,
    18,
    19,
    22,
    8,
    0,
    0,
    0,
    0,
    0,
    0,
    9,
    17,
    7,
    // Row 4
    0,
    11,
    9,
    9,
    9,
    8,
    0,
    0,
    0,
    0,
    27,
    27,
    16,
    9,
    9,
    2,
    // Row 5
    13,
    9,
    9,
    8,
    0,
    0,
    0,
    0,
    0,
    0,
    26,
    1,
    9,
    12,
    9,
    8,
    // Row 6
    0,
    13,
    0,
    0,
    0,
    0,
    27,
    27,
    0,
    0,
    7,
    21,
    11,
    9,
    8,
    0,
    // Row 7
    0,
    0,
    0,
    0,
    0,
    0,
    18,
    22,
    0,
    16,
    7,
    21,
    9,
    9,
    0,
    0,
    // Row 8
    0,
    0,
    0,
    0,
    0,
    0,
    10,
    12,
    9,
    9,
    2,
    3,
    1,
    0,
    0,
    0,
    // Row 9
    0,
    0,
    0,
    0,
    0,
    0,
    30,
    9,
    9,
    13,
    9,
    2,
    6,
    17,
    0,
    0,
    // Row 10
    0,
    0,
    0,
    0,
    0,
    0,
    29,
    9,
    0,
    0,
    0,
    13,
    9,
    9,
    17,
    0,
    // Row 11
    0,
    0,
    15,
    0,
    0,
    0,
    10,
    8,
    0,
    0,
    0,
    0,
    12,
    9,
    0,
    0,
    // Row 12
    19,
    19,
    19,
    22,
    0,
    0,
    10,
    0,
    0,
    0,
    0,
    14,
    13,
    9,
    23,
    0,
    // Row 13
    13,
    9,
    10,
    17,
    0,
    16,
    10,
    0,
    0,
    16,
    0,
    18,
    19,
    19,
    19,
    19,
    // Row 14
    0,
    12,
    10,
    9,
    17,
    9,
    30,
    0,
    0,
    9,
    23,
    31,
    10,
    9,
    9,
    9,
    // Row 15
    0,
    13,
    10,
    9,
    11,
    9,
    28,
    0,
    16,
    9,
    9,
    9,
    10,
    9,
    9,
    9
};

#define TILEMAP_LEVEL5_COUNT 256

// Background palette data for layer 'level5' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level5[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL5_COUNT 2

// Sprite palette data for layer 'level5' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level5[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL5_COUNT 4

// Collision data for layer 'level5'
const unsigned char collision_level5[] = {
    // Row 0
    1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    // Row 1
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    // Row 2
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    // Row 3
    0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    // Row 4
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 1,
    // Row 5
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    // Row 6
    0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 1, 1, 0, 0, 0, 0,
    // Row 7
    0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0,
    // Row 8
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
    // Row 9
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0,
    // Row 10
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 11
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 12
    1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 13
    0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    // Row 14
    0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
    // Row 15
    0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0
};

#define COLLISION_LEVEL5_COUNT 256

// Object data for layer 'level5'
const unsigned char object_level5[] = {
    20, 15, 12,
    8, 7, 14,
};

#define OBJECT_LEVEL5_COUNT 2

// Object sprite data for layer 'level5'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level5[] = {
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
};

#define OBJECT_SPRITE_LEVEL5_COUNT 9

// Object palette data for layer 'level5' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level5[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL5_COUNT 3

// Player start location for layer 'level5'
#define SPAWN_X_LEVEL5 1
#define SPAWN_Y_LEVEL5 11

#endif // TILEMAP_LEVEL5_NES_H
