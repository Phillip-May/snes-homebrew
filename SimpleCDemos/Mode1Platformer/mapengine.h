#ifndef MAPENGINE_H
#define MAPENGINE_H

// The whole platformer (state, physics, camera, column streaming, OAM build) is
// implemented in mapengine.c. That translation unit is compiled at reduced
// optimization for vbcc, whose -O4 miscompiles the fixed-point/streaming math
// (the camera scroll computed to 0); the rest of the demo keeps -O4. main() in
// mainBankZero.c only boots the SNES and drives these two entry points.
void engineInit(void);    // one-time VRAM/CGRAM/OAM/register setup + first frame
void engineFrame(void);   // one frame: vblank present + stream, then compute next

#endif
