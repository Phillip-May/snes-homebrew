# BouncingSprites

A re-creation of the pvsneslib `objects/moveobjects` example, but the sprites
move at varied speeds, bounce off the screen edges *and* off each other
(AABB overlap, resolved on the axis of least penetration with a velocity-
component exchange) instead of jittering randomly, and two OBJ sizes are on
screen at once.

A single 32x32 source sprite (`sprites.png`) drives **both** sizes, regenerated
at build time by `convert.py` via `pySnesDevTools` (see `../../pySnesDevTools/`):

- **16x16** small sprites — the source **downsized** 32→16
- **64x64** large sprites — the source **upsized** 32→64

This exercises the converter's automatic resizing in both directions. The SNES
`OBJSEL` register only exposes two OBJ sizes at once (one small + one large from
a fixed pair), so the demo uses the 16x16 & 64x64 pair; the per-sprite size bit
in the high OAM table selects which each sprite uses.

## Background effect

A scrolling rainbow gradient fills the screen behind the sprites, built only
from the background palette and HDMA. One HDMA channel (mode 3) streams a colour
into the backdrop entry `CGRAM[0]` once per band of scanlines — each entry
writes `CGADD=0` then a 15-bit colour to `$2122`. The screen is split into 56
bands of 4 lines; every frame the C code rewrites just the colour bytes of the
RAM-resident HDMA table (a cheap triangle-wave rainbow whose phase advances each
frame), so the gradient animates with no per-scanline CPU work. No BG layer is
used — the backdrop colour shows wherever a sprite pixel is transparent.

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=calypsi      # or llvm-mos, vbcc65816, tcc816
    make clean

Output ROM: `build/mainBankZero_<compiler>.smc`.

## Status

Runs on calypsi, llvm-mos, vbcc65816, and tcc816 (verified in an emulator: both
sprite sizes bounce).

llvm-mos needs the position/velocity state kept out of initialized mutable data
(this toolchain corrupts it at runtime), so the seed values live in `const` ROM
tables and only the live copies are in RAM, filled at startup.

cc65 is excluded: it builds a ROM but nothing initializes at runtime (the
graphics never DMA in; VRAM/CGRAM/OAM stay at power-on values). This is a cc65
issue, not a bank overflow.
