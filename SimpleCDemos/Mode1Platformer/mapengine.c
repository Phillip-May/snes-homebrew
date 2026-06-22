// Mode1Platformer engine -- the whole game lives here so this translation unit
// can be built at reduced optimization. vbcc's -O4 miscompiles the position and
// column-streaming integer math (the camera scroll stuck at 0; streamed columns
// read the wrong source). At -O3 (with the same -unroll-all/-inline-depth
// performance flags) it is correct, so the Makefile builds this file that way
// for vbcc; mainBankZero.c (the thin entry point) keeps -O4. Other toolchains
// build it normally.

#include <stdint.h>

#include "snes_regs_xc.h"
#include "initsnes.h"
#include "mapengine.h"
#include "build/assets/level.inc"
#include "build/assets/levelcol.inc"
#include "build/assets/player.inc"
#include "build/assets/walker.inc"
#include "build/assets/blocks.inc"   // OBJ overlays: used/brick/coin/score popups
#include "build/assets/hudfont.inc"  // 2bpp font for the BG3 score HUD
#include "build/assets/blockcells.inc"  // qblock_cells / brick_cells (from level.tmx)
#include "build/assets/spawns.inc"      // NENEMY, enemy_spawns[], PLAYER_START_* (level.tmx)

union uOAMCopy oamCopy;

// Position is stored the way SMB1 itself stores it: an integer PIXEL coordinate
// plus a separate subpixel fraction in units of 1/256 px (SUB). This is what
// makes faithful SMB physics possible -- its accelerations are tiny (~0.04
// px/frame^2), far below one pixel, so they need sub-pixel resolution to
// accumulate. It also sidesteps every compiler landmine this collection has hit:
// the pixel coordinate is a plain int16 (the 3375-px-wide level fits, no 12.4
// overflow and no 32-bit position), and the per-frame integration is pure int16
// add/compare/carry -- no fixed-point division (vbcc miscompiles >>/32-bit
// divide) and no signed shift (calypsi miscompiles it). Velocities and the
// physics constants below are all in 1/256-px units. The map is level_COLS x
// level_ROWS tiles of 8 px; collision is one byte per tile cell. The map
// (level_COLS = 422) is far wider than the 64-tile VRAM tilemap, so it is
// streamed a column at a time as the camera scrolls (see streamCol). The map
// fits the 32-tall VRAM screen vertically, so only X streams; CAM_Y is a fixed
// vertical scroll that sits the ground at the bottom of the screen.
#define SUB       256             // subpixel units per pixel (SMB's 1/256)
#define MAP_W     (level_COLS * 8)
#define CAM_MAX_X (MAP_W - 256)
#define CAM_Y     16              // (level_ROWS*8 - 224): bottom-align the map
#define MAP_BASE  0x6000          // BG1 tilemap base, VRAM word address
#define WIN_MARGIN 16             // resident columns kept left of the camera
// Player start (PLAYER_START_X/Y) and enemy spawns come from the level.tmx
// object layer via spawns.inc; the start y is above the ground so the player
// drops in.

// SMB1 physics, in 1/256-px units (so 256 = 1 px/frame). Horizontal motion has
// momentum: a small per-frame acceleration toward a speed ceiling, a turnaround
// "skid" deceleration, and ground friction; the run button (B) raises both the
// ceiling and the acceleration. The jump is the SMB variable jump: a fixed
// takeoff velocity (higher when running) with DUAL gravity -- weak while the
// jump button is held and the player is rising (the float), strong once it is
// released or the player is falling. Values dialled to the SMB feel in Mesen.
#define WALK_MAX   400      // 1.5625 px/f  walk speed ceiling
#define RUN_MAX    656      // 2.5625 px/f  run speed ceiling (B held)
#define WALK_ACC   10       // ~0.039 px/f^2 ground/air acceleration, walking
#define RUN_ACC    14       // ~0.055 px/f^2 acceleration, running
#define FRICTION   13       // ~0.051 px/f^2 ground deceleration, no input
#define SKID_DEC   24       // ~0.094 px/f^2 turnaround deceleration
#define JUMP_V     1120     // 4.375 px/f takeoff; peak ~76 px, clears 64-px pipes
#define JUMP_V_RUN 1320     // 5.156 px/f running takeoff; higher arc, SMB-style
#define GRAV_UP    32       // 0.125  px/f^2 weak gravity, holding A while rising
#define GRAV_UP_R  40       // 0.156  px/f^2 weak gravity, running jump
#define GRAV_DN    112      // 0.4375 px/f^2 strong gravity, released / falling
#define GRAV_DN_R  144      // 0.5625 px/f^2 strong gravity, running jump
#define MAXFALL    1152     // 4.5 px/f terminal fall speed
#define COYOTE     4        // frames of jump grace after leaving the ground
#define BOUNCE     1024     // 4.0 px/f upward speed after stomping an enemy
#define WMOVE_SPD  160      // 0.625 px/f enemy pacing speed
                            // NENEMY comes from spawns.inc (the level.tmx Entities)

// Scoring + interactive blocks. ? and brick blocks (cells from blockcells.inc)
// react to a bump from below; their reactions are drawn as OBJ sprites over the
// const, ROM, column-streamed BG tilemap (mutating that tilemap would revert on
// scroll-back). blocks.inc frame order: 0 used-block, 1 brick, 2 coin, 3 "100",
// 4 "200". The blocks OBJ tiles load just past player+walker in OBJ VRAM.
#define COIN_SCORE  200
#define STOMP_SCORE 100
#define BLK_USED    0
#define BLK_BRICK   1
#define BLK_COIN    2
#define BLK_100     3
#define BLK_200     4
#define BLOCKS_BASE (player_TILES + walker_TILES)  // OBJ CHARNUM base for blocks
#define BUMP_FRAMES 12      // block nudge-up animation length
#define NBB         3       // concurrent brick bumps
#define NPOP        3       // concurrent coin/score popups
#define POP_FRAMES  24      // popup lifetime (also its rise in px at 1 px/frame)

// OAM slot map: 0 player, 1..NENEMY enemies, then ? overlays, brick bumps, pops.
#define SLOT_Q      (1 + NENEMY)
#define SLOT_BRICK  (SLOT_Q + QBLOCK_N)
#define SLOT_POP    (SLOT_BRICK + NBB)
#define SLOT_TOTAL  (SLOT_POP + NPOP)

// Fixed score HUD on BG3 (2bpp, drawn on top via the Mode-1 BG3-priority bit).
#define HUD_MAP    0x1800   // BG3 tilemap base, VRAM word address (REG_BG3SC 0x18)
#define HUD_CHR    0x4000   // BG3 char base, VRAM byte address (REG_BG34NBA 0x02)
#define HUD_PAL    16       // CGRAM colour index of the HUD 2bpp palette (palette 4)
#define HUD_CELL(x, y) (((HUD_MAP) + (y) * 32 + (x)) << 1)  // LoadLoVram byte dest

typedef struct {
	int16_t px, py;     // integer pixel position (fits int16 across the level)
	int16_t sx, sy;     // subpixel fraction of the position, in 1/256 px
	int16_t vx, vy;     // velocity in 1/256-px units (256 = 1 px/frame)
	uint8_t onGround;
	uint8_t faceLeft;
	uint8_t frame;
	uint8_t dir;        // enemy: 0 = left, 1 = right
	uint8_t active;     // enemy alive / on the field
	int16_t bumpX, bumpY;  // tile cell hit by an upward move this frame...
	uint8_t bumped;        // ...and whether one was (player ceiling bump)
} Entity;

static Entity player, enemies[NENEMY];
static uint8_t animT;
static uint16_t camX;
static uint16_t winLo;          // desired leftmost map column for the window
static uint16_t vramLo;         // leftmost column actually streamed into VRAM
static uint8_t  fullReload;     // window jumped (respawn): re-stream all 64 cols
static uint8_t prevA;
static uint8_t coyote;          // remaining jump-grace frames

// Scoring + interactive-block state.
static uint16_t score;          // 16-bit avoids the vbcc 32-bit-divide bug in the
static uint16_t coins;          // HUD formatter; the level can't overflow 5 digits
static uint8_t  hudDirty;       // redraw the HUD digits next vblank
static uint8_t  qUsed[QBLOCK_N];   // a ? block has been hit (spent)
static uint8_t  qBumpT[QBLOCK_N];  // ? block nudge-animation timer
static struct { uint16_t col, row; uint8_t t; } brickBump[NBB];  // transient brick nudges
static struct { int16_t x, y; uint8_t t; uint8_t frame; } pops[NPOP];  // coin/score popups

// Advance a pixel coordinate by a 1/256-px velocity: accumulate into the
// subpixel fraction and carry whole pixels across. Pure int16 add/compare/carry
// (the velocity is at most a few px/frame, so the carry loops run a handful of
// times) -- no division or shift for either compiler to miscompile.
static void integrate(int16_t *p, int16_t *s, int16_t v){
	int16_t sub = *s + v;
	while (sub >= SUB){ sub -= SUB; (*p)++; }
	while (sub <= -SUB){ sub += SUB; (*p)--; }
	*s = sub;
}

// Pixel -> tile cell (8 px), floored. Unsigned DIVISION, not a shift: calypsi
// miscompiles a signed `>>` of a positive value and vbcc miscompiles the
// unsigned shift; division is correct on both.
static int16_t toCell(int16_t p){
	if (p >= 0) return (int16_t)((uint16_t)p / 8);
	return -(int16_t)(((uint16_t)(-p) + 7) / 8);
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
	int16_t ex;
	integrate(&e->px, &e->sx, e->vx);
	if (e->vx > 0){
		ex = e->px + 15;
		if (edgeV(ex, e->py)){
			e->px = (ex & ~7) - 16; e->sx = 0; e->vx = 0; return 1;
		}
	} else if (e->vx < 0){
		if (edgeV(e->px, e->py)){
			e->px = (e->px & ~7) + 8; e->sx = 0; e->vx = 0; return 1;
		}
	}
	return 0;
}

// Move on Y with tile collision, applying the caller's gravity (the player uses
// the SMB dual gravity; enemies always fall hard).
static void moveY(Entity *e, int16_t grav){
	int16_t ey;
	e->bumped = 0;
	// Resting on the ground (and not rising from a jump): hold still. Probing
	// here -- after moveX has run -- means walking off a ledge falls the same
	// frame, and skipping the move avoids the gravity-vs-snap jitter the old
	// "skip gravity while grounded" guard was working around.
	if (e->vy >= 0 && edgeH(e->px, e->py + 16)){
		e->vy = 0; e->sy = 0;
		e->onGround = 1;
		return;
	}
	// Airborne (or moving up): apply gravity and integrate.
	e->vy += grav;
	if (e->vy > MAXFALL) e->vy = MAXFALL;
	integrate(&e->py, &e->sy, e->vy);
	if (e->vy > 0){
		ey = e->py + 15;
		if (edgeH(e->px, ey)){
			e->py = (ey & ~7) - 16; e->sy = 0; e->vy = 0;
		}
	} else if (e->vy < 0){
		if (edgeH(e->px, e->py)){
			// Record which cell the head struck (for block bumps); the leftmost
			// solid of the three samples is the one we ran into.
			e->bumpX = solid(e->px, e->py) ? toCell(e->px)
			         : solid(e->px + 7, e->py) ? toCell(e->px + 7)
			         : toCell(e->px + 15);
			e->bumpY = toCell(e->py);
			e->bumped = 1;
			e->py = (e->py & ~7) + 8; e->sy = 0; e->vy = 0;
		}
	}
	// Grounded when a solid tile sits directly beneath the 16 px box.
	e->onGround = edgeH(e->px, e->py + 16);
}

static void respawnPlayer(void){
	player.px = PLAYER_START_X; player.py = PLAYER_START_Y;
	player.sx = 0; player.sy = 0;
	player.vx = 0; player.vy = 0; player.faceLeft = 0;
}

// Spawn a transient popup (world pixel position, blocks-sheet frame) in a free
// slot; it rises and fades. Silently dropped if all slots are busy.
static void spawnPop(int16_t x, int16_t y, uint8_t frame){
	uint8_t i;
	for (i = 0; i < NPOP; i++){
		if (!pops[i].t){ pops[i].x = x; pops[i].y = y; pops[i].t = POP_FRAMES;
		                 pops[i].frame = frame; return; }
	}
}

// A player ceiling-bump landed on cell (cx,cy): if it is a ? block, award a coin
// + score and spend it; if a brick, just nudge it. The player rises into the
// block's lower row, so a block whose authored top-left row is r is hit at r+1.
static void hitBlock(int16_t cx, int16_t cy){
	uint8_t i;
	for (i = 0; i < QBLOCK_N; i++){
		if (qblock_cells[i][1] + 1 == cy &&
		    (qblock_cells[i][0] == cx || qblock_cells[i][0] + 1 == cx)){
			qBumpT[i] = BUMP_FRAMES;
			if (!qUsed[i]){
				qUsed[i] = 1;
				score += COIN_SCORE; coins++; hudDirty = 1;
				spawnPop(qblock_cells[i][0] * 8, qblock_cells[i][1] * 8 - 8, BLK_COIN);
			}
			return;
		}
	}
	for (i = 0; i < BRICK_N; i++){
		if (brick_cells[i][1] + 1 == cy &&
		    (brick_cells[i][0] == cx || brick_cells[i][0] + 1 == cx)){
			uint8_t s;
			for (s = 0; s < NBB; s++){
				if (!brickBump[s].t || (brickBump[s].col == brick_cells[i][0] &&
				                        brickBump[s].row == brick_cells[i][1])){
					brickBump[s].col = brick_cells[i][0];
					brickBump[s].row = brick_cells[i][1];
					brickBump[s].t = BUMP_FRAMES;
					break;
				}
			}
			return;
		}
	}
}

static void playerUpdate(uint8_t inLo, uint8_t inHi){
	uint8_t a = inLo & 0x80;          // A button (JOY1L: axlr----): jump
	uint8_t run = inHi & 0x80;        // B button (JOY1H bit 7): run
	int8_t dir = 0;
	int16_t maxs = run ? RUN_MAX : WALK_MAX;
	int16_t acc  = run ? RUN_ACC : WALK_ACC;
	uint8_t fast;
	int16_t grav;
	if (inHi & 0x02){ dir = -1; player.faceLeft = 1; }
	else if (inHi & 0x01){ dir = 1; player.faceLeft = 0; }

	// Horizontal momentum. Pressing into the current motion accelerates toward
	// the speed ceiling; pressing the opposite way skids (a harder deceleration)
	// until the velocity reverses; no input applies ground friction (in the air
	// momentum is kept, the SMB way). Releasing run eases a run-speed dash back
	// down to the walk ceiling rather than snapping.
	if (dir > 0){
		if (player.vx < 0) player.vx += SKID_DEC;
		else { player.vx += acc; if (player.vx > maxs) player.vx = maxs; }
	} else if (dir < 0){
		if (player.vx > 0) player.vx -= SKID_DEC;
		else { player.vx -= acc; if (player.vx < -maxs) player.vx = -maxs; }
	} else if (player.onGround){
		if (player.vx > 0){ player.vx -= FRICTION; if (player.vx < 0) player.vx = 0; }
		else if (player.vx < 0){ player.vx += FRICTION; if (player.vx > 0) player.vx = 0; }
	}
	if (!run && player.onGround){
		if (player.vx > WALK_MAX){ player.vx -= FRICTION; if (player.vx < WALK_MAX) player.vx = WALK_MAX; }
		else if (player.vx < -WALK_MAX){ player.vx += FRICTION; if (player.vx > -WALK_MAX) player.vx = -WALK_MAX; }
	}

	// Coyote time: a few frames of jump grace after leaving the ground.
	if (player.onGround) coyote = COYOTE;
	else if (coyote) coyote--;

	// Jump takeoff: a fast (running) takeoff launches harder, SMB-style.
	fast = (player.vx > WALK_MAX || player.vx < -WALK_MAX);
	if (a && !prevA && coyote){
		player.vy = fast ? -JUMP_V_RUN : -JUMP_V;
		player.onGround = 0;
		coyote = 0;
	}
	prevA = a;

	// SMB variable jump: weak gravity while the button is held and the player is
	// still rising (the float), strong gravity once released or descending. The
	// fast/running tier falls a touch harder, as in SMB.
	if (a && player.vy < 0) grav = fast ? GRAV_UP_R : GRAV_UP;
	else                    grav = fast ? GRAV_DN_R : GRAV_DN;

	moveX(&player);
	moveY(&player, grav);
	if (player.bumped) hitBlock(player.bumpX, player.bumpY);

	if (player.py > level_ROWS * 8 + 16) respawnPlayer();

	// Animation: jump pose (2) in the air, 2-frame walk on the ground, else stand.
	if (!player.onGround) player.frame = 2;
	else if (player.vx > FRICTION || player.vx < -FRICTION) player.frame = (animT >> 3) & 1;
	else player.frame = 0;
}

// One autonomous enemy: paces along the ground, turning at a wall (moveX was
// blocked) or at a ledge (no ground one pixel ahead of its leading foot).
static void enemyUpdate(Entity *e){
	int16_t footX, footY;
	if (!e->active) return;
	e->vx = e->dir ? WMOVE_SPD : -WMOVE_SPD;
	footX = e->dir ? (e->px + 16) : (e->px - 1);
	footY = e->py + 16;
	if (e->onGround && !solid(footX, footY)){
		e->dir ^= 1;
		e->vx = -e->vx;
	}
	if (moveX(e)) e->dir ^= 1;
	moveY(e, GRAV_DN);
	e->faceLeft = !e->dir;
	e->frame = (animT >> 3) & 1;
	// Off the bottom of the world (shouldn't happen with ledge-turns): retire it.
	if (e->py > level_ROWS * 8 + 16) e->active = 0;
}

// Player vs enemies (16x16 AABB). Coming down onto an enemy's head stomps it and
// bounces the player; any other contact respawns the player.
static void checkStomp(void){
	int16_t px = player.px, py = player.py;
	uint8_t i;
	for (i = 0; i < NENEMY; i++){
		Entity *e = &enemies[i];
		int16_t ex, ey;
		if (!e->active) continue;
		ex = e->px; ey = e->py;
		if (px + 15 < ex || px > ex + 15 || py + 15 < ey || py > ey + 15) continue;
		if (player.vy > 0 && (py + 15) < (ey + 8)){
			e->active = 0;            // stomped
			player.vy = -BOUNCE;
			score += STOMP_SCORE; hudDirty = 1;
			spawnPop(ex, ey - 8, BLK_100);
		} else {
			respawnPlayer();          // hit from the side / below
			return;
		}
	}
}

static void setBGScroll(uint16_t hofs, uint16_t vofs){
	REG_BG1HOFS = (uint8_t)hofs;
	REG_BG1HOFS = (uint8_t)(hofs >> 8);
	REG_BG1VOFS = (uint8_t)vofs;
	REG_BG1VOFS = (uint8_t)(vofs >> 8);
}

// DMA one map column into its VRAM tilemap slot. The 64x32 VRAM map is two
// 32x32 screens (left = words MAP_BASE.., right = MAP_BASE+0x400..); a column's
// slot is (c & 63) and wraps every 64 columns, so writing column c overwrites
// whichever column last used that slot (always one off-screen by WIN_MARGIN).
// level_map is column-major, so a column is level_ROWS contiguous words.
static void streamCol(uint16_t c){
	uint16_t idx = c * level_ROWS;
	uint16_t slot = c & 63;
	uint16_t word = (slot < 32) ? (MAP_BASE + slot)
	                            : (MAP_BASE + 0x400 + (slot - 32));
	LoadVramColumn((const unsigned char *)&level_map[idx],
	               (uint16_t)(word << 1), level_ROWS * 2);
}

// Stream the columns that differ between the VRAM window (vramLo) and the
// desired window (winLo). updateCamera caps the per-frame step (bigger jumps go
// through fullReload), so the counts here are small.
static void streamWindow(void){
	uint16_t i, n;
	if (winLo > vramLo){
		n = winLo - vramLo;
		for (i = 0; i < n; i++) streamCol(vramLo + 64 + i);
		vramLo = winLo;
	} else if (winLo < vramLo){
		n = vramLo - winLo;
		for (i = 0; i < n; i++) streamCol(winLo + i);
		vramLo = winLo;
	}
}

// Track the player with the camera (clamped to the map), and slide the 64-wide
// resident column window so the view always has WIN_MARGIN columns of buffer on
// each side.
static void updateCamera(void){
	int16_t target = player.px + 8 - 128;
	uint16_t camCol, want;
	if (target < 0) target = 0;
	else if (target > CAM_MAX_X) target = CAM_MAX_X;
	camX = (uint16_t)target;

	camCol = camX >> 3;
	want = (camCol > WIN_MARGIN) ? (camCol - WIN_MARGIN) : 0;
	if (want > (uint16_t)(level_COLS - 64)) want = level_COLS - 64;
	winLo = want;
	// A large jump (the pit respawn snaps the camera back to the start) would
	// take more streaming than fits a vblank, so flag a full 64-column reload.
	if (want + 8 < vramLo || vramLo + 8 < want) fullReload = 1;
}

// Write one OBJ into the OAM mirror: screen position = world - camera, a CHARNUM
// and an attribute byte (priority 3, OBJ palette, H-flip when facing left). An
// off-screen or inactive entity is parked below the visible area.
static void drawObj(uint8_t slot, int16_t wx, int16_t wy, uint8_t ch,
                    uint8_t attr, uint8_t visible){
	int16_t sx = wx - camX, sy = wy - CAM_Y;
	uint8_t base = slot * 4;
	if (!visible || sx < -16 || sx > 255 || sy < -16 || sy > 239){
		oamCopy.Bytes[base + 0] = 0;
		oamCopy.Bytes[base + 1] = 0xE0;
	} else {
		oamCopy.Bytes[base + 0] = (uint8_t)sx;
		oamCopy.Bytes[base + 1] = (uint8_t)sy;
	}
	oamCopy.Bytes[base + 2] = ch;
	oamCopy.Bytes[base + 3] = attr;
}

// Advance the per-frame block/popup animation timers.
static void updateEffects(void){
	uint8_t i;
	for (i = 0; i < QBLOCK_N; i++) if (qBumpT[i]) qBumpT[i]--;
	for (i = 0; i < NBB; i++) if (brickBump[i].t) brickBump[i].t--;
	for (i = 0; i < NPOP; i++) if (pops[i].t){ pops[i].y--; pops[i].t--; }
}

// A nudge offset (px) for a block bump timer: rises to ~half BUMP_FRAMES then back.
static int16_t nudge(uint8_t t){
	return t >= (BUMP_FRAMES / 2) ? (int16_t)(BUMP_FRAMES - t) : (int16_t)t;
}

// OAM mirror: 0 player, 1..NENEMY enemies, then the block/popup overlays (drawn
// over the BG with OBJ palette 2, priority 3). drawObj culls anything off-screen.
static void drawFrame(void){
	uint8_t i;
	drawObj(0, player.px, player.py,
	        player.frame * player_STEP, player.faceLeft ? 0x70 : 0x30, 1);
	for (i = 0; i < NENEMY; i++){
		drawObj(1 + i, enemies[i].px, enemies[i].py,
		        player_TILES + enemies[i].frame * walker_STEP,
		        enemies[i].faceLeft ? 0x72 : 0x32, enemies[i].active);
	}
	// Spent ? blocks: the used-block sprite covers the BG ? tile, nudging up when
	// (re)hit. One OAM slot per ? block; off-screen ones are parked by drawObj.
	for (i = 0; i < QBLOCK_N; i++){
		drawObj(SLOT_Q + i, (int16_t)(qblock_cells[i][0] * 8),
		        (int16_t)(qblock_cells[i][1] * 8) - nudge(qBumpT[i]),
		        BLOCKS_BASE + BLK_USED * blocks_STEP, 0x34, qUsed[i]);
	}
	// Bricks: a sprite appears only during the nudge (the BG brick stays put).
	for (i = 0; i < NBB; i++){
		drawObj(SLOT_BRICK + i, (int16_t)(brickBump[i].col * 8),
		        (int16_t)(brickBump[i].row * 8) - nudge(brickBump[i].t),
		        BLOCKS_BASE + BLK_BRICK * blocks_STEP, 0x34, brickBump[i].t != 0);
	}
	// Coin / score popups.
	for (i = 0; i < NPOP; i++){
		drawObj(SLOT_POP + i, pops[i].x, pops[i].y,
		        BLOCKS_BASE + pops[i].frame * blocks_STEP, 0x34, pops[i].t != 0);
	}
}

// HUD 2bpp palette (colour 0 transparent backdrop, 1-3 white text) loaded at the
// HUD palette slot; and the blank-cell fill word for the BG3 map (tile 0x20 =
// space glyph, high byte 0x30 = priority + palette 4). Both const so they DMA
// from ROM (a RAM source mis-banks on some toolchains).
static const uint8_t hud_pal[8] = { 0x00, 0x00, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F };
static const uint16_t kSpace = 0x3020;

// Decimal digits of v into b (most significant first): put5 writes 5 digits
// (score), put2 writes 2 (coins). Each digit is found by repeated subtraction of
// its place value with a ROM digit table -- deliberately NO `/` or `%`, which
// calypsi miscompiles for 16-bit values (its divide returned wrong digits).
// Correct on calypsi and vbcc. score/coins are uint16 (vbcc miscompiles 32-bit).
static const char DIGIT[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
static void put5(uint16_t v, uint8_t *b){
	uint8_t d;
	d = 0; while (v >= 10000){ v -= 10000; d++; } b[0] = (uint8_t)DIGIT[d];
	d = 0; while (v >= 1000){ v -= 1000; d++; }   b[1] = (uint8_t)DIGIT[d];
	d = 0; while (v >= 100){ v -= 100; d++; }      b[2] = (uint8_t)DIGIT[d];
	d = 0; while (v >= 10){ v -= 10; d++; }        b[3] = (uint8_t)DIGIT[d];
	b[4] = (uint8_t)DIGIT[v];
}
static void put2(uint16_t v, uint8_t *b){
	uint8_t d = 0;
	while (v >= 10){ v -= 10; d++; }
	b[0] = (uint8_t)DIGIT[d];
	b[1] = (uint8_t)DIGIT[v];
}

// Redraw the HUD score/coin digits by DMA into the BG3 tilemap (low/tile bytes
// only; the palette high byte was set when the map was cleared). hudBuf is a
// file-scope global so the DMA has a stable RAM source (like oamCopy).
static uint8_t hudBuf[6];
static void updateHud(void){
	put5(score, hudBuf);
	LoadLoVram(hudBuf, HUD_CELL(1, 1), 5);
	put2(coins, hudBuf);
	LoadLoVram(hudBuf, HUD_CELL(23, 1), 2);
	hudDirty = 0;
}

void engineInit(void){
	uint16_t hi, i;

	LoadVram(player_chr, 0x0000, sizeof(player_chr));
	LoadVram(walker_chr, player_TILES * 32, sizeof(walker_chr));
	LoadVram(blocks_chr, BLOCKS_BASE * 32, sizeof(blocks_chr));
	LoadVram(level_chr, 0x2000, sizeof(level_chr));
	LoadVram(hudfont_chr, HUD_CHR, sizeof(hudfont_chr));
	LoadCGRam(level_pal, 0x00, sizeof(level_pal));
	LoadCGRam(player_pal, 0x80, sizeof(player_pal));
	LoadCGRam(walker_pal, 0x90, sizeof(walker_pal));
	LoadCGRam(blocks_pal, 0xA0, sizeof(blocks_pal));      // OBJ palette 2
	LoadCGRam(hud_pal, HUD_PAL, sizeof(hud_pal));         // BG3 2bpp HUD palette

	initOAMCopy(oamCopy.Bytes);
	// Make sprites 0..SLOT_TOTAL-1 16x16 (clear their size + X-high bits in the
	// OAM high table: 2 bits per sprite, 4 per byte, so SLOT_TOTAL sprites span
	// the first SLOT_TOTAL/4 + 1 bytes from 512).
	for (hi = 512; hi <= 512 + SLOT_TOTAL / 4; hi++) oamCopy.Bytes[hi] = 0x00;

	// Fixed score HUD on BG3: clear its tilemap to blank space (transparent, with
	// the HUD palette/priority preset in every high byte), then stamp the static
	// labels; updateHud() fills the digit fields.
	ClearVram((const unsigned char *)&kSpace, HUD_MAP << 1, 32 * 32);
	LoadLoVram((const unsigned char *)"SCORE", HUD_CELL(1, 0), 5);
	LoadLoVram((const unsigned char *)"COINS", HUD_CELL(22, 0), 5);
	LoadLoVram((const unsigned char *)"x", HUD_CELL(22, 1), 1);
	updateHud();

	respawnPlayer();

	// Enemies seeded from the level.tmx Entities layer (enemy_spawns from
	// spawns.inc); py starts above the local ground so gravity settles each onto it.
	for (i = 0; i < NENEMY; i++){
		enemies[i].px = enemy_spawns[i][0];
		enemies[i].py = enemy_spawns[i][1];
		enemies[i].sx = 0; enemies[i].sy = 0;
		enemies[i].vx = 0; enemies[i].vy = 0;
		enemies[i].dir = 0; enemies[i].onGround = 0; enemies[i].active = 1;
	}

	// Seed the camera/window, then DMA the first 64 columns into VRAM.
	updateCamera();
	for (i = 0; i < 64; i++) streamCol(winLo + i);
	vramLo = winLo;
	fullReload = 0;

	REG_OBJSEL = 0x60;   // sss=011: 16x16 small / 32x32 large, name base 0
	REG_BG1SC = 0x61;    // tilemap base word 0x6000, size 64x32
	REG_BG3SC = 0x18;    // BG3 (HUD) tilemap base word 0x1800, size 32x32
	REG_BG12NBA = 0x01;  // BG1 char base word 0x1000
	REG_BG34NBA = 0x02;  // BG3 char base word 0x2000 (the HUD font)
	REG_BG3HOFS = 0; REG_BG3HOFS = 0;   // HUD does not scroll
	REG_BG3VOFS = 0; REG_BG3VOFS = 0;
	REG_BGMODE = 0x09;   // mode 1 + BG3 priority (HUD draws on top)
	REG_TM = 0x15;       // BG1 + BG3 + OBJ on main screen
	REG_NMITIMEN = 0x01; // joypad autoread
	REG_INIDISP = 0x0F;  // screen on, full brightness

	drawFrame();
}

void engineFrame(void){
	uint8_t inLo, inHi;
	uint16_t i;

	// Two-phase HVBJOY vblank sync (read-only status, no clear side effect).
	while ((REG_HVBJOY & 0x80) != 0) { }
	while ((REG_HVBJOY & 0x80) == 0) { }

	// Time-sensitive PPU IO only: present the frame built last iteration and
	// stream any newly-revealed map columns. A full reload (respawn) re-DMAs all
	// 64 columns, which overruns vblank, so hold forced blank across it.
	LoadOAMCopy(oamCopy.Bytes, 0x0000, sizeof(union uOAMCopy));
	if (fullReload){
		REG_INIDISP = 0x80;
		for (i = 0; i < 64; i++) streamCol(winLo + i);
		vramLo = winLo;
		REG_INIDISP = 0x0F;
		fullReload = 0;
	} else {
		streamWindow();
	}
	setBGScroll(camX, CAM_Y);
	if (hudDirty) updateHud();    // small BG3 tilemap write, still in the vblank window

	// Compute the next frame during active display.
	while ((REG_HVBJOY & 0x01) != 0) { }
	inLo = REG_JOY1L;
	inHi = REG_JOY1H;

	animT++;
	playerUpdate(inLo, inHi);
	for (i = 0; i < NENEMY; i++) enemyUpdate(&enemies[i]);
	checkStomp();
	updateEffects();
	updateCamera();
	drawFrame();
}
