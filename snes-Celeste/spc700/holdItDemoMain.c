//Custom sound driver for the snes demake of pico8 celeste
//Mostly just proving the point that this can be done in c and there's nothing magic about assembly
//Also llvm-mos is a good compiler so this could be extended into a math co processor

#include <stdint.h>
#include "spc700regs.h"
#include "brrSamples.h"

void writeDSPREG(enum DSPRegisters inputReg,uint8_t value) {
	REG_DSPADDR = inputReg;
	REG_DSPDATA = value;
}

// Using c11 AlignAs
typedef struct {
	uint16_t sampleStart;
	uint16_t sampleLoop;
} BRRSampleTable;
_Alignas(256) BRRSampleTable Global_SampleTable;


void start_note(uint16_t pitch, uint8_t chan, uint8_t inst) {
	
	//Init_note
	writeDSPREG(DSP_V0VOLL,127); //Max volume
    writeDSPREG(DSP_V0VOLR,127); //Max volume
	writeDSPREG(DSP_V0PL,pitch);
	writeDSPREG(DSP_V0PH,pitch >> 8);
	
	writeDSPREG(DSP_V0SRCN, 0x00);
	writeDSPREG(DSP_V0ADSRL,0xCF);
	writeDSPREG(DSP_V0ADSRH,0x28);
	writeDSPREG(DSP_V0GAIN, 0xCF);
	
	writeDSPREG(DSP_KON, 0x01 << chan);
}
	

static const uint32_t TIMER_BPM = 120;
static const uint32_t BASE_HZ = 570*10;
static const uint32_t SAMPLES_PER_CYCLE = 2; // number of samples in fundamental wave cycle
static const uint32_t BASE_PITCH = BASE_HZ*10000/78125*SAMPLES_PER_CYCLE;

void delay_beats(uint8_t numBeats) {
	while (numBeats > 0) {
		// Capture current timer value
		unsigned char prev = REG_T0OUT;
		// Wait until the timer value changes (tick)
		while (prev == REG_T0OUT); // spin/wait
		// One beat elapsed
		numBeats--;
	}
}

void main() {
  int i = 0;
  Global_SampleTable.sampleStart = (uint16_t) &sample_holdit;
  Global_SampleTable.sampleLoop = (uint16_t) &sample_holdit;
  

  //DSPInit start
  writeDSPREG(DSP_FLG,0x20); //Unmute
  writeDSPREG(DSP_KON,0x00); //Reset Key on flags
  writeDSPREG(DSP_KOFF,0xFF); //Set Key off flags
  writeDSPREG(DSP_PMON,0x00); //Reset pitch modulation Enable Flags
  writeDSPREG(DSP_NON,0x00); //Reset Noise Enable Flags
  writeDSPREG(DSP_EON,0x00); //Reset Echo on Flags
  writeDSPREG(DSP_EVOLL,0x00); //Reset Echo Volume Left
  writeDSPREG(DSP_EVOLR,0x00); //Reset Echo Volume Right
  //DSPInit end
  writeDSPREG(DSP_DIR,((uint16_t)&Global_SampleTable) >> 8);
  writeDSPREG(DSP_KOFF,0x00);
  writeDSPREG(DSP_MVOL0L,127); //Max volume
  writeDSPREG(DSP_MVOL0R,127); //Max volume
  
  //Timer 0 TIMER_BPM
  REG_T0DIV = (8000 + TIMER_BPM/2) / TIMER_BPM;
  REG_CONTROL = 0x81;    // enable IPL ROM and TIMER0
  
  start_note((BASE_PITCH+100)*5/4, 0, 1);
  delay_beats(100);
  
  start_note((BASE_PITCH+140)*5/4, 0, 1);
  delay_beats(100);

  start_note((BASE_PITCH+180)*5/4, 0, 1);  

  //writeDSPREG(DSP_V0SRCN,0);  
  
  //So onconditional branches are broken I think, can't put while (1)
  // != also causes a compiler crash when the code in the loop is slightly complicated 
  while (REG_MAGIC < 0xFD) { 
	  REG_APUIO0 = i;
	  i++;
	  REG_APUIO1 = i;
	  i++;
	  REG_APUIO2 = i;
	  i++;
	  REG_APUIO3 = i;
	  i++;	  
  }	
}

//For compiling with clang no stdlibc
void _start() {
	main();
}