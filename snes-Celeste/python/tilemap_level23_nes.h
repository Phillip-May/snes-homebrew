// NES tilemap data for layer 'level23'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL23_NES_H
#define TILEMAP_LEVEL23_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL23_WIDTH 16
#define TILEMAP_LEVEL23_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level23' (GIDs, one byte per entry)
const unsigned char tilemap_level23[] = {
    // Row 0
    3,
    4,
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
    7,
    3,
    3,
    3,
    // Row 1
    3,
    3,
    3,
    4,
    21,
    35,
    16,
    0,
    0,
    0,
    0,
    0,
    7,
    3,
    4,
    3,
    // Row 2
    3,
    3,
    3,
    5,
    6,
    35,
    9,
    8,
    0,
    27,
    27,
    10,
    7,
    3,
    3,
    3,
    // Row 3
    3,
    3,
    6,
    9,
    9,
    9,
    9,
    11,
    16,
    26,
    20,
    20,
    3,
    3,
    3,
    3,
    // Row 4
    4,
    21,
    9,
    9,
    9,
    12,
    9,
    9,
    9,
    2,
    5,
    5,
    3,
    4,
    3,
    3,
    // Row 5
    3,
    21,
    9,
    9,
    11,
    13,
    0,
    13,
    9,
    9,
    8,
    0,
    2,
    5,
    5,
    5,
    // Row 6
    3,
    21,
    12,
    9,
    8,
    0,
    0,
    0,
    0,
    13,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 7
    3,
    21,
    9,
    9,
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
    // Row 8
    3,
    21,
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
    // Row 9
    4,
    21,
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
    // Row 10
    3,
    21,
    0,
    0,
    0,
    0,
    0,
    0,
    17,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    // Row 11
    5,
    6,
    0,
    0,
    0,
    0,
    0,
    0,
    9,
    8,
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
    0,
    0,
    0,
    0,
    0,
    9,
    0,
    0,
    24,
    31,
    0,
    0,
    0,
    // Row 13
    0,
    0,
    0,
    15,
    0,
    0,
    0,
    16,
    9,
    0,
    0,
    13,
    11,
    0,
    0,
    0,
    // Row 14
    0,
    0,
    0,
    26,
    22,
    0,
    0,
    12,
    9,
    17,
    0,
    0,
    9,
    0,
    0,
    0,
    // Row 15
    0,
    0,
    0,
    28,
    39,
    45,
    0,
    9,
    9,
    9,
    0,
    0,
    9,
    17,
    0,
    0
};

#define TILEMAP_LEVEL23_COUNT 256

// Background palette data for layer 'level23' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level23[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL23_COUNT 2

// Sprite palette data for layer 'level23' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level23[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL23_COUNT 4

// Collision data for layer 'level23'
const unsigned char collision_level23[] = {
    // Row 0
    1, 1, 1, 1, 1, 16, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    // Row 1
    1, 1, 1, 1, 1, 16, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    // Row 2
    1, 1, 1, 1, 1, 16, 0, 0, 0, 4, 4, 1, 1, 1, 1, 1,
    // Row 3
    1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    // Row 4
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    // Row 5
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    // Row 6
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 7
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 8
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 9
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 10
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 11
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 12
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 14
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Row 15
    0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define COLLISION_LEVEL23_COUNT 256

// Object data for layer 'level23'
const unsigned char object_level23[] = {
    28, 14, 14,
};

#define OBJECT_LEVEL23_COUNT 1

// Object sprite data for layer 'level23'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level23[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    28, 1, 81, 82, 83, 84,
};

#define OBJECT_SPRITE_LEVEL23_COUNT 8

// Object palette data for layer 'level23' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level23[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL23_COUNT 3

// Player start location for layer 'level23'
#define SPAWN_X_LEVEL23 4
#define SPAWN_Y_LEVEL23 13

#endif // TILEMAP_LEVEL23_NES_H
