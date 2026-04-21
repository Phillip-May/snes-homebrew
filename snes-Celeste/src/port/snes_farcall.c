#include "snes_farcall.h"

static volatile uint8_t s_portPrgActiveBank = PORT_PRG_BANK_0;
static volatile uint8_t s_portPrgBankStack[4];
static volatile uint8_t s_portPrgBankSp = 0u;

#ifdef __mos__
extern void snes_prg_bank_switch_pc(uint8_t bank);
#endif

PORT_FIXED_CODE void port_prg_bank_switch(uint8_t bank)
{
    if (bank > PORT_PRG_BANK_7) {
        bank = PORT_PRG_BANK_0;
    }
    s_portPrgActiveBank = bank;
#ifdef __mos__
    snes_prg_bank_switch_pc(bank);
#endif
}

PORT_FIXED_CODE void port_prg_bank_push(uint8_t bank)
{
    if (bank > PORT_PRG_BANK_7) {
        bank = PORT_PRG_BANK_0;
    }
    if (s_portPrgBankSp < (uint8_t)4u) {
        s_portPrgBankStack[s_portPrgBankSp++] = s_portPrgActiveBank;
    }
    s_portPrgActiveBank = bank;
#ifdef __mos__
    snes_prg_bank_switch_pc(bank);
#endif
}

PORT_FIXED_CODE void port_prg_bank_pop(void)
{
    uint8_t bank;
    if (s_portPrgBankSp > 0u) {
        bank = s_portPrgBankStack[--s_portPrgBankSp];
    } else {
        bank = PORT_PRG_BANK_0;
    }
    s_portPrgActiveBank = bank;
#ifdef __mos__
    snes_prg_bank_switch_pc(bank);
#endif
}

