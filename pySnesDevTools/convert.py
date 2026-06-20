"""Sprite, font and raw-binary converters (the non-BG paths)."""

from . import emit, imageio, snespal, tiles


def _palette_bytes(palette_rgb, count=16):
    pal = list(palette_rgb[:count])
    pal += [(0, 0, 0)] * (count - len(pal))
    return snespal.palette_to_bytes(snespal.rgb_to_snes(*c) for c in pal)


def convert_sprite(image_path, symbol):
    """4bpp sprite tiles in sheet order (no dedup) plus a 16-colour palette."""
    width, height, indices, palette = imageio.load_indexed(image_path, 16)
    tile_bytes, _ = tiles.tiles_from_indices(indices, width, height, 4, dedup=False)
    return [
        emit.u8(symbol + "_pic", tile_bytes),
        emit.u8(symbol + "_clr", _palette_bytes(palette)),
    ], None


def convert_font(image_path, symbol):
    """1bpp 8x8 tiles in sheet order from a 2-colour image."""
    width, height, indices, _ = imageio.load_indexed(image_path, 2)
    tile_bytes, _ = tiles.tiles_from_indices(indices, width, height, 1, dedup=False)
    return [emit.u8(symbol, tile_bytes)], None


def convert_bin2c(bin_path, symbol):
    with open(bin_path, "rb") as f:
        data = list(f.read())
    return [emit.u8(symbol, data)], None
