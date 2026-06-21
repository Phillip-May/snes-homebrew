#!/usr/bin/env python3
"""Regenerate Mode1Platformer's graphics as build/assets/*.inc via pySnesDevTools.

The full Super Mario Bros 1-1 layout (422 tiles wide) as a streaming map: a
deduped tileset plus a column-major tilemap that the C engine DMAs into VRAM one
column at a time while scrolling, a matching per-cell collision table, and two
OBJ animation strips (player + autonomous walker). The source PNGs are authored
by genart.py (Kenney CC0 art + the pvsneslib reference layout).
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..")))
os.chdir(HERE)

import pySnesDevTools as snes

snes.background("bg.png", "build/assets/level.inc", symbol="level", bpp=4,
                layout="columns")
snes.collision("bg_col.png", "build/assets/levelcol.inc", symbol="level")
snes.sprite_sheet("player.png", "build/assets/player.inc", symbol="player",
                  frame="16x16", bpp=4)
snes.sprite_sheet("walker.png", "build/assets/walker.inc", symbol="walker",
                  frame="16x16", bpp=4)
