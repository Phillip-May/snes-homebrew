# Banking Dependencies Analysis

## Functions Called by Object Functions (in Bank 5)

### Must Move to Bank 5 (called from object functions):
1. **initSimpleDecorSprite()** - called by flowerInit, decoTreeInit
2. **updateSimpleDecorSprite()** - called by flowerUpdate, decoTreeUpdate  
3. **port_updateCollapseTileNametable()** - called from collapseTileUpdate

### Can Stay in Fixed Bank (small utilities):
1. **playSoundEffect()** - placeholder function (tiny)
2. **randint16()** - random number generator (small utility)
3. **sign()** - small utility function
4. **clearObjectDirtyFlag()** - small utility (or move to bank 5 if convenient)

### Must Stay in Fixed Bank:
1. **initObject()** - dispatcher, called from fixed bank code, switches to bank 5
2. **FIXED_ macros** - expand to fix16_ functions (in math library, fixed bank)
3. **GLOBAL_ variables** - in RAM, not ROM
4. **approachFixed()** - only called from playerUpdate (fixed bank), not from object functions

## Functions Called from processObject/buildSpriteIfDirty

### Must Move to Bank 5:
1. **buildSpriteIfDirty()** - calls port_build functions (in bank 5)
2. All port_build functions (already planned)

### Can Stay in Fixed Bank:
- **clearObjectDirtyFlag()** - small utility (can move to bank 5 if convenient)

## Complete List for Bank 5

### From mainBankZero.c:
1. All object Init functions (13 functions)
2. All object Update functions (13 functions)
3. processObject()
4. buildSpriteIfDirty()
5. initSimpleDecorSprite()
6. updateSimpleDecorSprite()

### From nes.c:
1. All port_build* functions (13 functions)
2. port_updateCollapseTileNametable()
3. prepare_collapse_tiles_nametable()
4. execute_collapse_tiles_nametable_writes()
5. render_object_sprite() - if used by port_build functions
6. get_object_sprite_data() - if used by port_build functions

### Optional (small utilities - can move if convenient):
- clearObjectDirtyFlag() - small, but used by buildSpriteIfDirty

## Functions That MUST Stay in Fixed Bank

1. initObject() - dispatcher that switches to bank 5, then calls object functions
2. updateAllObjects() - calls processObject (which switches to bank 5)
3. All fix16_ math functions (used via FIXED_ macros)
4. playSoundEffect(), randint16(), sign() - small utilities (can stay)
5. approachFixed() - only used by playerUpdate (fixed bank)
