#!/usr/bin/env python3
"""Compare SPC700 audio against all 5 ccleste reference music tracks."""
import numpy as np, wave, subprocess, os, sys
from pathlib import Path

def load_wav(path, target_sr=22050, duration=None):
    with wave.open(str(path), 'r') as w:
        sr = w.getframerate(); n_ch = w.getnchannels()
        raw = w.readframes(w.getnframes())
    audio = np.frombuffer(raw, dtype=np.int16).astype(np.float32) / 32768.0
    if n_ch > 1: audio = audio.reshape(-1, n_ch).mean(axis=1)
    if sr != target_sr:
        from scipy.signal import resample
        audio = resample(audio, int(len(audio) * target_sr / sr)).astype(np.float32)
    if duration:
        max_samples = int(target_sr * duration)
        if len(audio) > max_samples: audio = audio[:max_samples]
    return audio

def score_track(wav_path, ogg_path, duration):
    import librosa
    sr = 22050; hop = 512
    spc = load_wav(str(wav_path), sr, duration)
    ref, _ = librosa.load(str(ogg_path), sr=sr, mono=True, duration=float(duration))
    cr = librosa.feature.chroma_stft(y=ref, sr=sr, hop_length=hop)
    cs = librosa.feature.chroma_stft(y=spc, sr=sr, hop_length=hop)
    n = min(cr.shape[1], cs.shape[1])
    sims = []
    for i in range(n):
        r, s = cr[:,i], cs[:,i]
        nr, ns = np.linalg.norm(r), np.linalg.norm(s)
        if nr > 0.01 and ns > 0.01:
            sims.append(np.dot(r,s)/(nr*ns))
    return np.mean(sims) if sims else 0

def main():
    project = Path(__file__).parent.parent
    spc_dir = project / "spc700"
    ref_dir = project / "reference_audio"
    main_c = spc_dir / "main.c"

    tracks = [
        ("mus0",  0, 47),
        ("mus10", 10, 45),
        ("mus20", 20, 59),
        ("mus30", 30, 30),
        ("mus40", 40, 8),
    ]

    # Read main.c
    src = main_c.read_text()

    scores = {}
    for name, pattern, duration in tracks:
        ref = ref_dir / f"{name}.ogg"
        if not ref.exists() or ref.stat().st_size < 100:
            print(f"  {name}: SKIP (no valid reference)")
            continue

        # Patch source to start at this pattern (only the auto-start, not declaration)
        patched = src.replace(
            '        current_pattern = 0;\n        music_playing = 1;',
            f'        current_pattern = {pattern};\n        music_playing = 1;'
        )
        main_c.write_text(patched)

        # Build
        r = subprocess.run(['C:/Program Files/Git/usr/bin/bash.exe', 'build.sh'], capture_output=True, text=True, cwd=str(spc_dir))
        if r.returncode != 0:
            print(f"  {name}: BUILD FAILED"); continue

        # Render
        wav = spc_dir / f"test_{name}.wav"
        subprocess.run(
            ['/c/cygwin64/bin/bash.exe', '-l', '-c',
             f'cd "{spc_dir}" && ./spc2wav.exe main.spc {wav.name} {duration}'],
            capture_output=True, text=True
        )
        if not wav.exists():
            print(f"  {name}: RENDER FAILED"); continue

        # Score
        s = score_track(wav, ref, duration)
        scores[name] = s
        print(f"  {name} (pat {pattern:2d}, {duration:2d}s): {s:.4f}")
        wav.unlink(missing_ok=True)

    # Restore original
    main_c.write_text(src)
    subprocess.run(['C:/Program Files/Git/usr/bin/bash.exe', 'build.sh'], capture_output=True, text=True, cwd=str(spc_dir))

    if scores:
        print(f"\n  Average: {np.mean(list(scores.values())):.4f}")

if __name__ == "__main__":
    print("=== SPC700 vs All Reference Tracks ===")
    main()
