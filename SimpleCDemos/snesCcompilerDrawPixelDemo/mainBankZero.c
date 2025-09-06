#pragma section CONST=BANK0
#pragma section CODE=BANK0

#define MY_REGISTER (*(volatile uint32_t*)0x7E2004u)
typedef	unsigned char byte;
//LoROM mememory map
#include <FLOAT.H>
#include <INTTYPES.H>
#include <STDINT.H>
#include <MALLOC.H>
#include <STRING.H>
#include <STDIO.h>

#include "include\mainBankZero.h"
#include "include\crashHandler.h"
#include "include\SNES.h"
#include "include\initsnes.h"
#include "include\imagedata.h"
#include "include\hwMath.h"


const char testString[40] = "Initial string in ROM";
const char testString2[40] = "\nInitial string in ROM 2";
//char testStringRam[40] = "Initial string in User Data";
char testStringRam[40] = "Error";
char testStringRam2[40] = "Error2";
char testStringRam3[40] = "Error3";

void main(void){
	//Variables
	static int something = 5;
	int8_t regRead1; //Variable for storing hardware registers
	int8_t regRead2; //Variable for storing hardware registers
	uint8_t lastInputLo;
	uint8_t lastInputHi;
	uint8_t xOffset = 10;
	
	//Initialization
	//Initialize the stack
	initSNES(SLOWROM);
	
	//LoadVram(school_bin, 0x0000, (uint16_t) school_bin_size, 7);
	//LoadCGRam(school_pal, 0x10, sizeof(school_pal), something);
	
	//Simple recreation of input test	
	
	mode7Init();
	
	M7DemoMain();
	
}














