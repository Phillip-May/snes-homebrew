cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --list-file=build/calypsi.lst -o build/mainBankZero.o src/mainBankZero.c
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --list-file=build/calypsi.lst -o build/celesteSimple.o src/celesteSimple.c
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --list-file=build/calypsi.lst -o build/initsnes.o lib/initsnes.c
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --list-file=build/calypsi.lst -o build/fix16.o src/libfixmath/fix16.c

ln65816 --raw-multiple-memories --rom-code build/mainBankZero.o build/celesteSimple.o build/initsnes.o build/fix16.o src/port/calypsi/linker-large-large.scm clib-lc-ld.a --list-file=calypsi.lst --cross-reference --output-format=intel-hex -o build/calypsi.bin
c:\Python310\python.exe src/port/calypsi/ConvertIntelHex.py  build/calypsi.hex build/calypsi.smc

cc65816 --code-model=large --data-model=large src/mainBankZero.c -o build/mainBankZero.o
cc65816 --code-model=large --data-model=large src/celesteSimple.c -o build/celesteSimple.o
cc65816 --code-model=large --data-model=large lib/initsnes.c -o build/initsnes.o


::cc65816 --code-model=large --data-model=large example.c -o example.o
::ln65816 --debug -o example.o linker-large-large-any.scm clib-lc-sd.a --list-file=hello-debug.lst --cross-reference --rtattr printf=reduced --semi-hosted
