// NES tilemap data for layer 'level27'
// Generated from baseCelesteTileMap.json

#ifndef TILEMAP_LEVEL27_NES_H
#define TILEMAP_LEVEL27_NES_H

// Tilemap dimensions: 16x16 tiles (NES 8x8)
#define TILEMAP_LEVEL27_WIDTH 16
#define TILEMAP_LEVEL27_HEIGHT 16

// Using shared GID mapping (see gid_to_tile_shared.h)
#include "gid_to_tile_shared.h"
// Using shared compression dictionary (see compression_dict_shared.h)
#include "compression_dict_shared.h"

// Compressed tilemap data for layer 'level27'
// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence
__attribute__((section(".prg_rom_1"))) const unsigned char tilemap_level27_compressed[] = {
    0xc2, 0xc2, 0xc2, 0xc2, 0xc2, 0xd5, 0xdf, 0x29, 0x09, 0xc2, 0xd2, 0xc2, 0xc3, 0xc3, 0xc9, 0xd6,
    0xf4, 0xc2, 0xc2, 0x03, 0x06, 0xd6, 0xd6, 0xfd, 0xc0, 0xc5, 0xc3, 0xc2, 0xed, 0xc0, 0xc0, 0x2a,
    0x23, 0xc0, 0xd7, 0xde, 0xdc, 0x06, 0xd0, 0xc0, 0xce, 0x21, 0xd9, 0xc6, 0xc2, 0xcf, 0x01, 0xd0,
    0xc0, 0xce, 0x21, 0xc4, 0xc0, 0xc2, 0xd5, 0xed, 0xc0, 0xe0, 0xf8, 0xd0, 0xc0, 0xc2, 0xc2, 0xed,
    0xce, 0x23, 0xce, 0x21, 0xd0, 0xc0, 0xc3, 0xc3, 0xed, 0xce, 0x21, 0xce, 0x21, 0xc0, 0xc0, 0xd6,
    0xd6, 0x22, 0xd0, 0xf8, 0x12, 0xf8, 0xc0, 0xc0, 0xc0, 0xd1, 0xc7, 0xce, 0x21, 0x09, 0x2a, 0x22,
    0xc0, 0xc0, 0xc0, 0x00, 0x0b, 0x0c, 0xc4, 0xf8, 0xc1, 0xc7, 0xc0, 0xc0, 0xc0, 0xc5, 0xc0, 0xf8,
    0xcd, 0xc4, 0xc0, 0xc0, 0xc0, 0xf3, 0x1c, 0xce, 0x21, 0xc6, 0xf2, 0xc0, 0xc0, 0xd1, 0xd3, 0xc4,
    0x2a, 0x22, 0xc0, 0x0c, 0xc4, 0xc0, 0x01, 0x00, 0xe2, 0xc7, 0xc0, 0xfc, 0xc5, 0x0b, 0xc0, 0x00
};

#define TILEMAP_LEVEL27_COMPRESSED_SIZE 144
#define TILEMAP_LEVEL27_COUNT 256

// Background palette data for layer 'level27' (NES 6-bit format)
// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]
// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)
__attribute__((section(".prg_rom_1"))) const unsigned char palette_background_level27[4][4] = {
    // Background Palette 0
    { 0x0d, 0x20, 0x21, 0x00 },
    // Background Palette 1
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 2
    { 0x0d, 0x0d, 0x0d, 0x0d },
    // Background Palette 3
    { 0x0d, 0x0d, 0x0d, 0x0d }
};

#define PALETTE_BACKGROUND_LEVEL27_COUNT 1

// Sprite palette data for layer 'level27' (NES 6-bit format)
// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]
// Used by non-background tiles
// Palette 3 is reserved for player sprites
__attribute__((section(".prg_rom_1"))) const unsigned char palette_sprite_level27[4][4] = {
    // Sprite Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Sprite Palette 1
    { 0x0d, 0x20, 0x21, 0x00 },
    // Sprite Palette 2
    { 0x0d, 0x25, 0x27, 0x10 },
    // Sprite Palette 3 (Player)
    { 0x0d, 0x15, 0x37, 0x1b }
};

#define PALETTE_SPRITE_LEVEL27_COUNT 4

// Object data for layer 'level27'
__attribute__((section(".prg_rom_1"))) const unsigned char object_level27[] = {
    22, 6, 8,
    22, 12, 9,
};

#define OBJECT_LEVEL27_COUNT 2

// Object sprite data for layer 'level27'
// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)
// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]
__attribute__((section(".prg_rom_1"))) const unsigned char object_sprite_level27[] = {
    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]
    1, 3, 1, 2, 3, 4,
    2, 3, 1, 2, 3, 5,
    3, 3, 6, 7, 8, 9,
    4, 3, 1, 2, 10, 4,
    5, 3, 11, 12, 13, 14,
    6, 3, 15, 16, 17, 18,
    7, 3, 19, 20, 21, 22,
    22, 0, 56, 57, 58, 59,
    23, 1, 60, 61, 62, 63,
    64, 1, 97, 167, 168, 100,
};

#define OBJECT_SPRITE_LEVEL27_COUNT 10

// Object palette data for layer 'level27' (NES 6-bit format)
// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]
// Note: Palette 3 is reserved for the player
__attribute__((section(".prg_rom_1"))) const unsigned char palette_object_level27[3][4] = {
    // Object Palette 0
    { 0x0d, 0x20, 0x28, 0x15 },
    // Object Palette 1
    { 0x0d, 0x25, 0x27, 0x10 },
    // Object Palette 2
    { 0x0d, 0x1a, 0x04, 0x17 }
};

#define PALETTE_OBJECT_LEVEL27_COUNT 3

// Player start location for layer 'level27'
#define SPAWN_X_LEVEL27 0
#define SPAWN_Y_LEVEL27 14

#endif // TILEMAP_LEVEL27_NES_H
