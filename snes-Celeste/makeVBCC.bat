::vc +src/port/vbcc/snes-hi-custom-print-mmap -lm src\mainBankZero.c  src\celesteSimple.c lib\initsnes.c src\libfixmath\fix16.c > map.txt
::vc +snes-hi -lm -maxoptpasses=300 -O4 src\mainBankZero.c  src\celesteSimple.c lib\initsnes.c src\libfixmath\fix16.c -o build/vbcc.smc
vc +snes-hi -lm -maxoptpasses=300 -O4 -inline-depth=1000 -unroll-all -fp-associative -force-statics -range-opt src\mainBankZero.c  src\celesteSimple.c src\libfixmath\fix16.c lib\initsnes.c -o build/vbcc.smc
