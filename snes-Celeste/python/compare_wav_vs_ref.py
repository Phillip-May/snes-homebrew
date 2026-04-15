#!/usr/bin/env python3
"""Compare spc_actual.wav (from spc2wav.exe) against reference mus0.ogg using chroma similarity."""

import numpy as np, wave
from pathlib import Path

def load_wav(path, target_sr=22050, duration=47.0):
    with wave.open(str(path), 'r') as w:
        sr = w.getframerate()
        n_ch = w.getnchannels()
        raw = w.readframes(w.getnframes())
    audio = np.frombuffer(raw, dtype=np.int16).astype(np.float32) / 32768.0
    if n_ch > 1:
        audio = audio.reshape(-1, n_ch).mean(axis=1)
    if sr != target_sr:
        from scipy.signal import resample
        n_target = int(len(audio) * target_sr / sr)
        audio = resample(audio, n_target).astype(np.float32)
    max_samples = int(target_sr * duration)
    if len(audio) > max_samples:
        audio = audio[:max_samples]
    return audio, target_sr

def main():
    import librosa
    project = Path(__file__).parent.parent
    spc_wav = project / "spc700" / "spc_actual.wav"
    ref_ogg = project / "reference_audio" / "mus0.ogg"

    sr = 22050
    hop = 512

    print("Loading SPC actual audio...")
    spc_audio, _ = load_wav(str(spc_wav), target_sr=sr, duration=47.0)
    print(f"  {len(spc_audio)} samples")

    print("Loading reference mus0.ogg...")
    ref_audio, _ = librosa.load(str(ref_ogg), sr=sr, mono=True, duration=47.0)
    print(f"  {len(ref_audio)} samples")

    print("Computing chromagrams...")
    chroma_ref = librosa.feature.chroma_stft(y=ref_audio, sr=sr, hop_length=hop, n_chroma=12)
    chroma_spc = librosa.feature.chroma_stft(y=spc_audio, sr=sr, hop_length=hop, n_chroma=12)

    n = min(chroma_ref.shape[1], chroma_spc.shape[1])

    # Strict frame-by-frame
    sims = np.full(n, np.nan)
    for i in range(n):
        r, s = chroma_ref[:, i], chroma_spc[:, i]
        nr, ns = np.linalg.norm(r), np.linalg.norm(s)
        if nr > 0.01 and ns > 0.01:
            sims[i] = np.dot(r, s) / (nr * ns)

    # Local-aligned +-3
    aligned = np.full(n, np.nan)
    for i in range(n):
        r = chroma_ref[:, i]
        nr = np.linalg.norm(r)
        if nr < 0.01:
            continue
        best = -1.0
        for j in range(max(0, i-3), min(n, i+4)):
            s = chroma_spc[:, j]
            ns = np.linalg.norm(s)
            if ns > 0.01:
                sim = np.dot(r, s) / (nr * ns)
                if sim > best:
                    best = sim
        if best >= 0:
            aligned[i] = best

    avg_strict = np.nanmean(sims)
    avg_aligned = np.nanmean(aligned)

    print(f"\n=== SPC700 ACTUAL AUDIO vs REFERENCE ===")
    print(f"  Strict frame-by-frame: {avg_strict:.4f}")
    print(f"  Local-aligned (+-3):   {avg_aligned:.4f}")

    fps = sr / hop
    print(f"\n  Per-second similarity:")
    for sec in range(47):
        s_idx = int(sec * fps)
        e_idx = min(int((sec+1) * fps), n)
        vals = sims[s_idx:e_idx]
        valid = vals[~np.isnan(vals)]
        if len(valid) > 0:
            m = np.mean(valid)
            bar = "#" * int(m * 40)
            print(f"    {sec:2d}-{sec+1:2d}s: {m:.3f} {bar}")

if __name__ == "__main__":
    main()
