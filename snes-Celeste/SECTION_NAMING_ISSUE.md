# Section Naming Issue Analysis

## Problem

After changing section names from `.prg_rom_X` to `.text.prg_rom_X` and `.rodata.prg_rom_X`, the linker errors got WORSE, not better. This suggests that:

1. The custom sections are NOT being recognized by the LLVM-MOS linker
2. Everything is defaulting to `prg_rom_fixed` (the fixed bank)
3. The linker script may not define these custom sections

## Current Error Status

Before changes: Overflows were smaller
After changes: Overflows are LARGER (worse)

This indicates the section attributes are being ignored entirely.

## Possible Causes

1. **LLVM-MOS UNROM-512 doesn't use custom linker scripts**: The config file uses `-fpost-link-tool=llvm-mlb` which might use a built-in linker script that doesn't support custom sections.

2. **Wrong section naming convention**: Maybe LLVM-MOS expects different section names entirely, or uses a different mechanism for banking.

3. **Linker script not defining custom sections**: The LLVM-MOS UNROM-512 linker script might not define sections like `.text.prg_rom_X` or `.rodata.prg_rom_X`, so they're ignored.

4. **Banking mechanism is different**: Maybe LLVM-MOS UNROM-512 uses a completely different approach (like function calls or macros) rather than section attributes.

## Next Steps

1. Check if LLVM-MOS UNROM-512 actually supports custom sections for banking
2. Look for LLVM-MOS UNROM-512 examples that show actual banking code
3. Check if the linker script is accessible/editable
4. Consider if banking needs to be done differently (e.g., via function calls rather than section attributes)

## Alternative Approach

If custom sections don't work, we might need to:
- Use a different mapper that supports more flexible banking
- Use a different approach to banking (function calls, manual bank switching without section attributes)
- Check if LLVM-MOS UNROM-512 has built-in banking support that doesn't require section attributes
