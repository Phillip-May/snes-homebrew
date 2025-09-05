//Custom sound driver for the snes demake of pico8 celeste
//Mostly just proving the point that this can be done in c and there's nothing magic about assembly
//Also llvm-mos is a good compiler so this could be extended into a math co processor

#include <stdint.h>
#include "spc700regs.h"
#include "brrSamples.h"

void main();

//For compiling with clang no stdlibc
void _start() {
	main();
}

void writeDSPREG(enum DSPRegisters inputReg,uint8_t value) {
	REG_DSPADDR = inputReg;
	REG_DSPDATA = value;
}

uint8_t readDSPREG(enum DSPRegisters inputReg) {
	REG_DSPADDR = inputReg;
	return REG_DSPDATA;
}



// Using c11 AlignAs
typedef struct {
	uint16_t sampleStart;
	uint16_t sampleLoop;
} BRRSampleTable;
_Alignas(256) BRRSampleTable Global_SampleTable;


void start_note(uint16_t pitch) {
	
	//Init_note
	writeDSPREG(DSP_V0PL,pitch);
	writeDSPREG(DSP_V0PH,pitch >> 8);
	
	writeDSPREG(DSP_V0SRCN, 0x00);
	writeDSPREG(DSP_V0ADSRL,0xCF);
	writeDSPREG(DSP_V0ADSRH,0x50);

	writeDSPREG(DSP_KON, 0x01);
}
	

volatile uint8_t global_ExitFlag = 1;
volatile uint8_t global_spinCounter = 0;

void play_sfx_00();
void play_sfx_01();
void play_sfx_02();
void play_sfx_03();
void play_sfx_04();
void play_sfx_05();
void play_sfx_06();
void play_sfx_07();


void set_instrument(uint8_t insrument) {
}

void set_volume(uint8_t value) {
	writeDSPREG(DSP_V0VOLL,value);
    writeDSPREG(DSP_V0VOLR,value);
}

void stop_note() {
}

void waitNoteFinished() {
    // Wait until voice 0 finishes playing (bit 0 of ENDX register)
    while (!(readDSPREG(DSP_ENDX) & 0x01)) {
		global_spinCounter++;
    }
}

void delayTicksT0(uint8_t numTicks) {
	while (numTicks > 0) {
		// Capture current timer value
		unsigned char prev = REG_T0OUT;
		// Wait until the timer value changes (tick)
		while (prev == REG_T0OUT); // spin/wait
		// One beat elapsed
		numTicks--;
	}
}

void delayTicksT2(uint16_t numTicks) {
	while (numTicks > 0) {
		// Capture current timer value
		unsigned char prev = REG_T2OUT;
		// Wait until the timer value changes (tick)
		while (prev == REG_T2OUT); // spin/wait
		// One beat elapsed
		numTicks--;
	}
}

__attribute__((always_inline)) void comms_poll(void);

// Function to clear communication ports
// Control Register bits:
// PC32 (bit 5): Clear ports 2 & 3
// PC10 (bit 4): Clear ports 0 & 1
// ST2, ST1, ST0 (bits 2,1,0): Timer enable bits
__attribute__((always_inline)) void comms_clear_ports() {
    // Clear ports 0-1 (PC10 bit)
    REG_CONTROL = 0x85 | 0x10;  // Set PC10 bit
    REG_CONTROL = 0x85;         // Clear PC10 bit
    
    // Clear ports 2-3 (PC32 bit)  
    REG_CONTROL = 0x85 | 0x20;  // Set PC32 bit
    REG_CONTROL = 0x85;         // Clear PC32 bit
}

void main() {
  int i = 0;
  Global_SampleTable.sampleStart = (uint16_t) &sample_1000HzSqaureWave;
  Global_SampleTable.sampleLoop = (uint16_t) &sample_1000HzSqaureWave;
  

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
  REG_T0DIV = 4; //8000 Hz / 4 = 2000 Hz clock
  REG_T2DIV = 1; //64000 Hz / 1 = 64000 Hz clock2

  REG_CONTROL = 0x85;    // enable IPL ROM, TIMER0 and TIMER2
  
  // Clear communication ports to ensure clean state
  comms_clear_ports();
  
  //Test tone
  //start_note(0x1000);    
  //delayTicksT0(8);  //8*(1/8000 HZ) 0.001s = 1000 Hz, frequency of the square wave
	  
  
  // Poll for SFX commands from SNES
	while(1) {
		comms_poll();
		for (uint16_t j = 0; j < 1000; j++) {
			  global_spinCounter++;
		} 
	}	   
}

// Communication command types
typedef enum {
    CMD_PLAY_SFX = 0x01,        // Play sound effect (data = SFX ID)
    CMD_SET_VOLUME = 0x02,      // Set volume (data = volume level 0-127)
    CMD_STOP_SOUND = 0x03,      // Stop all sounds
    CMD_PLAY_SFX_ID = 0x04,     // Play specific SFX by ID (data = SFX ID 0x00-0x07, 0x17 for music)
    CMD_GET_STATUS = 0x05,      // Get status (returns status in port 2, data in port 3)
    CMD_ERROR = 0xFF            // Error status (unknown command)
} CommCommand;

// Communication protocol variables
// Protocol: SNES writes command type to port 1 (2141), data to port 0 (2140)
// SPC reads these values and processes the command, then echoes back port 0 value
// This prevents conflicts and ensures reliable communication
volatile uint8_t comms_v = 0;  // Last value written to port 0
volatile uint8_t comms_command = 0;  // Current command being processed
volatile uint8_t comms_data = 0;     // Command data


__attribute__((always_inline)) void comms_poll(void) {
	static int i;
    static uint8_t port0_value;
	port0_value = REG_APUIO0;
	REG_APUIO3 = 0x99;
    if (port0_value != comms_v) {
        // Wait a few cycles to avoid read/write conflicts
        static volatile uint8_t dummy;
		dummy = 0;
        for (i = 0; i < 5; i++) {
            dummy++;
        }        
        // Re-read to ensure we have stable data
        static uint8_t stable_port0;
        static uint8_t stable_port1;
		stable_port0 = REG_APUIO0;
		stable_port1 = REG_APUIO1;
        
        // Only process if the data is still different (avoid race conditions)
        if (stable_port0 != comms_v) {
            comms_command = stable_port1;
            comms_data = stable_port0;
            comms_v = comms_data;
            
            // Process SFX commands directly
            if (comms_command == CMD_PLAY_SFX_ID) {  // Play specific SFX by ID
                switch (comms_data) {
                    case 0x00: play_sfx_00(); break;
                    case 0x01: play_sfx_01(); break;
                    case 0x02: play_sfx_02(); break;
                    case 0x03: play_sfx_03(); break;
                    case 0x04: play_sfx_04(); break;
                    case 0x05: play_sfx_05(); break;
                    case 0x06: play_sfx_06(); break;
                    case 0x07: play_sfx_07(); break;
                    default: break;
                }
            }
            
            // Echo back the port 0 value to complete the handshake
            REG_APUIO0 = comms_v;
        }
    }
	comms_clear_ports();
}

#include "C:\Users\Admin\Downloads\New_folder_21\SNES\python\15133.p8_playback.c"

