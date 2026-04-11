#!/usr/bin/env python3
"""Compare SPC700 DSP capture against PICO-8 sequencer simulation."""

import csv
import math
import re
from pathlib import Path

T0_HZ = 129       # 8000/62
FPS = 60
TICKS_PER_P8_TICK = 1

def pitch_to_dsp(p8_pitch):
    if p8_pitch == 0:
        return 0
    freq = 440.0 * (2 ** ((p8_pitch - 33) / 12.0))
    return max(0, min(0x3FFF, int(freq * 4.096 + 0.5)))

def dsp_to_freq(dsp_pitch):
    if dsp_pitch == 0:
        return 0
    return dsp_pitch * 32000.0 / (4096.0 * 32.0)

def freq_to_note(freq):
    if freq < 1:
        return "---"
    notes = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
    semi = 12 * math.log2(freq / 440.0) + 69
    return f"{notes[int(round(semi)) % 12]}{int(round(semi)) // 12 - 1}"


class P8Channel:
    def __init__(self):
        self.active = False
        self.sfx_id = 0
        self.current_note = 0
        self.ticks_remaining = 0
        self.pitch = 0
        self.volume = 0
        self.instrument = 0
        self.effect = 0
        self.base_volume = 0
        self.base_pitch = 0
        self.note_ticks = 0
        self.elapsed = 0
        self.sounding = False
        self.notes_played = 0
        self.is_music = False

    def start_sfx(self, sfx_id, is_music=False):
        self.sfx_id = sfx_id
        self.current_note = 0
        self.ticks_remaining = 0
        self.active = True
        self.sounding = False
        self.notes_played = 0
        self.is_music = is_music

    def tick(self, sfx_headers, sfx_notes):
        if not self.active:
            return

        # Apply per-tick effects
        if self.sounding and self.note_ticks > 0:
            self.elapsed += 1
            frac = self.elapsed / self.note_ticks if self.note_ticks > 0 else 1
            if self.effect == 5:  # Fade out
                self.volume = int(self.base_volume * max(0, 1.0 - frac))
            elif self.effect == 4:  # Fade in
                self.volume = int(self.base_volume * min(1.0, frac))

        if self.ticks_remaining > 0:
            self.ticks_remaining -= 1
            return

        # Music channels: cap at 32 notes per pattern
        if self.is_music and self.notes_played >= 32:
            self.active = False
            self.sounding = False
            self.pitch = 0
            self.volume = 0
            return

        hdr = sfx_headers[self.sfx_id]
        speed = hdr['speed']
        nc = hdr['note_count']
        ls = hdr['loop_start']
        le = hdr['loop_end']

        if self.current_note >= nc:
            if le > ls and le <= nc:
                self.current_note = ls
            else:
                self.active = False
                self.sounding = False
                self.pitch = 0
                self.volume = 0
                return

        note = sfx_notes[self.sfx_id][self.current_note]
        p = note['pitch']
        inst = (note['inst_vol'] >> 4) & 0xF
        vol = note['inst_vol'] & 0x7
        eff = note['effect']

        self.effect = eff
        self.note_ticks = speed * TICKS_PER_P8_TICK
        self.ticks_remaining = self.note_ticks
        self.elapsed = 0

        if p == 0 or vol == 0:
            self.sounding = False
            self.pitch = 0
            self.volume = 0
        else:
            self.sounding = True
            self.pitch = p
            self.instrument = inst
            self.base_volume = vol * 18
            self.volume = 0 if eff == 4 else self.base_volume  # Fade in starts at 0
            self.base_pitch = pitch_to_dsp(p)

        self.current_note += 1
        self.notes_played += 1


def load_sfx_data(data_path):
    sfx_headers = []
    sfx_notes = []
    music_patterns = []

    with open(data_path) as f:
        content = f.read()

    header_match = re.findall(r'\{\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}', content[:content.index('sfx_notes')])
    for m in header_match:
        sfx_headers.append({'speed': int(m[0]), 'loop_start': int(m[1]), 'loop_end': int(m[2]), 'note_count': int(m[3])})

    note_pattern = re.compile(r'\{\s*0x([0-9A-Fa-f]+),\s*0x([0-9A-Fa-f]+),\s*0x([0-9A-Fa-f]+)\s*\}')
    notes_section = content[content.index('sfx_notes'):]
    sfx_idx = -1
    current_notes = []
    for line in notes_section.split('\n'):
        if '// SFX' in line and '{' in line:
            if sfx_idx >= 0:
                sfx_notes.append(current_notes)
            sfx_idx += 1
            current_notes = []
        for m in note_pattern.finditer(line):
            current_notes.append({'pitch': int(m.group(1), 16), 'inst_vol': int(m.group(2), 16), 'effect': int(m.group(3), 16)})
    if current_notes:
        sfx_notes.append(current_notes)

    pat_section = content[content.index('music_patterns'):]
    pat_pattern = re.compile(r'\{\s*0x([0-9A-Fa-f]+),\s*\{\s*0x([0-9A-Fa-f]+),\s*0x([0-9A-Fa-f]+),\s*0x([0-9A-Fa-f]+),\s*0x([0-9A-Fa-f]+)\s*\}')
    for m in pat_pattern.finditer(pat_section):
        music_patterns.append({'flags': int(m.group(1), 16), 'sfx': [int(m.group(i), 16) for i in range(2, 6)]})

    return sfx_headers, sfx_notes, music_patterns


def simulate(sfx_headers, sfx_notes, music_patterns, num_frames, start_pattern=0):
    channels = [P8Channel() for _ in range(4)]
    frames = []
    cur_pat = start_pattern
    loop_start_pat = start_pattern
    music_active = True

    def start_pattern(pat_idx):
        nonlocal loop_start_pat
        pat = music_patterns[pat_idx]
        if pat['flags'] & 0x01:
            loop_start_pat = pat_idx
        for ch_idx in range(4):
            sfx_id = pat['sfx'][ch_idx]
            if sfx_id != 0xFF:
                channels[ch_idx].start_sfx(sfx_id, is_music=True)
                channels[ch_idx].tick(sfx_headers, sfx_notes)
            else:
                channels[ch_idx].active = False

    start_pattern(cur_pat)

    tick_accum = 0.0
    tpf = float(T0_HZ) / float(FPS)

    for frame in range(num_frames):
        tick_accum += tpf
        while tick_accum >= 1.0:
            tick_accum -= 1.0
            for ch in channels:
                ch.tick(sfx_headers, sfx_notes)

            # Check if all music channels done -> advance pattern
            if music_active and all(not ch.active for ch in channels):
                pat = music_patterns[cur_pat]
                if pat['flags'] & 0x02:  # loop end
                    cur_pat = loop_start_pat
                else:
                    cur_pat += 1
                if cur_pat >= 64 or (music_patterns[cur_pat]['flags'] & 0x04):
                    music_active = False
                else:
                    start_pattern(cur_pat)

        state = []
        for ch in channels:
            dp = pitch_to_dsp(ch.pitch) if ch.sounding else 0
            state.append({
                'pitch': dp,
                'volume': ch.volume if ch.sounding else 0,
                'instrument': ch.instrument if ch.sounding else 0,
                'sounding': ch.sounding,
            })
        frames.append(state)
    return frames


def load_capture(csv_path):
    frames = []
    with open(csv_path) as f:
        reader = csv.DictReader(f)
        for row in reader:
            state = []
            for v in range(4):
                pitch = int(row[f'v{v}_pitch'])
                env = int(row[f'v{v}_env'])
                vol = int(row[f'v{v}_vol'])
                srcn = int(row[f'v{v}_srcn'])
                # A voice is "sounding" if pitch > 0, env > 2, AND vol > 0
                # (vol=0 is used for note-off in the SPC driver)
                sounding = env > 2 and pitch > 0 and vol > 5
                state.append({'pitch': pitch, 'env': env, 'volume': vol, 'srcn': srcn, 'sounding': sounding})
            frames.append(state)
    return frames


def extract_note_events(frames, voice, use_sounding=True):
    """Extract (frame, pitch, sounding) events on note boundaries."""
    events = []
    prev = (0, False)
    for f, state in enumerate(frames):
        s = state[voice]
        if use_sounding:
            cur = (s['pitch'] if s['sounding'] else 0, s['sounding'])
        else:
            cur = (s['pitch'], s['pitch'] > 0)
        if cur != prev:
            events.append((f, cur[0], cur[1]))
            prev = cur
    return events


def compare_voice(expected, actual, voice, tolerance=3):
    """Compare note events for one voice with frame tolerance."""
    exp_ev = extract_note_events(expected, voice)
    act_ev = extract_note_events(actual, voice)

    # Only compare "note on" events (sounding transitions with pitch > 0)
    exp_notes = [(f, p) for f, p, s in exp_ev if s and p > 0]
    act_notes = [(f, p) for f, p, s in act_ev if s and p > 0]

    matched = 0
    mismatched = []
    used_act = set()

    for ef, ep in exp_notes:
        best = None
        for j, (af, ap) in enumerate(act_notes):
            if j in used_act:
                continue
            if ap == ep and abs(af - ef) <= tolerance:
                if best is None or abs(af - ef) < abs(act_notes[best][0] - ef):
                    best = j
        if best is not None:
            matched += 1
            used_act.add(best)
        else:
            mismatched.append((ef, ep))

    # Also find extra notes in actual that weren't in expected
    extra = [(af, ap) for j, (af, ap) in enumerate(act_notes) if j not in used_act]

    return matched, len(exp_notes), len(act_notes), mismatched, extra


def compare_volume(expected, actual, voice, max_frames):
    """Compare volume curves for sounding notes."""
    vol_errors = []
    n = min(len(expected), len(actual), max_frames)
    for f in range(n):
        e = expected[f][voice]
        a = actual[f][voice]
        if e['sounding'] and a['sounding'] and e['pitch'] == a['pitch']:
            err = abs(e['volume'] - a['volume'])
            if err > 5:
                vol_errors.append((f, e['volume'], a['volume'], err))
    return vol_errors


def main():
    script_dir = Path(__file__).parent
    spc_dir = script_dir.parent / "spc700"

    sfx_headers, sfx_notes, music_patterns = load_sfx_data(str(spc_dir / "p8_music_data.h"))
    actual = load_capture(str(spc_dir / "dsp_capture.csv"))

    print(f"Pattern 0: flags=0x{music_patterns[0]['flags']:02X}, sfx={[f'0x{s:02X}' for s in music_patterns[0]['sfx']]}")
    for ch_idx in range(4):
        sid = music_patterns[0]['sfx'][ch_idx]
        if sid != 0xFF:
            h = sfx_headers[sid]
            print(f"  Ch{ch_idx}: SFX {sid} (speed={h['speed']}, notes={h['note_count']}, loop={h['loop_start']}-{h['loop_end']})")

    expected = simulate(sfx_headers, sfx_notes, music_patterns, len(actual), start_pattern=0)

    MAX = 300
    TOLERANCE = 3

    print(f"\n{'='*60}")
    print(f"Note sequence comparison ({MAX} frames, tolerance={TOLERANCE} frames)")
    print(f"{'='*60}")

    total_exp = 0
    total_matched = 0

    for v in range(4):
        matched, n_exp, n_act, missed, extra = compare_voice(expected, actual, v, TOLERANCE)
        total_exp += n_exp
        total_matched += matched
        pct = matched * 100 // max(1, n_exp) if n_exp > 0 else 100
        status = "PERFECT" if pct == 100 else "OK" if pct >= 90 else "NEEDS WORK"
        print(f"\n  Voice {v}: {matched}/{n_exp} notes matched ({pct}%) [{status}]")
        print(f"    Expected {n_exp} note-ons, actual {n_act} note-ons")
        if missed:
            print(f"    Missing ({len(missed)}):")
            for f, p in missed[:8]:
                print(f"      frame {f:3d}: pitch={p:5d} ({freq_to_note(dsp_to_freq(p)):>5s})")
        if extra:
            print(f"    Extra ({len(extra)}):")
            for f, p in extra[:5]:
                print(f"      frame {f:3d}: pitch={p:5d} ({freq_to_note(dsp_to_freq(p)):>5s})")

    overall = total_matched * 100 // max(1, total_exp)
    print(f"\n{'='*60}")
    print(f"OVERALL: {total_matched}/{total_exp} notes matched ({overall}%)")
    print(f"{'='*60}")

    # Volume comparison for matched notes
    print(f"\nVolume accuracy (for matching note-on periods):")
    for v in range(4):
        errs = compare_volume(expected, actual, v, MAX)
        if errs:
            avg_err = sum(e[3] for e in errs) / len(errs)
            max_err = max(e[3] for e in errs)
            print(f"  Voice {v}: {len(errs)} frames with vol error > 5, avg={avg_err:.1f}, max={max_err}")
        else:
            print(f"  Voice {v}: volume matches within tolerance")

    # Frame 5 snapshot
    print(f"\nFrame 5 snapshot:")
    inst_names = ["tri", "tsaw", "saw", "sq", "pls", "org", "noi", "pha"]
    for v in range(4):
        if len(actual) > 5 and len(expected) > 5:
            e = expected[5][v]
            a = actual[5][v]
            ep = e['pitch']
            ap = a['pitch']
            en = freq_to_note(dsp_to_freq(ep)) if ep > 0 else "---"
            an = freq_to_note(dsp_to_freq(ap)) if ap > 0 else "---"
            ei = inst_names[e['instrument']] if e['instrument'] < 8 else "?"
            ai = inst_names[a.get('srcn', 0)] if a.get('srcn', 0) < 8 else "?"
            match = "OK" if ep == ap else "DIFF"
            print(f"  V{v}: exp={ep:5d}({en:>5s},{ei:>4s},v={e['volume']:3d}) act={ap:5d}({an:>5s},{ai:>4s},v={a['volume']:3d},env={a['env']:3d}) [{match}]")


if __name__ == "__main__":
    main()
