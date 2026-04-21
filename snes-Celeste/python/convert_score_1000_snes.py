#!/usr/bin/env python
from pathlib import Path
from PIL import Image

SCRIPT_DIR = Path(__file__).resolve().parent
FONT_SRC = SCRIPT_DIR / "pico8_font.bmp"
OUT = SCRIPT_DIR / "score_1000_snes.h"
PREVIEW = SCRIPT_DIR / "score_1000_snes_preview.png"

TEXT = "1000"
PICO_ADVANCE = 4
SCALE = 2
WIDTH = len(TEXT) * PICO_ADVANCE * SCALE
HEIGHT = 8 * SCALE


def read_font_rows():
    if not FONT_SRC.exists():
        raise SystemExit("missing Pico-8 font image: {}".format(FONT_SRC))

    img = Image.open(str(FONT_SRC)).convert("P")
    if img.width < 128 or img.height < 64:
        raise SystemExit("unexpected Pico-8 font dimensions: {}".format(img.size))

    rows = []
    for code in range(128):
        ox = (code % 16) * 8
        oy = (code // 16) * 8
        glyph = []
        for gy in range(8):
            row = 0
            for gx in range(8):
                if img.getpixel((ox + gx, oy + gy)) != 0:
                    row |= 1 << (7 - gx)
            glyph.append(row)
        rows.append(glyph)
    return rows


def render_score(rows):
    img = Image.new("P", (WIDTH, HEIGHT), 0)
    img.putpalette([
        0, 0, 0,
        255, 255, 255,
    ] + [0, 0, 0] * 254)

    pen_x = 0
    for ch in TEXT:
        glyph = rows[ord(ch)]
        for gy, row in enumerate(glyph):
            for gx in range(PICO_ADVANCE):
                if (row & (0x80 >> gx)) == 0:
                    continue
                px = pen_x + gx * SCALE
                py = gy * SCALE
                for sy in range(SCALE):
                    for sx in range(SCALE):
                        img.putpixel((px + sx, py + sy), 1)
        pen_x += PICO_ADVANCE * SCALE

    return img


def tile_to_4bpp(tile):
    out = bytearray(32)
    pixels = list(tile.getdata())
    for y in range(8):
        bp0 = 0
        bp1 = 0
        bp2 = 0
        bp3 = 0
        for x in range(8):
            value = pixels[y * 8 + x] & 0x0F
            bit = 7 - x
            bp0 |= (value & 1) << bit
            bp1 |= ((value >> 1) & 1) << bit
            bp2 |= ((value >> 2) & 1) << bit
            bp3 |= ((value >> 3) & 1) << bit
        out[y * 2] = bp0
        out[y * 2 + 1] = bp1
        out[y * 2 + 16] = bp2
        out[y * 2 + 17] = bp3
    return out


def format_array(name, data):
    lines = ["static const unsigned char {}[{}] = {{".format(name, len(data))]
    for i in range(0, len(data), 16):
        lines.append("    " + ", ".join("0x{:02X}".format(v) for v in data[i:i + 16]) + ",")
    lines.append("};")
    return "\n".join(lines)


def main():
    rows = read_font_rows()
    img = render_score(rows)
    img.convert("RGB").save(str(PREVIEW))

    top = bytearray()
    bottom = bytearray()
    for sprite in range(2):
        base_x = sprite * 16
        top.extend(tile_to_4bpp(img.crop((base_x, 0, base_x + 8, 8))))
        top.extend(tile_to_4bpp(img.crop((base_x + 8, 0, base_x + 16, 8))))
    for sprite in range(2):
        base_x = sprite * 16
        bottom.extend(tile_to_4bpp(img.crop((base_x, 8, base_x + 8, 16))))
        bottom.extend(tile_to_4bpp(img.crop((base_x + 8, 8, base_x + 16, 16))))

    header = "\n".join([
        "#ifndef SCORE_1000_SNES_H",
        "#define SCORE_1000_SNES_H",
        "",
        "#define SCORE_1000_SPRITE_TILE_LEFT 0xC0u",
        "#define SCORE_1000_SPRITE_TILE_RIGHT 0xC2u",
        "#define SCORE_1000_SPRITE_VRAM_TOP 0x9800u",
        "#define SCORE_1000_SPRITE_VRAM_BOTTOM 0x9A00u",
        "",
        format_array("score_1000_sprite_top_4bpp", top),
        "",
        format_array("score_1000_sprite_bottom_4bpp", bottom),
        "",
        "#endif",
        "",
    ])

    OUT.write_text(header)
    print("Wrote {}".format(OUT))
    print("Wrote {}".format(PREVIEW))


if __name__ == "__main__":
    main()
