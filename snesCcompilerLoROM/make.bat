wdc816cc -WL -SM -MK -MT -ML -WP -MU -MV -I "C:\wdc\Tools\include" mainBankZero.c
wdc816cc -WL -SM -MK -MT -ML -WP -MU -MV -I "C:\wdc\Tools\include" lib\kernel.c
wdc816cc -WL -SM -MK -MT -ML -WP -MU -MV -I "C:\wdc\Tools\include" lib\initsnes.c
wdc816as vectors.asm
wdcln -HB -ML -B -E -T -C018000,008000 mainBankZero.obj vectors.obj -C028000,010000 lib\kernel.obj lib\initsnes.obj -D7E2000,18000 -K048000,20000 -Lcl