#!/bin/bash
/c/llvm-mos/bin/mos-sim-spc700-clang.bat -Os -o main.spc main.c -T link.ld -save-temps
