#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include "snes_regs_xc.h"

// Screen module state
// Must be volatile since it's modified by interrupt handler
extern volatile uint16_t Screen_frameCounter;

// Wait for one VBlank frame
void Screen_WaitFrame(void);

// Wait for A frames
void Screen_WaitManyFrames(uint8_t frames);

// Fade screen from black to full brightness (1 frame per step)
void Screen_FadeIn(void);

// Slow fade in (A frames per brightness step)
void Screen_SlowFadeIn(uint8_t framesPerStep);

// Fade screen to black (1 frame per step)
void Screen_FadeOut(void);

// Slow fade out (A frames per brightness step)
void Screen_SlowFadeOut(uint8_t framesPerStep);

// Increment frame counter (call during VBlank)
void Screen_VBlank(void);

// INIDISP constants
#define INIDISP_FORCE  0x80
#define INIDISP_MAX    0x0F

#endif // SCREEN_H
