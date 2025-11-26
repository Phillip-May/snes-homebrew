// NES tilemap data for layer 'level22'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL22_NES_H
#define TILEMAP_LEVEL22_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL22_WIDTH 16
#define TILEMAP_LEVEL22_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level22' (GIDs, one byte per entry)
const unsigned char tilemap_level22[] = {
    42, 42, 42, 43, 38, 40, 40, 40, 46, 51, 0, 0, 0, 51, 38, 40,
    0, 0, 0, 0, 44, 42, 42, 42, 43, 51, 0, 0, 0, 51, 38, 40,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 51, 44, 42,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 17, 0, 0, 31, 9, 0, 0, 16, 0, 0, 0, 0,
    0, 0, 16, 24, 9, 9, 31, 9, 9, 9, 9, 9, 17, 0, 0, 0,
    31, 17, 9, 9, 9, 9, 11, 8, 0, 0, 13, 11, 9, 16, 23, 31,
    9, 9, 9, 13, 12, 9, 9, 0, 0, 0, 0, 0, 9, 9, 9, 9,
    9, 9, 8, 0, 9, 9, 8, 0, 0, 0, 0, 16, 9, 12, 9, 13,
    12, 9, 0, 0, 13, 9, 14, 0, 53, 53, 15, 9, 9, 8, 0, 0,
    9, 9, 17, 0, 0, 9, 26, 20, 1, 26, 20, 1, 9, 0, 0, 16,
    11, 8, 0, 0, 0, 10, 2, 5, 6, 7, 4, 21, 9, 39, 41, 41,
    9, 0, 0, 0, 0, 26, 20, 20, 20, 3, 3, 3, 1, 38, 40, 40,
    41, 41, 41, 41, 45, 7, 3, 4, 3, 3, 3, 3, 21, 38, 40, 40,
    40, 40, 40, 40, 46, 7, 3, 3, 3, 3, 3, 4, 21, 38, 40, 40
};

#define TILEMAP_LEVEL22_COUNT 256

// Background palette data for layer 'level22' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level22[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL22_COUNT 2

// Sprite palette data for layer 'level22' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level22[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x28, 0x17, 0x27 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL22_COUNT 4

// Object data for layer 'level22'
const unsigned char object_level22[] = {
    96, 8, 9,
    97, 9, 9,
};

#define OBJECT_LEVEL22_COUNT 2

// Object sprite data for layer 'level22'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level22[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    96, 0, 229, 230, 231, 232,
    97, 0, 230, 233, 232, 234,
};

#define OBJECT_SPRITE_LEVEL22_COUNT 9

// Object palette data for layer 'level22' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level22[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL22_COUNT 3

// Player start location for layer 'level22'
#define SPAWN_X_LEVEL22 2
#define SPAWN_Y_LEVEL22 13

#endif // TILEMAP_LEVEL22_NES_H
