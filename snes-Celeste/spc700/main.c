// SPC700 sound driver for SNES demake of PICO-8 Celeste
// Data-driven multi-voice sequencer with PICO-8 music/SFX playback

#include <stdint.h>
#include "spc700regs.h"
#include "brrSamples.h"
#include "p8_music_data.h"

void main();
void _start() { main(); }

// ── Debug logging ───────────────────────────────────────────────
#define DBG_BASE     0xFE00
#define DBG_SIZE     0xFE
#define DBG_WPTR     (*(volatile uint16_t*)0xFEFE)

static void debug_init(void) {
    volatile uint8_t *p = (volatile uint8_t*)DBG_BASE;
    for (uint16_t i = 0; i < DBG_SIZE; i++)
        p[i] = 0;
    DBG_WPTR = 0;
}

static void debug_putc(char c) {
    uint16_t wp = DBG_WPTR;
    if (wp < DBG_SIZE) {
        ((volatile uint8_t*)DBG_BASE)[wp] = c;
        DBG_WPTR = wp + 1;
    }
}

void debug_print(const char *s) {
    while (*s)
        debug_putc(*s++);
}

void debug_println(const char *s) {
    debug_print(s);
    debug_putc('\n');
}

static char hex_nibble(uint8_t n) {
    n &= 0xF;
    return n < 10 ? '0' + n : 'A' + (n - 10);
}

void debug_hex8(uint8_t val) {
    debug_putc(hex_nibble(val >> 4));
    debug_putc(hex_nibble(val & 0xF));
}

// ── DSP access ──────────────────────────────────────────────────
void writeDSPREG(uint8_t reg, uint8_t value) {
    REG_DSPADDR = reg;
    REG_DSPDATA = value;
}

uint8_t readDSPREG(uint8_t reg) {
    REG_DSPADDR = reg;
    return REG_DSPDATA;
}

// ── Sample table (8 instruments) ────────────────────────────────
typedef struct {
    uint16_t sampleStart;
    uint16_t sampleLoop;
} BRRSampleEntry;
_Alignas(256) BRRSampleEntry Global_SampleTable[8];

// ── Voice-parameterized DSP functions ───────────────────────────
// DSP registers are at voice*0x10 + offset
// V0=0x00, V1=0x10, V2=0x20, V3=0x30, etc.
#define DSP_VOLL_OFF  0x00
#define DSP_VOLR_OFF  0x01
#define DSP_PL_OFF    0x02
#define DSP_PH_OFF    0x03
#define DSP_SRCN_OFF  0x04
#define DSP_ADSR1_OFF 0x05
#define DSP_ADSR2_OFF 0x06
#define DSP_GAIN_OFF  0x07

uint8_t noise_voices = 0;      // bitmask of voices using hardware noise
uint8_t noise_clock_rate = 31; // DSP FLG low 5 bits

static uint8_t noise_rate_from_p8_pitch(uint8_t p8_pitch) {
    // Affine mapping from PICO-8 note pitch (0..63) to SPC noise rate (0..31).
    int16_t r = ((int16_t)p8_pitch * 4 + 26) >> 3; // ~0.5*p + 3.25
    if (r < 0) r = 0;
    if (r > 31) r = 31;
    return (uint8_t)r;
}

static void apply_noise_state(void) {
    writeDSPREG(DSP_NON, noise_voices);
    writeDSPREG(DSP_FLG, 0x20 | (noise_clock_rate & 0x1F));
}

void start_note_v(uint8_t voice, uint16_t pitch, uint8_t instrument, uint8_t volume) {
    uint8_t base = voice << 4;
    uint8_t inst = instrument & 0x07;
    // Read current SRCN directly from DSP to check if instrument changed.
    uint8_t cur_srcn = readDSPREG(base + DSP_SRCN_OFF);

    writeDSPREG(base + DSP_VOLL_OFF, volume);
    writeDSPREG(base + DSP_VOLR_OFF, volume);
    writeDSPREG(base + DSP_PL_OFF, pitch & 0xFF);
    writeDSPREG(base + DSP_PH_OFF, (pitch >> 8) & 0x3F);
    writeDSPREG(base + DSP_SRCN_OFF, inst);
    writeDSPREG(base + DSP_ADSR1_OFF, 0xFF);
    writeDSPREG(base + DSP_ADSR2_OFF, 0xE0);

    // KON when: instrument changed, or envelope decayed (after GAIN release).
    // Skip KON for same-instrument legato to avoid click.
    uint8_t envx = readDSPREG(base + 0x08);
    if (cur_srcn != inst || envx < 4) {
        writeDSPREG(DSP_KOFF, 0);
        writeDSPREG(DSP_KON, 1 << voice);
    }
}

void stop_note_v(uint8_t voice) {
    uint8_t base = voice << 4;
    // Set volume to 0 immediately, then switch to direct GAIN=0 mode.
    // This is cleaner than exponential release — avoids residual energy
    // bleeding into gaps between notes/patterns.
    writeDSPREG(base + DSP_VOLL_OFF, 0);
    writeDSPREG(base + DSP_VOLR_OFF, 0);
    writeDSPREG(base + DSP_ADSR1_OFF, 0x00);
    writeDSPREG(base + DSP_GAIN_OFF, 0x00);  // direct mode, value=0
}

void set_voice_volume(uint8_t voice, uint8_t volume) {
    uint8_t base = voice << 4;
    writeDSPREG(base + DSP_VOLL_OFF, volume);
    writeDSPREG(base + DSP_VOLR_OFF, volume);
}

void set_voice_pitch(uint8_t voice, uint16_t pitch) {
    uint8_t base = voice << 4;
    writeDSPREG(base + DSP_PL_OFF, pitch & 0xFF);
    writeDSPREG(base + DSP_PH_OFF, (pitch >> 8) & 0x3F);
}

// ── Channel sequencer ───────────────────────────────────────────
// Channels 0-3 for music, 4-7 for standalone SFX
#define NUM_MUSIC_CHANNELS 4
#define SFX_VOICE_MIN 4
#define SFX_VOICE_MAX 7
#define TICKS_PER_P8_TICK 1  // T0 at ~128Hz: 1 T0 tick = 1 PICO-8 tick

// Channel state as flat arrays — idiomatic for SPC700's 8-bit addressing modes
// and avoids LTO miscompilation of struct access through computed indices.
#define MAX_CH 8  // voices 0-7
volatile uint8_t ch_sfx_id[MAX_CH];
volatile uint8_t ch_current_note[MAX_CH];
volatile uint16_t ch_ticks_remaining[MAX_CH];
volatile uint8_t ch_active[MAX_CH];
volatile uint8_t ch_effect[MAX_CH];
volatile uint8_t ch_base_volume[MAX_CH];
volatile uint16_t ch_base_pitch[MAX_CH];
volatile uint16_t ch_target_pitch[MAX_CH];
volatile uint16_t ch_note_ticks[MAX_CH];
volatile uint16_t ch_elapsed_ticks[MAX_CH];
volatile uint8_t ch_notes_played[MAX_CH];
volatile uint8_t ch_is_music[MAX_CH];

// Per-instrument pitch tuning in Q12 (4096 = 1.0x).
// Used to compensate BRR waveform base-pitch drift per instrument.
static const uint16_t inst_pitch_tune_q12[8] = {
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096
};

static uint16_t tune_pitch_for_inst(uint16_t p, uint8_t inst) {
    uint16_t q12 = inst_pitch_tune_q12[inst & 0x07];
    return (uint16_t)(((uint32_t)p * q12) >> 12);
}

// Timer 2 Bresenham prescaler for ~120.47 Hz from 256 Hz base.
// PICO-8: 22050/183 = 120.49 Hz. Bresenham: 256*8/17 = 120.47 Hz.
// Higher-precision 241/512 (120.50 Hz) tested but regresses some tracks
// due to different Bresenham phase pattern vs cycle-accurate DSP timing.
#define BRES_STEP      43
#define BRES_THRESHOLD  91
volatile uint16_t t2_bres_acc = 0;
uint8_t music_playing = 0;
uint8_t current_pattern = 0;
uint8_t loop_start_pattern = 0;
volatile uint16_t pattern_ticks_remaining = 0;

static uint8_t alloc_sfx_voice_desc(void) {
    // Prefer highest free voice first: 7,6,5,4
    for (int8_t v = SFX_VOICE_MAX; v >= SFX_VOICE_MIN; v--) {
        if (!ch_active[(uint8_t)v]) return (uint8_t)v;
    }
    // If all are busy, steal voice 7.
    return SFX_VOICE_MAX;
}

uint16_t calc_pattern_duration(const MusicPattern *pat) {
    // Pattern duration = 32 * max(speed of active channels) ticks
    uint8_t max_speed = 1;
    for (uint8_t ch = 0; ch < NUM_MUSIC_CHANNELS; ch++) {
        if (pat->sfx[ch] != 0xFF) {
            uint8_t spd = sfx_headers[pat->sfx[ch]].speed;
            if (spd > max_speed) max_speed = spd;
        }
    }
    return (uint16_t)32 * max_speed;
}

void start_sfx_on_channel(uint8_t voice, uint8_t sfx_id, uint8_t is_music) {
    if (sfx_id >= 64) return;
    ch_sfx_id[voice] = sfx_id;
    ch_current_note[voice] = 0;
    ch_ticks_remaining[voice] = 0;
    ch_active[voice] = 1;
    ch_effect[voice] = 0;
    ch_elapsed_ticks[voice] = 0;
    ch_notes_played[voice] = 0;
    ch_is_music[voice] = is_music;
    ch_base_pitch[voice] = pitch_lut[24];  // default prev_key=C-2 for slide
}

void advance_channel(uint8_t voice) {
    const SFXHeader *hdr = &sfx_headers[ch_sfx_id[voice]];

    // Music channels: stop after 32 note steps UNLESS the SFX loops.
    // Looping SFXes in music mode continue until the pattern timer expires.
    if (ch_is_music[voice] && ch_notes_played[voice] >= 32) {
        if (hdr->loop_end > hdr->loop_start) {
            ch_current_note[voice] = hdr->loop_start;
            ch_notes_played[voice] = 0;  // reset so this check doesn't re-trigger
        } else {
            stop_note_v(voice);
            ch_active[voice] = 0;
            return;
        }
    }

    // Check if SFX note index wraps (for standalone SFX or within-loop)
    if (ch_current_note[voice] >= hdr->note_count) {
        if (hdr->loop_end > hdr->loop_start) {
            ch_current_note[voice] = hdr->loop_start;
        } else {
            stop_note_v(voice);
            ch_active[voice] = 0;
            return;
        }
    }

    const PackedNote *note = &sfx_notes[ch_sfx_id[voice]][ch_current_note[voice]];
    uint8_t pitch = note->pitch;
    uint8_t inst = (note->inst_vol >> 4) & 0x0F;
    uint8_t vol = note->inst_vol & 0x07;
    uint8_t effect = note->effect;

    ch_effect[voice] = effect;
    ch_note_ticks[voice] = (uint16_t)hdr->speed * TICKS_PER_P8_TICK;
    ch_ticks_remaining[voice] = ch_note_ticks[voice] - 1;
    ch_elapsed_ticks[voice] = 0;

    // Grab previous note's base pitch BEFORE overwriting.
    uint16_t prev_pitch = ch_base_pitch[voice];

    // Sustained noise (no fade-out) → hardware LFSR. Drum noise → BRR.
    if (inst == 6) {
        noise_voices |= (uint8_t)(1u << voice);
        noise_clock_rate = noise_rate_from_p8_pitch(pitch);
    } else {
        noise_voices &= (uint8_t)~(uint8_t)(1u << voice);
    }
    apply_noise_state();

    if (pitch == 0 || vol == 0) {
        // Rest note
        stop_note_v(voice);
    } else {
        uint16_t dsp_pitch = tune_pitch_for_inst(pitch_lut[pitch], inst);
        // Exact PICO-8 volume: vol * 128/7 ≈ vol * 73 >> 2
        uint8_t spc_vol = (uint8_t)(((uint16_t)vol * 73) >> 2);
        if (spc_vol > 127) spc_vol = 127;
        ch_base_volume[voice] = spc_vol;

        if (effect == 1 && prev_pitch > 0) {
            ch_base_pitch[voice] = prev_pitch;
            ch_target_pitch[voice] = dsp_pitch;
            start_note_v(voice, prev_pitch, inst, spc_vol);
        } else {
            // Arp: base_pitch = first group note (exact from SFX data),
            // target_pitch = base * 3/2 (perfect fifth approximation).
            // Only one SFX data read to stay under the LTO complexity limit.
            if ((effect == 6 || effect == 7) && ch_current_note[voice] < hdr->note_count) {
                uint8_t arp_base = ch_current_note[voice] & 0xFC;
                uint8_t arp_p = sfx_notes[ch_sfx_id[voice]][arp_base].pitch;
                if (arp_p > 0 && arp_p < 64) {
                    dsp_pitch = tune_pitch_for_inst(pitch_lut[arp_p], inst);
                }
            }
            ch_base_pitch[voice] = dsp_pitch;
            ch_target_pitch[voice] = dsp_pitch + (dsp_pitch >> 1); // ×3/2 fifth up
            if (effect == 4) spc_vol = 0;
            start_note_v(voice, dsp_pitch, inst, spc_vol);
        }
    }

    ch_current_note[voice]++;
    ch_notes_played[voice]++;
}

void tick_channel_effects(uint8_t voice) {
    if (!ch_active[voice] || ch_note_ticks[voice] == 0) return;
    ch_elapsed_ticks[voice]++;

    switch (ch_effect[voice]) {
        case 1: { // Slide — fast slide (complete in first half of note)
            if (ch_base_pitch[voice] != ch_target_pitch[voice] && ch_note_ticks[voice] > 0) {
                uint16_t bp = ch_base_pitch[voice];
                uint16_t tp = ch_target_pitch[voice];
                uint16_t et = ch_elapsed_ticks[voice];
                uint16_t ht = ch_note_ticks[voice] >> 1;  // half note duration
                if (ht == 0) ht = 1;
                if (et >= ht) {
                    // Past halfway: stay at target pitch
                    ch_base_pitch[voice] = tp;
                    set_voice_pitch(voice, tp);
                } else {
                    // First half: slide
                    if (tp > bp) {
                        set_voice_pitch(voice, bp + (uint16_t)((uint32_t)(tp - bp) * et / ht));
                    } else {
                        set_voice_pitch(voice, bp - (uint16_t)((uint32_t)(bp - tp) * et / ht));
                    }
                }
            }
            break;
        }
        case 2: { // Vibrato — ~7.5 Hz, ±1.5% of pitch
            uint16_t p = ch_base_pitch[voice];
            uint16_t wobble = p >> 6;  // ~1.5% of pitch
            if (ch_elapsed_ticks[voice] & 0x08) {
                set_voice_pitch(voice, p + wobble);
            } else {
                set_voice_pitch(voice, p - wobble);
            }
            break;
        }
        case 3: { // Drop (pitch drops to zero)
            if (ch_note_ticks[voice] > 0 && ch_elapsed_ticks[voice] <= ch_note_ticks[voice]) {
                uint32_t p = (uint32_t)ch_base_pitch[voice] * (ch_note_ticks[voice] - ch_elapsed_ticks[voice]) / ch_note_ticks[voice];
                set_voice_pitch(voice, (uint16_t)p);
            }
            break;
        }
        case 4: { // Fade In
            if (ch_note_ticks[voice] > 0 && ch_elapsed_ticks[voice] <= ch_note_ticks[voice]) {
                uint8_t v = (uint8_t)((uint16_t)ch_base_volume[voice] * ch_elapsed_ticks[voice] / ch_note_ticks[voice]);
                set_voice_volume(voice, v);
            }
            break;
        }
        case 5: { // Fade Out
            if (ch_note_ticks[voice] > 0 && ch_elapsed_ticks[voice] <= ch_note_ticks[voice]) {
                uint8_t v = (uint8_t)((uint16_t)ch_base_volume[voice] * (ch_note_ticks[voice] - ch_elapsed_ticks[voice]) / ch_note_ticks[voice]);
                set_voice_volume(voice, v);
            }
            break;
        }
        case 6: { // Arp Fast — alternate base_pitch and target_pitch (set in advance)
            uint16_t p = (ch_elapsed_ticks[voice] & 0x08)
                ? ch_target_pitch[voice] : ch_base_pitch[voice];
            set_voice_pitch(voice, p);
            break;
        }
        case 7: { // Arp Slow
            uint16_t p = (ch_elapsed_ticks[voice] & 0x10)
                ? ch_target_pitch[voice] : ch_base_pitch[voice];
            set_voice_pitch(voice, p);
            break;
        }
    }
}

void sequencer_tick(void) {
    // Music channels 0-3
    for (uint8_t ch = 0; ch < NUM_MUSIC_CHANNELS; ch++) {
        if (!ch_active[ch]) continue;

        tick_channel_effects(ch);

        if (ch_ticks_remaining[ch] > 0) {
            ch_ticks_remaining[ch]--;
            continue;
        }
        advance_channel(ch);
    }

    // Standalone SFX on voices 4-7
    for (uint8_t ch = SFX_VOICE_MIN; ch <= SFX_VOICE_MAX; ch++) {
        if (ch_active[ch]) {
            tick_channel_effects(ch);
            if (ch_ticks_remaining[ch] > 0) {
                ch_ticks_remaining[ch]--;
            } else {
                advance_channel(ch);
            }
        }
    }

    // Pattern timer: advance when max(speed)*32 ticks expire
    if (music_playing) {
        if (pattern_ticks_remaining > 0) {
            pattern_ticks_remaining--;
        } else {
            const MusicPattern *pat = &music_patterns[current_pattern];
            if (pat->flags & 0x02) {
                current_pattern = loop_start_pattern;
            } else {
                current_pattern++;
            }
            if (current_pattern >= 64 || (music_patterns[current_pattern].flags & 0x04)) {
                music_playing = 0;
                for (uint8_t ch = 0; ch < NUM_MUSIC_CHANNELS; ch++) {
                    ch_active[ch] = 0;
                    stop_note_v(ch);
                }
            }
            if (music_playing) {
                const MusicPattern *next = &music_patterns[current_pattern];
                if (next->flags & 0x01) loop_start_pattern = current_pattern;
                uint8_t any = 0;
                for (uint8_t ch = 0; ch < NUM_MUSIC_CHANNELS; ch++) {
                    if (next->sfx[ch] != 0xFF) {
                        start_sfx_on_channel(ch, next->sfx[ch], 1);
                        advance_channel(ch);
                        any = 1;
                    } else {
                        ch_active[ch] = 0;
                        stop_note_v(ch);
                    }
                }
                if (!any) {
                    music_playing = 0;
                } else {
                    pattern_ticks_remaining = calc_pattern_duration(next) - 1;
                    // Dynamic master volume: 127/active_channels.
                    // PICO-8 sums and clips; we scale to prevent clipping.
                    // Fewer channels → higher volume → louder bass.
                    uint8_t active_count = 0;
                    for (uint8_t i = 0; i < NUM_MUSIC_CHANNELS; i++)
                        if (ch_active[i]) active_count++;
                    if (active_count == 0) active_count = 1;
                    uint8_t mvol = 127 / active_count;
                    writeDSPREG(DSP_MVOL0L, mvol);
                    writeDSPREG(DSP_MVOL0R, mvol);
                    // Noise mask/rate is maintained per-note in advance_channel().
                    apply_noise_state();
                }
            }
        }
    }
}

// ── Communication ───────────────────────────────────────────────
#define CMD_PLAY_SFX    0x01
#define CMD_STOP_ALL    0x03
#define CMD_PLAY_SFX_ID 0x04
#define CMD_PLAY_MUSIC  0x06

volatile uint8_t comms_v = 0;
volatile uint8_t comms_cmd_v = 0xFF;

void comms_clear_ports() {
    // Intentionally empty — writing to REG_CONTROL resets timer counters
}

void comms_poll(void) {
    static uint8_t port0_value;
    static uint8_t port1_value;
    port0_value = REG_APUIO0;
    port1_value = REG_APUIO1;
    REG_APUIO3 = 0x99;

    // Deduplicate on the full (cmd,data) tuple, not data only.
    // This allows PLAY_SFX with id==current music pattern without being ignored.
    if (port0_value != comms_v || port1_value != comms_cmd_v) {
        // Debounce
        static volatile uint8_t dummy;
        dummy = 0;
        for (uint8_t i = 0; i < 5; i++) dummy++;

        static uint8_t stable_port0, stable_port1;
        stable_port0 = REG_APUIO0;
        stable_port1 = REG_APUIO1;

        if (stable_port0 != comms_v || stable_port1 != comms_cmd_v) {
            uint8_t cmd = stable_port1;
            uint8_t data = stable_port0;
            comms_v = data;
            comms_cmd_v = cmd;

            debug_putc('c'); debug_putc('m'); debug_putc('d'); debug_putc('=');
            debug_hex8(cmd);
            debug_putc(' ');
            debug_putc('d'); debug_putc('a'); debug_putc('t'); debug_putc('=');
            debug_hex8(data);
            debug_putc('\n');

            if (cmd == CMD_PLAY_SFX_ID || cmd == CMD_PLAY_SFX) {
                // Play SFX on first free standalone voice (7..4)
                {
                    uint8_t sfx_voice = alloc_sfx_voice_desc();
                    start_sfx_on_channel(sfx_voice, data, 0);
                    advance_channel(sfx_voice);
                }
            } else if (cmd == CMD_PLAY_MUSIC) {
                // Start music from pattern
                current_pattern = data;
                music_playing = 1;
                const MusicPattern *pat = &music_patterns[current_pattern];
                if (pat->flags & 0x01) {
                    loop_start_pattern = current_pattern;
                }
                for (uint8_t ch = 0; ch < NUM_MUSIC_CHANNELS; ch++) {
                    if (pat->sfx[ch] != 0xFF) {
                        start_sfx_on_channel(ch, pat->sfx[ch], 1);
                        advance_channel(ch);
                    } else {
                        ch_active[ch] = 0;
                        stop_note_v(ch);
                    }
                }
                pattern_ticks_remaining = calc_pattern_duration(pat) - 1;
                // Dynamic master volume
                {
                    uint8_t ac = 0;
                    for (uint8_t i = 0; i < NUM_MUSIC_CHANNELS; i++) {
                        if (!ch_active[i]) continue;
                        ac++;
                    }
                    if (ac == 0) ac = 1;
                    writeDSPREG(DSP_MVOL0L, 127 / ac);
                    writeDSPREG(DSP_MVOL0R, 127 / ac);
                    apply_noise_state();
                }
            } else if (cmd == CMD_STOP_ALL) {
                music_playing = 0;
                for (uint8_t ch = 0; ch < NUM_MUSIC_CHANNELS; ch++) {
                    ch_active[ch] = 0;
                    stop_note_v(ch);
                }
                for (uint8_t ch = SFX_VOICE_MIN; ch <= SFX_VOICE_MAX; ch++) {
                    ch_active[ch] = 0;
                    stop_note_v(ch);
                }
                noise_voices = 0;
                apply_noise_state();
            }

            REG_APUIO0 = comms_v;
        }
    }
    comms_clear_ports();
}

// ── Main ────────────────────────────────────────────────────────
void main() {
    debug_init();
    debug_println("SPC700 boot");

    // Init sample table — 8 instruments
    Global_SampleTable[0].sampleStart = (uint16_t)&sample_triangle;
    Global_SampleTable[0].sampleLoop  = (uint16_t)&sample_triangle;
    Global_SampleTable[1].sampleStart = (uint16_t)&sample_tilted_saw;
    Global_SampleTable[1].sampleLoop  = (uint16_t)&sample_tilted_saw;
    Global_SampleTable[2].sampleStart = (uint16_t)&sample_saw;
    Global_SampleTable[2].sampleLoop  = (uint16_t)&sample_saw;
    Global_SampleTable[3].sampleStart = (uint16_t)&sample_square;
    Global_SampleTable[3].sampleLoop  = (uint16_t)&sample_square;
    Global_SampleTable[4].sampleStart = (uint16_t)&sample_pulse;
    Global_SampleTable[4].sampleLoop  = (uint16_t)&sample_pulse;
    Global_SampleTable[5].sampleStart = (uint16_t)&sample_organ;
    Global_SampleTable[5].sampleLoop  = (uint16_t)&sample_organ;
    Global_SampleTable[6].sampleStart = (uint16_t)&sample_noise;
    Global_SampleTable[6].sampleLoop  = (uint16_t)&sample_noise;
    Global_SampleTable[7].sampleStart = (uint16_t)&sample_phaser;
    Global_SampleTable[7].sampleLoop  = (uint16_t)&sample_phaser;

    // DSP init — CRITICAL: disable echo FIRST to prevent echo buffer
    // from overwriting zero page RAM. The echo buffer writes to ESA*256
    // for EDL*2048 bytes. Default ESA=0 means it writes starting at 0x0000.
    // FLG: bit5=echo disable, bits0-4=noise clock rate.
    writeDSPREG(DSP_FLG, 0x20 | (noise_clock_rate & 0x1F));
    writeDSPREG(0x7D, 0x00);       // EDL = 0 (echo delay length = 0)
    writeDSPREG(0x6D, 0x00);       // ESA = 0 (echo start, doesn't matter since EDL=0)
    writeDSPREG(0x0D, 0x00);       // EFB = 0 (echo feedback = 0)

    writeDSPREG(DSP_KON, 0x00);
    writeDSPREG(DSP_KOFF, 0xFF);   // Stop all voices
    writeDSPREG(DSP_PMON, 0x00);
    writeDSPREG(DSP_NON, 0x00);
    writeDSPREG(DSP_EON, 0x00);
    writeDSPREG(DSP_EVOLL, 0x00);
    writeDSPREG(DSP_EVOLR, 0x00);
    writeDSPREG(DSP_DIR, ((uint16_t)&Global_SampleTable) >> 8);
    writeDSPREG(DSP_KOFF, 0x00);
    // Master volume: 43 prevents clipping with 3 simultaneous voices.
    // Per-voice max=126 (PICO-8 vol 7), 3 voices: 126*3*43/128 = 127 (full scale, no clip).
    // PICO-8 sums+clamps; our master volume achieves the same by scaling the sum.
    writeDSPREG(DSP_MVOL0L, 43);
    writeDSPREG(DSP_MVOL0R, 43);

    // Timer 2: 64000 Hz base / DIV=250 = 256 Hz exactly.
    // Software /2 prescaler gives exact 128 Hz (PICO-8 tick rate).
    // Timer 0 was unusable: 8000/128 = 62.5 (not integer), DIV=63 measured 124 Hz.
    REG_T2DIV = 251;
    REG_CONTROL = 0x04;  // enable TIMER2 (keep IPL ROM disabled in runtime)

    comms_clear_ports();
    debug_println("DSP init done");

    // Auto-start music for standalone .spc testing
    {
        current_pattern = 0;  // patched by compare_all_tracks for mus10/20/30/40
        music_playing = 1;
        const MusicPattern *pat = &music_patterns[current_pattern];
        if (pat->flags & 0x01) loop_start_pattern = current_pattern;
        for (uint8_t ch = 0; ch < NUM_MUSIC_CHANNELS; ch++) {
            if (pat->sfx[ch] != 0xFF) {
                start_sfx_on_channel(ch, pat->sfx[ch], 1);
                advance_channel(ch);
            }
        }
        pattern_ticks_remaining = calc_pattern_duration(pat) - 1;
        // Dynamic master volume based on active channels
        uint8_t ac = 0;
        for (uint8_t i = 0; i < NUM_MUSIC_CHANNELS; i++)
            if (ch_active[i]) ac++;
        if (ac == 0) ac = 1;
        writeDSPREG(DSP_MVOL0L, 127 / ac);
        writeDSPREG(DSP_MVOL0R, 127 / ac);
        apply_noise_state();
    }
    debug_println("music started");

    // Main loop: Timer 2 at 256 Hz with Bresenham prescaler → ~121 Hz.
    // This matches the ccleste reference audio tempo (mus0.ogg).
    // Accumulate BRES_STEP per T2 tick, fire when acc >= BRES_THRESHOLD.
    while (1) {
        uint8_t t = REG_T2OUT;
        if (t) {
            t2_bres_acc += (uint16_t)t * BRES_STEP;
            while (t2_bres_acc >= BRES_THRESHOLD) {
                t2_bres_acc -= BRES_THRESHOLD;
                comms_poll();
                sequencer_tick();
            }
        }
    }
}
