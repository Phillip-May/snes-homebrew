# BenchmarkDhrystone

A port of the Dhrystone 2.1 integer benchmark, more useful than Whetstone for
comparing CPU + compiler performance. Somewhat obsolete now that LLVM-MOS can
optimize much of the test away — which is itself a valid measure of an
optimizing compiler's progress.

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=cc65
    make COMPILER=calypsi
    make COMPILER=llvm-mos
    make clean

Output ROM: `build/mainBankZero_<compiler>.smc`.

## Status

Builds with cc65, calypsi, llvm-mos, vbcc65816, and tcc816.
