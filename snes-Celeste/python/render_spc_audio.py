#!/usr/bin/env python3
"""Render actual SPC700 DSP audio from BRR samples + DSP capture.

This emulates the S-DSP's audio pipeline:
  BRR decode → Gaussian interpolation → envelope × volume → mix

Produces a WAV that includes all hardware effects (Gaussian filter, BRR
quantization) for direct comparison against the reference OGG.
"""

import csv, re, struct, wave, math
import numpy as np
from pathlib import Path

# S-DSP Gaussian interpolation table (512 entries)
GAUSS = [
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,
   2,   2,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   5,   5,   5,   5,
   6,   6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,  10,  10,  10,
  11,  11,  11,  12,  12,  13,  13,  14,  14,  15,  15,  15,  16,  16,  17,  17,
  18,  19,  19,  20,  20,  21,  21,  22,  23,  23,  24,  24,  25,  26,  27,  27,
  28,  29,  29,  30,  31,  32,  32,  33,  34,  35,  36,  36,  37,  38,  39,  40,
  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,
  58,  59,  60,  61,  62,  64,  65,  66,  67,  69,  70,  71,  73,  74,  76,  77,
  78,  80,  81,  83,  84,  86,  87,  89,  90,  92,  94,  95,  97,  99, 100, 102,
 104, 106, 107, 109, 111, 113, 114, 116, 118, 120, 122, 123, 125, 127, 129, 131,
 133, 134, 136, 138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 160, 162,
 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 189, 191, 193, 195,
 197, 199, 201, 203, 205, 207, 210, 212, 214, 216, 218, 220, 222, 224, 227, 229,
 231, 233, 235, 237, 239, 241, 244, 246, 248, 250, 252, 254, 256, 258, 260, 262,
 265, 267, 269, 271, 273, 275, 277, 279, 281, 284, 286, 288, 290, 292, 294, 296,
 298, 300, 302, 304, 306, 309, 311, 313, 315, 317, 319, 321, 323, 325, 327, 329,
 331, 333, 335, 337, 339, 341, 343, 345, 347, 349, 351, 353, 355, 357, 359, 361,
 363, 365, 367, 369, 371, 373, 375, 377, 379, 381, 383, 385, 387, 389, 391, 393,
 395, 397, 399, 401, 403, 405, 407, 409, 411, 413, 415, 417, 419, 421, 423, 425,
 427, 429, 431, 434, 436, 438, 440, 442, 444, 446, 448, 450, 452, 454, 456, 458,
 460, 462, 464, 466, 468, 470, 472, 474, 476, 478, 480, 482, 484, 486, 488, 490,
 492, 494, 496, 498, 500, 502, 504, 506, 508, 510, 512, 514, 516, 518, 520, 522,
 524, 527, 529, 531, 533, 535, 537, 539, 541, 543, 545, 547, 549, 551, 553, 555,
 557, 559, 561, 563, 565, 567, 569, 571, 573, 575, 577, 579, 581, 583, 585, 587,
 589, 591, 593, 595, 597, 599, 601, 603, 605, 607, 609, 611, 613, 615, 617, 619,
 621, 623, 625, 627, 629, 631, 633, 635, 637, 639, 641, 643, 645, 647, 649, 651,
 653, 655, 657, 659, 661, 663, 665, 667, 669, 671, 673, 675, 677, 679, 681, 683,
 685, 687, 689, 691, 693, 695, 697, 699, 701, 703, 705, 707, 709, 711, 713, 715,
 717, 719, 720, 722, 724, 726, 728, 730, 732, 734, 736, 738, 739, 741, 743, 745,
 747, 749, 751, 752, 754, 756, 758, 760, 762, 763, 765, 767, 769, 771, 772, 774,
 776, 778, 780, 781, 783, 785, 787, 789, 790, 792, 794, 796, 797, 799, 801, 803,
]


def decode_brr_block(data, prev1=0, prev2=0):
    """Decode one 9-byte BRR block into 16 PCM samples."""
    header = data[0]
    shift = (header >> 4) & 0x0F
    filt = (header >> 2) & 0x03
    samples = []

    for i in range(1, 9):
        byte = data[i]
        for nibble_idx in range(2):
            if nibble_idx == 0:
                nib = (byte >> 4) & 0x0F
            else:
                nib = byte & 0x0F
            # Sign extend
            if nib >= 8:
                nib -= 16
            # Shift
            s = nib << shift
            # Clamp before filter
            s = max(-32768, min(32767, s))
            # Apply filter
            if filt == 1:
                s += int(prev1 * 15 / 16)
            elif filt == 2:
                s += int(prev1 * 61 / 32 - prev2 * 15 / 16)
            elif filt == 3:
                s += int(prev1 * 115 / 64 - prev2 * 13 / 16)
            s = max(-32768, min(32767, s))
            samples.append(s)
            prev2 = prev1
            prev1 = s

    is_end = header & 0x01
    is_loop = header & 0x02
    return samples, prev1, prev2, is_end, is_loop


def parse_brr_samples(header_path):
    """Parse BRR sample data from brrSamples.h."""
    with open(header_path) as f:
        content = f.read()

    instruments = {}
    for m in re.finditer(r'sample_(\w+)\[\]\s*=\s*\{([^}]+)\}', content):
        name = m.group(1)
        hex_bytes = re.findall(r'0x([0-9A-Fa-f]+)', m.group(2))
        data = bytes(int(h, 16) for h in hex_bytes)

        # Decode all BRR blocks
        pcm_samples = []
        prev1, prev2 = 0, 0
        loop_point = 0
        offset = 0
        while offset + 9 <= len(data):
            block = data[offset:offset+9]
            samples, prev1, prev2, is_end, is_loop = decode_brr_block(block, prev1, prev2)
            if offset == 0:
                loop_point = 0
            pcm_samples.extend(samples)
            offset += 9
            if is_end:
                break

        instruments[name] = {
            'pcm': np.array(pcm_samples, dtype=np.float64) / 32768.0,
            'loop_point': loop_point,
            'loop_len': len(pcm_samples),
        }

    return instruments


def gauss_interp(samples, frac_pos, sample_idx, n_samples):
    """Apply S-DSP 4-point Gaussian interpolation at fractional position."""
    i = int(frac_pos * 256) & 0xFF

    # Get 4 sample points (with wrapping for looping samples)
    def get_s(idx):
        return samples[idx % n_samples]

    s0 = get_s(sample_idx - 1)
    s1 = get_s(sample_idx)
    s2 = get_s(sample_idx + 1)
    s3 = get_s(sample_idx + 2)

    # Gaussian interpolation: 4-tap FIR
    out = (GAUSS[255 - i] * s0 +
           GAUSS[511 - i] * s1 +
           GAUSS[256 + i] * s2 +
           GAUSS[i] * s3)
    return out / 2048.0


INST_NAMES = ['triangle', 'tilted_saw', 'saw', 'square', 'pulse', 'organ', 'noise', 'phaser']


def render_audio(csv_path, brr_path, output_sr=22050, duration=47.0):
    """Render SPC700 audio from DSP capture + BRR samples."""
    instruments = parse_brr_samples(brr_path)
    dsp_sr = 32000  # S-DSP output rate

    # Read DSP capture
    with open(csv_path) as f:
        rows = list(csv.DictReader(f))

    n_dsp_samples = int(dsp_sr * duration)
    audio = np.zeros(n_dsp_samples, dtype=np.float64)

    # Per-voice state
    phases = [0.0] * 4  # fractional phase accumulator
    master_vol = 43.0 / 128.0  # from main.c

    fps = 60.0
    dsp_samples_per_frame = dsp_sr / fps

    for row in rows:
        frame = int(row['frame'])
        frame_start = int(frame * dsp_samples_per_frame)
        frame_end = min(int((frame + 1) * dsp_samples_per_frame), n_dsp_samples)

        for dsp_idx in range(frame_start, frame_end):
            sample_sum = 0.0

            for v in range(4):
                pitch = int(row[f'v{v}_pitch'])
                env = int(row[f'v{v}_env'])
                vol = int(row[f'v{v}_vol'])
                srcn = int(row[f'v{v}_srcn'])

                if pitch <= 0 or env <= 2 or vol <= 0 or srcn >= 8:
                    phases[v] = 0.0
                    continue

                inst_name = INST_NAMES[srcn]
                if inst_name not in instruments:
                    continue
                inst = instruments[inst_name]
                pcm = inst['pcm']
                n_samp = len(pcm)

                if n_samp == 0:
                    continue

                # Phase accumulation: pitch register * 32000 / 4096 = sample playback rate
                phase_inc = pitch / 4096.0  # samples per DSP clock

                # Get integer and fractional sample position
                int_pos = int(phases[v]) % n_samp
                frac_pos = phases[v] - int(phases[v])

                # Gaussian interpolation
                s = gauss_interp(pcm, frac_pos, int_pos, n_samp)

                # Apply envelope and volume
                amplitude = s * (env / 127.0) * (vol / 127.0)
                sample_sum += amplitude

                # Advance phase
                phases[v] += phase_inc

            # Apply master volume and accumulate
            audio[dsp_idx] = sample_sum * master_vol

    # Clip (matching PICO-8/SPC behavior)
    audio = np.clip(audio, -1.0, 1.0)

    # Resample from 32 kHz to output_sr if needed
    if output_sr != dsp_sr:
        from scipy.signal import resample
        n_out = int(len(audio) * output_sr / dsp_sr)
        audio = resample(audio, n_out)

    return audio, output_sr


def main():
    project = Path(__file__).parent.parent
    spc_dir = project / "spc700"
    ref_dir = project / "reference_audio"

    csv_path = spc_dir / "dsp_capture.csv"
    brr_path = spc_dir / "brrSamples.h"
    ogg_path = ref_dir / "mus0.ogg"

    print("Rendering SPC700 audio (BRR + Gaussian filter)...")
    audio, sr = render_audio(str(csv_path), str(brr_path), output_sr=22050, duration=47.0)
    print(f"  {len(audio)} samples at {sr} Hz")

    # Save rendered audio
    out_path = spc_dir / "spc_rendered.wav"
    audio_16 = (audio * 32767).astype(np.int16)
    with wave.open(str(out_path), 'w') as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(sr)
        w.writeframes(audio_16.tobytes())
    print(f"  Saved: {out_path}")

    # Compare against reference
    print("\nLoading reference...")
    import librosa
    ref, _ = librosa.load(str(ogg_path), sr=sr, mono=True, duration=47.0)
    print(f"  {len(ref)} samples")

    # Also load the math-synthesis version for comparison
    synth_path = spc_dir / "spc_synth.wav"
    with wave.open(str(synth_path), 'r') as w:
        synth = np.frombuffer(w.readframes(w.getnframes()), dtype=np.int16).astype(float) / 32767.0

    hop = 512
    print("\nComputing chromagrams...")
    chroma_ref = librosa.feature.chroma_stft(y=ref, sr=sr, hop_length=hop, n_chroma=12)
    chroma_rend = librosa.feature.chroma_stft(y=audio.astype(np.float32), sr=sr, hop_length=hop, n_chroma=12)
    chroma_synth = librosa.feature.chroma_stft(y=synth.astype(np.float32), sr=sr, hop_length=hop, n_chroma=12)

    def avg_sim(ca, cb):
        n = min(ca.shape[1], cb.shape[1])
        sims = []
        for i in range(n):
            r, s = ca[:, i], cb[:, i]
            nr, ns = np.linalg.norm(r), np.linalg.norm(s)
            if nr > 0.01 and ns > 0.01:
                sims.append(np.dot(r, s) / (nr * ns))
        return np.mean(sims) if sims else 0

    def aligned_sim(ca, cb):
        n = min(ca.shape[1], cb.shape[1])
        sims = []
        for i in range(n):
            r = ca[:, i]
            nr = np.linalg.norm(r)
            if nr < 0.01: continue
            best = -1
            for j in range(max(0, i-3), min(n, i+4)):
                s = cb[:, j]
                ns = np.linalg.norm(s)
                if ns > 0.01:
                    sim = np.dot(r, s) / (nr * ns)
                    if sim > best: best = sim
            if best >= 0: sims.append(best)
        return np.mean(sims) if sims else 0

    rend_strict = avg_sim(chroma_ref, chroma_rend)
    rend_aligned = aligned_sim(chroma_ref, chroma_rend)
    synth_strict = avg_sim(chroma_ref, chroma_synth)
    synth_aligned = aligned_sim(chroma_ref, chroma_synth)

    print(f"\nResults:")
    print(f"  DSP-rendered (BRR+Gauss): strict={rend_strict:.3f}  aligned={rend_aligned:.3f}")
    print(f"  Math synthesis (old):     strict={synth_strict:.3f}  aligned={synth_aligned:.3f}")

    # Spectral comparison
    sc_ref = np.mean(librosa.feature.spectral_centroid(y=ref, sr=sr))
    sc_rend = np.mean(librosa.feature.spectral_centroid(y=audio.astype(np.float32), sr=sr))
    sc_synth = np.mean(librosa.feature.spectral_centroid(y=synth.astype(np.float32), sr=sr))
    print(f"\n  Spectral centroid: ref={sc_ref:.0f} Hz, rendered={sc_rend:.0f} Hz, synth={sc_synth:.0f} Hz")


if __name__ == "__main__":
    main()
