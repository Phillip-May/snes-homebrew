// NES tilemap data for layer 'level18'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL18_NES_H
#define TILEMAP_LEVEL18_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL18_WIDTH 16
#define TILEMAP_LEVEL18_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level18'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level18_compressed[] = {
    0x06, 0xc4, 0xef, 0xc3, 0xe3, 0xc3, 0xc3, 0xe3, 0x05, 0xe8, 0xc0, 0xd6, 0xd6, 0x1f, 0xd6, 0xd6,
    0x26, 0x1f, 0xe8, 0xc0, 0xc0, 0xc0, 0x00, 0x20, 0xc0, 0xc0, 0x00, 0x20, 0xc0, 0xc0, 0xc0, 0xc0,
    0xe6, 0xc0, 0xc0, 0xe6, 0xc0, 0xc0, 0xc0, 0xe0, 0xc0, 0xc0, 0xe0, 0xc0, 0xc0, 0xc0, 0xce, 0x0a,
    0x27, 0xd4, 0xce, 0x0a, 0xd0, 0xc0, 0xc7, 0xd1, 0xe6, 0xd7, 0xd4, 0xe6, 0xc0, 0xc0, 0xd9, 0xc1,
    0xd4, 0xc1, 0xc0, 0xc0, 0xc0, 0xc0, 0xc1, 0xc1, 0xcd, 0xca, 0xc0, 0xc0, 0xc0, 0xc0, 0xcd, 0xc6,
    0xc1, 0xe1, 0x11, 0xd4, 0xdd, 0xc0, 0x00, 0xc4, 0xc0, 0xc6, 0xc1, 0xc1, 0x11, 0x28, 0x21, 0xd0,
    0xc0, 0xc0, 0xc0, 0xc0, 0xc1, 0xc1, 0xf8, 0xd0, 0xc0, 0xc0, 0xc0, 0xc8, 0xc8, 0xcd, 0x28, 0x20,
    0xd0, 0x10, 0xcf, 0x01, 0xc8, 0x1d, 0xdb, 0x16, 0xe2, 0xca, 0xc0, 0x0c, 0xd2, 0xfa, 0x14, 0xe4,
    0xc6, 0xc1, 0xc7, 0xd1, 0x09, 0xc2, 0xc2, 0xec, 0xc0, 0xc5, 0xca, 0xc0, 0xc1
};

#define TILEMAP_LEVEL18_COMPRESSED_SIZE 141
#define TILEMAP_LEVEL18_COUNT 256

// Background palette data for layer 'level18' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level18[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL18_COUNT 1

// Sprite palette data for layer 'level18' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level18[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL18_COUNT 4

// Object data for layer 'level18'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level18[] = {
    22, 2, 1,
    22, 2, 4,
    22, 8, 5,
    22, 14, 5,
    22, 14, 8,
    12, 6, 14,
};

#define OBJECT_LEVEL18_COUNT 6

// Object sprite data for layer 'level18'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level18[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    12, 0, 32, 33, 0, 0,
    22, 0, 56, 57, 58, 59,
};

#define OBJECT_SPRITE_LEVEL18_COUNT 9

// Object palette data for layer 'level18' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level18[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL18_COUNT 3

// Player start location for layer 'level18'
#define SPAWN_X_LEVEL18 1
#define SPAWN_Y_LEVEL18 12

#endif // TILEMAP_LEVEL18_NES_H
