#!/usr/bin/env python3
"""Compare actual Mesen SPC700 audio capture against reference OGG.

This uses the real audio output from the SPC700 DSP (captured via VB-Cable),
not the synthesized proxy. This is the most accurate comparison possible.
"""

import numpy as np, wave, librosa
from pathlib import Path


def load_wav(path, target_sr=22050, duration=47.0):
    """Load WAV, convert to mono, resample."""
    with wave.open(str(path), 'r') as w:
        sr = w.getframerate()
        n_channels = w.getnchannels()
        n_frames = w.getnframes()
        raw = w.readframes(n_frames)

    audio = np.frombuffer(raw, dtype=np.int16).astype(np.float32) / 32768.0
    if n_channels > 1:
        audio = audio.reshape(-1, n_channels).mean(axis=1)

    # Resample if needed
    if sr != target_sr:
        n_target = int(len(audio) * target_sr / sr)
        from scipy.signal import resample
        audio = resample(audio, n_target).astype(np.float32)

    # Trim to duration
    max_samples = int(target_sr * duration)
    if len(audio) > max_samples:
        audio = audio[:max_samples]

    return audio, target_sr


def find_music_onset(audio, sr, threshold=0.01):
    """Find the sample index where music starts (above noise floor)."""
    window = int(sr * 0.01)  # 10ms window
    for i in range(0, len(audio) - window, window):
        rms = np.sqrt(np.mean(audio[i:i+window]**2))
        if rms > threshold:
            return max(0, i - window)  # back up slightly
    return 0


def main():
    project = Path(__file__).parent.parent
    spc_dir = project / "spc700"
    ref_dir = project / "reference_audio"

    mesen_wav = spc_dir / "spc_mesen_audio.wav"
    ref_ogg = ref_dir / "mus0.ogg"

    if not mesen_wav.exists():
        print(f"ERROR: {mesen_wav} not found. Run capture_audio.sh first.")
        return

    sr = 22050
    hop = 512

    print("Loading Mesen audio capture...")
    mesen_audio, _ = load_wav(str(mesen_wav), target_sr=sr, duration=52.0)
    print(f"  {len(mesen_audio)} samples at {sr} Hz")

    print("Loading reference...")
    ref_audio, _ = librosa.load(str(ref_ogg), sr=sr, mono=True, duration=47.0)
    print(f"  {len(ref_audio)} samples")

    # Find music onset in Mesen audio (may have silence at start)
    onset = find_music_onset(mesen_audio, sr)
    onset_sec = onset / sr
    print(f"  Mesen audio onset at sample {onset} ({onset_sec:.2f}s)")

    # Trim Mesen audio from onset, match reference length
    mesen_trimmed = mesen_audio[onset:onset + len(ref_audio)]
    if len(mesen_trimmed) < len(ref_audio):
        mesen_trimmed = np.pad(mesen_trimmed, (0, len(ref_audio) - len(mesen_trimmed)))

    print("\nComputing chromagrams...")
    chroma_ref = librosa.feature.chroma_stft(y=ref_audio, sr=sr, hop_length=hop, n_chroma=12)
    chroma_mes = librosa.feature.chroma_stft(y=mesen_trimmed, sr=sr, hop_length=hop, n_chroma=12)

    n = min(chroma_ref.shape[1], chroma_mes.shape[1])

    # Strict frame-by-frame
    strict_sims = []
    for i in range(n):
        r, s = chroma_ref[:, i], chroma_mes[:, i]
        nr, ns = np.linalg.norm(r), np.linalg.norm(s)
        if nr > 0.01 and ns > 0.01:
            strict_sims.append(np.dot(r, s) / (nr * ns))

    # Local-aligned (±3 frames)
    aligned_sims = []
    for i in range(n):
        r = chroma_ref[:, i]
        nr = np.linalg.norm(r)
        if nr < 0.01:
            continue
        best = -1.0
        for j in range(max(0, i-3), min(n, i+4)):
            s = chroma_mes[:, j]
            ns = np.linalg.norm(s)
            if ns > 0.01:
                sim = np.dot(r, s) / (nr * ns)
                if sim > best:
                    best = sim
        if best >= 0:
            aligned_sims.append(best)

    strict = np.mean(strict_sims) if strict_sims else 0
    aligned = np.mean(aligned_sims) if aligned_sims else 0

    print(f"\n=== ACTUAL SPC700 AUDIO vs REFERENCE ===")
    print(f"  Strict frame-by-frame: {strict:.3f}")
    print(f"  Local-aligned (±3):    {aligned:.3f}")

    # Also try optimal time offset
    best_offset = 0
    best_score = strict
    fps = sr / hop
    for offset in range(-int(3*fps), int(3*fps)):
        ss = 0; ct = 0
        for i in range(n):
            j = i + offset
            if 0 <= j < chroma_mes.shape[1]:
                r, s = chroma_ref[:, i], chroma_mes[:, j]
                nr, ns = np.linalg.norm(r), np.linalg.norm(s)
                if nr > 0.01 and ns > 0.01:
                    ss += np.dot(r, s) / (nr * ns)
                    ct += 1
        if ct > 0:
            avg = ss / ct
            if avg > best_score:
                best_score = avg
                best_offset = offset

    if best_offset != 0:
        print(f"  Best offset: {best_offset} frames ({best_offset/fps:.2f}s) → {best_score:.3f}")

    # Spectral comparison
    sc_ref = np.mean(librosa.feature.spectral_centroid(y=ref_audio, sr=sr))
    sc_mes = np.mean(librosa.feature.spectral_centroid(y=mesen_trimmed, sr=sr))
    print(f"\n  Spectral centroid: ref={sc_ref:.0f} Hz, mesen={sc_mes:.0f} Hz")

    # Per-second breakdown
    print(f"\n  Per-second similarity:")
    for sec in range(min(47, int(len(mesen_trimmed)/sr))):
        s, e = int(sec*fps), min(int((sec+1)*fps), len(strict_sims))
        if s < len(strict_sims):
            vals = strict_sims[s:e]
            if vals:
                m = np.mean(vals)
                bar = "#" * int(m * 40)
                print(f"    {sec:2d}-{sec+1:2d}s: {m:.3f} {bar}")


if __name__ == "__main__":
    main()
