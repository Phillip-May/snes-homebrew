// Breakable wall GID to tile mapping
// Generated from baseCelesteSpriteSheet.png
// GID 27 for breakable walls (sprite index 64)
// Each entry: 16 tiles for 4x4 grid (row-major order), palette_idx, flip_flags
// flip_flags: bit 0=H, bit 1=V, bit 2=D
// Breakable walls are 32x32 sprites (4x4 tiles) extracted from sprite sheet

#ifndef GID_TO_TILE_BREAKABLE_WALL_H
#define GID_TO_TILE_BREAKABLE_WALL_H

#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5")))
#endif
const unsigned char gid_to_tile_breakable_wall[1][18] = {
    // GID 27 - BREAKABLE_WALL_SPRITE_1
    // 4x4 grid: Row 1: 99, 170, 172, 106 | Row 2: 171, 102, 107, 173 | Row 3: 194, 133, 138, 196 | Row 4: 134, 195, 197, 139
    { 99, 170, 172, 106, 171, 102, 107, 173, 194, 133, 138, 196, 134, 195, 197, 139, 0, 0 }
};

#define GID_TO_TILE_BREAKABLE_WALL_COUNT 1

#endif // GID_TO_TILE_BREAKABLE_WALL_H
