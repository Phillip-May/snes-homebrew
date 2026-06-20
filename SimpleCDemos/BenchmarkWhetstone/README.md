# BenchmarkWhetstone

A port of a Whetstone benchmark (itself a recreation of the original). Mostly a
joke / compiler comparison — the best compiler tops out around 2K FLOPS, so no
serious floating-point work is happening on a 2.67 MHz 65816.

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=cc65
    make COMPILER=calypsi
    make COMPILER=llvm-mos
    make clean

Output ROM: `build/mainBankZero_<compiler>.smc`.

## Status

Builds with calypsi and vbcc65816. cc65 (no floating point), llvm-mos (bank
overflow), and tcc816 (no soft-float runtime) fail.
