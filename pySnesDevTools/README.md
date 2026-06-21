# pySnesDevTools

Asset converters for SNES homebrew, used by the demos under `SimpleCDemos/`.
Pure Python with a single dependency — [Pillow](https://python-pillow.org/)
(`pip install -r requirements.txt`). No external native tools.

Each converter turns source art into a C `.inc` fragment that a demo `#include`s
into one translation unit. Builds regenerate these at compile time, so only the
source art is committed, never the generated data.

## Library API (how demos use it)

Each demo owns a small `convert.py` that maps its source art to outputs by
calling the library directly. The image converters: `graphics` emits tile
(character) data in raster order, `sprite` orders tiles for a single hardware
OBJ of a given size, `sprite_sheet` does the same for an N-frame animation
strip, and `background` lays the image out as a tilemap. `collision` turns a
mask PNG into a per-tile solid table. The image converters take `bpp`; a font
is `graphics(bpp=1, palette=False)`:

    import pySnesDevTools as snes

    # one hardware OBJ sprite (tiles ordered for the OBJ char grid)
    snes.sprite("biker.png", "build/assets/biker.inc", symbol="biker",
                size="32x32", bpp=4)

    # an OBJ animation strip: frame k is at CHARNUM k * <symbol>_STEP
    snes.sprite_sheet("player.png", "build/assets/player.inc", symbol="player",
                      frame="16x16", bpp=4)

    # tile graphics in raster order, no tilemap (fonts, CHR sheets)
    snes.graphics("snesfont.png", "build/assets/font.inc", symbol="snesfont",
                  bpp=1, palette=False)

    # a background layer: tiles + tilemap + palette
    snes.background("school.png", "build/assets/school.inc", symbol="school", bpp=4)
    snes.background("bg_001.png", "build/assets/bg.inc", symbol="bg", bpp=4,
                    palette="per_scanline")   # HDMA per-scanline palette table

    # a per-8x8-cell solid table for tile collision (white = solid)
    snes.collision("bg_col.png", "build/assets/levelcol.inc", symbol="level")

Functions — each writes `output` and returns its path:

- `graphics(image, output, symbol, bpp=4, palette=True)` — tiles in sheet order
  (**no tilemap, no dedup**). Emits `<symbol>_chr`, and `<symbol>_pal` unless
  `palette=False`. Use for fonts and raw CHR sheets, **not** large OBJs.
- `sprite(image, output, symbol, size="32x32", bpp=4, palette=True)` — one
  hardware OBJ sprite. `size` is a SNES OBJ size string (`"8x8"`, `"16x16"`,
  `"32x32"`, `"64x64"`, `"16x32"`, `"32x64"`) and the image must be exactly that
  many pixels. Tiles are emitted in OBJ order with each tile-row padded to the
  16-tile char stride (the OBJ name table is a 16-wide grid), so a clean square
  PNG displays correctly at a 16-aligned char base. Emits `<symbol>_chr`
  `[+ <symbol>_pal]`.
- `sprite_sheet(image, output, symbol, frame="16x16", frames=None, bpp=4,
  palette=True)` — an OBJ **animation strip**: a horizontal row of `frame`-sized
  frames sharing one VRAM block and palette. Every frame is laid out for the
  16-tile-wide OBJ char grid, so frame *k* is addressable on its own by OAM
  CHARNUM `k * <symbol>_STEP`. `frames` defaults to the count that fits the
  sheet width. Emits `<symbol>_chr` `[+ <symbol>_pal]` and the defines
  `<symbol>_FRAMES`, `<symbol>_STEP` and `<symbol>_TILES` (the CHARNUM base a
  following block must start at). Frames must fit one 16-tile char row.
- `background(image, output, symbol, bpp=4, palette="single")` — deduped tiles
  **+ tilemap** + palette. Emits `<symbol>_chr`, `<symbol>_map`, `<symbol>_pal`.
  `palette` is `"single"` (one `2**bpp`-colour palette) or `"per_scanline"`
  (a 16-colour palette per scanline, 4bpp only; also defines `SCANLINE_COUNT`).
  Images larger than one 32×32 tilemap screen are split into SNES screen blocks
  automatically, up to 64×64 tiles (512×512 px); larger raises.
- `collision(image, output, symbol, threshold=128)` — a per-8×8-cell **solid
  table** from a black/white mask PNG (same pixel size as the background). A
  cell is `1` when any pixel in it is at least `threshold` average brightness
  (white = solid), else `0`. Emits `<symbol>_col` (row-major, indexed
  `col[cy * <symbol>_COLS + cx]`) and defines `<symbol>_COLS` / `<symbol>_ROWS`.
- `raw_binary(binary, output, symbol)` — raw bytes → a bare `<symbol>` array.
- `decode(header, output, symbol, mode, bpp=4, tiles_x=32, columns=8)` — reverse
  step; `mode` is `"background"` or `"graphics"`.

A demo's `Makefile` runs `convert.py` (`$(PYTHON) convert.py`) as a build
prerequisite; see `SimpleCDemos/Mode1Sprite` for the pattern.

## Command line

The same converters are available as a CLI for ad-hoc use:

    python pySnesDevTools/snescli.py <command> <input> -o OUT.inc --symbol NAME

or, with the repo root on `sys.path`, `python -m pySnesDevTools <command> ...`.

| Command        | Key options                     | Emits                                            |
|----------------|---------------------------------|--------------------------------------------------|
| `graphics`     | `--bpp N`, `--no-palette`       | tiles (no tilemap) + optional palette            |
| `sprite`       | `--size WxH`, `--bpp N`, `--no-palette` | one OBJ sprite, tiles ordered for the OBJ grid |
| `sprite-sheet` | `--frame WxH`, `--frames N`, `--bpp N`, `--no-palette` | N OBJ frames + `_FRAMES/_STEP/_TILES` |
| `background`   | `--bpp N`, `--palette MODE`     | deduped tiles + tilemap + palette                |
| `collision`    | `--threshold N`                 | per-cell solid table + `_COLS/_ROWS`             |
| `bin2c`        | —                               | one `unsigned char` array                        |
| `decode`       | `--mode background/graphics`    | reconstructs a source PNG from existing C arrays |

### Examples

    python snescli.py sprite biker.png -o biker.inc --symbol biker --size 32x32 --bpp 4
    python snescli.py graphics snesfont.png -o font.inc --symbol snesfont --bpp 1 --no-palette
    python snescli.py background bg_001.png -o bg.inc --symbol bg --palette per_scanline

`decode` is the one-time reverse step used to recover source PNGs for assets
whose original art survived only as committed C arrays:

    python snescli.py decode old.h --symbol school --mode background --tiles-x 32 -o school.png

## Layout

- `snescli.py` — CLI entry / dispatch over the library API
- `snespal.py` — RGB888 <-> SNES BGR555
- `tiles.py` — pixels <-> planar 1/2/4 bpp tiles; dedup, tilemap, screen split
- `imageio.py` — PNG <-> indexed pixel buffers (Pillow)
- `bg.py` — background (single or per-scanline palettes)
- `convert.py` — graphics (raster tiles), sprite / sprite_sheet (OBJ-ordered),
  collision (per-cell solid table) and raw-binary converters
- `emit.py` — `.inc` writer
- `decode.py` — PNG reconstruction from C arrays
