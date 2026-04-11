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

void start_note_v(uint8_t voice, uint16_t pitch, uint8_t instrument, uint8_t volume) {
    uint8_t base = voice << 4;
    uint8_t inst = instrument & 0x07;

    uint8_t adsr1 = (inst == 6) ? 0x8F : 0xFF;
    uint8_t adsr2 = (inst == 6) ? 0xA0 : 0xE0;

    writeDSPREG(base + DSP_VOLL_OFF, volume);
    writeDSPREG(base + DSP_VOLR_OFF, volume);
    writeDSPREG(base + DSP_PL_OFF, pitch & 0xFF);
    writeDSPREG(base + DSP_PH_OFF, (pitch >> 8) & 0x3F);
    writeDSPREG(base + DSP_SRCN_OFF, inst);
    writeDSPREG(base + DSP_ADSR1_OFF, adsr1);
    writeDSPREG(base + DSP_ADSR2_OFF, adsr2);

    // Noise handling
    uint8_t non = readDSPREG(DSP_NON);
    if (inst == 6)
        non |= (1 << voice);
    else
        non &= ~(1 << voice);
    writeDSPREG(DSP_NON, non);

    // Use GAIN mode for fast attack to reduce click on note start
    // Then switch to ADSR after key-on
    writeDSPREG(DSP_KOFF, 0);
    writeDSPREG(DSP_KON, 1 << voice);
}

void stop_note_v(uint8_t voice) {
    uint8_t base = voice << 4;
    writeDSPREG(base + DSP_VOLL_OFF, 0);
    writeDSPREG(base + DSP_VOLR_OFF, 0);
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
// Channels 0-3 for music, 4-7 for SFX
#define NUM_CHANNELS 4
#define TICKS_PER_P8_TICK 1  // T0 at ~128Hz: 1 T0 tick = 1 PICO-8 tick

// Channel state as flat arrays to work around SPC700 compiler struct pointer bug
#define MAX_CH 5  // 0-3 music, 4 SFX
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

uint8_t music_playing = 0;
uint8_t current_pattern = 0;
uint8_t loop_start_pattern = 0;

#define SFX_VOICE 4

uint16_t calc_pattern_duration(const MusicPattern *pat) {
    // Pattern duration = 32 * max(speed of active channels) ticks
    uint8_t max_speed = 1;
    for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
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
}

void advance_channel(uint8_t voice) {
    const SFXHeader *hdr = &sfx_headers[ch_sfx_id[voice]];

    // Music channels: pattern ends after 32 note steps
    if (ch_is_music[voice] && ch_notes_played[voice] >= 32) {
        stop_note_v(voice);
        ch_active[voice] = 0;
        return;
    }

    // Check if SFX note index wraps
    if (ch_current_note[voice] >= hdr->note_count) {
        if (hdr->loop_end > hdr->loop_start && hdr->loop_end <= hdr->note_count) {
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
    ch_ticks_remaining[voice] = ch_note_ticks[voice];
    ch_elapsed_ticks[voice] = 0;

    if (pitch == 0 || vol == 0) {
        // Rest note
        stop_note_v(voice);
    } else {
        uint16_t dsp_pitch = pitch_lut[pitch];
        uint8_t spc_vol = vol * 18;  // PICO-8 0-7 -> 0-126

        ch_base_volume[voice] = spc_vol;
        ch_base_pitch[voice] = dsp_pitch;

        // For slide effect, look ahead to next note's pitch
        if (effect == 1 && ch_current_note[voice] + 1 < hdr->note_count) {
            uint8_t next_pitch = sfx_notes[ch_sfx_id[voice]][ch_current_note[voice] + 1].pitch;
            ch_target_pitch[voice] = (next_pitch > 0) ? pitch_lut[next_pitch] : dsp_pitch;
        } else {
            ch_target_pitch[voice] = dsp_pitch;
        }

        // For fade-in, start at zero volume
        if (effect == 4) {
            spc_vol = 0;
        }

        start_note_v(voice, dsp_pitch, inst, spc_vol);
    }

    ch_current_note[voice]++;
    ch_notes_played[voice]++;
}

void tick_channel_effects(uint8_t voice) {
    if (!ch_active[voice] || ch_note_ticks[voice] == 0) return;
    ch_elapsed_ticks[voice]++;

    switch (ch_effect[voice]) {
        case 1: { // Slide
            if (ch_base_pitch[voice] != ch_target_pitch[voice] && ch_note_ticks[voice] > 0) {
                int32_t diff = (int32_t)ch_target_pitch[voice] - (int32_t)ch_base_pitch[voice];
                int32_t p = (int32_t)ch_base_pitch[voice] + diff * (int32_t)ch_elapsed_ticks[voice] / (int32_t)ch_note_ticks[voice];
                set_voice_pitch(voice, (uint16_t)p);
            }
            break;
        }
        case 2: { // Vibrato
            // Oscillate pitch by +/- ~2% at ~8Hz
            int8_t mod = (ch_elapsed_ticks[voice] & 0x04) ? 2 : -2;
            uint16_t p = ch_base_pitch[voice];
            int16_t delta = (int16_t)(p >> 6) * mod;
            set_voice_pitch(voice, (uint16_t)((int16_t)p + delta));
            break;
        }
        case 3: { // Drop (pitch drops to zero)
            if (ch_note_ticks[voice] > 0) {
                uint16_t p = ch_base_pitch[voice] * (ch_note_ticks[voice] - ch_elapsed_ticks[voice]) / ch_note_ticks[voice];
                set_voice_pitch(voice, p);
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
        case 6: { // Arp Fast (alternate between note and note+4 semitones)
            uint16_t p = (ch_elapsed_ticks[voice] & 0x02)
                ? (uint16_t)((uint32_t)ch_base_pitch[voice] * 5 / 4)  // ~major 3rd up
                : ch_base_pitch[voice];
            set_voice_pitch(voice, p);
            break;
        }
        case 7: { // Arp Slow (same but slower)
            uint16_t p = (ch_elapsed_ticks[voice] & 0x08)
                ? (uint16_t)((uint32_t)ch_base_pitch[voice] * 5 / 4)
                : ch_base_pitch[voice];
            set_voice_pitch(voice, p);
            break;
        }
    }
}

void sequencer_tick(void) {
    // Music channels 0-3
    for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
        if (!ch_active[ch]) continue;

        tick_channel_effects(ch);

        if (ch_ticks_remaining[ch] > 0) {
            ch_ticks_remaining[ch]--;
            continue;
        }
        advance_channel(ch);
    }

    // Standalone SFX on voice 4
    if (ch_active[SFX_VOICE]) {
        tick_channel_effects(SFX_VOICE);
        if (ch_ticks_remaining[SFX_VOICE] > 0) {
            ch_ticks_remaining[SFX_VOICE]--;
        } else {
            advance_channel(SFX_VOICE);
        }
    }

    // Check if all music channels are done -> advance pattern
    if (music_playing) {
        uint8_t any_active = 0;
        for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
            if (ch_active[ch]) { any_active = 1; break; }
        }
        if (!any_active) {
            const MusicPattern *pat = &music_patterns[current_pattern];
            if (pat->flags & 0x02) {
                current_pattern = loop_start_pattern;
            } else {
                current_pattern++;
            }
            if (current_pattern >= 64 || (music_patterns[current_pattern].flags & 0x04)) {
                music_playing = 0;
            }
            if (music_playing) {
                const MusicPattern *next = &music_patterns[current_pattern];
                if (next->flags & 0x01) loop_start_pattern = current_pattern;
                for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
                    if (next->sfx[ch] != 0xFF) {
                        start_sfx_on_channel(ch, next->sfx[ch], 1);
                        advance_channel(ch);
                    } else {
                        ch_active[ch] = 0;
                    }
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

__attribute__((always_inline)) void comms_clear_ports() {
    // Intentionally empty — writing to REG_CONTROL resets timer counters
}

__attribute__((always_inline)) void comms_poll(void) {
    static uint8_t port0_value;
    port0_value = REG_APUIO0;
    REG_APUIO3 = 0x99;

    if (port0_value != comms_v) {
        // Debounce
        static volatile uint8_t dummy;
        dummy = 0;
        for (uint8_t i = 0; i < 5; i++) dummy++;

        static uint8_t stable_port0, stable_port1;
        stable_port0 = REG_APUIO0;
        stable_port1 = REG_APUIO1;

        if (stable_port0 != comms_v) {
            uint8_t cmd = stable_port1;
            uint8_t data = stable_port0;
            comms_v = data;

            debug_putc('c'); debug_putc('m'); debug_putc('d'); debug_putc('=');
            debug_hex8(cmd);
            debug_putc(' ');
            debug_putc('d'); debug_putc('a'); debug_putc('t'); debug_putc('=');
            debug_hex8(data);
            debug_putc('\n');

            if (cmd == CMD_PLAY_SFX_ID) {
                // Play SFX on standalone voice
                start_sfx_on_channel(SFX_VOICE, data, 0);
                advance_channel(SFX_VOICE);
            } else if (cmd == CMD_PLAY_MUSIC) {
                // Start music from pattern
                current_pattern = data;
                music_playing = 1;
                const MusicPattern *pat = &music_patterns[current_pattern];
                if (pat->flags & 0x01) {
                    loop_start_pattern = current_pattern;
                }
                for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
                    if (pat->sfx[ch] != 0xFF) {
                        start_sfx_on_channel(ch, pat->sfx[ch], 1);
                        advance_channel(ch);
                    } else {
                        ch_active[ch] = 0;
                    }
                }
                // pattern timer removed — using all-channels-done approach
            } else if (cmd == CMD_STOP_ALL) {
                music_playing = 0;
                for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
                    ch_active[ch] = 0;
                    stop_note_v(ch);
                }
                ch_active[SFX_VOICE] = 0;
                stop_note_v(SFX_VOICE);
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
    writeDSPREG(DSP_FLG, 0x20);    // Bit 5 = echo disable (stops echo writes to RAM)
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
    writeDSPREG(DSP_MVOL0L, 127);
    writeDSPREG(DSP_MVOL0R, 127);

    // Timer 0: 8000 Hz / 62 ≈ 129 Hz (0.8% fast vs PICO-8's 128Hz — inaudible)
    // Timer 0: 8000/62 ≈ 129 Hz
    REG_T0DIV = 62;
    REG_CONTROL = 0x81;  // enable IPL ROM + TIMER0

    comms_clear_ports();
    debug_println("DSP init done");

    // Auto-start music pattern 0 for standalone .spc testing
    {
        current_pattern = 0;
        music_playing = 1;
        const MusicPattern *pat = &music_patterns[0];
        if (pat->flags & 0x01) loop_start_pattern = 0;
        for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
            if (pat->sfx[ch] != 0xFF) {
                start_sfx_on_channel(ch, pat->sfx[ch], 1);
                advance_channel(ch);
            }
        }
    }
    debug_println("music started");

    // Main loop: spin-wait on T0OUT for each sequencer tick (~129Hz).
    // Uses the same approach as the working delayTicksT0 function.
    // NOTE: comms_poll's comms_clear_ports writes to REG_CONTROL which
    // resets timer counters, so we DON'T call comms_poll in this loop.
    while (REG_MAGIC < 0xFD) {
        // Wait for one T0 tick using the same spin method as delayTicksT0
        {
            unsigned char prev = REG_T0OUT;
            while (prev == REG_T0OUT) {}
        }
        sequencer_tick();
    }
}
