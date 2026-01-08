// NES tilemap data for layer 'level6'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL6_NES_H
#define TILEMAP_LEVEL6_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL6_WIDTH 16
#define TILEMAP_LEVEL6_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level6'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level6_compressed[] = {
    0xc2, 0xd2, 0xc2, 0xc2, 0xc2, 0xc9, 0xc1, 0xcc, 0xc2, 0xc2, 0xd2, 0xc2, 0xc3, 0xdf, 0xc1, 0xcc,
    0xc2, 0xc3, 0xc3, 0xdf, 0xe5, 0xde, 0xc1, 0xcc, 0xc9, 0xe5, 0x01, 0x0a, 0x1f, 0x01, 0xdc, 0xdf,
    0xc1, 0xcc, 0xdf, 0xcc, 0x16, 0x0b, 0xfd, 0x0a, 0xc1, 0xc1, 0x0c, 0xcc, 0xc1, 0xdc, 0xee, 0xca,
    0xc6, 0xc6, 0xc1, 0xcc, 0xc1, 0xd6, 0x28, 0xc1, 0xc0, 0xc0, 0xc6, 0x1f, 0xd2, 0xd3, 0xd4, 0xc0,
    0x0d, 0xc4, 0xc0, 0xc0, 0xdc, 0x05, 0xc1, 0xca, 0xc0, 0xc0, 0xc0, 0x11, 0x1c, 0xf3, 0xc7, 0xca,
    0xc0, 0xc0, 0xc0, 0xc0, 0xcb, 0xc1, 0xc4, 0xc7, 0xc0, 0x12, 0xd9, 0xc0, 0xc0, 0xc5, 0xc7, 0xc5,
    0xd1, 0xc1, 0xe2, 0x1d, 0xc0, 0xd1, 0xca, 0xc0, 0xcf, 0xf6, 0xc1, 0xc1, 0xd4, 0x1d, 0xcd, 0x09,
    0x0e, 0xfb, 0xc2, 0xcf, 0xde, 0xc1, 0xc1, 0xe2, 0x09, 0xe5, 0xd2, 0xc2, 0xc2, 0x16, 0xcb, 0x09,
    0xe5, 0xcf, 0xc2, 0xc2, 0xc2, 0xd5, 0xe4, 0xc6, 0xcc, 0xc2, 0xd5
};

#define TILEMAP_LEVEL6_COMPRESSED_SIZE 139
#define TILEMAP_LEVEL6_COUNT 256

// Background palette data for layer 'level6' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level6[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL6_COUNT 2

// Sprite palette data for layer 'level6' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level6[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL6_COUNT 4

// Object data for layer 'level6'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level6[] = {
    22, 9, 8,
};

#define OBJECT_LEVEL6_COUNT 1

// Object sprite data for layer 'level6'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level6[] = {
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
    22, 0, 56, 57, 58, 59,
    23, 1, 60, 61, 62, 63,
    64, 1, 97, 167, 168, 100,
};

#define OBJECT_SPRITE_LEVEL6_COUNT 15

// Object palette data for layer 'level6' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level6[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL6_COUNT 3

// Player start location for layer 'level6'
#define SPAWN_X_LEVEL6 1
#define SPAWN_Y_LEVEL6 11

#endif // TILEMAP_LEVEL6_NES_H
