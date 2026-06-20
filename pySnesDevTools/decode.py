"""Reconstruct a source PNG from already-generated C arrays.

A one-time reverse step for assets whose original art was lost and survives
only as committed C data. The re-encoded output is self-consistent with the
forward converters, so the demo renders identically.
"""

import re

from . import imageio, snespal, tiles


def parse_array(text, name):
    """Return the integer initialisers of `name`'s C array (any base)."""
    m = re.search(re.escape(name) + r"\s*\[[^\]]*\]\s*=\s*\{(.*?)\}",
                  text, re.DOTALL)
    if not m:
        raise KeyError("array %r not found" % name)
    return [int(tok, 0) for tok in re.findall(r"0x[0-9A-Fa-f]+|\d+", m.group(1))]


def parse_tilemap(text, name):
    """Read a tilemap as 16-bit entries, whether declared u8 (LE pairs) or u16."""
    vals = parse_array(text, name)
    if re.search(r"\bshort\b[^;]*\b" + re.escape(name), text):
        return vals
    return [vals[i] | (vals[i + 1] << 8) for i in range(0, len(vals), 2)]


def _palette_rgb(pal_bytes):
    return [snespal.snes_to_rgb(c) for c in snespal.bytes_to_palette(pal_bytes)]


def _tile_pixels(tile_bytes, n, bpp):
    step = bpp * 8
    return [tiles.tile_to_pixels(tile_bytes[i * step:(i + 1) * step], bpp)
            for i in range(n)]


def decode_bg(text, symbol, tiles_x, out_path):
    tile_bytes = parse_array(text, symbol + "_bin")
    pal = _palette_rgb(parse_array(text, symbol + "_pal"))
    entries = parse_tilemap(text, symbol + "_tilemap")
    ntiles = len(tile_bytes) // 32
    glyphs = _tile_pixels(tile_bytes, ntiles, 4)
    tiles_y = len(entries) // tiles_x
    width, height = tiles_x * 8, tiles_y * 8
    indices = [0] * (width * height)
    for cell, entry in enumerate(entries):
        num = entry & 0x3FF
        px = glyphs[num]
        if entry & 0x4000:
            px = tiles.hflip(px)
        if entry & 0x8000:
            px = tiles.vflip(px)
        tx, ty = cell % tiles_x, cell // tiles_x
        for y in range(8):
            for x in range(8):
                indices[(ty * 8 + y) * width + tx * 8 + x] = px[y * 8 + x]
    imageio.save_indexed(out_path, width, height, indices, pal)


def decode_sheet(text, pic_name, clr_name, bpp, columns, out_path):
    tile_bytes = parse_array(text, pic_name)
    pal = _palette_rgb(parse_array(text, clr_name)) if clr_name else \
        [(0, 0, 0), (255, 255, 255)]
    ntiles = len(tile_bytes) // (bpp * 8)
    glyphs = _tile_pixels(tile_bytes, ntiles, bpp)
    rows = (ntiles + columns - 1) // columns
    width, height = columns * 8, rows * 8
    indices = [0] * (width * height)
    for i, px in enumerate(glyphs):
        tx, ty = i % columns, i // columns
        for y in range(8):
            for x in range(8):
                indices[(ty * 8 + y) * width + tx * 8 + x] = px[y * 8 + x]
    imageio.save_indexed(out_path, width, height, indices, pal)
