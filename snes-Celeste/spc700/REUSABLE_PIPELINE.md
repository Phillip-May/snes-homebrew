# Reusable PICO-8 -> SPC700 Pipeline

This folder contains a reusable asset pipeline that converts a PICO-8 cart into:
- `p8_music_data.h` (pattern/note/song data)
- `brrSamples.h` (SPC700 BRR waveform samples)

It is deterministic: same cart + same parameters -> same output bytes.

## Quick Start

Run from repo root:

```powershell
python python\p8_to_spc_pipeline.py `
  --cart C:\path\to\game.p8.png `
  --out-base C:\path\to\out `
  --spc-header C:\path\to\project\spc700\p8_music_data.h `
  --brr-header C:\path\to\project\spc700\brrSamples.h
```

Config-based run:

```powershell
copy python\p8_to_spc_pipeline.example.json my_cart.json
python python\p8_to_spc_pipeline.py --config my_cart.json
```

## How It Works

1. `dumpP8Music_picotool.py`
- Reads `.p8` / `.p8.png` via picotool.
- Extracts song/pattern/note/effect data.
- Writes `p8_music_data.h`.

2. `generate_brr_samples.py`
- Synthesizes each PICO-8 instrument waveform.
- Applies optional pre-emphasis compensation (to counter SNES Gaussian rolloff).
- Encodes BRR blocks and writes `brrSamples.h`.

3. `p8_to_spc_pipeline.py`
- Runs both tools in one command/config flow.

## Why Tuning Is Needed

PICO-8 and SPC700 are not acoustically equivalent, so straight conversion is never exact:
- BRR compression introduces quantization/filter artifacts.
- SNES DSP Gaussian interpolation attenuates highs.
- PICO-8 noise and SPC playback noise behavior differ.
- Mixing/envelope behavior differs from original runtime.

Tuning adjusts generator parameters so output better matches reference audio.

## Tuning Knobs

`generate_brr_samples.py` exposes:
- `--cap INST=VALUE` per-instrument pre-emphasis cap.
- `--noise-seed N` noise waveform RNG seed.
- `--noise-ref N` reference noise pitch.
- `--noise-norm F` noise amplitude normalization.

These values are in `python/p8_to_spc_pipeline.example.json` and can be overridden per cart.

## Recommended Tune/Verify Loop

1. Generate assets:
```powershell
python python\p8_to_spc_pipeline.py --config my_cart.json
```
2. Build/render all track WAVs from `spc700`:
```powershell
powershell -ExecutionPolicy Bypass -File .\build_spc700.ps1
# then render patterns with your existing test loop/scripts
```
3. Score objective similarity:
```powershell
python spc700\score_all.py
```
4. Validate SFX correctness:
```powershell
python spc700\sfx_conformance.py
```
5. Iterate only on data-level knobs (`cap`, `noise_*`) unless engine changes are intentional.

## Current Fidelity Snapshot

Measured on this repo (April 15, 2026):
- Prior manually tuned set: average `0.9647`
- Fresh auto-converted default set: average `0.9612`
- Gap: `-0.0035` (auto is very close, but not yet best-known)
