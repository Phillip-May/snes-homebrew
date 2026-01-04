# Section Naming Analysis for LLVM-MOS UNROM-512

## Issue Summary

After changing section names from `.prg_rom_X` to `.text.prg_rom_X` and `.rodata.prg_rom_X`, the linker errors got WORSE (larger overflows). This indicates that the custom sections are not being recognized by the LLVM-MOS linker.

## Current State

- **Original section names**: `.prg_rom_1`, `.prg_rom_5`, etc.
- **Changed to**: `.text.prg_rom_1`, `.rodata.prg_rom_1`, `.text.prg_rom_5`, etc.
- **Result**: Larger overflow errors, suggesting sections aren't recognized

## Root Cause

The LLVM-MOS UNROM-512 linker script (managed by `llvm-mlb`) may:
1. Not define custom sections like `.text.prg_rom_X` or `.rodata.prg_rom_X`
2. Use a different section naming convention
3. Handle banking differently (not via section attributes)

## Next Steps

1. **Revert changes**: Go back to `.prg_rom_X` naming to restore previous state
2. **Investigate LLVM-MOS documentation**: Check if UNROM-512 actually supports custom sections for banking
3. **Check linker output**: Generate a map file to see what sections are actually being used
4. **Consider alternative approach**: Banking might need to be done differently (e.g., via function calls, not section attributes)

## Recommendation

Since the section naming changes made things worse, we should:
1. Revert to the original `.prg_rom_X` naming
2. Investigate what LLVM-MOS UNROM-512 actually expects
3. Check if banking is even supported via section attributes, or if it requires a different approach
