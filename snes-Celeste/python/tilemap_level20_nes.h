// NES tilemap data for layer 'level20'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL20_NES_H
#define TILEMAP_LEVEL20_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL20_WIDTH 16
#define TILEMAP_LEVEL20_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level20'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level20_compressed[] = {
    0xc9, 0xc6, 0xcd, 0xcc, 0xe7, 0xc3, 0xc3, 0xc3, 0x03, 0xf1, 0xc0, 0x0d, 0xdc, 0xf1, 0x00, 0xc4,
    0xf4, 0x08, 0xed, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe9, 0x16, 0x37, 0x88, 0x38, 0x39, 0xe0,
    0xc0, 0xfa, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0x01, 0xd0, 0xfb, 0xd5, 0xc2, 0xc2, 0xc3, 0xc3, 0x05,
    0xed, 0xc0, 0xc3, 0xc3, 0xd2, 0xda, 0xc4, 0x0d, 0x0b, 0x22, 0xd0, 0x00, 0xcf, 0xde, 0xdc, 0xf1,
    0xc0, 0x24, 0xc4, 0xc0, 0xfb, 0xd2, 0x16, 0xd3, 0xe9, 0xc0, 0xe9, 0xc0, 0xc0, 0xe7, 0xc9, 0xc4,
    0xc0, 0xc0, 0xd1, 0xc0, 0xc0, 0xe4, 0xfd, 0xc0, 0xc0, 0xc0, 0xc5, 0xd4, 0xc0, 0x16, 0xf7, 0xc0,
    0xc0, 0x00, 0xd4, 0xf4, 0xc7, 0xc0, 0x16, 0xd4, 0xc0, 0xc0, 0xc5, 0xc0, 0xc1, 0xc4, 0x00, 0xda,
    0xc4, 0xc0, 0xd1, 0xca, 0xc5, 0xc7, 0xc0, 0xda, 0xc0, 0xc0, 0xc5, 0xfc, 0xd7, 0x0b, 0xc0, 0x00,
    0x16, 0x0c, 0xd4, 0xc0, 0xc5, 0xc7, 0xc1, 0xd9, 0xc0
};

#define TILEMAP_LEVEL20_COMPRESSED_SIZE 137
#define TILEMAP_LEVEL20_COUNT 256

// Background palette data for layer 'level20' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level20[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0c, 0x27, 0x17 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL20_COUNT 2

// Sprite palette data for layer 'level20' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level20[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL20_COUNT 4

// Object data for layer 'level20'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level20[] = {
    22, 15, 3,
    22, 15, 6,
    22, 7, 7,
    22, 4, 10,
    23, 1, 11,
};

#define OBJECT_LEVEL20_COUNT 5

// Object sprite data for layer 'level20'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level20[] = {
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
    11, 0, 30, 31, 0, 0,
    12, 0, 32, 33, 0, 0,
    13, 2, 34, 35, 36, 37,
    14, 2, 0, 38, 39, 0,
    15, 2, 39, 0, 0, 38,
    22, 0, 56, 57, 58, 59,
    23, 1, 60, 61, 62, 63,
    29, 0, 81, 82, 83, 84,
    30, 0, 85, 86, 87, 88,
    31, 0, 89, 90, 91, 92,
    45, 0, 121, 122, 123, 124,
    46, 0, 0, 0, 123, 125,
    47, 0, 0, 0, 126, 16,
    64, 1, 97, 167, 168, 100,
};

#define OBJECT_SPRITE_LEVEL20_COUNT 23

// Object palette data for layer 'level20' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level20[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL20_COUNT 3

// Player start location for layer 'level20'
#define SPAWN_X_LEVEL20 1
#define SPAWN_Y_LEVEL20 10

#endif // TILEMAP_LEVEL20_NES_H
