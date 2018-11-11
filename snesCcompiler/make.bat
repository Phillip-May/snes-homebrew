wdc816cc -wl -sm -mk -mt mainBankZero.c
wdc816as vectors.asm
wdcln -HB -C018000,008000 -D00000, mainBankZero.obj vectors.obj
