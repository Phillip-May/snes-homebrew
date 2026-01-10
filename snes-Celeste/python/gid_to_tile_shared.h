// Shared GID to tile mapping for all levels
// Generated from baseCelesteTileMap.json
// Each entry: TL_tile, TR_tile, BL_tile, BR_tile, palette_idx, flip_flags
// flip_flags: bit 0=H, bit 1=V, bit 2=D
// GID 0 is reserved for empty tiles
// GIDs 24, 25, 26 are reserved for collapse tiles (see gid_to_tile_collapse.h)
// GID 27 is reserved for breakable walls (see gid_to_tile_breakable_wall.h)
// GIDs 70, 71, 86, 87 are reserved for monuments (see gid_to_tile_monument.h)

#ifndef GID_TO_TILE_SHARED_H
#define GID_TO_TILE_SHARED_H

#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5"))) const unsigned char gid_to_tile_shared[65][6] = {
#else
const unsigned char gid_to_tile_shared[65][6] = {
#endif
    // GID 0
    { 0, 0, 0, 0, 0, 0 },
    // GID 1
    { 98, 104, 105, 106, 0, 0 },
    // GID 2
    { 99, 130, 131, 132, 0, 0 },
    // GID 3
    { 103, 103, 103, 103, 0, 0 },
    // GID 4
    { 188, 189, 190, 189, 0, 0 },
    // GID 5
    { 103, 103, 133, 134, 0, 0 },
    // GID 6
    { 135, 106, 132, 136, 0, 0 },
    // GID 7
    { 107, 103, 108, 103, 0, 0 },
    // GID 8
    { 112, 113, 113, 0, 0, 0 },
    // GID 9
    { 112, 112, 112, 112, 0, 0 },
    // GID 10
    { 93, 94, 95, 96, 0, 0 },
    // GID 11
    { 40, 41, 42, 43, 0, 0 },
    // GID 12
    { 149, 41, 150, 112, 0, 0 },
    // GID 13
    { 114, 112, 0, 114, 0, 0 },
    // GID 14
    { 0, 0, 158, 159, 1, 0 },
    // GID 15
    { 0, 164, 165, 166, 1, 0 },
    // GID 16
    { 97, 167, 168, 100, 2, 0 },
    // GID 17
    { 0, 152, 152, 112, 0, 0 },
    // GID 18
    { 151, 0, 112, 151, 0, 0 },
    // GID 19
    { 97, 137, 131, 138, 0, 0 },
    // GID 20
    { 139, 140, 141, 142, 0, 0 },
    // GID 21
    { 101, 102, 103, 103, 0, 0 },
    // GID 22
    { 103, 109, 103, 110, 0, 0 },
    // GID 23
    { 143, 104, 144, 136, 0, 0 },
    // GID 24
    { 60, 61, 62, 63, 0, 0 },
    // GID 25
    { 64, 65, 66, 67, 0, 0 },
    // GID 26
    { 68, 69, 70, 71, 0, 0 },
    // GID 27
    { 0, 0, 0, 0, 0, 0 },
    // GID 28
    { 151, 0, 112, 112, 0, 0 },
    // GID 29
    { 0, 0, 112, 112, 0, 0 },
    // GID 30
    { 160, 161, 162, 163, 3, 0 },
    // GID 31
    { 97, 98, 99, 100, 0, 0 },
    // GID 32
    { 44, 44, 45, 45, 0, 0 },
    // GID 33
    { 127, 128, 127, 129, 0, 0 },
    // GID 34
    { 145, 146, 147, 148, 0, 0 },
    // GID 35
    { 160, 161, 162, 163, 1, 0 },
    // GID 36
    { 0, 0, 158, 159, 2, 0 },
    // GID 37
    { 97, 104, 111, 106, 0, 0 },
    // GID 38
    { 0, 152, 112, 112, 0, 0 },
    // GID 39
    { 0, 164, 165, 166, 2, 0 },
    // GID 40
    { 117, 118, 119, 120, 0, 0 },
    // GID 41
    { 154, 155, 156, 157, 1, 0 },
    // GID 42
    { 160, 161, 162, 163, 2, 0 },
    // GID 43
    { 75, 75, 76, 76, 0, 0 },
    // GID 44
    { 115, 116, 115, 116, 0, 0 },
    // GID 45
    { 90, 153, 90, 153, 0, 0 },
    // GID 46
    { 154, 155, 156, 157, 2, 0 },
    // GID 47
    { 97, 167, 168, 100, 1, 0 },
    // GID 48
    { 195, 196, 197, 0, 0, 0 },
    // GID 49
    { 171, 172, 173, 174, 0, 0 },
    // GID 50
    { 198, 199, 200, 0, 0, 0 },
    // GID 51
    { 175, 176, 177, 0, 0, 0 },
    // GID 52
    { 219, 200, 220, 221, 0, 0 },
    // GID 53
    { 0, 180, 218, 222, 0, 0 },
    // GID 54
    { 197, 216, 217, 218, 0, 0 },
    // GID 55
    { 175, 178, 179, 180, 0, 0 },
    // GID 56
    { 0, 201, 0, 180, 0, 0 },
    // GID 57
    { 181, 176, 238, 221, 0, 0 },
    // GID 58
    { 175, 176, 239, 221, 0, 0 },
    // GID 59
    { 175, 178, 239, 222, 0, 0 },
    // GID 60
    { 202, 201, 203, 204, 0, 0 },
    // GID 61
    { 223, 224, 217, 225, 0, 0 },
    // GID 62
    { 230, 231, 232, 233, 2, 0 },
    // GID 63
    { 234, 235, 236, 237, 2, 0 },
    // GID 64
    { 0, 0, 0, 0, 6, 0 }
};

#define GID_TO_TILE_SHARED_COUNT 65

// GID to collision flags mapping
// Collision flags: 0 = no collision, 1 = solid, 4/8/16/32 = pointy variants
#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5"))) const unsigned char gid_to_collision[GID_TO_TILE_SHARED_COUNT] = {
#else
const unsigned char gid_to_collision[GID_TO_TILE_SHARED_COUNT] = {
#endif
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
    0,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    4,
    1,
    1,
    0,
    0,
    1,
    0,
    0,
    0,
    0,
    0,
    8,
    16,
    32,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
    0,
    0
};

#define GID_TO_COLLISION_COUNT GID_TO_TILE_SHARED_COUNT

#endif // GID_TO_TILE_SHARED_H
