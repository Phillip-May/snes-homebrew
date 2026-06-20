# Mode1Sprite

Demo showing a Mode 1 4bpp background and a sprite moving with player input.
The background (`school.png`) and sprite (`biker.png`) are regenerated at build
time by `pySnesDevTools bg` / `sprite` (see `../../pySnesDevTools/`).

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=cc65
    make COMPILER=calypsi
    make COMPILER=llvm-mos
    make clean

Output ROM: `build/mainBankZero_<compiler>.smc`.

## Status

Builds with calypsi, vbcc65816, and tcc816. cc65 and llvm-mos overflow the
fixed ROM bank (image data exceeds the single-bank layout).
