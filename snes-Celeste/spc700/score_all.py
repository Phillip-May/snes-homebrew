#!/usr/bin/env python3
"""Score all rendered tracks against reference audio."""
import numpy as np, wave, sys
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

project = Path(__file__).resolve().parent.parent
spc_dir = project / "spc700"
ref_dir = project / "reference_audio"

tracks = [
    ("mus0",  0, 47),
    ("mus10", 10, 45),
    ("mus20", 20, 59),
    ("mus30", 30, 30),
    ("mus40", 40, 8),
]

scores = {}
for name, pat, dur in tracks:
    wav = spc_dir / f"data_{name}.wav"
    ref = ref_dir / f"{name}.ogg"
    if not wav.exists():
        print(f"  {name}: MISSING WAV"); continue
    if not ref.exists():
        print(f"  {name}: MISSING REF"); continue
    s = score_track(wav, ref, dur)
    scores[name] = s
    print(f"  {name} (pat {pat:2d}, {dur:2d}s): {s:.4f}")

if scores:
    print(f"\n  Average: {np.mean(list(scores.values())):.4f}")
