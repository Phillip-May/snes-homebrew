// NES tilemap data for layer 'level10'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL10_NES_H
#define TILEMAP_LEVEL10_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL10_WIDTH 16
#define TILEMAP_LEVEL10_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level10' (GIDs, one byte per entry)
const unsigned char tilemap_level10[] = {
    // Row 0
    3,
    3,
    21,
    9,
    9,
    9,
    9,
    7,
    3,
    5,
    5,
    5,
    5,
    3,
    4,
    3,
    // Row 1
    3,
    4,
    21,
    9,
    12,
    9,
    9,
    2,
    6,
    12,
    9,
    8,
    0,
    2,
    5,
    3,
    // Row 2
    3,
    3,
    21,
    9,
    9,
    9,
    9,
    10,
    9,
    8,
    13,
    0,
    0,
    0,
    13,
    2,
    // Row 3
    3,
    3,
    21,
    9,
    8,
    0,
    9,
    10,
    23,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 4
    3,
    3,
    21,
    30,
    0,
    0,
    13,
    10,
    8,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 5
    4,
    3,
    21,
    29,
    0,
    0,
    0,
    8,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    16,
    // Row 6
    5,
    5,
    6,
    10,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    30,
    9,
    9,
    // Row 7
    19,
    19,
    19,
    22,
    27,
    27,
    0,
    0,
    0,
    0,
    0,
    0,
    27,
    28,
    9,
    12,
    // Row 8
    9,
    9,
    9,
    18,
    22,
    10,
    0,
    0,
    0,
    0,
    0,
    0,
    10,
    28,
    9,
    9,
    // Row 9
    9,
    12,
    11,
    8,
    13,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    13,
    29,
    11,
    9,
    // Row 10
    9,
    9,
    9,
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
    13,
    9,
    9,
    // Row 11
    9,
    8,
    13,
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
    9,
    13,
    // Row 12
    13,
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
    13,
    9,
    0,
    // Row 13
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
    0,
    0,
    8,
    0,
    // Row 14
    0,
    30,
    17,
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
    0,
    // Row 15
    16,
    28,
    12,
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
    0
};

#define TILEMAP_LEVEL10_COUNT 256

// Background palette data for layer 'level10' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level10[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL10_COUNT 1

// Sprite palette data for layer 'level10' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level10[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL10_COUNT 4

// Collision data for layer 'level10'
const unsigned char collision_level10[] = {
    // Row 0
    1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    // Row 1
    1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1,
    // Row 2
    1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
    // Row 3
    1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 4
    1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 5
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 6
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    // Row 7
    1, 1, 1, 1, 4, 4, 0, 0, 0, 0, 0, 0, 4, 1, 0, 0,
    // Row 8
    0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
    // Row 9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    // Row 10
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 11
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 12
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 14
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 15
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define COLLISION_LEVEL10_COUNT 256

// Object data for layer 'level10'
const unsigned char object_level10[] = {
    23, 5, 14,
    23, 6, 14,
    23, 10, 14,
    23, 11, 14,
};

#define OBJECT_LEVEL10_COUNT 4

// Object sprite data for layer 'level10'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level10[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    23, 0, 63, 64, 65, 66,
};

#define OBJECT_SPRITE_LEVEL10_COUNT 8

// Object palette data for layer 'level10' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level10[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL10_COUNT 3

// Player start location for layer 'level10'
#define SPAWN_X_LEVEL10 1
#define SPAWN_Y_LEVEL10 13

#endif // TILEMAP_LEVEL10_NES_H
