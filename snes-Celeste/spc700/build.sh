#!/bin/bash
set -e
/c/llvm-mos/bin/mos-sim-spc700-clang.bat -Os -o main.spc main.c -T link.ld -save-temps
# Pad .spc to exact size: 256 header + 65536 RAM + 128 DSP = 65920 bytes
/c/Python37/python.exe -c "
spc = bytearray(open('main.spc','rb').read())
target = 65920  # header(256) + RAM(65536) + DSP(128)
if len(spc) < target:
    # Insert zeros before DSP section to pad RAM to 64KB
    dsp = spc[-128:]  # last 128 bytes are DSP regs
    ram = spc[:-128]  # everything before DSP
    pad = target - len(spc)
    spc = ram + bytearray(pad) + dsp
    open('main.spc','wb').write(spc)
    print(f'Padded {pad} bytes -> {len(spc)} total')
else:
    print(f'Size OK: {len(spc)}')
"
