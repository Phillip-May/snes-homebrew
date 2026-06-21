"""PNG <-> indexed pixel buffers, via Pillow."""

from PIL import Image


def load_indexed(path, max_colors, resize=None):
    """Load a PNG as palette indices plus an RGB palette.

    Already-indexed PNGs are read directly; anything else is quantized to
    max_colors. When resize=(w, h) is given and differs from the source size,
    the image is scaled with nearest-neighbour (keeps the indexed palette valid,
    right for pixel art in either direction). Returns
    (width, height, indices, palette_rgb).
    """
    img = Image.open(path)
    if img.mode != 'P':
        img = img.convert('RGB').quantize(colors=max_colors)
    if resize is not None and img.size != tuple(resize):
        img = img.resize(tuple(resize), Image.NEAREST)
    width, height = img.size
    indices = list(img.getdata())
    raw = img.getpalette() or []
    palette = []
    for i in range(0, len(raw), 3):
        palette.append((raw[i], raw[i + 1], raw[i + 2]))
    used = (max(indices) + 1) if indices else 0
    if len(palette) < used:
        palette += [(0, 0, 0)] * (used - len(palette))
    return width, height, indices, palette


def load_rgb(path):
    """Load a PNG as a flat list of (r, g, b) tuples plus its size."""
    img = Image.open(path).convert('RGB')
    width, height = img.size
    return width, height, list(img.getdata())


def save_indexed(path, width, height, indices, palette_rgb):
    """Write an indexed PNG from palette indices and an RGB palette."""
    img = Image.new('P', (width, height))
    flat = []
    for c in palette_rgb:
        flat.extend(c)
    flat += [0] * (768 - len(flat))
    img.putpalette(flat[:768])
    img.putdata(indices)
    img.save(path)
