// Shared object sprite dictionary for all levels
// Generated from baseCelesteTileMap.json
// On NES: Each entry is indexed directly by tile_idx: [pal_idx, tl, tr, bl, br] (no tile_idx field needed)
// On SNES: Each entry: [tile_idx, pal_idx, tl, tr, bl, br]

#ifndef OBJECT_SPRITE_DICT_SHARED_H
#define OBJECT_SPRITE_DICT_SHARED_H

#ifdef __NES__
    // NES: Direct indexing by tile_idx (sparse array)
    // Format: [pal_idx, tl, tr, bl, br] - no tile_idx field needed since array is indexed by it
    // Array index = tile_idx (0-26), only used: 1-7 (player), 18-19 (spring), 23-25 (collapse tile), 26 (strawberry)
    const unsigned char object_sprite_dict_shared[27][5] = {
        /* 0: unused */  { 0, 0, 0, 0, 0 },
        /* 1: Player IDLE */  { 3, 1, 2, 3, 4 },
        /* 2: Player WALK_1 */  { 3, 1, 2, 3, 5 },
        /* 3: Player WALK_2 */  { 3, 6, 7, 8, 9 },
        /* 4: Player WALK_3 */  { 3, 1, 2, 10, 4 },
        /* 5: Player WALL */  { 3, 11, 12, 13, 14 },
        /* 6: Player DOWN */  { 3, 15, 16, 17, 18 },
        /* 7: Player UP */  { 3, 19, 20, 21, 22 },
        /* 8: unused */  { 0, 0, 0, 0, 0 },
        /* 9: unused */  { 0, 0, 0, 0, 0 },
        /* 10: unused */  { 0, 0, 0, 0, 0 },
        /* 11: unused */  { 0, 0, 0, 0, 0 },
        /* 12: unused */  { 0, 0, 0, 0, 0 },
        /* 13: unused */  { 0, 0, 0, 0, 0 },
        /* 14: unused */  { 0, 0, 0, 0, 0 },
        /* 15: unused */  { 0, 0, 0, 0, 0 },
        /* 16: unused */  { 0, 0, 0, 0, 0 },
        /* 17: unused */  { 0, 0, 0, 0, 0 },
        /* 18: Spring 1 */  { 2, 46, 47, 48, 49 },
        /* 19: Spring 2 */  { 2, 0, 0, 46, 47 },
        /* 20: unused */  { 0, 0, 0, 0, 0 },
        /* 21: unused */  { 0, 0, 0, 0, 0 },
        /* 22: unused */  { 0, 0, 0, 0, 0 },
        /* 23: Collapse tile 1 */  { 0, 61, 62, 63, 64 },
        /* 24: Collapse tile 2 */  { 0, 65, 66, 67, 68 },
        /* 25: Collapse tile 3 */  { 0, 69, 70, 71, 72 },
        /* 26: Strawberry */  { 1, 73, 74, 75, 76 }
    };
    
    #define OBJECT_SPRITE_DICT_SHARED_COUNT 27
#else
    // SNES/Other: Original format with tile_idx field for lookup
    const unsigned char object_sprite_dict_shared[27][6] = {
        // Entry 0
        { 1, 3, 1, 2, 3, 4 },
        // Entry 1
        { 2, 3, 1, 2, 3, 5 },
        // Entry 2
        { 3, 3, 6, 7, 8, 9 },
        // Entry 3
        { 4, 3, 1, 2, 10, 4 },
        // Entry 4
        { 5, 3, 11, 12, 13, 14 },
        // Entry 5
        { 6, 3, 15, 16, 17, 18 },
        // Entry 6
        { 7, 3, 19, 20, 21, 22 },
        // Entry 7
        { 8, 0, 23, 24, 25, 26 },
        // Entry 8
        { 11, 0, 30, 31, 0, 0 },
        // Entry 9
        { 12, 0, 32, 33, 0, 0 },
        // Entry 10
        { 18, 2, 46, 47, 48, 49 },
        // Entry 11
        { 19, 2, 0, 0, 46, 47 },
        // Entry 12
        { 20, 0, 50, 51, 52, 53 },
        // Entry 13
        { 22, 1, 57, 58, 59, 60 },
        // Entry 14
        { 23, 0, 61, 62, 63, 64 },
        // Entry 15
        { 24, 0, 65, 66, 67, 68 },
        // Entry 16
        { 25, 0, 69, 70, 71, 72 },
        // Entry 17
        { 26, 1, 73, 74, 75, 76 },
        // Entry 18
        { 28, 1, 79, 80, 81, 82 },
        // Entry 19
        { 64, 2, 168, 169, 170, 104 },
        // Entry 20
        { 70, 1, 0, 0, 186, 187 },
        // Entry 21
        { 71, 1, 0, 0, 188, 189 },
        // Entry 22
        { 86, 1, 208, 209, 210, 104 },
        // Entry 23
        { 87, 1, 211, 212, 104, 213 },
        // Entry 24
        { 96, 0, 227, 228, 229, 230 },
        // Entry 25
        { 97, 0, 228, 231, 230, 232 },
        // Entry 26
        { 118, 2, 269, 270, 271, 0 }
    };
    
    #define OBJECT_SPRITE_DICT_SHARED_COUNT 27
#endif

#endif // OBJECT_SPRITE_DICT_SHARED_H
