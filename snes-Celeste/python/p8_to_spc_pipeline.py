#!/usr/bin/env python3
"""Reusable end-to-end pipeline: PICO-8 cart -> SPC700 assets.

Outputs:
- p8_music_data.h (compact SFX/music tables)
- brrSamples.h (instrument BRR + pitch LUT)
- optional reports/C playback helpers
"""

import argparse
import json
import subprocess
import sys
from pathlib import Path


def run_cmd(cmd):
    print(">", " ".join(str(c) for c in cmd))
    proc = subprocess.run(cmd)
    if proc.returncode != 0:
        raise SystemExit(proc.returncode)


def apply_config(args, cfg):
    for key, value in cfg.items():
        if getattr(args, key, None) in (None, [], False):
            setattr(args, key, value)


def main():
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent

    parser = argparse.ArgumentParser(
        description="Generate reusable SPC assets from any PICO-8 cartridge."
    )
    parser.add_argument("--config", type=Path, default=None,
                        help="Optional JSON config file.")
    parser.add_argument("--cart", type=Path, default=None,
                        help="Input .p8/.p8.png cartridge.")
    parser.add_argument("--out-base", type=Path, default=None,
                        help="Base output path for reports/C code.")
    parser.add_argument("--spc-header", type=Path, default=None,
                        help="Output p8_music_data.h path.")
    parser.add_argument("--brr-header", type=Path, default=None,
                        help="Output brrSamples.h path.")
    parser.add_argument("--noise-seed", type=int, default=None)
    parser.add_argument("--noise-ref", type=int, default=None)
    parser.add_argument("--noise-norm", type=float, default=None)
    parser.add_argument("--cap", action="append", default=[],
                        help="Per-instrument preemphasis cap INST=CAP (repeatable).")
    parser.add_argument("--no-reports", action="store_true")
    parser.add_argument("--no-c-code", action="store_true")
    parser.add_argument("--no-spc-data", action="store_true")
    args = parser.parse_args()

    if args.config:
        cfg = json.loads(args.config.read_text(encoding="utf-8"))
        apply_config(args, cfg)

    cart = Path(args.cart) if args.cart else (script_dir / "15133.p8.png")
    out_base = Path(args.out_base) if args.out_base else cart
    spc_header = Path(args.spc_header) if args.spc_header else (repo_root / "spc700" / "p8_music_data.h")
    brr_header = Path(args.brr_header) if args.brr_header else (repo_root / "spc700" / "brrSamples.h")

    dump_script = script_dir / "dumpP8Music_picotool.py"
    brr_script = script_dir / "generate_brr_samples.py"

    dump_cmd = [
        sys.executable,
        str(dump_script),
        "--cart", str(cart),
        "--out-base", str(out_base),
        "--spc-header", str(spc_header),
    ]
    if args.no_reports:
        dump_cmd.append("--no-reports")
    if args.no_c_code:
        dump_cmd.append("--no-c-code")
    if args.no_spc_data:
        dump_cmd.append("--no-spc-data")
    run_cmd(dump_cmd)

    brr_cmd = [sys.executable, str(brr_script), "--out", str(brr_header)]
    if args.noise_seed is not None:
        brr_cmd += ["--noise-seed", str(args.noise_seed)]
    if args.noise_ref is not None:
        brr_cmd += ["--noise-ref", str(args.noise_ref)]
    if args.noise_norm is not None:
        brr_cmd += ["--noise-norm", str(args.noise_norm)]
    for cap in args.cap:
        brr_cmd += ["--cap", cap]
    run_cmd(brr_cmd)

    print("Pipeline complete.")
    print(f"  cart:       {cart}")
    print(f"  spc header: {spc_header}")
    print(f"  brr header: {brr_header}")


if __name__ == "__main__":
    main()
