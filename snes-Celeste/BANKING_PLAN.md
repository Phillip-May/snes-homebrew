# UNROM-512 Banking Plan (16KB Fixed + 16KB Switchable)

## Problem Analysis
- **Current code size**: ~25KB
- **Fixed bank size**: 16KB (UNROM-512 constraint at $C000-$FFFF)
- **Overflow**: ~9KB needs to be moved to switchable banks
- **Level data**: Already configured for banks 1-3 (via Python script)
- **Music data**: Planned for bank 4+

## Key Findings
- `fix16_trig_sin_lut.h` (205KB lookup table) is **NOT being compiled** (FIXMATH_SIN_LUT not defined)
- Trigonometric functions don't appear to be used in game code
- Object system code is the best candidate for banking (~8-12KB estimated)

## Bank Organization

### Bank 0 (Fixed - 16KB at $C000-$FFFF) - MUST FIT HERE

**Critical code that must always be accessible:**

1. **Interrupt Handlers & Startup** (~1-2KB)
   - `interuptVBlank()` - called from NMI
   - `snesXC_nmi()`, `snesXC_cop()`, `snesXC_brk()`, `snesXC_abort()`
   - `_start()` (startup code from crt0)
   - `main()` entry point
   - `port_init()` - initialization

2. **Banking Infrastructure** (~0.5KB)
   - `prg_bank_switch()` - bank switching function
   - `get_level_bank()` - level-to-bank mapping
   - All banking code must be in fixed bank

3. **Critical Port Functions** (~2-3KB)
   - `port_getInputs()` - called every frame
   - `port_updatePlayerSprite()` - called every frame
   - `port_beginSpriteBuild()`, `port_finishSpriteBuild()`
   - `port_LoadRoomData()` - loads levels (switches banks internally)
   - `port_resetSprites()`
   - Basic sprite rendering: `render_16x16_sprite()`, `hide_sprites()`
   - OAM management functions

4. **Level Data Access Functions** (~1-2KB)
   - `load_background_palettes()`, `load_sprite_palettes()` - switch banks before accessing
   - `write_nametable()` - switch banks before accessing
   - `decompress_tilemap()` - used during level loading

5. **Core Game Loop** (~1-2KB)
   - `onVblank()` - main game loop function
   - `LoadRoomData()` wrapper (calls port_LoadRoomData)
   - `LoadNextRoom()`

6. **Player Core Logic** (~3-4KB)
   - `playerUpdate()` - CRITICAL, called every frame
   - `playerInit()` - initialization
   - `OBJ_isSolidAt()` - collision detection (called frequently)
   - `isTileSolidAtPoint()` - tile collision

7. **Object System Core Dispatchers** (~1-2KB)
   - `updateAllObjects()` - iterates and calls processObject
   - `initObject()` - object initialization dispatcher
   - `buildSpriteIfDirty()` - sprite building wrapper
   - `clearObjectDirtyFlag()` - utility
   - **NOTE**: These dispatchers stay in fixed bank, but switch to bank 5 before calling object functions

8. **Math Library - Essential Functions Only** (~2-3KB)
   - fix16_mul, fix16_add, fix16_sub, fix16_div
   - Basic math used in hot paths
   - fix16_from_float, fix16_to_int, etc.

9. **Global Variables** (in RAM, not ROM)
   - GLOBAL_PlayerData, GLOBAL_ActiveLevel, GLOBAL_OBJList, etc.
   - These are in RAM, not counting against ROM space

10. **Library Code** (~2-3KB)
    - LLVM-MOS runtime (crt0, startup)
    - NES library functions (neslib)

**Total Fixed Bank Target: ~16KB**

### Bank 1: Level Data - Levels 1-10
- Already planned via Python script section attributes
- `tilemap_level1_compressed` through `tilemap_level10_compressed`
- Palettes, objects for levels 1-10

### Bank 2: Level Data - Levels 11-20
- Already planned via Python script section attributes
- Levels 11-20 data

### Bank 3: Level Data - Levels 21-31
- Already planned via Python script section attributes  
- Levels 21-31 data

### Bank 4: Music Data
- Music/sound data (when added)
- Use: `__attribute__((section(".prg_rom_4")))`

### Bank 5: Object System Code ⭐ PRIMARY TARGET
**Move ALL object-specific code here:**

**Object Init Functions (mainBankZero.c):**
- `smokeInit()`, `breakableWallInit()`, `collapseTileInit()`
- `springInit()`, `balloonInit()`, `keyInit()`
- `chestInit()`, `monumentInit()`, `bigChestInit()`
- `doubleDashOrbInit()`, `strawberryInit()`, `decoTreeInit()`, `flyingBerryInit()`

**Object Update Functions (mainBankZero.c):**
- `smokeUpdate()`, `breakableWallUpdate()`, `collapseTileUpdate()`
- `springUpdate()`, `balloonUpdate()`, `keyUpdate()`
- `chestUpdate()`, `monumentUpdate()`, `bigChestUpdate()`
- `doubleDashOrbUpdate()`, `strawberryUpdate()`, `decoTreeUpdate()`, `flyingBerryUpdate()`

**Object Processing (mainBankZero.c):**
- `processObject()` - large function that dispatches to object functions

**Port Object Functions (nes.c):**
- `port_buildSmoke()`, `port_buildBreakableWall()`, `port_buildSpring()`
- `port_buildCollapseTile()`, `port_buildStrawberry()`, `port_buildPlatMov()`
- `port_buildFlyingBerry()`, `port_buildDoubleDashOrb()`, `port_buildStaticDecor()`
- `port_buildChest()`, `port_buildBigChest()`, `port_buildKey()`
- `port_buildMonument()`, `port_buildBalloon()`
- `prepare_collapse_tiles_nametable()`, `execute_collapse_tiles_nametable_writes()`
- `render_object_sprite()` (optional - if not needed in fixed bank)
- `get_object_sprite_data()` (optional - if not needed in fixed bank)

**Estimated size: ~8-12KB**

**Bank switching pattern:**
```c
void updateAllObjects(void) {
#ifdef __NES_UNROM_512__
    prg_bank_switch(5);  // Switch to object bank once
#endif
    for (uint8_t i = 0; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType != OBJ_UNUSED) {
            processObject(i);  // All object functions are in bank 5
        }
    }
}

void initObject(enum eOBJType eType, int16_t x, int16_t y) {
#ifdef __NES_UNROM_512__
    prg_bank_switch(5);  // Switch to object bank
#endif
    // ... existing code that calls object init functions ...
}
```

## Implementation Steps

### Step 1: Add Section Attributes to Object Functions

Add `__attribute__((section(".prg_rom_5")))` before function definitions:

```c
// In mainBankZero.c
#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5")))
#endif
void smokeInit(uint8_t index) { ... }

#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5")))
#endif
void smokeUpdate(uint8_t index) { ... }

// ... repeat for all object functions ...

#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5")))
#endif
static void processObject(uint8_t index) { ... }
```

```c
// In nes.c
#ifdef __NES_UNROM_512__
__attribute__((section(".prg_rom_5")))
#endif
void port_buildSmoke(uint8_t index) { ... }

// ... repeat for all port object functions ...
```

### Step 2: Modify Dispatcher Functions

Update `updateAllObjects()` and `initObject()` to switch banks:

```c
void updateAllObjects(void) {
#ifdef __NES_UNROM_512__
    prg_bank_switch(5);  // Switch to object bank
#endif
    for (uint8_t i = 0; i < GLBOAL_OBJ_LIST_SIZE; i++) {
        if (GLOBAL_OBJList[i].eType != OBJ_UNUSED) {
            processObject(i);
        }
    }
}

void initObject(enum eOBJType eType, int16_t x, int16_t y) {
#ifdef __NES_UNROM_512__
    prg_bank_switch(5);  // Switch to object bank
#endif
    // ... rest of function ...
}
```

### Step 3: Handle buildSpriteIfDirty()

If `buildSpriteIfDirty()` calls port object functions, either:
- Option A: Move `buildSpriteIfDirty()` to bank 5
- Option B: Switch banks in `buildSpriteIfDirty()` before calling port functions
- Option C: Keep port build functions in fixed bank if they're small enough

### Step 4: Build and Test

1. Build the project
2. Check if fixed bank fits in 16KB
3. If still too large, consider:
   - Moving additional code to bank 5
   - Optimizing code size
   - Moving some port functions

## Critical Dependencies

1. **Interrupt handlers** → Must be in fixed bank (vectors point to them)
2. **Bank switching function** → Must be in fixed bank (used to switch banks)
3. **Functions called from interrupts** → Must be in fixed bank
4. **Functions called every frame** → Prefer fixed bank, but can bank switch
5. **Level data access** → Already handles banking (switches before access)

## Expected Savings

- **Object system code**: ~8-12KB
- **Total savings**: ~8-12KB (should be enough to fit in 16KB fixed bank)

## Success Criteria

Fixed bank should be ≤ 16KB after moving object system to bank 5.
