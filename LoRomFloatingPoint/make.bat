wdc816cc -WL -SM -MK -MT -ML -WP -MU -MV -SI -SP -I "C:\wdc\Tools\include" mainBankZero.c
wdc816cc -WL -SM -MK -MT -ML -WP -MU -MV -SI -SP -I "C:\wdc\Tools\include" lib\kernel.c
wdc816cc -WL -SM -MK -MT -ML -WP -MU -MV -SI -SP -I "C:\wdc\Tools\include" lib\initsnes.c
wdc816as vectors.asm
wdcln -HB -ML -B -E -T -F PROG.LINK
