#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../../shared/src/snes_regs_xc.h"
#include "../../shared/src/initsnes.h"
#include "../../src/snes_font.h"
#include "spc_payload.h"

#define CMD_STOP_ALL    0x03u
#define CMD_PLAY_SFX_ID 0x04u
#define CMD_PLAY_MUSIC  0x06u

#define STATUS_IDLE 0x00u
#define STATUS_PASS 0x55u
#define STATUS_FAIL 0xEEu

#define TEST_MUSIC_COUNT 5u
static const uint8_t kMusicPatterns[TEST_MUSIC_COUNT] = {0u, 10u, 20u, 30u, 40u};

// Required by shared LLVM-MOS vector table.
void snesXC_nmi(void) {}
void snesXC_brk(void) {}
void snesXC_cop(void) {}
void snesXC_abort(void) {}

static uint8_t g_last_data = 0xFFu;
static uint8_t g_force_data = 0x80u;
static uint8_t g_sfx_cmd_toggle = 0u;

static void wait_vblank(void) {
    while ((REG_RDNMI & 0x80u) == 0u) {
    }
}

static void term_init(void) {
    static const uint8_t bg_pal[] = {0x00, 0x00, 0xFF, 0x7F};
    static const uint8_t blank_tile[] = {0x20, 0x00};

    LoadCGRam(bg_pal, 0x00, sizeof(bg_pal));
    LoadLoVram(SNESFONT_bin, 0x0000, sizeof(SNESFONT_bin));
    ClearVram(blank_tile, 0xF800, 0x400);

    REG_BGMODE = 0x08;
    REG_BG1SC = 0xFC;
    REG_BG12NBA = 0x00;
    REG_TM = 0x01;

    REG_BG1HOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG1VOFS = 0;

    REG_INIDISP = 0x0F;
}

static void term_print(uint8_t x, uint8_t y, const char *text) {
    if (text == NULL) {
        return;
    }
    LoadLoVram((const uint8_t *)text, (uint16_t)(0xF800u + x + ((uint16_t)y * 0x40u)), (uint16_t)strlen(text));
}

static void term_print_u8_hex(uint8_t x, uint8_t y, uint8_t v) {
    char buf[3];
    static const char h[] = "0123456789ABCDEF";
    buf[0] = h[(v >> 4) & 0x0F];
    buf[1] = h[v & 0x0F];
    buf[2] = '\0';
    term_print(x, y, buf);
}

static void term_print_u8_bits(uint8_t x, uint8_t y, uint8_t v) {
    char buf[9];
    uint8_t i;
    for (i = 0; i < 8u; i++) {
        uint8_t mask = (uint8_t)(0x80u >> i);
        buf[i] = (v & mask) ? '1' : '0';
    }
    buf[8] = '\0';
    term_print(x, y, buf);
}

static bool spc_wait_boot(uint32_t timeout_frames) {
    while (timeout_frames > 0u) {
        wait_vblank();
        if (REG_APUIO0 == 0xAAu && REG_APUIO1 == 0xBBu) {
            return true;
        }
        timeout_frames--;
    }
    return false;
}

static bool spc_transfer_block(const uint8_t *src, uint16_t dest, uint16_t size) {
    uint8_t token;
    uint16_t i;
    uint32_t timeout;

    REG_APUIO2 = (uint8_t)(dest & 0xFFu);
    REG_APUIO3 = (uint8_t)(dest >> 8);

    token = (uint8_t)(REG_APUIO0 + 0x22u);
    if (token == 0u) {
        token = 1u;
    }

    REG_APUIO1 = token;
    REG_APUIO0 = token;

    timeout = 250000u;
    while (REG_APUIO0 != token) {
        if (timeout == 0u) {
            return false;
        }
        timeout--;
    }

    for (i = 0; i < size; i++) {
        REG_APUIO1 = src[i];
        REG_APUIO0 = (uint8_t)i;

        timeout = 250000u;
        while (REG_APUIO0 != (uint8_t)i) {
            if (timeout == 0u) {
                return false;
            }
            timeout--;
        }
    }

    return true;
}

static bool spc_execute(uint16_t start_addr) {
    uint8_t exec_token;
    uint32_t timeout;

    REG_APUIO2 = (uint8_t)(start_addr & 0xFFu);
    REG_APUIO3 = (uint8_t)(start_addr >> 8);
    REG_APUIO1 = 0;

    exec_token = (uint8_t)(REG_APUIO0 + 2u);
    REG_APUIO0 = exec_token;

    timeout = 250000u;
    while (REG_APUIO0 != exec_token) {
        if (timeout == 0u) {
            return false;
        }
        timeout--;
    }

    return true;
}

static bool spc_upload_image(void) {
    uint16_t ci;
    for (ci = 0; ci < SPC_CHUNK_COUNT; ci++) {
        uint16_t dest = (uint16_t)(spc_load_addr + (uint16_t)(ci * SPC_CHUNK_SIZE));
        uint16_t remaining = (uint16_t)(spc_load_size - (uint16_t)(ci * SPC_CHUNK_SIZE));
        uint16_t size = remaining < (uint16_t)SPC_CHUNK_SIZE ? remaining : (uint16_t)SPC_CHUNK_SIZE;
        if (!spc_transfer_block(spc_chunks[ci], dest, size)) {
            return false;
        }
    }
    return true;
}

static bool spc_wait_runtime_ready(uint32_t timeout_frames) {
    while (timeout_frames > 0u) {
        wait_vblank();
        if (REG_APUIO3 == 0x99u) {
            return true;
        }
        timeout_frames--;
    }
    return false;
}

static bool spc_send_cmd(uint8_t cmd, uint8_t data) {
    uint32_t timeout = 200000u;
    REG_APUIO1 = cmd;
    REG_APUIO0 = data;
    while (REG_APUIO0 != data) {
        if (timeout == 0u) {
            return false;
        }
        timeout--;
    }
    g_last_data = data;
    return true;
}

static bool spc_force_stop(void) {
    uint8_t token0;
    uint8_t token1;

    // Ensure stop token changes APUIO0 edge as seen by SPC comms_poll().
    do {
        g_force_data++;
    } while (g_force_data == g_last_data || g_force_data == REG_APUIO0);
    token0 = g_force_data;

    if (!spc_send_cmd(CMD_STOP_ALL, token0)) {
        return false;
    }

    // Send a second stop token to tolerate transient race/missed poll.
    do {
        g_force_data++;
    } while (g_force_data == token0 || g_force_data == REG_APUIO0);
    token1 = g_force_data;
    if (!spc_send_cmd(CMD_STOP_ALL, token1)) {
        return false;
    }

    return true;
}

static bool spc_play_music(uint8_t pattern) {
    return spc_send_cmd(CMD_PLAY_MUSIC, pattern);
}

static bool spc_play_sfx(uint8_t sfx_id) {
    // Alternate SFX command IDs so repeated same-ID SFX still creates
    // a new (cmd,data) tuple for the SPC comms dedupe logic.
    uint8_t cmd = (g_sfx_cmd_toggle & 1u) ? CMD_PLAY_SFX_ID : 0x01u;
    g_sfx_cmd_toggle ^= 1u;
    return spc_send_cmd(cmd, sfx_id);
}

static void wait_autojoy_ready(void) {
    while (REG_HVBJOY & 0x01u) {
    }
}

static bool run_self_test(void) {
    uint8_t i;

    if (!spc_force_stop()) return false;

    for (i = 0; i < TEST_MUSIC_COUNT; i++) {
        if (!spc_play_music(kMusicPatterns[i])) return false;
        wait_vblank(); wait_vblank(); wait_vblank(); wait_vblank();
    }

    if (!spc_force_stop()) return false;

    for (i = 0; i < 8u; i++) {
        if (!spc_play_sfx((uint8_t)(i * 3u))) return false;
        wait_vblank(); wait_vblank();
    }

    if (!spc_force_stop()) return false;

    return true;
}

int main(void) {
    uint8_t music_idx = 0;
    uint8_t sfx_id = 0;
    uint8_t prev_joy1l = 0;
    uint8_t prev_joy1h = 0;
    bool ok;

    initSNES(SLOWROM);
    REG_NMITIMEN = 0x81; // NMI + auto joypad read
    term_init();

    term_print(0, 0, "SPC700 INTEGRATION TEST");
    term_print(0, 2, "BOOT: waiting for SPC IPL...");

    ok = spc_wait_boot(180u);
    if (!ok) {
        term_print(0, 3, "FAIL: SPC boot timeout");
        REG_APUIO2 = STATUS_FAIL;
        for (;;) { wait_vblank(); }
    }

    term_print(0, 3, "BOOT: OK                 ");
    term_print(0, 4, "UPLOAD: transferring 64KB");

    ok = spc_upload_image();
    if (!ok) {
        term_print(0, 5, "FAIL: upload error");
        REG_APUIO2 = STATUS_FAIL;
        for (;;) { wait_vblank(); }
    }

    term_print(0, 5, "UPLOAD: OK              ");
    term_print(0, 6, "EXEC: launching SPC code");

    ok = spc_execute(spc_start_pc);
    if (!ok) {
        term_print(0, 7, "FAIL: execute error");
        REG_APUIO2 = STATUS_FAIL;
        for (;;) { wait_vblank(); }
    }

    ok = spc_wait_runtime_ready(180u);
    if (!ok) {
        term_print(0, 7, "FAIL: no runtime heartbeat");
        REG_APUIO2 = STATUS_FAIL;
        for (;;) { wait_vblank(); }
    }

    term_print(0, 7, "RUNTIME: OK (APUIO3=99)   ");
    term_print(0, 9, "UP/DOWN: SONG  A:PLAY SONG");
    term_print(0,10, "L/R: SFX ID    B:PLAY SFX ");
    term_print(0,11, "X:STOP ALL     START:AUTO ");

    REG_APUIO2 = STATUS_IDLE;

    for (;;) {
        uint8_t raw_joy1l;
        uint8_t raw_joy1h;
        uint8_t edge_lo;
        uint8_t edge_hi;

        wait_vblank();
        wait_autojoy_ready();
        raw_joy1l = REG_JOY1L; // AXLR
        raw_joy1h = REG_JOY1H; // BYETUDLR
        edge_hi = (uint8_t)(raw_joy1l & (uint8_t)~prev_joy1l); // AXLR edges
        edge_lo = (uint8_t)(raw_joy1h & (uint8_t)~prev_joy1h); // BYETUDLR edges
        prev_joy1l = raw_joy1l;
        prev_joy1h = raw_joy1h;

        if (edge_lo & JOY_UP_MASK) {
            if (music_idx == 0u) {
                music_idx = TEST_MUSIC_COUNT - 1u;
            } else {
                music_idx--;
            }
        }
        if (edge_lo & JOY_DOWN_MASK) {
            music_idx = (uint8_t)((music_idx + 1u) % TEST_MUSIC_COUNT);
        }
        if (edge_lo & JOY_LEFT_MASK) {
            sfx_id = (uint8_t)((sfx_id == 0u) ? 63u : (sfx_id - 1u));
        }
        if (edge_lo & JOY_RIGHT_MASK) {
            sfx_id = (uint8_t)((sfx_id + 1u) & 63u);
        }

        if (edge_hi & JOY_A_MASK) {
            if (!spc_play_music(kMusicPatterns[music_idx])) {
                REG_APUIO2 = STATUS_FAIL;
            }
        }
        if (edge_lo & JOY_B_MASK) {
            if (!spc_play_sfx(sfx_id)) {
                REG_APUIO2 = STATUS_FAIL;
            }
        }
        // Stop alias: X (SNES high byte) and Y (SNES low byte).
        // Some emulator keyboard layouts map "X key" to SNES Y by default.
        if ((edge_hi & JOY_X_MASK) || (edge_lo & JOY_Y_MASK)) {
            if (!spc_force_stop()) {
                REG_APUIO2 = STATUS_FAIL;
            }
        }
        if (edge_lo & JOY_START_MASK) {
            bool pass = run_self_test();
            REG_APUIO2 = pass ? STATUS_PASS : STATUS_FAIL;
        }

        term_print(0, 13, "Selected Song:");
        term_print_u8_hex(14, 13, kMusicPatterns[music_idx]);

        term_print(0, 14, "Selected SFX :");
        term_print_u8_hex(14, 14, sfx_id);

        term_print(0, 16, "Last APUIO0 echo:");
        term_print_u8_hex(17, 16, REG_APUIO0);

        term_print(0, 17, "APUIO2 status :");
        term_print_u8_hex(17, 17, REG_APUIO2);

        // Live controller debug (below main menu/status)
        term_print(0, 19, "JOY1L raw bits:");
        term_print_u8_bits(15, 19, raw_joy1l);
        term_print(24, 19, "hex:");
        term_print_u8_hex(28, 19, raw_joy1l);

        term_print(0, 20, "JOY1H raw bits:");
        term_print_u8_bits(15, 20, raw_joy1h);
        term_print(24, 20, "hex:");
        term_print_u8_hex(28, 20, raw_joy1h);

        term_print(0, 21, "EDGE L bits   :");
        term_print_u8_bits(15, 21, edge_lo);
        term_print(24, 21, "hex:");
        term_print_u8_hex(28, 21, edge_lo);

        term_print(0, 22, "EDGE H bits   :");
        term_print_u8_bits(15, 22, edge_hi);
        term_print(24, 22, "hex:");
        term_print_u8_hex(28, 22, edge_hi);
    }
}
