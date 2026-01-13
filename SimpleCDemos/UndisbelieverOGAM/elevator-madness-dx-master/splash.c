#include "splash.h"
#include "screen.h"
#include "random.h"
#include "controller.h"
#include "initsnes.h"
#include "resources.h"

// Include actual resource definitions
#include "resources/images4bpp/splash.h"
#include "resources/images4bpp/controls.h"

static uint8_t Splash_FadeInOutScreen(uint16_t timeout);

void Splash_SetupScreen(void);

uint8_t Splash_SplashScreen(void) {
    Splash_SetupScreen();
    
    while (1) {
        // Force blank
        REG_INIDISP = 0x80; // INIDISP_FORCE
        
        // Load splash screen
        // Load tiles first (4bpp bitplane format - VMAIN=0x80 is set by LoadVram)
        // SPLASH_BG1_TILES is a word address, but LoadVram expects byte address
        // Convert: byte_addr = word_addr * 2
        LoadVram((const uint8_t*)splashTiles, SPLASH_BG1_TILES * 2, SPLASHTILES_SIZE);
        
        // Load tilemap (16-bit words)
        // SPLASH_BG1_MAP is a word address (0x0000), but LoadVram expects byte address
        // So we need to convert: byte_addr = word_addr * 2
        // But LoadVram does (pVRAMDestination >> 1) internally, so we pass byte address
        // For manual DMA, VMADD expects word address, so we use SPLASH_BG1_MAP directly
        REG_VMAIN = 0x80; // Increment high byte first, increment by 1 word (matches assembly)
        REG_VMADD = SPLASH_BG1_MAP; // Word address (VMADD expects word address, SPLASH_BG1_MAP is already word address)
        REG_DMAP0 = 0x01; // CPU to PPU (bit 7=0), DMAP_TRANSFER_2REGS (bits 0-2=001)
        REG_BBAD0 = 0x18; // Destination: VMDATA (0x2118)
        REG_A1T0 = (uint16_t)((uint32_t)splashMap & 0xFFFF);
        REG_A1B0 = (uint8_t)((uint32_t)splashMap >> 16);
        REG_DAS0 = SPLASHMAP_SIZE; // Size in bytes
        REG_MDMAEN = 0x01; // Start DMA channel 0
        
        LoadCGRam((const uint8_t*)splashPalette, 0, SPLASHPALETTE_SIZE * 2);  // 16 colors * 2 bytes
        
        if (Splash_FadeInOutScreen(SPLASH_TIMEOUT) == 0) {
            return 0;  // User pressed button
        }
        
        // Force blank
        REG_INIDISP = 0x80;
        
        // Load controls screen
        // Load tiles first (4bpp bitplane format - VMAIN=0x80 is set by LoadVram)
        // SPLASH_BG1_TILES is a word address, but LoadVram expects byte address
        // Convert: byte_addr = word_addr * 2
        LoadVram((const uint8_t*)controlsTiles, SPLASH_BG1_TILES * 2, CONTROLSTILES_SIZE);
        
        // Load tilemap (16-bit words)
        // SPLASH_BG1_MAP is a word address (0x0000), VMADD expects word address
        REG_VMAIN = 0x80; // Increment high byte first, increment by 1 word (matches assembly)
        REG_VMADD = SPLASH_BG1_MAP; // Word address (VMADD expects word address, SPLASH_BG1_MAP is already word address)
        REG_DMAP0 = 0x01; // CPU to PPU, 2 regs write twice (VMDATA low then high)
        REG_BBAD0 = 0x18; // Destination: VMDATA
        REG_A1T0 = (uint16_t)((uint32_t)controlsMap & 0xFFFF);
        REG_A1B0 = (uint8_t)((uint32_t)controlsMap >> 16);
        REG_DAS0 = CONTROLSMAP_SIZE;
        REG_MDMAEN = 0x01; // Start DMA
        
        LoadCGRam((const uint8_t*)controlsPalette, 0, CONTROLSPALETTE_SIZE * 2);  // 16 colors * 2 bytes
        
        if (Splash_FadeInOutScreen(CONTROLS_TIMEOUT) == 0) {
            return 0;  // User pressed button
        }
    }
}

static uint8_t Splash_FadeInOutScreen(uint16_t timeout) {
    uint16_t timer = timeout;
    
    Screen_FadeIn();
    
    while (1) {
        Screen_WaitFrame();
        Random_AddJoypadEntropy();
        
        if (timer > 0) {
            timer--;
        }
        if (timer == 0) {
            Screen_FadeOut();
            return 1;  // Timeout
        }
        
        // Check if button pressed
        if (Controller_current & (JOY_BUTTONS | JOY_START | JOY_SELECT)) {
            Screen_FadeOut();
            return 0;  // User pressed button
        }
    }
}

void Splash_SetupScreen(void) {
    // Force blank
    REG_INIDISP = 0x80;
    
    // Set BG mode 2
    REG_BGMODE = 0x02;
    
    // Reset scroll registers (write twice)
    REG_BG1HOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2HOFS = 0;
    
    // Set VRAM base addresses and size
    // BG1SC format: aaaa aayx where aaaa aaaa = address>>10, yx = size (00=32x32)
    // SPLASH_BG1_MAP = 0x0000, size = 32x32 (0x00)
    REG_BG1SC = ((SPLASH_BG1_MAP / 1024) << 2) | 0x00; // 32x32 tilemap
    // BG12NBA format: bbbb aaaa where aaaa = BG1 tiles>>12, bbbb = BG2 tiles>>12
    REG_BG12NBA = (SPLASH_BG1_TILES / 4096);
    
    // Enable BG1 only
    REG_TM = 0x01; // TM_BG1
}
