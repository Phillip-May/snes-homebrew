#include "metasprite.h"
#include "initsnes.h"
#include <string.h>

// MetaSprite module state
uint8_t MetaSprite_updateOam = 0;
uint16_t MetaSprite_oamBufferPos = 0;
uint16_t MetaSprite_prevOamBufferPos = 512; // sizeof(oamBuffer)
uint16_t MetaSprite_oamBuffer2Pos = 0;
uint8_t MetaSprite_oamBuffer2Temp = 0x80;

// Current sprite parameters
uint16_t MetaSprite_xPos = 0;
uint16_t MetaSprite_yPos = 0;
uint16_t MetaSprite_charAttr = 0;
uint8_t MetaSprite_size = 0;

// OAM buffer using uOAMCopy union
union uOAMCopy MetaSprite_oamCopy;

void MetaSprite_InitLoop(void) {
    MetaSprite_oamBufferPos = 0;
    MetaSprite_oamBuffer2Pos = 0;
    MetaSprite_oamBuffer2Temp = 0x80;
}

void MetaSprite_FinalizeLoop(void) {
    // Finalize oamBuffer2Temp if needed
    if (MetaSprite_oamBuffer2Temp != 0x80) {
        if (MetaSprite_oamBuffer2Pos < 32) {
            // Shift right until carry is set
            uint8_t temp = MetaSprite_oamBuffer2Temp;
            while (!(temp & 0x01)) {
                temp >>= 1;
                // Safety check: if temp becomes 0, break to avoid infinite loop
                if (temp == 0) {
                    temp = 0x01; // Set to 1 to break loop
                    break;
                }
            }
            MetaSprite_oamCopy.arr.OAMTable2[MetaSprite_oamBuffer2Pos] = temp;
            MetaSprite_oamBuffer2Pos++;
        }
    }
    
    // Clear remaining oamBuffer2 entries
    for (uint16_t i = MetaSprite_oamBuffer2Pos + 1; i < 32; i++) {
        MetaSprite_oamCopy.arr.OAMTable2[i] = 0;
    }
    
    // Set unused sprites offscreen (Y = 240)
    if (MetaSprite_oamBufferPos < 512) {
        uint16_t pos = MetaSprite_oamBufferPos;
        while (pos < MetaSprite_prevOamBufferPos) {
            // pos is byte offset, convert to sprite index: spriteIndex = pos / 4
            uint8_t spriteIndex = (uint8_t)(pos / 4);
            MetaSprite_oamCopy.arr.OAMArray[spriteIndex].OBJY = 240; // Y position
            pos += 4;
        }
        MetaSprite_prevOamBufferPos = MetaSprite_oamBufferPos;
    } else {
        MetaSprite_prevOamBufferPos = 512;
    }
    
    MetaSprite_updateOam = 1;
}

void MetaSprite_ProcessSprite(void) {
    // Check bounds
    int16_t x = (int16_t)MetaSprite_xPos;
    int16_t y = (int16_t)MetaSprite_yPos;
    
    if (x < (-SPRITE_SIZE + 1) || x >= 256) {
        return;
    }
    if (y < (-SPRITE_SIZE + 1) || y >= 240) {
        return;
    }
    
    if (MetaSprite_oamBufferPos >= 512) {
        return;
    }
    
    // Calculate sprite index from byte position
    uint8_t spriteIndex = (uint8_t)(MetaSprite_oamBufferPos / 4);
    
    // Write sprite to OAM buffer
    // OAM format: X (byte 0), Y (byte 1), char (byte 2), attr (byte 3)
    // Assembly stores charAttr as 16-bit word at offset char (bytes 2-3)
    MetaSprite_oamCopy.arr.OAMArray[spriteIndex].OBJX = (uint8_t)MetaSprite_xPos; // X position (low byte)
    MetaSprite_oamCopy.arr.OAMArray[spriteIndex].OBJY = (uint8_t)MetaSprite_yPos; // Y position
    MetaSprite_oamCopy.arr.OAMArray[spriteIndex].CHARNUM = (uint8_t)MetaSprite_charAttr; // Character number (low byte)
    MetaSprite_oamCopy.arr.OAMArray[spriteIndex].PROPERTIES = (uint8_t)(MetaSprite_charAttr >> 8); // Properties (high byte)
    
    MetaSprite_oamBufferPos += 4;
    
    // Update high table
    // Assembly: LDA xPos + 1; LSR; ROR oamBuffer2Temp
    // xPos + 1 is the high byte, LSR shifts bit 1 (bit 9 of xPos) into carry, ROR rotates carry into temp
    uint8_t xHighBit = (uint8_t)((MetaSprite_xPos >> 9) & 0x01); // Extract bit 9 (X sign bit)
    uint8_t sizeBit = MetaSprite_size & 0x01;
    
    // Simulate ROR operation: Rotate Right through Carry
    // ROR: temp = (temp >> 1) | (carry << 7), new_carry = old bit 0
    // First rotation: X high bit
    uint8_t temp = MetaSprite_oamBuffer2Temp;
    uint8_t carry = xHighBit; // Carry from LSR (bit 9 of xPos)
    uint8_t oldBit0 = temp & 0x01; // Save bit 0 (will become new carry)
    temp = (temp >> 1) | (carry << 7); // Shift right, insert carry at bit 7
    MetaSprite_oamBuffer2Temp = temp;
    
    // Second rotation: size bit
    temp = MetaSprite_oamBuffer2Temp;
    carry = sizeBit; // Carry from LSR (bit 0 of size)
    oldBit0 = temp & 0x01; // Save bit 0 (this will be the carry flag after ROR)
    temp = (temp >> 1) | (carry << 7); // Shift right, insert carry at bit 7
    MetaSprite_oamBuffer2Temp = temp;
    
    // Check if we need to write to oamBuffer2 (carry was set after second ROR)
    // Assembly checks IF_C_SET, which is the carry flag after the second ROR
    // The carry flag contains oldBit0 (the bit that was rotated out)
    if (oldBit0) {
        // Underflow - write byte
        if (MetaSprite_oamBuffer2Pos < 32) {
            MetaSprite_oamCopy.arr.OAMTable2[MetaSprite_oamBuffer2Pos] = MetaSprite_oamBuffer2Temp;
            MetaSprite_oamBuffer2Pos++;
            MetaSprite_oamBuffer2Temp = 0x80;
        }
    }
}

void MetaSprite_ProcessMetaSprite_Y(uint16_t xPos, uint16_t yPos, uint16_t charAttr, const uint8_t *metaspritePtr) {
    if (!metaspritePtr) {
        consoleNocashMessage("MetaSprite: null ptr");
        return;
    }
    
    // Assembly: LDA f:MetaSpriteLayoutBank << 16, X (first byte is count)
    uint8_t nObjects = metaspritePtr[0];
    
    // Assembly reads objects sequentially: xPos (byte), yPos (byte), charAttr (word), size (byte)
    // Access data directly as bytes to avoid struct alignment issues
    const uint8_t *dataPtr = metaspritePtr + 1;
    
    for (uint8_t i = 0; i < nObjects; i++) {
        if (MetaSprite_oamBufferPos >= 512) {
            break;
        }
        
        // Read object data (5 bytes: xOffset, yOffset, charAttr low, charAttr high, size)
        int8_t xOffset = (int8_t)dataPtr[0];
        int8_t yOffset = (int8_t)dataPtr[1];
        uint16_t objCharAttr = dataPtr[2] | (dataPtr[3] << 8);  // Little-endian word
        uint8_t size = dataPtr[4];
        
        // Calculate display position
        int16_t displayY = yPos + yOffset;
        if (displayY < -15 || displayY >= 240) {
            dataPtr += 5;  // Skip to next object
            continue; // Skip offscreen sprites
        }
        
        int16_t displayX = xPos + xOffset;
        if (displayX < -15 || displayX >= 256) {
            dataPtr += 5;  // Skip to next object
            continue; // Skip offscreen sprites
        }
        
        // Set sprite parameters
        MetaSprite_xPos = (uint16_t)displayX;
        MetaSprite_yPos = (uint16_t)displayY;
        // Assembly: LDA charAttr; ADC f:MetaSpriteObjects + MetaSpriteObjectFormat::charAttr, X
        MetaSprite_charAttr = charAttr + objCharAttr;
        MetaSprite_size = size;
        
        // Process this sprite
        MetaSprite_ProcessSprite();
        
        // Move to next object (5 bytes)
        dataPtr += 5;
    }
}

void MetaSprite_VBlank(void) {
    if (MetaSprite_updateOam) {
        // Transfer OAM buffer to hardware using LoadOAMCopy
        LoadOAMCopy(MetaSprite_oamCopy.Bytes, 0x0000, sizeof(union uOAMCopy));
        
        MetaSprite_updateOam = 0;
    }
}
