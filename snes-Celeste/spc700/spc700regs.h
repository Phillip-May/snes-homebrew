//SPC700 Registers
#define REG_HWTEST1   (*(volatile uint8_t*)0x00F0)
#define REG_CONTROL   (*(volatile uint8_t*)0x00F1)
#define REG_DSPADDR   (*(volatile uint8_t*)0x00F2)
#define REG_DSPDATA   (*(volatile uint8_t*)0x00F3)
#define REG_APUIO0    (*(volatile uint8_t*)0x00F4)
#define REG_APUIO1    (*(volatile uint8_t*)0x00F5)
#define REG_APUIO2    (*(volatile uint8_t*)0x00F6)
#define REG_APUIO3    (*(volatile uint8_t*)0x00F7)
#define REG_AUXIO4    (*(volatile uint8_t*)0x00F8)
#define REG_AUXIO5    (*(volatile uint8_t*)0x00F9)
#define REG_T0DIV     (*(volatile uint8_t*)0x00FA)
#define REG_T1DIV     (*(volatile uint8_t*)0x00FB)
#define REG_T2DIV     (*(volatile uint8_t*)0x00FC)
#define REG_T0OUT     (*(volatile uint8_t*)0x00FD)
#define REG_T1OUT     (*(volatile uint8_t*)0x00FE)
#define REG_T2OUT     (*(volatile uint8_t*)0x00FF)

//Workaround for uncondtional branches
#define REG_MAGIC   (*(volatile uint8_t*)0xFFF0)
enum DSPRegisters {
  DSP_V0VOLL  = 0x00,
  DSP_V0VOLR  = 0x01,
  DSP_V0PL    = 0x02,
  DSP_V0PH    = 0x03,
  DSP_V0SRCN  = 0x04,
  DSP_V0ADSRL = 0x05,
  DSP_V0ADSRH = 0x06,  
  DSP_V0GAIN  = 0x07,
  DSP_V0ENVX  = 0x08,
  DSP_V0OUTX  = 0x09,
  DSP_C0      = 0x0F,

  DSP_EFB     = 0x0D,
  DSP_EVOLL   = 0x2C,
  DSP_EVOLR   = 0x3C,
  DSP_KON     = 0x4C,
  DSP_KOFF    = 0x5C,
  
  DSP_MVOL0L  = 0x0C,
  DSP_MVOL0R  = 0x1C,  
  DSP_PMON    = 0x20,
  DSP_NON     = 0x3D,
  DSP_EON     = 0x40,
  DSP_ESA     = 0x6D,
  DSP_EDL     = 0x7D,
  
  DSP_FLG     = 0x6C, //RMEN NNNN 	RW 	Flags: soft reset (R), mute all (M), echo disable (E), noise frequency (N). 
  DSP_ENDX    = 0x7C,
  DSP_DIR     = 0x5D, //Upper 2 bytes of the brr directory location
};