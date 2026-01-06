// NES tilemap data for layer 'level7'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL7_NES_H
#define TILEMAP_LEVEL7_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL7_WIDTH 16
#define TILEMAP_LEVEL7_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level7'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level7_compressed[] = {
    0xd2, 0xc2, 0xc9, 0xc1, 0xc1, 0xcc, 0xd5, 0xc2, 0xc2, 0xd5, 0xdf, 0xd3, 0xcb, 0xdc, 0xc2, 0xd5,
    0xc2, 0xdf, 0xc1, 0xca, 0xc0, 0xcb, 0xdc, 0xc2, 0xec, 0xd0, 0xc0, 0xc0, 0xc0, 0xc0, 0xce, 0xcc,
    0xc9, 0xd0, 0xc0, 0xc0, 0xc0, 0xc0, 0xce, 0xcc, 0x03, 0x06, 0xd0, 0xc0, 0xdd, 0xe0, 0xc0, 0xce,
    0x02, 0xec, 0xd9, 0xc0, 0xce, 0x12, 0x16, 0xd0, 0xc0, 0xc5, 0xeb, 0x0b, 0x0b, 0xc0, 0xc0, 0xc1,
    0xd4, 0xc0, 0xc6, 0x07, 0x06, 0xd3, 0xc0, 0xc0, 0x0b, 0xd3, 0xc0, 0xc0, 0xef, 0xcb, 0xd4, 0xc0,
    0xc6, 0xca, 0xc0, 0xc0, 0xc0, 0xc5, 0xc4, 0xc0, 0xcb, 0xc7, 0xc0, 0xc0, 0xc0, 0xc6, 0xc7, 0xc0,
    0xd1, 0xfc, 0xc0, 0xc0, 0xc0, 0xd1, 0xc4, 0xc0, 0xc5, 0xc7, 0xc0, 0xc0, 0xc0, 0x00, 0x0b, 0xc0,
    0xc0, 0xc5, 0x0b, 0xc0, 0xc0, 0xc0, 0xd1, 0xc7, 0x0e, 0xc0, 0x00, 0xf3, 0xd4, 0xc0, 0xc0, 0xc5,
    0xe5, 0x01, 0xc0, 0xc5, 0xc1, 0xc0, 0xc0, 0xc0
};

#define TILEMAP_LEVEL7_COMPRESSED_SIZE 136
#define TILEMAP_LEVEL7_COUNT 256

// Background palette data for layer 'level7' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level7[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL7_COUNT 2

// Sprite palette data for layer 'level7' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level7[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL7_COUNT 4

// Object data for layer 'level7'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level7[] = {
    28, 10, 3,
    12, 3, 5,
    12, 12, 5,
    11, 3, 8,
    11, 12, 8,
    12, 3, 11,
    12, 9, 11,
    12, 15, 11,
    11, 0, 13,
    11, 6, 13,
    11, 12, 13,
};

#define OBJECT_LEVEL7_COUNT 11

// Object sprite data for layer 'level7'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level7[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    11, 0, 30, 31, 0, 0,
    12, 0, 32, 33, 0, 0,
    28, 0, 77, 78, 79, 80,
};

#define OBJECT_SPRITE_LEVEL7_COUNT 10

// Object palette data for layer 'level7' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level7[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL7_COUNT 3

// Player start location for layer 'level7'
#define SPAWN_X_LEVEL7 2
#define SPAWN_Y_LEVEL7 14

#endif // TILEMAP_LEVEL7_NES_H
