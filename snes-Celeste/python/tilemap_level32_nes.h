// NES tilemap data for layer 'level32'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL32_NES_H
#define TILEMAP_LEVEL32_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL32_WIDTH 16
#define TILEMAP_LEVEL32_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"

// Tilemap data for layer 'level32' (GIDs, one byte per entry)
const unsigned char tilemap_level32[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 54, 54, 54, 55, 54, 54, 54, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 56, 57, 58, 59, 60, 61, 57, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 62, 63, 64, 65, 66, 67, 65, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 68, 69, 54, 54, 54, 70, 71, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define TILEMAP_LEVEL32_COUNT 256

// Background palette data for layer 'level32' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level32[4][4] = {
    // Background Palette 0
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL32_COUNT 0

// Sprite palette data for layer 'level32' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level32[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x21, 0x0d, 0x0d },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL32_COUNT 4

// Object data for layer 'level32'
const unsigned char object_level32[] = {};

#define OBJECT_LEVEL32_COUNT 0

// Object sprite data for layer 'level32'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level32[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
};

#define OBJECT_SPRITE_LEVEL32_COUNT 7

// Object palette data for layer 'level32' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level32[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL32_COUNT 3

// Player start location for layer 'level32'
#define SPAWN_X_LEVEL32 0
#define SPAWN_Y_LEVEL32 0

#endif // TILEMAP_LEVEL32_NES_H
