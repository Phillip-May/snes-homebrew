int termM0Init(void);
int LoadVram(const unsigned char *pSource, uint16_t pVRAMDestination, uint16_t cSize,
			 uint8_t cChannel);
int termM0PrintStringXY(char *szInput, uint8_t inpX, uint8_t inpY);
void ButtonTest(void);