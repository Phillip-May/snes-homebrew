#ifndef SPRITE_ANIMATION_ENUMS_H
#define SPRITE_ANIMATION_ENUMS_H

// Shared sprite animation enums for all platforms
// Platform-specific values are handled via #ifdef __NES__
// Note: When building for NES, __NES__ should be defined globally (e.g., via compiler flags)
// This ensures both mainBankZero.c and nes.c use the same enum values

// Player sprite states
// On NES: values directly map to tile indices (1-7)
// On SNES/other: values are OAM tile numbers (0, 2, 4, 6, 8, 10, 12)
#ifdef __NES__
    // NES: Direct tile index mapping
    enum ePlayerSprite {
        PLAYER_SPRITE_IDLE = 1,
        PLAYER_SPRITE_WALK_1 = 2,
        PLAYER_SPRITE_WALK_2 = 3,
        PLAYER_SPRITE_WALK_3 = 4,
        PLAYER_SPRITE_WALL = 5,
        PLAYER_SPRITE_DOWN = 6,
        PLAYER_SPRITE_UP = 7
    };
#else
    // SNES/Other: OAM tile numbers
    enum ePlayerSprite {
        PLAYER_SPRITE_IDLE = 0,
        PLAYER_SPRITE_WALK_1 = 2,
        PLAYER_SPRITE_WALK_2 = 4,
        PLAYER_SPRITE_WALK_3 = 6,
        PLAYER_SPRITE_WALL = 8,
        PLAYER_SPRITE_DOWN = 10,
        PLAYER_SPRITE_UP = 12
    };
#endif

// Spring sprite states
// On NES: values directly map to tile indices (18-19)
// On SNES/other: values are OAM tile numbers (0x2E, 0x40)
#ifdef __NES__
    enum eSpringSprite {
        SPRING_SPRITE_1 = 18,
        SPRING_SPRITE_2 = 19
    };
#else
    enum eSpringSprite {
        SPRING_SPRITE_1 = 0x2E,
        SPRING_SPRITE_2 = 0x40
    };
#endif

// Collapse tile sprite states
// On NES: values directly map to tile indices (23-25)
// On SNES/other: values are OAM tile numbers (0x48, 0x4A, 0x4C)
#ifdef __NES__
    enum eCollapseTileSprite {
        COLLAPSE_TILE_SPRITE_1 = 23,
        COLLAPSE_TILE_SPRITE_2 = 24,
        COLLAPSE_TILE_SPRITE_3 = 25
    };
#else
    enum eCollapseTileSprite {
        COLLAPSE_TILE_SPRITE_1 = 0x48,
        COLLAPSE_TILE_SPRITE_2 = 0x4A,
        COLLAPSE_TILE_SPRITE_3 = 0x4C
    };
#endif

// Strawberry sprite state
// On NES: values directly map to tile indices (26)
// On SNES/other: values are OAM tile numbers (0x4E)
#ifdef __NES__
    enum eStrawberrySprite {
        STRAWBERRY_SPRITE_1 = 26
    };
#else
    enum eStrawberrySprite {
        STRAWBERRY_SPRITE_1 = 0x4E
    };
#endif

// Other sprite enums (these are platform-independent, but kept here for consistency)
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

#endif // SPRITE_ANIMATION_ENUMS_H
