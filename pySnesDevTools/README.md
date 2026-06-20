# pySnesDevTools

Asset converters for SNES homebrew, used by the demos under `SimpleCDemos/`.
Pure Python with a single dependency — [Pillow](https://python-pillow.org/)
(`pip install -r requirements.txt`). No external native tools.

Each command turns source art into a C `.inc` fragment that a demo `#include`s
into one translation unit. Builds regenerate these at compile time, so only the
source art is committed, never the generated data.

## Usage

    python pySnesDevTools/snescli.py <command> <input> -o OUT.inc --symbol NAME

or, with the repo root on `sys.path`, `python -m pySnesDevTools <command> ...`.

### Commands

| Command  | Input            | Emits                                              |
|----------|------------------|----------------------------------------------------|
| `bg`     | PNG              | 4bpp tiles + tilemap + palette (deduped tiles)     |
| `sprite` | PNG              | 4bpp tiles in sheet order (no dedup) + palette     |
| `font`   | 2-colour PNG     | 1bpp 8x8 tiles                                     |
| `bin2c`  | any file         | one `unsigned char` array                          |
| `decode` | a `.h`/`.inc`    | reconstructs a source PNG from existing C arrays   |

`bg --scanline-palettes` emits a per-scanline 16-colour palette table
(`scanline_palettes[H][16]` + `#define SCANLINE_COUNT`) for an HDMA palette
effect; otherwise `bg` emits a single 16-colour palette.

Emitted symbol names follow each consumer's convention via `--symbol`:
`bg --symbol school` -> `school_bin` / `school_pal` / `school_tilemap`;
`sprite --symbol biker` -> `biker_pic` / `biker_clr`;
`bg --symbol bg --scanline-palettes` -> `bg_tiles` / `bg_tilemap` /
`scanline_palettes`.

### Examples

    python snescli.py bg bg_001.png -o build/assets/bg.inc --symbol bg --scanline-palettes
    python snescli.py sprite biker.png -o build/assets/biker.inc --symbol biker
    python snescli.py font snesfont.png -o build/assets/imagedata.inc --symbol SNESFONT_bin

`decode` is the one-time reverse step used to recover source PNGs for assets
whose original art was lost and survived only as committed C arrays:

    python snescli.py decode old.h --symbol school --mode bg --tiles-x 32 -o school.png

## Layout

- `snescli.py` — CLI entry / dispatch
- `snespal.py` — RGB888 <-> SNES BGR555
- `tiles.py` — pixels <-> planar 1/2/4 bpp tiles, dedup + tilemap
- `imageio.py` — PNG <-> indexed pixel buffers (Pillow)
- `bg.py` — Mode 1 4bpp background (single or per-scanline palettes)
- `convert.py` — sprite, font, bin2c
- `emit.py` — `.inc` writer
- `decode.py` — PNG reconstruction from C arrays
