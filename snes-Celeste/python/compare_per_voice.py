#!/usr/bin/env python3
"""Per-voice comparison: extract note events from single-voice DSP captures
and compare against reference SFX WAV pitch sequences."""

import csv, math, wave, numpy as np
from pathlib import Path
from collections import Counter

def dsp_to_freq(p):
    return p * 32000.0 / (4096.0 * 32.0) if p > 0 else 0

def freq_to_midi(f):
    return 69 + 12 * math.log2(f / 440) if f > 20 else 0

def midi_to_note(m):
    notes = ["C","C#","D","D#","E","F","F#","G","G#","A","A#","B"]
    mi = int(round(m))
    return f"{notes[mi%12]}{mi//12-1}" if mi > 0 else "---"

def extract_notes_from_csv(path):
    """Extract note-on events from single-voice capture CSV."""
    notes = []
    prev_midi = 0
    with open(path) as f:
        for row in csv.DictReader(f):
            pitch = int(row['pitch'])
            env = int(row['env'])
            frame = int(row['frame'])
            freq = dsp_to_freq(pitch)
            midi = int(round(freq_to_midi(freq))) if env > 2 and pitch > 0 else 0
            if midi != prev_midi and midi > 0:
                notes.append((frame / 60.0, midi, midi_to_note(midi)))
                prev_midi = midi
            elif midi == 0:
                prev_midi = 0
    return notes

def extract_notes_from_wav(path, sr_target=22050):
    """Extract pitch sequence from reference WAV using FFT."""
    with wave.open(str(path), 'r') as w:
        sr = w.getframerate()
        n = w.getnframes()
        data = np.frombuffer(w.readframes(n), dtype=np.int16).astype(float)

    # Analyze in windows
    win_size = 2048
    hop = win_size // 2
    notes = []
    prev_midi = 0
    for start in range(0, len(data) - win_size, hop):
        chunk = data[start:start + win_size] * np.hanning(win_size)
        fft = np.abs(np.fft.rfft(chunk))
        freqs = np.fft.rfftfreq(win_size, 1.0/sr)
        # Find peak above 30 Hz
        mask = freqs > 30
        if not mask.any() or fft[mask].max() < 1000:
            if prev_midi > 0:
                prev_midi = 0
            continue
        peak_idx = np.argmax(fft[mask]) + np.argmax(mask)
        freq = freqs[peak_idx]
        midi = int(round(freq_to_midi(freq)))
        t = start / sr
        if midi != prev_midi and midi > 0:
            notes.append((t, midi, midi_to_note(midi)))
            prev_midi = midi
    return notes

def compare_sequences(spc_notes, ref_notes, tolerance=0.2):
    """Compare with both exact and pitch-class matching."""
    matched_exact = 0
    matched_class = 0
    used = set()
    details = []

    for st, sm, sn in spc_notes:
        best_j = None
        best_type = None
        best_dt = tolerance + 1
        for j, (rt, rm, rn) in enumerate(ref_notes):
            if j in used:
                continue
            dt = abs(st - rt)
            if dt < tolerance:
                if sm == rm and dt < best_dt:
                    best_j = j
                    best_type = "EXACT"
                    best_dt = dt
                elif sm % 12 == rm % 12 and dt < best_dt:
                    best_j = j
                    best_type = "CLASS"
                    best_dt = dt
        if best_j is not None:
            used.add(best_j)
            if best_type == "EXACT":
                matched_exact += 1
                matched_class += 1
            else:
                matched_class += 1
            details.append((st, sn, ref_notes[best_j][2], best_type, best_dt))
        else:
            details.append((st, sn, "???", "MISS", 0))

    return matched_exact, matched_class, details


def main():
    project = Path(__file__).parent.parent
    spc_dir = project / "spc700"
    ref_dir = project / "reference_audio"

    # Pattern 0 channels: ch1=SFX10, ch2=SFX22, ch3=SFX4
    voice_sfx = {
        1: [10],  # SFX 10
        2: [22],  # SFX 22
        3: [4],   # SFX 4
    }

    for voice, sfx_ids in voice_sfx.items():
        csv_path = spc_dir / f"voice{voice}_capture.csv"
        if not csv_path.exists():
            print(f"\nVoice {voice}: no capture file")
            continue

        spc_notes = extract_notes_from_csv(str(csv_path))
        print(f"\n{'='*60}")
        print(f"Voice {voice} (SFX {sfx_ids}): {len(spc_notes)} SPC note events")
        print(f"{'='*60}")

        # Show SPC notes
        print(f"  SPC notes (first 15):")
        for t, m, n in spc_notes[:15]:
            print(f"    {t:6.3f}s  {n:>5s} (MIDI {m})")

        # Compare against reference SFX WAVs
        for sfx_id in sfx_ids:
            wav_path = ref_dir / f"snd{sfx_id}.wav"
            if not wav_path.exists():
                print(f"  No reference WAV for SFX {sfx_id}")
                continue

            ref_notes = extract_notes_from_wav(str(wav_path))
            print(f"\n  vs snd{sfx_id}.wav ({len(ref_notes)} ref notes):")
            print(f"  Ref notes (first 10):")
            for t, m, n in ref_notes[:10]:
                print(f"    {t:6.3f}s  {n:>5s} (MIDI {m})")

            if ref_notes and spc_notes:
                me, mc, details = compare_sequences(spc_notes[:len(ref_notes)*2], ref_notes, tolerance=0.3)
                n = len(ref_notes)
                print(f"\n  Exact: {me}/{n} ({me*100//max(1,n)}%)")
                print(f"  Class: {mc}/{n} ({mc*100//max(1,n)}%)")

                # Show mismatches
                misses = [(t, sn, rn, typ, dt) for t, sn, rn, typ, dt in details if typ == "MISS"]
                if misses:
                    print(f"  Misses ({len(misses)}):")
                    for t, sn, rn, typ, dt in misses[:5]:
                        print(f"    {t:6.3f}s  SPC={sn:>5s}  ref={rn}")

    # Also compare against full mus0.ogg using per-voice captures
    print(f"\n{'='*60}")
    print(f"Combined vs mus0.ogg")
    print(f"{'='*60}")

    all_spc = []
    for voice in [1, 2, 3]:
        csv_path = spc_dir / f"voice{voice}_capture.csv"
        if csv_path.exists():
            notes = extract_notes_from_csv(str(csv_path))
            all_spc.extend(notes)
    all_spc.sort(key=lambda x: x[0])

    ogg_path = ref_dir / "mus0.ogg"
    if ogg_path.exists():
        import librosa
        y, sr = librosa.load(str(ogg_path), sr=22050, mono=True)
        f0, voiced, _ = librosa.pyin(y, fmin=30, fmax=2000, sr=sr)
        times = librosa.frames_to_time(range(len(f0)), sr=sr)

        ref_notes = []
        prev_midi = 0
        for t, freq, v in zip(times, f0, voiced):
            if v and not np.isnan(freq) and freq > 20:
                midi = int(round(freq_to_midi(freq)))
                if midi != prev_midi:
                    ref_notes.append((t, midi, midi_to_note(midi)))
                    prev_midi = midi
            else:
                prev_midi = 0

        # Compare first 10s
        ref_10 = [n for n in ref_notes if n[0] < 10]
        spc_10 = [n for n in all_spc if n[0] < 10]
        me, mc, _ = compare_sequences(spc_10, ref_10, tolerance=0.2)
        n = len(ref_10)
        print(f"  First 10s: ref={n} notes, spc={len(spc_10)} notes")
        print(f"  Exact: {me}/{n} ({me*100//max(1,n)}%)")
        print(f"  Class: {mc}/{n} ({mc*100//max(1,n)}%)")


if __name__ == "__main__":
    main()
