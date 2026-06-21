#!/usr/bin/env python3
"""Regenerate the shared SNES font as build/assets/imagedata.inc via pySnesDevTools."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..")))
os.chdir(HERE)

import pySnesDevTools as snes

snes.graphics("../shared/assets/snesfont.png", "build/assets/imagedata.inc",
              symbol="snesfont", bpp=1, palette=False)
