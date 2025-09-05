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

#ifndef _INITSNES
#define _INITSNES
void initSNES(uint8_t ROMSPEED);
FUNCTIONATR int LoadCGRam(const unsigned char *pSource, uint16_t pCGRAMDestination,
				uint16_t cSize, int cChannel);
FUNCTIONATR int LoadVram(const unsigned char *pSource, uint16_t pVRAMDestination, 
				uint16_t cSize, int cChannel);
FUNCTIONATR int LoadLoVram(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize, int cChannel);
FUNCTIONATR int ClearVram(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize, int cChannel);
FUNCTIONATR int LoadOAMCopy(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize, int cChannel);
int initOAMCopy(unsigned char *pSource);			

#endif