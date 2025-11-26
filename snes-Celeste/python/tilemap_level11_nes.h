// NES tilemap data for layer 'level11'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL11_NES_H
#define TILEMAP_LEVEL11_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL11_WIDTH 16
#define TILEMAP_LEVEL11_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level11' (GIDs, one byte per entry)
const unsigned char tilemap_level11[] = {
    3, 5, 5, 5, 5, 5, 5, 5, 3, 21, 9, 9, 9, 7, 4, 3,
    21, 9, 0, 0, 0, 16, 9, 9, 2, 6, 9, 9, 12, 7, 3, 3,
    6, 9, 27, 27, 27, 9, 9, 9, 0, 0, 9, 0, 13, 2, 3, 3,
    12, 11, 26, 20, 1, 9, 12, 8, 0, 16, 11, 8, 0, 13, 7, 3,
    9, 18, 3, 3, 6, 8, 13, 0, 0, 0, 13, 0, 27, 27, 7, 3,
    8, 36, 7, 21, 9, 17, 0, 0, 0, 0, 0, 36, 26, 20, 3, 3,
    0, 36, 7, 21, 8, 0, 0, 0, 0, 0, 0, 36, 2, 3, 4, 3,
    0, 36, 2, 6, 0, 0, 0, 0, 0, 0, 0, 13, 9, 2, 5, 3,
    0, 0, 13, 8, 0, 0, 27, 27, 0, 0, 0, 0, 9, 9, 9, 2,
    27, 27, 27, 27, 27, 27, 26, 22, 0, 0, 0, 0, 13, 9, 12, 0,
    20, 19, 19, 19, 19, 19, 6, 0, 0, 0, 0, 0, 0, 9, 9, 17,
    6, 9, 12, 9, 9, 8, 0, 0, 0, 0, 0, 0, 0, 11, 9, 9,
    9, 9, 8, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 23, 31, 9, 8, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 12, 17, 16,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 9, 9, 9, 9, 9
};

#define TILEMAP_LEVEL11_COUNT 256

// Background palette data for layer 'level11' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level11[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL11_COUNT 1

// Sprite palette data for layer 'level11' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level11[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL11_COUNT 4

// Object data for layer 'level11'
const unsigned char object_level11[] = {
    22, 4, 1,
    11, 15, 9,
    23, 6, 14,
    23, 7, 14,
    23, 1, 15,
    23, 2, 15,
};

#define OBJECT_LEVEL11_COUNT 6

// Object sprite data for layer 'level11'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level11[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    11, 0, 30, 31, 0, 0,
    22, 1, 59, 60, 61, 62,
    23, 0, 63, 64, 65, 66,
};

#define OBJECT_SPRITE_LEVEL11_COUNT 10

// Object palette data for layer 'level11' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level11[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL11_COUNT 3

// Player start location for layer 'level11'
#define SPAWN_X_LEVEL11 1
#define SPAWN_Y_LEVEL11 14

#endif // TILEMAP_LEVEL11_NES_H
