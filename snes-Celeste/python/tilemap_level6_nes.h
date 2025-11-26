// NES tilemap data for layer 'level6'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL6_NES_H
#define TILEMAP_LEVEL6_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL6_WIDTH 16
#define TILEMAP_LEVEL6_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level6' (GIDs, one byte per entry)
const unsigned char tilemap_level6[] = {
    // Row 0
    3,
    3,
    3,
    4,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    21,
    9,
    9,
    7,
    3,
    // Row 1
    3,
    3,
    3,
    3,
    3,
    4,
    3,
    3,
    5,
    5,
    5,
    6,
    9,
    9,
    7,
    3,
    // Row 2
    3,
    3,
    5,
    5,
    5,
    5,
    5,
    6,
    26,
    20,
    20,
    1,
    9,
    9,
    7,
    3,
    // Row 3
    3,
    21,
    26,
    20,
    1,
    10,
    26,
    1,
    2,
    5,
    5,
    6,
    9,
    9,
    7,
    3,
    // Row 4
    5,
    6,
    7,
    3,
    21,
    11,
    2,
    6,
    10,
    9,
    9,
    9,
    9,
    12,
    7,
    3,
    // Row 5
    9,
    9,
    2,
    5,
    6,
    9,
    9,
    8,
    0,
    13,
    0,
    13,
    9,
    9,
    7,
    3,
    // Row 6
    9,
    9,
    34,
    34,
    34,
    9,
    9,
    0,
    0,
    0,
    0,
    0,
    13,
    26,
    3,
    4,
    // Row 7
    12,
    9,
    17,
    0,
    0,
    0,
    13,
    8,
    0,
    0,
    0,
    0,
    0,
    2,
    5,
    5,
    // Row 8
    9,
    9,
    9,
    8,
    0,
    0,
    0,
    0,
    0,
    0,
    16,
    23,
    31,
    9,
    9,
    0,
    // Row 9
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
    13,
    9,
    9,
    9,
    8,
    0,
    // Row 10
    9,
    0,
    0,
    0,
    17,
    9,
    17,
    0,
    0,
    0,
    0,
    0,
    9,
    9,
    0,
    0,
    // Row 11
    9,
    0,
    16,
    9,
    9,
    11,
    9,
    24,
    0,
    0,
    0,
    16,
    9,
    8,
    0,
    0,
    // Row 12
    20,
    20,
    1,
    9,
    9,
    9,
    9,
    9,
    17,
    0,
    24,
    9,
    12,
    9,
    14,
    0,
    // Row 13
    3,
    3,
    3,
    20,
    20,
    20,
    1,
    9,
    9,
    9,
    9,
    11,
    9,
    9,
    26,
    20,
    // Row 14
    3,
    4,
    3,
    3,
    3,
    3,
    21,
    13,
    9,
    9,
    26,
    20,
    20,
    20,
    3,
    3,
    // Row 15
    3,
    3,
    3,
    3,
    4,
    3,
    21,
    0,
    0,
    13,
    7,
    3,
    3,
    3,
    4,
    3
};

#define TILEMAP_LEVEL6_COUNT 256

// Background palette data for layer 'level6' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level6[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL6_COUNT 2

// Sprite palette data for layer 'level6' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level6[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL6_COUNT 4

// Collision data for layer 'level6'
const unsigned char collision_level6[] = {
    // Row 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1,
    // Row 1
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1,
    // Row 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1,
    // Row 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1,
    // Row 4
    1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1,
    // Row 5
    0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    // Row 6
    0, 0, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    // Row 7
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    // Row 8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 10
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 11
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 12
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 13
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    // Row 14
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    // Row 15
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1
};

#define COLLISION_LEVEL6_COUNT 256

// Object data for layer 'level6'
const unsigned char object_level6[] = {
    22, 9, 8,
};

#define OBJECT_LEVEL6_COUNT 1

// Object sprite data for layer 'level6'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level6[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    22, 1, 59, 60, 61, 62,
};

#define OBJECT_SPRITE_LEVEL6_COUNT 8

// Object palette data for layer 'level6' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level6[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL6_COUNT 3

// Player start location for layer 'level6'
#define SPAWN_X_LEVEL6 1
#define SPAWN_Y_LEVEL6 11

#endif // TILEMAP_LEVEL6_NES_H
