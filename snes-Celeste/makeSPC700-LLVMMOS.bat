set PATH=%PATH%;C:\Users\Admin\Documents\llvm-mos\buildSPC\bin
set PATH=%PATH%;C:\llvm-mos\mos-platform\common\lib
mos-clang.exe -nostdlib -mcpu=spc700 -o main.spc -Os main.c
