// NES tilemap data for layer 'level30'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL30_NES_H
#define TILEMAP_LEVEL30_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL30_WIDTH 16
#define TILEMAP_LEVEL30_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level30' (GIDs, one byte per entry)
const unsigned char tilemap_level30[] = {
    3, 4, 3, 3, 5, 6, 9, 9, 8, 7, 3, 5, 5, 5, 3, 3,
    3, 3, 5, 6, 34, 34, 11, 9, 0, 2, 6, 12, 9, 9, 7, 4,
    3, 21, 34, 34, 0, 0, 12, 26, 1, 35, 0, 0, 0, 11, 7, 3,
    3, 21, 35, 0, 0, 0, 13, 2, 6, 35, 0, 0, 31, 9, 7, 3,
    3, 21, 35, 0, 0, 0, 0, 34, 34, 0, 0, 13, 12, 13, 7, 3,
    3, 21, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 36, 7, 4,
    4, 21, 27, 27, 27, 27, 0, 0, 0, 0, 0, 0, 0, 36, 2, 3,
    3, 3, 1, 26, 20, 1, 35, 0, 0, 0, 0, 0, 0, 0, 34, 7,
    5, 5, 6, 2, 5, 6, 35, 0, 0, 27, 27, 0, 0, 0, 36, 7,
    34, 34, 34, 34, 34, 34, 0, 27, 27, 26, 1, 27, 0, 0, 36, 7,
    35, 0, 0, 0, 0, 0, 36, 26, 20, 3, 5, 22, 35, 0, 36, 2,
    35, 0, 0, 0, 0, 0, 36, 7, 3, 6, 34, 34, 0, 0, 0, 34,
    27, 27, 27, 27, 0, 0, 36, 2, 6, 34, 0, 16, 0, 0, 0, 0,
    20, 20, 20, 1, 35, 0, 0, 34, 34, 0, 0, 12, 0, 0, 0, 31,
    3, 4, 3, 21, 35, 0, 0, 0, 17, 0, 0, 9, 16, 0, 16, 9,
    3, 3, 4, 21, 35, 0, 0, 16, 12, 0, 16, 9, 11, 9, 12, 9
};

#define TILEMAP_LEVEL30_COUNT 256

// Background palette data for layer 'level30' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level30[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL30_COUNT 1

// Sprite palette data for layer 'level30' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level30[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL30_COUNT 4

// Object data for layer 'level30'
const unsigned char object_level30[] = {
    20, 8, 1,
    8, 12, 2,
    22, 9, 6,
    23, 1, 11,
    22, 13, 12,
    22, 5, 14,
};

#define OBJECT_LEVEL30_COUNT 6

// Object sprite data for layer 'level30'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level30[] = {
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

#define OBJECT_SPRITE_LEVEL30_COUNT 11

// Object palette data for layer 'level30' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level30[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL30_COUNT 3

// Player start location for layer 'level30'
#define SPAWN_X_LEVEL30 1
#define SPAWN_Y_LEVEL30 10

#endif // TILEMAP_LEVEL30_NES_H
