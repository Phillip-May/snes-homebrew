"""Command-line wrapper over the pySnesDevTools API.

Runs either as a module (`python -m pySnesDevTools ...`) or as a plain script
by path (`python .../pySnesDevTools/snescli.py ...`); the latter bootstraps
sys.path so the package imports resolve without installation. Projects that
build assets call the library directly from their own `convert.py` instead.
"""

import argparse
import os
import sys

if __package__ in (None, ""):
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pySnesDevTools as snes


def main(argv=None):
    p = argparse.ArgumentParser(prog="pySnesDevTools",
                                description="SNES asset converters")
    sub = p.add_subparsers(dest="cmd")
    sub.required = True

    sp = sub.add_parser("graphics",
                        help="tile graphics only (no tilemap) + optional palette")
    sp.add_argument("image")
    sp.add_argument("-o", "--output", required=True)
    sp.add_argument("--symbol", required=True)
    sp.add_argument("--bpp", type=int, default=4)
    sp.add_argument("--no-palette", action="store_true",
                    help="omit the palette (e.g. a monochrome font)")

    sp = sub.add_parser("sprite",
                        help="one hardware OBJ sprite, ordered for the OBJ grid")
    sp.add_argument("image")
    sp.add_argument("-o", "--output", required=True)
    sp.add_argument("--symbol", required=True)
    sp.add_argument("--size", default="32x32",
                    help="SNES OBJ size: 8x8 16x16 32x32 64x64 16x32 32x64")
    sp.add_argument("--bpp", type=int, default=4)
    sp.add_argument("--no-palette", action="store_true")

    sp = sub.add_parser("sprite-sheet",
                        help="OBJ animation strip: N frames in one VRAM block")
    sp.add_argument("image")
    sp.add_argument("-o", "--output", required=True)
    sp.add_argument("--symbol", required=True)
    sp.add_argument("--frame", default="16x16",
                    help="SNES OBJ size of one frame: 8x8 16x16 32x32 ...")
    sp.add_argument("--frames", type=int, default=None,
                    help="frame count (default: inferred from sheet width)")
    sp.add_argument("--bpp", type=int, default=4)
    sp.add_argument("--no-palette", action="store_true")

    sp = sub.add_parser("background",
                        help="background layer: tiles + tilemap + palette")
    sp.add_argument("image")
    sp.add_argument("-o", "--output", required=True)
    sp.add_argument("--symbol", required=True)
    sp.add_argument("--bpp", type=int, default=4)
    sp.add_argument("--palette", choices=["single", "per_scanline"],
                    default="single")

    sp = sub.add_parser("collision",
                        help="per-cell solid table from a black/white mask PNG")
    sp.add_argument("image")
    sp.add_argument("-o", "--output", required=True)
    sp.add_argument("--symbol", required=True)
    sp.add_argument("--threshold", type=int, default=128,
                    help="min average brightness for a cell to count as solid")

    sp = sub.add_parser("bin2c", help="raw bytes -> C array")
    sp.add_argument("binary")
    sp.add_argument("-o", "--output", required=True)
    sp.add_argument("--symbol", required=True)

    sp = sub.add_parser("decode", help="reconstruct a source PNG from C arrays")
    sp.add_argument("header")
    sp.add_argument("-o", "--output", required=True)
    sp.add_argument("--symbol", required=True)
    sp.add_argument("--mode", choices=["background", "graphics"], required=True)
    sp.add_argument("--bpp", type=int, default=4)
    sp.add_argument("--tiles-x", type=int, default=32)
    sp.add_argument("--columns", type=int, default=8)

    args = p.parse_args(argv)

    if args.cmd == "graphics":
        snes.graphics(args.image, args.output, args.symbol, bpp=args.bpp,
                      palette=not args.no_palette)
    elif args.cmd == "sprite":
        snes.sprite(args.image, args.output, args.symbol, size=args.size,
                    bpp=args.bpp, palette=not args.no_palette)
    elif args.cmd == "sprite-sheet":
        snes.sprite_sheet(args.image, args.output, args.symbol,
                          frame=args.frame, frames=args.frames, bpp=args.bpp,
                          palette=not args.no_palette)
    elif args.cmd == "background":
        snes.background(args.image, args.output, args.symbol,
                        bpp=args.bpp, palette=args.palette)
    elif args.cmd == "collision":
        snes.collision(args.image, args.output, args.symbol,
                       threshold=args.threshold)
    elif args.cmd == "bin2c":
        snes.raw_binary(args.binary, args.output, args.symbol)
    elif args.cmd == "decode":
        snes.decode(args.header, args.output, args.symbol, args.mode,
                    bpp=args.bpp, tiles_x=args.tiles_x, columns=args.columns)
    return 0


if __name__ == "__main__":
    sys.exit(main())
