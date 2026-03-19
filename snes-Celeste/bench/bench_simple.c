// Simple cycle benchmark for key operations
// mos-sim-clang -Os -o bench/bench_simple bench/bench_simple.c src/libfixmath/fix16.c -Isrc -Isrc/libfixmath
// mos-sim bench/bench_simple

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fixedPointSNES.h"

// Volatile to prevent constant folding
volatile fixed_t v_a, v_b, v_result;
volatile int16_t v_i16a, v_i16b;
volatile uint8_t v_u8a;
volatile bool v_bool;

// Simulated collision array
static uint8_t collision[256];

static void print_ulong(unsigned long n) {
    char buf[12]; int i = 0;
    if (n == 0) { putchar('0'); return; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (--i >= 0) putchar(buf[i]);
}

int main(void) {
    unsigned long c;
    fixed_t a, b, r;
    int i;

    // Setup
    for (i = 0; i < 256; i++) collision[i] = (i >= 224) ? 0x01 : 0x00;
    v_a = FLOAT_TO_FIXED(2.5f);
    v_b = FLOAT_TO_FIXED(0.75f);

    printf("=== 6502 Cycle Counts ===\n\n");

    // 1. fix16_mul (32-bit multiply)
    a = v_a; b = v_b;
    reset_clock();
    r = fix16_mul(a, b);
    c = clock();
    v_result = r;
    printf("fix16_mul:        "); print_ulong(c); printf(" cyc\n");

    // 2. Shift-based x*0.75
    a = v_a;
    reset_clock();
    r = a - (a >> 2);
    c = clock();
    v_result = r;
    printf("shift *0.75:      "); print_ulong(c); printf(" cyc\n");

    // 3. fix16_mul x*0.5
    a = v_a; b = FLOAT_TO_FIXED(0.5f);
    reset_clock();
    r = fix16_mul(a, b);
    c = clock();
    v_result = r;
    printf("fix16_mul *0.5:   "); print_ulong(c); printf(" cyc\n");

    // 4. Shift x>>1
    a = v_a;
    reset_clock();
    r = a >> 1;
    c = clock();
    v_result = r;
    printf("shift >>1  *0.5:  "); print_ulong(c); printf(" cyc\n");

    // 5. fix16_add
    a = v_a; b = v_b;
    reset_clock();
    r = fix16_add(a, b);
    c = clock();
    v_result = r;
    printf("fix16_add:        "); print_ulong(c); printf(" cyc\n");

    // 6. fix16_sub
    a = v_a; b = v_b;
    reset_clock();
    r = fix16_sub(a, b);
    c = clock();
    v_result = r;
    printf("fix16_sub:        "); print_ulong(c); printf(" cyc\n");

    // 7. INT_TO_FIXED (fix16_from_int)
    v_i16a = 5;
    reset_clock();
    r = INT_TO_FIXED((int)v_i16a);
    c = clock();
    v_result = r;
    printf("INT_TO_FIXED:     "); print_ulong(c); printf(" cyc\n");

    // 8. FIXED_TO_INT
    a = v_a;
    reset_clock();
    v_i16a = FIXED_TO_INT(a);
    c = clock();
    printf("FIXED_TO_INT:     "); print_ulong(c); printf(" cyc\n");

    // 9. approachFixed (the hot function)
    a = FLOAT_TO_FIXED(1.5f); v_a = a;
    b = FLOAT_TO_FIXED(4.0f); v_b = b;
    a = v_a; b = v_b;
    {
        fixed_t step = FLOAT_TO_FIXED(0.21f);
        reset_clock();
        // inline approachFixed logic
        fixed_t diff = b - a;
        if (diff > step) r = a + step;
        else if (diff < -step) r = a - step;
        else r = b;
        c = clock();
    }
    v_result = r;
    printf("approachFixed:    "); print_ulong(c); printf(" cyc\n");

    // 10. Collision check: tileY*16+tileX + array lookup
    v_i16a = 48; v_i16b = 200;
    {
        int16_t x = v_i16a, y = v_i16b;
        reset_clock();
        int tx = x / 16;
        int ty = y / 16;
        if (tx >= 0 && tx < 16 && ty >= 0 && ty < 16) {
            v_bool = collision[ty * 16 + tx] & 0x01;
        }
        c = clock();
    }
    printf("1x tileSolid:     "); print_ulong(c); printf(" cyc\n");

    // 11. 4x collision (OBJ_isSolidAt pattern)
    v_i16a = 48; v_i16b = 200;
    {
        int16_t x = v_i16a, y = v_i16b;
        bool solid = false;
        reset_clock();
        {
            int tx, ty;
            tx = (x) / 16; ty = (y + 8) / 16;
            if (tx >= 0 && tx < 16 && ty >= 0 && ty < 16)
                solid |= (collision[ty * 16 + tx] & 0x01);
            tx = (x + 14) / 16; ty = (y + 8) / 16;
            if (tx >= 0 && tx < 16 && ty >= 0 && ty < 16)
                solid |= (collision[ty * 16 + tx] & 0x01);
            tx = (x) / 16; ty = (y + 16) / 16;
            if (tx >= 0 && tx < 16 && ty >= 0 && ty < 16)
                solid |= (collision[ty * 16 + tx] & 0x01);
            tx = (x + 14) / 16; ty = (y + 16) / 16;
            if (tx >= 0 && tx < 16 && ty >= 0 && ty < 16)
                solid |= (collision[ty * 16 + tx] & 0x01);
        }
        c = clock();
        v_bool = solid;
    }
    printf("4x tileSolid:     "); print_ulong(c); printf(" cyc\n");

    // 12. Division by 16 (used everywhere)
    v_i16a = 200;
    {
        int16_t x = v_i16a;
        reset_clock();
        v_i16b = x / 16;
        c = clock();
    }
    printf("int16 / 16:       "); print_ulong(c); printf(" cyc\n");

    // 13. Shift >>4 (same result for positive values)
    v_i16a = 200;
    {
        int16_t x = v_i16a;
        reset_clock();
        v_i16b = x >> 4;
        c = clock();
    }
    printf("int16 >> 4:       "); print_ulong(c); printf(" cyc\n");

    // 14. Full frame estimate: 29 obj iterations (mostly empty) + player
    printf("\n--- Frame Budget ---\n");
    printf("NES NTSC: 29780 cyc/frame\n");
    printf("Vblank overhead: ~4000 cyc (OAM DMA + PPU writes)\n");
    printf("Available for game logic: ~25780 cyc\n");

    return 0;
}
