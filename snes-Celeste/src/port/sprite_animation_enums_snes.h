#ifndef SPRITE_ANIMATION_ENUMS_SNES_H
#define SPRITE_ANIMATION_ENUMS_SNES_H

// SNES-specific sprite animation enums (AUTO-GENERATED)
// On SNES, values are OAM tile numbers

enum ePlayerSprite {
    PLAYER_SPRITE_IDLE = 0x00,
    PLAYER_SPRITE_WALK_1 = 0x02,
    PLAYER_SPRITE_WALK_2 = 0x04,
    PLAYER_SPRITE_WALK_3 = 0x06,
    PLAYER_SPRITE_WALL = 0x08,
    PLAYER_SPRITE_DOWN = 0x0A,
    PLAYER_SPRITE_UP = 0x0C
};

enum eKeySprite {
    KEY_SPRITE_1 = 0x0E,
    KEY_SPRITE_2 = 0x20,
    KEY_SPRITE_3 = 0x22
};

enum ePlatMovSprite {
    PLATMOV_SPRITE_1 = 0x24,
    PLATMOV_SPRITE_2 = 0x26
};

enum eBalloonSprite {
    BALLOON_STRING_1 = 0x28,
    BALLOON_STRING_2 = 0x2A,
    BALLOON_STRING_3 = 0x2C,
    BALLOON_SPRITE_1 = 0x46
};

enum eSpringSprite {
    SPRING_SPRITE_1 = 0x2E,
    SPRING_SPRITE_2 = 0x40
};

enum eChestSprite {
    CHEST_SPRITE_1 = 0x42
};

enum eCollapseTileSprite {
    COLLAPSE_TILE_SPRITE_1 = 0x48,
    COLLAPSE_TILE_SPRITE_2 = 0x4A,
    COLLAPSE_TILE_SPRITE_3 = 0x4C
};

enum eStrawberrySprite {
    STRAWBERRY_SPRITE_1 = 0x4E
};

enum eFlyingBerrySprite {
    FLYING_BERRY_SPRITE_1 = 0x60,
    WING_SPRITE_1 = 0x68,
    WING_SPRITE_2 = 0x6A,
    WING_SPRITE_3 = 0x6C
};

enum smokeStates {
    SMOKE_SPRITE_1 = 0x62,
    SMOKE_SPRITE_2 = 0x64,
    SMOKE_SPRITE_3 = 0x66
};

enum eFlowerSprite {
    FLOWER_SPRITE_1 = 0x80
};

enum eBreakableWallSprite {
    BREAKABLE_WALL_SPRITE_1 = 0x82
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

enum eFlagSprite {
    FLAG_SPRITE_1 = 0xA2,
    FLAG_SPRITE_2 = 0xA4,
    FLAG_SPRITE_3 = 0xA6
};

enum eDecoTreeSprite {
    DECO_TREE_SPRITE_1 = 0x6E
};

#endif // SPRITE_ANIMATION_ENUMS_SNES_H
