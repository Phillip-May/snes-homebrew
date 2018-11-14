wdc816cc -WL -SM -MK -MT -ML -WP -I "C:\wdc\Tools\include" mainBankZero.c
wdc816cc -WL -SM -MK -MT -ML -WP -I "C:\wdc\Tools\include" kernel.c
wdc816as vectors.asm
wdc816as lib/snes.inc
wdcln -HB -ML -B -E -T -C018000,008000 mainBankZero.obj vectors.obj -C028000,010000 lib/snes.obj kernel.obj -D7E2000,18000 -Lcl