/*
 * PICO-8 SFX Playback Code
 * Generated from PICO-8 cartridge data
 * 
 * Usage: Call play_sfx_XX() to play a specific sound effect
 *        Call play_music_XX() to play a specific music pattern
 */

#include <stdint.h>
#include <stdbool.h>

// Individual SFX Playback Functions
void play_sfx_00() {
    // SFX 00 - 18 notes, Speed: 2
    // Step  0: F#4 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x171F);
    delayTicksT0(19); // 9500.0μs at 2KHz (converted from 608 T2 ticks)
    stop_note();
    
    // Step  1: B2 (Pulse) Vol:7 Effect:0
    set_instrument(4);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x07B7);
    delayTicksT0(57); // 28500.0μs at 2KHz (converted from 1824 T2 ticks)
    stop_note();
    
    // Step  2: B3 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0F6F);
    delayTicksT0(28); // 14000.0μs at 2KHz (converted from 912 T2 ticks)
    stop_note();
    
    // Step  3: F2 (Pulse) Vol:7 Effect:0
    set_instrument(4);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(80); // 40000.0μs at 2KHz (converted from 2580 T2 ticks)
    stop_note();
    
    // Step  4: F#3 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0B8F);
    delayTicksT0(38); // 19000.0μs at 2KHz (converted from 1216 T2 ticks)
    stop_note();
    
    // Step  5: B1 (Pulse) Vol:7 Effect:0
    set_instrument(4);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03DB);
    delayTicksT0(114); // 57000.0μs at 2KHz (converted from 3648 T2 ticks)
    stop_note();
    
    // Step  6: D#3 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(45); // 22500.0μs at 2KHz (converted from 1448 T2 ticks)
    stop_note();
    
    // Step  7: G1 (Pulse) Vol:7 Effect:0
    set_instrument(4);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: B2 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x07B7);
    delayTicksT0(57); // 28500.0μs at 2KHz (converted from 1824 T2 ticks)
    stop_note();
    
    // Step  9: F1 (Pulse) Vol:7 Effect:0
    set_instrument(4);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: F#2 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x05C7);
    delayTicksT0(76); // 38000.0μs at 2KHz (converted from 2434 T2 ticks)
    stop_note();
    
    // Step 11: D1 (Pulse) Vol:7 Effect:0
    set_instrument(4);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x024B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: D2 (Square) Vol:6 Effect:0
    set_instrument(3);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0496);
    delayTicksT0(95); // 47500.0μs at 2KHz (converted from 3068 T2 ticks)
    stop_note();
    
    // Step 13: C1 (Pulse) Vol:6 Effect:0
    set_instrument(4);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: A#1 (Square) Vol:5 Effect:0
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3866 T2 ticks)
    stop_note();
    
    // Step 15: G#0 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x019F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: F#1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x02E3);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: F0 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_01() {
    // SFX 01 - 4 notes, Speed: 2
    // Step  0: F1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: G1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: D2 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0496);
    delayTicksT0(95); // 47500.0μs at 2KHz (converted from 3068 T2 ticks)
    stop_note();
    
    // Step  3: C3 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(53); // 26500.0μs at 2KHz (converted from 1722 T2 ticks)
    stop_note();
    
}

void play_sfx_02() {
    // SFX 02 - 4 notes, Speed: 3
    // Step  0: C#1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x022A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: E1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0293);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: A#1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: A#2 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2898 T2 ticks)
    stop_note();
    
}

void play_sfx_03() {
    // SFX 03 - 25 notes, Speed: 2
    // Step  0: F#0 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0171);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: G#0 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x019F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: A0 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x01B8);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: B0 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x01ED);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: A#2 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1932 T2 ticks)
    stop_note();
    
    // Step  5: F#3 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0B8F);
    delayTicksT0(38); // 19000.0μs at 2KHz (converted from 1216 T2 ticks)
    stop_note();
    
    // Step  6: C5 (Noise) Vol:5 Effect:0
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x2101);
    delayTicksT0(13); // 6500.0μs at 2KHz (converted from 430 T2 ticks)
    stop_note();
    
    // Step  7: B4 (Noise) Vol:5 Effect:0
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x1EDE);
    delayTicksT0(14); // 7000.0μs at 2KHz (converted from 456 T2 ticks)
    stop_note();
    
    // Step  8: B4 (Noise) Vol:5 Effect:0
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x1EDE);
    delayTicksT0(14); // 7000.0μs at 2KHz (converted from 456 T2 ticks)
    stop_note();
    
    // Step  9: A4 (Noise) Vol:5 Effect:0
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x1B80);
    delayTicksT0(16); // 8000.0μs at 2KHz (converted from 512 T2 ticks)
    stop_note();
    
    // Step 10: F#4 (Noise) Vol:5 Effect:0
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x171F);
    delayTicksT0(19); // 9500.0μs at 2KHz (converted from 608 T2 ticks)
    stop_note();
    
    // Step 11: D4 (Noise) Vol:5 Effect:0
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x125A);
    delayTicksT0(23); // 11500.0μs at 2KHz (converted from 766 T2 ticks)
    stop_note();
    
    // Step 12: A3 (Noise) Vol:5 Effect:0
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0DC0);
    delayTicksT0(32); // 16000.0μs at 2KHz (converted from 1024 T2 ticks)
    stop_note();
    
    // Step 13: E3 (Noise) Vol:5 Effect:0
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0A4D);
    delayTicksT0(42); // 21000.0μs at 2KHz (converted from 1366 T2 ticks)
    stop_note();
    
    // Step 14: C3 (Noise) Vol:4 Effect:0
    set_instrument(6);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(53); // 26500.0μs at 2KHz (converted from 1722 T2 ticks)
    stop_note();
    
    // Step 15: A2 (Noise) Vol:4 Effect:0
    set_instrument(6);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x06E0);
    delayTicksT0(64); // 32000.0μs at 2KHz (converted from 2048 T2 ticks)
    stop_note();
    
    // Step 16: F2 (Noise) Vol:4 Effect:0
    set_instrument(6);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(80); // 40000.0μs at 2KHz (converted from 2580 T2 ticks)
    stop_note();
    
    // Step 17: D2 (Noise) Vol:4 Effect:0
    set_instrument(6);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0496);
    delayTicksT0(95); // 47500.0μs at 2KHz (converted from 3068 T2 ticks)
    stop_note();
    
    // Step 18: A#1 (Noise) Vol:3 Effect:0
    set_instrument(6);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3866 T2 ticks)
    stop_note();
    
    // Step 19: F1 (Noise) Vol:3 Effect:0
    set_instrument(6);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: D1 (Noise) Vol:3 Effect:0
    set_instrument(6);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x024B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: B0 (Noise) Vol:2 Effect:0
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x01ED);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: G0 (Noise) Vol:2 Effect:0
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: F0 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: D#0 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0137);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_04() {
    // SFX 04 - 17 notes, Speed: 4
    // Step  0: D#1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F#2 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x05C7);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F#1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x02E3);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: A#2 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step  4: B1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03DB);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: D3 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x092D);
    delayTicksT0(95); // 47500.0μs at 2KHz (converted from 3068 T2 ticks)
    stop_note();
    
    // Step  6: D#2 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G#3 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0CFA);
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2168 T2 ticks)
    stop_note();
    
    // Step  8: A2 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x06E0);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C#4 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1624 T2 ticks)
    stop_note();
    
    // Step 10: D#3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step 11: F#4 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x171F);
    delayTicksT0(38); // 19000.0μs at 2KHz (converted from 1216 T2 ticks)
    stop_note();
    
    // Step 12: G3 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(71); // 35500.0μs at 2KHz (converted from 2296 T2 ticks)
    stop_note();
    
    // Step 13: A#4 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x1D22);
    delayTicksT0(30); // 15000.0μs at 2KHz (converted from 964 T2 ticks)
    stop_note();
    
    // Step 14: C4 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(53); // 26500.0μs at 2KHz (converted from 1720 T2 ticks)
    stop_note();
    
    // Step 15: D5 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x25FD);
    delayTicksT0(23); // 11500.0μs at 2KHz (converted from 764 T2 ticks)
    stop_note();
    
    // Step 16: F4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(40); // 20000.0μs at 2KHz (converted from 1288 T2 ticks)
    stop_note();
    
}

void play_sfx_05() {
    // SFX 05 - 8 notes, Speed: 3
    // Step  0: A0 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01B8);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: A0 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01B8);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: A0 (Phaser) Vol:6 Effect:0
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01B8);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: A0 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01B8);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: G#0 (Phaser) Vol:4 Effect:0
    set_instrument(7);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x019F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: G0 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: F#0 (Phaser) Vol:2 Effect:0
    set_instrument(7);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0171);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: F0 (Phaser) Vol:1 Effect:5
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x015D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_06() {
    // SFX 06 - 14 notes, Speed: 3
    // Step  0: C3 (Tilted Saw) Vol:7 Effect:0
    set_instrument(1);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(80); // 40000.0μs at 2KHz (converted from 2583 T2 ticks)
    stop_note();
    
    // Step  1: D1 (Tilted Saw) Vol:7 Effect:0
    set_instrument(1);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x024B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: A3 (Tilted Saw) Vol:7 Effect:0
    set_instrument(1);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0DC0);
    delayTicksT0(48); // 24000.0μs at 2KHz (converted from 1536 T2 ticks)
    stop_note();
    
    // Step  3: A#1 (Tilted Saw) Vol:7 Effect:0
    set_instrument(1);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: E4 (Tilted Saw) Vol:7 Effect:0
    set_instrument(1);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x149A);
    delayTicksT0(31); // 15500.0μs at 2KHz (converted from 1023 T2 ticks)
    stop_note();
    
    // Step  5: G#2 (Tilted Saw) Vol:6 Effect:0
    set_instrument(1);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x067D);
    delayTicksT0(101); // 50500.0μs at 2KHz (converted from 3252 T2 ticks)
    stop_note();
    
    // Step  6: B4 (Tilted Saw) Vol:6 Effect:0
    set_instrument(1);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x1EDE);
    delayTicksT0(21); // 10500.0μs at 2KHz (converted from 684 T2 ticks)
    stop_note();
    
    // Step  7: E3 (Tilted Saw) Vol:5 Effect:0
    set_instrument(1);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0A4D);
    delayTicksT0(64); // 32000.0μs at 2KHz (converted from 2049 T2 ticks)
    stop_note();
    
    // Step  8: D#5 (Tilted Saw) Vol:4 Effect:0
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(16); // 8000.0μs at 2KHz (converted from 543 T2 ticks)
    stop_note();
    
    // Step  9: B3 (Tilted Saw) Vol:2 Effect:0
    set_instrument(1);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0F6F);
    delayTicksT0(42); // 21000.0μs at 2KHz (converted from 1368 T2 ticks)
    stop_note();
    
    // Step 10: E3 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0A4D);
    delayTicksT0(64); // 32000.0μs at 2KHz (converted from 2049 T2 ticks)
    stop_note();
    
    // Step 11: F2 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3870 T2 ticks)
    stop_note();
    
    // Step 12: E1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0293);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: D#0 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0137);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_07() {
    // SFX 07 - 9 notes, Speed: 2
    // Step  0: E1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0293);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F#1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x02E3);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: G#1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x033E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: A#1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3866 T2 ticks)
    stop_note();
    
    // Step  4: D2 (Tilted Saw) Vol:2 Effect:0
    set_instrument(1);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0496);
    delayTicksT0(95); // 47500.0μs at 2KHz (converted from 3068 T2 ticks)
    stop_note();
    
    // Step  5: G#2 (Tilted Saw) Vol:2 Effect:0
    set_instrument(1);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x067D);
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2168 T2 ticks)
    stop_note();
    
    // Step  6: D3 (Tilted Saw) Vol:3 Effect:0
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x092D);
    delayTicksT0(47); // 23500.0μs at 2KHz (converted from 1534 T2 ticks)
    stop_note();
    
    // Step  7: D4 (Tilted Saw) Vol:4 Effect:0
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x125A);
    delayTicksT0(23); // 11500.0μs at 2KHz (converted from 766 T2 ticks)
    stop_note();
    
    // Step  8: D4 (Tilted Saw) Vol:4 Effect:0
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x125A);
    delayTicksT0(23); // 11500.0μs at 2KHz (converted from 766 T2 ticks)
    stop_note();
    
}

void play_sfx_08() {
    // SFX 08 - 16 notes, Speed: 3
    // Step  0: G0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: A#0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: D1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x024B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: E1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0293);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: A#1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: A#2 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2898 T2 ticks)
    stop_note();
    
    // Step  6: B3 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0F6F);
    delayTicksT0(42); // 21000.0μs at 2KHz (converted from 1368 T2 ticks)
    stop_note();
    
    // Step  7: B3 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0F6F);
    delayTicksT0(42); // 21000.0μs at 2KHz (converted from 1368 T2 ticks)
    stop_note();
    
    // Step  8: G#3 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0CFA);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1626 T2 ticks)
    stop_note();
    
    // Step  9: G#3 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0CFA);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1626 T2 ticks)
    stop_note();
    
    // Step 10: B3 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    delayTicksT0(42); // 21000.0μs at 2KHz (converted from 1368 T2 ticks)
    stop_note();
    
    // Step 11: B3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0F6F);
    delayTicksT0(42); // 21000.0μs at 2KHz (converted from 1368 T2 ticks)
    stop_note();
    
    // Step 12: G#3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0CFA);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1626 T2 ticks)
    stop_note();
    
    // Step 13: G#3 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0CFA);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1626 T2 ticks)
    stop_note();
    
    // Step 14: B3 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0F6F);
    delayTicksT0(42); // 21000.0μs at 2KHz (converted from 1368 T2 ticks)
    stop_note();
    
    // Step 15: B3 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0F6F);
    delayTicksT0(42); // 21000.0μs at 2KHz (converted from 1368 T2 ticks)
    stop_note();
    
}

void play_sfx_09() {
    // SFX 09 - 8 notes, Speed: 3
    // Step  0: F0 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: G0 (Tilted Saw) Vol:3 Effect:0
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: D#5 (Noise) Vol:4 Effect:0
    set_instrument(6);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(16); // 8000.0μs at 2KHz (converted from 543 T2 ticks)
    stop_note();
    
    // Step  3: D#5 (Noise) Vol:4 Effect:0
    set_instrument(6);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(16); // 8000.0μs at 2KHz (converted from 543 T2 ticks)
    stop_note();
    
    // Step  4: D#5 (Noise) Vol:3 Effect:0
    set_instrument(6);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(16); // 8000.0μs at 2KHz (converted from 543 T2 ticks)
    stop_note();
    
    // Step  5: D#5 (Noise) Vol:2 Effect:0
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(16); // 8000.0μs at 2KHz (converted from 543 T2 ticks)
    stop_note();
    
    // Step  6: D#5 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(16); // 8000.0μs at 2KHz (converted from 543 T2 ticks)
    stop_note();
    
    // Step  7: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(16); // 8000.0μs at 2KHz (converted from 543 T2 ticks)
    stop_note();
    
}

void play_sfx_10() {
    // SFX 10 - 15 notes, Speed: 16
    // Step  0: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: B2 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x07B7);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: B2 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x07B7);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: B2 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x07B7);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: B2 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x07B7);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: B2 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x07B7);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_11() {
    // SFX 11 - 30 notes, Speed: 32
    // Step  0: F2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: F2 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: C2 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: D#2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: D#2 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: A#2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: A#2 (Triangle) Vol:4 Effect:6
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0748);
    // Apply effect 6 (Arp Fast)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: G2 (Triangle) Vol:3 Effect:5
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: G2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: A#1 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: A#1 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: F2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: F2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: G1 (Triangle) Vol:6 Effect:1
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: C2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: C2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: G2 (Triangle) Vol:6 Effect:1
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x061F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: C3 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: A#2 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: A#1 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: G1 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: F2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: D#2 (Triangle) Vol:2 Effect:2
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: C2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: C2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_12() {
    // SFX 12 - 29 notes, Speed: 16
    // Step  0: G0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: G0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: G0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: G0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: G0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: D#0 (Triangle) Vol:5 Effect:1
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0137);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: D#1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: A#0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: A#0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: A#0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: A#0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: A#0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: F0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: F0 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: D#0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0137);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: D#0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0137);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0137);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: C1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: C1 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: F1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: A#1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#1 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: D#1 (Triangle) Vol:7 Effect:1
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: F0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: A#0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: F0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: D#0 (Triangle) Vol:5 Effect:1
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0137);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: A#0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: A#0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_13() {
    // SFX 13 - 27 notes, Speed: 4
    // Step  0: C1 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: E2 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0526);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: E1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0293);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: B2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x07B7);
    delayTicksT0(114); // 57000.0μs at 2KHz (converted from 3648 T2 ticks)
    stop_note();
    
    // Step  4: C#2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0454);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: G#3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0CFA);
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2168 T2 ticks)
    stop_note();
    
    // Step  6: A2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x06E0);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G4 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x187F);
    delayTicksT0(35); // 17500.0μs at 2KHz (converted from 1148 T2 ticks)
    stop_note();
    
    // Step  8: E3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0A4D);
    delayTicksT0(85); // 42500.0μs at 2KHz (converted from 2732 T2 ticks)
    stop_note();
    
    // Step  9: B4 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x1EDE);
    delayTicksT0(28); // 14000.0μs at 2KHz (converted from 912 T2 ticks)
    stop_note();
    
    // Step 10: G#3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0CFA);
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2168 T2 ticks)
    stop_note();
    
    // Step 11: D5 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x25FD);
    delayTicksT0(23); // 11500.0μs at 2KHz (converted from 764 T2 ticks)
    stop_note();
    
    // Step 12: C#4 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1624 T2 ticks)
    stop_note();
    
    // Step 13: D5 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x25FD);
    delayTicksT0(23); // 11500.0μs at 2KHz (converted from 764 T2 ticks)
    stop_note();
    
    // Step 14: C#4 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1624 T2 ticks)
    stop_note();
    
    // Step 15: D5 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x25FD);
    delayTicksT0(23); // 11500.0μs at 2KHz (converted from 764 T2 ticks)
    stop_note();
    
    // Step 16: C#4 (Organ) Vol:2 Effect:0
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1624 T2 ticks)
    stop_note();
    
    // Step 17: D#5 (Organ) Vol:2 Effect:0
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(22); // 11000.0μs at 2KHz (converted from 724 T2 ticks)
    stop_note();
    
    // Step 18: C#4 (Organ) Vol:2 Effect:0
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1624 T2 ticks)
    stop_note();
    
    // Step 19: D#5 (Organ) Vol:2 Effect:0
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(22); // 11000.0μs at 2KHz (converted from 724 T2 ticks)
    stop_note();
    
    // Step 20: C#4 (Organ) Vol:1 Effect:0
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1624 T2 ticks)
    stop_note();
    
    // Step 21: D#5 (Organ) Vol:1 Effect:0
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(22); // 11000.0μs at 2KHz (converted from 724 T2 ticks)
    stop_note();
    
    // Step 22: C#4 (Organ) Vol:1 Effect:0
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1624 T2 ticks)
    stop_note();
    
    // Step 23: D#5 (Organ) Vol:1 Effect:0
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(22); // 11000.0μs at 2KHz (converted from 724 T2 ticks)
    stop_note();
    
    // Step 24: C#4 (Organ) Vol:1 Effect:0
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1624 T2 ticks)
    stop_note();
    
    // Step 25: D#5 (Organ) Vol:1 Effect:0
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    delayTicksT0(22); // 11000.0μs at 2KHz (converted from 724 T2 ticks)
    stop_note();
    
    // Step 26: C#4 (Organ) Vol:1 Effect:0
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(50); // 25000.0μs at 2KHz (converted from 1624 T2 ticks)
    stop_note();
    
}

void play_sfx_14() {
    // SFX 14 - 9 notes, Speed: 4
    // Step  0: B3 (Phaser) Vol:4 Effect:0
    set_instrument(7);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0F6F);
    delayTicksT0(57); // 28500.0μs at 2KHz (converted from 1824 T2 ticks)
    stop_note();
    
    // Step  1: G3 (Phaser) Vol:6 Effect:0
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(71); // 35500.0μs at 2KHz (converted from 2296 T2 ticks)
    stop_note();
    
    // Step  2: D3 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x092D);
    delayTicksT0(95); // 47500.0μs at 2KHz (converted from 3068 T2 ticks)
    stop_note();
    
    // Step  3: F2 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: A1 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0370);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: A1 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0370);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C#2 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0454);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: E2 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0526);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: B1 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x03DB);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_15() {
    // SFX 15 - 14 notes, Speed: 3
    // Step  0: A0 (Noise) Vol:4 Effect:5
    set_instrument(6);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x01B8);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: D1 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x024B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F#0 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0171);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: A#0 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C#1 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x022A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: F0 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x015D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: F1 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G0 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C1 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: E0 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0149);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: A0 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01B8);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: F1 (Noise) Vol:4 Effect:5
    set_instrument(6);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: G#0 (Noise) Vol:3 Effect:5
    set_instrument(6);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x019F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: C#1 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x022A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_16() {
    // SFX 16 - 3 notes, Speed: 16
    // Step  0: G2 (Square) Vol:7 Effect:5
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C2 (Square) Vol:7 Effect:5
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0416);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: D#3 (Square) Vol:7 Effect:5
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_17() {
    // SFX 17 - 29 notes, Speed: 16
    // Step  0: F3 (Organ) Vol:3 Effect:4
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F3 (Organ) Vol:5 Effect:4
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F3 (Organ) Vol:7 Effect:4
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: F2 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: A#2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: A#2 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: C2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C2 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: C2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: C2 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: A#1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: A#1 (Organ) Vol:7 Effect:2
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: A#1 (Organ) Vol:6 Effect:2
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: D#3 (Organ) Vol:1 Effect:4
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: D#3 (Organ) Vol:3 Effect:4
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: D#3 (Organ) Vol:5 Effect:4
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#3 (Organ) Vol:7 Effect:4
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: G2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: G2 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: G1 (Organ) Vol:2 Effect:0
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: D#2 (Organ) Vol:5 Effect:1
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: F4 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: C4 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: C3 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: F3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: F3 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: A#2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: A#2 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_18() {
    // SFX 18 - 24 notes, Speed: 16
    // Step  0: A#0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: A#0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: D#1 (Triangle) Vol:7 Effect:1
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: D#1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: A#0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: A#0 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: F1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: F1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: F1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: F1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: G0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: G0 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: A#0 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: A#0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: D#1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: A#0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: A#0 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: G1 (Triangle) Vol:7 Effect:1
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: G1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: G1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: G1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: D#1 (Triangle) Vol:7 Effect:0
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: D#1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_19() {
    // SFX 19 - 32 notes, Speed: 32
    // Step  0: A#2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: A#2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: A#2 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: D#2 (Triangle) Vol:1 Effect:1
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C3 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: D#3 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G2 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: G3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: A#2 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: D#3 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: A#2 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: F3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: F3 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: F3 (Triangle) Vol:2 Effect:0
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: A#1 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: A#2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: A#2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: G3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: C3 (Triangle) Vol:4 Effect:2
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: C3 (Triangle) Vol:3 Effect:2
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: D#3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: C2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: F2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: F2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: G2 (Triangle) Vol:5 Effect:2
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x061F);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: G2 (Triangle) Vol:4 Effect:2
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x061F);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: G2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: F2 (Triangle) Vol:2 Effect:1
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0574);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: F2 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: F2 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_20() {
    // SFX 20 - 15 notes, Speed: 8
    // Step  0: C#0 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: D#5 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(45); // 22500.0μs at 2KHz (converted from 1448 T2 ticks)
    stop_note();
    
    // Step  6: D#5 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(45); // 22500.0μs at 2KHz (converted from 1448 T2 ticks)
    stop_note();
    
    // Step  8: B2 (Noise) Vol:5 Effect:0
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x07B7);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: B2 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x07B7);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: D#5 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(45); // 22500.0μs at 2KHz (converted from 1448 T2 ticks)
    stop_note();
    
    // Step 14: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: C#0 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: C#0 (Phaser) Vol:7 Effect:5
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: D#5 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(45); // 22500.0μs at 2KHz (converted from 1448 T2 ticks)
    stop_note();
    
    // Step 22: D#5 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(45); // 22500.0μs at 2KHz (converted from 1448 T2 ticks)
    stop_note();
    
    // Step 24: B2 (Noise) Vol:5 Effect:0
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x07B7);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: B2 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x07B7);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: D#5 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(45); // 22500.0μs at 2KHz (converted from 1448 T2 ticks)
    stop_note();
    
}

void play_sfx_21() {
    // SFX 21 - 31 notes, Speed: 32
    // Step  0: A#0 (Tilted Saw) Vol:4 Effect:0
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: A#0 (Tilted Saw) Vol:3 Effect:0
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: A#0 (Tilted Saw) Vol:2 Effect:0
    set_instrument(1);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: F1 (Tilted Saw) Vol:3 Effect:0
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: F1 (Tilted Saw) Vol:2 Effect:0
    set_instrument(1);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: F1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: D#2 (Tilted Saw) Vol:4 Effect:0
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: D#2 (Tilted Saw) Vol:3 Effect:0
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C2 (Tilted Saw) Vol:5 Effect:2
    set_instrument(1);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C2 (Tilted Saw) Vol:4 Effect:2
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0416);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: C2 (Tilted Saw) Vol:3 Effect:2
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0416);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: G1 (Tilted Saw) Vol:4 Effect:0
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: G1 (Tilted Saw) Vol:4 Effect:0
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: G1 (Tilted Saw) Vol:3 Effect:0
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: G1 (Tilted Saw) Vol:2 Effect:0
    set_instrument(1);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: G1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: D#1 (Tilted Saw) Vol:4 Effect:0
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: D#1 (Tilted Saw) Vol:3 Effect:0
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: D#1 (Tilted Saw) Vol:2 Effect:0
    set_instrument(1);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: F1 (Tilted Saw) Vol:3 Effect:0
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: F1 (Tilted Saw) Vol:2 Effect:0
    set_instrument(1);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: F1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: A#1 (Tilted Saw) Vol:4 Effect:2
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: A#1 (Tilted Saw) Vol:3 Effect:2
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: G1 (Tilted Saw) Vol:5 Effect:0
    set_instrument(1);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: G1 (Tilted Saw) Vol:4 Effect:0
    set_instrument(1);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: G1 (Tilted Saw) Vol:3 Effect:0
    set_instrument(1);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: G1 (Tilted Saw) Vol:2 Effect:0
    set_instrument(1);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: G1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: G1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: G1 (Tilted Saw) Vol:1 Effect:0
    set_instrument(1);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_22() {
    // SFX 22 - 32 notes, Speed: 16
    // Step  0: A#3 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: G4 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x187F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: A#3 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: G4 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x187F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: A#3 (Phaser) Vol:2 Effect:0
    set_instrument(7);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: G4 (Phaser) Vol:2 Effect:0
    set_instrument(7);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x187F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: A#3 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x187F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: A#2 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: G3 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: A#2 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: G3 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: F2 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: C3 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: F2 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: C3 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: G2 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: D#3 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: G2 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#3 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: G2 (Phaser) Vol:2 Effect:0
    set_instrument(7);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: D#3 (Phaser) Vol:2 Effect:0
    set_instrument(7);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: F3 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: C4 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: F3 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: C4 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: F3 (Phaser) Vol:2 Effect:0
    set_instrument(7);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: C4 (Phaser) Vol:2 Effect:0
    set_instrument(7);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: F3 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: C4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: F3 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: C4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_23() {
    // SFX 23 - 9 notes, Speed: 6
    // Step  0: C2 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F4 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1932 T2 ticks)
    stop_note();
    
    // Step  2: F4 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1932 T2 ticks)
    stop_note();
    
    // Step  3: F4 (Phaser) Vol:6 Effect:0
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1932 T2 ticks)
    stop_note();
    
    // Step  4: F4 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1932 T2 ticks)
    stop_note();
    
    // Step  5: F4 (Phaser) Vol:4 Effect:0
    set_instrument(7);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1932 T2 ticks)
    stop_note();
    
    // Step  6: F4 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1932 T2 ticks)
    stop_note();
    
    // Step  7: F4 (Phaser) Vol:2 Effect:0
    set_instrument(7);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1932 T2 ticks)
    stop_note();
    
    // Step  8: F4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1932 T2 ticks)
    stop_note();
    
}

void play_sfx_24() {
    // SFX 24 - 32 notes, Speed: 24
    // Step  0: F3 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F3 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: F4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: F3 (Pulse) Vol:3 Effect:0
    set_instrument(4);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: G4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x187F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: F3 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x187F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: A#2 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: F4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: A#2 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: D#3 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: C5 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x2101);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: D#3 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: C5 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x2101);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: D#3 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: A#3 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: F4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: A#3 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: F4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: A#3 (Pulse) Vol:3 Effect:0
    set_instrument(4);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: G4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x187F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: A#3 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: G4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x187F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#3 (Pulse) Vol:1 Effect:0
    set_instrument(4);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: C3 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: G3 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: F4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: F3 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: C5 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x2101);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: F3 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: C5 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x2101);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_25() {
    // SFX 25 - 26 notes, Speed: 24
    // Step  0: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: A#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: A#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: A#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: A#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: A#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: D#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0137);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: F0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: G0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: C1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: C1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: D#1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: A#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: G0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_26() {
    // SFX 26 - 9 notes, Speed: 12
    // Step  0: B4 (Noise) Vol:3 Effect:5
    set_instrument(6);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x1EDE);
    // Apply effect 5 (Fade Out)
    delayTicksT0(85); // 42500.0μs at 2KHz (converted from 2736 T2 ticks)
    stop_note();
    
    // Step  2: B4 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1EDE);
    // Apply effect 5 (Fade Out)
    delayTicksT0(85); // 42500.0μs at 2KHz (converted from 2736 T2 ticks)
    stop_note();
    
    // Step  4: B4 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1EDE);
    // Apply effect 5 (Fade Out)
    delayTicksT0(85); // 42500.0μs at 2KHz (converted from 2736 T2 ticks)
    stop_note();
    
    // Step  8: D#4 (Noise) Vol:4 Effect:0
    set_instrument(6);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x1372);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: D#4 (Noise) Vol:3 Effect:0
    set_instrument(6);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x1372);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: D#4 (Noise) Vol:2 Effect:0
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1372);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: D#4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: D#4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2172 T2 ticks)
    stop_note();
    
}

void play_sfx_27() {
    // SFX 27 - 27 notes, Speed: 12
    // Step  0: C3 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: G3 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C3 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: G3 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C3 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: A#4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2892 T2 ticks)
    stop_note();
    
    // Step 10: G3 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: A#4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2892 T2 ticks)
    stop_note();
    
    // Step 12: C3 (Pulse) Vol:1 Effect:0
    set_instrument(4);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: F4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step 14: G3 (Pulse) Vol:1 Effect:0
    set_instrument(4);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: F4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step 16: F2 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: D#4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: C3 (Pulse) Vol:5 Effect:0
    set_instrument(4);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: C5 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x2101);
    delayTicksT0(80); // 40000.0μs at 2KHz (converted from 2580 T2 ticks)
    stop_note();
    
    // Step 20: F2 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: G4 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x187F);
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3444 T2 ticks)
    stop_note();
    
    // Step 22: C3 (Pulse) Vol:4 Effect:0
    set_instrument(4);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: F2 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: C3 (Pulse) Vol:2 Effect:0
    set_instrument(4);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: A#3 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: F2 (Pulse) Vol:1 Effect:0
    set_instrument(4);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: A#3 (Phaser) Vol:1 Effect:0
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: C3 (Pulse) Vol:1 Effect:0
    set_instrument(4);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_28() {
    // SFX 28 - 27 notes, Speed: 24
    // Step  0: C1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C1 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: F1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: F1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: F1 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: D#1 (Organ) Vol:7 Effect:1
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: D#1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: G1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: G1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: A#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: A#0 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: C1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: C1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: C1 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: D#1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: D#1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: D#1 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: A#0 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: A#0 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: F1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: F1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: A#0 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: A#0 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_29() {
    // SFX 29 - 20 notes, Speed: 24
    // Step  0: C1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C1 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: F1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: F1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: F1 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: D#1 (Organ) Vol:7 Effect:1
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: D#1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: G1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: G1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: D#1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: D#1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: C1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: C1 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: C1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: C1 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: C1 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: C1 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_30() {
    // SFX 30 - 25 notes, Speed: 12
    // Step  0: C3 (Phaser) Vol:7 Effect:1
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C3 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C3 (Phaser) Vol:6 Effect:2
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C3 (Phaser) Vol:5 Effect:2
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: A#4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2892 T2 ticks)
    stop_note();
    
    // Step  5: A#4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2892 T2 ticks)
    stop_note();
    
    // Step  6: C2 (Phaser) Vol:5 Effect:2
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: A#4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2892 T2 ticks)
    stop_note();
    
    // Step  8: F4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step  9: F4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step 10: C2 (Phaser) Vol:5 Effect:2
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: F4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step 12: C2 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: A#2 (Phaser) Vol:7 Effect:1
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: A#2 (Phaser) Vol:7 Effect:2
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: A#2 (Phaser) Vol:6 Effect:2
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0748);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: G2 (Phaser) Vol:7 Effect:1
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: G2 (Phaser) Vol:7 Effect:2
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: G2 (Phaser) Vol:6 Effect:2
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x061F);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#2 (Phaser) Vol:7 Effect:1
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: A#2 (Phaser) Vol:7 Effect:2
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: A#2 (Phaser) Vol:6 Effect:2
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0748);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: D#3 (Phaser) Vol:7 Effect:1
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: D#3 (Phaser) Vol:7 Effect:2
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: D#3 (Phaser) Vol:6 Effect:2
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_31() {
    // SFX 31 - 25 notes, Speed: 12
    // Step  0: C3 (Phaser) Vol:7 Effect:1
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C3 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C3 (Phaser) Vol:6 Effect:2
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C3 (Phaser) Vol:5 Effect:2
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: A#4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2892 T2 ticks)
    stop_note();
    
    // Step  5: A#4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2892 T2 ticks)
    stop_note();
    
    // Step  6: C2 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: A#4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2892 T2 ticks)
    stop_note();
    
    // Step  8: F4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step  9: F4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step 10: C2 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: F4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step 12: C2 (Phaser) Vol:5 Effect:0
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: G2 (Phaser) Vol:7 Effect:1
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: G2 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: G2 (Phaser) Vol:6 Effect:2
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x061F);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: G2 (Phaser) Vol:5 Effect:2
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x061F);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: C2 (Phaser) Vol:5 Effect:1
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#2 (Phaser) Vol:7 Effect:1
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: A#2 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: A#2 (Phaser) Vol:6 Effect:2
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0748);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: A#2 (Phaser) Vol:5 Effect:2
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0748);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: G4 (Triangle) Vol:1 Effect:2
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x187F);
    // Apply effect 2 (Vibrato)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3444 T2 ticks)
    stop_note();
    
    // Step 29: G4 (Triangle) Vol:1 Effect:2
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x187F);
    // Apply effect 2 (Vibrato)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3444 T2 ticks)
    stop_note();
    
    // Step 30: G4 (Triangle) Vol:1 Effect:2
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x187F);
    // Apply effect 2 (Vibrato)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3444 T2 ticks)
    stop_note();
    
}

void play_sfx_32() {
    // SFX 32 - 23 notes, Speed: 12
    // Step  0: C3 (Phaser) Vol:7 Effect:1
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C3 (Phaser) Vol:7 Effect:0
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C3 (Phaser) Vol:7 Effect:2
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C3 (Phaser) Vol:7 Effect:2
    set_instrument(7);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C3 (Phaser) Vol:6 Effect:2
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C3 (Phaser) Vol:5 Effect:2
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C3 (Phaser) Vol:4 Effect:2
    set_instrument(7);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: C3 (Phaser) Vol:3 Effect:2
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C3 (Phaser) Vol:2 Effect:2
    set_instrument(7);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C3 (Phaser) Vol:1 Effect:2
    set_instrument(7);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x082D);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: A#3 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: A#3 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: F4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step 21: F4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3864 T2 ticks)
    stop_note();
    
    // Step 22: D#4 (Triangle) Vol:1 Effect:1
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: D#4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: G3 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: G3 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: G3 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: C4 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: C4 (Triangle) Vol:1 Effect:2
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: C4 (Triangle) Vol:1 Effect:2
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: C4 (Triangle) Vol:1 Effect:2
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_33() {
    // SFX 33 - 28 notes, Speed: 12
    // Step  0: C1 (Square) Vol:3 Effect:2
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C1 (Square) Vol:3 Effect:2
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C1 (Square) Vol:2 Effect:2
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C1 (Square) Vol:2 Effect:2
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C1 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C1 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C1 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C1 (Square) Vol:3 Effect:2
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C1 (Square) Vol:3 Effect:2
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: C1 (Square) Vol:2 Effect:2
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: C1 (Square) Vol:2 Effect:2
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: C1 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: C1 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: C1 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: G0 (Square) Vol:3 Effect:2
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0187);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: G0 (Square) Vol:3 Effect:2
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0187);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: G0 (Square) Vol:2 Effect:2
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0187);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: G0 (Square) Vol:2 Effect:2
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0187);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: G0 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0187);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: G0 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0187);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: G0 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0187);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#0 (Square) Vol:3 Effect:2
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: A#0 (Square) Vol:3 Effect:2
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: A#0 (Square) Vol:2 Effect:2
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: A#0 (Square) Vol:2 Effect:2
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: A#0 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: A#0 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: A#0 (Square) Vol:1 Effect:2
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_34() {
    // SFX 34 - 27 notes, Speed: 12
    // Step  0: C1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: C1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: C1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: C1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: C1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: C1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: A#0 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: A#0 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: G1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: G1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: G0 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: G0 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: G0 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#0 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: A#0 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: A#0 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: D#1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: D#1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: D#1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_35() {
    // SFX 35 - 1 notes, Speed: 4
    // Step  0: D#4 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(45); // 22500.0μs at 2KHz (converted from 1448 T2 ticks)
    stop_note();
    
}

void play_sfx_36() {
    // SFX 36 - 14 notes, Speed: 12
    // Step  0: C1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: C1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C1 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: A#0 (Square) Vol:3 Effect:1
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: A#0 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: D#0 (Square) Vol:2 Effect:1
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0137);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: D#0 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0137);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_37() {
    // SFX 37 - 22 notes, Speed: 16
    // Step  0: C1 (Square) Vol:5 Effect:0
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C1 (Square) Vol:4 Effect:0
    set_instrument(3);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: D#1 (Square) Vol:5 Effect:0
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: D#1 (Square) Vol:4 Effect:0
    set_instrument(3);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: D#1 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: D#1 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C2 (Square) Vol:5 Effect:0
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C2 (Square) Vol:4 Effect:0
    set_instrument(3);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: G1 (Square) Vol:5 Effect:0
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: G1 (Square) Vol:4 Effect:0
    set_instrument(3);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: C2 (Square) Vol:5 Effect:0
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: G1 (Square) Vol:5 Effect:0
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: A#1 (Square) Vol:4 Effect:0
    set_instrument(3);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: F2 (Square) Vol:6 Effect:0
    set_instrument(3);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: A#2 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: A#2 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: A#2 (Square) Vol:6 Effect:0
    set_instrument(3);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: A#2 (Square) Vol:5 Effect:0
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: A#2 (Square) Vol:4 Effect:0
    set_instrument(3);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: A#2 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_38() {
    // SFX 38 - 21 notes, Speed: 12
    // Step  0: C3 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: G3 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C4 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C3 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: G3 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C4 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C3 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G3 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C4 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C3 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: G3 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: C4 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: C3 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: G3 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: C4 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: C3 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: G3 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: C4 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: C3 (Saw) Vol:1 Effect:5
    set_instrument(2);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: G3 (Saw) Vol:1 Effect:5
    set_instrument(2);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: C4 (Saw) Vol:1 Effect:5
    set_instrument(2);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_39() {
    // SFX 39 - 16 notes, Speed: 16
    // Step  0: B3 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step  4: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step  6: B3 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step 10: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step 12: B3 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step 14: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
}

void play_sfx_40() {
    // SFX 40 - 20 notes, Speed: 16
    // Step  0: A#1 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: A#1 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: G2 (Saw) Vol:7 Effect:1
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: G2 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: G2 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: G2 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C2 (Saw) Vol:7 Effect:1
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0416);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: C2 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: G1 (Saw) Vol:7 Effect:1
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: G1 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: F2 (Saw) Vol:7 Effect:1
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0574);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: F2 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: A#1 (Saw) Vol:7 Effect:1
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: A#1 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: A#1 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: A#1 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: D#2 (Saw) Vol:7 Effect:1
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: D#2 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: D#2 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#2 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_41() {
    // SFX 41 - 32 notes, Speed: 8
    // Step  0: C3 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C4 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3440 T2 ticks)
    stop_note();
    
    // Step  2: C3 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3440 T2 ticks)
    stop_note();
    
    // Step  4: D#2 (Organ) Vol:6 Effect:5
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: D#3 (Organ) Vol:6 Effect:5
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: G2 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G3 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: G2 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: G3 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: G2 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: G3 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: G2 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: G3 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: G2 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: G3 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: D#2 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: D#3 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: D#2 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#3 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: D#2 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: D#3 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: F2 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0574);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: F3 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: F2 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0574);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: F3 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: F2 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0574);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: F3 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: G2 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: G3 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: G2 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: G3 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_42() {
    // SFX 42 - 32 notes, Speed: 32
    // Step  0: C1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: A#0 (Saw) Vol:3 Effect:1
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: D#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: D#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: D#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: D#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: D#1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: D#1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: D#1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: A#1 (Saw) Vol:3 Effect:1
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: G1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: G1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: G1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: G1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: G1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: G1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: G1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: G1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: G1 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: G0 (Saw) Vol:4 Effect:0
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: A#1 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: G1 (Saw) Vol:6 Effect:1
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: G1 (Saw) Vol:5 Effect:0
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: G1 (Saw) Vol:4 Effect:2
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x030F);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: D#1 (Saw) Vol:6 Effect:0
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: D#1 (Saw) Vol:5 Effect:0
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_43() {
    // SFX 43 - 32 notes, Speed: 16
    // Step  0: G0 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: G0 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: G0 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: G0 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: D#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: D#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: C1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: C1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: C1 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: G0 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: G0 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: G0 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: G0 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: G0 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: G0 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: G0 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: G0 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0187);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: C1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: C1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: F1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: F1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: F1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: F1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: G1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: G1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: A#1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: A#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: A#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: A#1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_44() {
    // SFX 44 - 32 notes, Speed: 8
    // Step  0: G2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: G3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: G2 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: G3 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C2 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C3 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: D#2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: D#3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: D#2 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: D#3 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: C2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: C3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: C2 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: C3 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: C2 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0416);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: C3 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: D#2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: D#3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: D#2 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#3 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: F2 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: F3 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: F2 (Organ) Vol:2 Effect:0
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: F3 (Organ) Vol:2 Effect:0
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: G2 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: G3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: G2 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: G3 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: G2 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: G3 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: D#2 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: D#3 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_45() {
    // SFX 45 - 32 notes, Speed: 16
    // Step  0: F1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: F1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: G1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: G1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C2 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0416);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: C2 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0416);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C2 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0416);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C2 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0416);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: F2 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0574);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: F2 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0574);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: D#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: C2 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0416);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: G1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: A#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: D#1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: D#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: D#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: F1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: F1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: A#1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: A#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: A#1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: D#2 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: D#2 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: A#2 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0748);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: G2 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: C2 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0416);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: G1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_46() {
    // SFX 46 - 16 notes, Speed: 16
    // Step  0: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: B3 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step  4: B3 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step  6: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step  8: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step 10: B3 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: B3 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step 14: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
}

void play_sfx_47() {
    // SFX 47 - 16 notes, Speed: 16
    // Step  0: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step  4: B3 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step  6: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step  7: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step 10: C#0 (Triangle) Vol:7 Effect:5
    set_instrument(0);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0115);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step 12: B3 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
    // Step 14: B3 (Noise) Vol:5 Effect:5
    set_instrument(6);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0F6F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: D#5 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x28CA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2896 T2 ticks)
    stop_note();
    
}

void play_sfx_48() {
    // SFX 48 - 32 notes, Speed: 32
    // Step  0: F3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F3 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: G3 (Triangle) Vol:3 Effect:1
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: F3 (Triangle) Vol:2 Effect:4
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: G3 (Triangle) Vol:2 Effect:1
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: F3 (Triangle) Vol:1 Effect:4
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G3 (Triangle) Vol:1 Effect:1
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: D#4 (Triangle) Vol:4 Effect:4
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x1372);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C4 (Triangle) Vol:4 Effect:1
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: A#3 (Triangle) Vol:4 Effect:4
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: A#3 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: C4 (Triangle) Vol:4 Effect:4
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x105A);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: C4 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: G3 (Triangle) Vol:4 Effect:1
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: G3 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: A#3 (Triangle) Vol:4 Effect:4
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: A#3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: A#3 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: C4 (Triangle) Vol:3 Effect:1
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: A#3 (Triangle) Vol:2 Effect:4
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: C4 (Triangle) Vol:2 Effect:1
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: A#3 (Triangle) Vol:2 Effect:4
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: C4 (Triangle) Vol:2 Effect:1
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: G3 (Triangle) Vol:4 Effect:4
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: A#3 (Triangle) Vol:4 Effect:1
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: G3 (Triangle) Vol:3 Effect:4
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: A#3 (Triangle) Vol:2 Effect:1
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: G3 (Triangle) Vol:4 Effect:4
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 4 (Fade In)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: G3 (Triangle) Vol:4 Effect:0
    set_instrument(0);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: F3 (Triangle) Vol:3 Effect:1
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: F3 (Triangle) Vol:2 Effect:2
    set_instrument(0);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_49() {
    // SFX 49 - 22 notes, Speed: 8
    // Step  0: C3 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C3 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C3 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C3 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C3 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C3 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C3 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: C3 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C3 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C3 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: C3 (Organ) Vol:6 Effect:5
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: C3 (Organ) Vol:6 Effect:5
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: C3 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: C3 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: C3 (Organ) Vol:6 Effect:5
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: C3 (Organ) Vol:6 Effect:5
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: C3 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: C3 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: C3 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: C3 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: C3 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: C3 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_50() {
    // SFX 50 - 22 notes, Speed: 8
    // Step  0: G2 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: G2 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: G2 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: G2 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: G2 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: G2 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: G2 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G2 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: G2 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: G2 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: G2 (Organ) Vol:6 Effect:5
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: G2 (Organ) Vol:6 Effect:5
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: G2 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: G2 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: G2 (Organ) Vol:6 Effect:5
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: G2 (Organ) Vol:6 Effect:5
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: G2 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: G2 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: G2 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: G2 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: G2 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: G2 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x061F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_51() {
    // SFX 51 - 32 notes, Speed: 5
    // Step  0: D#0 (Phaser) Vol:3 Effect:0
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0137);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F0 (Phaser) Vol:3 Effect:1
    set_instrument(7);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x015D);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: G0 (Phaser) Vol:4 Effect:1
    set_instrument(7);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0187);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C1 (Phaser) Vol:4 Effect:1
    set_instrument(7);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x020B);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: G1 (Phaser) Vol:5 Effect:1
    set_instrument(7);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: D#2 (Phaser) Vol:6 Effect:1
    set_instrument(7);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C3 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: C4 (Square) Vol:7 Effect:1
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2150 T2 ticks)
    stop_note();
    
    // Step  8: D#3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(113); // 56500.0μs at 2KHz (converted from 3620 T2 ticks)
    stop_note();
    
    // Step  9: A#3 (Organ) Vol:7 Effect:1
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 1 (Slide)
    delayTicksT0(75); // 37500.0μs at 2KHz (converted from 2415 T2 ticks)
    stop_note();
    
    // Step 10: C3 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: C4 (Square) Vol:7 Effect:1
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2150 T2 ticks)
    stop_note();
    
    // Step 12: D#3 (Organ) Vol:7 Effect:0
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(113); // 56500.0μs at 2KHz (converted from 3620 T2 ticks)
    stop_note();
    
    // Step 13: A#3 (Organ) Vol:7 Effect:1
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 1 (Slide)
    delayTicksT0(75); // 37500.0μs at 2KHz (converted from 2415 T2 ticks)
    stop_note();
    
    // Step 14: C3 (Square) Vol:6 Effect:0
    set_instrument(3);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: C4 (Square) Vol:6 Effect:1
    set_instrument(3);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2150 T2 ticks)
    stop_note();
    
    // Step 16: D#3 (Organ) Vol:6 Effect:0
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(113); // 56500.0μs at 2KHz (converted from 3620 T2 ticks)
    stop_note();
    
    // Step 17: A#3 (Organ) Vol:6 Effect:1
    set_instrument(5);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 1 (Slide)
    delayTicksT0(75); // 37500.0μs at 2KHz (converted from 2415 T2 ticks)
    stop_note();
    
    // Step 18: C3 (Square) Vol:5 Effect:0
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: C4 (Square) Vol:5 Effect:1
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2150 T2 ticks)
    stop_note();
    
    // Step 20: D#3 (Organ) Vol:5 Effect:0
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(113); // 56500.0μs at 2KHz (converted from 3620 T2 ticks)
    stop_note();
    
    // Step 21: A#3 (Organ) Vol:5 Effect:1
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 1 (Slide)
    delayTicksT0(75); // 37500.0μs at 2KHz (converted from 2415 T2 ticks)
    stop_note();
    
    // Step 22: C3 (Square) Vol:4 Effect:0
    set_instrument(3);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: C4 (Square) Vol:4 Effect:1
    set_instrument(3);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2150 T2 ticks)
    stop_note();
    
    // Step 24: D#3 (Organ) Vol:4 Effect:0
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(113); // 56500.0μs at 2KHz (converted from 3620 T2 ticks)
    stop_note();
    
    // Step 25: A#3 (Organ) Vol:4 Effect:1
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 1 (Slide)
    delayTicksT0(75); // 37500.0μs at 2KHz (converted from 2415 T2 ticks)
    stop_note();
    
    // Step 26: C3 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: C4 (Square) Vol:3 Effect:1
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2150 T2 ticks)
    stop_note();
    
    // Step 28: D#3 (Organ) Vol:2 Effect:0
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x09B9);
    delayTicksT0(113); // 56500.0μs at 2KHz (converted from 3620 T2 ticks)
    stop_note();
    
    // Step 29: A#3 (Organ) Vol:2 Effect:1
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 1 (Slide)
    delayTicksT0(75); // 37500.0μs at 2KHz (converted from 2415 T2 ticks)
    stop_note();
    
    // Step 30: C3 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: C4 (Square) Vol:1 Effect:1
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    // Apply effect 1 (Slide)
    delayTicksT0(67); // 33500.0μs at 2KHz (converted from 2150 T2 ticks)
    stop_note();
    
}

void play_sfx_52() {
    // SFX 52 - 32 notes, Speed: 32
    // Step  0: C1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: A#0 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: A#0 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: A#0 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: D#1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: D#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: D#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: D#1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: D#1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: C1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: C1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: C1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: C1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: C1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: C1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: C1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: C1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: A#0 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: A#0 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: A#0 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: D#1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: D#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: D#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: D#1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: D#1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: F1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: F1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: F1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_53() {
    // SFX 53 - 32 notes, Speed: 32
    // Step  0: G1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: G1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: G1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: G1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: G1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: F1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: F1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: F1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: A#1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: A#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: A#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: A#1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: A#1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: G1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: G1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: G1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: G1 (Saw) Vol:7 Effect:5
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: G1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: G1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: G1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: G1 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x030F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: D#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: D#1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: D#1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: C1 (Saw) Vol:5 Effect:0
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: F1 (Saw) Vol:3 Effect:1
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 1 (Slide)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: A#1 (Saw) Vol:6 Effect:5
    set_instrument(2);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: D#1 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x026E);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: A#1 (Saw) Vol:7 Effect:2
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: A#1 (Saw) Vol:5 Effect:2
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x03A4);
    // Apply effect 2 (Vibrato)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: C1 (Saw) Vol:7 Effect:0
    set_instrument(2);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: C1 (Saw) Vol:5 Effect:5
    set_instrument(2);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_54() {
    // SFX 54 - 12 notes, Speed: 3
    // Step  0: G2 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3447 T2 ticks)
    stop_note();
    
    // Step  1: G3 (Square) Vol:3 Effect:0
    set_instrument(3);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(53); // 26500.0μs at 2KHz (converted from 1722 T2 ticks)
    stop_note();
    
    // Step  2: A#2 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2898 T2 ticks)
    stop_note();
    
    // Step  3: F3 (Organ) Vol:3 Effect:0
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1935 T2 ticks)
    stop_note();
    
    // Step  4: G2 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3447 T2 ticks)
    stop_note();
    
    // Step  5: G3 (Square) Vol:2 Effect:0
    set_instrument(3);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(53); // 26500.0μs at 2KHz (converted from 1722 T2 ticks)
    stop_note();
    
    // Step  6: A#2 (Organ) Vol:2 Effect:0
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2898 T2 ticks)
    stop_note();
    
    // Step  7: F3 (Organ) Vol:2 Effect:0
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1935 T2 ticks)
    stop_note();
    
    // Step  8: G2 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x061F);
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3447 T2 ticks)
    stop_note();
    
    // Step  9: G3 (Square) Vol:1 Effect:0
    set_instrument(3);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0C3F);
    delayTicksT0(53); // 26500.0μs at 2KHz (converted from 1722 T2 ticks)
    stop_note();
    
    // Step 10: A#2 (Organ) Vol:1 Effect:0
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0748);
    delayTicksT0(90); // 45000.0μs at 2KHz (converted from 2898 T2 ticks)
    stop_note();
    
    // Step 11: F3 (Organ) Vol:1 Effect:0
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0AE9);
    delayTicksT0(60); // 30000.0μs at 2KHz (converted from 1935 T2 ticks)
    stop_note();
    
}

void play_sfx_55() {
    // SFX 55 - 5 notes, Speed: 11
    // Step  0: F3 (Square) Vol:5 Effect:5
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F3 (Square) Vol:4 Effect:5
    set_instrument(3);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C4 (Square) Vol:7 Effect:0
    set_instrument(3);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C4 (Square) Vol:6 Effect:0
    set_instrument(3);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C4 (Square) Vol:5 Effect:5
    set_instrument(3);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_56() {
    // SFX 56 - 32 notes, Speed: 16
    // Step  0: C5 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x2101);
    // Apply effect 5 (Fade Out)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3440 T2 ticks)
    stop_note();
    
    // Step  1: C5 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x2101);
    // Apply effect 5 (Fade Out)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3440 T2 ticks)
    stop_note();
    
    // Step  2: C5 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x2101);
    // Apply effect 5 (Fade Out)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3440 T2 ticks)
    stop_note();
    
    // Step  3: C5 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x2101);
    // Apply effect 5 (Fade Out)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3440 T2 ticks)
    stop_note();
    
    // Step  4: C5 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x2101);
    // Apply effect 5 (Fade Out)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3440 T2 ticks)
    stop_note();
    
    // Step  5: C5 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x2101);
    // Apply effect 5 (Fade Out)
    delayTicksT0(107); // 53500.0μs at 2KHz (converted from 3440 T2 ticks)
    stop_note();
    
    // Step  6: G4 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x187F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x187F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: A#4 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step  9: A#4 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 10: A#4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 11: A#4 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 12: A#4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 13: A#4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 14: A#4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 15: A#4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 16: F4 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: F4 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: F4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: F4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: F4 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: F4 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: F4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: F4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: F4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: F4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: D#4 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: D#4 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: D#4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: D#4 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: D#4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: D#4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_57() {
    // SFX 57 - 32 notes, Speed: 16
    // Step  0: F4 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F4 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: F4 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: F4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: F4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: F4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: F4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x15D3);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: G4 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x187F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: G4 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x187F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: D#4 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: D#4 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: D#4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: D#4 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: D#4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: D#4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: A#4 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 17: A#4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 18: A#4 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 19: A#4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 20: A#4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 21: A#4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1D22);
    // Apply effect 5 (Fade Out)
    delayTicksT0(120); // 60000.0μs at 2KHz (converted from 3856 T2 ticks)
    stop_note();
    
    // Step 22: D#4 (Organ) Vol:7 Effect:5
    set_instrument(5);
    set_volume(126); // PICO-8 vol 7 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: D#4 (Organ) Vol:5 Effect:5
    set_instrument(5);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: D#4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: D#4 (Organ) Vol:4 Effect:5
    set_instrument(5);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: D#4 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: D#4 (Organ) Vol:3 Effect:5
    set_instrument(5);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: D#4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: D#4 (Organ) Vol:2 Effect:5
    set_instrument(5);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: D#4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: D#4 (Organ) Vol:1 Effect:5
    set_instrument(5);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_58() {
    // SFX 58 - 28 notes, Speed: 16
    // Step  0: C1 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C1 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: C1 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: C1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: C1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: C1 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: F1 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: F1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: F1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: F1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: F1 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x02BA);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: A#0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: A#0 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: A#0 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: A#0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: A#0 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: A#0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: A#0 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: A#0 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x01D2);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_59() {
    // SFX 59 - 27 notes, Speed: 16
    // Step  0: F0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: F0 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: F0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: F0 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: F0 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x015D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: G0 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: G0 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: G0 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: G0 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: G0 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0187);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: D#1 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: D#1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: D#1 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: C1 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: C1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: C1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: C1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: C1 (Triangle) Vol:5 Effect:0
    set_instrument(0);
    set_volume(90); // PICO-8 vol 5 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: C1 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: C1 (Triangle) Vol:6 Effect:0
    set_instrument(0);
    set_volume(108); // PICO-8 vol 6 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: C1 (Triangle) Vol:3 Effect:0
    set_instrument(0);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: C1 (Triangle) Vol:1 Effect:0
    set_instrument(0);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_60() {
    // SFX 60 - 27 notes, Speed: 16
    // Step  0: D#0 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0137);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: C3 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: D#0 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0137);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C3 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: D#2 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x04DC);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: A#2 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0748);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: D#0 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0137);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: D#0 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0137);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: F1 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: D#4 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: A#2 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0748);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: F2 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0574);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: A#0 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x01D2);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: G4 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x187F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: C2 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0416);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: A#3 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: F2 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0574);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: G4 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x187F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: G4 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x187F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: C2 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0416);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#3 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0E91);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: F2 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0574);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: F1 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x02BA);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: D#0 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0137);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: D#0 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0137);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: C3 (Noise) Vol:2 Effect:5
    set_instrument(6);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x082D);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: F2 (Noise) Vol:1 Effect:5
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0574);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_61() {
    // SFX 61 - 32 notes, Speed: 16
    // Step  0: D4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x125A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  1: D4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x125A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: D4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x125A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: D4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x125A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: C#4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  5: C#4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  7: A#3 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  8: F#3 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0B8F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C#3 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x08A9);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 10: D#2 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x04DC);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 11: G1 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x030F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: D#1 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 13: C#1 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x022A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 14: C1 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: C1 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: C1 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 17: C1 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: C1 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x020B);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#1 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x026E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: G#1 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x033E);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 21: F2 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0574);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: C3 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x082D);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 23: F#3 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0B8F);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 24: A#3 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0E91);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: C4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 26: C#4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1152);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 27: D#4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: D#4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x1372);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 29: E4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x149A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 30: E4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x149A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: E4 (Noise) Vol:1 Effect:0
    set_instrument(6);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x149A);
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

void play_sfx_62() {
    // SFX 62 - 16 notes, Speed: 64
    // Step  0: C4 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  2: C4 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  3: D#4 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x1372);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  4: G3 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  6: C4 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step  9: C4 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 12: C4 (Saw) Vol:1 Effect:5
    set_instrument(2);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 15: C4 (Saw) Vol:1 Effect:5
    set_instrument(2);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x105A);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 16: G3 (Saw) Vol:4 Effect:5
    set_instrument(2);
    set_volume(72); // PICO-8 vol 4 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 18: G3 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 19: D#3 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x09B9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 20: F3 (Saw) Vol:3 Effect:5
    set_instrument(2);
    set_volume(54); // PICO-8 vol 3 -> 0-127 range
    start_note(0x0AE9);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 22: G3 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 25: G3 (Saw) Vol:2 Effect:5
    set_instrument(2);
    set_volume(36); // PICO-8 vol 2 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 28: G3 (Saw) Vol:1 Effect:5
    set_instrument(2);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
    // Step 31: G3 (Saw) Vol:1 Effect:5
    set_instrument(2);
    set_volume(18); // PICO-8 vol 1 -> 0-127 range
    start_note(0x0C3F);
    // Apply effect 5 (Fade Out)
    delayTicksT0(128); // 64000.0μs at 2KHz (converted from 4096 T2 ticks)
    stop_note();
    
}

// Individual Music Pattern Functions
void play_music_00() {
    // Music Pattern 00
    // Playing SFX on channels: 1, 2, 3
    play_sfx_10(); // Channel 1: SFX 10
}

void play_music_01() {
    // Music Pattern 01
    // Playing SFX on channels: 1, 2, 3
    play_sfx_22(); // Channel 1: SFX 22
    play_sfx_12(); // Channel 2: SFX 12
}

void play_music_02() {
    // Music Pattern 02
    // Playing SFX on channels: 1, 2, 3
    play_sfx_22(); // Channel 1: SFX 22
    play_sfx_12(); // Channel 2: SFX 12
}

void play_music_03() {
    // Music Pattern 03
    // Playing SFX on channels: 1, 2, 3
    play_sfx_11(); // Channel 1: SFX 11
    play_sfx_12(); // Channel 2: SFX 12
}

void play_music_04() {
    // Music Pattern 04
    // Playing SFX on channels: 1, 2, 3
    play_sfx_19(); // Channel 1: SFX 19
    play_sfx_18(); // Channel 2: SFX 18
}

void play_music_05() {
    // Music Pattern 05
    // Playing SFX on channels: 1, 2, 3
    play_sfx_22(); // Channel 1: SFX 22
    play_sfx_12(); // Channel 2: SFX 12
}

void play_music_06() {
    // Music Pattern 06
    // Playing SFX on channels: 1, 2, 3
    play_sfx_22(); // Channel 1: SFX 22
    play_sfx_12(); // Channel 2: SFX 12
}

void play_music_07() {
    // Music Pattern 07
    // Playing SFX on channels: 1, 2, 3
    play_sfx_18(); // Channel 2: SFX 18
}

void play_music_08() {
    // Music Pattern 08
    // Playing SFX on channels: 1, 2, 3
}

void play_music_09() {
    // Music Pattern 09
    // Playing SFX on channels: 1, 2, 3
}

void play_music_10() {
    // Music Pattern 10
    // Playing SFX on channels: 1, 2, 3
    play_sfx_25(); // Channel 1: SFX 25
    play_sfx_26(); // Channel 2: SFX 26
}

void play_music_11() {
    // Music Pattern 11
    // Playing SFX on channels: 1, 2, 3
    play_sfx_25(); // Channel 1: SFX 25
    play_sfx_26(); // Channel 2: SFX 26
}

void play_music_12() {
    // Music Pattern 12
    // Playing SFX on channels: 1, 2, 3
    play_sfx_27(); // Channel 1: SFX 27
    play_sfx_26(); // Channel 2: SFX 26
}

void play_music_13() {
    // Music Pattern 13
    // Playing SFX on channels: 1, 2, 3
    play_sfx_27(); // Channel 1: SFX 27
    play_sfx_26(); // Channel 2: SFX 26
}

void play_music_14() {
    // Music Pattern 14
    // Playing SFX on channels: 1, 2, 3
    play_sfx_33(); // Channel 1: SFX 33
    play_sfx_26(); // Channel 2: SFX 26
}

void play_music_15() {
    // Music Pattern 15
    // Playing SFX on channels: 1, 2, 3
    play_sfx_26(); // Channel 1: SFX 26
    play_sfx_33(); // Channel 2: SFX 33
}

void play_music_16() {
    // Music Pattern 16
    // Playing SFX on channels: 1, 2, 3
    play_sfx_26(); // Channel 1: SFX 26
    play_sfx_34(); // Channel 2: SFX 34
}

void play_music_17() {
    // Music Pattern 17
    // Playing SFX on channels: 1, 2, 3
    play_sfx_36(); // Channel 2: SFX 36
}

void play_music_18() {
    // Music Pattern 18
    // Playing SFX on channels: 1, 2, 3
}

void play_music_19() {
    // Music Pattern 19
    // Playing SFX on channels: 1, 2, 3
}

void play_music_20() {
    // Music Pattern 20
    // Playing SFX on channels: 1, 2, 3
    play_sfx_39(); // Channel 1: SFX 39
    play_sfx_41(); // Channel 2: SFX 41
}

void play_music_21() {
    // Music Pattern 21
    // Playing SFX on channels: 1, 2, 3
    play_sfx_39(); // Channel 1: SFX 39
    play_sfx_41(); // Channel 2: SFX 41
}

void play_music_22() {
    // Music Pattern 22
    // Playing SFX on channels: 1, 2, 3
    play_sfx_43(); // Channel 1: SFX 43
    play_sfx_41(); // Channel 2: SFX 41
}

void play_music_23() {
    // Music Pattern 23
    // Playing SFX on channels: 1, 2, 3
    play_sfx_43(); // Channel 1: SFX 43
    play_sfx_44(); // Channel 2: SFX 44
}

void play_music_24() {
    // Music Pattern 24
    // Playing SFX on channels: 1, 2, 3
    play_sfx_43(); // Channel 1: SFX 43
    play_sfx_41(); // Channel 2: SFX 41
}

void play_music_25() {
    // Music Pattern 25
    // Playing SFX on channels: 1, 2, 3
    play_sfx_43(); // Channel 1: SFX 43
    play_sfx_44(); // Channel 2: SFX 44
}

void play_music_26() {
    // Music Pattern 26
    // Playing SFX on channels: 1, 2, 3
    play_sfx_45(); // Channel 1: SFX 45
    play_sfx_48(); // Channel 2: SFX 48
}

void play_music_27() {
    // Music Pattern 27
    // Playing SFX on channels: 1, 2, 3
    play_sfx_49(); // Channel 1: SFX 49
    play_sfx_39(); // Channel 2: SFX 39
}

void play_music_28() {
    // Music Pattern 28
    // Playing SFX on channels: 1, 2, 3
    play_sfx_39(); // Channel 2: SFX 39
}

void play_music_29() {
    // Music Pattern 29
    // Playing SFX on channels: 1, 2, 3
}

void play_music_30() {
    // Music Pattern 30
    // Playing SFX on channels: 1, 2, 3
}

void play_music_31() {
    // Music Pattern 31
    // Playing SFX on channels: 1, 2, 3
}

void play_music_32() {
    // Music Pattern 32
    // Playing SFX on channels: 1, 2, 3
}

void play_music_33() {
    // Music Pattern 33
    // Playing SFX on channels: 1, 2, 3
}

void play_music_34() {
    // Music Pattern 34
    // Playing SFX on channels: 1, 2, 3
}

void play_music_35() {
    // Music Pattern 35
    // Playing SFX on channels: 1, 2, 3
}

void play_music_36() {
    // Music Pattern 36
    // Playing SFX on channels: 1, 2, 3
}

void play_music_37() {
    // Music Pattern 37
    // Playing SFX on channels: 1, 2, 3
}

void play_music_38() {
    // Music Pattern 38
    // Playing SFX on channels: 1, 2, 3
}

void play_music_39() {
    // Music Pattern 39
    // Playing SFX on channels: 1, 2, 3
}

void play_music_40() {
    // Music Pattern 40
    // Playing SFX on channels: 1, 2, 3
    play_sfx_58(); // Channel 1: SFX 58
    play_sfx_60(); // Channel 2: SFX 60
}

void play_music_41() {
    // Music Pattern 41
    // Playing SFX on channels: 1, 2, 3
    play_sfx_60(); // Channel 2: SFX 60
}

void play_music_42() {
    // Music Pattern 42
    // Playing SFX on channels: 1, 2, 3
}

void play_music_43() {
    // Music Pattern 43
    // Playing SFX on channels: 1, 2, 3
}

void play_music_44() {
    // Music Pattern 44
    // Playing SFX on channels: 1, 2, 3
}

void play_music_45() {
    // Music Pattern 45
    // Playing SFX on channels: 1, 2, 3
}

void play_music_46() {
    // Music Pattern 46
    // Playing SFX on channels: 1, 2, 3
}

void play_music_47() {
    // Music Pattern 47
    // Playing SFX on channels: 1, 2, 3
}

void play_music_48() {
    // Music Pattern 48
    // Playing SFX on channels: 1, 2, 3
}

void play_music_49() {
    // Music Pattern 49
    // Playing SFX on channels: 1, 2, 3
}

void play_music_50() {
    // Music Pattern 50
    // Playing SFX on channels: 1, 2, 3
}

void play_music_51() {
    // Music Pattern 51
    // Playing SFX on channels: 1, 2, 3
}

void play_music_52() {
    // Music Pattern 52
    // Playing SFX on channels: 1, 2, 3
}

void play_music_53() {
    // Music Pattern 53
    // Playing SFX on channels: 1, 2, 3
}

void play_music_54() {
    // Music Pattern 54
    // Playing SFX on channels: 1, 2, 3
}

void play_music_55() {
    // Music Pattern 55
    // Playing SFX on channels: 1, 2, 3
}

void play_music_56() {
    // Music Pattern 56
    // Playing SFX on channels: 1, 2, 3
}

void play_music_57() {
    // Music Pattern 57
    // Playing SFX on channels: 1, 2, 3
}

void play_music_58() {
    // Music Pattern 58
    // Playing SFX on channels: 1, 2, 3
}

void play_music_59() {
    // Music Pattern 59
    // Playing SFX on channels: 1, 2, 3
}

void play_music_60() {
    // Music Pattern 60
    // Playing SFX on channels: 1, 2, 3
}

void play_music_61() {
    // Music Pattern 61
    // Playing SFX on channels: 1, 2, 3
}

void play_music_62() {
    // Music Pattern 62
    // Playing SFX on channels: 1, 2, 3
}

void play_music_63() {
    // Music Pattern 63
    // Playing SFX on channels: 1, 2, 3
}

// Lookup Functions
void play_sfx(uint8_t sfx_num) {
    switch (sfx_num) {
        case  0: play_sfx_00(); break;
        case  1: play_sfx_01(); break;
        case  2: play_sfx_02(); break;
        case  3: play_sfx_03(); break;
        case  4: play_sfx_04(); break;
        case  5: play_sfx_05(); break;
        case  6: play_sfx_06(); break;
        case  7: play_sfx_07(); break;
        case  8: play_sfx_08(); break;
        case  9: play_sfx_09(); break;
        case 10: play_sfx_10(); break;
        case 11: play_sfx_11(); break;
        case 12: play_sfx_12(); break;
        case 13: play_sfx_13(); break;
        case 14: play_sfx_14(); break;
        case 15: play_sfx_15(); break;
        case 16: play_sfx_16(); break;
        case 17: play_sfx_17(); break;
        case 18: play_sfx_18(); break;
        case 19: play_sfx_19(); break;
        case 20: play_sfx_20(); break;
        case 21: play_sfx_21(); break;
        case 22: play_sfx_22(); break;
        case 23: play_sfx_23(); break;
        case 24: play_sfx_24(); break;
        case 25: play_sfx_25(); break;
        case 26: play_sfx_26(); break;
        case 27: play_sfx_27(); break;
        case 28: play_sfx_28(); break;
        case 29: play_sfx_29(); break;
        case 30: play_sfx_30(); break;
        case 31: play_sfx_31(); break;
        case 32: play_sfx_32(); break;
        case 33: play_sfx_33(); break;
        case 34: play_sfx_34(); break;
        case 35: play_sfx_35(); break;
        case 36: play_sfx_36(); break;
        case 37: play_sfx_37(); break;
        case 38: play_sfx_38(); break;
        case 39: play_sfx_39(); break;
        case 40: play_sfx_40(); break;
        case 41: play_sfx_41(); break;
        case 42: play_sfx_42(); break;
        case 43: play_sfx_43(); break;
        case 44: play_sfx_44(); break;
        case 45: play_sfx_45(); break;
        case 46: play_sfx_46(); break;
        case 47: play_sfx_47(); break;
        case 48: play_sfx_48(); break;
        case 49: play_sfx_49(); break;
        case 50: play_sfx_50(); break;
        case 51: play_sfx_51(); break;
        case 52: play_sfx_52(); break;
        case 53: play_sfx_53(); break;
        case 54: play_sfx_54(); break;
        case 55: play_sfx_55(); break;
        case 56: play_sfx_56(); break;
        case 57: play_sfx_57(); break;
        case 58: play_sfx_58(); break;
        case 59: play_sfx_59(); break;
        case 60: play_sfx_60(); break;
        case 61: play_sfx_61(); break;
        case 62: play_sfx_62(); break;
        default: break; // Invalid or empty SFX
    }
}

void play_music(uint8_t pattern_num) {
    switch (pattern_num) {
        case  0: play_music_00(); break;
        case  1: play_music_01(); break;
        case  2: play_music_02(); break;
        case  3: play_music_03(); break;
        case  4: play_music_04(); break;
        case  5: play_music_05(); break;
        case  6: play_music_06(); break;
        case  7: play_music_07(); break;
        case  8: play_music_08(); break;
        case  9: play_music_09(); break;
        case 10: play_music_10(); break;
        case 11: play_music_11(); break;
        case 12: play_music_12(); break;
        case 13: play_music_13(); break;
        case 14: play_music_14(); break;
        case 15: play_music_15(); break;
        case 16: play_music_16(); break;
        case 17: play_music_17(); break;
        case 18: play_music_18(); break;
        case 19: play_music_19(); break;
        case 20: play_music_20(); break;
        case 21: play_music_21(); break;
        case 22: play_music_22(); break;
        case 23: play_music_23(); break;
        case 24: play_music_24(); break;
        case 25: play_music_25(); break;
        case 26: play_music_26(); break;
        case 27: play_music_27(); break;
        case 28: play_music_28(); break;
        case 29: play_music_29(); break;
        case 30: play_music_30(); break;
        case 31: play_music_31(); break;
        case 32: play_music_32(); break;
        case 33: play_music_33(); break;
        case 34: play_music_34(); break;
        case 35: play_music_35(); break;
        case 36: play_music_36(); break;
        case 37: play_music_37(); break;
        case 38: play_music_38(); break;
        case 39: play_music_39(); break;
        case 40: play_music_40(); break;
        case 41: play_music_41(); break;
        case 42: play_music_42(); break;
        case 43: play_music_43(); break;
        case 44: play_music_44(); break;
        case 45: play_music_45(); break;
        case 46: play_music_46(); break;
        case 47: play_music_47(); break;
        case 48: play_music_48(); break;
        case 49: play_music_49(); break;
        case 50: play_music_50(); break;
        case 51: play_music_51(); break;
        case 52: play_music_52(); break;
        case 53: play_music_53(); break;
        case 54: play_music_54(); break;
        case 55: play_music_55(); break;
        case 56: play_music_56(); break;
        case 57: play_music_57(); break;
        case 58: play_music_58(); break;
        case 59: play_music_59(); break;
        case 60: play_music_60(); break;
        case 61: play_music_61(); break;
        case 62: play_music_62(); break;
        case 63: play_music_63(); break;
        default: break; // Invalid or empty pattern
    }
}

