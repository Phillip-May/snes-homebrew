// Shared object sprite dictionary for all levels (NES format - compact)
// Generated from baseCelesteTileMap.json
// Compact format: only used entries stored, with lookup table for fast access
// Compact array format: [pal_idx, tl, tr, bl, br] (tile_idx not stored, use lookup table)

#ifndef OBJECT_SPRITE_DICT_SHARED_NES_H
#define OBJECT_SPRITE_DICT_SHARED_NES_H

// Lookup table: maps tile_idx -> compact_index (0xFF = not found)
const unsigned char object_sprite_lookup_table[33] = {
    /* tile 0 */  255,
    /* tile 1 */  0,
    /* tile 2 */  1,
    /* tile 3 */  2,
    /* tile 4 */  3,
    /* tile 5 */  4,
    /* tile 6 */  5,
    /* tile 7 */  6,
    /* tile 8 */  7,
    /* tile 9 */  8,
    /* tile 10 */  9,
    /* tile 11 */  10,
    /* tile 12 */  11,
    /* tile 13 */  12,
    /* tile 14 */  13,
    /* tile 15 */  14,
    /* tile 16 */  255,
    /* tile 17 */  255,
    /* tile 18 */  15,
    /* tile 19 */  16,
    /* tile 20 */  17,
    /* tile 21 */  255,
    /* tile 22 */  18,
    /* tile 23 */  19,
    /* tile 24 */  20,
    /* tile 25 */  21,
    /* tile 26 */  22,
    /* tile 27 */  255,
    /* tile 28 */  23,
    /* tile 29 */  255,
    /* tile 30 */  255,
    /* tile 31 */  255,
    /* tile 32 */  255
};

// Compact sprite data array (only used entries)
// Format: [pal_idx, tl, tr, bl, br]
const unsigned char object_sprite_dict_compact[24][5] = {
    /* compact_idx 0: tile 1 */  { 3, 1, 2, 3, 4 },
    /* compact_idx 1: tile 2 */  { 3, 1, 2, 3, 5 },
    /* compact_idx 2: tile 3 */  { 3, 6, 7, 8, 9 },
    /* compact_idx 3: tile 4 */  { 3, 1, 2, 10, 4 },
    /* compact_idx 4: tile 5 */  { 3, 11, 12, 13, 14 },
    /* compact_idx 5: tile 6 */  { 3, 15, 16, 17, 18 },
    /* compact_idx 6: tile 7 */  { 3, 19, 20, 21, 22 },
    /* compact_idx 7: tile 8 */  { 0, 23, 24, 25, 26 },
    /* compact_idx 8: tile 9 */  { 0, 27, 28, 29, 26 },
    /* compact_idx 9: tile 10 */  { 0, 0, 26, 0, 26 },
    /* compact_idx 10: tile 11 */  { 0, 30, 31, 0, 0 },
    /* compact_idx 11: tile 12 */  { 0, 32, 33, 0, 0 },
    /* compact_idx 12: tile 13 */  { 0, 34, 35, 36, 37 },
    /* compact_idx 13: tile 14 */  { 0, 0, 38, 39, 0 },
    /* compact_idx 14: tile 15 */  { 0, 39, 0, 0, 38 },
    /* compact_idx 15: tile 18 */  { 0, 46, 47, 48, 49 },
    /* compact_idx 16: tile 19 */  { 0, 0, 0, 46, 47 },
    /* compact_idx 17: tile 20 */  { 0, 50, 51, 52, 53 },
    /* compact_idx 18: tile 22 */  { 0, 56, 57, 58, 59 },
    /* compact_idx 19: tile 23 */  { 1, 60, 61, 62, 63 },
    /* compact_idx 20: tile 24 */  { 1, 64, 65, 66, 67 },
    /* compact_idx 21: tile 25 */  { 1, 68, 69, 70, 71 },
    /* compact_idx 22: tile 26 */  { 0, 72, 55, 73, 74 },
    /* compact_idx 23: tile 28 */  { 0, 77, 78, 79, 80 }
};

#define OBJECT_SPRITE_DICT_LOOKUP_TABLE_SIZE 33
#define OBJECT_SPRITE_DICT_COMPACT_COUNT 24

#endif // OBJECT_SPRITE_DICT_SHARED_NES_H
