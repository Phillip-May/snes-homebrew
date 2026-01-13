#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "snes_regs_xc.h"
#include "initsnes.h"
#include "controller.h"
#include "screen.h"
#include "metasprite.h"
#include "random.h"
#include "game.h"
#include "splash.h"

// Console message function - writes formatted message to no$sns debug register
void consoleNocashMessage(const char *format, ...) {
    char text_buffer[64];
    va_list ap;
    
    // Format the string using minimal printf implementation
    va_start(ap, format);
    vsprintf(text_buffer, format, ap);
    va_end(ap);
    
    // Write each character to REG_DEBUG until null terminator
    const char *p = text_buffer;
    while (*p != '\0') {
        REG_DEBUG = *p;
        p++;
    }
}

int main(void) {
    // Initialize SNES
    initSNES(SLOWROM);
    
    // Initialize random seed (from main.s)
    // Random_seed is initialized in random.c
    
    // Initialize metasprite system
    // Initialize OAM copy structure (call once at startup)
    initOAMCopy(MetaSprite_oamCopy.Bytes);
    MetaSprite_InitLoop();
    
    // Enable VBlank and Auto-joypad
    REG_NMITIMEN = 0x81; // VBlank enable (bit 7) + Auto-joypad (bit 0)
    
    // Main game loop
    while (1) {
        Splash_SplashScreen();
        Game_PlayGame();
    }
    
    return 0;
}

// VBlank interrupt handler
void snesXC_nmi(void) {
    // Read NMI flag (clears it)
    (void)REG_RDNMI;
    
    // Update screen frame counter
    Screen_VBlank();
    
    // Update metasprite (transfer OAM to hardware)
    MetaSprite_VBlank();
    
    // Transfer interactive background buffer to VRAM if needed
    if (updateBgBufferOnZero == 0) {
        // Transfer interactiveBgBuffer to VRAM at GAME_BG2_MAP
        // Buffer is 32*32 tiles, 2 bytes per tile = 32*32*2 bytes
        // GAME_BG2_MAP is a word address, but LoadVram expects byte address
        // However, for tilemaps we need to use manual DMA with VMAIN=0x80
        REG_VMAIN = 0x80; // Increment high byte first, increment by 1 word
        REG_VMADD = GAME_BG2_MAP; // Word address
        REG_DMAP0 = 0x01; // CPU to PPU, 2 regs write twice (VMDATA low then high)
        REG_BBAD0 = 0x18; // Destination: VMDATA
        REG_A1T0 = (uint16_t)((uint32_t)interactiveBgBuffer & 0xFFFF);
        REG_A1B0 = (uint8_t)((uint32_t)interactiveBgBuffer >> 16);
        REG_DAS0 = 32 * 32 * 2; // 32x32 tiles, 2 bytes per tile
        REG_MDMAEN = 0x01; // Start DMA
        
        // Mark as updated
        updateBgBufferOnZero = 1;
    }
    
    // Update controller
    Controller_Update();
}

// COP interrupt handler
void snesXC_cop(void) {
}

// BRK interrupt handler
void snesXC_brk(void) {
    // BRK handler
}

// ABORT interrupt handler
void snesXC_abort(void) {
    // ABORT handler
}
