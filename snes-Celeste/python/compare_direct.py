#!/usr/bin/env python3
"""Direct comparison: simulate PICO-8 sequencer → synthesize audio → compare chroma.

This bypasses the DSP capture entirely and synthesizes audio directly from the
PICO-8 music data, giving the theoretical maximum chroma score. The gap between
this score and the DSP-capture-based score shows how much error comes from the
60fps DSP snapshot approximation vs the actual SPC700 output.
"""

import csv, math, re, numpy as np, wave
from pathlib import Path


# PICO-8 uses 32-sample lookup tables with linear interpolation.
_WAVE_TABLE_SIZE = 32
_WAVE_TABLES = {}
for _inst in range(8):
    _tbl = [0.0] * _WAVE_TABLE_SIZE
    for _i in range(_WAVE_TABLE_SIZE):
        _t = _i / _WAVE_TABLE_SIZE
        if _inst == 0: _tbl[_i] = (1.0 - abs(4.0*_t - 2.0)) * 0.5
        elif _inst == 1:
            _a = 0.875; _tbl[_i] = (2.0*_t/_a - 1.0 if _t < _a else 2.0*(1.0-_t)/(1.0-_a) - 1.0) * 0.5
        elif _inst == 2: _tbl[_i] = 0.653 * (_t if _t < 0.5 else _t - 1.0)
        elif _inst == 3: _tbl[_i] = 0.25 if _t < 0.5 else -0.25
        elif _inst == 4: _tbl[_i] = 0.25 if _t < 0.316 else -0.25
        elif _inst == 5:
            _r = 3.0 - abs(24.0*_t - 6.0) if _t < 0.5 else 1.0 - abs(16.0*_t - 12.0)
            _tbl[_i] = _r / 9.0
        elif _inst == 6: _tbl[_i] = 0.25 if _t < 0.5 else -0.25
        elif _inst == 7:
            _r = 2.0 - abs(8.0*_t - 4.0)
            _t2 = (_t * 109.0/110.0) % 1.0
            _r += 1.0 - abs(4.0*_t2 - 2.0)
            _tbl[_i] = _r / 6.0
    _WAVE_TABLES[_inst] = _tbl

def waveform_sample(phase, inst):
    """Table-based waveform with linear interpolation, matching PICO-8's 32-sample tables."""
    if inst == 6:
        return np.random.uniform(-1, 1) * 0.25
    tbl = _WAVE_TABLES[inst]
    pos = (phase % 1.0) * _WAVE_TABLE_SIZE
    idx = int(pos)
    frac = pos - idx
    s0 = tbl[idx % _WAVE_TABLE_SIZE]
    s1 = tbl[(idx + 1) % _WAVE_TABLE_SIZE]
    return s0 + (s1 - s0) * frac


def key_to_freq(key):
    """PICO-8 pitch to frequency."""
    return 440.0 * (2 ** ((key - 33) / 12.0))


# ── Parse music data from p8_music_data.h ────────────────────────
def parse_music_data(path):
    with open(path) as f:
        content = f.read()

    # Parse SFX headers
    headers = []
    m = re.search(r'sfx_headers\[64\]\s*=\s*\{(.+?)\};', content, re.DOTALL)
    for line in m.group(1).strip().split('\n'):
        vals = re.findall(r'(\d+)', line)
        if len(vals) >= 4:
            headers.append({
                'speed': int(vals[0]),
                'loop_start': int(vals[1]),
                'loop_end': int(vals[2]),
                'note_count': int(vals[3])
            })

    # Parse SFX notes
    sfx_notes = {}
    current_sfx = -1
    for line in content.split('\n'):
        if '// SFX ' in line:
            try:
                current_sfx = int(line.strip().split('SFX ')[1].split()[0])
                sfx_notes[current_sfx] = []
            except (ValueError, IndexError):
                pass
        elif '0x' in line and current_sfx >= 0 and '{' in line:
            vals = re.findall(r'0x([0-9A-Fa-f]+)', line)
            if len(vals) >= 3:
                sfx_notes[current_sfx].append({
                    'pitch': int(vals[0], 16),
                    'inst': (int(vals[1], 16) >> 4) & 0xF,
                    'vol': int(vals[1], 16) & 0x7,
                    'effect': int(vals[2], 16)
                })

    # Parse music patterns
    patterns = []
    m = re.search(r'music_patterns\[\d+\]\s*=\s*\{(.+?)\};', content, re.DOTALL)
    for line in m.group(1).strip().split('\n'):
        vals = re.findall(r'0x([0-9A-Fa-f]+)', line)
        if len(vals) >= 5:
            patterns.append({
                'flags': int(vals[0], 16),
                'sfx': [int(v, 16) for v in vals[1:5]]
            })

    return headers, sfx_notes, patterns


# ── Sequencer simulation ─────────────────────────────────────────
class Channel:
    def __init__(self):
        self.sfx_id = 0
        self.note_idx = 0
        self.ticks_remaining = 0
        self.active = False
        self.notes_played = 0
        self.is_music = False
        self.pitch = 0
        self.inst = 0
        self.vol = 0
        self.effect = 0
        self.base_vol = 0
        self.base_freq = 0.0
        self.target_freq = 0.0
        self.note_ticks = 0
        self.elapsed = 0
        self.prev_freq = key_to_freq(24)  # PICO-8 default prev_key = 24
        self.prev_vol = 5.0 / 7.0


def simulate_and_synthesize(headers, sfx_notes, patterns, sr=22050, tick_rate=22050.0/183, duration=47.0):
    """Run PICO-8 sequencer simulation and synthesize audio sample-by-sample."""
    n_samples = int(sr * duration)
    audio = np.zeros(n_samples)
    phases = [0.0] * 4

    channels = [Channel() for _ in range(4)]
    music_playing = True
    current_pattern = 0
    loop_start = 0
    pattern_ticks_remaining = 0

    def calc_pattern_duration(pat_idx):
        max_spd = 1
        for s in patterns[pat_idx]['sfx']:
            if s != 0xFF and s < len(headers):
                max_spd = max(max_spd, headers[s]['speed'])
        return 32 * max_spd

    def start_channel(ch_idx, sfx_id, is_music):
        ch = channels[ch_idx]
        ch.sfx_id = sfx_id
        ch.note_idx = 0
        ch.ticks_remaining = 0
        ch.active = True
        ch.notes_played = 0
        ch.is_music = is_music
        ch.elapsed = 0

    def advance_channel(ch_idx):
        ch = channels[ch_idx]
        hdr = headers[ch.sfx_id]

        if ch.is_music and ch.notes_played >= 32:
            ch.active = False
            ch.vol = 0
            return

        if ch.note_idx >= hdr['note_count']:
            if hdr['loop_end'] > hdr['loop_start'] and hdr['loop_end'] <= hdr['note_count']:
                ch.note_idx = hdr['loop_start']
            else:
                ch.active = False
                ch.vol = 0
                return

        note = sfx_notes[ch.sfx_id][ch.note_idx]
        ch.pitch = note['pitch']
        ch.inst = note['inst']
        ch.vol = note['vol']
        ch.effect = note['effect']
        ch.note_ticks = hdr['speed']
        ch.ticks_remaining = hdr['speed'] - 1
        ch.elapsed = 0

        if ch.pitch > 0 and ch.vol > 0:
            current_freq = key_to_freq(ch.pitch)
            ch.base_vol = ch.vol / 7.0

            # PICO-8 slide: FROM previous note TO current note
            if ch.effect == 1:
                ch.base_freq = ch.prev_freq  # start from previous
                ch.target_freq = current_freq  # slide to current
            else:
                ch.base_freq = current_freq
                ch.target_freq = current_freq

            # Fade in starts at 0
            if ch.effect == 4:
                ch.base_vol_start = 0.0
            else:
                ch.base_vol_start = ch.base_vol

            # Update prev for next note
            ch.prev_freq = current_freq
            ch.prev_vol = ch.base_vol
        else:
            if ch.base_freq > 0:
                ch.prev_freq = ch.base_freq
                ch.prev_vol = ch.base_vol
            ch.base_freq = 0.0
            ch.base_vol = 0.0

        ch.note_idx += 1
        ch.notes_played += 1

    def get_channel_state(ch):
        """Get current freq/vol for a channel with effects applied."""
        if not ch.active or ch.base_freq <= 0 or ch.vol <= 0:
            return 0.0, 0.0

        freq = ch.base_freq
        vol = ch.base_vol
        nt = ch.note_ticks
        et = ch.elapsed

        if ch.effect == 1 and nt > 0:  # Slide (prev→current)
            frac = min(et / nt, 1.0)
            freq = ch.base_freq + (ch.target_freq - ch.base_freq) * frac
        elif ch.effect == 2 and nt > 0:  # Vibrato: 7.5 Hz, +1 semitone
            # PICO-8: t = |fmod(7.5 * offset/rate, 1) - 0.5| - 0.25
            phase = (7.5 * et / nt) % 1.0
            t = abs(phase - 0.5) - 0.25  # triangle wave: -0.25 to +0.25
            # freq = lerp(freq, freq * 2^(1/12), t) where t is 0..0.25
            semitone_up = freq * 1.05946  # 2^(1/12)
            freq = freq + (semitone_up - freq) * max(0, t * 4)  # scale t to 0..1
        elif ch.effect == 3 and nt > 0:  # Drop
            freq *= max(0, (nt - et) / nt)
        elif ch.effect == 4 and nt > 0:  # Fade in
            vol = ch.base_vol * min(et / nt, 1.0)
        elif ch.effect == 5 and nt > 0:  # Fade out
            vol = ch.base_vol * max(0, (nt - et) / nt)
        elif ch.effect in (6, 7) and nt > 0:  # Arp: cycle through 4 consecutive notes
            speed = headers[ch.sfx_id]['speed']
            m = (32 if speed <= 8 else 16) // (4 if ch.effect == 6 else 8)
            n = int(m * 7.5 * et / nt)
            base_note = ((ch.note_idx - 1) // 4) * 4  # group of 4
            arp_note = base_note + (n % 4)
            if arp_note < headers[ch.sfx_id]['note_count']:
                arp_pitch = sfx_notes[ch.sfx_id][arp_note]['pitch']
                if arp_pitch > 0:
                    freq = key_to_freq(arp_pitch)

        return freq, vol

    # Start pattern 0
    pat = patterns[0]
    if pat['flags'] & 0x01:
        loop_start = 0
    for ch_idx in range(4):
        if pat['sfx'][ch_idx] != 0xFF:
            start_channel(ch_idx, pat['sfx'][ch_idx], True)
            advance_channel(ch_idx)
    pattern_ticks_remaining = calc_pattern_duration(0) - 1

    # Synthesize sample-by-sample with tick-accurate sequencer
    samples_per_tick = sr / tick_rate
    tick_acc = 0.0

    for sample_idx in range(n_samples):
        tick_acc += 1.0

        # Process ticks
        while tick_acc >= samples_per_tick:
            tick_acc -= samples_per_tick

            # Channel ticks
            for ch_idx in range(4):
                ch = channels[ch_idx]
                if not ch.active:
                    continue
                ch.elapsed += 1
                if ch.ticks_remaining > 0:
                    ch.ticks_remaining -= 1
                else:
                    advance_channel(ch_idx)

            # Pattern timer
            if music_playing:
                if pattern_ticks_remaining > 0:
                    pattern_ticks_remaining -= 1
                else:
                    p = patterns[current_pattern]
                    if p['flags'] & 0x02:
                        current_pattern = loop_start
                    else:
                        current_pattern += 1
                    if current_pattern >= len(patterns) or (patterns[current_pattern]['flags'] & 0x04):
                        music_playing = False
                        for ch_idx in range(4):
                            channels[ch_idx].active = False
                        continue
                    if patterns[current_pattern]['flags'] & 0x01:
                        loop_start = current_pattern
                    nxt = patterns[current_pattern]
                    for ch_idx in range(4):
                        if nxt['sfx'][ch_idx] != 0xFF:
                            start_channel(ch_idx, nxt['sfx'][ch_idx], True)
                            advance_channel(ch_idx)
                        else:
                            channels[ch_idx].active = False
                            channels[ch_idx].vol = 0
                    pattern_ticks_remaining = calc_pattern_duration(current_pattern) - 1

        # Synthesize this sample
        for ch_idx in range(4):
            freq, vol = get_channel_state(channels[ch_idx])
            if freq > 0 and vol > 0:
                audio[sample_idx] += vol * waveform_sample(phases[ch_idx], channels[ch_idx].inst)
                phases[ch_idx] += freq / sr
            else:
                phases[ch_idx] = 0.0

    # Normalize
    peak = np.max(np.abs(audio))
    if peak > 0:
        audio = audio / peak * 0.9
    return audio


def main():
    project = Path(__file__).parent.parent
    spc_dir = project / "spc700"
    ref_dir = project / "reference_audio"

    print("Parsing music data...")
    headers, sfx_notes, patterns = parse_music_data(str(spc_dir / "p8_music_data.h"))
    print(f"  {len(headers)} SFX headers, {len(sfx_notes)} SFX note sets, {len(patterns)} patterns")

    sr = 22050
    duration = 47.0

    print(f"Simulating PICO-8 sequencer at 120.3 Hz, synthesizing {duration}s audio...")
    sim_audio = simulate_and_synthesize(headers, sfx_notes, patterns, sr=sr, tick_rate=22050.0/183, duration=duration)
    print(f"  {len(sim_audio)} samples")

    print("Loading reference mus0.ogg...")
    import librosa
    ref_audio, _ = librosa.load(str(ref_dir / "mus0.ogg"), sr=sr, mono=True, duration=duration)
    print(f"  {len(ref_audio)} samples")

    # Also load the DSP-capture-based synthesis for comparison
    print("Loading DSP-capture synthesis...")
    import wave as wavmod
    spc_wav_path = spc_dir / "spc_synth.wav"
    with wavmod.open(str(spc_wav_path), 'r') as w:
        spc_audio = np.frombuffer(w.readframes(w.getnframes()), dtype=np.int16).astype(float) / 32767.0

    hop = 512
    print("Computing chromagrams...")
    chroma_ref = librosa.feature.chroma_stft(y=ref_audio, sr=sr, hop_length=hop, n_chroma=12)
    chroma_sim = librosa.feature.chroma_stft(y=sim_audio, sr=sr, hop_length=hop, n_chroma=12)
    chroma_spc = librosa.feature.chroma_stft(y=spc_audio, sr=sr, hop_length=hop, n_chroma=12)

    def avg_chroma_sim(ca, cb):
        n = min(ca.shape[1], cb.shape[1])
        sims = []
        for i in range(n):
            r, s = ca[:, i], cb[:, i]
            nr, ns = np.linalg.norm(r), np.linalg.norm(s)
            if nr > 0.01 and ns > 0.01:
                sims.append(np.dot(r, s) / (nr * ns))
        return np.mean(sims) if sims else 0, sims

    sim_score, sim_sims = avg_chroma_sim(chroma_ref, chroma_sim)
    spc_score, spc_sims = avg_chroma_sim(chroma_ref, chroma_spc)

    print(f"\n  Direct simulation vs reference: {sim_score:.3f} (theoretical maximum)")
    print(f"  DSP capture synth vs reference: {spc_score:.3f} (current measurement)")
    print(f"  Gap from capture approx:        {sim_score - spc_score:.3f}")

    fps = sr / hop
    print(f"\n  Per-second comparison (sim | spc):")
    for sec in range(47):
        start = int(sec * fps)
        end = min(int((sec + 1) * fps), min(len(sim_sims), len(spc_sims)))
        sim_vals = np.array(sim_sims[start:end])
        spc_vals = np.array(spc_sims[start:end])
        sv = np.nanmean(sim_vals) if len(sim_vals) > 0 else 0
        cv = np.nanmean(spc_vals) if len(spc_vals) > 0 else 0
        bar_s = "#" * int(sv * 30)
        bar_c = "#" * int(cv * 30)
        print(f"    {sec:2d}-{sec+1:2d}s: sim={sv:.3f} {bar_s}")
        print(f"          spc={cv:.3f} {bar_c}")

    # Save simulation audio
    sim_wav_path = spc_dir / "sim_synth.wav"
    audio_16 = (sim_audio * 32767).astype(np.int16)
    with wavmod.open(str(sim_wav_path), 'w') as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(sr)
        w.writeframes(audio_16.tobytes())
    print(f"\n  Saved simulation audio: {sim_wav_path}")


if __name__ == "__main__":
    main()
