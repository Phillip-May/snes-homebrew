// NES tilemap data for layer 'level27'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL27_NES_H
#define TILEMAP_LEVEL27_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL27_WIDTH 16
#define TILEMAP_LEVEL27_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level27' (GIDs, one byte per entry)
const unsigned char tilemap_level27[] = {
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 5, 6, 35, 9,
    3, 3, 3, 4, 3, 3, 5, 5, 5, 5, 3, 21, 34, 34, 0, 12,
    3, 3, 3, 3, 3, 6, 34, 34, 34, 34, 2, 6, 0, 0, 0, 9,
    5, 5, 3, 3, 21, 35, 0, 0, 0, 0, 36, 30, 0, 0, 16, 9,
    20, 1, 2, 5, 6, 35, 0, 0, 0, 0, 36, 28, 9, 17, 0, 13,
    3, 3, 20, 20, 1, 35, 0, 0, 0, 0, 36, 28, 8, 0, 0, 0,
    3, 3, 4, 3, 21, 35, 0, 0, 27, 0, 36, 28, 35, 0, 0, 0,
    3, 3, 3, 3, 21, 35, 0, 36, 30, 0, 36, 28, 35, 0, 0, 0,
    5, 5, 5, 5, 21, 35, 0, 36, 28, 0, 36, 28, 0, 0, 0, 0,
    34, 34, 34, 34, 29, 35, 0, 36, 28, 17, 36, 28, 0, 0, 0, 0,
    0, 0, 0, 16, 9, 0, 0, 36, 28, 9, 36, 29, 0, 0, 0, 0,
    0, 0, 0, 11, 12, 8, 0, 36, 28, 9, 9, 9, 0, 0, 0, 0,
    0, 0, 0, 0, 9, 0, 0, 36, 28, 9, 12, 8, 0, 0, 0, 0,
    0, 0, 0, 31, 9, 23, 0, 36, 28, 0, 13, 9, 23, 0, 0, 0,
    0, 0, 16, 12, 9, 8, 0, 36, 29, 0, 0, 12, 8, 0, 0, 0,
    1, 0, 11, 9, 9, 0, 0, 0, 12, 0, 0, 9, 11, 0, 0, 0
};

#define TILEMAP_LEVEL27_COUNT 256

// Background palette data for layer 'level27' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level27[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL27_COUNT 1

// Sprite palette data for layer 'level27' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level27[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL27_COUNT 4

// Object data for layer 'level27'
const unsigned char object_level27[] = {
    22, 6, 8,
    22, 12, 9,
};

#define OBJECT_LEVEL27_COUNT 2

// Object sprite data for layer 'level27'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level27[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    22, 1, 59, 60, 61, 62,
};

#define OBJECT_SPRITE_LEVEL27_COUNT 8

// Object palette data for layer 'level27' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level27[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL27_COUNT 3

// Player start location for layer 'level27'
#define SPAWN_X_LEVEL27 0
#define SPAWN_Y_LEVEL27 14

#endif // TILEMAP_LEVEL27_NES_H
