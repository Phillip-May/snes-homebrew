// Compare our jump arc to ccleste's OBJ_move remainder system
// Build: mos-sim-clang -Os -o bench/trace_ccleste bench/trace_ccleste.c src/libfixmath/fix16.c -Isrc -Isrc/port -Isrc/libfixmath -Ipython
// Run:   mos-sim bench/trace_ccleste

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#define _WIN32
#define PORT_FUNC_BANK6
#define PORT_FUNC_BANK5
#define PORT_FUNC_BANK4
#define PORT_FUNC_BANK3
#define PORT_DATA_BANK6
#define PORT_DATA_BANK5
#define PORT_DATA_BANK4
#define PORT_DATA_BANK3
#define __NES__
#include "fixedPointSNES.h"
#include "port/port.h"

static void print_int(long v) {
    char buf[12]; int n = 0;
    if (v < 0) { putchar('-'); v = -v; }
    if (v == 0) { putchar('0'); return; }
    while(v) { buf[n++] = '0' + (v % 10); v /= 10; }
    while(n--) putchar(buf[n]);
}

// ccleste-style OBJ_appr
static fix16_t cappr(fix16_t val, fix16_t target, fix16_t amount) {
    fix16_t diff = target - val;
    if (diff > amount) return val + amount;
    if (diff < -amount) return val - amount;
    return target;
}

int main(void) {
    int16_t pos_y = 104;
    fix16_t rem_y = 0;
    fix16_t spd_y = 0;
    fix16_t gravity;
    int i;

    fix16_t maxfall = fix16_from_int(2);
    fix16_t grav_full = fix16_from_float(0.21f);
    fix16_t threshold = fix16_from_float(0.15f);

    fputs("=== ccleste-style jump trace (rem accumulator) ===\n", stdout);
    fputs("Fr | pos.y | spd.y(fix) | rem.y(fix) | moved\n", stdout);

    // Frame 0: jump
    spd_y = fix16_from_int(-2);
    {
        rem_y += spd_y;
        int my = fix16_to_int(rem_y);
        rem_y -= fix16_from_int(my);
        pos_y += my;
        fputs(" 0 | ", stdout); print_int(pos_y);
        fputs(" | ", stdout); print_int(spd_y);
        fputs(" | ", stdout); print_int(rem_y);
        fputs(" | ", stdout); print_int(my);
        putchar('\n');
    }

    for (i = 1; i <= 25; i++) {
        // Gravity
        gravity = grav_full;
        if (spd_y < 0) {
            fix16_t abs_spd = -spd_y;
            if (abs_spd <= threshold) gravity = gravity / 2;
        } else {
            if (spd_y <= threshold) gravity = gravity / 2;
        }
        spd_y = cappr(spd_y, maxfall, gravity);

        // OBJ_move (ccleste style)
        rem_y += spd_y;
        int my = fix16_to_int(rem_y);
        rem_y -= fix16_from_int(my);
        pos_y += my;

        if (i < 10) fputs(" ", stdout);
        print_int(i);
        fputs(" | ", stdout); print_int(pos_y);
        fputs(" | ", stdout); print_int(spd_y);
        fputs(" | ", stdout); print_int(rem_y);
        fputs(" | ", stdout); print_int(my);
        putchar('\n');

        // Land on ground
        if (pos_y >= 104) {
            pos_y = 104;
            spd_y = 0;
            rem_y = 0;
        }
    }

    return 0;
}
