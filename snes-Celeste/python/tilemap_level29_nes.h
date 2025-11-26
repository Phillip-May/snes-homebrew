// NES tilemap data for layer 'level29'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL29_NES_H
#define TILEMAP_LEVEL29_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL29_WIDTH 16
#define TILEMAP_LEVEL29_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level29' (GIDs, one byte per entry)
const unsigned char tilemap_level29[] = {
    3, 4, 3, 3, 3, 3, 3, 4, 4, 3, 3, 3, 21, 12, 9, 7,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 21, 11, 9, 2,
    0, 9, 8, 0, 13, 9, 12, 9, 9, 11, 9, 7, 21, 0, 13, 12,
    0, 11, 0, 27, 0, 8, 13, 9, 8, 27, 13, 2, 6, 0, 16, 9,
    0, 13, 36, 10, 35, 16, 0, 9, 36, 10, 35, 0, 27, 0, 0, 9,
    0, 0, 0, 34, 0, 13, 9, 9, 0, 34, 0, 36, 10, 35, 27, 13,
    0, 0, 0, 0, 0, 0, 27, 11, 23, 24, 27, 0, 34, 36, 10, 0,
    0, 0, 0, 0, 0, 36, 10, 9, 9, 12, 30, 35, 0, 0, 34, 0,
    0, 27, 0, 0, 0, 0, 34, 13, 9, 9, 29, 35, 0, 0, 0, 0,
    36, 10, 35, 27, 0, 0, 0, 27, 0, 8, 34, 0, 0, 0, 0, 16,
    0, 34, 36, 10, 35, 0, 36, 30, 35, 0, 0, 0, 0, 0, 0, 9,
    0, 0, 0, 34, 0, 0, 36, 28, 35, 0, 0, 0, 0, 24, 31, 9,
    1, 17, 0, 0, 0, 0, 36, 29, 35, 0, 0, 0, 0, 13, 11, 9,
    21, 9, 0, 0, 0, 0, 0, 34, 0, 0, 0, 17, 16, 9, 12, 9,
    21, 12, 17, 0, 0, 0, 0, 0, 0, 0, 16, 9, 9, 9, 9, 11,
    21, 9, 11, 30, 17, 0, 0, 0, 0, 13, 9, 9, 12, 9, 9, 9
};

#define TILEMAP_LEVEL29_COUNT 256

// Background palette data for layer 'level29' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level29[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL29_COUNT 1

// Sprite palette data for layer 'level29' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level29[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL29_COUNT 4

// Object data for layer 'level29'
const unsigned char object_level29[] = {
    26, 0, 2,
    22, 6, 4,
    22, 15, 6,
    22, 12, 7,
    18, 3, 14,
};

#define OBJECT_LEVEL29_COUNT 5

// Object sprite data for layer 'level29'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level29[] = {
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
    26, 1, 75, 76, 77, 78,
};

#define OBJECT_SPRITE_LEVEL29_COUNT 10

// Object palette data for layer 'level29' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level29[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL29_COUNT 3

// Player start location for layer 'level29'
#define SPAWN_X_LEVEL29 0
#define SPAWN_Y_LEVEL29 11

#endif // TILEMAP_LEVEL29_NES_H
