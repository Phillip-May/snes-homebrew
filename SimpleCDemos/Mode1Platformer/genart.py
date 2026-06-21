#!/usr/bin/env python3
"""Author Mode1Platformer's source art from CC0 inputs (one-time, not built).

Reproduces the committed PNGs that `convert.py` turns into C data:

  - bg.png      the full Super Mario Bros World 1-1 layout, re-skinned with
                Kenney "Pixel Platformer" (CC0) tiles. The layout (which tile
                sits where) is taken verbatim from the pvsneslib reference
                `objects/mapandobjects` Tiled map; only the *art* is swapped.
  - bg_col.png  a per-8x8-cell collision mask derived from the reference's
                authoritative per-tile solidity table (tiledMario.b16), so the
                collision matches the original exactly (independent of the
                cosmetic re-skin).
  - player.png  2 frames (stand, walk) from a Kenney character.
  - walker.png  2 frames from a Kenney block creature.

Art: Kenney "Pixel Platformer" 1.2, CC0 (www.kenney.nl). Layout: pvsneslib
sample (zlib). Set KENNEY_PP / PVSNESLIB_REF to point at the inputs.
"""
import json
import os
import struct

from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
KENNEY = os.environ.get(
    "KENNEY_PP",
    r"C:\Users\Admin\AppData\Local\Temp\claude\C--Users-Admin-Documents-snes-homebrew"
    r"\dec2247f-35fc-4153-ba43-cba3bfacac15\scratchpad\kenney_pp")
REF = os.environ.get(
    "PVSNESLIB_REF",
    r"C:\pvsneslib\snes-examples\objects\mapandobjects")

SKY = (107, 140, 255)            # SMB sky blue, becomes a palette colour

# --- Kenney tile sheets -----------------------------------------------------
TILES = Image.open(os.path.join(KENNEY, "Tilemap", "tilemap_packed.png")).convert("RGBA")
CHARS = Image.open(os.path.join(KENNEY, "Tilemap", "tilemap-characters_packed.png")).convert("RGBA")
TCOLS = TILES.width // 18        # 20 tiles per row
CCOLS = CHARS.width // 24        # 9 chars per row


def ktile(idx):
    """A Kenney 18x18 tile, resized to a crisp 16x16 over the sky colour."""
    tx, ty = idx % TCOLS, idx // TCOLS
    cell = TILES.crop((tx * 18, ty * 18, tx * 18 + 18, ty * 18 + 18))
    flat = Image.new("RGBA", (18, 18), SKY + (255,))
    flat.alpha_composite(cell)
    return flat.convert("RGB").resize((16, 16), Image.LANCZOS)


def kchar(idx):
    """A Kenney 24x24 character, resized to 16x16 keeping its alpha."""
    cx, cy = idx % CCOLS, idx // CCOLS
    cell = CHARS.crop((cx * 24, cy * 24, cx * 24 + 24, cy * 24 + 24))
    return cell.resize((16, 16), Image.LANCZOS)


# Kenney tile index used for each re-skin category (sheet is 20 wide).
KTILE = {
    "CLOUD": 154, "BUSH": 125, "POLE": 52, "QBLOCK": 10, "BRICK": 26,
    "PIPE_TOP": 17, "PIPE_BODY": 37, "GREEN_TL": 16, "GREEN_TR": 19,
    "GREEN_FILL": 57, "DARK": 6, "GROUND_TOP": 0, "GROUND_FILL": 24,
}

# The 36 distinct 16x16 metatiles of the reference layout (each a 2x2 gid
# tuple), in the order they first appear, mapped to a re-skin category. "GROUND"
# resolves to a grass top or dirt fill by what sits above it. Collision is NOT
# taken from this table -- it comes from tiledMario.b16 (see build_collision).
META = [
    ((1, 1, 1, 1), "SKY"),       ((1, 1, 1, 4), "CLOUD"),
    ((2, 3, 5, 5), "CLOUD"),     ((1, 1, 6, 1), "CLOUD"),
    ((1, 1, 7, 8), "BUSH"),      ((1, 9, 1, 1), "CLOUD"),
    ((10, 11, 1, 1), "CLOUD"),   ((12, 1, 1, 1), "CLOUD"),
    ((1, 13, 1, 16), "POLE"),    ((14, 15, 17, 15), "POLE"),
    ((18, 15, 19, 15), "POLE"),  ((20, 21, 25, 26), "QBLOCK"),
    ((22, 22, 27, 27), "BRICK"), ((23, 24, 28, 29), "GROUND"),
    ((19, 15, 19, 15), "POLE"),  ((30, 31, 27, 27), "BRICK"),
    ((32, 33, 37, 38), "PIPE_TOP"), ((34, 35, 39, 40), "PIPE_TOP"),
    ((27, 36, 27, 36), "BRICK"), ((27, 27, 27, 27), "BRICK"),
    ((36, 27, 36, 27), "BRICK"), ((1, 1, 47, 48), "GROUND"),
    ((41, 42, 41, 42), "PIPE_BODY"), ((43, 44, 43, 44), "PIPE_BODY"),
    ((45, 46, 27, 27), "BRICK"), ((1, 49, 49, 43), "GREEN_TL"),
    ((43, 50, 43, 43), "GREEN_TR"), ((51, 1, 43, 51), "GREEN_FILL"),
    ((52, 53, 36, 36), "BRICK"), ((43, 43, 43, 43), "GREEN_FILL"),
    ((50, 43, 43, 43), "GREEN_FILL"), ((1, 1, 1, 56), "GREEN_FILL"),
    ((54, 55, 57, 57), "BUSH"),  ((1, 1, 58, 1), "BUSH"),
    ((36, 36, 36, 36), "DARK"),  ((59, 60, 61, 62), "GROUND"),
]
METACAT = dict(META)


def load_layout():
    tmj = json.load(open(os.path.join(REF, "tiledMario.tmj")))
    layer = next(l for l in tmj["layers"] if l["type"] == "tilelayer")
    return layer["width"], layer["height"], layer["data"]


def solid_gids():
    b = open(os.path.join(REF, "tiledMario.b16"), "rb").read()
    props = struct.unpack("<%dH" % (len(b) // 2), b)
    return {i + 1 for i, v in enumerate(props) if v != 0}  # gid = index + 1


def build_bg(W, H, data):
    """Composite the re-skinned wide level, then index it to <=16 colours."""
    mw, mh = W // 2, H // 2                  # metatile grid (16x16 cells)
    cat = [[METACAT.get((data[(my * 2) * W + mx * 2],
                         data[(my * 2) * W + mx * 2 + 1],
                         data[(my * 2 + 1) * W + mx * 2],
                         data[(my * 2 + 1) * W + mx * 2 + 1]), "SKY")
            for mx in range(mw)] for my in range(mh)]
    img = Image.new("RGB", (W * 8, H * 8), SKY)
    for my in range(mh):
        for mx in range(mw):
            c = cat[my][mx]
            if c == "SKY":
                continue
            if c == "GROUND":
                above = cat[my - 1][mx] if my > 0 else "SKY"
                c = "GROUND_FILL" if above == "GROUND" else "GROUND_TOP"
            img.paste(ktile(KTILE[c]), (mx * 16, my * 16))
    pal = img.quantize(colors=16, method=Image.MEDIANCUT)
    pal.save(os.path.join(HERE, "bg.png"))
    return pal.getcolors()


def build_collision(W, H, data, solid):
    """White 8x8 cell where the reference marks that gid solid, else black."""
    img = Image.new("RGB", (W * 8, H * 8), (0, 0, 0))
    white = Image.new("RGB", (8, 8), (255, 255, 255))
    for cy in range(H):
        for cx in range(W):
            if data[cy * W + cx] in solid:
                img.paste(white, (cx * 8, cy * 8))
    img.save(os.path.join(HERE, "bg_col.png"))


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


def main():
    W, H, data = load_layout()
    used = build_bg(W, H, data)
    print("bg.png %dx%d, %d palette colours" % (W * 8, H * 8, len(used)))
    build_collision(W, H, data, solid_gids())
    print("bg_col.png written")
    # Player: Kenney green character (chars 0 stand, 1 walk). Walker: orange
    # block creature (chars 11, 12).
    sprite_strip([kchar(0), kchar(1)], "player.png")
    sprite_strip([kchar(11), kchar(12)], "walker.png")
    print("player.png / walker.png written")


if __name__ == "__main__":
    main()
