#include "controller.h"

// Controller module state
uint16_t Controller_pressed = 0;
uint16_t Controller_current = 0;

// Internal state for edge detection
static uint16_t invertedPrevious = 0xFFFF;

void Controller_Update(void) {
    // Wait for AUTOJOY to complete
    // AUTOJOY bit (bit 0) must be clear before reading joypad
    // Read into variable first to avoid multiple volatile reads
    uint8_t hvjoyStatus;
    do {
        hvjoyStatus = REG_HVBJOY;
    } while ((hvjoyStatus & HVJOY_AUTOJOY) != 0);
    
    // Read 16-bit joypad state
    // JOY1L contains A/X/L/R (low byte)
    // JOY1H contains B/Y/SELECT/START/UP/DOWN/LEFT/RIGHT (high byte)
    uint16_t joy1 = REG_JOY1L | ((uint16_t)REG_JOY1H << 8);
    
    // Store current state
    Controller_current = joy1;
    
    // Calculate newly pressed buttons (edge detection)
    // pressed = current & ~previous = current & invertedPrevious
    Controller_pressed = Controller_current & invertedPrevious;
    
    // Update inverted previous for next frame
    // invertedPrevious = ~current
    invertedPrevious = Controller_current ^ 0xFFFF;
}
