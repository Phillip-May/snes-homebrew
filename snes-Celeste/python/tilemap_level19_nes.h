// NES tilemap data for layer 'level19'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL19_NES_H
#define TILEMAP_LEVEL19_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL19_WIDTH 16
#define TILEMAP_LEVEL19_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level19' (GIDs, one byte per entry)
const unsigned char tilemap_level19[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 36, 10, 0, 0, 2, 5, 5, 3,
    0, 0, 0, 0, 0, 0, 0, 27, 27, 10, 0, 0, 0, 13, 9, 7,
    0, 0, 0, 0, 0, 0, 36, 18, 19, 22, 0, 0, 0, 0, 11, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 36, 18, 19, 22, 17, 0, 0, 34,
    27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 9, 0, 27,
    30, 17, 16, 0, 0, 0, 0, 0, 0, 0, 0, 34, 13, 9, 17, 26,
    29, 9, 12, 17, 27, 0, 0, 0, 0, 0, 0, 27, 16, 9, 9, 2,
    34, 0, 13, 9, 30, 27, 0, 0, 0, 0, 36, 30, 9, 9, 12, 34,
    27, 0, 16, 9, 29, 30, 0, 0, 0, 0, 36, 29, 13, 9, 8, 27,
    30, 9, 9, 8, 34, 28, 17, 0, 0, 0, 0, 34, 0, 13, 9, 26,
    29, 9, 11, 0, 27, 28, 9, 16, 0, 0, 0, 27, 0, 9, 9, 2,
    34, 13, 9, 12, 30, 28, 9, 11, 17, 0, 0, 30, 16, 9, 8, 34,
    0, 0, 9, 9, 29, 28, 8, 0, 9, 12, 0, 29, 9, 12, 0, 27,
    0, 0, 8, 0, 34, 28, 0, 0, 13, 9, 0, 0, 9, 9, 0, 26,
    0, 0, 0, 0, 16, 29, 0, 0, 0, 9, 17, 0, 9, 8, 0, 2,
    1, 0, 0, 0, 12, 9, 17, 0, 0, 13, 9, 11, 9, 0, 0, 34
};

#define TILEMAP_LEVEL19_COUNT 256

// Background palette data for layer 'level19' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level19[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL19_COUNT 1

// Sprite palette data for layer 'level19' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level19[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL19_COUNT 4

// Object data for layer 'level19'
const unsigned char object_level19[] = {
    20, 11, 2,
    8, 6, 14,
};

#define OBJECT_LEVEL19_COUNT 2

// Object sprite data for layer 'level19'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level19[] = {
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

#define OBJECT_SPRITE_LEVEL19_COUNT 9

// Object palette data for layer 'level19' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level19[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL19_COUNT 3

// Player start location for layer 'level19'
#define SPAWN_X_LEVEL19 0
#define SPAWN_Y_LEVEL19 14

#endif // TILEMAP_LEVEL19_NES_H
