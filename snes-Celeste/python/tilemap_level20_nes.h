// NES tilemap data for layer 'level20'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL20_NES_H
#define TILEMAP_LEVEL20_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL20_WIDTH 16
#define TILEMAP_LEVEL20_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level20' (GIDs, one byte per entry)
const unsigned char tilemap_level20[] = {
    // Row 0
    3,
    21,
    0,
    13,
    9,
    12,
    7,
    3,
    3,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    // Row 1
    3,
    6,
    0,
    0,
    0,
    13,
    2,
    5,
    6,
    0,
    0,
    8,
    0,
    0,
    12,
    8,
    // Row 2
    21,
    35,
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
    0,
    // Row 3
    21,
    48,
    49,
    49,
    49,
    49,
    49,
    49,
    49,
    49,
    49,
    50,
    27,
    0,
    0,
    0,
    // Row 4
    3,
    20,
    20,
    20,
    20,
    20,
    20,
    20,
    20,
    20,
    20,
    20,
    1,
    35,
    0,
    0,
    // Row 5
    3,
    4,
    3,
    3,
    3,
    3,
    3,
    5,
    5,
    5,
    5,
    5,
    21,
    35,
    0,
    0,
    // Row 6
    5,
    5,
    5,
    5,
    3,
    4,
    21,
    9,
    8,
    0,
    13,
    11,
    29,
    35,
    0,
    0,
    // Row 7
    20,
    20,
    20,
    1,
    2,
    5,
    6,
    0,
    0,
    0,
    31,
    8,
    0,
    0,
    0,
    0,
    // Row 8
    3,
    3,
    4,
    21,
    12,
    9,
    13,
    0,
    0,
    0,
    13,
    0,
    0,
    0,
    0,
    0,
    // Row 9
    3,
    5,
    3,
    21,
    8,
    0,
    0,
    0,
    0,
    0,
    0,
    16,
    0,
    0,
    0,
    0,
    // Row 10
    21,
    0,
    2,
    6,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    9,
    17,
    0,
    0,
    0,
    // Row 11
    21,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    17,
    0,
    0,
    12,
    9,
    0,
    0,
    0,
    // Row 12
    21,
    17,
    0,
    0,
    0,
    0,
    0,
    0,
    9,
    0,
    0,
    9,
    9,
    8,
    0,
    0,
    // Row 13
    21,
    9,
    8,
    0,
    0,
    0,
    0,
    16,
    9,
    8,
    0,
    9,
    9,
    0,
    0,
    0,
    // Row 14
    21,
    9,
    0,
    0,
    0,
    0,
    0,
    9,
    12,
    0,
    16,
    9,
    11,
    0,
    0,
    0,
    // Row 15
    21,
    12,
    17,
    0,
    0,
    0,
    0,
    9,
    9,
    0,
    9,
    9,
    9,
    17,
    0,
    0
};

#define TILEMAP_LEVEL20_COUNT 256

// Background palette data for layer 'level20' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level20[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL20_COUNT 1

// Sprite palette data for layer 'level20' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level20[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL20_COUNT 4

// Collision data for layer 'level20'
const unsigned char collision_level20[] = {
    // Row 0
    1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    // Row 1
    1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    // Row 2
    1, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0,
    // Row 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 16, 0, 0,
    // Row 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 16, 0, 0,
    // Row 6
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 16, 0, 0,
    // Row 7
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 8
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 9
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 10
    1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 11
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 12
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 13
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 14
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 15
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define COLLISION_LEVEL20_COUNT 256

// Object data for layer 'level20'
const unsigned char object_level20[] = {
    22, 15, 3,
    22, 15, 6,
    22, 7, 7,
    22, 4, 10,
    23, 1, 11,
};

#define OBJECT_LEVEL20_COUNT 5

// Object sprite data for layer 'level20'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level20[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    22, 1, 59, 60, 61, 62,
    23, 0, 63, 64, 65, 66,
};

#define OBJECT_SPRITE_LEVEL20_COUNT 9

// Object palette data for layer 'level20' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level20[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL20_COUNT 3

// Player start location for layer 'level20'
#define SPAWN_X_LEVEL20 1
#define SPAWN_Y_LEVEL20 10

#endif // TILEMAP_LEVEL20_NES_H
