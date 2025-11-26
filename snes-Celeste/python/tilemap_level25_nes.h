// NES tilemap data for layer 'level25'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL25_NES_H
#define TILEMAP_LEVEL25_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL25_WIDTH 16
#define TILEMAP_LEVEL25_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level25' (GIDs, one byte per entry)
const unsigned char tilemap_level25[] = {
    // Row 0
    3,
    3,
    3,
    3,
    3,
    21,
    35,
    34,
    34,
    34,
    2,
    5,
    5,
    3,
    21,
    0,
    // Row 1
    3,
    3,
    3,
    4,
    3,
    21,
    35,
    0,
    0,
    0,
    34,
    34,
    34,
    7,
    21,
    0,
    // Row 2
    3,
    3,
    3,
    3,
    3,
    21,
    35,
    0,
    0,
    0,
    0,
    0,
    13,
    7,
    21,
    23,
    // Row 3
    3,
    4,
    3,
    5,
    5,
    6,
    35,
    0,
    0,
    0,
    27,
    0,
    9,
    7,
    21,
    8,
    // Row 4
    3,
    3,
    21,
    26,
    20,
    1,
    35,
    0,
    16,
    23,
    30,
    35,
    13,
    7,
    21,
    12,
    // Row 5
    3,
    3,
    21,
    7,
    4,
    21,
    35,
    0,
    13,
    12,
    28,
    35,
    0,
    7,
    21,
    8,
    // Row 6
    5,
    5,
    6,
    2,
    5,
    21,
    35,
    0,
    0,
    9,
    28,
    35,
    0,
    2,
    6,
    0,
    // Row 7
    9,
    0,
    0,
    0,
    13,
    28,
    27,
    30,
    0,
    13,
    28,
    35,
    0,
    11,
    11,
    17,
    // Row 8
    8,
    0,
    27,
    0,
    0,
    2,
    19,
    21,
    35,
    0,
    2,
    19,
    19,
    19,
    19,
    19,
    // Row 9
    0,
    36,
    30,
    35,
    0,
    13,
    12,
    28,
    35,
    0,
    0,
    0,
    13,
    9,
    17,
    36,
    // Row 10
    0,
    36,
    28,
    35,
    0,
    0,
    9,
    28,
    27,
    27,
    27,
    27,
    0,
    9,
    12,
    36,
    // Row 11
    0,
    36,
    29,
    35,
    0,
    0,
    13,
    2,
    19,
    19,
    19,
    22,
    0,
    9,
    8,
    36,
    // Row 12
    17,
    0,
    34,
    0,
    0,
    0,
    0,
    34,
    34,
    34,
    34,
    34,
    0,
    13,
    0,
    36,
    // Row 13
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
    0,
    0,
    0,
    0,
    0,
    36,
    // Row 14
    11,
    23,
    24,
    16,
    26,
    20,
    1,
    27,
    27,
    27,
    27,
    27,
    0,
    0,
    0,
    36,
    // Row 15
    9,
    12,
    9,
    9,
    7,
    3,
    3,
    20,
    20,
    20,
    20,
    1,
    0,
    0,
    0,
    36
};

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

// Collision data for layer 'level25'
const unsigned char collision_level25[] = {
    // Row 0
    1, 1, 1, 1, 1, 1, 16, 8, 8, 8, 1, 1, 1, 1, 1, 0,
    // Row 1
    1, 1, 1, 1, 1, 1, 16, 0, 0, 0, 8, 8, 8, 1, 1, 0,
    // Row 2
    1, 1, 1, 1, 1, 1, 16, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    // Row 3
    1, 1, 1, 1, 1, 1, 16, 0, 0, 0, 4, 0, 0, 1, 1, 0,
    // Row 4
    1, 1, 1, 1, 1, 1, 16, 0, 0, 0, 1, 16, 0, 1, 1, 0,
    // Row 5
    1, 1, 1, 1, 1, 1, 16, 0, 0, 0, 1, 16, 0, 1, 1, 0,
    // Row 6
    1, 1, 1, 1, 1, 1, 16, 0, 0, 0, 1, 16, 0, 1, 1, 0,
    // Row 7
    0, 0, 0, 0, 0, 1, 4, 1, 0, 0, 1, 16, 0, 0, 0, 0,
    // Row 8
    0, 0, 4, 0, 0, 1, 1, 1, 16, 0, 1, 1, 1, 1, 1, 1,
    // Row 9
    0, 32, 1, 16, 0, 0, 0, 1, 16, 0, 0, 0, 0, 0, 0, 32,
    // Row 10
    0, 32, 1, 16, 0, 0, 0, 1, 4, 4, 4, 4, 0, 0, 0, 32,
    // Row 11
    0, 32, 1, 16, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 32,
    // Row 12
    0, 0, 8, 0, 0, 0, 0, 8, 8, 8, 8, 8, 0, 0, 0, 32,
    // Row 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32,
    // Row 14
    0, 0, 0, 0, 1, 1, 1, 4, 4, 4, 4, 4, 0, 0, 0, 32,
    // Row 15
    0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 32
};

#define COLLISION_LEVEL25_COUNT 256

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
    20, 0, 52, 53, 54, 55,
    23, 0, 63, 64, 65, 66,
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
