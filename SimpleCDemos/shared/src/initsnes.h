#ifndef SLOWROM
#define SLOWROM ((uint8_t)0)
#endif

#ifndef FASTROM
#define FASTROM ((uint8_t)1)
#endif

//So if this isn't present vbcc optimizes out some register writes
//Such as, the kind that set the DMA's destination to be OAM
//instead of VRAM
#ifdef __VBCC__
#define FUNCTIONATR __noinline
#else
#define FUNCTIONATR
#endif

// ---- Banked rodata support (collapses to nothing where unneeded) ----
// llvm-mos near-pointers can't carry a data-bank byte, so a large asset that
// won't fit bank 0 is placed in data_bank_1 ($04:8000) and DMA'd with an
// explicit source bank. Other toolchains keep the asset in normal rodata and
// the pointer already carries the bank, so the annotations collapse away.
#if defined(__mos__)
#define PORT_DATA_BANK1 __attribute__((used, section(".rodata.bank1")))
#define LOAD_VRAM_BANK1(src, dest, size) LoadVramBanked((src), 0x04, (dest), (size))
void LoadVramBanked(const unsigned char *pSource, uint8_t bank,
				uint16_t pVRAMDestination, uint16_t cSize);
#else
#define PORT_DATA_BANK1
#define LOAD_VRAM_BANK1(src, dest, size) LoadVram((src), (dest), (size))
#endif

#ifndef _INITSNES
#define _INITSNES
void initSNES(uint8_t ROMSPEED);
FUNCTIONATR void LoadCGRam(const unsigned char *pSource, uint16_t pCGRAMDestination,
				uint16_t cSize);
FUNCTIONATR void LoadVram(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize);
FUNCTIONATR void LoadVramColumn(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize);
FUNCTIONATR void LoadLoVram(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize);
FUNCTIONATR void LoadHiVram(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize);
FUNCTIONATR void ClearVram(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize);
FUNCTIONATR void LoadOAMCopy(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize);
void initOAMCopy(unsigned char *pSource);			

unsigned char* snesXC_getIRQ_ASM_Buffer(void);
void emitWAI(void);
void emitCLI(void);

void initSA1(void);


#endif