#ifndef SLOWROM
#define SLOWROM ((uint8_t)0)
#endif

#ifndef FASTROM
#define FASTROM ((uint8_t)1)
#endif

#ifndef _INITSNES
#define _INITSNES
void initSNES(uint8_t ROMSPEED);

#endif