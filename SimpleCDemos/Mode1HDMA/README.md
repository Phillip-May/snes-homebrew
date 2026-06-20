# Mode1HDMA

Mode 1 4bpp background with an HDMA effect driven by a runtime-assembled IRQ
handler (the demo writes 65816 opcodes into a buffer at startup). Background
data is converted by `ConvertImageMode1.py`.

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=cc65
    make COMPILER=calypsi
    make COMPILER=llvm-mos
    make clean

Output ROM: `build/mainBankZero_<compiler>.smc`.

## Status

Builds with calypsi, vbcc65816, and tcc816. cc65 and llvm-mos overflow the
fixed ROM bank.
