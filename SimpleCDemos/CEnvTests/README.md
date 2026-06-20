# CEnvTests

Exercises the C banking runtime: places data and const arrays in WRAM bank
`$7E` and in ROM banks 0–3 via the `BANK*_CONST` / `BANK7E_DATA` section
attributes and the `SET_*_BANK` macros, then reads back the active data bank.
Validates the banking abstraction in `shared/src/snes_regs_xc.h`.

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=cc65
    make COMPILER=calypsi
    make COMPILER=llvm-mos
    make clean

Output ROM: `build/mainBankZero_<compiler>.smc`.

## Status

Builds with calypsi, llvm-mos, and vbcc65816. cc65 cannot parse the source.
