"""Tiled background converter (deduped tiles + tilemap + palette).

  - single():   one palette of 2**bpp colours.
  - scanline(): a distinct 16-colour palette per scanline, swapped by an
                HDMA/IRQ handler at runtime (the Mode1HDMA effect, 4bpp).

Backgrounds larger than one 32x32 tilemap screen are split into SNES screen
blocks by tiles.arrange_screens (up to 64x64 tiles / 512x512 px).
"""

import math

from PIL import Image

from . import emit, imageio, snespal, tiles

SCREEN_W = 256
SCREEN_H = 224


def _check_tile_count(tile_bytes, bpp):
    count = len(tile_bytes) // (bpp * 8)
    if count > 1024:
        raise ValueError("%d unique tiles; tilemap entries hold at most 1024"
                         % count)


def single(image_path, symbol, bpp=4):
    """One palette, deduped tiles + tilemap. Emits <symbol>_chr/_pal/_map."""
    ncolors = 2 ** bpp
    width, height, indices, palette = imageio.load_indexed(image_path, ncolors)
    tile_bytes, tilemap = tiles.tiles_from_indices(
        indices, width, height, bpp, dedup=True)
    _check_tile_count(tile_bytes, bpp)
    tilemap = tiles.arrange_screens(tilemap, width // 8, height // 8)
    pal = list(palette[:ncolors]) + [(0, 0, 0)] * (ncolors - len(palette[:ncolors]))
    pal_bytes = snespal.palette_to_bytes(snespal.rgb_to_snes(*c) for c in pal)
    tm_bytes = []
    for e in tilemap:
        tm_bytes.append(e & 0xFF)
        tm_bytes.append((e >> 8) & 0xFF)
    return [
        emit.u8(symbol + "_chr", tile_bytes),
        emit.u8(symbol + "_pal", pal_bytes),
        emit.u8(symbol + "_map", tm_bytes),
    ], None


def columns(image_path, symbol, bpp=4):
    """Deduped tiles + a COLUMN-MAJOR tilemap, for a map streamed while scrolling.

    For a level wider than the 64-tile SNES tilemap the whole map stays in ROM
    and is streamed one column at a time into VRAM as the camera scrolls. The
    tilemap is emitted column-major (map[gx * rows + ry]) so a single VRAM
    column is a contiguous DMA source. Emits <symbol>_chr/_pal/_map; the column
    height (rows) and column count (cols) come from the matching collision()
    output's <symbol>_ROWS / <symbol>_COLS.
    """
    ncolors = 2 ** bpp
    width, height, indices, palette = imageio.load_indexed(image_path, ncolors)
    tile_bytes, tilemap = tiles.tiles_from_indices(
        indices, width, height, bpp, dedup=True)
    _check_tile_count(tile_bytes, bpp)
    cols, rows = width // 8, height // 8
    colmajor = [tilemap[ry * cols + gx] for gx in range(cols) for ry in range(rows)]
    pal = list(palette[:ncolors]) + [(0, 0, 0)] * (ncolors - len(palette[:ncolors]))
    pal_bytes = snespal.palette_to_bytes(snespal.rgb_to_snes(*c) for c in pal)
    return [
        emit.u8(symbol + "_chr", tile_bytes),
        emit.u8(symbol + "_pal", pal_bytes),
        emit.u16(symbol + "_map", colmajor),
    ], None


def _fit_image(image_path):
    img = Image.open(image_path).convert('RGB')
    if img.width != SCREEN_W:
        new_h = int(SCREEN_W * img.height / img.width)
        img = img.resize((SCREEN_W, new_h), Image.LANCZOS)
    if img.height < SCREEN_H:
        padded = Image.new('RGB', (SCREEN_W, SCREEN_H), (0, 0, 0))
        padded.paste(img, (0, 0))
        img = padded
    elif img.height > SCREEN_H:
        img = img.crop((0, 0, SCREEN_W, SCREEN_H))
    if img.getcolors(maxcolors=512) is None:
        img = img.quantize(colors=256).convert('RGB')
    return img


def _quantize_scanline(counts):
    """Pick 16 colours from a scanline by diversity + frequency."""
    ordered = sorted(counts.items(), key=lambda kv: kv[1], reverse=True)
    top = ordered[0][1]
    selected = [ordered[0][0]]
    remaining = [c for c, _ in ordered[1:]]
    while len(selected) < 16 and remaining:
        best = None
        best_score = -1.0
        for cand in remaining:
            min_d = min(sum((a - b) ** 2 for a, b in zip(cand, s))
                        for s in selected)
            diversity = math.sqrt(min_d) / 441.67
            score = diversity * 0.7 + (counts[cand] / top) * 0.3
            if score > best_score:
                best_score = score
                best = cand
        selected.append(best)
        remaining.remove(best)
    selected += [(0, 0, 0)] * (16 - len(selected))
    return selected


def _scanline_palettes(pixels):
    palettes = []
    for y in range(SCREEN_H):
        counts = {}
        for x in range(SCREEN_W):
            c = pixels[y * SCREEN_W + x]
            counts[c] = counts.get(c, 0) + 1
        if len(counts) <= 16:
            pal = list(counts.keys())
            pal += [(0, 0, 0)] * (16 - len(pal))
        else:
            pal = _quantize_scanline(counts)
        palettes.append(pal)
    return palettes


def scanline(image_path, symbol):
    """Per-scanline 16-colour palettes (HDMA effect), 4bpp deduped tiles.
    Emits <symbol>_chr, <symbol>_map, <symbol>_pal[H][16] + SCANLINE_COUNT."""
    img = _fit_image(image_path)
    pixels = list(img.getdata())
    palettes = _scanline_palettes(pixels)

    indices = [0] * (SCREEN_W * SCREEN_H)
    for y in range(SCREEN_H):
        pal = palettes[y]
        for x in range(SCREEN_W):
            rgb = pixels[y * SCREEN_W + x]
            best_i = 0
            best_d = None
            for i, pc in enumerate(pal):
                d = sum((a - b) ** 2 for a, b in zip(rgb, pc))
                if best_d is None or d < best_d:
                    best_d = d
                    best_i = i
            indices[y * SCREEN_W + x] = best_i

    tile_bytes, tilemap = tiles.tiles_from_indices(
        indices, SCREEN_W, SCREEN_H, 4, dedup=True)
    _check_tile_count(tile_bytes, 4)
    tilemap = tiles.arrange_screens(tilemap, SCREEN_W // 8, SCREEN_H // 8)
    rows = [[snespal.rgb_to_snes(*c) for c in pal] for pal in palettes]
    arrays = [
        emit.u16_2d(symbol + "_pal", rows),
        emit.u8(symbol + "_chr", tile_bytes),
        emit.u16(symbol + "_map", tilemap),
    ]
    return arrays, {"SCANLINE_COUNT": SCREEN_H}
