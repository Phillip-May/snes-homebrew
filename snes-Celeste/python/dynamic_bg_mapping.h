// Dynamic BG tile mapping for collapse tiles
// Generated from baseCelesteTileMap.json
// Maps GID (24, 25, 26) to tile entries for collapse tiles
// GID 24 = COLLAPSE_TILE_SPRITE_1 (sprite index 23), GID 25 = COLLAPSE_TILE_SPRITE_2 (24), GID 26 = COLLAPSE_TILE_SPRITE_3 (25)
// Each entry: TL_tile, TR_tile, BL_tile, BR_tile, palette_idx, flip_flags
// flip_flags: bit 0=H, bit 1=V, bit 2=D

#ifndef DYNAMIC_BG_MAPPING_H
#define DYNAMIC_BG_MAPPING_H

const unsigned char dynamic_bg_mapping[3][6] = {
    // GID 24 (COLLAPSE_TILE_SPRITE_1, sprite index 23)
    { 60, 61, 62, 63, 0, 0 },
    // GID 25 (COLLAPSE_TILE_SPRITE_2, sprite index 24)
    { 0, 0, 0, 0, 0, 0 },
    // GID 26 (COLLAPSE_TILE_SPRITE_3, sprite index 25)
    { 0, 0, 0, 0, 0, 0 }
};

#define DYNAMIC_BG_MAPPING_COUNT 3
#define DYNAMIC_BG_MAPPING_GID_START 24
#define DYNAMIC_BG_MAPPING_GID_END 26

#endif // DYNAMIC_BG_MAPPING_H
