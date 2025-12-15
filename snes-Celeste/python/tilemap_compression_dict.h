// Shared compression dictionary for tilemap RLE compression
// Generated from baseCelesteTileMap.json
// Common patterns found across all levels

#ifndef TILEMAP_COMPRESSION_DICT_H
#define TILEMAP_COMPRESSION_DICT_H

// Dictionary with 63 patterns
// Format: pattern length, then pattern bytes
const unsigned char tilemap_compression_dict[] = {
    // Pattern 0: 2 bytes
    2, 0, 0,
    // Pattern 1: 3 bytes
    3, 0, 0, 0,
    // Pattern 2: 4 bytes
    4, 0, 0, 0, 0,
    // Pattern 3: 2 bytes
    2, 3, 3,
    // Pattern 4: 3 bytes
    3, 3, 3, 3,
    // Pattern 5: 4 bytes
    4, 3, 3, 3, 3,
    // Pattern 6: 2 bytes
    2, 9, 9,
    // Pattern 7: 3 bytes
    3, 8, 0, 0,
    // Pattern 8: 3 bytes
    3, 0, 0, 9,
    // Pattern 9: 2 bytes
    2, 5, 5,
    // Pattern 10: 2 bytes
    2, 7, 3,
    // Pattern 11: 2 bytes
    2, 8, 0,
    // Pattern 12: 2 bytes
    2, 16, 9,
    // Pattern 13: 4 bytes
    4, 9, 8, 0, 0,
    // Pattern 14: 4 bytes
    4, 8, 0, 0, 0,
    // Pattern 15: 2 bytes
    2, 0, 9,
    // Pattern 16: 2 bytes
    2, 9, 12,
    // Pattern 17: 2 bytes
    2, 20, 3,
    // Pattern 18: 3 bytes
    3, 5, 5, 5,
    // Pattern 19: 3 bytes
    3, 9, 8, 0,
    // Pattern 20: 3 bytes
    3, 9, 9, 9,
    // Pattern 21: 3 bytes
    3, 16, 9, 9,
    // Pattern 22: 3 bytes
    3, 0, 0, 16,
    // Pattern 23: 3 bytes
    3, 20, 20, 3,
    // Pattern 24: 4 bytes
    4, 3, 3, 4, 3,
    // Pattern 25: 4 bytes
    4, 3, 4, 3, 3,
    // Pattern 26: 4 bytes
    4, 16, 9, 9, 9,
    // Pattern 27: 4 bytes
    4, 0, 0, 0, 16,
    // Pattern 28: 4 bytes
    4, 0, 0, 16, 9,
    // Pattern 29: 4 bytes
    4, 0, 0, 0, 9,
    // Pattern 30: 4 bytes
    4, 20, 20, 3, 3,
    // Pattern 31: 4 bytes
    4, 20, 3, 3, 3,
    // Pattern 32: 2 bytes
    2, 9, 8,
    // Pattern 33: 2 bytes
    2, 20, 20,
    // Pattern 34: 3 bytes
    3, 3, 3, 4,
    // Pattern 35: 3 bytes
    3, 3, 4, 3,
    // Pattern 36: 3 bytes
    3, 4, 3, 3,
    // Pattern 37: 3 bytes
    3, 5, 5, 6,
    // Pattern 38: 3 bytes
    3, 0, 0, 13,
    // Pattern 39: 3 bytes
    3, 0, 9, 12,
    // Pattern 40: 3 bytes
    3, 9, 12, 9,
    // Pattern 41: 3 bytes
    3, 9, 0, 0,
    // Pattern 42: 3 bytes
    3, 0, 16, 9,
    // Pattern 43: 3 bytes
    3, 9, 7, 3,
    // Pattern 44: 3 bytes
    3, 3, 0, 0,
    // Pattern 45: 3 bytes
    3, 3, 20, 20,
    // Pattern 46: 3 bytes
    3, 20, 3, 3,
    // Pattern 47: 2 bytes
    2, 12, 9,
    // Pattern 48: 2 bytes
    2, 3, 20,
    // Pattern 49: 2 bytes
    2, 0, 16,
    // Pattern 50: 2 bytes
    2, 9, 7,
    // Pattern 51: 2 bytes
    2, 27, 27,
    // Pattern 52: 4 bytes
    4, 5, 5, 5, 5,
    // Pattern 53: 4 bytes
    4, 5, 5, 5, 6,
    // Pattern 54: 4 bytes
    4, 0, 0, 7, 3,
    // Pattern 55: 4 bytes
    4, 9, 9, 8, 0,
    // Pattern 56: 4 bytes
    4, 0, 9, 12, 9,
    // Pattern 57: 4 bytes
    4, 9, 0, 0, 0,
    // Pattern 58: 4 bytes
    4, 0, 0, 9, 12,
    // Pattern 59: 4 bytes
    4, 0, 0, 13, 9,
    // Pattern 60: 4 bytes
    4, 3, 0, 0, 0,
    // Pattern 61: 4 bytes
    4, 9, 7, 3, 0,
    // Pattern 62: 4 bytes
    4, 7, 3, 0, 0
};

#define TILEMAP_COMPRESSION_DICT_COUNT 63

#endif // TILEMAP_COMPRESSION_DICT_H
