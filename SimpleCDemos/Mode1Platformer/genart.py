#!/usr/bin/env python3
"""Mode1Platformer level + sprite art from CC0 inputs.

Three roles:

  * Build-time level builder (imported by convert.py). Reads `level.tmx` -- a CC0
    16x16-metatile Tiled map over `level_tiles.png` (each tile carries a re-skin
    `category` property) -- via `categories()`, and turns it into the demo's level
    assets, painted with Kenney "Pixel Platformer" (CC0) tiles:
      - build_bg()        -> bg.png:        the full SMB 1-1 layout, re-skinned.
      - build_collision() -> bg_col.png:    a solid mask from the metatile
                             categories (solid = ground / brick / ? block / pipe;
                             the rounded hills are passable background).
      - emit_blockcells() -> blockcells.inc: the ? / brick cell lists.
      - emit_spawns()     -> spawns.inc:     player start + enemy spawn table, from
                             the TMX object layer ("Entities").
    The re-skin needs only the Kenney *tile* sheet, committed alongside this demo
    as `kenney_tilemap_packed.png`.

  * One-time bootstrap (`python genart.py bootstrap`, not part of the build).
    Migrates the original pvsneslib Mario 8x8-gid map (MARIO_SRC) into the CC0
    `level.tmx` + `level_tiles.png` committed here, so the TMX uses the same CC0
    tileset the ROM renders and opens cleanly in Tiled.

  * One-time sprite authoring (`python genart.py`, not part of the build). Slices
    the player/walker frames and the interactive-block overlays from the full
    Kenney pack into committed PNGs. Needs the Kenney character sheet, so point
    KENNEY_PP at the full pack; the build does not run this.

Art: Kenney "Pixel Platformer" 1.2, CC0 (www.kenney.nl). Layout: pvsneslib
`objects/mapandobjects` (zlib), migrated into the CC0-metatile level.tmx.
"""
import os
import sys

from PIL import Image

# Allow running standalone (bootstrap / sprite authoring): put the repo root that
# holds pySnesDevTools on the path. convert.py also does this when it imports us.
sys.path.insert(0, os.path.abspath(os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", "..")))
import pySnesDevTools as snes

HERE = os.path.dirname(os.path.abspath(__file__))
# Committed Kenney tile sheet (CC0) -- the only art input the build-time level
# re-skin needs.
KENNEY_TILES = os.environ.get(
    "KENNEY_TILES", os.path.join(HERE, "kenney_tilemap_packed.png"))
# Full Kenney pack -- only the one-time sprite authoring (main) reads the
# character sheet from here.
KENNEY = os.environ.get(
    "KENNEY_PP",
    r"C:\Users\Admin\AppData\Local\Temp\claude\C--Users-Admin-Documents-snes-homebrew"
    r"\dec2247f-35fc-4153-ba43-cba3bfacac15\scratchpad\kenney_pp")
# The CC0-metatile Tiled map the build reads (committed; built by the one-time
# `python genart.py bootstrap`, which migrates the original Mario layout below).
TMX = os.environ.get("LEVEL_TMX", os.path.join(HERE, "level.tmx"))
# Original pvsneslib Mario 8x8-gid map -- input to the one-time bootstrap only;
# the normal build never touches it.
MARIO_SRC = os.environ.get(
    "MARIO_TMX",
    r"C:\pvsneslib\snes-examples\objects\mapandobjects\tiledMario.tmx")
SNESFONT = os.path.join(HERE, "..", "shared", "assets", "snesfont.png")

SKY = (107, 140, 255)            # SMB sky blue, becomes a palette colour

# --- Kenney tile sheets (loaded lazily so importing this module for the level
# build needs only the committed tile sheet, not the full pack) -----------------
_TILES = None
_CHARS = None
_FONT = None


def _tiles():
    global _TILES
    if _TILES is None:
        _TILES = Image.open(KENNEY_TILES).convert("RGBA")
    return _TILES


def _chars():
    global _CHARS
    if _CHARS is None:
        _CHARS = Image.open(
            os.path.join(KENNEY, "Tilemap", "tilemap-characters_packed.png")
        ).convert("RGBA")
    return _CHARS


def _font():
    global _FONT
    if _FONT is None:
        _FONT = Image.open(SNESFONT).convert("RGBA")
    return _FONT


def ktile(idx):
    """A Kenney 18x18 tile, resized to a crisp 16x16 over the sky colour."""
    t = _tiles()
    tcols = t.width // 18
    tx, ty = idx % tcols, idx // tcols
    cell = t.crop((tx * 18, ty * 18, tx * 18 + 18, ty * 18 + 18))
    flat = Image.new("RGBA", (18, 18), SKY + (255,))
    flat.alpha_composite(cell)
    return flat.convert("RGB").resize((16, 16), Image.LANCZOS)


def kchar(idx):
    """A Kenney 24x24 character, resized to 16x16 keeping its alpha."""
    c = _chars()
    ccols = c.width // 24
    cx, cy = idx % ccols, idx // ccols
    cell = c.crop((cx * 24, cy * 24, cx * 24 + 24, cy * 24 + 24))
    return cell.resize((16, 16), Image.LANCZOS)


def ktile_rgba(idx):
    """A Kenney 18x18 tile resized to 16x16 keeping its alpha -- for OBJ sprites
    (the block/coin overlays), which need transparency, unlike the BG ktile()."""
    t = _tiles()
    tcols = t.width // 18
    tx, ty = idx % tcols, idx // tcols
    cell = t.crop((tx * 18, ty * 18, tx * 18 + 18, ty * 18 + 18))
    return cell.resize((16, 16), Image.LANCZOS)


def glyph(ch):
    """An 8x8 RGBA glyph for a character: the font's white pixels become opaque
    white, everything else transparent."""
    g = _font().crop((0, ord(ch) * 8, 8, ord(ch) * 8 + 8)).convert("L")
    out = Image.new("RGBA", (8, 8), (0, 0, 0, 0))
    px = g.load()
    for y in range(8):
        for x in range(8):
            if px[x, y] > 127:
                out.putpixel((x, y), (255, 255, 255, 255))
    return out


def digits_token(s):
    """A 16x16 RGBA score-popup ("100"/"200"): the digits packed small and
    centred, white on transparent."""
    out = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
    w = 5                                    # squeezed glyph width so 3 fit in 16
    x0 = (16 - w * len(s)) // 2
    for i, ch in enumerate(s):
        out.alpha_composite(glyph(ch).resize((w, 8), Image.LANCZOS), (x0 + i * w, 4))
    return out


# Kenney tile index used for each re-skin category (sheet is 20 wide). The
# cloud is a single row of left/middle/right pieces (153/154/155); SMB clouds
# and bushes are 2 metatile-rows tall, so their top row takes these tiles and
# their bottom row becomes sky (see META) -- a clean 3-wide cloud, not a slab.
KTILE = {
    "CLOUD_L": 153, "CLOUD_M": 154, "CLOUD_R": 155, "BUSH": 125, "POLE": 52,
    "QBLOCK": 10, "QBLOCK_USED": 11, "COIN": 151,
    "BRICK": 26, "PIPE_TOP": 17, "PIPE_BODY": 37, "GREEN_TL": 16,
    "GREEN_TR": 19, "GREEN_FILL": 57, "DARK": 6, "GROUND_TOP": 0,
    "GROUND_FILL": 24,
    # Decorative connecting pieces (see the refinement pass below). Bushes and
    # hills share Kenney's rounded green-platform block (rounded corners 16/19,
    # flat top 17, side edges 36/39, fill 37); a standalone bush is the sprout
    # 125. The flagpole is a wooden trunk: top 32 / body 52 / base 72.
    "G_TL": 16, "G_T": 17, "G_TR": 19, "G_L": 36, "G_C": 37, "G_R": 39,
    "G_S": 125, "POLE_TOP": 32, "POLE_BODY": 52, "POLE_BASE": 72,
}

# Categories that are solid. Collision follows what the player reads as a solid
# block -- ground, brick, ? block, pipe -- and never the decorative background.
# The GREEN_* tiles are the rounded hills of SMB 1-1: pure background you pass in
# front of (the reference's collision table marks them empty), so they are NOT
# solid. ? blocks are forced solid (the reference leaves them empty, but a player
# expects to stand on / bump them). Every solid category stamps a full 16x16
# Kenney tile, so a solid metatile is solid across its whole cell.
SOLID_CATS = {
    "GROUND", "GROUND_TOP", "GROUND_FILL", "BRICK", "QBLOCK",
    "PIPE_TOP", "PIPE_BODY",
}

# [Bootstrap only] The 36 distinct 16x16 metatiles of the ORIGINAL Mario layout
# (each a 2x2 8x8-gid tuple), mapped to a re-skin category. The live build no
# longer uses this -- level.tmx now stores the category per metatile directly;
# this table is only how the one-time bootstrap migrates the Mario map. "GROUND"
# resolves to a grass top or dirt fill by what sits above it.
META = [
    # A cloud is a 2x3 metatile block; its top row carries the Kenney cloud
    # left/middle/right pieces and its bottom row is sky (a single-row cloud).
    ((1, 1, 1, 1), "SKY"),       ((1, 1, 1, 4), "CLOUD_L"),
    ((2, 3, 5, 5), "CLOUD_M"),   ((1, 1, 6, 1), "CLOUD_R"),
    ((1, 1, 7, 8), "BUSH"),      ((1, 9, 1, 1), "SKY"),
    ((10, 11, 1, 1), "SKY"),     ((12, 1, 1, 1), "SKY"),
    ((1, 13, 1, 16), "POLE"),    ((14, 15, 17, 15), "POLE"),
    ((18, 15, 19, 15), "POLE"),  ((20, 21, 25, 26), "QBLOCK"),
    ((22, 22, 27, 27), "BRICK"), ((23, 24, 28, 29), "GROUND"),
    ((19, 15, 19, 15), "POLE"),  ((30, 31, 27, 27), "BRICK"),
    ((32, 33, 37, 38), "PIPE_TOP"), ((34, 35, 39, 40), "PIPE_TOP"),
    ((27, 36, 27, 36), "BRICK"), ((27, 27, 27, 27), "BRICK"),
    ((36, 27, 36, 27), "BRICK"), ((1, 1, 47, 48), "GREEN_FILL"),
    ((41, 42, 41, 42), "PIPE_BODY"), ((43, 44, 43, 44), "PIPE_BODY"),
    ((45, 46, 27, 27), "BRICK"), ((1, 49, 49, 43), "GREEN_TL"),
    ((43, 50, 43, 43), "GREEN_TR"), ((51, 1, 43, 51), "GREEN_FILL"),
    ((52, 53, 36, 36), "BRICK"), ((43, 43, 43, 43), "GREEN_FILL"),
    ((50, 43, 43, 43), "GREEN_FILL"), ((1, 1, 1, 56), "GREEN_FILL"),
    ((54, 55, 57, 57), "BUSH"),  ((1, 1, 58, 1), "BUSH"),
    ((36, 36, 36, 36), "DARK"),  ((59, 60, 61, 62), "GROUND"),
]
METACAT = dict(META)


def load_map():
    """The vendored Tiled map (layout + tile properties + object spawns)."""
    return snes.load_tmx(TMX)


def categories(m):
    """The re-skin category of every 16x16 metatile cell, as a 2D list, read from
    the CC0-metatile map: each tile carries a `category` property (gid 0 = SKY)."""
    cat_by_gid = {gid: p.get("category", "SKY")
                  for gid, p in m.tile_props.items()}
    W, H, data = m.width, m.height, m.tilelayer
    return [[cat_by_gid.get(data[my * W + mx], "SKY") for mx in range(W)]
            for my in range(H)]


# Bush/hill green. Kenney's green-platform blocks (16-19/36-39) each carry a full
# dark border, so they never tile into a continuous hedge. The cloud set (153-155)
# IS seamless, and an SMB bush is just a green cloud -- so bushes/hills are painted
# as green-tinted, puffs-up cloud tops over a solid green fill, which connect.
# The exact greens of Kenney's green-platform tiles (already in the level palette
# via the pipes), so tinted bushes/hills share those colours -- consistent green,
# no extra palette pressure. Two-tone: a dark outline over a single body green that
# is identical to the solid fill, so a cloud top and the fill below it match exactly.
_G_DARK = (52, 85, 81)     # outline
_G_MID = (46, 176, 130)    # body / solid fill


def _tint_green(rgba):
    """Recolour a 16x16 RGBA tile to the platform greens, over sky: the dark cloud
    outline becomes _G_DARK, everything else _G_MID (= the solid fill green)."""
    out = Image.new("RGB", (16, 16), SKY)
    src, dst = rgba.load(), out.load()
    for y in range(16):
        for x in range(16):
            r, g, b, a = src[x, y]
            if a < 128:
                continue
            dst[x, y] = _G_DARK if (r + g + b) // 3 < 100 else _G_MID
    return out


def _green_top(idx):
    """A seamless rounded green top from a cloud tile (flipped puffs-up, tinted).
    The cloud's flat edge (now the bottom) carries a dark outline; clear it to fill
    green so a top tile blends into the solid body below it (and into the ground
    under a 1-tall hedge) with no horizontal seam."""
    t = _tint_green(ktile_rgba(idx).transpose(Image.FLIP_TOP_BOTTOM))
    px = t.load()
    for y in (14, 15):
        for x in range(16):
            if px[x, y] == _G_DARK:
                px[x, y] = _G_MID
    return t


def _green_single():
    """A lone bush: rounded on both ends (left cap of 153 + right cap of 155)."""
    out = Image.new("RGB", (16, 16), SKY)
    out.paste(_green_top(153).crop((0, 0, 8, 16)), (0, 0))
    out.paste(_green_top(155).crop((8, 0, 16, 16)), (8, 0))
    return out


def cat_image(cat):
    """The 16x16 RGB tile a category paints to. Bushes/hills use the green cloud
    tops (G_TL/G_T/G_TR), a solid green fill body (G_L/G_C/G_R) and a both-capped
    single (G_S); clouds flip puffs-up; everything else is a straight Kenney tile."""
    if cat == "G_TL":
        return _green_top(153)
    if cat == "G_T":
        return _green_top(154)
    if cat == "G_TR":
        return _green_top(155)
    if cat == "G_S":
        return _green_single()
    if cat in ("G_L", "G_C", "G_R"):
        return Image.new("RGB", (16, 16), _G_MID)
    tile = ktile(KTILE[cat])
    if cat.startswith("CLOUD"):
        # Kenney's cloud tiles are a ceiling piece (puffs hang down); flip up.
        tile = tile.transpose(Image.FLIP_TOP_BOTTOM)
    return tile


def build_bg(cat, out):
    """Composite the re-skinned wide level from a category grid, index it to <=16
    colours, save to out. (GROUND_TOP/GROUND_FILL are explicit in the map; the
    legacy generic-GROUND resolution is kept for robustness but normally unused.)"""
    mh, mw = len(cat), len(cat[0])           # metatile grid (16x16 cells)
    img = Image.new("RGB", (mw * 16, mh * 16), SKY)
    for my in range(mh):
        for mx in range(mw):
            c = cat[my][mx]
            if c == "SKY":
                continue
            if c == "GROUND":
                above = cat[my - 1][mx] if my > 0 else "SKY"
                c = "GROUND_FILL" if above == "GROUND" else "GROUND_TOP"
            img.paste(cat_image(c), (mx * 16, my * 16))
    pal = img.quantize(colors=16, method=Image.MEDIANCUT)
    # The clouds are the only near-white art, but with just 16 shared colours
    # MEDIANCUT lands their bin on a washed-out pale green. That lightest entry
    # is cloud-only (every foliage colour is distinctly darker/greener), so nudge
    # it to a clean soft white -- the clouds read white without touching foliage.
    raw = pal.getpalette()
    lightest = max(range(16), key=lambda i: sum(raw[i * 3:i * 3 + 3]))
    raw[lightest * 3:lightest * 3 + 3] = [236, 240, 236]
    pal.putpalette(raw)
    pal.save(out)
    return pal.getcolors()


def build_collision(cat, out):
    """White where the metatile's visual category is solid, black elsewhere -- so
    collision always matches what is drawn (the whole 16x16 cell of a solid
    category is solid)."""
    mh, mw = len(cat), len(cat[0])
    img = Image.new("RGB", (mw * 16, mh * 16), (0, 0, 0))
    block = Image.new("RGB", (16, 16), (255, 255, 255))
    for my, row in enumerate(cat):
        for mx, c in enumerate(row):
            if c in SOLID_CATS:
                img.paste(block, (mx * 16, my * 16))
    img.save(out)


def jump_pose(frame):
    """Synthesize an airborne pose from the stand frame: lift the body 2 px so
    the legs tuck up off the ground (the Kenney green character has no jump
    frame of its own), giving a third pose distinct from stand and walk."""
    out = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
    out.alpha_composite(frame.crop((0, 2, 16, 16)), (0, 0))
    return out


def sprite_strip(frames, path):
    """Pack frames (RGBA 16x16) into a horizontal strip, indexed with index 0
    transparent (SNES OBJ colour 0). Opaque colours quantise to indices 1..15."""
    n = len(frames)
    strip = Image.new("RGBA", (16 * n, 16), (0, 0, 0, 0))
    for i, f in enumerate(frames):
        strip.alpha_composite(f, (i * 16, 0))
    q = strip.convert("RGB").quantize(colors=15, method=Image.MEDIANCUT)
    qpal = q.getpalette()[:15 * 3]
    alpha = list(strip.getdata())
    idx = [0 if a[3] < 128 else qi + 1 for qi, a in zip(q.getdata(), alpha)]
    out = Image.new("P", strip.size)
    out.putpalette([0, 0, 0] + qpal)         # index 0 = transparent backdrop
    out.putdata(idx)
    out.save(os.path.join(HERE, path))


def build_blocks():
    """The OBJ overlay strip for interactive blocks, in engine frame order:
    0 used-block, 1 brick, 2 coin, 3 "100", 4 "200". The block reactions are
    drawn as sprites over the (const, ROM, streamed) BG tilemap; see mapengine.c."""
    sprite_strip([ktile_rgba(KTILE["QBLOCK_USED"]), ktile_rgba(KTILE["BRICK"]),
                  ktile_rgba(KTILE["COIN"]), digits_token("100"),
                  digits_token("200")], "blocks.png")


def emit_blockcells(cat, out):
    """Emit blockcells.inc: the ? and brick block cells (the 8px-cell top-left of
    each 16x16 metatile), so the engine can recognise which bumped cells are
    interactive without any runtime tile-type lookup (the collision table is just
    solid/empty). Authored here because only the re-skin knows the metatile category."""
    q, b = [], []
    for my, row in enumerate(cat):
        for mx, c in enumerate(row):
            if c == "QBLOCK":
                q.append((mx * 2, my * 2))
            elif c == "BRICK":
                b.append((mx * 2, my * 2))

    def emit(f, name, cells):
        f.write("#define %s_N %d\n" % (name.upper(), len(cells)))
        f.write("static const uint16_t %s_cells[%s_N][2] = {\n" % (name, name.upper()))
        for col, rw in cells:
            f.write("\t{%d, %d},\n" % (col, rw))
        f.write("};\n\n")

    with open(out, "w") as f:
        f.write("// Generated by genart.py from level.tmx -- do not edit.\n")
        f.write("// ? and brick block cells (8px-cell top-left of each 16x16\n")
        f.write("// metatile) for the interactive-block engine in mapengine.c.\n\n")
        emit(f, "qblock", q)
        emit(f, "brick", b)
    return len(q), len(b)


# Map the TMX object "class" to whether it is an enemy (a pacing creature) or the
# player start. The original pvsneslib layer used class 0 = player, 1/2 = enemies;
# all enemies share this demo's one walker behaviour.
_ENEMY_CLASSES = {"1", "2"}      # enemy classes in the source map
_PLAYER_CLASS = "0"              # player class

# CC0-neutral object names for the bootstrap to write, keyed by class. The demo's
# sprites are Kenney art (a green character + an orange block creature), not the
# pvsneslib Mario/Goomba/koopatroopa, so the objects are named for what they are.
_OBJ_NAME = {"0": "Player", "1": "Walker", "2": "Walker"}


def emit_spawns(objects, out):
    """Emit spawns.inc from the TMX object layer: the player start (the Player
    object) and the enemy spawn table (the Walker enemies). Tiled places an
    object by its top-left and y just above the local ground; the engine's 16px
    entity origin is the top-left too, so py = object.y - 16 lets gravity settle
    each onto the ground (matching the reference 1-1 placement)."""
    player = None
    enemies = []
    for o in objects:
        if o.kind == _PLAYER_CLASS:
            player = o
        elif o.kind in _ENEMY_CLASSES:
            enemies.append(o)
    if player is None:
        raise ValueError("level.tmx has no player (Mario) object")

    def origin(o):
        return int(round(o.x)), int(round(o.y)) - 16

    psx, psy = origin(player)
    with open(out, "w") as f:
        f.write("// Generated by genart.py from level.tmx -- do not edit.\n")
        f.write("// Player start + enemy spawns from the TMX 'Entities' layer.\n\n")
        f.write("#define NENEMY %d\n" % len(enemies))
        f.write("#define PLAYER_START_X %d\n" % psx)
        f.write("#define PLAYER_START_Y %d\n\n" % psy)
        f.write("static const short enemy_spawns[NENEMY][2] = {\n")
        for o in enemies:
            ex, ey = origin(o)
            f.write("\t{%d, %d},\n" % (ex, ey))
        f.write("};\n")
    return len(enemies)


# --- One-time bootstrap: build the CC0 metatile tileset + migrate level.tmx ----
# Categories that get a painted 16x16 tile (SKY is empty / gid 0). The order
# fixes the tileset-image columns and the gid each category maps to. Decorations
# use position-aware connecting pieces (see _refine_decorations): G_* are the
# green-platform bush/hill pieces, POLE_* the flagpole trunk.
TILESET_CATS = ["GROUND_TOP", "GROUND_FILL", "BRICK", "QBLOCK", "PIPE_TOP",
                "PIPE_BODY", "G_TL", "G_T", "G_TR", "G_L", "G_C", "G_R", "G_S",
                "CLOUD_L", "CLOUD_M", "CLOUD_R", "DARK",
                "POLE_TOP", "POLE_BODY", "POLE_BASE"]
CAT_TO_GID = {c: i + 1 for i, c in enumerate(TILESET_CATS)}

# Coarse categories (from the Mario metatiles) that _refine_decorations rewrites
# into connecting pieces. Bushes and hills are one green family: a 1-tall run is a
# hedge, a taller cluster a mound. GROUND_* marks where a lone bush may sit.
_GREEN_COARSE = {"BUSH", "GREEN_TL", "GREEN_TR", "GREEN_FILL"}
_GROUND_CATS = {"GROUND_TOP", "GROUND_FILL"}


def _refine_decorations(cat):
    """Rewrite decorative cells to position-aware connecting pieces so multi-tile
    bushes/hills/poles tile seamlessly instead of repeating one stamp.

    Green (bush/hill) cells autotile over the rounded green-platform set by their
    4 green neighbours: a cell with nothing above is a top edge (rounded ends
    G_TL/G_TR, flat G_T), otherwise body (side edges G_L/G_R, fill G_C); a lone
    green cell becomes a standalone bush G_S if it sits on ground, else it is a
    floating-conversion artifact and is dropped to SKY. Pole cells become
    POLE_TOP / POLE_BODY / POLE_BASE by their vertical run."""
    H, W = len(cat), len(cat[0])

    def green(x, y):
        return 0 <= x < W and 0 <= y < H and cat[y][x] in _GREEN_COARSE

    def pole(x, y):
        return 0 <= x < W and 0 <= y < H and cat[y][x] == "POLE"

    out = [row[:] for row in cat]
    for y in range(H):
        for x in range(W):
            c = cat[y][x]
            if c in _GREEN_COARSE:
                u, d = green(x, y - 1), green(x, y + 1)
                l, r = green(x - 1, y), green(x + 1, y)
                if not (u or d or l or r):
                    below = cat[y + 1][x] if y + 1 < H else "SKY"
                    out[y][x] = "G_S" if below in _GROUND_CATS else "SKY"
                elif not u:
                    out[y][x] = "G_T" if l == r else ("G_TL" if r else "G_TR")
                else:
                    out[y][x] = "G_C" if l == r else ("G_L" if r else "G_R")
            elif c == "POLE":
                u, d = pole(x, y - 1), pole(x, y + 1)
                if not (u or d):
                    below = cat[y + 1][x] if y + 1 < H else "SKY"
                    out[y][x] = "POLE_TOP" if below in _GROUND_CATS else "SKY"
                else:
                    out[y][x] = ("POLE_TOP" if not u
                                 else "POLE_BASE" if not d else "POLE_BODY")
    return out


def _cat_tile(cat):
    """The 16x16 RGB tile for a category, exactly as build_bg paints it."""
    return cat_image(cat)


def build_metatile_tileset(out):
    """level_tiles.png: one 16x16 CC0 tile per category, in TILESET_CATS order
    (the tileset level.tmx references, so Tiled shows the art the ROM renders)."""
    n = len(TILESET_CATS)
    img = Image.new("RGB", (n * 16, 16), SKY)
    for i, c in enumerate(TILESET_CATS):
        img.paste(_cat_tile(c), (i * 16, 0))
    img.save(out)
    return n


def _mario_metacats(W, H, data):
    """[Bootstrap only] Category of each 16x16 metatile from the original Mario
    8x8-gid layout (2x2 tuple -> METACAT); GROUND stays unresolved here."""
    mw, mh = W // 2, H // 2
    return [[METACAT.get((data[(my * 2) * W + mx * 2],
                          data[(my * 2) * W + mx * 2 + 1],
                          data[(my * 2 + 1) * W + mx * 2],
                          data[(my * 2 + 1) * W + mx * 2 + 1]), "SKY")
             for mx in range(mw)] for my in range(mh)]


def bootstrap_tmx(mario_src, tiles_out, tmx_out):
    """[One-time migration] Convert the original Mario 8x8-gid Tiled map into a CC0
    16x16-metatile level.tmx over level_tiles.png. GROUND_TOP/GROUND_FILL are
    resolved exactly as build_bg does, so the built ROM is unchanged; the object
    layer (Entities) is copied verbatim (object x/y are pixels -> unaffected)."""
    import copy
    import xml.etree.ElementTree as ET

    build_metatile_tileset(tiles_out)
    src = ET.parse(mario_src).getroot()
    W, H = int(src.get("width")), int(src.get("height"))
    data = [int(v) for v in src.find("layer").find("data").text
            .replace("\n", "").split(",") if v.strip()]
    raw = _mario_metacats(W, H, data)
    mw, mh = W // 2, H // 2
    # Resolve GROUND_TOP/GROUND_FILL (as build_bg does), then refine decorations
    # into connecting pieces; finally map each category to its tileset gid.
    grid = [[raw[my][mx] for mx in range(mw)] for my in range(mh)]
    for my in range(mh):
        for mx in range(mw):
            if grid[my][mx] == "GROUND":
                above = raw[my - 1][mx] if my > 0 else "SKY"
                grid[my][mx] = "GROUND_FILL" if above == "GROUND" else "GROUND_TOP"
    grid = _refine_decorations(grid)
    gids = [0 if grid[my][mx] == "SKY" else CAT_TO_GID[grid[my][mx]]
            for my in range(mh) for mx in range(mw)]

    n = len(TILESET_CATS)
    root = ET.Element("map", {
        "version": "1.9", "tiledversion": "1.9.2", "orientation": "orthogonal",
        "renderorder": "right-down", "width": str(mw), "height": str(mh),
        "tilewidth": "16", "tileheight": "16", "infinite": "0",
        "nextlayerid": "3", "nextobjectid": src.get("nextobjectid", "6")})
    ts = ET.SubElement(root, "tileset", {
        "firstgid": "1", "name": "cc0tiles", "tilewidth": "16",
        "tileheight": "16", "tilecount": str(n), "columns": str(n)})
    ET.SubElement(ts, "image", {"source": os.path.basename(tiles_out),
                                "width": str(n * 16), "height": "16"})
    for i, c in enumerate(TILESET_CATS):
        tile = ET.SubElement(ts, "tile", {"id": str(i)})
        props = ET.SubElement(tile, "properties")
        ET.SubElement(props, "property", {"name": "category", "value": c})
    layer = ET.SubElement(root, "layer", {
        "id": "1", "name": "Background", "width": str(mw), "height": str(mh)})
    data_el = ET.SubElement(layer, "data", {"encoding": "csv"})
    rows = [",".join(str(gids[my * mw + mx]) for mx in range(mw))
            for my in range(mh)]
    data_el.text = "\n" + ",\n".join(rows) + "\n"
    objs = src.find("objectgroup")
    if objs is not None:
        objs = copy.deepcopy(objs)
        for o in objs.findall("object"):       # rename to the CC0 entity names
            name = _OBJ_NAME.get(o.get("class") or o.get("type"))
            if name:
                o.set("name", name)
        root.append(objs)
    if hasattr(ET, "indent"):
        ET.indent(root)            # pretty-print (Python 3.9+); cosmetic only
    ET.ElementTree(root).write(tmx_out, encoding="UTF-8", xml_declaration=True)


def main():
    """`python genart.py bootstrap` -> migrate the Mario map to the CC0 level.tmx
    + level_tiles.png (one-time). With no argument, re-author the sprite strips
    (needs the full Kenney pack via KENNEY_PP). Neither is run by the build."""
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == "bootstrap":
        bootstrap_tmx(MARIO_SRC, os.path.join(HERE, "level_tiles.png"),
                      os.path.join(HERE, "level.tmx"))
        print("level_tiles.png + level.tmx written")
        return
    # Player: Kenney green character (chars 0 stand, 1 walk) + a synthesized jump
    # pose. Walker/enemy: orange block creature (chars 11, 12).
    sprite_strip([kchar(0), kchar(1), jump_pose(kchar(0))], "player.png")
    sprite_strip([kchar(11), kchar(12)], "walker.png")
    print("player.png / walker.png written")
    build_blocks()
    print("blocks.png written")


if __name__ == "__main__":
    main()
