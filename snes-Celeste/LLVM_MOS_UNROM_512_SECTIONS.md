# LLVM-MOS UNROM-512 Section Naming - FOUND!

## Key Finding

The linker script at `C:\llvm-mos\mos-platform\nes-unrom-512\lib\_prg-rom-banked.ld` shows that:

**The section naming `.prg_rom_X` is CORRECT!**

The linker script defines sections as:
```ld
.prg_rom_0 : { *(.prg_rom_0 .prg_rom_0.*) } >prg_rom_0
.prg_rom_1 : { *(.prg_rom_1 .prg_rom_1.*) } >prg_rom_1
.prg_rom_2 : { *(.prg_rom_2 .prg_rom_2.*) } >prg_rom_2
...
.prg_rom_5 : { *(.prg_rom_5 .prg_rom_5.*) } >prg_rom_5
...
```

So using `__attribute__((section(".prg_rom_5")))` should work!

## The Real Problem

Looking at the linker script:
- `REGION_ALIAS("c_readonly", prg_rom_fixed)` - This means default readonly sections go to the fixed bank!
- `prg_rom_fixed : ORIGIN = __prg_rom_fixed_lma, LENGTH = 0x4000 - 0x6` (16378 bytes, just under 16KB)

So the issue is that:
1. Code/data WITHOUT section attributes goes to `prg_rom_fixed` (the default)
2. Code/data WITH `.prg_rom_X` section attributes should go to the switchable banks
3. But the linker errors show everything is going to `prg_rom_fixed`, which means the custom sections aren't being recognized

## Why Sections Might Not Be Working

The sections `.prg_rom_X` are defined in the linker script, so they should work. The fact that they're not working suggests:

1. The sections are being placed, but maybe there's an issue with how they're being used
2. Maybe there's a conflict or issue with the linking process
3. Maybe the code needs to be recompiled/relinked after adding section attributes

## Next Steps

1. The section naming `.prg_rom_X` is correct (no `.text.` or `.rodata.` prefix needed)
2. The sections should work as-is
3. The issue might be that the linker isn't seeing these sections, or there's another problem
4. We should verify the build process is correctly using the LLVM-MOS linker script
