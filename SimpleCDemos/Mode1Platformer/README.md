# Mode1Platformer

A re-creation of the pvsneslib `objects/mapandobjects` example — the full Super
Mario Bros World 1-1 layout, scrolled past a pad-controlled player and several
autonomous enemies, with tile collision — rebuilt in this collection's style.
Where pvsneslib hides the map engine, the dynamic-sprite streamer and the object
engine inside its library, this demo implements only what it needs, directly and
readably. `mainBankZero.c` is a thin entry point; the whole game lives in
`mapengine.c` (see *Building* for why it is its own translation unit). It borrows
the reference's level *layout* and re-skins it with CC0 art.

## What it does

- The level is **422×30 tiles** (3376×240 px) — far wider than the 64-tile SNES
  tilemap, so it cannot live in VRAM whole. The 64×32 VRAM tilemap holds a
  sliding window of the map, and the engine **streams one column at a time**
  into VRAM as the camera scrolls (`streamCol` / `updateCamera`), DMA'd in the
  vblank window via the new `LoadVramColumn` helper. The map is 30 tiles tall —
  it fits the VRAM screen vertically — so only X streams; a fixed `REG_BG1VOFS`
  bottom-aligns the ground.
- A **player** with **faithful Super Mario Bros 1 physics**, modelled the way the
  original does it — an integer-pixel position plus a 1/256-px subpixel fraction,
  with velocities and accelerations in those subpixel units. Horizontal motion
  has momentum: a small acceleration toward a speed ceiling (walk ≈ 1.56 px/f),
  **hold B to run** (ceiling ≈ 2.56 px/f and a quicker acceleration), a harder
  *skid* deceleration when you press against your motion to turn around, and
  ground friction when you let go (in the air your momentum is kept). **A** is the
  SMB **variable jump**: a fixed takeoff velocity with *dual gravity* — weak while
  A is held and you are rising (the float), strong once you release or start
  falling — so a tap is a ~22 px hop and a full hold is a ~74 px jump (a running
  takeoff launches harder, ~82 px). A few frames of coyote-time grace soften
  leaving a ledge. Always-on gravity with solid-tile collision means walking off a
  ledge falls immediately. Stand / walk / jump are three OBJ frames with H-flip
  for facing; falling into a pit respawns you at the start.
- Several **autonomous enemies** (seeded from the `level.tmx` object layer) that
  pace the ground and turn at a wall (their move was blocked) or a ledge (no
  ground one pixel ahead of the leading foot), sharing the player's
  movement/collision code — no generic object engine. **Stomp** one from above to
  squash it and bounce (a rising "+100" pops and the score ticks up); touching one
  from the side respawns the player.
- **Interactive blocks.** Bumping a **? block** from below pops a coin, awards
  **200** points and a coin, and turns the block into a spent "used" block that
  cannot be hit again; **brick blocks** nudge when bumped (bump-only, no break).
  Because the level tilemap is `const`/ROM and column-streamed (a runtime tile
  swap would revert when a column re-streams), these reactions are drawn as **OBJ
  sprite overlays** over the BG tile, not by mutating the tilemap. The build
  emits the ? / brick cell lists to `blockcells.inc`; the engine recognises a
  bumped cell from those.
- A fixed **score / coin HUD** at the top of the screen on **BG3** (2bpp), drawn
  on top via the Mode-1 BG3-priority bit and held at scroll 0 so it does not move
  with the level. Labels are written once; the digits update by DMA into the BG3
  tilemap when the score or coin count changes. The font is the shared
  `snesfont` loaded as 2bpp (glyph N = tile N, so an ASCII byte indexes its glyph).
- Sprite animation loads **all frames into VRAM once** and switches the OAM
  CHARNUM per frame (no per-frame tile streaming); the player is OBJ 0, enemies
  are OBJ 1.., then the block/coin/score overlays — all with off-screen culling.

## Assets

The level is built from **`level.tmx`** as its single source of truth — a Tiled
map of **16×16 CC0 metatiles** over the committed `level_tiles.png` tileset, where
each tile carries a re-skin **`category`** property (`GROUND_TOP`, `QBLOCK`,
`PIPE_TOP`, …). `level_tiles.png` is the actual Kenney "Pixel Platformer" (CC0) art
the ROM renders, so `level.tmx` opens cleanly in Tiled and shows the real level. At
build time `convert.py` (via `pySnesDevTools`, see `../../pySnesDevTools/`) reads it
with `pySnesDevTools.load_tmx()`, and `genart.py` paints each metatile with the
Kenney tile (from `kenney_tilemap_packed.png`) and emits the C data:

- **layout** (`level.inc`) — each 16×16 metatile's category is painted to an
  intermediate `build/assets/bg.png`, then `background(..., layout="columns")`
  dedupes it to an 8×8 tileset plus a **column-major** tilemap (so a VRAM column is
  a contiguous DMA source), emitted unsplit rather than as fixed screen blocks.
  Decorative tiles use **position-aware connecting pieces** so multi-tile features
  tile seamlessly. Kenney's green-platform blocks each carry a full per-tile border
  (they never join), so bushes/hills are painted from the **seamless cloud set
  tinted to the platform greens**: rounded puffs-up tops (`G_TL/G_T/G_TR`, lone bush
  `G_S`) over a solid green fill body (`G_L/G_C/G_R`). The flagpole is a trunk
  (`POLE_TOP/BODY/BASE`) and clouds are `CLOUD_L/M/R`. The bootstrap auto-tiles these
  from the layout (see `_refine_decorations` / `cat_image`) and drops floating
  conversion artifacts, so the pieces are explicit in `level.tmx` and editable in
  Tiled.
- **collision** (`levelcol.inc`) — a black/white mask (`build/assets/bg_col.png`)
  is built from the metatile categories (solid = ground / brick / ? block / pipe;
  the rounded hills stay passable background); `collision()` turns each 8×8 cell
  into one byte of `level_col` (white = solid) and defines `level_COLS`/`level_ROWS`.
- **blockcells.inc** — the 8-px-cell coordinates of every ? block and brick
  metatile, so the engine can tell which bumped cell is interactive (the
  collision table only says solid/empty).
- **spawns.inc** — `NENEMY`, the player start (`PLAYER_START_X/Y`) and the
  `enemy_spawns[]` table, read from the TMX object layer ("Entities": Mario =
  player, Goomba / koopatroopa = enemies). The engine seeds the player and
  enemies from these instead of hardcoded positions.
- `player.png` (stand / walk / jump) / `walker.png` (2 enemy frames) /
  `blocks.png` (used-block, brick, coin, "100", "200" overlays) — committed
  horizontal sprite strips (not TMX-derived). `sprite_sheet()` lays each 16×16
  frame out for the OBJ char grid so frame *k* is reachable by
  `CHARNUM = k * <symbol>_STEP`; it also emits `<symbol>_TILES` so the C side
  knows where the next sprite block begins.
- `hudfont.inc` — the shared `../shared/assets/snesfont.png` run through
  `graphics(..., bpp=2, palette=False)`: tiles in raster (ASCII) order, no
  tilemap, for the BG3 HUD.

`genart.py` is the build-time level builder (its paint / collision / blockcells /
spawns functions are what `convert.py` calls). It also has two one-time helpers that
the build never runs: `python genart.py bootstrap` (re)generates `level.tmx` +
`level_tiles.png` by migrating the original pvsneslib Mario 8×8-gid map into the CC0
metatile form (needs the Kenney tile sheet); and `python genart.py` re-authors the
sprite strips. The sprite strips are authored once (not part of the build, needs the
full Kenney pack via `KENNEY_PP`): it slices the enemy frames
and the player's stand/walk from the Kenney character sheet (the green character
has no jump pose, so the third player frame is synthesized by lifting the stand
pose a couple of pixels) and builds the `blocks.png` overlay strip (baking the
"100"/"200" popups from the font glyphs).

### Art credit / licence

Graphics are from Kenney's **Pixel Platformer** pack
([www.kenney.nl](https://kenney.nl/assets/pixel-platformer)), released under
**CC0 1.0** (public domain); the committed `kenney_tilemap_packed.png` and the
`level_tiles.png` tileset are from that pack. The level *layout, collision and object
placement* come from `level.tmx`, a CC0 16×16-metatile Tiled map (editable in Tiled)
migrated from the pvsneslib `objects/mapandobjects` reference layout.

## Building

From this directory (see ../README.md for one-time toolchain setup):

    make COMPILER=calypsi      # or vbcc65816, or tcc816
    make clean

Output ROM: `build/mainBankZero_<compiler>.smc` (`.sfc` for tcc816).

The game logic lives in its own translation unit, `mapengine.c`, because
**vbcc's `-O4` miscompiles its fixed-point/streaming math** (the camera scroll
computed to 0) and its unused-object elimination strips the engine's entry
points from a separately-compiled object. The Makefile builds just that file at
`-O3` for vbcc (keeping the perf flags `-unroll-all`/`-inline-depth`/
`-fp-associative`, so it's ≈`-O4` speed) and links its object; everything else,
including `mainBankZero.c`, stays at the shared `-O4`. calypsi and other
toolchains compile `mapengine.c` normally.

## Status

Runs on **calypsi, vbcc65816 and tcc816** (the streamed full-1-1 level is ~3 ROM
banks; the camera scrolls the whole level, the player walks/jumps with momentum
and is stopped by pipes/bricks/the staircase, the enemies pace and turn at walls
and pits, stomping/side-contact work, ? blocks pop a coin and become used, bricks
nudge, and the BG3 score/coin HUD updates). **tcc816** was previously excluded
because its `.rodata` landed in a single fixed ROM bank that the ~40 KB tilemap +
tiles overflowed; it now builds since tcc816 **auto-banks `.rodata`** across the
ROM (`mapengine.c` compiles through the standard path, no `-O3` workaround).
**cc65** and **llvm-mos** remain excluded — they still place `.rodata` in one
fixed bank, which the tilemap overflows (a linker error).

### Compiler gotchas worth knowing

Position is stored as an **integer pixel coordinate plus a separate 1/256-px
subpixel fraction** (the SMB scheme), and each frame is integrated by
accumulating the subpixel velocity and carrying whole pixels across — pure
`int16` add / compare / increment. This is not just for SMB faithfulness; it is
also what keeps the code off two compiler landmines this collection has repeatedly
hit. The earlier design used 12.4 fixed point and a `toPix` divide, which forced
two bad choices:

- A 12.4 `int16` position **overflows** once the player passes the middle of the
  3376-px level (`3375 × 16 > 32767`), garbling the map and sticking the player at
  the right edge. Dropping to 13.3 (`FP = 3`) avoided the overflow but halved the
  sub-pixel resolution — too coarse for SMB's ~0.04 px/f² accelerations.
- Widening the fixed-point position to **32-bit** is *worse*: vbcc miscompiles the
  32-bit divide in `toPix` (the camera scroll computes to 0, freezing the level).

The pixel + subpixel split has neither problem: the pixel coordinate is a plain
`int16` that spans the level, the subpixel carry needs no division or shift, and
the fraction gives full SMB-grade resolution. The one remaining conversion,
`toCell` (pixel → 8-px tile cell), still uses an **unsigned division** rather than
a shift, because calypsi (65816 5.12) miscompiles a **signed `int16_t >> const` of
a positive value** (`640 >> 4` → `-8`) and vbcc treats the **unsigned `>>`** as an
8-bit shift of the low byte (`0x1B00 >> 4` → `0`); unsigned division is correct on
both. See also the `-O3` note under *Building* for the broader vbcc `-O4` issue
that motivated isolating `mapengine.c`.

The HUD digit formatter (`put5`/`put2`) extracts decimal digits by **repeated
subtraction of place values + a ROM digit table**, deliberately avoiding `/` and
`%`: calypsi 5.12 **miscompiles 16-bit divide/modulo** here (a `'0' + v % 10`
formatter rendered garbage — every digit cell read as tile 0). Subtraction and a
table are correct on both compilers. Relatedly, the digit buffer DMA'd into the
HUD must be a **file-scope global, not a stack local**: a stack-local source hung
calypsi in `engineInit` (the same way the OAM mirror `oamCopy` is a global).
