//LoROM memory map

#include <stdint.h>

#include "snes_regs_xc.h"
#include "initsnes.h"
#include "build/assets/level.inc"
#include "build/assets/levelcol.inc"
#include "build/assets/player.inc"
#include "build/assets/walker.inc"

union uOAMCopy oamCopy;

// World coordinates are 12.4 fixed point (>>4 gives a pixel). The map is
// level_COLS x level_ROWS tiles of 8 px; collision is one byte per tile cell.
// The map (level_COLS = 422) is far wider than the 64-tile VRAM tilemap, so it
// is streamed a column at a time as the camera scrolls (see streamCol). The map
// fits the 32-tall VRAM screen vertically, so only X streams; CAM_Y is a fixed
// vertical scroll that sits the ground at the bottom of the screen.
#define FP        4
#define MAP_W     (level_COLS * 8)
#define CAM_MAX_X (MAP_W - 256)
#define CAM_Y     16              // (level_ROWS*8 - 224): bottom-align the map
#define MAP_BASE  0x6000          // BG1 tilemap base, VRAM word address
#define WIN_MARGIN 16             // resident columns kept left of the camera

#define GRAVITY   4
#define MAXFALL   80
#define WALK_SPD  20
#define WMOVE_SPD 10
#define JUMP_VY   80

typedef struct {
	int16_t px, py;     // position (fixed point)
	int16_t vx, vy;     // velocity (fixed point)
	uint8_t onGround;
	uint8_t faceLeft;
	uint8_t frame;
	uint8_t dir;        // walker: 0 = left, 1 = right
} Entity;

static Entity player, walker;
static uint8_t animT;
static uint16_t camX;
static uint16_t winLo;          // leftmost map column resident in VRAM
static uint16_t pendCol[4];     // columns queued this frame, streamed in vblank
static uint8_t  pendN;
static uint8_t prevA;

// Fixed-point -> pixel and pixel -> tile-cell, with floor (arithmetic) division.
// calypsi miscompiles a signed `int16_t >> const` of a positive value (e.g.
// 640 >> 4 yields -8), so these route through the unsigned shift it compiles
// correctly and restore the sign by hand.
static int16_t toPix(int16_t v){
	if (v >= 0) return (int16_t)((uint16_t)v >> FP);
	return -(int16_t)(((uint16_t)(-v) + ((1 << FP) - 1)) >> FP);
}
static int16_t toCell(int16_t p){
	if (p >= 0) return (int16_t)((uint16_t)p >> 3);
	return -(int16_t)(((uint16_t)(-p) + 7) >> 3);
}

// A tile cell is solid if the collision table marks it; off the left/right/top
// edges reads as a wall, below the map reads as empty (the pit).
static uint8_t solid(int16_t x, int16_t y){
	int16_t cx = toCell(x), cy = toCell(y);
	if (cx < 0 || cx >= level_COLS || cy < 0) return 1;
	if (cy >= level_ROWS) return 0;
	return level_col[cy * level_COLS + cx];
}

// A 16 px edge is sampled at its two ends and middle (each tile is 8 px wide).
static uint8_t edgeV(int16_t x, int16_t ytop){
	return solid(x, ytop) || solid(x, ytop + 7) || solid(x, ytop + 15);
}
static uint8_t edgeH(int16_t xleft, int16_t y){
	return solid(xleft, y) || solid(xleft + 7, y) || solid(xleft + 15, y);
}

// Move on X with tile collision; returns 1 if a wall stopped the entity.
static uint8_t moveX(Entity *e){
	int16_t x, y, ex;
	e->px += e->vx;
	x = toPix(e->px);
	y = toPix(e->py);
	if (e->vx > 0){
		ex = x + 15;
		if (edgeV(ex, y)){
			x = (ex & ~7) - 16;
			e->px = x << FP; e->vx = 0; return 1;
		}
	} else if (e->vx < 0){
		if (edgeV(x, y)){
			x = (x & ~7) + 8;
			e->px = x << FP; e->vx = 0; return 1;
		}
	}
	return 0;
}

static void moveY(Entity *e){
	int16_t x, y, ey;
	if (!e->onGround){            // grounded entities skip gravity (no 1px sink)
		e->vy += GRAVITY;
		if (e->vy > MAXFALL) e->vy = MAXFALL;
	}
	e->py += e->vy;
	x = toPix(e->px);
	y = toPix(e->py);
	if (e->vy > 0){
		ey = y + 15;
		if (edgeH(x, ey)){
			y = (ey & ~7) - 16;
			e->py = y << FP; e->vy = 0;
		}
	} else if (e->vy < 0){
		if (edgeH(x, y)){
			y = (y & ~7) + 8;
			e->py = y << FP; e->vy = 0;
		}
	}
	// Grounded when a solid tile sits directly beneath the 16 px box.
	e->onGround = edgeH(toPix(e->px), toPix(e->py) + 16);
}

static void playerUpdate(uint8_t inLo, uint8_t inHi){
	uint8_t a = inLo & 0x80;          // A button (JOY1L: axlr----)
	player.vx = 0;
	if (inHi & 0x02){ player.vx = -WALK_SPD; player.faceLeft = 1; }
	else if (inHi & 0x01){ player.vx = WALK_SPD; player.faceLeft = 0; }
	if (a && !prevA && player.onGround) player.vy = -JUMP_VY;
	prevA = a;

	moveX(&player);
	moveY(&player);

	if (player.py > ((level_ROWS * 8 + 16) << FP)){
		player.px = 24 << FP; player.py = 176 << FP;
		player.vx = 0; player.vy = 0;
	}

	// Two animation frames: stand (0) and step (1); use step in the air too.
	if (!player.onGround) player.frame = 1;
	else if (player.vx == 0) player.frame = 0;
	else player.frame = (animT >> 3) & 1;
}

// One autonomous walker: paces along the ground, turning at a wall (moveX was
// blocked) or at a ledge (no ground one pixel ahead of its leading foot).
static void walkerUpdate(void){
	int16_t footX, footY;
	walker.vx = walker.dir ? WMOVE_SPD : -WMOVE_SPD;
	footX = walker.dir ? (toPix(walker.px) + 16) : (toPix(walker.px) - 1);
	footY = toPix(walker.py) + 16;
	if (walker.onGround && !solid(footX, footY)){
		walker.dir ^= 1;
		walker.vx = -walker.vx;
	}
	if (moveX(&walker)) walker.dir ^= 1;
	moveY(&walker);
	walker.faceLeft = !walker.dir;
	walker.frame = (animT >> 3) & 1;
}

// DMA one map column into its VRAM tilemap slot. The 64x32 VRAM map is two
// 32x32 screens (left = words MAP_BASE.., right = MAP_BASE+0x400..); a column's
// slot is (c & 63) and wraps every 64 columns, so writing column c overwrites
// whichever column last used that slot (always one off-screen by WIN_MARGIN).
// level_map is column-major, so a column is level_ROWS contiguous words.
static void streamCol(uint16_t c){
	uint16_t slot = c & 63;
	uint16_t word = (slot < 32) ? (MAP_BASE + slot)
	                            : (MAP_BASE + 0x400 + (slot - 32));
	LoadVramColumn((const unsigned char *)&level_map[(uint16_t)(c * level_ROWS)],
	               (uint16_t)(word << 1), level_ROWS * 2);
}

// Track the player with the camera (clamped to the map), and slide the 64-wide
// resident column window so the view always has WIN_MARGIN columns of buffer on
// each side. New columns are queued here and DMA'd in the vblank window.
static void updateCamera(void){
	int16_t target = toPix(player.px) + 8 - 128;
	uint16_t camCol, want;
	if (target < 0) target = 0;
	else if (target > CAM_MAX_X) target = CAM_MAX_X;
	camX = (uint16_t)target;

	camCol = camX >> 3;
	want = (camCol > WIN_MARGIN) ? (camCol - WIN_MARGIN) : 0;
	if (want > (uint16_t)(level_COLS - 64)) want = level_COLS - 64;
	pendN = 0;
	while (winLo < want){ if (pendN < 4) pendCol[pendN++] = winLo + 64; winLo++; }
	while (winLo > want){ winLo--; if (pendN < 4) pendCol[pendN++] = winLo; }
}

// Build sprites 0 (player) and 1 (walker) into the OAM mirror: screen position
// = world position - camera, the current frame's CHARNUM, and an attribute byte
// (priority 3, OBJ palette, H-flip when facing left). An off-screen sprite is
// parked below the visible area.
static void drawFrame(void){
	int16_t sx[2], sy[2];
	uint8_t ch[2], at[2], i;

	sx[0] = toPix(player.px) - camX;
	sy[0] = toPix(player.py) - CAM_Y;
	ch[0] = player.frame * player_STEP;
	at[0] = player.faceLeft ? 0x70 : 0x30;

	sx[1] = toPix(walker.px) - camX;
	sy[1] = toPix(walker.py) - CAM_Y;
	ch[1] = player_TILES + walker.frame * walker_STEP;
	at[1] = walker.faceLeft ? 0x72 : 0x32;

	for (i = 0; i < 2; i++){
		if (sx[i] < -16 || sx[i] > 255 || sy[i] < -16 || sy[i] > 239){
			oamCopy.Bytes[i * 4 + 0] = 0;
			oamCopy.Bytes[i * 4 + 1] = 0xE0;
		} else {
			oamCopy.Bytes[i * 4 + 0] = (uint8_t)sx[i];
			oamCopy.Bytes[i * 4 + 1] = (uint8_t)sy[i];
		}
		oamCopy.Bytes[i * 4 + 2] = ch[i];
		oamCopy.Bytes[i * 4 + 3] = at[i];
	}
}

void main(void){
	uint8_t inLo, inHi;
	uint16_t hi, i;

	initSNES(SLOWROM);

	LoadVram(player_chr, 0x0000, sizeof(player_chr));
	LoadVram(walker_chr, player_TILES * 32, sizeof(walker_chr));
	LoadVram(level_chr, 0x2000, sizeof(level_chr));
	LoadCGRam(level_pal, 0x00, sizeof(level_pal));
	LoadCGRam(player_pal, 0x80, sizeof(player_pal));
	LoadCGRam(walker_pal, 0x90, sizeof(walker_pal));

	initOAMCopy(oamCopy.Bytes);
	// Reveal sprites 0 and 1: clear their X-high + size bits (small 16x16).
	hi = 512;
	oamCopy.Bytes[hi] &= ~0x0F;

	player.px = 24 << FP;  player.py = 176 << FP;  player.faceLeft = 0;
	walker.px = 528 << FP; walker.py = 176 << FP;  walker.dir = 0;

	// Seed the camera/window, then DMA the first 64 columns into VRAM.
	updateCamera();
	for (i = 0; i < 64; i++) streamCol(winLo + i);
	pendN = 0;

	REG_OBJSEL = 0x60;   // sss=011: 16x16 small / 32x32 large, name base 0
	REG_BG1SC = 0x61;    // tilemap base word 0x6000, size 64x32
	REG_BG12NBA = 0x01;  // BG1 char base word 0x1000
	REG_BGMODE = 0x01;   // mode 1
	REG_TM = 0x11;       // BG1 + OBJ on main screen
	REG_NMITIMEN = 0x01; // joypad autoread
	REG_INIDISP = 0x0F;  // screen on, full brightness

	drawFrame();

	while(1){
		// Two-phase HVBJOY vblank sync (read-only status, no clear side effect).
		while ((REG_HVBJOY & 0x80) != 0) { }
		while ((REG_HVBJOY & 0x80) == 0) { }

		// Time-sensitive PPU IO only: present the frame built last iteration and
		// stream any newly-revealed map columns.
		LoadOAMCopy(oamCopy.Bytes, 0x0000, sizeof(union uOAMCopy));
		for (i = 0; i < pendN; i++) streamCol(pendCol[i]);
		REG_BG1HOFS = (uint8_t)camX;
		REG_BG1HOFS = (uint8_t)(camX >> 8);
		REG_BG1VOFS = (uint8_t)CAM_Y;
		REG_BG1VOFS = 0x00;

		// Compute the next frame during active display.
		while ((REG_HVBJOY & 0x01) != 0) { }
		inLo = REG_JOY1L;
		inHi = REG_JOY1H;

		animT++;
		playerUpdate(inLo, inHi);
		walkerUpdate();
		updateCamera();
		drawFrame();
	}
}

// Cross-compiler interrupt handler implementations
void snesXC_cop(void) {
}

void snesXC_brk(void) {
}

void snesXC_abort(void) {
}

void snesXC_nmi(void) {
}
