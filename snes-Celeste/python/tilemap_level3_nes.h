// NES tilemap data for layer 'level3'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL3_NES_H
#define TILEMAP_LEVEL3_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL3_WIDTH 16
#define TILEMAP_LEVEL3_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level3' (GIDs, one byte per entry)
const unsigned char tilemap_level3[] = {
    3, 3, 3, 3, 3, 3, 5, 6, 9, 12, 9, 9, 2, 3, 3, 3,
    3, 4, 3, 3, 3, 21, 0, 13, 9, 9, 8, 9, 11, 7, 4, 3,
    3, 3, 3, 4, 3, 6, 0, 0, 8, 0, 0, 13, 0, 2, 3, 3,
    3, 5, 3, 3, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3,
    6, 0, 2, 3, 6, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    0, 0, 0, 29, 26, 20, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 7, 3, 21, 27, 27, 27, 27, 0, 0, 0, 0, 32,
    0, 0, 0, 16, 2, 5, 5, 19, 19, 19, 22, 23, 24, 0, 0, 33,
    0, 0, 0, 9, 9, 9, 9, 9, 10, 10, 9, 9, 9, 17, 26, 20,
    0, 0, 16, 9, 12, 9, 11, 8, 0, 0, 13, 9, 12, 9, 7, 3,
    0, 0, 0, 13, 9, 9, 9, 0, 0, 0, 0, 9, 11, 9, 7, 3,
    0, 0, 0, 0, 0, 13, 9, 27, 27, 27, 27, 9, 9, 9, 7, 4,
    0, 0, 0, 0, 0, 0, 9, 18, 20, 20, 22, 8, 13, 0, 7, 3,
    0, 0, 15, 0, 0, 0, 13, 9, 7, 21, 8, 0, 0, 0, 7, 3,
    20, 20, 1, 0, 0, 0, 0, 13, 7, 21, 0, 0, 0, 0, 7, 3,
    3, 4, 21, 0, 0, 30, 0, 0, 7, 21, 0, 0, 0, 26, 3, 3
};

#define TILEMAP_LEVEL3_COUNT 256

// Background palette data for layer 'level3' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level3[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x1a, 0x17, 0x27 },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL3_COUNT 3

// Sprite palette data for layer 'level3' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level3[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL3_COUNT 4

// Object data for layer 'level3'
const unsigned char object_level3[] = {
    26, 1, 6,
    18, 5, 14,
    18, 13, 14,
};

#define OBJECT_LEVEL3_COUNT 3

// Object sprite data for layer 'level3'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level3[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    18, 2, 46, 47, 48, 49,
    26, 1, 75, 76, 77, 78,
};

#define OBJECT_SPRITE_LEVEL3_COUNT 9

// Object palette data for layer 'level3' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level3[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL3_COUNT 3

// Player start location for layer 'level3'
#define SPAWN_X_LEVEL3 1
#define SPAWN_Y_LEVEL3 13

#endif // TILEMAP_LEVEL3_NES_H
