#!/usr/bin/env python
from pathlib import Path
from PIL import Image

SCRIPT_DIR = Path(__file__).resolve().parent
SRC = SCRIPT_DIR / 'baseCelesteLevelData.png'
FONT_SRC = SCRIPT_DIR / 'pico8_font.bmp'
OUT = SCRIPT_DIR / 'title_screen_snes.h'
CHECKSUM_OUT = SCRIPT_DIR.parent / 'build' / 'title_screen_checksums.lua'

TITLE_PIXELS = 256
MAP_ENTRIES = 32
VISIBLE_MACROS = 16

TITLE_TEXT = [
    ('x+c', 58, 80),
    ('matt thorson', 42, 96),
    ('noel berry', 46, 102),
]

# Fixed 4-color title palette (mode 0 BGs are 2bpp on SNES).
# Chosen to preserve the Celeste logo look better than frequency-based quantization.
TITLE_FIXED_PALETTE = [
    (0, 0, 0),         # black
    (255, 241, 232),   # Pico-8 light text
    (131, 118, 156),   # Pico-8 lavender
    (41, 173, 255),    # Pico-8 blue
]

TITLE_TEXT_COLOR_INDEX = 2


def rgb_to_snes_bgr15(r, g, b):
    r5 = (r * 31 + 127) // 255
    g5 = (g * 31 + 127) // 255
    b5 = (b * 31 + 127) // 255
    return (b5 << 10) | (g5 << 5) | r5


def nearest_idx(rgb, palette):
    best_i = 0
    best_d = 1 << 30
    r, g, b = rgb
    for i, (pr, pg, pb) in enumerate(palette):
        d = (r - pr) * (r - pr) + (g - pg) * (g - pg) + (b - pb) * (b - pb)
        if d < best_d:
            best_d = d
            best_i = i
    return best_i


def tile8_to_snes_2bpp(tile_vals):
    out = bytearray()
    for row in tile_vals:
        p0 = 0
        p1 = 0
        for x, v in enumerate(row):
            bit = 7 - x
            p0 |= (v & 1) << bit
            p1 |= ((v >> 1) & 1) << bit
        out.append(p0)
        out.append(p1)
    return bytes(out)


def fmt_u8(name, vals, cols=16):
    lines = [f"static const unsigned char {name}[{len(vals)}] = {{"]
    for i in range(0, len(vals), cols):
        lines.append("    " + ", ".join(f"0x{v:02X}" for v in vals[i:i + cols]) + ",")
    lines.append("};")
    return "\n".join(lines)


def fmt_u16(name, vals, cols=8):
    lines = [f"static const unsigned short {name}[{len(vals)}] = {{"]
    for i in range(0, len(vals), cols):
        lines.append("    " + ", ".join(f"0x{v:04X}" for v in vals[i:i + cols]) + ",")
    lines.append("};")
    return "\n".join(lines)


def checksum_bytes(data):
    s1 = 0
    s2 = 0
    for b in data:
        s1 = (s1 + b) & 0xFFFFFFFF
        s2 = (s2 + s1) & 0xFFFFFFFF
    return s1, s2


def draw_overlay_text_mask(font_img):
    mask = [0] * (TITLE_PIXELS * TITLE_PIXELS)

    def set_px(x, y):
        if 0 <= x < TITLE_PIXELS and 0 <= y < TITLE_PIXELS:
            mask[y * TITLE_PIXELS + x] = 1

    for text, sx, sy in TITLE_TEXT:
        x = sx
        for ch in text:
            c = ord(ch) & 0x7F
            ox = (c % 16) * 8
            oy = (c // 16) * 8
            for gy in range(8):
                for gx in range(8):
                    if font_img.getpixel((ox + gx, oy + gy)) != 0:
                        px = x + gx
                        py = sy + gy
                        set_px(px * 2 + 0, py * 2 + 0)
                        set_px(px * 2 + 1, py * 2 + 0)
                        set_px(px * 2 + 0, py * 2 + 1)
                        set_px(px * 2 + 1, py * 2 + 1)
            x += 4

    return mask


def main():
    if not SRC.exists():
        raise SystemExit(f"missing source image: {SRC}")
    if not FONT_SRC.exists():
        raise SystemExit(f"missing Pico-8 font image: {FONT_SRC}")

    img = Image.open(SRC).convert('RGBA')
    font_img = Image.open(FONT_SRC).convert('P')
    w, h = img.size
    cw, ch = w // 8, h // 4

    # Title is chunk 31 (bottom-right) of base level data.
    chunk = img.crop((7 * cw, 3 * ch, 8 * cw, 4 * ch)).convert('RGB')
    chunk = chunk.resize((TITLE_PIXELS, TITLE_PIXELS), Image.NEAREST)

    # Use fixed palette instead of dynamic quantization to avoid muddy color picks.
    palette = list(TITLE_FIXED_PALETTE)

    pix = list(chunk.getdata())
    idx = [nearest_idx(p, palette) for p in pix]
    overlay_mask = draw_overlay_text_mask(font_img)
    for i, on in enumerate(overlay_mask):
        if on:
            idx[i] = TITLE_TEXT_COLOR_INDEX

    # Dedup 16x16 macro-tiles (matching gameplay BG3 16x16 mode).
    macro_map = [0] * (MAP_ENTRIES * MAP_ENTRIES)
    macro_to_id = {}
    macro_tiles = []

    for my in range(VISIBLE_MACROS):
        for mx in range(VISIBLE_MACROS):
            block = []
            for py in range(16):
                for px in range(16):
                    x = mx * 16 + px
                    y = my * 16 + py
                    block.append(idx[y * TITLE_PIXELS + x])
            key = tuple(block)
            mid = macro_to_id.get(key)
            if mid is None:
                mid = len(macro_tiles)
                if mid >= 256:
                    raise SystemExit('title screen exceeds 256 unique 16x16 macro-tiles')
                macro_to_id[key] = mid
                macro_tiles.append(key)
            macro_map[my * MAP_ENTRIES + mx] = mid

    # Pack macro-tiles into 8x8 tile memory layout expected by 16x16 BG mode:
    # top-left at n, top-right n+1, bottom-left n+16, bottom-right n+17.
    tile_slots = {}
    max_slot = 0
    for mid, key in enumerate(macro_tiles):
        mr = mid // 8
        mc = mid % 8
        n = mr * 32 + mc * 2
        if n + 17 > 1023:
            raise SystemExit('tile slot overflow in 16x16 packing')

        def sub_tile(offx, offy):
            rows = []
            for y in range(8):
                row = []
                for x in range(8):
                    row.append(key[(offy + y) * 16 + (offx + x)])
                rows.append(row)
            return tile8_to_snes_2bpp(rows)

        tile_slots[n] = sub_tile(0, 0)
        tile_slots[n + 1] = sub_tile(8, 0)
        tile_slots[n + 16] = sub_tile(0, 8)
        tile_slots[n + 17] = sub_tile(8, 8)
        max_slot = max(max_slot, n + 17)

    chr_bytes = bytearray((max_slot + 1) * 16)
    for slot, data in tile_slots.items():
        start = slot * 16
        chr_bytes[start:start + 16] = data

    # Build BG3 map (32x32); visible content in top-left 16x16 entries.
    tilemap32 = [0] * (MAP_ENTRIES * MAP_ENTRIES)
    for my in range(VISIBLE_MACROS):
        for mx in range(VISIBLE_MACROS):
            mid = macro_map[my * MAP_ENTRIES + mx]
            mr = mid // 8
            mc = mid % 8
            n = mr * 32 + mc * 2
            tilemap32[my * MAP_ENTRIES + mx] = n

    pal = [0] * 16
    for i in range(4):
        r, g, b = palette[i]
        pal[i] = rgb_to_snes_bgr15(r, g, b)

    out = []
    out.append('// Auto-generated by convert_title_screen_snes.py from baseCelesteLevelData.png chunk 31')
    out.append('// Includes Pico-8 title text baked from pico8_font.bmp')
    out.append('#ifndef TITLE_SCREEN_SNES_H')
    out.append('#define TITLE_SCREEN_SNES_H')
    out.append('')
    out.append(fmt_u8('title_tiledata_snes_2bpp', list(chr_bytes)))
    out.append('')
    out.append(fmt_u16('title_tilemap_bg3_snes', tilemap32))
    out.append('')
    out.append(fmt_u16('title_palette_snes_bgr15', pal))
    out.append('')
    out.append('#endif // TITLE_SCREEN_SNES_H')
    out.append('')
    OUT.write_text('\n'.join(out), encoding='utf-8')

    map_bytes = bytearray()
    for v in tilemap32:
        map_bytes.append(v & 0xFF)
        map_bytes.append((v >> 8) & 0xFF)

    tile_s1, tile_s2 = checksum_bytes(chr_bytes)
    map_s1, map_s2 = checksum_bytes(map_bytes)

    CHECKSUM_OUT.parent.mkdir(parents=True, exist_ok=True)
    CHECKSUM_OUT.write_text(
        '\n'.join([
            '-- Auto-generated by convert_title_screen_snes.py',
            'return {',
            f'    tile_s1 = {tile_s1},',
            f'    tile_s2 = {tile_s2},',
            f'    tile_bytes = {len(chr_bytes)},',
            f'    map_s1 = {map_s1},',
            f'    map_s2 = {map_s2},',
            '}','',
        ]),
        encoding='utf-8'
    )
    print(f'Wrote {OUT} (unique_macro_tiles={len(macro_tiles)}, tile_bytes={len(chr_bytes)})')
    print(f'Wrote {CHECKSUM_OUT}')


if __name__ == '__main__':
    main()
