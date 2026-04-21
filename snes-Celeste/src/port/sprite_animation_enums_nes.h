#ifndef SPRITE_ANIMATION_ENUMS_NES_H
#define SPRITE_ANIMATION_ENUMS_NES_H

// NES-specific sprite animation enums (AUTO-GENERATED)
// On NES, values are tile indices

enum ePlayerSprite {
    PLAYER_SPRITE_IDLE = 0x01,
    PLAYER_SPRITE_WALK_1 = 0x02,
    PLAYER_SPRITE_WALK_2 = 0x03,
    PLAYER_SPRITE_WALK_3 = 0x04,
    PLAYER_SPRITE_WALL = 0x05,
    PLAYER_SPRITE_DOWN = 0x06,
    PLAYER_SPRITE_UP = 0x07
};

enum eKeySprite {
    KEY_SPRITE_1 = 0x08,
    KEY_SPRITE_2 = 0x09,
    KEY_SPRITE_3 = 0x0A
};

enum ePlatMovSprite {
    PLATMOV_SPRITE_1 = 0x0B,
    PLATMOV_SPRITE_2 = 0x0C
};

enum eBalloonSprite {
    BALLOON_STRING_1 = 0x0D,
    BALLOON_STRING_2 = 0x0E,
    BALLOON_STRING_3 = 0x0F,
    BALLOON_SPRITE_1 = 0x16
};

enum eSpringSprite {
    SPRING_SPRITE_1 = 0x12,
    SPRING_SPRITE_2 = 0x13
};

enum eChestSprite {
    CHEST_SPRITE_1 = 0x14
};

enum eCollapseTileSprite {
    COLLAPSE_TILE_SPRITE_1 = 0x17,
    COLLAPSE_TILE_SPRITE_2 = 0x18,
    COLLAPSE_TILE_SPRITE_3 = 0x19
};

enum eStrawberrySprite {
    STRAWBERRY_SPRITE_1 = 0x1A
};

enum eFlyingBerrySprite {
    FLYING_BERRY_SPRITE_1 = 0x1C,
    FLYING_BERRY_WING_UP = 0x2D,
    FLYING_BERRY_WING_MID = 0x2E,
    FLYING_BERRY_WING_DOWN = 0x2F
};

enum smokeStates {
    SMOKE_SPRITE_1 = 0x1D,
    SMOKE_SPRITE_2 = 0x1E,
    SMOKE_SPRITE_3 = 0x1F
};

enum eFlowerSprite {
    FLOWER_SPRITE_1 = 0x3C
};

enum eBreakableWallSprite {
    BREAKABLE_WALL_SPRITE_1 = 0x40
};

enum eMonumentSprite {
    MONUMENT_SPRITE_1 = 0x46,
    MONUMENT_SPRITE_2 = 0x47,
    MONUMENT_SPRITE_3 = 0x56,
    MONUMENT_SPRITE_4 = 0x57
};

enum eBigChestSprite {
    BIG_CHEST_SPRITE_1 = 0x60,
    BIG_CHEST_SPRITE_2 = 0x61
};

enum eDoubleJumpOrbSprite {
    DOUBLE_JUMP_ORB_SPRITE_1 = 0x66
};

enum eFlagSprite {
    FLAG_SPRITE_1 = 0x76,
    FLAG_SPRITE_2 = 0x77,
    FLAG_SPRITE_3 = 0x78
};

enum eDecoTreeSprite {
    DECO_TREE_SPRITE_1 = 0x6E
};

#endif // SPRITE_ANIMATION_ENUMS_NES_H
