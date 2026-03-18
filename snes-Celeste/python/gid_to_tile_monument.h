// Monument GID to tile mapping
// Generated from baseCelesteSpriteSheet.png
// GID 70 for monuments (composed of sprites 70, 71, 86, 87)
// Each entry: 16 tiles for 4x4 grid (row-major order), palette_idx, flip_flags
// flip_flags: bit 0=H, bit 1=V, bit 2=D
// Monuments are 32x32 sprites (4x4 tiles) extracted from sprite sheet
// Arranged as: [Sprite 70] [Sprite 71] / [Sprite 86] [Sprite 87]

#ifndef GID_TO_TILE_MONUMENT_H
#define GID_TO_TILE_MONUMENT_H

#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5")))
#endif
const unsigned char gid_to_tile_monument[1][18] = {
    // GID 70 - MONUMENT (composed of sprites 70, 71, 86, 87)
    // 4x4 grid: Row 1: 0, 0, 0, 0 | Row 2: 200, 201, 202, 203 | Row 3: 221, 222, 224, 225 | Row 4: 223, 125, 125, 226
    { 0, 0, 0, 0, 200, 201, 202, 203, 221, 222, 224, 225, 223, 125, 125, 226, 0, 0 }
};

#define GID_TO_TILE_MONUMENT_COUNT 1

#endif // GID_TO_TILE_MONUMENT_H
