#!/usr/bin/env python3
"""Regenerate Mode1Sprite's graphics as build/assets/*.inc via pySnesDevTools."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..")))
os.chdir(HERE)

import pySnesDevTools as snes

snes.background("school.png", "build/assets/school.inc", symbol="school", bpp=4)
snes.sprite("biker.png", "build/assets/biker.inc", symbol="biker", size="32x32", bpp=4)
