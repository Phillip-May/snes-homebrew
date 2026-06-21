"""Tile graphics (no tilemap) and raw-binary converters."""

from . import emit, imageio, snespal, tiles


def _palette_bytes(palette_rgb, count):
    pal = list(palette_rgb[:count])
    pal += [(0, 0, 0)] * (count - len(pal))
    return snespal.palette_to_bytes(snespal.rgb_to_snes(*c) for c in pal)


def graphics(image_path, symbol, bpp=4, palette=True):
    """Tiles in sheet order (no tilemap, no dedup). Emits <symbol>_chr, and
    <symbol>_pal when palette is requested. A font is bpp=1 with palette=False;
    for a hardware OBJ use sprite(), which orders tiles for the OBJ char grid."""
    ncolors = 2 ** bpp
    width, height, indices, pal = imageio.load_indexed(image_path, ncolors)
    tile_bytes, _ = tiles.tiles_from_indices(indices, width, height, bpp,
                                             dedup=False)
    arrays = [emit.u8(symbol + "_chr", tile_bytes)]
    if palette:
        arrays.append(emit.u8(symbol + "_pal", _palette_bytes(pal, ncolors)))
    return arrays, None


# SNES OBJ sizes -> (tiles_wide, tiles_tall). The OBJ name table is always a
# 16-tile-wide grid, so a large sprite's tile-rows are 16 char numbers apart.
OBJ_SIZES = {
    "8x8": (1, 1), "16x16": (2, 2), "32x32": (4, 4), "64x64": (8, 8),
    "16x32": (2, 4), "32x64": (4, 8),
}
OBJ_GRID_W = 16


def sprite(image_path, symbol, size="32x32", bpp=4, palette=True):
    """One hardware OBJ sprite, laid out for the 16-tile-wide OBJ char grid.

    size is a SNES OBJ size: "8x8", "16x16", "32x32", "64x64", "16x32" or
    "32x64". A source that is not already that many pixels is auto-resized to it
    with nearest-neighbour (so the same art can drive several OBJ sizes). Tiles
    are emitted in OBJ order with each tile-row padded to the 16-tile char
    stride, so a clean square PNG displays correctly at any 16-aligned char
    base. Emits <symbol>_chr, and <symbol>_pal when palette is requested."""
    if size not in OBJ_SIZES:
        raise ValueError("size must be one of %s"
                         % ", ".join(sorted(OBJ_SIZES)))
    tw, th = OBJ_SIZES[size]
    ncolors = 2 ** bpp
    width, height, indices, pal = imageio.load_indexed(
        image_path, ncolors, resize=(tw * 8, th * 8))
    blank = [0] * (bpp * 8)
    tile_bytes = []
    for ty in range(th):
        for tx in range(OBJ_GRID_W):
            if tx < tw:
                tile_bytes.extend(tiles.pixels_to_tile(
                    tiles.extract_tile(indices, width, tx, ty), bpp))
            elif ty < th - 1:
                tile_bytes.extend(blank)  # pad row out to the 16-tile stride
    arrays = [emit.u8(symbol + "_chr", tile_bytes)]
    if palette:
        arrays.append(emit.u8(symbol + "_pal", _palette_bytes(pal, ncolors)))
    return arrays, None


def sprite_sheet(image_path, symbol, frame="16x16", frames=None, bpp=4,
                 palette=True):
    """An animation strip: N OBJ frames sharing one VRAM block + palette.

    The source is a horizontal strip of frames, each `frame` pixels (a SNES OBJ
    size string). Every frame is laid out for the 16-tile-wide OBJ char grid so
    frame k is addressable on its own by OAM CHARNUM = k * <symbol>_STEP. Emits
    <symbol>_chr [+ <symbol>_pal] and the defines <symbol>_FRAMES (frame count),
    <symbol>_STEP (CHARNUM stride between frames) and <symbol>_TILES (char count
    the block occupies, i.e. the CHARNUM base a following block must start at).
    """
    if frame not in OBJ_SIZES:
        raise ValueError("frame must be one of %s"
                         % ", ".join(sorted(OBJ_SIZES)))
    ftw, fth = OBJ_SIZES[frame]
    ncolors = 2 ** bpp
    width, height, indices, pal = imageio.load_indexed(image_path, ncolors)
    count = frames if frames is not None else width // (ftw * 8)
    if count < 1 or count * ftw * 8 > width or fth * 8 > height:
        raise ValueError("%s sheet holds no %s frames" % (image_path, frame))
    if count * ftw > OBJ_GRID_W:
        raise ValueError("%d %s frames exceed the 16-tile OBJ char row"
                         % (count, frame))
    blank = [0] * (bpp * 8)
    tile_bytes = []
    for ty in range(fth):
        for tx in range(OBJ_GRID_W):
            f, lx = tx // ftw, tx % ftw
            if f < count:
                tile_bytes.extend(tiles.pixels_to_tile(
                    tiles.extract_tile(indices, width, f * ftw + lx, ty), bpp))
            else:
                tile_bytes.extend(blank)
    last = (fth - 1) * OBJ_GRID_W + (count - 1) * ftw + (ftw - 1)
    tile_bytes = tile_bytes[:(last + 1) * bpp * 8]
    arrays = [emit.u8(symbol + "_chr", tile_bytes)]
    if palette:
        arrays.append(emit.u8(symbol + "_pal", _palette_bytes(pal, ncolors)))
    defines = {symbol + "_FRAMES": count, symbol + "_STEP": ftw,
               symbol + "_TILES": OBJ_GRID_W * fth}
    return arrays, defines


def collision(mask_path, symbol, threshold=128):
    """A per-cell solid table from a black/white collision-mask PNG.

    Each 8x8 cell of the mask becomes one byte in <symbol>_col (row-major,
    col[cy * <symbol>_COLS + cx]): 1 if any pixel in the cell is at least
    `threshold` average brightness (white = solid), else 0. The mask must match
    the background image's pixel size. Also defines <symbol>_COLS / <symbol>_ROWS.
    """
    width, height, pixels = imageio.load_rgb(mask_path)
    cols, rows = width // 8, height // 8
    table = []
    for cy in range(rows):
        for cx in range(cols):
            solid = 0
            for y in range(8):
                for x in range(8):
                    r, g, b = pixels[(cy * 8 + y) * width + cx * 8 + x]
                    if (r + g + b) // 3 >= threshold:
                        solid = 1
                        break
                if solid:
                    break
            table.append(solid)
    return ([emit.u8(symbol + "_col", table)],
            {symbol + "_COLS": cols, symbol + "_ROWS": rows})


def raw_binary(bin_path, symbol):
    """Raw file bytes -> one unsigned char array. Emits <symbol>."""
    with open(bin_path, "rb") as f:
        data = list(f.read())
    return [emit.u8(symbol, data)], None
