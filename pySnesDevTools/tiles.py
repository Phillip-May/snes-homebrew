"""8x8 pixel-index tiles <-> SNES planar bitplane bytes (1/2/4 bpp).

A tile is a flat list of 64 palette indices, row-major (y*8 + x).
SNES planar format interleaves bitplane pairs: for each row, plane0 then
plane1; higher plane pairs follow in 16-byte blocks.
"""


def _byte_index(plane, y, bpp):
    """Byte offset of a plane's row within a tile.

    Planes are stored in pairs (16 bytes each, rows interleaved lo/hi); an
    odd final plane (e.g. the single plane of 1bpp) is a lone 8-byte block.
    """
    if plane == bpp - 1 and bpp % 2 == 1:
        return (bpp // 2) * 16 + y
    return (plane // 2) * 16 + y * 2 + (plane % 2)


def pixels_to_tile(px, bpp):
    data = [0] * (bpp * 8)
    for y in range(8):
        for plane in range(bpp):
            byte = 0
            for x in range(8):
                byte |= ((px[y * 8 + x] >> plane) & 1) << (7 - x)
            data[_byte_index(plane, y, bpp)] = byte
    return data


def tile_to_pixels(data, bpp):
    px = [0] * 64
    for y in range(8):
        for plane in range(bpp):
            byte = data[_byte_index(plane, y, bpp)]
            for x in range(8):
                px[y * 8 + x] |= ((byte >> (7 - x)) & 1) << plane
    return px


def hflip(px):
    return [px[y * 8 + (7 - x)] for y in range(8) for x in range(8)]


def vflip(px):
    return [px[(7 - y) * 8 + x] for y in range(8) for x in range(8)]


def extract_tile(indices, width, tile_x, tile_y):
    """Pull an 8x8 tile (flat 64-index list) out of a flat index buffer."""
    return [indices[(tile_y * 8 + y) * width + tile_x * 8 + x]
            for y in range(8) for x in range(8)]


SCREEN_TILES = 32  # one SNES tilemap screen is 32x32 tiles (256x256 px)


def arrange_screens(tilemap, tiles_x, tiles_y):
    """Reorder a row-major tilemap into SNES 32x32 screen blocks.

    A background wider or taller than one screen (32 tiles) is stored in VRAM
    as separate 32x32 screens, laid out row-major (TL, TR, BL, BR) and each
    padded to a full 32x32. A map that fits one screen is returned unchanged,
    so single-screen backgrounds keep their exact byte layout. Raises if the
    map exceeds the SNES maximum of 2x2 screens (64x64 tiles / 512x512 px).
    """
    if tiles_x <= SCREEN_TILES and tiles_y <= SCREEN_TILES:
        return tilemap
    screens_w = (tiles_x + SCREEN_TILES - 1) // SCREEN_TILES
    screens_h = (tiles_y + SCREEN_TILES - 1) // SCREEN_TILES
    if screens_w > 2 or screens_h > 2:
        raise ValueError(
            "background is %dx%d tiles; SNES tilemaps allow at most 64x64"
            % (tiles_x, tiles_y))
    out = []
    for sr in range(screens_h):
        for sc in range(screens_w):
            for y in range(SCREEN_TILES):
                for x in range(SCREEN_TILES):
                    gx, gy = sc * SCREEN_TILES + x, sr * SCREEN_TILES + y
                    if gx < tiles_x and gy < tiles_y:
                        out.append(tilemap[gy * tiles_x + gx])
                    else:
                        out.append(0)
    return out


def tiles_from_indices(indices, width, height, bpp, dedup):
    """Cut an indexed image into tiles.

    Returns (tile_bytes, tilemap) where tile_bytes is a flat byte list and
    tilemap is a list of 16-bit SNES entries (vhopppTT TTTTTTTT). With dedup,
    identical tiles collapse to one entry; without, tiles stay in raster order.
    """
    tiles_x = width // 8
    tiles_y = height // 8
    tile_bytes = []
    tilemap = []
    seen = {}
    for ty in range(tiles_y):
        for tx in range(tiles_x):
            data = pixels_to_tile(extract_tile(indices, width, tx, ty), bpp)
            sig = tuple(data)
            if dedup and sig in seen:
                num = seen[sig]
            else:
                num = len(tile_bytes) // (bpp * 8)
                if dedup:
                    seen[sig] = num
                tile_bytes.extend(data)
            tilemap.append(num & 0x3FF)
    return tile_bytes, tilemap
