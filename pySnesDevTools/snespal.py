"""RGB888 <-> SNES BGR555 colour conversion."""


def rgb_to_snes(r, g, b):
    """Pack 8-bit RGB into a 15-bit SNES colour (0bbbbbgg gggrrrrr)."""
    return ((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3)


def snes_to_rgb(c):
    """Unpack a 15-bit SNES colour to 8-bit RGB, expanding 5 bits to 8."""
    r5 = c & 0x1F
    g5 = (c >> 5) & 0x1F
    b5 = (c >> 10) & 0x1F
    return ((r5 << 3) | (r5 >> 2),
            (g5 << 3) | (g5 >> 2),
            (b5 << 3) | (b5 >> 2))


def palette_to_bytes(colors):
    """16-bit SNES colours -> little-endian byte list (CGRAM order)."""
    out = []
    for c in colors:
        out.append(c & 0xFF)
        out.append((c >> 8) & 0xFF)
    return out


def bytes_to_palette(data):
    """Little-endian CGRAM bytes -> list of 15-bit SNES colours."""
    return [data[i] | (data[i + 1] << 8) for i in range(0, len(data), 2)]
