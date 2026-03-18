// Collapse tile GID to tile mapping
// Generated from baseCelesteSpriteSheet.png
// GIDs 24, 25, 26 for collapse tiles (sprite indices 23, 24, 25)
// Each entry: TL_tile, TR_tile, BL_tile, BR_tile, palette_idx, flip_flags
// flip_flags: bit 0=H, bit 1=V, bit 2=D

#ifndef GID_TO_TILE_COLLAPSE_H
#define GID_TO_TILE_COLLAPSE_H

#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5")))
#endif
const unsigned char gid_to_tile_collapse[3][6] = {
    // GID 24 - COLLAPSE_TILE_SPRITE_1 (idle)
    { 61, 62, 63, 64, 0, 0 },
    // GID 25 - COLLAPSE_TILE_SPRITE_2 (collapsing)
    { 65, 66, 67, 68, 0, 0 },
    // GID 26 - COLLAPSE_TILE_SPRITE_3 (collapsing)
    { 69, 70, 71, 72, 0, 0 }
};

#define GID_TO_TILE_COLLAPSE_COUNT 3

#endif // GID_TO_TILE_COLLAPSE_H
