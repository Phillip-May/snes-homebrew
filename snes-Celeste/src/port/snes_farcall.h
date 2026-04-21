#ifndef SNES_FARCALL_H
#define SNES_FARCALL_H

#include <stdint.h>

enum {
    PORT_PRG_BANK_0 = 0u,
    PORT_PRG_BANK_1 = 1u,
    PORT_PRG_BANK_2 = 2u,
    PORT_PRG_BANK_3 = 3u,
    PORT_PRG_BANK_4 = 4u,
    PORT_PRG_BANK_5 = 5u,
    PORT_PRG_BANK_6 = 6u,
    PORT_PRG_BANK_7 = 7u
};

/* Mark code that must live in a specific SNES ROM bank. */
#ifdef __mos__
#define PORT_CODE_BANK1 __attribute__((noinline, used, section("rom_bank_1")))
#define PORT_CODE_BANK2 __attribute__((noinline, used, section("rom_bank_2")))
#define PORT_CODE_BANK3 __attribute__((noinline, used, section("rom_bank_3")))
#define PORT_CODE_BANK4 __attribute__((noinline, used, section("rom_bank_4")))
#define PORT_CODE_BANK5 __attribute__((noinline, used, section("rom_bank_5")))
#define PORT_CODE_BANK6 __attribute__((noinline, used, section("rom_bank_6")))
#define PORT_CODE_BANK7 __attribute__((noinline, used, section("rom_bank_7")))
#define PORT_FIXED_CODE __attribute__((noinline, used, section("rom_fixed")))
#else
#define PORT_CODE_BANK1
#define PORT_CODE_BANK2
#define PORT_CODE_BANK3
#define PORT_CODE_BANK4
#define PORT_CODE_BANK5
#define PORT_CODE_BANK6
#define PORT_CODE_BANK7
#define PORT_FIXED_CODE
#endif

#endif
