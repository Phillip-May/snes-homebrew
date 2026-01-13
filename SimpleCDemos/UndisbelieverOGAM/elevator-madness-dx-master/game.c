#include "game.h"
#include "screen.h"
#include "metasprite.h"
#include "random.h"
#include "math.h"
#include "controller.h"
#include "initsnes.h"
#include "resources.h"
#include <string.h>

// Forward declaration for debug function
extern void consoleNocashMessage(const char *format, ...);

// Include actual resource definitions
#include "resources/images4bpp/background.h"
#include "resources/tiles4bpp/interactive-tiles.h"
#include "resources/metasprites/player.h"
#include "resources/metasprites/business-man.h"
#include "resources/metasprites/strike-animations.h"

// Forward declarations (will be implemented in other modules)
void Player_Init(void);
void Player_Process(void);
void Player_DrawSprites(void);
void Elevators_Init(void);
void Elevators_Process(void);
void Npcs_Init(void);
void Npcs_Process(void);
void Npcs_DrawSprites(void);
void Elevators_StrikeAnimationRopeBreaking(void);
void Npcs_StrikeAnimationWrongFloor(void);
void Npcs_StrikeAnimationFighting(void);

// Game module state
uint8_t Game_strikes = 0;
uint16_t Game_score = 0;
StrikeAnimationType Game_strikeAnimationType = STRIKE_ANIMATION_NONE;
uint16_t Game_strikeTimeout = 0;

// Interactive background buffer (32x32 tiles, 2 bytes per tile = word per tile)
uint16_t interactiveBgBuffer[32 * 32];
uint8_t updateBgBufferOnZero = 1;  // Start at 1 so first VBlank will transfer (after Game_InitForLife sets it to 0)

// Strike animation tables
static const uint16_t StrikeTableTop[4][4] = {
    {0, 0, 0, 0},  // 0 strikes
    {STRIKES_FULL_TOP_TILE | STRIKES_TILE_CHARATTR, 0, 0, 0},  // 1 strike
    {STRIKES_FULL_TOP_TILE | STRIKES_TILE_CHARATTR,
     STRIKES_HALF_TOP_TILE | STRIKES_TILE_CHARATTR,
     STRIKES_HALF_TOP_TILE | (STRIKES_TILE_CHARATTR ^ TILEMAP_H_FLIP_FLAG),
     0},  // 2 strikes
    {STRIKES_FULL_TOP_TILE | STRIKES_TILE_CHARATTR,
     STRIKES_HALF_TOP_TILE | STRIKES_TILE_CHARATTR,
     STRIKES_HALF_TOP_TILE | (STRIKES_TILE_CHARATTR ^ TILEMAP_H_FLIP_FLAG),
     STRIKES_FULL_TOP_TILE | STRIKES_TILE_CHARATTR}  // 3 strikes
};

static const uint16_t StrikeTableBottom[4][4] = {
    {0, 0, 0, 0},  // 0 strikes
    {STRIKES_FULL_BOTTOM_TILE | STRIKES_TILE_CHARATTR, 0, 0, 0},  // 1 strike
    {STRIKES_FULL_BOTTOM_TILE | STRIKES_TILE_CHARATTR,
     STRIKES_HALF_BOTTOM_TILE | STRIKES_TILE_CHARATTR,
     STRIKES_HALF_BOTTOM_TILE | (STRIKES_TILE_CHARATTR ^ TILEMAP_H_FLIP_FLAG),
     0},  // 2 strikes
    {STRIKES_FULL_BOTTOM_TILE | STRIKES_TILE_CHARATTR,
     STRIKES_HALF_BOTTOM_TILE | STRIKES_TILE_CHARATTR,
     STRIKES_HALF_BOTTOM_TILE | (STRIKES_TILE_CHARATTR ^ TILEMAP_H_FLIP_FLAG),
     STRIKES_FULL_BOTTOM_TILE | STRIKES_TILE_CHARATTR}  // 3 strikes
};

void Game_PlayGame(void) {
    // Initialize game state
    Game_score = 0;
    Game_strikes = 0;
    
    // Setup screen
    Game_SetupScreen();
    
    // Main game loop
    while (1) {
        Game_InitForLife();
        
        // Show player
        MetaSprite_InitLoop();
        Player_DrawSprites();
        MetaSprite_FinalizeLoop();
        
        Screen_FadeIn();
        
        Game_Loop();
        
        Game_StrikeAnimation();
        
        Game_strikes++;
        Game_DrawStrikes();
        
        if (Game_strikes >= N_STRIKES) {
            break;
        }
        
        Screen_FadeOut();
    }
    
    Screen_SlowFadeOut(GAME_OVER_FADEOUT_SPEED);
}

void Game_InitForLife(void) {
    Game_strikeAnimationType = STRIKE_ANIMATION_NONE;
    
    // Clear interactive background buffer
    memset(interactiveBgBuffer, 0, sizeof(interactiveBgBuffer));
    
    Game_DrawScore();
    Game_DrawStrikes();
    
    Player_Init();
    Elevators_Init();
    Npcs_Init();
    
    updateBgBufferOnZero = 0;
    
    consoleNocashMessage("InitForLife: score=%d strikes=%d", Game_score, Game_strikes);
}

void Game_Loop(void) {
    while (Game_strikeAnimationType == STRIKE_ANIMATION_NONE) {
        Screen_WaitFrame();
        Random_AddJoypadEntropy();
        
        MetaSprite_InitLoop();
        
        Player_Process();
        Elevators_Process();
        Npcs_Process();
        
        Player_DrawSprites();
        Npcs_DrawSprites();
        
        Game_DrawScore();
        
        updateBgBufferOnZero = 0;
        
        MetaSprite_FinalizeLoop();
    }
}

void Game_StrikeAnimation(void) {
    // First phase: minimum frames
    uint16_t timeout = STRIKE_MIN_FRAMES;
    while (timeout > 0) {
        Game_strikeTimeout = timeout;
        Game_StrikeAnimationFrame();
        timeout--;
    }
    
    // Second phase: until timeout or button pressed
    timeout = STRIKE_TIMEOUT;
    while (timeout > 0) {
        Game_strikeTimeout = timeout;
        Game_StrikeAnimationFrame();
        
        // Check if any button pressed
        if (Controller_pressed & JOY_BUTTONS) {
            break;
        }
        
        timeout--;
    }
}

void Game_StrikeAnimationFrame(void) {
    Screen_WaitFrame();
    MetaSprite_InitLoop();
    
    // Call appropriate strike animation routine
    switch (Game_strikeAnimationType) {
        case STRIKE_ANIMATION_ROPE_BREAKING:
            Elevators_StrikeAnimationRopeBreaking();
            break;
        case STRIKE_ANIMATION_WRONG_FLOOR:
            Npcs_StrikeAnimationWrongFloor();
            break;
        case STRIKE_ANIMATION_FIGHTING:
            Npcs_StrikeAnimationFighting();
            break;
        case STRIKE_ANIMATION_NONE:
            break;
    }
    
    Player_DrawSprites();
    Npcs_DrawSprites();
    
    updateBgBufferOnZero = 0;
    MetaSprite_FinalizeLoop();
}

void Game_DrawScore(void) {
    uint16_t score = Game_score;
    
    // Draw 4 digits
    // Assembly: interactiveBgBuffer + SCORE_TILE_COLUMN * 32 * 2 + (SCORE_TILE_ROW + 3 - i) * 2
    // In C (word-indexed): SCORE_TILE_COLUMN * 32 + (SCORE_TILE_ROW + 3 - i)
    for (uint8_t i = 0; i < 4; i++) {
        uint16_t remainder;
        uint16_t quotient = Math_Divide_U16Y_U8A(score, 10, &remainder);
        
        // Calculate tile position (word index in buffer)
        // Assembly uses byte addressing: column * 32 * 2 + row * 2
        // C uses word addressing: column * 32 + row
        uint16_t tileWordIndex = SCORE_TILE_COLUMN * 32 + (SCORE_TILE_ROW + 3 - i);
        interactiveBgBuffer[tileWordIndex] = (remainder + SCORE_DIGIT_DELTA) | (SCORE_TILE_ATTR << 8);
        
        score = quotient;
    }
}

void Game_DrawStrikes(void) {
    uint8_t strikes = Game_strikes;
    if (strikes > 3) strikes = 3;
    
    // Draw strike tiles
    // Assembly: interactiveBgBuffer + STRIKES_TILE_COLUMN * 64 + STRIKES_TILE_ROW * 2, Y (Y increments by 2)
    // In C (word-indexed): STRIKES_TILE_COLUMN * 32 + STRIKES_TILE_ROW + i (i increments by 1)
    for (uint8_t i = 0; i < 4; i++) {
        uint16_t topTile = StrikeTableTop[strikes][i];
        uint16_t bottomTile = StrikeTableBottom[strikes][i];
        
        // Calculate word indices (buffer is word-indexed, 32 words per row)
        // Assembly byte address: column * 64 + row * 2 + Y (where Y = i * 2)
        // C word address: column * 32 + row + i
        uint16_t topWordIndex = STRIKES_TILE_COLUMN * 32 + STRIKES_TILE_ROW + i;
        uint16_t bottomWordIndex = (STRIKES_TILE_COLUMN + 1) * 32 + STRIKES_TILE_ROW + i;
        
        interactiveBgBuffer[topWordIndex] = topTile;
        interactiveBgBuffer[bottomWordIndex] = bottomTile;
    }
}

void Game_SetupScreen(void) {
    // Force blank
    REG_INIDISP = 0x80; // INIDISP_FORCE
    
    // Set BG mode 2
    REG_BGMODE = 0x02; // BGMODE_MODE2
    
    // Reset scroll registers (write twice)
    REG_BG1HOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2HOFS = 0;
    
    // Set VRAM base addresses and sizes
    // BG1SC: map at $0000, size 32x32 (size bits = 0x00)
    REG_BG1SC = ((GAME_BG1_MAP / 1024) << 2) | GAME_BG1_SIZE;
    // BG2SC: map at $0400, size 32x32 (size bits = 0x00)
    REG_BG2SC = ((GAME_BG2_MAP / 1024) << 2) | GAME_BG2_SIZE;
    // BG3SC: map at $0800, size 32x32 (size bits = 0x00)
    REG_BG3SC = ((GAME_BG3_MAP / 1024) << 2) | GAME_BG3_SIZE;
    
    // BG12NBA: BG1 tiles at $5000, BG2 tiles at $1000
    REG_BG12NBA = ((GAME_BG2_TILES / 4096) << 4) | (GAME_BG1_TILES / 4096);
    // BG34NBA: not used, set to 0
    REG_BG34NBA = 0;
    
    // OBSEL: OAM tiles at $6000, size 8x8/16x16, name 0
    REG_OBSEL = (GAME_OAM_TILES / 8192) & 0x07;
    
    // Load resources
    // Load tiles first (4bpp bitplane format - VMAIN=0x80 is set by LoadVram)
    // GAME_BG1_TILES is a word address, but LoadVram expects byte address
    // Convert: byte_addr = word_addr * 2
    LoadVram((const uint8_t*)backgroundTiles, GAME_BG1_TILES * 2, BACKGROUNDTILES_SIZE);
    
    // Load tilemap (16-bit words)
    // GAME_BG1_MAP is a word address, VMADD expects word address
    REG_VMAIN = 0x80; // Increment high byte first, increment by 1 word (matches assembly)
    REG_VMADD = GAME_BG1_MAP; // Word address (VMADD expects word address, GAME_BG1_MAP is already word address)
    REG_DMAP0 = 0x01; // CPU to PPU, 2 regs write twice (VMDATA low then high)
    REG_BBAD0 = 0x18; // Destination: VMDATA
    REG_A1T0 = (uint16_t)((uint32_t)backgroundMap & 0xFFFF);
    REG_A1B0 = (uint8_t)((uint32_t)backgroundMap >> 16);
    REG_DAS0 = BACKGROUNDMAP_SIZE;
    REG_MDMAEN = 0x01; // Start DMA
    
    LoadCGRam((const uint8_t*)backgroundPalette, 0, BACKGROUNDPALETTE_SIZE * 2);  // 16 colors * 2 bytes
    
    // GAME_BG2_TILES is a word address, but LoadVram expects byte address
    LoadVram((const uint8_t*)interactiveTilesTiles, GAME_BG2_TILES * 2, INTERACTIVETILESTILES_SIZE);
    LoadCGRam((const uint8_t*)interactiveTilesPalette, 7 * 16, INTERACTIVETILESPALETTE_SIZE * 2);  // Palette 7
    
    // GAME_OAM_TILES is a word address, but LoadVram expects byte address
    LoadVram((const uint8_t*)playerTiles, GAME_OAM_TILES * 2, PLAYERTILES_SIZE);
    LoadCGRam((const uint8_t*)playerPalette, 8 * 16, PLAYERPALETTE_SIZE * 2);  // Palette 8
    
    // GAME_OAM_TILES is a word address, calculate byte address: (GAME_OAM_TILES + offset) * 2
    // 32 tiles * 16 bytes per tile = 512 bytes = 256 words, so offset in words = 32 * 16 / 2 = 256
    LoadVram((const uint8_t*)strikeAnimationsTiles, (GAME_OAM_TILES + 256) * 2, STRIKEANIMATIONSTILES_SIZE);
    LoadCGRam((const uint8_t*)strikeAnimationsPalette, 9 * 16, STRIKEANIMATIONSPALETTE_SIZE * 2);  // Palette 9
    
    // GAME_OAM_TILES is a word address, calculate byte address: (GAME_OAM_TILES + offset) * 2
    // (512 - 32) tiles * 16 bytes per tile = 7680 bytes = 3840 words, so offset in words = (512 - 32) * 16 / 2 = 3840
    LoadVram((const uint8_t*)businessManTiles, (GAME_OAM_TILES + 3840) * 2, BUSINESSMANTILES_SIZE);
    LoadCGRam((const uint8_t*)businessManPalette, 128 + 7 * 16, BUSINESSMANPALETTE_SIZE * 2);  // Palette 7 in second CGRAM bank
    
    // Enable layers
    REG_TM = 0x11; // TM_BG1 | TM_BG2 | TM_BG3 | TM_OBJ
}
