#ifndef SPRITE_ANIMATION_ENUMS_SNES_H
#define SPRITE_ANIMATION_ENUMS_SNES_H

// SNES-specific sprite animation enums
// On SNES, values are OAM tile numbers

// Player sprite states - OAM tile numbers
enum ePlayerSprite {
    PLAYER_SPRITE_IDLE = 0,
    PLAYER_SPRITE_WALK_1 = 2,
    PLAYER_SPRITE_WALK_2 = 4,
    PLAYER_SPRITE_WALK_3 = 6,
    PLAYER_SPRITE_WALL = 8,
    PLAYER_SPRITE_DOWN = 10,
    PLAYER_SPRITE_UP = 12
};

// Spring sprite states - OAM tile numbers
enum eSpringSprite {
    SPRING_SPRITE_1 = 0x2E,
    SPRING_SPRITE_2 = 0x40
};

// Collapse tile sprite states - OAM tile numbers
enum eCollapseTileSprite {
    COLLAPSE_TILE_SPRITE_1 = 0x48,
    COLLAPSE_TILE_SPRITE_2 = 0x4A,
    COLLAPSE_TILE_SPRITE_3 = 0x4C
};

// Strawberry sprite state - OAM tile number
enum eStrawberrySprite {
    STRAWBERRY_SPRITE_1 = 0x4E
};

// Flying berry sprite state - OAM tile numbers
enum eFlyingBerrySprite {
    FLYING_BERRY_SPRITE_1 = 0x60,
    WING_SPRITE_1 = 0x68,
    WING_SPRITE_2 = 0x6A,
    WING_SPRITE_3 = 0x6C
};

// Platform-independent sprite enums (shared with NES)
enum eBreakableWallSprite {
    BREAKABLE_WALL_SPRITE_1 = 0x82
};

enum eFlowerSprite {
    FLOWER_SPRITE_1 = 0x80
};

enum eBalloonSprite {
    BALLOON_SPRITE_1 = 0x46,
    BALLOON_STRING_1 = 0x28,
    BALLOON_STRING_2 = 0x2A,
    BALLOON_STRING_3 = 0x2C
};

enum ePlatMovSprite {
    PLATMOV_SPRITE_1 = 0x24,
    PLATMOV_SPRITE_2 = 0x26
};

enum eKeySprite {
    KEY_SPRITE_1 = 0x0E,
    KEY_SPRITE_2 = 0x20,
    KEY_SPRITE_3 = 0x22
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

#endif // SPRITE_ANIMATION_ENUMS_SNES_H

