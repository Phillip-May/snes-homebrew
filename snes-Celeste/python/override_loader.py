"""
Override Loader for snes-Celeste Asset Pipeline

Provides file-based override resolution for tiles, levels, and clouds.
Resolution order: overrides/<platform>/ > overrides/shared/ > original auto-generated.

Exported tiles are organized as horizontal spritesheets per group:
  tiles/
    player.png           7 frames: idle, walk_1-3, wall, down, up
    terrain.png          16 frames: standard ground tiles
    terrain_ice.png      16 frames: icy terrain variant
    background.png       10 frames: far background layer
    key.png              3 frames
    platform.png         2 frames
    balloon.png          4 frames: string_1-3 + balloon
    spring.png           2 frames
    chest.png            3 frames: chest + big_chest_l + big_chest_r
    collapse.png         3 frames
    strawberry.png       5 frames: berry + flying berry + wings
    smoke.png            6 frames: smoke_1-3 + smoke_alt_1-3
    flower.png           1 frame
    breakable_wall.png   1 frame
    monument.png         4 frames: tl, tr, bl, br
    orb.png              1 frame
    deco.png             6 frames
    spikes.png           4 frames: up, right, down, left
    other.png            remaining unmapped tiles

Each spritesheet is 8px tall, (N*8)px wide. Frame order matches the lists below.
A {name}.txt label file is written alongside each sheet listing frame names.
"""

import os
import json
from collections import OrderedDict
from PIL import Image

# Base directory for this script
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
OVERRIDES_DIR = os.path.join(SCRIPT_DIR, 'overrides')
EXPORTS_DIR = os.path.join(SCRIPT_DIR, 'exports')

# --- Tile groups ---
# Each group is (folder_name, [(tile_index, frame_name), ...])
# Order within a group defines the left-to-right frame order in the spritesheet.

TILE_GROUPS = OrderedDict([
    ("player", [
        (1, "idle"), (2, "walk_1"), (3, "walk_2"), (4, "walk_3"),
        (5, "wall"), (6, "down"), (7, "up"),
    ]),
    ("key", [
        (8, "key_1"), (9, "key_2"), (10, "key_3"),
    ]),
    ("platform", [
        (11, "platform_1"), (12, "platform_2"),
    ]),
    ("balloon", [
        (13, "string_1"), (14, "string_2"), (15, "string_3"), (22, "balloon"),
    ]),
    ("spikes", [
        (17, "spike_up"), (27, "spike_right"), (43, "spike_down"), (59, "spike_left"),
    ]),
    ("spring", [
        (18, "spring_1"), (19, "spring_2"),
    ]),
    ("chest", [
        (20, "chest"),
    ]),
    ("big_chest", [
        (96, "big_chest_l"), (97, "big_chest_r"),
    ]),
    ("collapse", [
        (23, "collapse_1"), (24, "collapse_2"), (25, "collapse_3"),
    ]),
    ("strawberry", [
        (26, "strawberry"), (28, "flying_berry"),
        (45, "wing_1"), (46, "wing_2"), (47, "wing_3"),
    ]),
    ("smoke", [
        (29, "smoke_1"), (30, "smoke_2"), (31, "smoke_3"),
    ]),
    ("flag", [
        (118, "flag_1"), (119, "flag_2"), (120, "flag_3"),
    ]),
    ("terrain", [
        (32, "ground_00"), (33, "ground_01"), (34, "ground_02"), (35, "ground_03"),
        (36, "ground_04"), (37, "ground_05"), (38, "ground_06"), (39, "ground_07"),
        (48, "ground_08"), (49, "ground_09"), (50, "ground_10"), (51, "ground_11"),
        (52, "ground_12"), (53, "ground_13"), (54, "ground_14"), (55, "ground_15"),
    ]),
    ("background", [
        (16, "bg_00"),
        (40, "bg_01"), (41, "bg_02"), (42, "bg_03"),
        (56, "bg_04"), (57, "bg_05"), (58, "bg_06"),
        (88, "bg_07"), (103, "bg_08"), (104, "bg_09"),
    ]),
    ("terrain_ice", [
        (66, "ice_00"), (67, "ice_01"), (68, "ice_02"), (69, "ice_03"),
        (82, "ice_04"), (83, "ice_05"), (84, "ice_06"), (85, "ice_07"),
        (98, "ice_08"), (99, "ice_09"), (100, "ice_10"), (101, "ice_11"),
        (114, "ice_12"), (115, "ice_13"), (116, "ice_14"), (117, "ice_15"),
    ]),
    ("flower", [
        (60, "flower"),
    ]),
    ("deco", [
        (44, "deco_00"), (61, "deco_01"), (62, "deco_02"), (63, "deco_03"),
        (110, "tree"), (112, "deco_04"), (113, "deco_05"),
    ]),
    ("breakable_wall", [
        (64, "wall"),
    ]),
    ("monument", [
        (70, "monument_tl"), (71, "monument_tr"),
        (86, "monument_bl"), (87, "monument_br"),
    ]),
    ("orb", [
        (102, "double_jump_orb"),
    ]),
])

# NES render type: determines subfolder (bg/ or sprites/) in exports/overrides
# "bg" = background tile (CHR bank, 4 bg palettes)
# "sprites" = OAM sprite (CHR bank, 4 sprite palettes)
GROUP_RENDER_TYPE = {
    "player":         "sprites",
    "key":            "sprites",
    "platform":       "sprites",
    "balloon":        "sprites",
    "spikes":         "bg",
    "spring":         "sprites",
    "chest":          "sprites",
    "big_chest":      "bg",
    "collapse":       "bg",       # rendered as bg tiles on NES
    "strawberry":     "sprites",
    "smoke":          "sprites",
    "flag":           "sprites",
    "terrain":        "bg",
    "background":     "bg",
    "terrain_ice":    "bg",
    "flower":         "bg",
    "deco":           "bg",
    "breakable_wall": "bg",
    "monument":       "bg",
    "orb":            "sprites",
}

# Build reverse lookups from the groups
# tile_index -> (group_name, frame_index_within_group)
_TILE_TO_GROUP = {}
for _group_name, _frames in TILE_GROUPS.items():
    for _frame_idx, (_tile_idx, _frame_name) in enumerate(_frames):
        _TILE_TO_GROUP[_tile_idx] = (_group_name, _frame_idx)

# Object/sprite tiles (4bpp on SNES) - used for bpp validation
OBJECT_GIDS = [
    8, 9, 10, 11, 12, 13, 14, 15,
    18, 19, 20, 21, 22, 23, 24, 25, 26,
    28, 29, 30, 31, 45, 46, 47,
    60, 62, 64, 70, 71, 86, 87,
    96, 97, 102, 118, 119, 120
]

# --- Sprite semantic mapping ---
# Maps source sprite sheet tile index -> (enum_type, enum_name)
# Used to auto-generate sprite_animation_enums_{snes,nes}.h

SPRITE_SEMANTIC_MAP = {
    1:  ("ePlayerSprite", "PLAYER_SPRITE_IDLE"),
    2:  ("ePlayerSprite", "PLAYER_SPRITE_WALK_1"),
    3:  ("ePlayerSprite", "PLAYER_SPRITE_WALK_2"),
    4:  ("ePlayerSprite", "PLAYER_SPRITE_WALK_3"),
    5:  ("ePlayerSprite", "PLAYER_SPRITE_WALL"),
    6:  ("ePlayerSprite", "PLAYER_SPRITE_DOWN"),
    7:  ("ePlayerSprite", "PLAYER_SPRITE_UP"),
    8:  ("eKeySprite", "KEY_SPRITE_1"),
    9:  ("eKeySprite", "KEY_SPRITE_2"),
    10: ("eKeySprite", "KEY_SPRITE_3"),
    11: ("ePlatMovSprite", "PLATMOV_SPRITE_1"),
    12: ("ePlatMovSprite", "PLATMOV_SPRITE_2"),
    13: ("eBalloonSprite", "BALLOON_STRING_1"),
    14: ("eBalloonSprite", "BALLOON_STRING_2"),
    15: ("eBalloonSprite", "BALLOON_STRING_3"),
    18: ("eSpringSprite", "SPRING_SPRITE_1"),
    19: ("eSpringSprite", "SPRING_SPRITE_2"),
    20: ("eChestSprite", "CHEST_SPRITE_1"),
    22: ("eBalloonSprite", "BALLOON_SPRITE_1"),
    23: ("eCollapseTileSprite", "COLLAPSE_TILE_SPRITE_1"),
    24: ("eCollapseTileSprite", "COLLAPSE_TILE_SPRITE_2"),
    25: ("eCollapseTileSprite", "COLLAPSE_TILE_SPRITE_3"),
    26: ("eStrawberrySprite", "STRAWBERRY_SPRITE_1"),
    28: ("eFlyingBerrySprite", "FLYING_BERRY_SPRITE_1"),
    29: ("smokeStates", "SMOKE_SPRITE_1"),
    30: ("smokeStates", "SMOKE_SPRITE_2"),
    31: ("smokeStates", "SMOKE_SPRITE_3"),
    45: ("eFlyingBerrySprite", "WING_SPRITE_1"),
    46: ("eFlyingBerrySprite", "WING_SPRITE_2"),
    47: ("eFlyingBerrySprite", "WING_SPRITE_3"),
    60: ("eFlowerSprite", "FLOWER_SPRITE_1"),
    64: ("eBreakableWallSprite", "BREAKABLE_WALL_SPRITE_1"),
    70: ("eMonumentSprite", "MONUMENT_SPRITE_1"),
    71: ("eMonumentSprite", "MONUMENT_SPRITE_2"),
    86: ("eMonumentSprite", "MONUMENT_SPRITE_3"),
    87: ("eMonumentSprite", "MONUMENT_SPRITE_4"),
    96: ("eBigChestSprite", "BIG_CHEST_SPRITE_1"),
    97: ("eBigChestSprite", "BIG_CHEST_SPRITE_2"),
    102: ("eDoubleJumpOrbSprite", "DOUBLE_JUMP_ORB_SPRITE_1"),
    110: ("eDecoTreeSprite", "DECO_TREE_SPRITE_1"),
}

# NES-specific name overrides (where NES uses different enum names than SNES)
NES_SPRITE_NAME_OVERRIDES = {
    45: ("eFlyingBerrySprite", "FLYING_BERRY_WING_UP"),
    46: ("eFlyingBerrySprite", "FLYING_BERRY_WING_MID"),
    47: ("eFlyingBerrySprite", "FLYING_BERRY_WING_DOWN"),
}

# SNES-specific fixed OAM tile values used by runtime code and data.
# Keep these stable even if sprite packing internals change.
SNES_SPRITE_VALUE_OVERRIDES = {
    "PLAYER_SPRITE_IDLE": 0x00,
    "PLAYER_SPRITE_WALK_1": 0x02,
    "PLAYER_SPRITE_WALK_2": 0x04,
    "PLAYER_SPRITE_WALK_3": 0x06,
    "PLAYER_SPRITE_WALL": 0x08,
    "PLAYER_SPRITE_DOWN": 0x0A,
    "PLAYER_SPRITE_UP": 0x0C,
    "KEY_SPRITE_1": 0x0E,
    "KEY_SPRITE_2": 0x20,
    "KEY_SPRITE_3": 0x22,
    "PLATMOV_SPRITE_1": 0x24,
    "PLATMOV_SPRITE_2": 0x26,
    "BALLOON_STRING_1": 0x28,
    "BALLOON_STRING_2": 0x2A,
    "BALLOON_STRING_3": 0x2C,
    "SPRING_SPRITE_1": 0x2E,
    "SPRING_SPRITE_2": 0x40,
    "CHEST_SPRITE_1": 0x42,
    "BALLOON_SPRITE_1": 0x46,
    "COLLAPSE_TILE_SPRITE_1": 0x48,
    "COLLAPSE_TILE_SPRITE_2": 0x4A,
    "COLLAPSE_TILE_SPRITE_3": 0x4C,
    "STRAWBERRY_SPRITE_1": 0x4E,
    "FLYING_BERRY_SPRITE_1": 0x60,
    "SMOKE_SPRITE_1": 0x62,
    "SMOKE_SPRITE_2": 0x64,
    "SMOKE_SPRITE_3": 0x66,
    "WING_SPRITE_1": 0x68,
    "WING_SPRITE_2": 0x6A,
    "WING_SPRITE_3": 0x6C,
    "DECO_TREE_SPRITE_1": 0x6E,
    "FLOWER_SPRITE_1": 0x80,
    "BREAKABLE_WALL_SPRITE_1": 0x82,
    "MONUMENT_SPRITE_1": 0x84,
    "MONUMENT_SPRITE_2": 0x86,
    "MONUMENT_SPRITE_3": 0x88,
    "MONUMENT_SPRITE_4": 0x8A,
    "BIG_CHEST_SPRITE_1": 0x8C,
    "BIG_CHEST_SPRITE_2": 0x8E,
    "DOUBLE_JUMP_ORB_SPRITE_1": 0xA0,
}


def get_tile_group(tile_index):
    """Returns (group_name, frame_index) for a tile index, or None if unmapped."""
    return _TILE_TO_GROUP.get(tile_index)


# --- Export buffering ---
# Tiles are collected during processing, then flushed as spritesheets at the end.

_export_buffers = {}  # key: (platform, group_name) -> dict of frame_index -> PIL.Image
_export_other = {}    # key: platform -> list of (tile_index, PIL.Image) for unmapped tiles


def export_tile(tile_image, tile_index, platform=None, export_dir=None):
    """
    Buffers a tile for later export as part of a spritesheet.
    Call flush_exports() after all tiles are processed.
    """
    group_info = get_tile_group(tile_index)
    # Scale 8x8 to 16x16 for export (matches actual render size on SNES/NES)
    scaled = tile_image.resize((16, 16), Image.NEAREST)

    if group_info:
        group_name, frame_idx = group_info
        key = (platform, group_name)
        if key not in _export_buffers:
            _export_buffers[key] = {}
        _export_buffers[key][frame_idx] = scaled
    else:
        if platform not in _export_other:
            _export_other[platform] = []
        _export_other[platform].append((tile_index, scaled))


def flush_exports(platform=None, export_dir=None):
    """
    Writes all buffered tiles as horizontal spritesheets.
    Each group becomes {group_name}.png with a {group_name}.txt label file.
    Unmapped tiles go to other.png.
    """
    if export_dir is None:
        export_dir = EXPORTS_DIR
    base_dir = os.path.join(export_dir, platform, 'tiles') if platform else os.path.join(export_dir, 'tiles')

    # Write each named group into its own subfolder
    for (buf_platform, group_name), frames in _export_buffers.items():
        if buf_platform != platform:
            continue

        render_type = GROUP_RENDER_TYPE.get(group_name, "other")
        group_dir = os.path.join(base_dir, render_type, group_name)
        os.makedirs(group_dir, exist_ok=True)

        group_frames = TILE_GROUPS[group_name]
        num_frames = len(group_frames)

        sheet = Image.new('RGB', (num_frames * 16, 16), (0, 0, 0))
        for frame_idx, (tile_idx, frame_name) in enumerate(group_frames):
            if frame_idx in frames:
                frame_img = frames[frame_idx].convert('RGB')
                sheet.paste(frame_img, (frame_idx * 16, 0))

        sheet_path = os.path.join(group_dir, f"{group_name}.png")
        sheet.save(sheet_path)

        # Write label file
        label_path = os.path.join(group_dir, f"{group_name}.txt")
        with open(label_path, 'w') as f:
            for frame_idx, (tile_idx, frame_name) in enumerate(group_frames):
                f.write(f"frame {frame_idx}: {frame_name} (tile {tile_idx})\n")

    # Write "other" group
    if platform in _export_other and _export_other[platform]:
        other_dir = os.path.join(base_dir, 'other')
        os.makedirs(other_dir, exist_ok=True)

        other_tiles = _export_other[platform]
        num = len(other_tiles)
        sheet = Image.new('RGB', (num * 16, 16), (0, 0, 0))
        label_lines = []
        for i, (tile_idx, tile_img) in enumerate(other_tiles):
            sheet.paste(tile_img.convert('RGB'), (i * 16, 0))
            label_lines.append(f"frame {i}: tile_{tile_idx:03d} (tile {tile_idx})")

        sheet_path = os.path.join(other_dir, "other.png")
        sheet.save(sheet_path)

        label_path = os.path.join(other_dir, "other.txt")
        with open(label_path, 'w') as f:
            f.write('\n'.join(label_lines) + '\n')

    # Clear buffers for this platform
    keys_to_remove = [k for k in _export_buffers if k[0] == platform]
    for k in keys_to_remove:
        del _export_buffers[k]
    _export_other.pop(platform, None)

    print(f"Exported tile spritesheets to {base_dir}")


def get_override_path(platform, category, filename):
    """
    Resolves an override file with priority: platform-specific > shared > None.
    """
    platform_path = os.path.join(OVERRIDES_DIR, platform, category, filename)
    if os.path.exists(platform_path):
        return platform_path

    shared_path = os.path.join(OVERRIDES_DIR, 'shared', category, filename)
    if os.path.exists(shared_path):
        return shared_path

    return None


def parse_palette_file(path):
    """
    Parses a .pal palette sidecar file.
    Format: one R,G,B per line. Lines starting with # are comments.
    """
    palette = []
    with open(path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.split(',')
            if len(parts) >= 3:
                r, g, b = int(parts[0].strip()), int(parts[1].strip()), int(parts[2].strip())
                palette.append((r, g, b))
    return palette


def load_tile_override(platform, tile_index):
    """
    Searches override directories for a tile override.

    Returns the 16x16 frame directly (no downscaling). The converter
    splits it into four 8x8 NES tiles for CHR, or downscales if needed
    for palette quantization.

    Returns:
        (PIL.Image 16x16, palette_or_None) if override found, (None, None) otherwise.
    """
    group_info = get_tile_group(tile_index)

    if group_info:
        group_name, frame_idx = group_info
        sheet_name = f"{group_name}.png"
        pal_name = f"{group_name}.pal"

        render_type = GROUP_RENDER_TYPE.get(group_name, "other")
        override_path = get_override_path(platform, f'tiles/{render_type}/{group_name}', sheet_name)
        if override_path is None:
            return None, None

        try:
            sheet = Image.open(override_path)

            if sheet.height != 16:
                print(f"  [OVERRIDE WARNING] {override_path} height is {sheet.height}, expected 16")
                return None, None
            if sheet.width < (frame_idx + 1) * 16:
                print(f"  [OVERRIDE WARNING] {override_path} is too narrow for frame {frame_idx}")
                return None, None

            override_img = sheet.crop((frame_idx * 16, 0, (frame_idx + 1) * 16, 16))

            # bpp validation (warn only - converter will quantize)
            if tile_index not in OBJECT_GIDS:
                rgb_img = override_img.convert('RGB')
                colors = rgb_img.getcolors(maxcolors=256)
                if colors and len(colors) > 4:
                    print(f"  [OVERRIDE WARNING] {override_path} frame {frame_idx} has {len(colors)} colors but tile {tile_index} is 2bpp (max 4). Colors will be quantized.")

            override_pal = None
            pal_path = get_override_path(platform, f'tiles/{render_type}/{group_name}', pal_name)
            if pal_path:
                override_pal = parse_palette_file(pal_path)

            return override_img, override_pal

        except Exception as e:
            print(f"  [OVERRIDE ERROR] Failed to load {override_path}: {e}")
            return None, None
    else:
        filename = f"tile_{tile_index:03d}.png"
        pal_filename = f"tile_{tile_index:03d}.pal"

        override_path = get_override_path(platform, 'tiles/other', filename)
        if override_path is None:
            return None, None

        try:
            override_img = Image.open(override_path)
            # Accept 8x8 or 16x16; upscale 8x8 to 16x16
            if override_img.size == (8, 8):
                override_img = override_img.resize((16, 16), Image.NEAREST)
            elif override_img.size != (16, 16):
                print(f"  [OVERRIDE WARNING] {override_path} is {override_img.size}, expected (16, 16) or (8, 8)")
                return None, None

            override_pal = None
            pal_path = get_override_path(platform, 'tiles/other', pal_filename)
            if pal_path:
                override_pal = parse_palette_file(pal_path)

            return override_img, override_pal

        except Exception as e:
            print(f"  [OVERRIDE ERROR] Failed to load {override_path}: {e}")
            return None, None


def load_level_override(platform, level_num, asset_type):
    """Checks for a level override file."""
    return get_override_path(platform, f"levels/level{level_num}", asset_type)


def load_clouds_override(platform):
    """Checks for a cloud image override."""
    return get_override_path(platform, 'clouds', 'clouds.png')


def export_level_tilemap(level_data, level_num, platform=None, export_dir=None):
    """Exports level data as a standalone Tiled-format JSON file."""
    if export_dir is None:
        export_dir = EXPORTS_DIR
    if platform:
        export_dir = os.path.join(export_dir, platform)

    level_dir = os.path.join(export_dir, 'levels', f'level{level_num}')
    os.makedirs(level_dir, exist_ok=True)

    flat_data = []
    for row in level_data:
        flat_data.extend(row)

    tiled_json = {
        "compressionlevel": -1,
        "height": 16,
        "infinite": False,
        "layers": [{
            "data": flat_data,
            "height": 16,
            "id": 1,
            "name": f"level{level_num}",
            "opacity": 1,
            "type": "tilelayer",
            "visible": True,
            "width": 16,
            "x": 0,
            "y": 0
        }],
        "nextlayerid": 2,
        "nextobjectid": 1,
        "orientation": "orthogonal",
        "renderorder": "right-down",
        "tileheight": 8,
        "tilewidth": 8,
        "type": "map",
        "version": "1.10",
        "width": 16
    }

    filepath = os.path.join(level_dir, 'tilemap.json')
    with open(filepath, 'w') as f:
        json.dump(tiled_json, f, indent=2)


def write_generated_tables(output_index_translation_table, all_2bpp_palettes, filepath=None):
    """Writes outputIndexTranslationTable and all_2bpp_palettes to a shared JSON file."""
    if filepath is None:
        filepath = os.path.join(SCRIPT_DIR, 'generated_tables.json')

    data = {
        'outputIndexTranslationTable': output_index_translation_table,
        'all_2bpp_palettes': [
            [list(color) for color in palette]
            for palette in all_2bpp_palettes
        ]
    }

    with open(filepath, 'w') as f:
        json.dump(data, f, indent=2)

    print(f"Wrote generated tables to {filepath}")


def load_generated_tables(filepath=None):
    """Loads outputIndexTranslationTable and all_2bpp_palettes from the shared JSON file."""
    if filepath is None:
        filepath = os.path.join(SCRIPT_DIR, 'generated_tables.json')

    if not os.path.exists(filepath):
        return None, None

    with open(filepath, 'r') as f:
        data = json.load(f)

    table = data['outputIndexTranslationTable']
    palettes = [
        [tuple(color) for color in palette]
        for palette in data['all_2bpp_palettes']
    ]

    return table, palettes


def generate_sprite_enums_snes(sprite_info, output_path):
    """Auto-generates sprite_animation_enums_snes.h from sprite_info and SPRITE_SEMANTIC_MAP."""
    def _get_gfx_index(info):
        # Accept both dict-based entries from converters and struct-like inputs.
        if isinstance(info, dict):
            if 'gfx_idx' in info:
                return info['gfx_idx']
            if 'gfx_index' in info:
                return info['gfx_index']
            return None
        return getattr(info, 'gfx_idx', getattr(info, 'gfx_index', None))

    enums = {}
    for tile_index, (enum_type, enum_name) in sorted(SPRITE_SEMANTIC_MAP.items()):
        if tile_index < len(sprite_info):
            info = sprite_info[tile_index]
            gfx_idx = _get_gfx_index(info)
            if gfx_idx is None:
                raise KeyError(f"Missing gfx index for tile {tile_index} while generating SNES enums")
            oam_tile = gfx_idx * 2
            if enum_name in SNES_SPRITE_VALUE_OVERRIDES:
                oam_tile = SNES_SPRITE_VALUE_OVERRIDES[enum_name]
            if oam_tile > 0xFF:
                raise ValueError(f"OAM tile out of range for {enum_name}: 0x{oam_tile:X}")
            if enum_type not in enums:
                enums[enum_type] = []
            enums[enum_type].append((enum_name, oam_tile))

    # Guardrail: if almost all non-player sprites collapse to tile 0, fail fast.
    non_player_members = []
    for enum_type, members in enums.items():
        if enum_type != "ePlayerSprite":
            non_player_members.extend(members)
    if non_player_members:
        zero_count = sum(1 for _, value in non_player_members if value == 0)
        if zero_count >= int(len(non_player_members) * 0.7):
            raise ValueError(
                f"Refusing to write SNES enums: {zero_count}/{len(non_player_members)} non-player entries resolve to 0x00"
            )

    _write_enum_header(enums, output_path, 'SNES', 'OAM tile numbers')


def generate_sprite_enums_nes(tile_indices_map, output_path):
    """Auto-generates sprite_animation_enums_nes.h from tile index mapping."""
    enums = {}
    for tile_index, (enum_type, enum_name) in sorted(SPRITE_SEMANTIC_MAP.items()):
        if tile_index in NES_SPRITE_NAME_OVERRIDES:
            enum_type, enum_name = NES_SPRITE_NAME_OVERRIDES[tile_index]
        value = tile_indices_map.get(tile_index, tile_index)
        if enum_type not in enums:
            enums[enum_type] = []
        enums[enum_type].append((enum_name, value))

    _write_enum_header(enums, output_path, 'NES', 'tile indices')


def _write_enum_header(enums, output_path, platform, value_desc):
    """Helper to write a sprite animation enums header file."""
    guard = f"SPRITE_ANIMATION_ENUMS_{platform}_H"

    with open(output_path, 'w') as f:
        f.write(f"#ifndef {guard}\n")
        f.write(f"#define {guard}\n\n")
        f.write(f"// {platform}-specific sprite animation enums (AUTO-GENERATED)\n")
        f.write(f"// On {platform}, values are {value_desc}\n\n")

        for enum_type, members in enums.items():
            f.write(f"enum {enum_type} {{\n")
            for i, (name, value) in enumerate(members):
                comma = "," if i < len(members) - 1 else ""
                f.write(f"    {name} = 0x{value:02X}{comma}\n")
            f.write(f"}};\n\n")

        f.write(f"#endif // {guard}\n")

    print(f"Generated {output_path}")
