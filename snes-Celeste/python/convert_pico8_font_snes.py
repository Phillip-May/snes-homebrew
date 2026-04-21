#!/usr/bin/env python
from pathlib import Path
from PIL import Image

SCRIPT_DIR = Path(__file__).resolve().parent
FONT_SRC = SCRIPT_DIR / "pico8_font.bmp"
OUT = SCRIPT_DIR / "pico8_font_snes.h"
OUT_BIN = SCRIPT_DIR / "pico8_font_rows.bin"


def fmt_rows(rows):
    out = [
        "#ifndef PICO8_FONT_SNES_H",
        "#define PICO8_FONT_SNES_H",
        "#ifndef PICO8_FONT_STORAGE",
        "#define PICO8_FONT_STORAGE",
        "#endif",
        "",
        "PICO8_FONT_STORAGE const unsigned char pico8_font_rows[128][8] = {",
    ]
    for code in range(128):
        glyph = rows[code]
        out.append(
            "    {" + ", ".join(f"0x{row:02X}" for row in glyph) + "},"
        )
    out.extend([
        "};",
        "",
        "#endif",
        "",
    ])
    return "\n".join(out)


def main():
    if not FONT_SRC.exists():
        raise SystemExit(f"missing Pico-8 font image: {FONT_SRC}")

    img = Image.open(FONT_SRC).convert("P")
    if img.width < 128 or img.height < 64:
        raise SystemExit(f"unexpected Pico-8 font dimensions: {img.size}")

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

    OUT.write_text(fmt_rows(rows))
    OUT_BIN.write_bytes(bytes(row for glyph in rows for row in glyph))
    print(f"Wrote {OUT}")
    print(f"Wrote {OUT_BIN}")


if __name__ == "__main__":
    main()
