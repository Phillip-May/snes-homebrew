#ifndef SLOWROM
#define SLOWROM ((uint8_t)0)
#endif

#ifndef FASTROM
#define FASTROM ((uint8_t)1)
#endif

#ifndef _INITSNES
#define _INITSNES
void initSNES(uint8_t ROMSPEED);
int LoadCGRam(const unsigned char *pSource, uint16_t pCGRAMDestination,
				uint16_t cSize, int cChannel);
int LoadVram(const unsigned char *pSource, uint16_t pVRAMDestination, 
				uint16_t cSize, int cChannel);
int LoadLoVram(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize, int cChannel);
int ClearVram(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize, int cChannel);				
int LoadOAMCopy(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize, int cChannel);
int initOAMCopy(unsigned char *pSource);

int ClearLoVram(const unsigned char *pSource, uint16_t pVRAMDestination,
			 uint16_t cSize, int cChannel);

int LoadHiVram(const unsigned char *pSource, uint16_t pVRAMDestination,
			 uint16_t cSize, int cChannel);			 
			 
#endif