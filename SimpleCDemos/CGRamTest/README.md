# CGRamTest

A port of an early SNES homebrew assembly guide. Essentially boils down to
setting 3 registers to display a CGRAM color.

Guide: https://blog.wesleyac.com/posts/snes-dev-1-getting-started

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=cc65
    make COMPILER=calypsi
    make COMPILER=llvm-mos
    make clean

Output ROM: `build/mainBankZero_<compiler>.smc`.

## Status

Builds with cc65, calypsi, llvm-mos, vbcc65816, and tcc816.
