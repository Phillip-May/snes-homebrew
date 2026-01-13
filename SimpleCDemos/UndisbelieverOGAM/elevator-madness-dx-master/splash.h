#ifndef SPLASH_H
#define SPLASH_H

#include <stdint.h>
#include "snes_regs_xc.h"

// VRAM Map (word addresses)
#define SPLASH_BG1_MAP    0x0000
#define SPLASH_BG1_TILES  0x5000
#define SPLASH_BG1_SIZE   0x00  // 32x32

#define SPLASH_TIMEOUT    (5 * 60)
#define CONTROLS_TIMEOUT  (10 * 60)

// Shows splash screen and controls screen
// Returns 0 if user pressed button, 1 if timeout
uint8_t Splash_SplashScreen(void);

#endif // SPLASH_H
