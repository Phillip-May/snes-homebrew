// Breakable wall GID to tile mapping
// Generated from baseCelesteSpriteSheet.png
// GID 27 for breakable walls (sprite index 64)
// Each entry: 16 tiles for 4x4 grid (row-major order), palette_idx, flip_flags
// flip_flags: bit 0=H, bit 1=V, bit 2=D
// Pattern repeats the 2x2 block (TL,TR,BL,BR) 4 times to make 4x4

#ifndef GID_TO_TILE_BREAKABLE_WALL_H
#define GID_TO_TILE_BREAKABLE_WALL_H

#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5")))
#endif
const unsigned char gid_to_tile_breakable_wall[1][18] = {
    // GID 27 - BREAKABLE_WALL_SPRITE_1
    // 4x4 grid: Row 1: 97,167,97,167 | Row 2: 168,100,168,100 | Row 3: 97,167,97,167 | Row 4: 168,100,168,100
    { 97, 167, 97, 167, 168, 100, 168, 100, 97, 167, 97, 167, 168, 100, 168, 100, 0, 0 }
};

#define GID_TO_TILE_BREAKABLE_WALL_COUNT 1

#endif // GID_TO_TILE_BREAKABLE_WALL_H
