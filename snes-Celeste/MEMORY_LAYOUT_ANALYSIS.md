# NES UNROM-512 Memory Layout Analysis

## Problem Summary

The build is failing with linker errors indicating sections are overflowing their memory regions:

```
ld.lld: error: section '.text' will not fit in region 'prg_rom_fixed': overflowed by 1029 bytes
ld.lld: error: section '.rodata' will not fit in region 'prg_rom_fixed': overflowed by 3139 bytes
ld.lld: error: section '.data' will not fit in region 'prg_rom_fixed': overflowed by 3139 bytes
ld.lld: error: section '.zp.data' will not fit in region 'prg_rom_fixed': overflowed by 3177 bytes
ld.lld: error: section '.chr_rom' will not fit in region 'chr_rom': overflowed by 8192 bytes
ld.lld: error: section '.rom_poke_table_0' will not fit in region 'prg_rom_fixed': overflowed by 3178 bytes
```

## UNROM-512 Memory Architecture

**UNROM-512 Mapper 30:**
- **Fixed Bank**: 16KB at $C000-$FFFF (always accessible)
- **Switchable Bank**: 16KB at $8000-$BFFF (can switch between up to 512KB total)
- **CHR-ROM**: 8KB (fixed)

The fixed bank has **only 16KB** available, but the code/data is trying to use **much more**.

## Current Code Placement

### 1. Default Sections (Going to `prg_rom_fixed` - 16KB)

**These sections contain code/data WITHOUT custom section attributes:**

#### `.text` (Code) - Overflowing by 1029 bytes
**Functions currently in fixed bank (no section attribute):**
- `main()` - entry point ✅ (must be in fixed)
- `onVblank()` - main game loop ✅ (must be in fixed)
- `interuptVBlank()` - NMI handler ✅ (must be in fixed)
- `snesXC_nmi()`, `snesXC_cop()`, `snesXC_brk()`, `snesXC_abort()` ✅ (must be in fixed)
- `playerUpdate()` - called every frame ⚠️ (could be banked but currently in fixed)
- `playerInit()` - initialization
- `OBJ_isSolidAt()` - collision detection (called frequently)
- `isTileSolidAtPoint()` - tile collision
- `updateAllObjects()` - object dispatcher
- `initObject()` - object initialization dispatcher
- `processObject()` - object processing (LARGE function)
- `buildSpriteIfDirty()` - sprite building wrapper
- `clearObjectDirtyFlag()` - utility
- `LoadRoomData()`, `LoadNextRoom()` - level loading
- `port_init()` - initialization
- `port_getInputs()` - called every frame
- `port_updatePlayerSprite()` - called every frame
- `port_vblank()` - called every frame
- `port_beginSpriteBuild()`, `port_finishSpriteBuild()` - sprite building
- `port_LoadRoomData()` - level loading (switches banks internally)
- All math library functions (`fix16_mul`, `fix16_add`, etc.)
- Library code (LLVM-MOS runtime, neslib)

**Functions with `.prg_rom_5` attribute (should go to switchable bank 5):**
- Object init functions: `smokeInit()`, `breakableWallInit()`, `collapseTileInit()`, etc.
- Object update functions: `smokeUpdate()`, `breakableWallUpdate()`, etc.
- Port object functions: `port_buildSmoke()`, `port_buildBreakableWall()`, etc.

#### `.rodata` (Read-only data) - Overflowing by 3139 bytes
**Data currently in fixed bank:**
- `rand_table[256]` - random number table (512 bytes) in `mainBankZero.c`
- `monumentText[][]` - text strings
- Level data arrays (tilemap, objects, palettes) - **These have section attributes in generated headers**
- Generated sprite data headers

**Data with section attributes:**
- Level data: `.prg_rom_1`, `.prg_rom_2`, `.prg_rom_3` (via Python script)
- Some shared data: `.prg_rom_5` (via Python script)

#### `.data` (Initialized data) - Overflowing by 3139 bytes
**Global variables (should be in RAM, not ROM):**
- `GLOBAL_PlayerData` - player state
- `GLOBAL_ActiveLevel` - level state
- `GLOBAL_OBJList[]` - object list
- `GLOBAL_InputState` - input state
- Various static variables

**Note:** `.data` section contains initialized data that needs to be copied to RAM at startup. The overflow suggests too much initialized data.

#### `.zp.data` (Zero page initialized data) - Overflowing by 3177 bytes
**Zero page variables:**
- Should be minimal, but overflow suggests too much data in zero page

#### `.chr_rom` - Overflowing by 8192 bytes (trying to use 16KB, only 8KB available)
**CHR-ROM data:**
- Sprite CHR data: `sprite_chr.o` - appears to be 16KB but UNROM-512 only supports 8KB CHR-ROM
- This is a separate issue from PRG-ROM banking

#### `.rom_poke_table_0` - Overflowing by 3178 bytes
**ROM poke table:**
- LLVM-MOS uses this for bank switching tables
- Should be minimal, overflow suggests it's too large

## Root Cause Analysis

### Problem 1: Too Much Code in Fixed Bank
Most functions don't have section attributes, so they default to `.text` → `prg_rom_fixed`. Only object functions have `.prg_rom_5` attributes, but:
1. **Custom sections like `.prg_rom_5` need to be defined in the linker script**
2. LLVM-MOS UNROM-512 config may not automatically map `.prg_rom_5` to switchable banks
3. The linker script might not have sections defined for `.prg_rom_1`, `.prg_rom_2`, `.prg_rom_3`, `.prg_rom_5`, etc.

### Problem 2: Custom Sections Not Defined
The code uses attributes like:
```c
__attribute__((section(".prg_rom_5")))
```
But if the linker script doesn't define `.prg_rom_5` section mapping, the linker may:
- Ignore the attribute and use default sections
- Map it incorrectly
- Put it in the fixed bank anyway

### Problem 3: CHR-ROM Size Mismatch
The sprite CHR data is 16KB, but UNROM-512 mapper only supports 8KB CHR-ROM. Need to reduce CHR data or use CHR-RAM.

### Problem 4: Large Functions Still in Fixed Bank
- `processObject()` - large switch statement dispatching to object functions
- `playerUpdate()` - large function called every frame
- Various port functions

## Expected Solution

### Step 1: Verify Linker Script Defines Custom Sections
The LLVM-MOS UNROM-512 config needs to define sections like:
- `.prg_rom_1` → switchable bank 1
- `.prg_rom_2` → switchable bank 2
- `.prg_rom_3` → switchable bank 3
- `.prg_rom_5` → switchable bank 5

If these aren't defined, we need to:
- Check if LLVM-MOS has a linker script that defines them
- Or create a custom linker script
- Or use different section names that LLVM-MOS recognizes

### Step 2: Move More Code to Switchable Banks
Based on BANKING_PLAN.md, we need to:
1. Keep critical code in fixed bank:
   - Interrupt handlers ✅
   - Entry point/main ✅
   - Bank switching functions ✅
   - Functions called from interrupts ✅
   
2. Move to switchable banks:
   - Object system code (already marked with `.prg_rom_5`) ✅
   - Level data (already marked with `.prg_rom_1/2/3`) ✅
   - Large functions like `processObject()` (needs section attribute)
   - `playerUpdate()` (could stay in fixed if needed, or move to bank)

### Step 3: Fix CHR-ROM Size
- Reduce sprite CHR data to 8KB
- Or switch to a mapper that supports 16KB CHR-ROM
- Or use CHR-RAM if available

### Step 4: Reduce Data in Fixed Bank
- Move large data tables (like `rand_table`) to switchable banks
- Ensure global variables are in RAM (`.data` section shouldn't be in ROM)

## Current Status

### What's Working:
- Level data has section attributes (`.prg_rom_1`, `.prg_rom_2`, `.prg_rom_3`)
- Object functions have section attributes (`.prg_rom_5`)
- Python script generates headers with correct section attributes

### What's Not Working:
- Custom sections (`.prg_rom_1`, `.prg_rom_2`, `.prg_rom_3`, `.prg_rom_5`) may not be defined in linker script
- Most functions don't have section attributes → default to fixed bank
- CHR-ROM is 16KB but mapper only supports 8KB
- Too much code/data in fixed bank (exceeds 16KB limit)

## Next Steps

1. **Check LLVM-MOS UNROM-512 linker script** to see if custom sections are defined
2. **Add section attributes to large functions** that can be banked
3. **Verify linker actually respects section attributes** for custom sections
4. **Reduce CHR-ROM size** from 16KB to 8KB
5. **Move data tables to switchable banks** if possible
6. **Ensure global variables are in RAM**, not ROM
