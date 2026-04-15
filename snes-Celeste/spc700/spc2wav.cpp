// Minimal SPC-to-WAV converter using blargg's snes_spc library.
// Renders the actual SPC700 DSP audio (BRR, Gaussian filter, ADSR, mixing).
// Usage: spc2wav input.spc output.wav [duration_seconds]

#define BLARGG_LITTLE_ENDIAN 1
#include "Snes_Spc.h"
#include "SPC_Filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void write_le16(FILE* f, unsigned short v) {
    fputc(v & 0xFF, f);
    fputc((v >> 8) & 0xFF, f);
}

static void write_le32(FILE* f, unsigned int v) {
    fputc(v & 0xFF, f);
    fputc((v >> 8) & 0xFF, f);
    fputc((v >> 16) & 0xFF, f);
    fputc((v >> 24) & 0xFF, f);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input.spc output.wav [seconds]\n", argv[0]);
        return 1;
    }

    const char* spc_path = argv[1];
    const char* wav_path = argv[2];
    double duration = (argc > 3) ? atof(argv[3]) : 47.0;

    // Load SPC file
    FILE* f = fopen(spc_path, "rb");
    if (!f) { fprintf(stderr, "Cannot open %s\n", spc_path); return 1; }
    fseek(f, 0, SEEK_END);
    long spc_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* spc_data = (unsigned char*)malloc(spc_size);
    fread(spc_data, 1, spc_size, f);
    fclose(f);

    // Init SPC emulator
    Snes_Spc spc;
    if (spc.init()) { fprintf(stderr, "SPC init failed\n"); return 1; }

    SPC_Filter filter;
    filter.clear();

    if (spc.load_spc(spc_data, spc_size)) {
        fprintf(stderr, "SPC load failed\n");
        return 1;
    }
    free(spc_data);

    // Render audio
    int sample_rate = 32000;  // SPC700 native rate
    int total_samples = (int)(duration * sample_rate) * 2; // stereo
    short* buf = (short*)malloc(total_samples * sizeof(short));

    fprintf(stderr, "Rendering %.1fs of audio at %d Hz...\n", duration, sample_rate);

    // Render in chunks
    int chunk = 4096;
    int pos = 0;
    while (pos < total_samples) {
        int count = chunk;
        if (pos + count > total_samples) count = total_samples - pos;

        if (spc.play(count, buf + pos)) {
            fprintf(stderr, "SPC play error at sample %d\n", pos);
            break;
        }
        filter.run(buf + pos, count);
        pos += count;
    }

    // Write WAV
    f = fopen(wav_path, "wb");
    if (!f) { fprintf(stderr, "Cannot write %s\n", wav_path); return 1; }

    int data_size = total_samples * sizeof(short);
    int channels = 2;  // stereo
    int bits = 16;
    int byte_rate = sample_rate * channels * (bits / 8);
    int block_align = channels * (bits / 8);

    // RIFF header
    fputs("RIFF", f);
    write_le32(f, 36 + data_size);
    fputs("WAVE", f);

    // fmt chunk
    fputs("fmt ", f);
    write_le32(f, 16);
    write_le16(f, 1);          // PCM
    write_le16(f, channels);
    write_le32(f, sample_rate);
    write_le32(f, byte_rate);
    write_le16(f, block_align);
    write_le16(f, bits);

    // data chunk
    fputs("data", f);
    write_le32(f, data_size);
    fwrite(buf, sizeof(short), total_samples, f);
    fclose(f);

    free(buf);
    fprintf(stderr, "Written %s (%d samples, %.1fs)\n", wav_path, total_samples/2, duration);
    return 0;
}
