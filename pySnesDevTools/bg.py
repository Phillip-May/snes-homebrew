"""Mode 1 4bpp background converter.

Two modes:
  - single palette: one 16-colour palette, deduped tiles + tilemap (e.g. a
    static 4bpp background).
  - scanline palettes: a distinct 16-colour palette per scanline, swapped by
    an HDMA/IRQ handler at runtime (the Mode1HDMA effect).
"""

import math

from PIL import Image

from . import emit, imageio, snespal, tiles

SCREEN_W = 256
SCREEN_H = 224


def convert_single(image_path, symbol):
    width, height, indices, palette = imageio.load_indexed(image_path, 16)
    tile_bytes, tilemap = tiles.tiles_from_indices(
        indices, width, height, 4, dedup=True)
    pal = list(palette[:16]) + [(0, 0, 0)] * (16 - len(palette[:16]))
    pal_bytes = snespal.palette_to_bytes(snespal.rgb_to_snes(*c) for c in pal)
    tm_bytes = []
    for e in tilemap:
        tm_bytes.append(e & 0xFF)
        tm_bytes.append((e >> 8) & 0xFF)
    return [
        emit.u8(symbol + "_bin", tile_bytes),
        emit.u8(symbol + "_pal", pal_bytes),
        emit.u8(symbol + "_tilemap", tm_bytes),
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


def convert_scanline(image_path, symbol):
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
    rows = [[snespal.rgb_to_snes(*c) for c in pal] for pal in palettes]
    arrays = [
        emit.u16_2d("scanline_palettes", rows),
        emit.u8(symbol + "_tiles", tile_bytes),
        emit.u16(symbol + "_tilemap", tilemap),
    ]
    return arrays, {"SCANLINE_COUNT": SCREEN_H, "COLORS_PER_SCANLINE": 16}
