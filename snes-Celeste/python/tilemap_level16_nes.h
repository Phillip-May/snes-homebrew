// NES tilemap data for layer 'level16'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL16_NES_H
#define TILEMAP_LEVEL16_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL16_WIDTH 16
#define TILEMAP_LEVEL16_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level16' (GIDs, one byte per entry)
const unsigned char tilemap_level16[] = {
    3, 3, 4, 21, 9, 12, 9, 2, 5, 5, 5, 5, 5, 5, 3, 4,
    5, 5, 5, 6, 9, 9, 9, 9, 9, 9, 11, 9, 9, 10, 2, 3,
    9, 9, 9, 9, 9, 8, 0, 13, 9, 0, 9, 9, 12, 9, 9, 2,
    11, 9, 8, 0, 0, 0, 0, 0, 8, 0, 13, 9, 9, 9, 8, 0,
    9, 12, 0, 27, 27, 0, 0, 0, 0, 0, 0, 9, 8, 13, 0, 0,
    9, 8, 36, 26, 1, 0, 0, 0, 0, 0, 27, 9, 23, 0, 0, 0,
    0, 0, 36, 2, 6, 27, 27, 27, 27, 27, 30, 9, 8, 0, 0, 36,
    0, 0, 36, 18, 19, 19, 19, 19, 19, 22, 28, 12, 0, 0, 0, 0,
    0, 0, 36, 10, 34, 34, 34, 34, 34, 34, 28, 9, 0, 0, 0, 0,
    0, 0, 0, 34, 0, 0, 0, 0, 0, 0, 28, 8, 0, 0, 0, 36,
    0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 29, 0, 0, 0, 0, 0,
    0, 0, 0, 27, 0, 0, 16, 30, 0, 0, 34, 0, 0, 0, 0, 0,
    0, 0, 0, 30, 17, 0, 12, 28, 0, 0, 0, 0, 0, 0, 0, 0,
    26, 1, 0, 29, 9, 8, 9, 28, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 6, 17, 30, 11, 0, 9, 29, 0, 0, 0, 0, 0, 0, 0, 0,
    20, 1, 12, 28, 9, 0, 9, 30, 0, 0, 0, 0, 0, 0, 0, 0
};

#define TILEMAP_LEVEL16_COUNT 256

// Background palette data for layer 'level16' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level16[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL16_COUNT 1

// Sprite palette data for layer 'level16' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level16[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL16_COUNT 4

// Object data for layer 'level16'
const unsigned char object_level16[] = {
    18, 7, 4,
    22, 14, 4,
    23, 7, 5,
    23, 15, 7,
    23, 15, 8,
    18, 5, 14,
    23, 5, 15,
};

#define OBJECT_LEVEL16_COUNT 7

// Object sprite data for layer 'level16'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level16[] = {
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

#define OBJECT_SPRITE_LEVEL16_COUNT 10

// Object palette data for layer 'level16' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level16[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL16_COUNT 3

// Player start location for layer 'level16'
#define SPAWN_X_LEVEL16 0
#define SPAWN_Y_LEVEL16 12

#endif // TILEMAP_LEVEL16_NES_H
