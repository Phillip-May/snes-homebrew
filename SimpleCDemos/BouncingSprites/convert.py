#!/usr/bin/env python3
"""Regenerate BouncingSprites' graphics as build/assets/*.inc via pySnesDevTools.

One 32x32 source sprite drives two OBJ sizes: 16x16 (downsized) and 64x64
(upsized), exercising the converter's automatic resizing.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..")))
os.chdir(HERE)

import pySnesDevTools as snes

snes.sprite("sprites.png", "build/assets/ball16.inc", symbol="ball16",
            size="16x16", bpp=4)
# The 64x64 tiles are large (3840 bytes); place them out of the full bank 0
# (PORT_DATA_BANK1) so the llvm-mos fixed bank has room for the startup RAM
# clear. The annotation collapses to nothing on toolchains that don't need it.
snes.sprite("sprites.png", "build/assets/ball64.inc", symbol="ball64",
            size="64x64", bpp=4, palette=False, section="PORT_DATA_BANK1")
