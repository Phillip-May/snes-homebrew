// Big chest GID to tile mapping
// Generated from baseCelesteSpriteSheet.png
// GID 96 for big chest (composed of sprites 96, 97, 112, 113)
// Each entry: 16 tiles for 4x4 grid (row-major order), palette_idx, flip_flags
// flip_flags: bit 0=H, bit 1=V, bit 2=D
// Big chest is 32x32 sprites (4x4 tiles) extracted from sprite sheet
// Arranged as: [Sprite 96] [Sprite 97] / [Sprite 112] [Sprite 113]

#ifndef GID_TO_TILE_BIG_CHEST_H
#define GID_TO_TILE_BIG_CHEST_H

#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5")))
#endif
const unsigned char gid_to_tile_big_chest[1][18] = {
    // GID 96 - BIG_CHEST (composed of sprites 96, 97)
    // 4x4 grid: Row 1: 213, 214, 214, 215 | Row 2: 103, 103, 103, 103 | Row 3: 230, 231, 234, 235 | Row 4: 232, 233, 236, 237
    { 213, 214, 214, 215, 103, 103, 103, 103, 230, 231, 234, 235, 232, 233, 236, 237, 0, 0 }
};

#define GID_TO_TILE_BIG_CHEST_COUNT 1

#endif // GID_TO_TILE_BIG_CHEST_H
