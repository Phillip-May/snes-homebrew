#include "screen.h"

// Screen module state
// Must be volatile since it's modified by interrupt handler
volatile uint16_t Screen_frameCounter = 0;

void Screen_WaitFrame(void) {
    // Wait for VBlank by polling REG_RDNMI manually
    // Pattern matches Mode1Sprite example: wait while flag is set (to clear it), then wait for next VBlank
    uint8_t regRead1;
    do {
        regRead1 = REG_RDNMI;
    } while ((regRead1 & 0x80) != 0);  // Wait while VBlank flag (bit 7) is set (reading clears it)
    
    // Now wait for next VBlank to occur
    do {
        regRead1 = REG_RDNMI;
    } while ((regRead1 & 0x80) == 0);  // Wait for VBlank flag (bit 7) to be set
}

void Screen_WaitManyFrames(uint8_t frames) {
    while (frames > 0) {
        Screen_WaitFrame();
        frames--;
    }
}

void Screen_FadeIn(void) {
    Screen_SlowFadeIn(1);
}

void Screen_SlowFadeIn(uint8_t framesPerStep) {
    uint8_t brightness = 0;
    
    while (brightness < 16) {
        REG_INIDISP = brightness;
        Screen_WaitManyFrames(framesPerStep);
        brightness++;
    }
}

void Screen_FadeOut(void) {
    Screen_SlowFadeOut(1);
}

void Screen_SlowFadeOut(uint8_t framesPerStep) {
    // Match assembly: start at 14, decrement until negative
    int8_t brightness = 14;
    
    do {
        REG_INIDISP = (uint8_t)brightness;
        Screen_WaitManyFrames(framesPerStep);
        brightness--;
    } while (brightness >= 0);
    
    // Force blank
    REG_INIDISP = INIDISP_FORCE;
}

void Screen_VBlank(void) {
    Screen_frameCounter++;
}
