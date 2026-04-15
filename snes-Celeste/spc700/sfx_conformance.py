#!/usr/bin/env python3
"""Render used SFX via blargg SPC decode and compare current data vs canonical agentbase.

This is a deterministic conformance check:
1) Patch SPC runtime to auto-play one SFX in isolation.
2) Build `main.spc` and render with `spc2wav.exe` (blargg snes_spc).
3) Repeat with `p8_music_data.h.agentbase` as canonical data.
4) Report similarity metrics for each used SFX.
"""

from __future__ import annotations

import contextlib
import re
import shutil
import subprocess
from pathlib import Path

import numpy as np

try:
    import librosa
except Exception as exc:  # pragma: no cover
    raise SystemExit(f"librosa is required: {exc}")


ROOT = Path(__file__).resolve().parent
MAIN_C = ROOT / "main.c"
DATA_H = ROOT / "p8_music_data.h"
DATA_BASE = ROOT / "p8_music_data.h.agentbase"
SPC = ROOT / "main.spc"
SPC2WAV = ROOT / "spc2wav.exe"
BUILD_SH = ROOT / "build.sh"
GIT_BASH = Path(r"C:\Program Files\Git\bin\bash.exe")
CYG_BASH = Path(r"C:\cygwin64\bin\bash.exe")

# SFX used by game logic (mapped from mainBankZero to Pico-8 SFX IDs)
USED_SFX = {
    "jump": 1,
    "wall_jump": 2,
    "dash_start": 3,
    "dash_misfire": 9,
    "dash_restore": 54,
    "death": 0,
    "break_wall": 16,
    "strawberry": 13,
    "spring": 8,
    "balloon_pop": 14,
    "key_collect": 23,
    "text_tick": 35,
    "big_chest": 37,
}


def run_bash(script: str, bash_exe: Path) -> None:
    proc = subprocess.run(
        [str(bash_exe), "-lc", script],
        cwd=str(ROOT),
        capture_output=True,
        text=True,
    )
    if proc.returncode != 0:
        raise RuntimeError(
            f"Command failed: {script}\nstdout:\n{proc.stdout}\nstderr:\n{proc.stderr}"
        )


def build_spc() -> None:
    run_bash(f'cd "{ROOT.as_posix()}" && bash build.sh', GIT_BASH)


def render_wav(out_name: str, duration: float) -> Path:
    out = ROOT / out_name
    if out.exists():
        out.unlink()
    secs = int(round(duration))
    run_bash(f'cd "{ROOT.as_posix()}" && ./spc2wav.exe main.spc {out.name} {secs}', CYG_BASH)
    if not out.exists():
        raise RuntimeError(f"Render failed: {out}")
    return out


def patched_main_for_sfx(src: str, sfx_id: int) -> str:
    end = '    debug_println("music started");'
    j = src.find(end)
    if j < 0:
        raise RuntimeError("Could not locate auto-start end marker in main.c")
    # Accept either music or single-SFX boot comment in main.c.
    m = re.search(r"^\s*// Auto-start .* for standalone \.spc testing\s*$", src[:j], re.M)
    if not m:
        raise RuntimeError("Could not locate auto-start block header in main.c")
    i = m.start()
    repl = f"""    // Auto-start single SFX for standalone .spc testing
    {{
        music_playing = 0;
        current_pattern = 0;
        start_sfx_on_channel(SFX_VOICE, {sfx_id}, 0);
        advance_channel(SFX_VOICE);
        pattern_ticks_remaining = 0;
        writeDSPREG(DSP_MVOL0L, 127);
        writeDSPREG(DSP_MVOL0R, 127);
        writeDSPREG(DSP_NON, 0);
    }}
"""
    return src[:i] + repl + src[j:]


def load_mono(path: Path, sr: int = 22050) -> np.ndarray:
    y, _ = librosa.load(str(path), sr=sr, mono=True)
    return y.astype(np.float32)


def align_signals(a: np.ndarray, b: np.ndarray, max_lag: int = 4096) -> tuple[np.ndarray, np.ndarray]:
    # coarse cross-correlation alignment in +-max_lag samples
    n = min(len(a), len(b))
    if n <= max_lag * 2:
        n = min(len(a), len(b))
    aa = a[:n]
    bb = b[:n]
    best_lag = 0
    best = -1e30
    for lag in range(-max_lag, max_lag + 1, 32):
        if lag >= 0:
            x = aa[lag:]
            y = bb[: len(x)]
        else:
            y = bb[-lag:]
            x = aa[: len(y)]
        if len(x) < 256:
            continue
        c = float(np.dot(x, y))
        if c > best:
            best = c
            best_lag = lag
    if best_lag >= 0:
        x = aa[best_lag:]
        y = bb[: len(x)]
    else:
        y = bb[-best_lag:]
        x = aa[: len(y)]
    m = min(len(x), len(y))
    return x[:m], y[:m]


def compare(cur_wav: Path, ref_wav: Path) -> dict[str, float]:
    cur = load_mono(cur_wav)
    ref = load_mono(ref_wav)
    cur, ref = align_signals(cur, ref, max_lag=4096)
    if len(cur) < 2048:
        return {"corr": 0.0, "chroma": 0.0, "centroid_err_pct": 100.0}

    corr = float(np.corrcoef(cur, ref)[0, 1])
    if not np.isfinite(corr):
        corr = 0.0

    sr = 22050
    hop = 256
    c_cur = librosa.feature.chroma_stft(y=cur, sr=sr, hop_length=hop)
    c_ref = librosa.feature.chroma_stft(y=ref, sr=sr, hop_length=hop)
    n = min(c_cur.shape[1], c_ref.shape[1])
    sims = []
    for i in range(n):
        a = c_cur[:, i]
        b = c_ref[:, i]
        na = np.linalg.norm(a)
        nb = np.linalg.norm(b)
        if na > 1e-6 and nb > 1e-6:
            sims.append(float(np.dot(a, b) / (na * nb)))
    chroma = float(np.mean(sims)) if sims else 0.0

    sc_cur = float(np.mean(librosa.feature.spectral_centroid(y=cur, sr=sr)))
    sc_ref = float(np.mean(librosa.feature.spectral_centroid(y=ref, sr=sr)))
    centroid_err_pct = abs(sc_cur - sc_ref) / max(1.0, sc_ref) * 100.0

    return {"corr": corr, "chroma": chroma, "centroid_err_pct": centroid_err_pct}


def main() -> None:
    if not DATA_BASE.exists():
        raise SystemExit(f"Missing canonical data file: {DATA_BASE}")
    if not SPC2WAV.exists():
        raise SystemExit(f"Missing converter: {SPC2WAV}")

    original_main = MAIN_C.read_text(encoding="utf-8")
    original_data = DATA_H.read_text(encoding="utf-8")

    temp_files: list[Path] = []
    try:
        print("SFX conformance against canonical agentbase data")
        print(f"Using converter: {SPC2WAV.name} (blargg snes_spc)")
        print("")
        for name, sfx_id in USED_SFX.items():
            MAIN_C.write_text(patched_main_for_sfx(original_main, sfx_id), encoding="utf-8")

            DATA_H.write_text(original_data, encoding="utf-8")
            build_spc()
            cur_wav = render_wav(f"tmp_cur_sfx_{sfx_id:02d}.wav", 2.5)
            temp_files.append(cur_wav)

            DATA_H.write_text(DATA_BASE.read_text(encoding="utf-8"), encoding="utf-8")
            build_spc()
            ref_wav = render_wav(f"tmp_ref_sfx_{sfx_id:02d}.wav", 2.5)
            temp_files.append(ref_wav)

            m = compare(cur_wav, ref_wav)
            print(
                f"{name:12s} sfx={sfx_id:2d}  "
                f"corr={m['corr']:.4f}  chroma={m['chroma']:.4f}  "
                f"centroid_err={m['centroid_err_pct']:.2f}%"
            )
    finally:
        MAIN_C.write_text(original_main, encoding="utf-8")
        DATA_H.write_text(original_data, encoding="utf-8")
        with contextlib.suppress(Exception):
            build_spc()
        for p in temp_files:
            with contextlib.suppress(Exception):
                p.unlink()


if __name__ == "__main__":
    main()
