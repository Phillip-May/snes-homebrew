// NES tilemap data for layer 'level24'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL24_NES_H
#define TILEMAP_LEVEL24_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL24_WIDTH 16
#define TILEMAP_LEVEL24_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level24'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level24_compressed[] = {
    0xda, 0xc1, 0xcc, 0xd5, 0xc2, 0xd5, 0xc2, 0xc2, 0xda, 0xd3, 0xdc, 0xc3, 0xc3, 0xc3, 0xc2, 0xd5,
    0x16, 0x0b, 0xc4, 0xca, 0xc0, 0xc0, 0xcb, 0xf9, 0xc2, 0xda, 0xc0, 0xe9, 0xc8, 0xe0, 0xc5, 0x0c,
    0xcc, 0xc9, 0xc4, 0xc0, 0x20, 0x2d, 0x30, 0x34, 0xc0, 0xcb, 0xcc, 0xec, 0xc8, 0xc8, 0x2d, 0x2e,
    0x2e, 0x35, 0xc0, 0x2a, 0x1f, 0xc2, 0xc2, 0xcf, 0xde, 0x33, 0x31, 0x31, 0x32, 0xc0, 0xf0, 0xc2,
    0xc2, 0xd2, 0xc2, 0xf6, 0x0c, 0xc4, 0xce, 0xfe, 0xc2, 0xc2, 0xc2, 0xc9, 0x0b, 0xe9, 0xc0, 0xf0,
    0xc2, 0xc2, 0xd5, 0xc2, 0x16, 0xc4, 0xdd, 0xc8, 0xcc, 0xc2, 0xc2, 0xc2, 0xec, 0xc0, 0x2a, 0xe5,
    0x01, 0xf9, 0xd5, 0xc3, 0xc3, 0xdf, 0xc0, 0xf0, 0xc2, 0x01, 0xf9, 0x03, 0xd3, 0xca, 0xc0, 0xc0,
    0xf0, 0xd2, 0x03, 0x01, 0xdc, 0xca, 0xc0, 0xc0, 0xd4, 0xf0, 0xc2, 0xc2, 0xcf, 0xc7, 0x0e, 0x1e,
    0xd1, 0xc7, 0xf0, 0xc2, 0xd2, 0xc2, 0xcf, 0xde, 0xe2, 0x0c, 0x12, 0xf0, 0xc2, 0xc2, 0xc2
};

#define TILEMAP_LEVEL24_COMPRESSED_SIZE 143
#define TILEMAP_LEVEL24_COUNT 256

// Background palette data for layer 'level24' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level24[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL24_COUNT 2

// Sprite palette data for layer 'level24' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level24[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL24_COUNT 4

// Object data for layer 'level24'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level24[] = {};

#define OBJECT_LEVEL24_COUNT 0

// Object sprite data for layer 'level24'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level24[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    9, 0, 27, 28, 29, 26,
    10, 0, 0, 26, 0, 26,
    13, 2, 34, 35, 36, 37,
    14, 2, 0, 38, 39, 0,
    15, 2, 39, 0, 0, 38,
    23, 1, 60, 61, 62, 63,
    64, 1, 97, 167, 168, 100,
};

#define OBJECT_SPRITE_LEVEL24_COUNT 14

// Object palette data for layer 'level24' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level24[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL24_COUNT 3

// Player start location for layer 'level24'
#define SPAWN_X_LEVEL24 1
#define SPAWN_Y_LEVEL24 14

#endif // TILEMAP_LEVEL24_NES_H
