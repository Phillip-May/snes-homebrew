#!/usr/bin/env python3
"""Compare SPC700 output vs reference by synthesizing expected audio and
comparing spectrograms. This avoids unreliable polyphonic pitch detection."""

import csv, math, numpy as np, wave
from pathlib import Path

def key_to_freq(key):
    return 440.0 * (2 ** ((key - 33) / 12.0))

def dsp_to_freq(p):
    return p * 32000.0 / (4096.0 * 32.0) if p > 0 else 0

def waveform_sample(phase, inst):
    """Generate one sample for a PICO-8-like waveform based on instrument."""
    p = phase % 1.0
    if inst == 0:  # triangle
        return 4 * p - 1 if p < 0.5 else 3 - 4 * p
    elif inst == 1:  # tilted saw
        return p / 0.75 * 2 - 1 if p < 0.75 else 1 - (p - 0.75) / 0.25 * 2
    elif inst == 2:  # saw
        return 2 * p - 1
    elif inst == 3:  # square
        return 1.0 if p < 0.5 else -1.0
    elif inst == 4:  # pulse
        return 1.0 if p < 0.25 else -1.0
    elif inst == 5:  # organ
        return 0.7 * math.sin(2 * math.pi * p) + 0.3 * math.sin(4 * math.pi * p)
    elif inst == 6:  # noise
        return np.random.uniform(-1, 1)
    elif inst == 7:  # phaser
        return 0.6 * math.sin(2 * math.pi * p) + 0.4 * math.sin(2 * math.pi * p * 1.02 + 0.5)
    return math.sin(2 * math.pi * p)

def synthesize_from_capture(csv_path, sr=22050, duration=47.0):
    """Synthesize audio from DSP capture using PICO-8 waveform shapes."""
    n_samples = int(sr * duration)
    audio = np.zeros(n_samples)
    phases = [0.0] * 4

    with open(csv_path) as f:
        rows = list(csv.DictReader(f))

    frame_duration = 1.0 / 60.0
    samples_per_frame = int(sr * frame_duration)

    for row in rows:
        frame = int(row['frame'])
        if frame * samples_per_frame >= n_samples:
            break

        start = frame * samples_per_frame
        end = min(start + samples_per_frame, n_samples)

        for v in range(4):
            pitch = int(row[f'v{v}_pitch'])
            env = int(row[f'v{v}_env'])
            vol = int(row[f'v{v}_vol'])
            srcn = int(row[f'v{v}_srcn'])
            if pitch > 0 and env > 2 and vol > 0:
                freq = dsp_to_freq(pitch)
                amp = (vol / 127.0) * (env / 127.0) * 0.25
                for i in range(end - start):
                    audio[start + i] += amp * waveform_sample(phases[v], srcn)
                    phases[v] += freq / sr
            else:
                phases[v] = 0

    peak = np.max(np.abs(audio))
    if peak > 0:
        audio = audio / peak * 0.9
    return audio, sr

def load_reference(ogg_path, sr=22050, duration=47.0):
    """Load reference OGG."""
    import librosa
    y, sr_out = librosa.load(str(ogg_path), sr=sr, mono=True, duration=duration)
    return y, sr_out

def compute_chroma(audio, sr, hop=512):
    """Compute chromagram (pitch class energy over time)."""
    import librosa
    return librosa.feature.chroma_stft(y=audio, sr=sr, hop_length=hop, n_chroma=12)

def compare_chromas(chroma_ref, chroma_spc, fps=43):
    """Compare two chromagrams frame-by-frame."""
    n = min(chroma_ref.shape[1], chroma_spc.shape[1])
    notes = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]

    # Per-frame cosine similarity (frame-indexed, not compressed)
    frame_sims = np.full(n, np.nan)
    for i in range(n):
        ref = chroma_ref[:, i]
        spc = chroma_spc[:, i]
        norm_r = np.linalg.norm(ref)
        norm_s = np.linalg.norm(spc)
        if norm_r > 0.01 and norm_s > 0.01:
            frame_sims[i] = np.dot(ref, spc) / (norm_r * norm_s)

    valid = ~np.isnan(frame_sims)
    avg_sim = np.nanmean(frame_sims) if valid.any() else 0
    return avg_sim, frame_sims

def main():
    project = Path(__file__).parent.parent
    spc_dir = project / "spc700"
    ref_dir = project / "reference_audio"
    csv_path = spc_dir / "dsp_capture.csv"
    ogg_path = ref_dir / "mus0.ogg"

    print("Synthesizing SPC700 audio from DSP capture...")
    spc_audio, sr = synthesize_from_capture(str(csv_path), duration=47.0)
    print(f"  {len(spc_audio)} samples at {sr}Hz")

    print("Loading reference mus0.ogg...")
    ref_audio, _ = load_reference(str(ogg_path), sr=sr, duration=47.0)
    print(f"  {len(ref_audio)} samples at {sr}Hz")

    print("Computing chromagrams...")
    import librosa
    hop = 512
    chroma_ref = compute_chroma(ref_audio, sr, hop)
    chroma_spc = compute_chroma(spc_audio, sr, hop)
    print(f"  Ref: {chroma_ref.shape}, SPC: {chroma_spc.shape}")

    print("\nComparing...")
    avg_sim, sims = compare_chromas(chroma_ref, chroma_spc)
    print(f"  Average chroma similarity: {avg_sim:.3f} (1.0 = perfect)")

    # Per-second breakdown
    fps = sr / hop
    print(f"\n  Per-second similarity:")
    for sec in range(47):
        start = int(sec * fps)
        end = min(int((sec + 1) * fps), len(sims))
        sec_vals = sims[start:end]
        valid_vals = sec_vals[~np.isnan(sec_vals)]
        if len(valid_vals) > 0:
            s = np.mean(valid_vals)
            bar = "#" * int(s * 40)
            print(f"    {sec:2d}-{sec+1:2d}s: {s:.3f} {bar}")

    # Per-pitch-class analysis
    notes = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
    n = min(chroma_ref.shape[1], chroma_spc.shape[1])
    print(f"\n  Per-pitch-class energy correlation:")
    for pc in range(12):
        ref_energy = chroma_ref[pc, :n]
        spc_energy = chroma_spc[pc, :n]
        if np.std(ref_energy) > 0.01 and np.std(spc_energy) > 0.01:
            corr = np.corrcoef(ref_energy, spc_energy)[0, 1]
            print(f"    {notes[pc]:>2s}: {corr:+.3f}")
        else:
            print(f"    {notes[pc]:>2s}: (low energy)")

    # Save synthesized audio for manual listening comparison
    spc_wav_path = spc_dir / "spc_synth.wav"
    audio_16 = (spc_audio * 32767).astype(np.int16)
    with wave.open(str(spc_wav_path), 'w') as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(sr)
        w.writeframes(audio_16.tobytes())
    print(f"\n  Saved synthesized SPC audio: {spc_wav_path}")

if __name__ == "__main__":
    main()
