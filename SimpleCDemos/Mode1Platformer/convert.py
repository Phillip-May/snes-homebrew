#!/usr/bin/env python3
"""Regenerate Mode1Platformer's assets as build/assets/*.inc via pySnesDevTools.

The level is built from `level.tmx` -- a CC0 16x16-metatile Tiled map over
`level_tiles.png` (each tile tagged with a re-skin `category`) -- as its single
source of truth:

  - layout  -> the full SMB 1-1 metatile layout, painted with Kenney CC0 art and
               deduped into a tileset + a column-major tilemap the C engine DMAs
               into VRAM one column at a time while scrolling.
  - collision-> a per-cell solid table from the metatile categories.
  - blockcells-> the ? / brick interactive cells.
  - spawns  -> the player start + enemy positions from the TMX object layer.

The level paint uses the committed Kenney tile sheet (kenney_tilemap_packed.png);
the player/walker/block sprite strips are committed PNGs (authored once by
genart.py) and are not derived from the TMX.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..")))
os.chdir(HERE)

import pySnesDevTools as snes
import genart

OUT = "build/assets"
os.makedirs(OUT, exist_ok=True)

# Level from the TMX: read the per-metatile re-skin categories, paint to
# intermediate PNGs, then run the standard background/collision converters on
# them (column-major map for streaming).
m = genart.load_map()
cat = genart.categories(m)
genart.build_bg(cat, os.path.join(OUT, "bg.png"))
genart.build_collision(cat, os.path.join(OUT, "bg_col.png"))
snes.background(os.path.join(OUT, "bg.png"), os.path.join(OUT, "level.inc"),
                symbol="level", bpp=4, layout="columns")
snes.collision(os.path.join(OUT, "bg_col.png"), os.path.join(OUT, "levelcol.inc"),
               symbol="level")
genart.emit_blockcells(cat, os.path.join(OUT, "blockcells.inc"))
genart.emit_spawns(m.objects, os.path.join(OUT, "spawns.inc"))

# OBJ sprite strips (committed PNGs, not TMX-derived). sprite_sheet lays each
# 16x16 frame out for the OBJ char grid so frame k is at CHARNUM k * <sym>_STEP.
snes.sprite_sheet("player.png", os.path.join(OUT, "player.inc"), symbol="player",
                  frame="16x16", bpp=4)
snes.sprite_sheet("walker.png", os.path.join(OUT, "walker.inc"), symbol="walker",
                  frame="16x16", bpp=4)
# Interactive-block overlays (used-block / brick / coin / score popups) as an OBJ
# strip, and the shared SNES font as a 2bpp tilesheet for the fixed BG3 score HUD
# (glyph N = tile N, so ASCII bytes index straight into the HUD tilemap).
snes.sprite_sheet("blocks.png", os.path.join(OUT, "blocks.inc"), symbol="blocks",
                  frame="16x16", bpp=4)
snes.graphics("../shared/assets/snesfont.png", os.path.join(OUT, "hudfont.inc"),
              symbol="hudfont", bpp=2, palette=False)
