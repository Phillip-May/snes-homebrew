# DemoLua548

Embeds the eLua 0.9 Lua interpreter on the SNES (integer-only, `LUA_NUMBER_INTEGRAL`)
and runs Lua scripts. Built with Calypsi using the huge data model and HiROM
mapping. Bundles the external `elua-0.9` source tree.

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=calypsi
    make clean

Output ROM: `build/mainBankZero_calypsi.smc`. `SUPPORTED_COMPILERS = calypsi`.

## Status

Builds with calypsi (huge data model, HiROM). This demo targets calypsi only;
other compilers are rejected by the `SUPPORTED_COMPILERS` guard.
