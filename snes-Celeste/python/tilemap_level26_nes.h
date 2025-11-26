// NES tilemap data for layer 'level26'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL26_NES_H
#define TILEMAP_LEVEL26_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL26_WIDTH 16
#define TILEMAP_LEVEL26_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level26' (GIDs, one byte per entry)
const unsigned char tilemap_level26[] = {
    4, 3, 5, 5, 5, 5, 5, 5, 3, 3, 5, 5, 5, 6, 9, 9,
    5, 6, 9, 23, 0, 0, 0, 9, 7, 6, 35, 13, 9, 11, 9, 12,
    9, 9, 12, 8, 0, 0, 27, 13, 29, 35, 0, 16, 9, 30, 8, 0,
    9, 11, 13, 0, 0, 36, 30, 0, 8, 0, 0, 0, 12, 28, 0, 0,
    9, 8, 0, 0, 0, 36, 28, 0, 0, 0, 0, 16, 9, 28, 0, 0,
    12, 0, 0, 0, 0, 36, 28, 0, 0, 0, 0, 0, 13, 28, 25, 0,
    9, 17, 24, 31, 17, 36, 28, 27, 27, 27, 27, 27, 27, 7, 20, 20,
    9, 9, 12, 9, 18, 19, 5, 19, 19, 19, 19, 19, 19, 3, 3, 4,
    8, 0, 13, 0, 0, 11, 9, 9, 9, 9, 8, 0, 36, 7, 3, 3,
    0, 0, 0, 0, 0, 0, 13, 9, 12, 9, 9, 23, 26, 3, 3, 3,
    27, 27, 27, 27, 27, 0, 0, 9, 8, 0, 8, 9, 7, 3, 4, 3,
    20, 20, 20, 20, 1, 27, 27, 10, 35, 0, 0, 13, 2, 5, 3, 3,
    5, 5, 5, 5, 5, 19, 22, 34, 0, 0, 0, 0, 34, 34, 2, 3,
    17, 0, 14, 0, 0, 13, 9, 0, 0, 0, 0, 0, 0, 0, 9, 7,
    20, 20, 1, 17, 0, 11, 9, 17, 0, 0, 0, 0, 0, 16, 11, 2,
    3, 3, 21, 9, 16, 9, 12, 9, 0, 0, 0, 0, 0, 12, 9, 9
};

#define TILEMAP_LEVEL26_COUNT 256

// Background palette data for layer 'level26' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level26[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL26_COUNT 2

// Sprite palette data for layer 'level26' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level26[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL26_COUNT 4

// Object data for layer 'level26'
const unsigned char object_level26[] = {
    20, 15, 5,
    8, 12, 14,
};

#define OBJECT_LEVEL26_COUNT 2

// Object sprite data for layer 'level26'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level26[] = {
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
};

#define OBJECT_SPRITE_LEVEL26_COUNT 9

// Object palette data for layer 'level26' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level26[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL26_COUNT 3

// Player start location for layer 'level26'
#define SPAWN_X_LEVEL26 1
#define SPAWN_Y_LEVEL26 13

#endif // TILEMAP_LEVEL26_NES_H
