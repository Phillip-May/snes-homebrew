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
