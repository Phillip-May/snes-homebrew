// NES tilemap data for layer 'level31'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL31_NES_H
#define TILEMAP_LEVEL31_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL31_WIDTH 16
#define TILEMAP_LEVEL31_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level31'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
const unsigned char tilemap_level31_compressed[] = {
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0x10, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x0b, 0xc0, 0xd4, 0xc0,
    0xc0, 0xc0, 0xc0, 0xd1, 0xc5, 0xc0, 0xfb, 0xc0, 0xc0, 0xc0, 0xc0, 0xc5, 0x17, 0xc7, 0x00, 0x0b,
    0x00, 0xd4, 0xc0, 0xc0, 0xc0, 0xc5, 0xd3, 0xc0, 0xf2, 0xc7, 0xc0, 0xc0, 0xc0, 0xc6, 0xc1, 0x1a,
    0xf6, 0x0c, 0xc4, 0xc0, 0xc0, 0xc0, 0x1f, 0xd3, 0x1a, 0xc2, 0xf6, 0x11, 0x10, 0xc0, 0xc0, 0xc0,
    0xcb, 0x1a, 0xd2, 0xc2, 0xf6, 0x0c, 0x1f, 0xc0, 0x18, 0x18, 0xf3, 0x08, 0xcc, 0xc2, 0xc9, 0xe2,
    0x09, 0x1f, 0xc5, 0xe2, 0xfb, 0xdc, 0xc2, 0xec, 0xc4, 0xcb, 0xc0, 0xcb, 0x11, 0x0f, 0x1a, 0x01,
    0xcc, 0xdf, 0x0a, 0xc0, 0xc7, 0xc0, 0xe5, 0xff, 0x15, 0xfc, 0xe5, 0xf6, 0x11, 0xf2, 0xc0, 0xf8,
    0xd2, 0xf9, 0xff, 0xc2, 0x01, 0x0b, 0xd3, 0xe5, 0x01, 0xcc, 0xc2, 0xc2, 0xd2, 0xc2, 0xd0, 0x01
};

#define TILEMAP_LEVEL31_COMPRESSED_SIZE 144
#define TILEMAP_LEVEL31_COUNT 256

// Background palette data for layer 'level31' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
const unsigned char palette_background_level31[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL31_COUNT 2

// Sprite palette data for layer 'level31' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
const unsigned char palette_sprite_level31[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Sprite Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Sprite Palette 2
    { 0x0d, 0x17, 0x10, 0x1a },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL31_COUNT 4

// Object data for layer 'level31'
const unsigned char object_level31[] = {
    118, 7, 6,
};

#define OBJECT_LEVEL31_COUNT 1

// Object sprite data for layer 'level31'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
const unsigned char object_sprite_level31[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    118, 2, 271, 272, 273, 0,
};

#define OBJECT_SPRITE_LEVEL31_COUNT 8

// Object palette data for layer 'level31' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
const unsigned char palette_object_level31[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x27, 0x28 },
    // Object Palette 1
    { 0x0d, 0x15, 0x25, 0x0c },
    // Object Palette 2
    { 0x0d, 0x17, 0x10, 0x1a }
};

#define PALETTE_OBJECT_LEVEL31_COUNT 3

// Player start location for layer 'level31'
#define SPAWN_X_LEVEL31 0
#define SPAWN_Y_LEVEL31 14

#endif // TILEMAP_LEVEL31_NES_H
