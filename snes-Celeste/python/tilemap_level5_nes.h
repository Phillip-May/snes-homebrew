// NES tilemap data for layer 'level5'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL5_NES_H
#define TILEMAP_LEVEL5_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL5_WIDTH 16
#define TILEMAP_LEVEL5_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level5'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level5_compressed[] = {
    0xdf, 0xc0, 0xf5, 0xc3, 0xdf, 0xdc, 0xc3, 0xc2, 0xe1, 0xc0, 0x00, 0x20, 0xca, 0xc0, 0xc0, 0xcb,
    0x02, 0x04, 0xd3, 0x11, 0x1c, 0xd1, 0xc7, 0xc0, 0xc0, 0xc5, 0xf5, 0xcb, 0x09, 0x12, 0x13, 0x16,
    0xc4, 0xc0, 0xc0, 0xc5, 0x11, 0x07, 0x00, 0xe2, 0xc1, 0xc4, 0xc0, 0xdd, 0x1e, 0xd7, 0xea, 0xcb,
    0xca, 0xc0, 0xc0, 0xc0, 0x1d, 0xf6, 0xd3, 0xc4, 0xe9, 0xc0, 0xdd, 0xe0, 0xf5, 0x15, 0xe2, 0xc4,
    0xc0, 0xc0, 0xc0, 0x12, 0x16, 0xd1, 0xeb, 0xc1, 0xc0, 0xc0, 0xc0, 0xc0, 0x0a, 0xd3, 0xea, 0x03,
    0x01, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x21, 0xc1, 0xcb, 0xfd, 0xd4, 0xc0, 0xc0, 0xc0, 0x00, 0x20,
    0xc7, 0xc0, 0xcb, 0xd9, 0xc0, 0x00, 0x0f, 0xc0, 0x00, 0x0a, 0xc4, 0xc0, 0xf4, 0xc7, 0x00, 0xdb,
    0x13, 0x16, 0xc0, 0x0a, 0xc0, 0xc0, 0x0e, 0xcb, 0x17, 0xc6, 0x09, 0x0a, 0xd4, 0x10, 0x0a, 0xc0,
    0x10, 0x00, 0x12, 0xdb, 0xdb, 0xf4, 0x0a, 0xd9, 0x09, 0x21, 0xc0, 0xf2, 0x22, 0x0a, 0xc1, 0xc7,
    0x0d, 0x0a, 0xe1, 0x09, 0x1f, 0xd1, 0xc1, 0x09, 0x0a, 0xc1, 0x09
};

#define TILEMAP_LEVEL5_COMPRESSED_SIZE 155
#define TILEMAP_LEVEL5_COUNT 256

// Background palette data for layer 'level5' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level5[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL5_COUNT 2

// Sprite palette data for layer 'level5' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level5[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL5_COUNT 4

// Object data for layer 'level5'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level5[] = {
    20, 15, 12,
    8, 7, 14,
};

#define OBJECT_LEVEL5_COUNT 2

// Object sprite data for layer 'level5'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level5[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    8, 0, 23, 24, 25, 26,
    20, 0, 50, 51, 52, 53,
};

#define OBJECT_SPRITE_LEVEL5_COUNT 9

// Object palette data for layer 'level5' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level5[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL5_COUNT 3

// Player start location for layer 'level5'
#define SPAWN_X_LEVEL5 1
#define SPAWN_Y_LEVEL5 11

#endif // TILEMAP_LEVEL5_NES_H
