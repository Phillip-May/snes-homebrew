rm build\mainBankZero.smc
wdc816cc -WL -SM -MK -MT -ML -WP -MU -MV -SI -SP -I "C:\wdc\Tools\include" -I ".\lib" -I ".\src" -I ".\src\libfixmath" -D__WDC816CC__=1 src\mainBankZero.c
wdc816cc     -SM -MK -MT -ML -WP -MU -MV -SI     -I "C:\wdc\Tools\include" -I ".\lib" -I ".\src" -I ".\src\libfixmath" -D__WDC816CC__=1 src\celesteSimple.c
wdc816cc -WL -SM -MK -MT -ML -WP -MU -MV -SI -SP -I "C:\wdc\Tools\include" -I ".\lib" -I ".\src" -I ".\src\libfixmath" -D__WDC816CC__=1 src\port\wdc816cc\kernel.c
wdc816cc -WL -SM -MK -MT -ML -WP -MU -MV -SI -SP -I "C:\wdc\Tools\include" -I ".\lib" -I ".\src" -I ".\src\libfixmath" -D__WDC816CC__=1 lib\initsnes.c
wdc816as src\port\wdc816cc\vectors.asm
wdcln -HB -ML -B -E -T -F src\port\wdc816cc\PROG.LINK
move src\mainBankZero.bin build
ren build\mainBankZero.bin mainBankZero.smc
