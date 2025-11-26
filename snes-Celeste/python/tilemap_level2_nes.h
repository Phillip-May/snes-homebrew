// NES tilemap data for layer 'level2'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL2_NES_H
#define TILEMAP_LEVEL2_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL2_WIDTH 16
#define TILEMAP_LEVEL2_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level2' (GIDs, one byte per entry)
const unsigned char tilemap_level2[] = {
    3, 21, 7, 3, 3, 21, 2, 5, 5, 5, 3, 21, 9, 9, 9, 7,
    3, 21, 2, 5, 5, 6, 9, 9, 0, 9, 7, 21, 13, 11, 9, 7,
    3, 3, 1, 10, 11, 9, 8, 8, 0, 9, 7, 21, 0, 16, 12, 7,
    4, 3, 21, 9, 9, 8, 0, 0, 0, 13, 7, 6, 0, 0, 13, 7,
    3, 4, 21, 8, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 7,
    3, 5, 6, 0, 0, 0, 0, 27, 0, 0, 29, 0, 0, 0, 25, 7,
    21, 0, 14, 0, 16, 17, 0, 30, 0, 0, 0, 0, 0, 0, 26, 3,
    3, 1, 10, 10, 11, 8, 0, 28, 17, 0, 0, 0, 0, 24, 7, 4,
    3, 3, 20, 1, 9, 17, 0, 29, 9, 24, 17, 0, 31, 9, 2, 5,
    3, 3, 4, 21, 9, 9, 23, 10, 9, 9, 9, 12, 9, 9, 26, 20,
    4, 3, 3, 21, 0, 13, 9, 30, 8, 0, 13, 9, 9, 18, 5, 3,
    5, 5, 3, 21, 0, 16, 9, 28, 0, 0, 0, 0, 13, 9, 9, 7,
    0, 9, 2, 21, 16, 12, 8, 28, 0, 0, 0, 0, 0, 0, 13, 2,
    0, 13, 12, 29, 9, 8, 0, 28, 27, 27, 27, 0, 0, 0, 16, 9,
    0, 0, 13, 9, 9, 23, 15, 7, 20, 20, 1, 0, 0, 0, 12, 9,
    20, 20, 20, 20, 20, 20, 20, 3, 3, 4, 21, 23, 24, 31, 9, 9
};

#define TILEMAP_LEVEL2_COUNT 256

// Background palette data for layer 'level2' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level2[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL2_COUNT 2

// Sprite palette data for layer 'level2' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level2[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL2_COUNT 4

// Object data for layer 'level2'
const unsigned char object_level2[] = {};

#define OBJECT_LEVEL2_COUNT 0

// Object sprite data for layer 'level2'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level2[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
};

#define OBJECT_SPRITE_LEVEL2_COUNT 7

// Object palette data for layer 'level2' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level2[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL2_COUNT 3

// Player start location for layer 'level2'
#define SPAWN_X_LEVEL2 1
#define SPAWN_Y_LEVEL2 14

#endif // TILEMAP_LEVEL2_NES_H
