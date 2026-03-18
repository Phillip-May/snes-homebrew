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
    // 4x4 grid: Row 1: 227, 228, 228, 229 | Row 2: 117, 117, 117, 117 | Row 3: 244, 245, 248, 249 | Row 4: 246, 247, 250, 251
    { 227, 228, 228, 229, 117, 117, 117, 117, 244, 245, 248, 249, 246, 247, 250, 251, 0, 0 }
};

#define GID_TO_TILE_BIG_CHEST_COUNT 1

#endif // GID_TO_TILE_BIG_CHEST_H
