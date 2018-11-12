wdc816cc -wl -sm -mk -mt -ml -I "C:\wdc\Tools\include" mainBankZero.c
wdc816as vectors.asm
wdc816as lib/snes.inc
wdcln -HB -ml -C018000,008000 mainBankZero.obj vectors.obj lib/snes.obj -LCL