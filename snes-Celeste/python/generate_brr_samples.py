#!/usr/bin/env python3
"""Generate single-cycle BRR waveform samples for PICO-8 instruments on SPC700.

Supports ADPCM filter modes 0-3 and configurable samples-per-cycle (default 64).
"""

import math
import struct
import argparse
import numpy as np

NUM_SAMPLES = 32  # samples per cycle (2 BRR blocks = 18 bytes), matches PICO-8


# ── S-DSP Gaussian interpolation filter compensation ──────────────
# The S-DSP uses 4-point Gaussian interpolation when playing BRR samples,
# which acts as a low-pass filter rolling off higher harmonics.
# We pre-boost (pre-emphasize) harmonics to compensate, so the output
# after the Gaussian filter matches the intended PICO-8 waveform.

# Gaussian table (512 entries, from Anomie's S-DSP doc)
_GAUSS = [
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

def _compute_gauss_avg_response():
    """Compute averaged Gaussian filter frequency response."""
    N = 256
    avg_response = np.zeros(N)
    for i in range(256):
        h = np.array([_GAUSS[255-i], _GAUSS[511-i], _GAUSS[256+i], _GAUSS[i]], dtype=float) / 2048.0
        h_padded = np.zeros(N)
        h_padded[:4] = h
        H = np.abs(np.fft.fft(h_padded))
        avg_response += H
    avg_response /= 256
    avg_response /= avg_response[0]  # normalize DC = 1
    return avg_response

_GAUSS_AVG_RESPONSE = _compute_gauss_avg_response()

def compute_gauss_compensation(n_samples, cap=3.0):
    """Compute per-harmonic boost factors to compensate for S-DSP Gaussian filter.
    Returns array of length n_samples//2+1 (one per FFT bin).
    cap: maximum boost factor (default 3.0x = ~10 dB)."""
    avg_response = _GAUSS_AVG_RESPONSE
    N = len(avg_response)
    n_bins = n_samples // 2 + 1
    boost = np.ones(n_bins)
    for k in range(n_bins):
        frac = k / n_samples
        idx = min(int(frac * N), N - 1)
        resp = avg_response[idx]
        if resp > 0.05:
            boost[k] = min(1.0 / resp, cap)
        else:
            boost[k] = cap
    return boost

_GAUSS_BOOST = compute_gauss_compensation(NUM_SAMPLES, cap=3.0)

# Noise generation parameters (CLI overridable).
# seed=100, norm=1.0 found by sweep: +0.007 mus10, +0.003 mus20.
NOISE_PARAMS = {'seed': 100, 'ref_pitch': 35, 'norm': 1.0}

# Per-instrument preemphasis caps, tuned by sweeping vs chroma similarity.
# Re-tuned after noise BRR change (seed=100): simple waveforms prefer even
# lower preemphasis (1.5x) since the tuned noise already adds harmonic content.
INSTRUMENT_PREEMPHASIS_CAPS = {
    0: 1.5,  # triangle — very simple, minimal preemphasis
    1: 4.0,  # tilted_saw — rich harmonics benefit from higher cap
    2: 1.5,  # saw — already bright, low cap reduces overshoot
    3: 1.5,  # square — odd harmonics, low preemphasis
    4: 2.0,  # pulse — low preemphasis matches harmonic structure
    5: 4.0,  # organ — complex multi-harmonic, moderate compensation
    6: 3.0,  # noise — skipped (no preemphasis applied)
    7: 4.0,  # phaser — two detuned triangles, higher cap
}

def apply_gauss_preemphasis(samples_float, cap=3.0):
    """Apply inverse Gaussian filter to waveform (boost attenuated harmonics)."""
    boost = compute_gauss_compensation(NUM_SAMPLES, cap) if cap != 3.0 else _GAUSS_BOOST
    spectrum = np.fft.rfft(samples_float)
    for k in range(len(spectrum)):
        if k < len(boost):
            spectrum[k] *= boost[k]
    compensated = np.fft.irfft(spectrum, n=len(samples_float))
    orig_peak = max(abs(s) for s in samples_float) or 1.0
    comp_peak = np.max(np.abs(compensated)) or 1.0
    compensated = compensated * (orig_peak / comp_peak)
    return list(compensated)

def generate_waveform(instrument_id):
    """Generate float samples using exact PICO-8 waveform formulas from zepto-8/fake-08.
    Source: https://github.com/jtothebell/fake-08/blob/master/source/synth.cpp"""
    n = NUM_SAMPLES
    samples = []

    for i in range(n):
        t = i / n  # phase 0..1

        if instrument_id == 0:  # Triangle
            ret = 1.0 - abs(4.0 * t - 2.0)
            samples.append(ret * 0.5)

        elif instrument_id == 1:  # Tilted Saw
            a = 0.875
            ret = 2.0 * t / a - 1.0 if t < a else 2.0 * (1.0 - t) / (1.0 - a) - 1.0
            samples.append(ret * 0.5)

        elif instrument_id == 2:  # Saw
            ret = t if t < 0.5 else t - 1.0
            samples.append(0.653 * ret)

        elif instrument_id == 3:  # Square
            samples.append(0.25 if t < 0.5 else -0.25)

        elif instrument_id == 4:  # Pulse
            samples.append(0.25 if t < 0.316 else -0.25)

        elif instrument_id == 5:  # Organ
            ret = 3.0 - abs(24.0 * t - 6.0) if t < 0.5 else 1.0 - abs(16.0 * t - 12.0)
            samples.append(ret / 9.0)

        elif instrument_id == 6:  # Noise (Brownian bridge for seamless loop)
            pass  # handled separately below

        elif instrument_id == 7:  # Phaser (two detuned triangles)
            ret = 2.0 - abs(8.0 * t - 4.0)
            t2 = (t * 109.0 / 110.0) % 1.0
            ret += 1.0 - abs(4.0 * t2 - 2.0)
            samples.append(ret / 6.0)

    if instrument_id == 6:
        # Noise: PICO-8 filtered random walk at a reference pitch.
        noise_seed = NOISE_PARAMS.get('seed', 42)
        noise_ref_pitch = NOISE_PARAMS.get('ref_pitch', 35)
        noise_norm = NOISE_PARAMS.get('norm', 0.7)
        np.random.seed(noise_seed)
        ref_freq = 440.0 * (2 ** ((noise_ref_pitch - 33) / 12.0))
        advance_per_sample = ref_freq / 22050.0
        scale = advance_per_sample * 8.858923
        last_sample = 0.0
        raw = []
        for i in range(n):
            noise_val = np.random.uniform(-1, 1)
            last_sample = (last_sample + scale * noise_val) / (1 + scale)
            factor = 1 - noise_ref_pitch / 63.0
            raw.append(last_sample * 1.5 * (1 + factor * factor))
        raw = np.array(raw)
        # Make loopable: crossfade last few samples toward first
        fade = 8
        for i in range(fade):
            t = i / fade
            raw[n - fade + i] = raw[n - fade + i] * (1 - t) + raw[0] * t
        peak = np.max(np.abs(raw)) or 1.0
        samples = list(raw / peak * noise_norm)
        return samples

    # Normalize to [-0.9, 0.9] (leave headroom for mixing)
    peak = max(abs(s) for s in samples) or 1.0
    samples = [s / peak * 0.9 for s in samples]
    return samples


# ── BRR filter coefficients (S-DSP hardware spec) ─────────────────
# Filter 0: out = sample
# Filter 1: out = sample + prev1 * 15/16
# Filter 2: out = sample + prev1 * 61/32 - prev2 * 15/16
# Filter 3: out = sample + prev1 * 115/64 - prev2 * 13/16

FILTER_COEFFS = [
    (0.0,        0.0),       # filter 0
    (15.0/16,    0.0),       # filter 1
    (61.0/32,   -15.0/16),   # filter 2
    (115.0/64,  -13.0/16),   # filter 3
]


def decode_brr_sample(nibble, shift, filt, prev1, prev2):
    """Decode a single BRR nibble exactly as the S-DSP hardware does."""
    # Sign-extend nibble (4-bit signed → int)
    if nibble >= 8:
        nibble -= 16
    sample = nibble << shift
    # Clamp to 15-bit signed range before filter (hardware clips here)
    sample = max(-32768, min(32767, sample))

    c1, c2 = FILTER_COEFFS[filt]
    sample += int(prev1 * c1 + prev2 * c2)
    # Clamp output to 16-bit signed
    sample = max(-32768, min(32767, sample))
    return sample


def encode_brr_block(pcm_16, filt, prev1, prev2, is_last=False, is_loop=False):
    """Encode 16 PCM samples into one 9-byte BRR block using a specific filter.
    Returns (brr_bytes, final_prev1, final_prev2, total_squared_error)."""
    best_shift = 0
    best_error = float('inf')
    best_nibbles = []
    best_p1, best_p2 = prev1, prev2

    c1, c2 = FILTER_COEFFS[filt]

    for shift in range(13):
        nibbles = []
        total_error = 0
        p1, p2 = prev1, prev2

        for si, s in enumerate(pcm_16):
            predicted = int(p1 * c1 + p2 * c2)
            residual = s - predicted
            if shift == 0:
                nib = max(-8, min(7, round(residual)))
            else:
                nib = max(-8, min(7, round(residual / (1 << shift))))
            nibbles.append(nib)
            decoded = decode_brr_sample(nib & 0xF, shift, filt, p1, p2)
            err = (decoded - s) ** 2
            total_error += err
            p2 = p1
            p1 = decoded

        if total_error < best_error:
            best_error = total_error
            best_shift = shift
            best_nibbles = nibbles[:]
            best_p1, best_p2 = p1, p2

    # Header: shift[7:4] | filter[3:2] | loop[1] | end[0]
    flags = 0
    if is_last:
        flags |= 0x01  # end flag
    if is_loop:
        flags |= 0x03  # end + loop flags
    header = (best_shift << 4) | (filt << 2) | flags

    # Pack nibbles: high nibble = even sample, low nibble = odd sample
    data = []
    for i in range(0, 16, 2):
        hi = best_nibbles[i] & 0x0F
        lo = best_nibbles[i + 1] & 0x0F
        data.append((hi << 4) | lo)

    return bytes([header] + data), best_p1, best_p2, best_error


def generate_brr(samples_float, skip_preemphasis=False, preemphasis_cap=3.0):
    """Generate BRR data for a waveform using optimal filter selection per block.

    Applies Gaussian preemphasis to compensate for S-DSP hardware filter.
    Block 0 (loop point) must use filter 0 to ensure clean loop-back.
    Subsequent blocks try all 4 filters and pick the one with lowest error.
    """
    # Apply Gaussian preemphasis to compensate for S-DSP interpolation filter
    if not skip_preemphasis:
        samples_float = apply_gauss_preemphasis(samples_float, cap=preemphasis_cap)
    # Force loop continuity: last sample must equal first sample
    samples_float[-1] = samples_float[0]
    pcm = [int(s * 32767) for s in samples_float]

    n = len(pcm)
    assert n % 16 == 0, f"Sample count {n} must be multiple of 16"
    num_blocks = n // 16

    brr_data = bytearray()
    prev1, prev2 = 0, 0

    for blk in range(num_blocks):
        chunk = pcm[blk * 16 : (blk + 1) * 16]
        is_last = (blk == num_blocks - 1)

        if blk == 0:
            # Loop-point block: must use filter 0 (no state dependency)
            block_bytes, prev1, prev2, _ = encode_brr_block(
                chunk, filt=0, prev1=0, prev2=0,
                is_last=is_last, is_loop=is_last)
            brr_data += block_bytes
        else:
            # Try all 4 filters, pick the one with lowest error
            best_bytes = None
            best_err = float('inf')
            best_state = (prev1, prev2)
            for f in range(4):
                b, p1, p2, err = encode_brr_block(
                    chunk, filt=f, prev1=prev1, prev2=prev2,
                    is_last=is_last, is_loop=is_last)
                if err < best_err:
                    best_err = err
                    best_bytes = b
                    best_state = (p1, p2)
            prev1, prev2 = best_state
            brr_data += best_bytes

    return bytes(brr_data)


def decode_brr(brr_data):
    """Decode BRR data back to PCM samples (simulates S-DSP decode)."""
    num_blocks = len(brr_data) // 9
    samples = []
    prev1, prev2 = 0, 0
    for blk in range(num_blocks):
        header = brr_data[blk * 9]
        shift = (header >> 4) & 0x0F
        filt = (header >> 2) & 0x03
        data = brr_data[blk * 9 + 1 : blk * 9 + 9]
        for byte in data:
            hi = (byte >> 4) & 0x0F
            lo = byte & 0x0F
            s = decode_brr_sample(hi, shift, filt, prev1, prev2)
            prev2 = prev1; prev1 = s
            samples.append(s)
            s = decode_brr_sample(lo, shift, filt, prev1, prev2)
            prev2 = prev1; prev1 = s
            samples.append(s)
    return samples


def generate_brr_iterative(samples_float, preemphasis_cap=3.0, iterations=3):
    """Generate BRR with iterative error correction.
    Encode → decode → compute error → adjust target → re-encode."""
    target = list(samples_float)  # what we want after BRR decode
    current = apply_gauss_preemphasis(target, cap=preemphasis_cap)
    current[-1] = current[0]

    for it in range(iterations):
        pcm = [int(s * 32767) for s in current]
        n = len(pcm)
        num_blocks = n // 16
        brr_data = bytearray()
        prev1, prev2 = 0, 0
        for blk in range(num_blocks):
            chunk = pcm[blk * 16 : (blk + 1) * 16]
            is_last = (blk == num_blocks - 1)
            if blk == 0:
                block_bytes, prev1, prev2, _ = encode_brr_block(
                    chunk, filt=0, prev1=0, prev2=0,
                    is_last=is_last, is_loop=is_last)
                brr_data += block_bytes
            else:
                best_bytes = None; best_err = float('inf')
                best_state = (prev1, prev2)
                for f in range(4):
                    b, p1, p2, err = encode_brr_block(
                        chunk, filt=f, prev1=prev1, prev2=prev2,
                        is_last=is_last, is_loop=is_last)
                    if err < best_err:
                        best_err = err; best_bytes = b
                        best_state = (p1, p2)
                prev1, prev2 = best_state
                brr_data += best_bytes

        if it < iterations - 1:
            # Decode and compute error for next iteration
            decoded = decode_brr(bytes(brr_data))
            decoded_f = [d / 32767.0 for d in decoded]
            # Error = target - decoded (what we missed)
            for i in range(len(current)):
                err = current[i] - decoded_f[i]
                current[i] += err * 0.5  # add half the error back (damped)
            current[-1] = current[0]  # maintain loop continuity

    return bytes(brr_data)


INSTRUMENT_NAMES = [
    "triangle", "tilted_saw", "saw", "square",
    "pulse", "organ", "noise", "phaser"
]


def generate_header():
    """Generate brrSamples.h with all 8 instrument waveforms."""
    lines = []
    lines.append("// Auto-generated BRR waveform samples for PICO-8 instruments")
    lines.append(f"// Each sample is {NUM_SAMPLES} samples/cycle = {NUM_SAMPLES // 16} BRR blocks = {NUM_SAMPLES // 16 * 9} bytes")
    lines.append("// Generated by generate_brr_samples.py")
    lines.append("// Uses ADPCM filter modes 0-3 for optimal fidelity")
    lines.append("")
    lines.append("#include <stdint.h>")
    lines.append("")

    for inst_id in range(8):
        name = INSTRUMENT_NAMES[inst_id]
        waveform = generate_waveform(inst_id)
        cap = INSTRUMENT_PREEMPHASIS_CAPS.get(inst_id, 3.0)
        brr_data = generate_brr(waveform, skip_preemphasis=(inst_id == 6), preemphasis_cap=cap)

        lines.append(f"// Instrument {inst_id}: {name} ({len(brr_data)} bytes)")
        hex_bytes = ", ".join(f"0x{b:02X}" for b in brr_data)
        lines.append(f"static const uint8_t sample_{name}[] = {{ {hex_bytes} }};")
        lines.append("")

    # Pitch lookup table
    # For N-sample waveforms: DSP_pitch = freq * 4096 * N / 32000
    pitch_mult = 4096.0 * NUM_SAMPLES / 32000.0
    lines.append(f"// Pitch lookup table: PICO-8 pitch (0-63) -> 14-bit SPC700 DSP pitch")
    lines.append(f"// For {NUM_SAMPLES}-sample single-cycle waveforms")
    lines.append(f"// Formula: freq = 440 * 2^((pitch - 33) / 12), dsp_pitch = freq * {pitch_mult:.3f}")
    lines.append("static const uint16_t pitch_lut[64] = {")
    row = "    "
    for p in range(64):
        freq = 440.0 * (2 ** ((p - 33) / 12.0))
        dsp_pitch = int(freq * pitch_mult + 0.5)
        dsp_pitch = max(0, min(0x3FFF, dsp_pitch))  # clamp to 14-bit
        row += f"0x{dsp_pitch:04X}, "
        if (p + 1) % 8 == 0:
            lines.append(row)
            row = "    "
    if row.strip():
        lines.append(row)
    lines.append("};")
    lines.append("")

    return "\n".join(lines)


def main(out_path=None):
    import os
    if out_path is None:
        spc_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                               "spc700")
        header_path = os.path.join(spc_dir, "brrSamples.h")
    else:
        header_path = out_path

    content = generate_header()
    with open(header_path, "w") as f:
        f.write(content)
    print(f"Written {header_path}")

    # Print summary
    for inst_id in range(8):
        waveform = generate_waveform(inst_id)
        brr = generate_brr(waveform)
        # Count filter modes used
        filters_used = {}
        for blk_idx in range(len(brr) // 9):
            hdr = brr[blk_idx * 9]
            filt = (hdr >> 2) & 0x03
            filters_used[filt] = filters_used.get(filt, 0) + 1
        filter_str = ", ".join(f"f{f}:{c}" for f, c in sorted(filters_used.items()))
        print(f"  Instrument {inst_id} ({INSTRUMENT_NAMES[inst_id]}): {len(brr)} bytes, filters: {filter_str}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate reusable BRR waveform header for PICO-8-style instruments."
    )
    parser.add_argument(
        "--out",
        default=None,
        help="Output header path (default: repo/spc700/brrSamples.h)",
    )
    parser.add_argument(
        "--cap",
        action="append",
        default=[],
        help="Per-instrument preemphasis cap as INST=CAP (repeatable), e.g. --cap 4=2.0",
    )
    parser.add_argument("--noise-seed", type=int, default=None,
                        help="Noise waveform RNG seed.")
    parser.add_argument("--noise-ref", type=int, default=None,
                        help="Noise reference pitch.")
    parser.add_argument("--noise-norm", type=float, default=None,
                        help="Noise normalization factor.")
    args = parser.parse_args()

    for cap_arg in args.cap:
        inst_str, cap_str = cap_arg.split('=')
        INSTRUMENT_PREEMPHASIS_CAPS[int(inst_str)] = float(cap_str)
    if args.noise_seed is not None:
        NOISE_PARAMS['seed'] = int(args.noise_seed)
    if args.noise_ref is not None:
        NOISE_PARAMS['ref_pitch'] = int(args.noise_ref)
    if args.noise_norm is not None:
        NOISE_PARAMS['norm'] = float(args.noise_norm)
    main(out_path=args.out)
