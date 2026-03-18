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
    // 4x4 grid: Row 1: 110, 183, 185, 117 | Row 2: 184, 113, 118, 186 | Row 3: 207, 145, 150, 209 | Row 4: 146, 208, 210, 151
    { 110, 183, 185, 117, 184, 113, 118, 186, 207, 145, 150, 209, 146, 208, 210, 151, 0, 0 }
};

#define GID_TO_TILE_BREAKABLE_WALL_COUNT 1

#endif // GID_TO_TILE_BREAKABLE_WALL_H
