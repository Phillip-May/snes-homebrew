#!/usr/bin/env python3
"""Regenerate Mode1HDMA's background as build/assets/bg.inc via pySnesDevTools."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..")))
os.chdir(HERE)

import pySnesDevTools as snes

snes.background("bg_001.png", "build/assets/bg.inc",
                symbol="bg", bpp=4, palette="per_scanline")
