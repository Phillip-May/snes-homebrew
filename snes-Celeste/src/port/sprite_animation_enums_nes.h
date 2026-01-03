#ifndef SPRITE_ANIMATION_ENUMS_NES_H
#define SPRITE_ANIMATION_ENUMS_NES_H

// NES-specific sprite animation enums
// On NES, values directly map to tile indices

// Player sprite states - values map to tile indices (1-7)
enum ePlayerSprite {
    PLAYER_SPRITE_IDLE = 1,
    PLAYER_SPRITE_WALK_1 = 2,
    PLAYER_SPRITE_WALK_2 = 3,
    PLAYER_SPRITE_WALK_3 = 4,
    PLAYER_SPRITE_WALL = 5,
    PLAYER_SPRITE_DOWN = 6,
    PLAYER_SPRITE_UP = 7
};

// Spring sprite states - values map to tile indices (18-19)
enum eSpringSprite {
    SPRING_SPRITE_1 = 18,
    SPRING_SPRITE_2 = 19
};

// Collapse tile sprite states - values map to tile indices (23-25)
enum eCollapseTileSprite {
    COLLAPSE_TILE_SPRITE_1 = 23,
    COLLAPSE_TILE_SPRITE_2 = 24,
    COLLAPSE_TILE_SPRITE_3 = 25
};

// Strawberry sprite state - value maps to tile index (26)
enum eStrawberrySprite {
    STRAWBERRY_SPRITE_1 = 26
};

// Flying berry sprite state - value maps to tile index (28)
enum eFlyingBerrySprite {
    FLYING_BERRY_SPRITE_1 = 28
};

// Platform-independent sprite enums (shared with SNES)
enum eBreakableWallSprite {
    BREAKABLE_WALL_SPRITE_1 = 0x40
};

enum eFlowerSprite {
    FLOWER_SPRITE_1 = 0x3E
};

enum eBalloonSprite {
    BALLOON_SPRITE_1 = 0x16,
    BALLOON_STRING_1 = 13,
    BALLOON_STRING_2 = 14,
    BALLOON_STRING_3 = 15
};

enum ePlatMovSprite {
    PLATMOV_SPRITE_1 = 11,
    PLATMOV_SPRITE_2 = 12
};

enum eKeySprite {
    KEY_SPRITE_1 = 8,
    KEY_SPRITE_2 = 9,
    KEY_SPRITE_3 = 10
};

enum eChestSprite {
    CHEST_SPRITE_1 = 0x42
};

enum eMonumentSprite {
    MONUMENT_SPRITE_1 = 0x84,
    MONUMENT_SPRITE_2 = 0x86,
    MONUMENT_SPRITE_3 = 0x88,
    MONUMENT_SPRITE_4 = 0x8A
};

enum eBigChestSprite {
    BIG_CHEST_SPRITE_1 = 0x8C,
    BIG_CHEST_SPRITE_2 = 0x8E
};

enum eDoubleJumpOrbSprite {
    DOUBLE_JUMP_ORB_SPRITE_1 = 0xA0
};

enum eDecoTreeSprite {
    DECO_TREE_SPRITE_1 = 0x6E
};

enum smokeStates {
    SMOKE_SPRITE_1 = 0x62,
    SMOKE_SPRITE_2 = 0x64,
    SMOKE_SPRITE_3 = 0x66
};

#endif // SPRITE_ANIMATION_ENUMS_NES_H

