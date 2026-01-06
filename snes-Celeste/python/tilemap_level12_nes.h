// NES tilemap data for layer 'level12'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL12_NES_H
#define TILEMAP_LEVEL12_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL12_WIDTH 16
#define TILEMAP_LEVEL12_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level12'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level12_compressed[] = {
    0xc2, 0x03, 0xf1, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x29, 0xd5, 0xe4, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0x00, 0x29, 0xc2, 0xe4, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x29, 0xc3, 0x06, 0x17, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0x2a, 0x00, 0xde, 0xc1, 0xc0, 0x23, 0xc0, 0x23, 0xc0, 0x00, 0x2a, 0x2b, 0x2b,
    0xc9, 0xd3, 0x17, 0x00, 0x2c, 0xc0, 0x2c, 0x2a, 0x2d, 0x2d, 0x2b, 0x2e, 0x2e, 0x03, 0xee, 0xc1,
    0x11, 0xe5, 0xde, 0x29, 0x2b, 0x2b, 0x2f, 0xc0, 0x15, 0xcb, 0xe1, 0xd8, 0xc2, 0x15, 0x30, 0x2e,
    0x2f, 0xc0, 0x00, 0xe4, 0xc4, 0xcb, 0xfe, 0xc2, 0x01, 0xc0, 0xc0, 0x00, 0xe4, 0xc0, 0x00, 0x1d,
    0xc2, 0xc2, 0xe4, 0xc0, 0xc0, 0xf1, 0xc0, 0xef, 0xe3, 0xc2, 0xf1, 0xc0, 0xc0, 0xc0, 0xc0, 0x2a,
    0x2d, 0x31, 0xcc, 0xda, 0xd4, 0xc0, 0xc0, 0xc0, 0x00, 0x29, 0x2b, 0x32, 0xfe, 0xda, 0xc7, 0xc0,
    0xc0, 0x33, 0xc0, 0x29, 0x2b, 0x32, 0xcc, 0xda, 0x0b, 0x17, 0x10, 0xd4, 0x00, 0xe5, 0x01, 0x29,
    0x2b, 0x32, 0xcc, 0xee, 0xc1, 0x0c, 0xc4, 0xf5, 0xc9, 0x29, 0x2b, 0x32, 0xeb, 0xc1, 0xc1, 0xc1,
    0x11
};

#define TILEMAP_LEVEL12_COMPRESSED_SIZE 161
#define TILEMAP_LEVEL12_COUNT 256

// Background palette data for layer 'level12' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level12[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x1a, 0x17, 0x27 },
    // Background Palette 2
    { 0x0d, 0x1b, 0x10, 0x25 },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL12_COUNT 3

// Sprite palette data for layer 'level12' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level12[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL12_COUNT 4

// Object data for layer 'level12'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level12[] = {
    70, 7, 4,
    71, 8, 4,
    86, 7, 5,
    87, 8, 5,
    28, 12, 9,
};

#define OBJECT_LEVEL12_COUNT 5

// Object sprite data for layer 'level12'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level12[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    28, 0, 77, 78, 79, 80,
    70, 2, 0, 0, 185, 186,
    71, 2, 0, 0, 187, 188,
    86, 2, 209, 210, 211, 212,
    87, 2, 213, 214, 215, 216,
};

#define OBJECT_SPRITE_LEVEL12_COUNT 12

// Object palette data for layer 'level12' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level12[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL12_COUNT 3

// Player start location for layer 'level12'
#define SPAWN_X_LEVEL12 2
#define SPAWN_Y_LEVEL12 13

#endif // TILEMAP_LEVEL12_NES_H
