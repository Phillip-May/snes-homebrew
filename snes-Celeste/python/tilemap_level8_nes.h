// NES tilemap data for layer 'level8'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL8_NES_H
#define TILEMAP_LEVEL8_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL8_WIDTH 16
#define TILEMAP_LEVEL8_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level8' (GIDs, one byte per entry)
const unsigned char tilemap_level8[] = {
    // Row 0
    21,
    9,
    9,
    9,
    9,
    2,
    5,
    5,
    5,
    3,
    4,
    3,
    3,
    3,
    3,
    3,
    // Row 1
    21,
    9,
    12,
    9,
    9,
    9,
    13,
    13,
    9,
    2,
    5,
    5,
    5,
    5,
    3,
    3,
    // Row 2
    21,
    9,
    9,
    9,
    9,
    8,
    0,
    0,
    13,
    9,
    9,
    9,
    12,
    9,
    7,
    4,
    // Row 3
    21,
    9,
    9,
    9,
    0,
    0,
    0,
    0,
    0,
    9,
    9,
    9,
    9,
    9,
    7,
    3,
    // Row 4
    21,
    13,
    9,
    9,
    23,
    0,
    0,
    0,
    13,
    9,
    9,
    12,
    9,
    9,
    7,
    3,
    // Row 5
    21,
    27,
    9,
    11,
    0,
    0,
    0,
    0,
    31,
    9,
    9,
    9,
    9,
    9,
    7,
    3,
    // Row 6
    3,
    1,
    9,
    9,
    16,
    23,
    0,
    16,
    9,
    9,
    9,
    8,
    0,
    13,
    2,
    5,
    // Row 7
    3,
    6,
    9,
    9,
    9,
    12,
    0,
    9,
    9,
    9,
    17,
    0,
    0,
    0,
    0,
    0,
    // Row 8
    6,
    9,
    9,
    9,
    9,
    9,
    31,
    9,
    9,
    9,
    9,
    0,
    0,
    0,
    0,
    0,
    // Row 9
    9,
    9,
    9,
    12,
    9,
    9,
    9,
    9,
    9,
    9,
    8,
    0,
    0,
    0,
    0,
    0,
    // Row 10
    13,
    9,
    9,
    9,
    9,
    11,
    9,
    9,
    9,
    9,
    23,
    24,
    0,
    0,
    0,
    0,
    // Row 11
    0,
    0,
    13,
    9,
    9,
    9,
    9,
    9,
    8,
    9,
    9,
    8,
    0,
    0,
    0,
    16,
    // Row 12
    0,
    0,
    13,
    9,
    9,
    13,
    8,
    0,
    0,
    24,
    11,
    0,
    0,
    0,
    13,
    9,
    // Row 13
    32,
    0,
    0,
    9,
    12,
    0,
    0,
    0,
    0,
    13,
    9,
    17,
    0,
    0,
    0,
    9,
    // Row 14
    37,
    0,
    31,
    9,
    9,
    0,
    0,
    0,
    0,
    0,
    16,
    9,
    0,
    0,
    16,
    9,
    // Row 15
    20,
    20,
    20,
    1,
    9,
    23,
    0,
    0,
    0,
    0,
    9,
    9,
    17,
    0,
    9,
    12
};

#define TILEMAP_LEVEL8_COUNT 256

// Background palette data for layer 'level8' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level8[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1a, 0x17, 0x27 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL8_COUNT 2

// Sprite palette data for layer 'level8' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level8[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL8_COUNT 4

// Collision data for layer 'level8'
const unsigned char collision_level8[] = {
    // Row 0
    1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    // Row 1
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    // Row 2
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    // Row 3
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    // Row 4
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    // Row 5
    1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    // Row 6
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    // Row 7
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 8
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 10
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 11
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 12
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 14
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 15
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define COLLISION_LEVEL8_COUNT 256

// Object data for layer 'level8'
const unsigned char object_level8[] = {
    22, 6, 4,
    23, 14, 7,
    23, 14, 8,
    18, 12, 12,
    23, 12, 13,
    23, 6, 14,
    23, 7, 14,
    23, 8, 14,
};

#define OBJECT_LEVEL8_COUNT 8

// Object sprite data for layer 'level8'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level8[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    18, 2, 46, 47, 48, 49,
    22, 1, 59, 60, 61, 62,
    23, 0, 63, 64, 65, 66,
};

#define OBJECT_SPRITE_LEVEL8_COUNT 10

// Object palette data for layer 'level8' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level8[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL8_COUNT 3

// Player start location for layer 'level8'
#define SPAWN_X_LEVEL8 1
#define SPAWN_Y_LEVEL8 14

#endif // TILEMAP_LEVEL8_NES_H
