// NES tilemap data for layer 'level16'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL16_NES_H
#define TILEMAP_LEVEL16_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL16_WIDTH 16
#define TILEMAP_LEVEL16_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level16'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level16_compressed[] = {
    0xc2, 0xec, 0xcd, 0xea, 0xc3, 0xc3, 0xc3, 0xd2, 0xc3, 0xdf, 0xc1, 0xc1, 0xc1, 0xe2, 0x09, 0x0a,
    0xf9, 0xc1, 0xc1, 0xca, 0xc6, 0xc7, 0xc1, 0xd3, 0xea, 0xe2, 0xc4, 0xc0, 0xc0, 0xc4, 0xcb, 0xc1,
    0xc4, 0xcd, 0xdd, 0xe0, 0xc0, 0xc0, 0xc5, 0x08, 0xe9, 0xc5, 0x08, 0x28, 0x1d, 0x01, 0xc0, 0xf7,
    0xdd, 0xf2, 0xc0, 0xc0, 0xce, 0xfd, 0xc8, 0xc8, 0x1e, 0x21, 0xca, 0xc0, 0x28, 0xc0, 0x28, 0x12,
    0xdb, 0xdb, 0x13, 0x16, 0x1f, 0xfc, 0xc0, 0xf7, 0xce, 0x0a, 0xd6, 0xd6, 0xd6, 0x1f, 0xc7, 0xc0,
    0xf7, 0xc0, 0xe8, 0xc0, 0xc0, 0x00, 0x1f, 0xc4, 0xc0, 0x28, 0xc0, 0xc0, 0xc0, 0xdd, 0xc0, 0x20,
    0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xd1, 0x21, 0xc0, 0xe8, 0xc0, 0xc0, 0xc0, 0x00, 0x21, 0xd4, 0x0c,
    0x1f, 0xc0, 0xc0, 0xc0, 0xc0, 0x1d, 0x01, 0x00, 0x20, 0xca, 0x09, 0x1f, 0xc0, 0xc0, 0xc0, 0xc0,
    0xfd, 0x11, 0x21, 0x0b, 0xc5, 0x20, 0xc0, 0xc0, 0xc0, 0xc0, 0xde, 0x0c, 0x1f, 0x09, 0x18, 0x09,
    0x21, 0xc0, 0xc0, 0xc0, 0xc0
};

#define TILEMAP_LEVEL16_COMPRESSED_SIZE 149
#define TILEMAP_LEVEL16_COUNT 256

// Background palette data for layer 'level16' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level16[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0c, 0x27, 0x17 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL16_COUNT 2

// Sprite palette data for layer 'level16' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level16[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL16_COUNT 4

// Object data for layer 'level16'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level16[] = {
    18, 7, 4,
    22, 14, 4,
    23, 7, 5,
    23, 15, 7,
    23, 15, 8,
    18, 5, 14,
    23, 5, 15,
};

#define OBJECT_LEVEL16_COUNT 7

// Object sprite data for layer 'level16'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level16[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    18, 0, 46, 47, 48, 49,
    22, 0, 56, 57, 58, 59,
    23, 1, 60, 61, 62, 63,
};

#define OBJECT_SPRITE_LEVEL16_COUNT 10

// Object palette data for layer 'level16' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level16[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL16_COUNT 3

// Player start location for layer 'level16'
#define SPAWN_X_LEVEL16 0
#define SPAWN_Y_LEVEL16 12

#endif // TILEMAP_LEVEL16_NES_H
