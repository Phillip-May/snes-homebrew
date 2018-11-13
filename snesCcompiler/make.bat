wdc816cc -WL -SM -MK -MT -ML -I "C:\wdc\Tools\include" mainBankZero.c
wdc816as vectors.asm
wdc816as lib/snes.inc
wdcln -HB -ml -C018000,008000 mainBankZero.obj vectors.obj -C028000,010000 lib/snes.obj -Lcl