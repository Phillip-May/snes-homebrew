// NES tilemap data for layer 'level21'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL21_NES_H
#define TILEMAP_LEVEL21_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL21_WIDTH 16
#define TILEMAP_LEVEL21_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level21' (GIDs, one byte per entry)
const unsigned char tilemap_level21[] = {
    // Row 0
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
    8,
    0,
    2,
    5,
    5,
    3,
    // Row 1
    0,
    0,
    0,
    0,
    0,
    0,
    27,
    0,
    0,
    13,
    0,
    0,
    0,
    13,
    9,
    2,
    // Row 2
    0,
    0,
    0,
    0,
    0,
    36,
    10,
    35,
    0,
    0,
    0,
    0,
    0,
    0,
    13,
    9,
    // Row 3
    0,
    0,
    0,
    27,
    0,
    0,
    34,
    0,
    36,
    10,
    35,
    0,
    0,
    0,
    0,
    9,
    // Row 4
    0,
    0,
    36,
    10,
    35,
    0,
    0,
    0,
    0,
    34,
    0,
    0,
    0,
    0,
    0,
    13,
    // Row 5
    0,
    0,
    0,
    34,
    0,
    0,
    0,
    27,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 6
    0,
    0,
    0,
    0,
    0,
    0,
    36,
    10,
    35,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 7
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    34,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 8
    0,
    10,
    27,
    27,
    27,
    27,
    27,
    27,
    27,
    27,
    27,
    27,
    27,
    27,
    0,
    0,
    // Row 9
    0,
    18,
    19,
    19,
    19,
    19,
    19,
    19,
    19,
    19,
    19,
    19,
    19,
    22,
    35,
    0,
    // Row 10
    0,
    10,
    9,
    12,
    9,
    9,
    9,
    10,
    9,
    9,
    9,
    9,
    9,
    30,
    35,
    0,
    // Row 11
    0,
    34,
    13,
    9,
    9,
    11,
    9,
    34,
    13,
    9,
    12,
    13,
    9,
    29,
    35,
    0,
    // Row 12
    0,
    0,
    0,
    8,
    0,
    13,
    9,
    0,
    0,
    9,
    0,
    0,
    9,
    9,
    0,
    0,
    // Row 13
    0,
    24,
    31,
    0,
    0,
    9,
    9,
    16,
    9,
    9,
    17,
    0,
    9,
    8,
    0,
    0,
    // Row 14
    0,
    13,
    9,
    9,
    9,
    8,
    13,
    9,
    9,
    12,
    9,
    9,
    9,
    0,
    0,
    0,
    // Row 15
    0,
    0,
    0,
    12,
    9,
    0,
    0,
    0,
    13,
    9,
    11,
    9,
    0,
    0,
    0,
    0
};

#define TILEMAP_LEVEL21_COUNT 256

// Background palette data for layer 'level21' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level21[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL21_COUNT 1

// Sprite palette data for layer 'level21' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level21[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL21_COUNT 4

// Collision data for layer 'level21'
const unsigned char collision_level21[] = {
    // Row 0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    // Row 1
    0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    // Row 2
    0, 0, 0, 0, 0, 32, 1, 16, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 3
    0, 0, 0, 4, 0, 0, 8, 0, 32, 1, 16, 0, 0, 0, 0, 0,
    // Row 4
    0, 0, 32, 1, 16, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
    // Row 5
    0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 6
    0, 0, 0, 0, 0, 0, 32, 1, 16, 0, 0, 0, 0, 0, 0, 0,
    // Row 7
    0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 8
    0, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0,
    // Row 9
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 16, 0,
    // Row 10
    0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 16, 0,
    // Row 11
    0, 8, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 1, 16, 0,
    // Row 12
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 14
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define COLLISION_LEVEL21_COUNT 256

// Object data for layer 'level21'
const unsigned char object_level21[] = {
    18, 9, 2,
    22, 14, 3,
    22, 11, 6,
    22, 15, 8,
    22, 4, 12,
    22, 10, 12,
    22, 14, 13,
};

#define OBJECT_LEVEL21_COUNT 7

// Object sprite data for layer 'level21'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level21[] = {
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
};

#define OBJECT_SPRITE_LEVEL21_COUNT 9

// Object palette data for layer 'level21' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level21[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL21_COUNT 3

// Player start location for layer 'level21'
#define SPAWN_X_LEVEL21 1
#define SPAWN_Y_LEVEL21 7

#endif // TILEMAP_LEVEL21_NES_H
