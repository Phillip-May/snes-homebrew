#!/usr/bin/env python3
"""Compare SPC700 DSP capture against ccleste reference audio (mus0.ogg).
Extracts pitch via autocorrelation and compares note sequences."""

import csv
import math
import numpy as np
from pathlib import Path

try:
    import librosa
except ImportError:
    print("pip install librosa soundfile")
    exit(1)

def dsp_to_freq(dsp_pitch):
    if dsp_pitch == 0:
        return 0
    return dsp_pitch * 32000.0 / (4096.0 * 32.0)

def freq_to_midi(freq):
    if freq < 20:
        return 0
    return 69 + 12 * math.log2(freq / 440.0)

def midi_to_note(midi):
    if midi < 1:
        return "---"
    notes = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
    m = int(round(midi))
    return f"{notes[m % 12]}{m // 12 - 1}"

def extract_pitches_from_ogg(ogg_path, hop_length=512, sr=22050):
    """Extract dominant pitch per frame from OGG file using librosa."""
    y, sr = librosa.load(ogg_path, sr=sr, mono=True)
    duration = len(y) / sr

    # Use pyin for pitch detection
    f0, voiced, _ = librosa.pyin(y, fmin=30, fmax=2000, sr=sr, hop_length=hop_length)

    frame_times = librosa.frames_to_time(range(len(f0)), sr=sr, hop_length=hop_length)

    pitches = []
    for i, (t, freq, v) in enumerate(zip(frame_times, f0, voiced)):
        if v and not np.isnan(freq) and freq > 20:
            pitches.append((t, freq, freq_to_midi(freq)))
        else:
            pitches.append((t, 0, 0))

    return pitches, duration, sr

def extract_notes_from_pitches(pitches, min_duration=0.02):
    """Convert frame-level pitches to note events."""
    notes = []
    current_midi = 0
    start_time = 0
    for t, freq, midi in pitches:
        quantized = int(round(midi)) if midi > 0 else 0
        if quantized != current_midi:
            if current_midi > 0 and t - start_time >= min_duration:
                notes.append((start_time, t, current_midi, midi_to_note(current_midi)))
            current_midi = quantized
            start_time = t
    return notes

def extract_notes_from_capture(csv_path, voices=[1, 2, 3]):
    """Extract note events from DSP capture CSV."""
    frames = []
    with open(csv_path) as f:
        reader = csv.DictReader(f)
        for row in reader:
            frame_data = {}
            for v in voices:
                pitch = int(row[f'v{v}_pitch'])
                env = int(row[f'v{v}_env'])
                freq = dsp_to_freq(pitch) if pitch > 0 and env > 2 else 0
                frame_data[v] = {'freq': freq, 'midi': freq_to_midi(freq) if freq > 20 else 0}
            frames.append(frame_data)

    # Extract note events per voice
    all_notes = {}
    for v in voices:
        events = []
        current_midi = 0
        start_frame = 0
        for f, data in enumerate(frames):
            quantized = int(round(data[v]['midi'])) if data[v]['midi'] > 0 else 0
            if quantized != current_midi:
                if current_midi > 0:
                    start_t = start_frame / 60.0
                    end_t = f / 60.0
                    events.append((start_t, end_t, current_midi, midi_to_note(current_midi)))
                current_midi = quantized
                start_frame = f
        all_notes[v] = events
    return all_notes


def compare_note_sequences(ref_notes, spc_notes, tolerance_sec=0.15):
    """Compare two note sequences with timing tolerance."""
    matched_exact = 0
    matched_class = 0  # same pitch class (semitone), any octave
    for rt, _, rmidi, rnote in ref_notes:
        for st, _, smidi, snote in spc_notes:
            if abs(rt - st) < tolerance_sec:
                if rmidi == smidi:
                    matched_exact += 1
                    matched_class += 1
                    break
                elif rmidi % 12 == smidi % 12:
                    matched_class += 1
                    break
    return matched_exact, matched_class, len(ref_notes), len(spc_notes)


def main():
    script_dir = Path(__file__).parent
    project_dir = script_dir.parent
    ref_path = project_dir / "reference_audio" / "mus0.ogg"
    csv_path = project_dir / "spc700" / "dsp_capture.csv"

    if not ref_path.exists():
        print(f"Reference audio not found: {ref_path}")
        return

    print(f"Analyzing reference audio: {ref_path}")
    ref_pitches, duration, sr = extract_pitches_from_ogg(str(ref_path))
    ref_notes = extract_notes_from_pitches(ref_pitches)
    print(f"  Duration: {duration:.1f}s, {len(ref_notes)} note events detected")

    # Show first 20 reference notes
    print(f"\n  First 20 reference notes:")
    for t_start, t_end, midi, note in ref_notes[:20]:
        freq = 440 * 2**((midi-69)/12)
        print(f"    {t_start:6.3f}s - {t_end:6.3f}s  {note:>5s} (MIDI {midi}, {freq:.1f}Hz)")

    print(f"\nAnalyzing SPC700 capture: {csv_path}")
    spc_notes = extract_notes_from_capture(str(csv_path), voices=[1, 2, 3])

    # Combine all SPC voices into one stream for comparison
    all_spc = []
    for v in [1, 2, 3]:
        for note in spc_notes[v]:
            all_spc.append(note)
    all_spc.sort(key=lambda x: x[0])

    print(f"  SPC700 total note events: {len(all_spc)}")
    print(f"\n  First 20 SPC700 notes:")
    for t_start, t_end, midi, note in all_spc[:20]:
        freq = 440 * 2**((midi-69)/12)
        print(f"    {t_start:6.3f}s - {t_end:6.3f}s  {note:>5s} (MIDI {midi}, {freq:.1f}Hz)")

    # Compare (using first 5 seconds of reference vs full SPC capture)
    ref_5s = [n for n in ref_notes if n[0] < 5.0]
    spc_5s = [n for n in all_spc if n[0] < 5.0]

    print(f"\n{'='*60}")
    print(f"Comparison (first 5 seconds, tolerance=150ms)")
    print(f"{'='*60}")

    matched_exact, matched_class, n_ref, n_spc = compare_note_sequences(ref_5s, spc_5s, tolerance_sec=0.15)
    pct_exact = matched_exact * 100 // max(1, n_ref)
    pct_class = matched_class * 100 // max(1, n_ref)
    print(f"  Reference notes (0-5s): {n_ref}")
    print(f"  SPC700 notes (0-5s):    {n_spc}")
    print(f"  Exact pitch match:      {matched_exact}/{n_ref} ({pct_exact}%)")
    print(f"  Pitch class match:      {matched_class}/{n_ref} ({pct_class}%) (same note name, any octave)")

    # Full song comparison (up to 10s)
    ref_10s = [n for n in ref_notes if n[0] < 10.0]
    spc_10s = [n for n in all_spc if n[0] < 10.0]
    me, mc, nr, ns = compare_note_sequences(ref_10s, spc_10s, tolerance_sec=0.2)
    print(f"\n  10-second comparison:")
    print(f"  Reference: {nr}, SPC700: {ns}")
    print(f"  Exact: {me}/{nr} ({me*100//max(1,nr)}%), Class: {mc}/{nr} ({mc*100//max(1,nr)}%)")

    # Per-voice analysis
    print(f"\n  Per-voice SPC700:")
    for v in [1, 2, 3]:
        notes = [n for n in spc_notes[v] if n[0] < 5.0]
        print(f"    Voice {v}: {len(notes)} notes in first 5s")
        for t_start, t_end, midi, note in notes[:8]:
            print(f"      {t_start:6.3f}s  {note:>5s}")

    # Pitch histogram comparison
    print(f"\n  Pitch class distribution (first 5s):")
    ref_classes = {}
    spc_classes = {}
    for _, _, midi, note in ref_5s:
        pc = midi % 12
        ref_classes[pc] = ref_classes.get(pc, 0) + 1
    for _, _, midi, note in spc_5s:
        pc = midi % 12
        spc_classes[pc] = spc_classes.get(pc, 0) + 1

    note_names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
    print(f"    {'Note':>4s} {'Ref':>4s} {'SPC':>4s}")
    for pc in range(12):
        r = ref_classes.get(pc, 0)
        s = spc_classes.get(pc, 0)
        if r > 0 or s > 0:
            print(f"    {note_names[pc]:>4s} {r:4d} {s:4d}")


if __name__ == "__main__":
    main()
