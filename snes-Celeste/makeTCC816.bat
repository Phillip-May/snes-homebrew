816-tcc -c example.c -o example.ps
816-opt example.ps > example.asp
constify example.c example.asp example.asm
::Had to fix assembly
wla-65816 -s -x -o example.obj example.asm
wla-65816 -d -s -x -o hdr.obj hdr.asm
