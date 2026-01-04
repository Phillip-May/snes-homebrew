# UNROM-512 Banking Implementation Summary

## Current Situation
- **Code size**: ~25KB
- **Fixed bank size**: 16KB (UNROM-512 constraint)
- **Overflow**: ~9KB needs to be moved to switchable banks
- **Level data**: Already configured for banks 1-3 (via Python script)
- **Music data**: Planned for bank 4+

## Solution: Move Object System to Bank 5

The object system (object init/update functions, processObject) is the best candidate for moving to a switchable bank because:
1. It's large (~8-12KB estimated)
2. It's only called from `updateAllObjects()` and `initObject()`
3. These functions can easily switch banks before calling object code

## Implementation Steps

### Step 1: Add Section Attributes to Object Functions

Add `__attribute__((section(".prg_rom_5")))` to all object functions in `mainBankZero.c`:

**Object Init Functions:**
- `smokeInit()`
- `breakableWallInit()`
- `collapseTileInit()`
- `springInit()`
- `balloonInit()`
- `keyInit()`
- `chestInit()`
- `monumentInit()`
- `bigChestInit()`
- `doubleDashOrbInit()`
- `strawberryInit()`
- `decoTreeInit()`
- `flyingBerryInit()`

**Object Update Functions:**
- `smokeUpdate()`
- `breakableWallUpdate()`
- `collapseTileUpdate()`
- `springUpdate()`
- `balloonUpdate()`
- `keyUpdate()`
- `chestUpdate()`
- `monumentUpdate()`
- `bigChestUpdate()`
- `doubleDashOrbUpdate()`
- `strawberryUpdate()`
- `decoTreeUpdate()`
- `flyingBerryUpdate()`

**Object Processing:**
- `processObject()` - the large function that dispatches to object functions

**Port Object Functions (in nes.c):**
- `port_buildSmoke()`
- `port_buildBreakableWall()`
- `port_buildSpring()`
- `port_buildCollapseTile()`
- `port_buildStrawberry()`
- `port_buildPlatMov()`
- `port_buildFlyingBerry()`
- `port_buildDoubleDashOrb()`
- `port_buildStaticDecor()`
- `port_buildChest()`
- `port_buildBigChest()`
- `port_buildKey()`
- `port_buildMonument()`
- `port_buildBalloon()`
- `prepare_collapse_tiles_nametable()`
- `execute_collapse_tiles_nametable_writes()`
- `render_object_sprite()` (optional - if not needed in fixed bank)
- `get_object_sprite_data()` (optional - if not needed in fixed bank)

### Step 2: Modify processObject() and updateAllObjects()

Option A: Switch banks inside processObject()
```c
static void processObject(uint8_t index) {
#ifdef __NES_UNROM_512__
    prg_bank_switch(5);  // Switch to object bank
#endif
    // ... existing code that calls object functions ...
}
```

Option B: Switch banks in updateAllObjects() (better - fewer switches)
```c
void updateAllObjects(void) {
#ifdef __NES_UNROM_512__
    prg_bank_switch(5);  // Switch to object bank once
#endif
    for (uint8_t i = 0; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType != OBJ_UNUSED) {
            processObject(i);  // All object functions are in bank 5 now
        }
    }
}
```

Similarly for `initObject()`:
```c
void initObject(enum eOBJType eType, int16_t x, int16_t y) {
#ifdef __NES_UNROM_512__
    prg_bank_switch(5);  // Switch to object bank
#endif
    // ... existing code that calls object init functions ...
}
```

### Step 3: Handle Port Object Functions

For port object build functions called from `buildSpriteIfDirty()`:
- Option 1: Move `buildSpriteIfDirty()` to bank 5 (if it only calls object functions)
- Option 2: Switch banks in `buildSpriteIfDirty()` before calling port functions
- Option 3: Keep port build functions in fixed bank if they're small enough

### Step 4: Test and Verify

1. Build the project
2. Check if fixed bank fits in 16KB
3. If still too large, consider moving more code (e.g., some port functions)

## Bank Organization Summary

- **Bank 0 (Fixed)**: Interrupt handlers, startup, banking code, core game loop, player logic, critical port functions, level access functions, basic math
- **Bank 1**: Level data (levels 1-10)
- **Bank 2**: Level data (levels 11-20)
- **Bank 3**: Level data (levels 21-31)
- **Bank 4**: Music data (future)
- **Bank 5**: Object system code (init/update functions, processObject, port object functions)

## Expected Result

Moving the object system to bank 5 should save ~10-15KB from the fixed bank, bringing it from ~25KB down to ~10-15KB, which fits comfortably in the 16KB fixed bank.
