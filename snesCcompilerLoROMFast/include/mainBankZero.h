int termM0Init(void);
int termM0PrintString(const char *szInput);
void ButtonTest(void);
void Mode0Text(void);
int LoadOAMCopy(const unsigned char *pSource, uint16_t pVRAMDestination,
				uint16_t cSize, int cChannel);
int initOAMCopy(unsigned char *pSource);