// NES tilemap data for layer 'level28'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL28_NES_H
#define TILEMAP_LEVEL28_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL28_WIDTH 16
#define TILEMAP_LEVEL28_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level28' (GIDs, one byte per entry)
const unsigned char tilemap_level28[] = {
    // Row 0
    5,
    5,
    5,
    5,
    5,
    3,
    21,
    9,
    9,
    0,
    0,
    36,
    7,
    3,
    3,
    3,
    // Row 1
    8,
    0,
    0,
    16,
    9,
    7,
    21,
    12,
    8,
    0,
    0,
    36,
    7,
    5,
    5,
    5,
    // Row 2
    19,
    19,
    19,
    22,
    9,
    7,
    21,
    9,
    0,
    0,
    0,
    36,
    28,
    9,
    13,
    13,
    // Row 3
    9,
    9,
    12,
    9,
    9,
    7,
    3,
    20,
    1,
    35,
    0,
    36,
    29,
    8,
    0,
    0,
    // Row 4
    12,
    9,
    8,
    11,
    8,
    7,
    4,
    3,
    21,
    35,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 5
    9,
    8,
    0,
    0,
    0,
    7,
    3,
    3,
    21,
    35,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 6
    9,
    27,
    27,
    27,
    27,
    7,
    3,
    4,
    21,
    35,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 7
    9,
    18,
    19,
    19,
    19,
    5,
    5,
    3,
    21,
    35,
    0,
    0,
    27,
    0,
    0,
    0,
    // Row 8
    8,
    34,
    34,
    34,
    34,
    34,
    36,
    7,
    21,
    35,
    0,
    36,
    30,
    35,
    0,
    0,
    // Row 9
    0,
    0,
    0,
    0,
    0,
    0,
    36,
    7,
    21,
    35,
    0,
    36,
    28,
    35,
    0,
    0,
    // Row 10
    0,
    0,
    0,
    0,
    0,
    0,
    36,
    7,
    21,
    27,
    27,
    27,
    28,
    35,
    0,
    0,
    // Row 11
    27,
    27,
    27,
    27,
    17,
    0,
    27,
    7,
    3,
    20,
    20,
    20,
    6,
    35,
    0,
    0,
    // Row 12
    20,
    20,
    20,
    1,
    9,
    12,
    18,
    5,
    5,
    5,
    5,
    6,
    35,
    0,
    0,
    0,
    // Row 13
    3,
    3,
    3,
    21,
    0,
    9,
    9,
    0,
    16,
    11,
    9,
    13,
    0,
    0,
    0,
    16,
    // Row 14
    3,
    4,
    3,
    21,
    0,
    13,
    9,
    9,
    9,
    12,
    8,
    0,
    0,
    0,
    0,
    9,
    // Row 15
    3,
    3,
    3,
    21,
    0,
    0,
    9,
    12,
    9,
    0,
    0,
    0,
    0,
    0,
    16,
    9
};

#define TILEMAP_LEVEL28_COUNT 256

// Background palette data for layer 'level28' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level28[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL28_COUNT 1

// Sprite palette data for layer 'level28' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level28[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL28_COUNT 4

// Collision data for layer 'level28'
const unsigned char collision_level28[] = {
    // Row 0
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 32, 1, 1, 1, 1,
    // Row 1
    0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 32, 1, 1, 1, 1,
    // Row 2
    1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 32, 1, 0, 0, 0,
    // Row 3
    0, 0, 0, 0, 0, 1, 1, 1, 1, 16, 0, 32, 1, 0, 0, 0,
    // Row 4
    0, 0, 0, 0, 0, 1, 1, 1, 1, 16, 0, 0, 0, 0, 0, 0,
    // Row 5
    0, 0, 0, 0, 0, 1, 1, 1, 1, 16, 0, 0, 0, 0, 0, 0,
    // Row 6
    0, 4, 4, 4, 4, 1, 1, 1, 1, 16, 0, 0, 0, 0, 0, 0,
    // Row 7
    0, 1, 1, 1, 1, 1, 1, 1, 1, 16, 0, 0, 4, 0, 0, 0,
    // Row 8
    0, 8, 8, 8, 8, 8, 32, 1, 1, 16, 0, 32, 1, 16, 0, 0,
    // Row 9
    0, 0, 0, 0, 0, 0, 32, 1, 1, 16, 0, 32, 1, 16, 0, 0,
    // Row 10
    0, 0, 0, 0, 0, 0, 32, 1, 1, 4, 4, 4, 1, 16, 0, 0,
    // Row 11
    4, 4, 4, 4, 0, 0, 4, 1, 1, 1, 1, 1, 1, 16, 0, 0,
    // Row 12
    1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 16, 0, 0, 0,
    // Row 13
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 14
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 15
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define COLLISION_LEVEL28_COUNT 256

// Object data for layer 'level28'
const unsigned char object_level28[] = {
    20, 8, 2,
    8, 14, 3,
    23, 10, 9,
    22, 13, 13,
    23, 4, 15,
    23, 5, 15,
};

#define OBJECT_LEVEL28_COUNT 6

// Object sprite data for layer 'level28'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level28[] = {
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
    23, 0, 63, 64, 65, 66,
};

#define OBJECT_SPRITE_LEVEL28_COUNT 11

// Object palette data for layer 'level28' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level28[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL28_COUNT 3

// Player start location for layer 'level28'
#define SPAWN_X_LEVEL28 1
#define SPAWN_Y_LEVEL28 1

#endif // TILEMAP_LEVEL28_NES_H
