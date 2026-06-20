# <DemoName>

<One or two lines: what the demo shows / where it came from.>

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=cc65
    make COMPILER=calypsi
    make COMPILER=llvm-mos
    make clean

Output ROM: `build/mainBankZero_<compiler>.smc`. Declared compilers are in
`SUPPORTED_COMPILERS`; an unsupported one errors unless `ALLOW_UNSUPPORTED=1`.

## Status

<Which compilers currently build, and any known-failing combos.>
