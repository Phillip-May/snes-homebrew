#ifndef METASPRITE_H
#define METASPRITE_H

#include <stdint.h>
#include "snes_regs_xc.h"

// MetaSprite object format (from layout data)
// Assembly format: byte xPos, byte yPos, word charAttr, byte size (5 bytes total)
// Must be packed to match assembly layout exactly
typedef struct __attribute__((packed)) {
    int8_t xOffset;      // Signed X offset (byte 0)
    int8_t yOffset;      // Signed Y offset (byte 1)
    uint16_t charAttr;   // Character and attributes (bytes 2-3, little-endian)
    uint8_t size;        // Size bit (byte 4, only lowest bit used)
} MetaSpriteObject;

// MetaSprite layout format: first byte is count, then array of MetaSpriteObject

// MetaSprite module state
extern uint8_t MetaSprite_updateOam;
extern uint16_t MetaSprite_oamBufferPos;
extern uint16_t MetaSprite_prevOamBufferPos;
extern uint16_t MetaSprite_oamBuffer2Pos;
extern uint8_t MetaSprite_oamBuffer2Temp;

// Current sprite parameters (set before ProcessMetaSprite_Y)
extern uint16_t MetaSprite_xPos;
extern uint16_t MetaSprite_yPos;
extern uint16_t MetaSprite_charAttr;
extern uint8_t MetaSprite_size;

// OAM buffer using uOAMCopy union
extern union uOAMCopy MetaSprite_oamCopy;

// Initialize OAM buffer at start of frame
void MetaSprite_InitLoop(void);

// Finalize OAM buffer after sprites added
void MetaSprite_FinalizeLoop(void);

// Process a single sprite
void MetaSprite_ProcessSprite(void);

// Process a metasprite (charAttr in Y register equivalent - passed as parameter)
// X = pointer to metasprite layout data (in MetaSpriteLayoutBank)
void MetaSprite_ProcessMetaSprite_Y(uint16_t xPos, uint16_t yPos, uint16_t charAttr, const uint8_t *metaspritePtr);

// Transfer OAM buffer to hardware (call during VBlank)
void MetaSprite_VBlank(void);

// SPRITE_SIZE constant (default 16)
#define SPRITE_SIZE 16

#endif // METASPRITE_H
