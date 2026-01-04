# Memory Layout Issue Summary

## Confirmed Findings

1. **Mapper**: UNROM-512 (Mapper 30) ✅
2. **Section Naming**: `.prg_rom_X` is CORRECT (verified in `C:\llvm-mos\mos-platform\nes-unrom-512\lib\_prg-rom-banked.ld`) ✅
3. **Fixed Bank**: 16KB (0x4000 - 6 = 16378 bytes) at $C000-$FFFF
4. **Switchable Banks**: Multiple 16KB banks (`.prg_rom_0` through `.prg_rom_30`)

## The Problem

The linker errors show that sections are overflowing `prg_rom_fixed`:
- `.text` overflows by 1029 bytes
- `.rodata` overflows by 3139 bytes
- `.data` overflows by 3139 bytes
- etc.

This means **everything is going to the fixed bank** because:

1. **Most code/data has NO section attributes** → defaults to `prg_rom_fixed`
2. Only a small amount of code/data has `.prg_rom_X` attributes
3. The linker script has `REGION_ALIAS("c_readonly", prg_rom_fixed)`, so default sections go to fixed bank

## Current Code Placement

**With Section Attributes (in switchable banks):**
- Some object functions: `.prg_rom_5`
- Level data: `.prg_rom_1`, `.prg_rom_2`, `.prg_rom_3`

**Without Section Attributes (in fixed bank):**
- Most functions (main, playerUpdate, processObject, etc.)
- Most data (rand_table, monumentText, etc.)
- All default `.text`, `.rodata`, `.data` sections

## Solution

The section naming is correct. The problem is that **not enough code/data is being moved to switchable banks**. 

We need to:
1. Add `.prg_rom_X` section attributes to MORE code/data
2. Move large functions/data out of the fixed bank
3. Keep only essential code in the fixed bank (interrupt handlers, main, critical functions)

## Next Steps

1. Identify which code/data can be moved to switchable banks
2. Add section attributes to move code/data out of fixed bank
3. Ensure critical code (interrupts, main) stays in fixed bank
4. Balance the fixed bank to fit within 16KB
