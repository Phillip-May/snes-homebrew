//LoROM memory map

#include <stdint.h>

#include "snes_regs_xc.h"
#include "initsnes.h"
#include "build/assets/ball16.inc"
#include "build/assets/ball64.inc"

#define NSPR 10

//Try not to put blocks of memory on stack
union uOAMCopy oamCopy;

static const int16_t initX[NSPR] = {  10,  60, 120, 180, 220,  40, 150,  30, 200,  90 };
static const int16_t initY[NSPR] = {  20,  90, 150,  40, 180, 120,  60, 160,  30, 100 };
static const int8_t  initDX[NSPR] = {   3,  -2,   2,  -3,   1,  -2,   1,  -1,   1,  -1 };
static const int8_t  initDY[NSPR] = {   2,   3,  -3,  -1,   3,  -2,   1,   1,  -2,  -1 };
static const uint8_t large[NSPR]  = {   0,   0,   0,   0,   0,   0,   1,   1,   1,   1 };

static int16_t sx[NSPR], sy[NSPR];
static int8_t  dx[NSPR], dy[NSPR];

#define BANDS    56
#define BAND_H   4
#define HDMA_LEN (BANDS * 5 + 1)

static uint8_t hdmaTable[HDMA_LEN];
static uint8_t gradT;

static uint8_t tri(uint8_t p){
	p &= 63;
	return (p < 32) ? p : (63 - p);
}

static void updateGradient(void){
	uint8_t b, ph;
	uint16_t c;
	for (b = 0; b < BANDS; b++){
		ph = (uint8_t)(b + gradT);
		c = ((uint16_t)tri(ph + 42) << 10)
		  | ((uint16_t)tri(ph + 21) << 5)
		  |  (uint16_t)tri(ph);
		hdmaTable[b * 5 + 3] = (uint8_t)(c & 0xFF);
		hdmaTable[b * 5 + 4] = (uint8_t)(c >> 8);
	}
	gradT++;
}

void main(void){
	uint8_t i, j;
	uint16_t idx;
	uint8_t sh, b;
	int16_t maxx, maxy;
	int16_t si, sj, penX, penY, half, ei, ej;
	int8_t tmp;

	initSNES(SLOWROM);

	LoadVram(ball16_chr, 0x0000, sizeof(ball16_chr));
	LOAD_VRAM_BANK1(ball64_chr, 0x2000, sizeof(ball64_chr));
	LoadCGRam(ball16_pal, 0x80, sizeof(ball16_pal)); // OBJ palette 0

	initOAMCopy(oamCopy.Bytes);

	for (i = 0; i < NSPR; i++){
		sx[i] = initX[i];
		sy[i] = initY[i];
		dx[i] = initDX[i];
		dy[i] = initDY[i];
	}

	for (i = 0; i < NSPR; i++){
		oamCopy.Bytes[i * 4 + 2] = 0x00;
		oamCopy.Bytes[i * 4 + 3] = large[i] ? 0x31 : 0x30;
		idx = 512 + (i >> 2);
		sh = (i & 3) * 2;
		b = oamCopy.Bytes[idx];
		b &= ~(3 << sh);
		if (large[i])
			b |= (2 << sh);
		oamCopy.Bytes[idx] = b;
	}

	gradT = 0;   // seed before first read (don't rely on .bss being zeroed)
	for (i = 0; i < BANDS; i++){
		hdmaTable[i * 5 + 0] = BAND_H;
		hdmaTable[i * 5 + 1] = 0x00;
		hdmaTable[i * 5 + 2] = 0x00;
	}
	hdmaTable[BANDS * 5] = 0x00;
	updateGradient();

	REG_DMAP1 = 0x03;   // A->B, direct, mode 3: $2121,$2121,$2122,$2122
	REG_BBAD1 = 0x21;   // B-bus base = $2121 (CGADD)
#ifdef __TCC816__
	{
		union { const unsigned char *ptr;
		        struct { uint16_t low16; uint8_t high8; } parts; } src;
		src.ptr = hdmaTable;
		REG_A1T1 = src.parts.low16;
		REG_A1B1 = src.parts.high8;
	}
#else
	REG_A1T1 = (uint16_t)((uint32_t)&hdmaTable[0]);
	REG_A1B1 = (uint8_t)(((uint32_t)&hdmaTable[0]) >> 16);
#endif

	REG_OBJSEL = 0x80;   // sss=100: 16x16 small / 64x64 large, name base 0
	REG_TM = 0x10;       // OBJ on main screen
	REG_BGMODE = 0x01;   // mode 1
	REG_NMITIMEN = 0x01; // joypad autoread
	// Enable HDMA only at vblank so it initialises cleanly at the next frame
	// start. Enabling it mid-frame starts the channel with stale internal state,
	// which streamed garbage into CGRAM (corrupting the OBJ palette) on boot.
	while ((REG_HVBJOY & 0x80) != 0) { }
	while ((REG_HVBJOY & 0x80) == 0) { }
	REG_HDMAEN = 0x02;   // gradient HDMA (channel 1)
	REG_INIDISP = 0x0F;  // screen on, full brightness

	while(1){
		while ((REG_HVBJOY & 0x80) != 0) { } // wait out any current vblank
		while ((REG_HVBJOY & 0x80) == 0) { } // wait for next vblank

		LoadOAMCopy(oamCopy.Bytes, 0x0000, sizeof(union uOAMCopy));
		updateGradient();

		for (i = 0; i < NSPR; i++){
			maxx = large[i] ? (256 - 64) : (256 - 16);
			maxy = large[i] ? (224 - 64) : (224 - 16);

			sx[i] += dx[i];
			if (sx[i] < 0){ sx[i] = 0; dx[i] = -dx[i]; }
			else if (sx[i] > maxx){ sx[i] = maxx; dx[i] = -dx[i]; }

			sy[i] += dy[i];
			if (sy[i] < 0){ sy[i] = 0; dy[i] = -dy[i]; }
			else if (sy[i] > maxy){ sy[i] = maxy; dy[i] = -dy[i]; }
		}

		for (i = 0; i < NSPR; i++){
			si = large[i] ? 64 : 16;
			for (j = i + 1; j < NSPR; j++){
				sj = large[j] ? 64 : 16;

				ei = sx[i] + si; ej = sx[j] + sj;
				penX = (ei < ej ? ei : ej) - (sx[i] > sx[j] ? sx[i] : sx[j]);
				if (penX <= 0) continue;
				ei = sy[i] + si; ej = sy[j] + sj;
				penY = (ei < ej ? ei : ej) - (sy[i] > sy[j] ? sy[i] : sy[j]);
				if (penY <= 0) continue;

				if (penX < penY){
					half = (penX + 1) >> 1;
					tmp = dx[i]; dx[i] = dx[j]; dx[j] = tmp;
					if (sx[i] < sx[j]){
						sx[i] -= half; sx[j] += half;
						if (dx[i] > 0) dx[i] = -dx[i];
						if (dx[j] < 0) dx[j] = -dx[j];
					} else {
						sx[i] += half; sx[j] -= half;
						if (dx[i] < 0) dx[i] = -dx[i];
						if (dx[j] > 0) dx[j] = -dx[j];
					}
				} else {
					half = (penY + 1) >> 1;
					tmp = dy[i]; dy[i] = dy[j]; dy[j] = tmp;
					if (sy[i] < sy[j]){
						sy[i] -= half; sy[j] += half;
						if (dy[i] > 0) dy[i] = -dy[i];
						if (dy[j] < 0) dy[j] = -dy[j];
					} else {
						sy[i] += half; sy[j] -= half;
						if (dy[i] < 0) dy[i] = -dy[i];
						if (dy[j] > 0) dy[j] = -dy[j];
					}
				}
			}
		}

		for (i = 0; i < NSPR; i++){
			maxx = large[i] ? (256 - 64) : (256 - 16);
			maxy = large[i] ? (224 - 64) : (224 - 16);
			if (sx[i] < 0) sx[i] = 0; else if (sx[i] > maxx) sx[i] = maxx;
			if (sy[i] < 0) sy[i] = 0; else if (sy[i] > maxy) sy[i] = maxy;

			oamCopy.Bytes[i * 4 + 0] = (uint8_t)sx[i];
			oamCopy.Bytes[i * 4 + 1] = (uint8_t)sy[i];
		}
	}
}

// Cross-compiler interrupt handler implementations
void snesXC_cop(void) {
}

void snesXC_brk(void) {
}

void snesXC_abort(void) {
}

void snesXC_nmi(void) {
}
