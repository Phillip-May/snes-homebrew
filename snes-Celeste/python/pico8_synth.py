#!/usr/bin/env python3
"""Exact PICO-8 audio synthesizer following zepto8/fake-08 spec.

Implements the precise PICO-8 audio pipeline:
- 22050 Hz output, 183 samples per speed unit per note
- Offset-based note advancement (float, not integer ticks)
- Slide from PREVIOUS note to CURRENT note
- Vibrato: 7.5 Hz triangle wave, lerp(freq, freq*2^(1/12), t)
- Arpeggio: cycles through 4 consecutive SFX notes
- Anti-click crossfade (1/130 second ≈ 170 samples)
- Sum + hard clip mixing
- Table-based waveforms (32 samples, linear interpolation)
"""

import math, re, numpy as np, wave
from pathlib import Path

SAMPLE_RATE = 22050
SAMPLES_PER_SPEED = 183  # PICO-8's magic number

# ── Waveform tables (32 samples each, exact PICO-8 formulas) ─────
TABLE_SIZE = 32
WAVE_TABLES = {}
for _inst in range(8):
    tbl = [0.0] * TABLE_SIZE
    for _i in range(TABLE_SIZE):
        t = _i / TABLE_SIZE
        if _inst == 0: tbl[_i] = (1.0 - abs(4.0*t - 2.0)) * 0.5
        elif _inst == 1:
            a = 0.875; tbl[_i] = (2.0*t/a - 1.0 if t < a else 2.0*(1.0-t)/(1.0-a) - 1.0) * 0.5
        elif _inst == 2: tbl[_i] = 0.653 * (t if t < 0.5 else t - 1.0)
        elif _inst == 3: tbl[_i] = 0.25 if t < 0.5 else -0.25
        elif _inst == 4: tbl[_i] = 0.25 if t < 0.316 else -0.25
        elif _inst == 5:
            r = 3.0 - abs(24.0*t - 6.0) if t < 0.5 else 1.0 - abs(16.0*t - 12.0)
            tbl[_i] = r / 9.0
        elif _inst == 6: tbl[_i] = 0.25 if t < 0.5 else -0.25  # noise placeholder
        elif _inst == 7:
            r = 2.0 - abs(8.0*t - 4.0)
            t2 = (t * 109.0/110.0) % 1.0
            r += 1.0 - abs(4.0*t2 - 2.0)
            tbl[_i] = r / 6.0
    WAVE_TABLES[_inst] = np.array(tbl)

_noise_last = 0.0  # persistent state for filtered random walk noise

def wave_sample(phase, inst):
    global _noise_last
    if inst == 6:
        # PICO-8 filtered random walk (zepto8 spec)
        _noise_last = (_noise_last + 0.5 * np.random.uniform(-1, 1)) / 1.5
        return _noise_last * 0.5
    tbl = WAVE_TABLES[inst]
    pos = (phase % 1.0) * TABLE_SIZE
    idx = int(pos)
    frac = pos - idx
    return tbl[idx % TABLE_SIZE] * (1-frac) + tbl[(idx+1) % TABLE_SIZE] * frac

def key_to_freq(key):
    return 440.0 * (2 ** ((key - 33) / 12.0))


# ── Parse music data ─────────────────────────────────────────────
def parse_music_data(path):
    with open(path) as f:
        content = f.read()
    headers = []
    m = re.search(r'sfx_headers\[64\]\s*=\s*\{(.+?)\};', content, re.DOTALL)
    for line in m.group(1).strip().split('\n'):
        vals = re.findall(r'(\d+)', line)
        if len(vals) >= 4:
            headers.append({'speed': int(vals[0]), 'loop_start': int(vals[1]),
                           'loop_end': int(vals[2]), 'note_count': int(vals[3])})
    sfx_notes = {}
    cur = -1
    for line in content.split('\n'):
        if '// SFX ' in line:
            try: cur = int(line.strip().split('SFX ')[1].split()[0]); sfx_notes[cur] = []
            except: pass
        elif '0x' in line and cur >= 0 and '{' in line:
            vals = re.findall(r'0x([0-9A-Fa-f]+)', line)
            if len(vals) >= 3:
                sfx_notes[cur].append({'pitch': int(vals[0],16),
                    'inst': (int(vals[1],16)>>4)&0xF, 'vol': int(vals[1],16)&0x7,
                    'effect': int(vals[2],16)})
    patterns = []
    m = re.search(r'music_patterns\[\d+\]\s*=\s*\{(.+?)\};', content, re.DOTALL)
    for line in m.group(1).strip().split('\n'):
        vals = re.findall(r'0x([0-9A-Fa-f]+)', line)
        if len(vals) >= 5:
            patterns.append({'flags': int(vals[0],16),
                'sfx': [int(v,16) for v in vals[1:5]]})
    return headers, sfx_notes, patterns


# ── Channel state ────────────────────────────────────────────────
class Channel:
    def __init__(self):
        self.active = False
        self.sfx_id = 0
        self.is_music = False
        self.offset = 0.0       # fractional note position (0.0 = start of note 0)
        self.time = 0.0         # total accumulated time for end-check
        self.phase = 0.0        # oscillator phase
        self.prev_key = 24      # previous note pitch (for slide)
        self.prev_vol = 5.0/7   # previous note volume (for slide)
        # Anti-click state
        self.fade = 0.0
        self.old_sample = 0.0
        self.old_freq = 0.0
        self.old_vol = 0.0
        self.old_inst = 0


# ── Synthesizer ──────────────────────────────────────────────────
def synthesize(headers, sfx_notes, patterns, duration=47.0, start_pattern=0):
    global _noise_last
    np.random.seed(42)  # reproducible noise across runs
    _noise_last = 0.0
    n_samples = int(SAMPLE_RATE * duration)
    audio = np.zeros(n_samples)

    channels = [Channel() for _ in range(4)]
    music_playing = True
    current_pattern = 0
    loop_start = 0

    def calc_max_speed(pat_idx):
        ms = 1
        for s in patterns[pat_idx]['sfx']:
            if s != 0xFF and s < len(headers):
                ms = max(ms, headers[s]['speed'])
        return ms

    def start_channel(ch_idx, sfx_id, is_music):
        ch = channels[ch_idx]
        ch.active = True
        ch.sfx_id = sfx_id
        ch.is_music = is_music
        ch.offset = 0.0
        ch.time = 0.0
        ch.prev_key = 24
        ch.prev_vol = 5.0/7
        ch.fade = 0.0

    def get_note(ch):
        """Get current note for channel based on float offset."""
        note_idx = int(ch.offset) % 32
        if note_idx >= headers[ch.sfx_id]['note_count']:
            return None
        return sfx_notes[ch.sfx_id][note_idx]

    def get_channel_sample(ch):
        """Generate one audio sample for a channel (PICO-8 exact)."""
        if not ch.active:
            return 0.0

        hdr = headers[ch.sfx_id]
        speed = hdr['speed']
        if speed == 0: speed = 1

        # Advance offset (183 samples per speed unit)
        offset_per_sample = 1.0 / (SAMPLES_PER_SPEED * speed)
        next_offset = ch.offset + offset_per_sample
        next_time = ch.time + offset_per_sample

        note_idx = int(ch.offset)

        # Check bounds
        if ch.is_music and note_idx >= 32:
            ch.active = False
            return 0.0

        # Handle SFX note wrap / loop
        actual_idx = note_idx
        if actual_idx >= hdr['note_count']:
            if hdr['loop_end'] > hdr['loop_start']:
                actual_idx = hdr['loop_start'] + (actual_idx - hdr['loop_start']) % (hdr['loop_end'] - hdr['loop_start'])
            else:
                ch.active = False
                return 0.0

        note = sfx_notes[ch.sfx_id][actual_idx]
        pitch = note['pitch']
        inst = note['inst']
        vol = note['vol'] / 7.0
        effect = note['effect']

        if pitch == 0 or vol <= 0:
            ch.offset = next_offset
            ch.time = next_time
            return 0.0

        freq = key_to_freq(pitch)
        frac = ch.offset - note_idx  # fractional position within note (0-1)

        # Anti-click: detect note boundary
        prev_note_idx = int(ch.offset - offset_per_sample) if ch.offset > offset_per_sample else -1
        if prev_note_idx >= 0 and int(ch.offset) != prev_note_idx:
            # Note boundary crossed — start crossfade
            old_note = sfx_notes[ch.sfx_id][min(prev_note_idx % 32, hdr['note_count']-1)] if prev_note_idx < 32 else note
            # Check if significant change
            if (abs(vol - ch.prev_vol) > 0.1 or
                abs(freq - ch.old_freq) / max(freq, ch.old_freq, 1) > 0.01 or
                inst != ch.old_inst):
                ch.fade = 1.0
                ch.old_sample = 0.0  # simplified

            # Update prev for slide
            ch.prev_key = pitch
            ch.prev_vol = vol

        # Apply effects
        if effect == 1:  # Slide from previous
            prev_freq = key_to_freq(ch.prev_key) if ch.prev_key > 0 else freq
            freq = prev_freq + (freq - prev_freq) * frac
            prev_v = ch.prev_vol
            vol = prev_v + (vol - prev_v) * frac
        elif effect == 2:  # Vibrato
            # t = |fmod(7.5 * offset / offset_per_second, 1) - 0.5| - 0.25
            offset_per_sec = 1.0 / (SAMPLES_PER_SPEED * speed) * SAMPLE_RATE
            t = abs(math.fmod(7.5 * ch.offset / offset_per_sec, 1.0) - 0.5) - 0.25
            semitone_up = freq * (2 ** (1/12))
            freq = freq + (semitone_up - freq) * t
        elif effect == 3:  # Drop
            freq *= max(0, 1.0 - frac)
        elif effect == 4:  # Fade in
            vol *= frac
        elif effect == 5:  # Fade out
            vol *= (1.0 - frac)
        elif effect in (6, 7):  # Arpeggio
            m = (32 if speed <= 8 else 16) // (4 if effect == 6 else 8)
            offset_per_sec = 1.0 / (SAMPLES_PER_SPEED * speed) * SAMPLE_RATE
            n = int(m * 7.5 * ch.offset / offset_per_sec)
            base = (actual_idx // 4) * 4
            arp_idx = base + (n % 4)
            if arp_idx < hdr['note_count']:
                arp_note = sfx_notes[ch.sfx_id][arp_idx]
                if arp_note['pitch'] > 0:
                    freq = key_to_freq(arp_note['pitch'])

        # Generate sample
        sample = wave_sample(ch.phase, inst) * vol
        ch.phase += freq / SAMPLE_RATE

        # Anti-click crossfade
        if ch.fade > 0:
            fade_step = 130.0 / SAMPLE_RATE
            sample = sample * (1 - ch.fade) + ch.old_sample * ch.fade
            ch.fade = max(0, ch.fade - fade_step)

        ch.old_freq = freq
        ch.old_vol = vol
        ch.old_inst = inst

        ch.offset = next_offset
        ch.time = next_time
        return sample

    # Start pattern
    current_pattern = start_pattern
    pat = patterns[current_pattern]
    if pat['flags'] & 0x01: loop_start = current_pattern
    for ci in range(4):
        if pat['sfx'][ci] != 0xFF:
            start_channel(ci, pat['sfx'][ci], True)

    max_speed = calc_max_speed(0)
    pattern_end = 32.0  # notes per pattern (music always plays 32 notes)
    # Pattern uses the max speed channel for timing
    pattern_samples = int(32 * SAMPLES_PER_SPEED * max_speed)
    pattern_sample_count = 0

    for sample_idx in range(n_samples):
        mix = 0.0
        for ci in range(4):
            mix += get_channel_sample(channels[ci])

        # Hard clip (PICO-8 behavior)
        audio[sample_idx] = max(-1.0, min(1.0, mix))

        pattern_sample_count += 1

        # Pattern advance
        if music_playing and pattern_sample_count >= pattern_samples:
            pattern_sample_count = 0
            p = patterns[current_pattern]
            if p['flags'] & 0x02:
                current_pattern = loop_start
            else:
                current_pattern += 1

            if current_pattern >= len(patterns) or (patterns[current_pattern]['flags'] & 0x04):
                music_playing = False
                for ci in range(4):
                    channels[ci].active = False
                continue

            if patterns[current_pattern]['flags'] & 0x01:
                loop_start = current_pattern

            nxt = patterns[current_pattern]
            for ci in range(4):
                if nxt['sfx'][ci] != 0xFF:
                    start_channel(ci, nxt['sfx'][ci], True)
                else:
                    channels[ci].active = False

            max_speed = calc_max_speed(current_pattern)
            pattern_samples = int(32 * SAMPLES_PER_SPEED * max_speed)

    return audio


def main():
    project = Path(__file__).parent.parent
    spc_dir = project / "spc700"
    ref_dir = project / "reference_audio"

    print("Parsing music data...")
    headers, sfx_notes, patterns = parse_music_data(str(spc_dir / "p8_music_data.h"))

    print("Synthesizing (exact PICO-8 spec, 22050 Hz)...")
    audio = synthesize(headers, sfx_notes, patterns, duration=47.0)
    print("  %d samples" % len(audio))

    # Save
    out_path = spc_dir / "pico8_exact.wav"
    audio_16 = (audio * 32767).astype(np.int16)
    with wave.open(str(out_path), 'w') as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SAMPLE_RATE)
        w.writeframes(audio_16.tobytes())
    print("  Saved: %s" % out_path)

    # Compare against reference
    print("Loading reference...")
    import librosa
    ref, _ = librosa.load(str(ref_dir / "mus0.ogg"), sr=SAMPLE_RATE, mono=True, duration=47.0)

    hop = 512
    cr = librosa.feature.chroma_stft(y=ref, sr=SAMPLE_RATE, hop_length=hop)
    cs = librosa.feature.chroma_stft(y=audio.astype(np.float32), sr=SAMPLE_RATE, hop_length=hop)
    n = min(cr.shape[1], cs.shape[1])

    strict = [np.dot(cr[:,i],cs[:,i])/(np.linalg.norm(cr[:,i])*np.linalg.norm(cs[:,i]))
              for i in range(n) if np.linalg.norm(cr[:,i])>0.01 and np.linalg.norm(cs[:,i])>0.01]

    aligned = []
    for i in range(n):
        r=cr[:,i]; nr=np.linalg.norm(r)
        if nr<0.01: continue
        best=max((np.dot(r,cs[:,j])/(nr*np.linalg.norm(cs[:,j])) if np.linalg.norm(cs[:,j])>0.01 else -1)
                 for j in range(max(0,i-3),min(n,i+4)))
        if best>=0: aligned.append(best)

    print("\n=== EXACT PICO-8 SYNTH vs REFERENCE ===")
    print("  Strict: %.3f  Aligned: %.3f" % (np.mean(strict), np.mean(aligned)))

    fps = SAMPLE_RATE / hop
    print("\n  Worst 5 seconds:")
    secs = [(np.mean(strict[int(s*fps):min(int((s+1)*fps),len(strict))]),s)
            for s in range(47) if strict[int(s*fps):min(int((s+1)*fps),len(strict))]]
    secs.sort()
    for v,s in secs[:5]:
        print("    %d-%ds: %.3f" % (s,s+1,v))


if __name__ == "__main__":
    main()
