// NES tilemap data for layer 'level1'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL1_NES_H
#define TILEMAP_LEVEL1_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL1_WIDTH 16
#define TILEMAP_LEVEL1_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level1' (GIDs, one byte per entry)
const unsigned char tilemap_level1[] = {
    1, 2, 3, 3, 4, 3, 3, 5, 5, 5, 5, 5, 6, 0, 0, 7,
    3, 1, 2, 5, 5, 5, 6, 8, 0, 0, 9, 8, 0, 0, 0, 7,
    3, 3, 1, 10, 11, 9, 12, 0, 0, 0, 13, 0, 0, 0, 14, 7,
    5, 5, 6, 9, 9, 9, 8, 0, 0, 0, 0, 0, 15, 10, 10, 7,
    1, 0, 9, 12, 9, 8, 16, 9, 17, 0, 0, 0, 18, 19, 20, 3,
    21, 16, 9, 9, 9, 11, 9, 8, 0, 0, 0, 0, 0, 0, 2, 3,
    3, 20, 19, 19, 22, 9, 9, 0, 0, 0, 0, 0, 0, 16, 9, 7,
    3, 6, 12, 9, 9, 8, 0, 0, 0, 0, 0, 0, 0, 9, 12, 7,
    21, 0, 0, 13, 9, 0, 0, 0, 0, 16, 9, 16, 9, 9, 9, 7,
    6, 0, 0, 0, 9, 23, 24, 0, 0, 9, 11, 9, 9, 18, 20, 3,
    0, 0, 0, 16, 9, 9, 12, 25, 16, 9, 9, 9, 12, 9, 7, 3,
    0, 0, 0, 9, 12, 9, 9, 26, 1, 9, 0, 0, 13, 9, 7, 3,
    0, 0, 16, 26, 1, 9, 13, 2, 6, 8, 0, 27, 27, 27, 7, 3,
    20, 20, 20, 3, 21, 8, 0, 26, 1, 27, 27, 26, 20, 20, 3, 3,
    3, 4, 3, 3, 21, 27, 27, 7, 3, 20, 20, 3, 3, 3, 4, 3,
    3, 3, 3, 3, 3, 20, 20, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

#define TILEMAP_LEVEL1_COUNT 256

// Background palette data for layer 'level1' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level1[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL1_COUNT 2

// Sprite palette data for layer 'level1' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level1[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL1_COUNT 4

// Object data for layer 'level1'
const unsigned char object_level1[] = {
    64, 1, 4,
};

#define OBJECT_LEVEL1_COUNT 1

// Object sprite data for layer 'level1'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level1[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    64, 2, 170, 171, 172, 106,
};

#define OBJECT_SPRITE_LEVEL1_COUNT 8

// Object palette data for layer 'level1' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level1[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL1_COUNT 3

// Player start location for layer 'level1'
#define SPAWN_X_LEVEL1 1
#define SPAWN_Y_LEVEL1 12

#endif // TILEMAP_LEVEL1_NES_H
