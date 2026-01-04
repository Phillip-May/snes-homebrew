# UNROM-512 Banking Notes

## Current Setup

- **Makefile**: Uses `mos-nes-unrom-512.cfg`
- **Banking code**: Already in `src/port/nes.c` (wrapped in `#ifdef __NES_UNROM_512__`)
- **Level data**: Python script adds section attributes (`.prg_rom_1`, `.prg_rom_2`, `.prg_rom_3`)

## Bank Organization

- **Bank 0 (Fixed)**: Core code, interrupt handlers, reset code (16KB at $C000-$FFFF)
- **Bank 1**: Levels 1-10
- **Bank 2**: Levels 11-20
- **Bank 3**: Levels 21-31
- **Bank 4+**: Available for music data, additional code, etc.

## Adding Music Data to Banks

When you add music data, use section attributes like this:

```c
// Place music data in bank 4
__attribute__((section(".prg_rom_4")))
const unsigned char music_data[] = {
    // ... music data ...
};
```

Then switch to bank 4 before accessing music:

```c
#ifdef __NES_UNROM_512__
    prg_bank_switch(4);  // Switch to music bank
#endif
// Access music data
```

## Regenerating Level Headers

After modifying `convertSpriteSheetToNES.py`, regenerate level headers to apply banking attributes:

```bash
python python/convertSpriteSheetToNES.py
```

## Important Notes

- The `level_data` array in `nes.c` should stay in the fixed bank (no section attribute)
- Only the actual level data arrays (tilemaps, palettes, objects) go in switchable banks
- Bank switching happens automatically before accessing level data (already implemented)
