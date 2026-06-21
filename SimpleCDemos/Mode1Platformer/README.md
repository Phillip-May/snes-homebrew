# Mode1Platformer

A re-creation of the *spirit* of the pvsneslib `objects/mapandobjects` example —
a scrolling tile map with a pad-controlled player sprite and an autonomous
animated creature moving over it with simple tile collision — rebuilt in this
collection's style. Where pvsneslib hides the map engine, the dynamic-sprite
streamer and the object engine inside its library, this demo implements only
what it needs, directly and readably, in `mainBankZero.c`.

## What it does

- A **64×32-tile** background (512×256 px, two SNES screens wide) lives entirely
  in VRAM and scrolls horizontally via the hardware BG scroll register
  (`REG_BG1HOFS`) following a camera that tracks the player and clamps to the map.
- A **player** sprite with gravity, walk and jump: read the pad (`REG_JOY1L/H`),
  move with solid-tile collision, multi-frame walk animation, OBJ H-flip for
  facing. Walk off into the pit and you respawn at the start.
- One **autonomous walker** that paces the ground and turns around at a wall
  (its move was blocked) or at a ledge (no ground one pixel ahead of its leading
  foot). It shares the player's movement/collision code; there is no generic
  object engine.
- Sprite animation loads **all frames into VRAM once** and switches the OAM
  CHARNUM per frame (no per-frame tile streaming).

## Assets

Source art is committed as indexed PNGs (index 0 = transparent/backdrop) and the
C data is regenerated at build time by `convert.py` via `pySnesDevTools`
(see `../../pySnesDevTools/`):

- `bg.png` — the level. `background()` dedupes it to a tiny tileset (sky / grass
  / dirt / brick) plus a 64×32 tilemap, split into the two SNES screen blocks.
- `bg_col.png` — a black/white collision mask the same size as `bg.png`. The new
  `collision()` converter turns each 8×8 cell into one byte of `level_col`
  (white = solid), which the C collision code indexes directly.
- `player.png` / `walker.png` — horizontal sprite strips. The new
  `sprite_sheet()` converter lays each 16×16 frame out for the OBJ char grid so
  frame *k* is reachable by `CHARNUM = k * <symbol>_STEP`; it also emits
  `<symbol>_TILES` so the C side knows where the next sprite block begins.

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=calypsi      # or vbcc65816, tcc816
    make clean

Output ROM: `build/mainBankZero_<compiler>.smc` (`.sfc` for tcc816).

## Status

Runs on **calypsi, vbcc65816 and tcc816** (verified in Mesen: the map renders
and scrolls with the camera, the player walks/jumps and is stopped by the brick
pillar, and the walker paces and turns at the pillar and the pit). cc65 and
llvm-mos are excluded: the 4 KB tilemap plus tile/sprite data overflows their
single fixed ROM bank (a linker error on llvm-mos).

### A calypsi gotcha worth knowing

calypsi (65816 5.12, `-O2`) miscompiles a **signed `int16_t >> const` of a
positive value** — `640 >> 4` returns `-8`, `640 >> 3` returns `0` — while
unsigned shift, left shift, `uint8_t` shift and multiply are all correct. The
fixed-point→pixel and pixel→cell conversions therefore go through the `toPix` /
`toCell` helpers, which use the unsigned shift and restore the sign by hand.
This silently broke all collision before it was tracked down.
