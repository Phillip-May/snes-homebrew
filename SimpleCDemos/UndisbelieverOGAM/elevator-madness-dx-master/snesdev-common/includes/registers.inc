;;
;; SNES Register Defines.
;;
;; Names and flags taken from Anomie's SNES Documents
;;
;;

.ifndef _REGISTERS_INC_
_REGISTERS_INC_ = 1

;; Tile Map Format
;;
;; cccccccc
;; vhopppcc
;;
;; cccccccccc = character of tile
;; ppp        = palette
;; o          = order
;; h          = horizontal flip
;; v          = vertical flip

;;; Flags and Masks
.define TILEMAP_CHAR_MASK    %0000001111111111
.define TILEMAP_PALETTE_MASK %0001110000000000
.define TILEMAP_ORDER_FLAG   %0010000000000000
.define TILEMAP_H_FLIP_FLAG  %0100000000000000
.define TILEMAP_V_FLIP_FLAG  %1000000000000000

;;; Flags and Masks (high byte only)
.define TILEMAP_HIGH_CHAR_MASK    %00000011
.define TILEMAP_HIGH_PALETTE_MASK %00011100
.define TILEMAP_HIGH_ORDER_FLAG   %00100000
.define TILEMAP_HIGH_H_FLIP_FLAG  %01000000
.define TILEMAP_HIGH_V_FLIP_FLAG  %10000000

;;; Shifts
.define TILEMAP_HIGH_PALETTE_SHIFT  2
.define TILEMAP_PALETTE_SHIFT      10
.define TILEMAP_HIGH_ORDER_SHIFT    5
.define TILEMAP_ORDER_SHIFT        13



;; OAM Table 0 Format
;;
;; xxxxxxxx
;; yyyyyyyy
;; cccccccc
;; vhoopppN
;;
;; xxxxxxxx = lower 8 bits of x position of object
;; yyyyyyyy = y position of object
;; cccccccc = character of object
;; N        = Character Table to use (equivalent to 9th bit of c)
;; ppp      = palette
;; oo       = order
;; h        = horizontal flip
;; v        = vertical flip
.struct OamFormat
	xPos	.byte
	yPos	.byte
	char	.byte
	attr	.byte
.endstruct

;;; Flags and Masks
.define OAM_ATTR_TABLE_FLAG   %00000001
.define OAM_ATTR_PALETTE_MASK %00001110
.define OAM_ATTR_ORDER_MASK   %00110000
.define OAM_ATTR_H_FLIP_FLAG  %01000000
.define OAM_ATTR_V_FLIP_FLAG  %10000000

.define OAM_CHARATTR_TABLE_FLAG   %0000000100000000
.define OAM_CHARATTR_PALETTE_MASK %0000111000000000
.define OAM_CHARATTR_ORDER_MASK   %0011000000000000
.define OAM_CHARATTR_H_FLIP_FLAG  %0100000000000000
.define OAM_CHARATTR_V_FLIP_FLAG  %1000000000000000

;;; Shifts
.define OAM_ATTR_PALETTE_SHIFT   1
.define OAM_ATTR_ORDER_SHIFT     4

.define OAM_CHARATTR_PALETTE_SHIFT   9
.define OAM_CHARATTR_ORDER_SHIFT     12


;; Screen Display
;; (write, any)
;;
;; f---bbbb
;;
;; f    = Force Blank
;; bbbb = Screen Brightness, 0 = Dull, 16 = max
INIDISP = $2100

;;; Flags and Masks
.define INIDISP_FORCE           %10000000
.define INIDISP_BRIGHTNESS_MASK %00001111



;; Object Size and Character Address
;; (write, v-blank)
;;
;; sssnnbbb
;;
;; sss = Object Size
;; nn  = Name Select
;; bbb = Name Base Address (addr>>14)
OBSEL = $2101

;;; Masks
.define OBSEL_SIZE_MASK        %11100000
.define OBSEL_NAME_MASK        %00011000
.define OBSEL_BASE_MASK        %00000111

;; Shifts
.define OBSEL_SIZE_SHIFT 5
.define OBSEL_NAME_SHIFT 3

;;; Object Sizes.
.define OBSEL_SIZE_8_16        %00000000
.define OBSEL_SIZE_8_32        %00100000
.define OBSEL_SIZE_8_64        %01000000
.define OBSEL_SIZE_16_32       %01100000
.define OBSEL_SIZE_16_64       %10000000
.define OBSEL_SIZE_32_64       %10100000
;;;; Undocumented
.define OBSEL_SIZE_16X32_32X64 %11000000
;;;; Undocumented
.define OBSEL_SIZE_16X32_32X32 %11100000

;;; Object Tile Address segments sizes.
;;; The VRAM address used is 16384 * `bbb`
.define OBSEL_BASE_WALIGN  8192
.define OBSEL_BASE_ALIGN  16384



;; OAM Address and Priority
;; (write, v-blank)
;;
;; p------b aaaaaaaa
;;
;; p        = Obj priority bit
;; b        = Table access (0 = Low Table, 1 = High Table)
;; aaaaaaaa = Address
OAMADDL = $2102
OAMADDH = $2103

;;; Flags and Masks
.define OAMADDL_ADDRESS_MASK  %11111111
.define OAMADDH_TABLE_MASK    %00000001

;;; Sprite Priority Select
;;; When set, `OAMADDL` handles sprite priority.
.define OAMADDH_PRIORITY_FLAG %10000000

;;; Table Select
.define OAMADDH_TABLE_LOW     %00000000
.define OAMADDH_TABLE_HIGH    %00000001



;; Word Access of OAMADDL/OAMADDH
;; (write, word, v-blank)
OAMADD = $2102

;;; Masks
.define OAMADD_TABLE_MASK       %0000000100000000
.define OAMADD_ADDRESS_MASK     %0000000011111111

;;; Sprite Priority Select
;;; When set, `OAMADDL` handles sprite priority.
.define OAMADD_PRIORITY_FLAG    %1000000000000000

;;; Table Select
.define OAMADD_TABLE_LOW        %0000000000000000
.define OAMADD_TABLE_HIGH       %0000000100000000



;; OAM Data Write
;; (write, write twice, v-blank)
OAMDATA = $2104



;; BG Mode and Character Size
;; (write, h-blank, v-blank)
;;
;; DCBAemmm
;;
;; A/B/C/D = Character size for BG1 - BG4 (1 = 16x16 tiles, 0 = 8x8 tiles)
;; e       = Mode 1 BG3 Priority
;; mmm     = BG Mode
BGMODE = $2105

;;; Masks
.define BGMODE_SIZE_MASK          %11110000
.define BGMODE_MODE_MASK          %00000111

;;; MODES
.define BGMODE_MODE0              %00000000
.define BGMODE_MODE1              %00000001
.define BGMODE_MODE1_BG3_PRIORITY %00001001
.define BGMODE_MODE2              %00000010
.define BGMODE_MODE3              %00000011
.define BGMODE_MODE4              %00000100
.define BGMODE_MODE5              %00000101
.define BGMODE_MODE6              %00000110
.define BGMODE_MODE7              %00000111

;;; BG Character Size
.define BGMODE_SIZE_BG1_8         %00000000
.define BGMODE_SIZE_BG1_16        %00010000
.define BGMODE_SIZE_BG2_8         %00000000
.define BGMODE_SIZE_BG2_16        %00100000
.define BGMODE_SIZE_BG3_8         %00000000
.define BGMODE_SIZE_BG3_16        %01000000
.define BGMODE_SIZE_BG4_8         %00000000
.define BGMODE_SIZE_BG4_16        %10000000



;; Screen Pixelation
;; (write, h-blank, v-blank)
;;
;; xxxxDCBA
;;
;; A/B/C/D = Affect BG1 - BG4
;; xxxx    = pixel size (0 to 16)
MOSAIC = $2106

;;; Masks
.define MOSAIC_PIXEL_MASK  %11110000
.define MOSAIC_BG_MASK     %00001111

;;; Left shift for pixel size
.define MOSAIC_PIXEL_SHIFT 4

;;; BGs to Affect
.define MOSAIC_BG1 %00000001
.define MOSAIC_BG2 %00000010
.define MOSAIC_BG3 %00000100
.define MOSAIC_BG4 %00001000



;; BG Tilemap Address and Size
;; (write, v-blank)
;;
;; aaaaaass
;;
;; aaaaaa = Tilemap base address in VRAM (Addr>>10)
;; ss     = Tilemap size
BG1SC = $2107
BG2SC = $2108
BG3SC = $2109
BG4SC = $210A

;;; Masks
.define BGXSC_BASE_MASK     %11111100
.define BGXSC_MAP_SIZE_MASK %00000011

;;; Shifts
.define BGXSC_BASE_SHIFT 2

;;; Sizes
.define BGXSC_SIZE_32X32 %00000000
.define BGXSC_SIZE_64X32 %00000001
.define BGXSC_SIZE_32X64 %00000010
.define BGXSC_SIZE_64X64 %00000011

;;; Address segments sizes.
;;; The VRAM address used is 2048 * `aaaaaa`
.define BGXSC_BASE_WALIGN 1024
.define BGXSC_BASE_ALIGN  2048



;; BG1/2 Character Address
;; (write, v-blank)
;;
;; bbbbaaaa
;;
;; aaaa - Base address for BG1 (Addr>>13)
;; bbbb - Base address for BG2 (Addr>>13)
BG12NBA = $210B

;;; Masks
.define BG12NBA_BG1_MASK %00001111
.define BG12NBA_BG2_MASK %11110000

;;; Shifts
.define BG12NBA_BG2_SHIFT 4

;;; Address segments sizes.
;;; The VRAM address used is 8192 * `aaaa`
.define BG12NBA_BASE_WALIGN 4096
.define BG12NBA_BASE_ALIGN  8192



;; BG 3/4 Character Address
;; (write, v-blank)
;;
;; bbbbaaaa
;;
;; aaaa - Base address for BG3 (Addr>>13)
;; bbbb - Base address for BG4 (Addr>>13)
BG34NBA = $210C

;;; Masks
.define BG34NBA_BG3_MASK %00001111
.define BG34NBA_BG4_MASK %11110000

;;; Shifts
.define BG34NBA_BG4_SHIFT 4

;;; Address segments sizes.
;;; The VRAM address used is 8192 * `aaaa`
.define BG34NBA_BASE_WALIGN 4096
.define BG34NBA_BASE_ALIGN  8192



;; BG Horizontal Scroll
;; (write, write twice, h-blank, v-blank)
;;
;; ------xx xxxxxxxx
;;
;; x = BG Horizontal offset
BG1HOFS = $210D
BG2HOFS = $210F
BG3HOFS = $2111
BG4HOFS = $2113

;; Mode 7 BG Horizontal Scroll
;; (write, write twice, h-blank, v-blank)
;;
;; ---mmmmm mmmmmmmm
;;
;; m = Mode 7 BG Offset (M7HOFS only)
M7HOFS = $210D

;; BG Vertical Scroll
;; (write, write twice, h-blank, v-blank)
;;
;; ------yy yyyyyyyy
;;
;; y = BG Vertical offset
BG1VOFS = $210E
BG2VOFS = $2110
BG3VOFS = $2112
BG4VOFS = $2114

;; Mode 7 BG Vertical Scroll
;; (write, write twice, h-blank, v-blank)
;;
;; ---mmmmm mmmmmmmm
;;
;; m = Mode 7 BG1 Offset (M7VOFS only)
M7VOFS = $210E



;; Video Port Control
;; (write, v-blank)
;;
;; i---mmii
;;
;; i  = Address increment mode
;; ii = Address increment amount
;; mm = Address remapping
;;
;; According to the manual only `ii` or `mm` should be set.
VMAIN = $2115

;;; Masks
.define VMAIN_INCREMENT_MODE_MASK   %10000000
.define VMAIN_INCREMENT_AMOUNT_MASK %00000011
.define VMAIN_REMAP_MASK            %00001100

;;; Increment after writing $2118 or reading $2139
.define VMAIN_INCREMENT_LOW  %00000000
;;; Increment after writing $2119 or reading $213A (recommended)
.define VMAIN_INCREMENT_HIGH %10000000

;;; Increment Amount
.define VMAIN_INCREMENT_1    %00000000
.define VMAIN_INCREMENT_32   %00000001
.define VMAIN_INCREMENT_128  %00000010

;;; Address remapping
.define VMAIN_REMAP_FOR_32  %00000100
.define VMAIN_REMAP_FOR_64  %00001000
.define VMAIN_REMAP_FOR_128 %00001100



;; VRAM Word Address
;; (write, word, v-blank)
VMADD  = $2116
VMADDL = $2116
VMADDH = $2117



;; VRAM Data Write
;; (write, word, v-blank)
VMDATA  = $2118
VMDATAL = $2118
VMDATAH = $2119


;; Mode 7 Select
;; (write, v-blank)
;;
;; oo----vh
;;
;; oo = Out of screen area
;;        00 - Screen Repeat
;;        01 - Color 0
;;        10 - Tile 0
;; h  = horizontal flip
;; v  = vertical flip
M7SEL = $211A

;;; Out of screen area
.define M7SEL_OUTSIDE_REPEAT  %00000000
.define M7SEL_OUTSIDE_COLOR0  %10000000
.define M7SEL_OUTSIDE_TILE0   %11000000

;;; Horizontal Flip
.define M7SEL_HORIZONTAL_FLIP %00000001

;;; Vertical flip
.define M7SEL_VERTICAL_FLIP   %00000010



;; Mode 7 Matrix
;; (write, write twice, 1:7:8 signed fixed point, h-blank, v-blank)
;;
;; The Matrix transformation is:
;;    [ X ]   [ M7A  M7B ]   [ SX + M7HOFS - M7X ]   [ M7X ]
;;    [ Y ] = [ M7C  M7D ] * [ SY + M7VOFS - M7Y ] + [ M7Y ]
;;
;; Where SX and SY are screen coordinates
M7A = $211B
M7B = $211C
M7C = $211D
M7D = $211E

;; Mode 7 Center
;; (write, write twice, 13 bit signed integer, h-blank, v-blank)
M7X = $211F
M7Y = $2120


;; CGRAM Address
;; (write, h-blank, v-blank)
CGADD = $2121

;;; Starting color of the OAM palette
CGADD_OBJ_PALETTE = 128


;; CGRAM Data Write
;; (write, write twice, h-blank, v-blank)
;;
;; -bbbbbgg gggrrrrr
CGDATA = $2122

;; Masks
.define CGDATA_BLUE_MASK  %0111110000000000
.define CGDATA_GREEN_MASK %0000001111100000
.define CGDATA_RED_MASK   %0000000000011111

;; Shifts
.define CGDATA_BLUE_SHIFT  10
.define CGDATA_GREEN_SHIFT  5



;; Window Mask Settings
;; (write, h-blank, v-blank)
;;
;; For each nibble:
;;   EIei
;;
;;   e = Enable Window 1
;;   i = Window 1 In/Out (1 = Inside, 0 = Outside)
;;   E = Enable Window 2
;;   I = Window 2 In/Out (1 = Inside, 0 = Outside)
;; 
W12SEL  = $2123
W34SEL  = $2124
WOBJSEL = $2125

;;; Enable
.define WSEL_WIN1_ENABLE   %0010
.define WSEL_WIN2_ENABLE   %1000

;;; Window In/Out
.define WSEL_WIN1_INSIDE   %0000
.define WSEL_WIN1_OUTSIDE  %0001
.define WSEL_WIN2_INSIDE   %0000
.define WSEL_WIN2_OUTSIDE  %0100

;;; Masks
.define W12SEL_BG1_MASK    %00001111
.define W12SEL_BG2_MASK    %11110000
.define W34SEL_BG3_MASK    %00001111
.define W34SEL_BG4_MASK    %11110000
.define WOBJSEL_OBJ_MASK   %00001111
.define WOBJSEL_COLOR_MASK %11110000

;;; Shifts
.define W12SEL_BG1_SHIFT    0
.define W12SEL_BG2_SHIFT    4
.define W34SEL_BG3_SHIFT    0
.define W34SEL_BG4_SHIFT    4
.define WOBJSEL_OBJ_SHIFT   0
.define WOBJSEL_COLOR_SHIFT 4


;; Window 1 Left Position
;; (write, h-blank, v-blank)
WH0 = $2126

;; Window 1 Right Position
;; (write, h-blank, v-blank)
WH1 = $2127

;; Window 2 Left Position
;; (write, h-blank, v-blank)
WH2 = $2128

;; Window 2 Right Position
;; (write, h-blank, v-blank)
WH3 = $2129



;; Window mask logic for Backgrounds
;; (write, h-blank, v-blank)
;;
;; 44332211
;;
;; For each bit pair:
;;   00 = OR
;;   01 = AND
;;   10 = XOR
;;   11 = XNOR
WBGLOG = $212A

;;; Logic
.define WBGLOG_OR   %00
.define WBGLOG_AND  %01
.define WBGLOG_XOR  %10
.define WBGLOG_XNOR %11

;;; Masks
.define WBGLOG_BG1_MASK %00000011
.define WBGLOG_BG2_MASK %00001100
.define WBGLOG_BG3_MASK %00110000
.define WBGLOG_BG4_MASK %11000000

;;; Shifts
.define WBGLOG_BG1_SHIFT 0
.define WBGLOG_BG2_SHIFT 2
.define WBGLOG_BG3_SHIFT 4
.define WBGLOG_BG4_SHIFT 6



;; Window mask logic for Objects and Color Window
;; (write, h-blank, v-blank)
;;
;; ----ccoo
;;
;; For each bit pair:
;;   00 = OR
;;   01 = AND
;;   10 = XOR
;;   11 = XNOR
WOBJLOG = $212B

;;; Logic
.define WOBJLOG_OR   %00
.define WOBJLOG_AND  %01
.define WOBJLOG_XOR  %10
.define WOBJLOG_XNOR %11

;;; Masks
.define WOBJLOG_BG1_MASK %00000011
.define WOBJLOG_BG2_MASK %00001100
.define WOBJLOG_BG3_MASK %00110000
.define WOBJLOG_BG4_MASK %11000000

;;; Shifts
.define WOBJLOG_OBJ_SHIFT   0
.define WOBJLOG_COLOR_SHIFT 2



;; Main Screen Designation
;; (write, h-blank, v-blank)
;;
;; ----o4321
;;
;; 1/2/3/4/o - enable BG1 - BG4 & Objects for the main screen
TM = $212C

;;; layers to enable
.define TM_BG1 %00000001
.define TM_BG2 %00000010
.define TM_BG3 %00000100
.define TM_BG4 %00001000
.define TM_OBJ %00010000


;; Sub Screen Designation
;; (write, h-blank, v-blank)
;;
;; ----o4321
;;
;; 1/2/3/4/o - enable BG1 - BG4 & Objects for the sub screen
TS = $212D

;;; layers to enable
.define TS_BG1 %00000001
.define TS_BG2 %00000010
.define TS_BG3 %00000100
.define TS_BG4 %00001000
.define TS_OAJ %00010000

;; Window Mask Designation for the Main Screen
;; (write, h-blank, v-blank)
;;
;; ----o4321
;;
;; 1/2/3/4/o - enable window masking for BG1-BG4 & Objects on the main screen
TMW = $212E

;;; layers to enable
.define TMS_BG1 %00000001
.define TMS_BG2 %00000010
.define TMS_BG3 %00000100
.define TMS_BG4 %00001000
.define TMS_OAM %00010000

;; Window Mask Designation for the Sub Screen
;; (write, h-blank, v-blank)
;;
;; ----o4321
;;
;; 1/2/3/4/o - enable window masking for BG1 - BG4 & Objects for the sub screen
TSW = $212F

;;; layers to enable
.define TSW_BG1 %00000001
.define TSW_BG2 %00000010
.define TSW_BG3 %00000100
.define TSW_BG4 %00001000
.define TSW_OAM %00010000



;; Color Addition Select
;; (write, h-blank, v-blank)
;;
;; ccmm--sd
;;
;; cc = clip color
;;       00 - Never
;;       01 - Outside color window
;;       10 - Inside color window
;;       11 - always
;;
;; mm = prevent color math
;;       00 - Never
;;       01 - Outside color window
;;       10 - Inside color window
;;       11 - always
;;
;; s  = Add subscreen enable
;; d  = Direct color mode for 256 color BG
CGWSEL = $2130

;;; Flags
.define CGWSEL_ADD_SUBSCREEN_FLAG %00000010
.define CGWSEL_DIRECT_COLOR_FLAG  %00000001

;;; Masks
.define CGWSEL_CLIP_MASK    %11000000
.define CFWSEL_PREVENT_MASK %00110000

;;; Shifts
.define CGWSEL_CLIP_SHIFT    6
.define CGWSEL_PREVENT_SHIFT 4

;;; Clips
.define CGWSEL_CLIP_NEVER   %00000000
.define CGWSEL_CLIP_INSIDE  %01000000
.define GGWSEL_CLIP_OUTSIDE %10000000
.define CGWSEL_CLIP_ALWAYS  %11000000

;;; Prevent Color Math
.define CGWSEL_PREVENT_NEVER   %00000000
.define CGWSEL_PREVENT_INSIDE  %00010000
.define GGWSEL_PREVENT_OUTSIDE %00100000
.define CGWSEL_PREVENT_ALWAYS  %00110000



;; Color Math Designation
;; (write, h-blank, v-blank)
;;
;; shbo4321
;;
;; s           = add/subtract colors (0 = add, 1 = subtract)
;; h           = Half color math
;; 4/3/2/1/o/b = Enable color math for BG1 - BG4, Objects and Backdrop
CGADSUB = $2131

;;; Color Add/Subtract
.define CGADSUB_COLOR_ADD       %00000000
.define CGADSUB_COLOR_SUBRACT   %10000000

;;; Half Color Math
.define CGADSUB_HALF_COLOR      %01000000

;;; Enable
.define CGADSUB_ENABLE_BG1      %00000001
.define CGADSUB_ENABLE_BG2      %00000010
.define CGADSUB_ENABLE_BG3      %00000100
.define CGADSUB_ENABLE_BG4      %00001000
.define CGADSUB_ENABLE_OBJ      %00010000
.define CGADSUB_ENABLE_BACKDROP %00100000



;; Fixed Color Data
;; (write, h-blank, v-blank)
;;
;; bgrccccc
;;
;; b/g/r = color plane(s)
;; ccccc = color intensity
COLDATA = $2132

;;; Masks
.define COLDATA_PLANE_MASK       %11100000
.define COLDATA_INTENSITY_MASK   %00011111

;;; Planes
.define COLDATA_PLANE_BLUE_FLAG  %10000000
.define COLDATA_PLANE_GREEN_FLAG %01000000
.define COLDATA_PLANE_RED_FLAG   %00100000



;; Screen Mode/ Video Select
;; (write, h-blank, v-blank)
;;
;; se--poIi
;;
;; s = External Sync
;; e = Enable Mode 7 EXTBG
;; p = Enable Pseudo-hires mode
;; o = Enable overscan
;; I = Oject Interlace
;; i = Screen Interlace
SETINI = $2133

;;; Flags
.define SETINI_EXTSYNC          %10000000
.define SETINI_EXTBG            %01000000
.define SETINI_PSUEDO_HIRES     %00001000
.define SETINI_OVERSCAN         %00000100
.define SETINI_OBJ_INTERLACE    %00000010
.define SETINI_SCREEN_INTERLACE %00000001



;; Multiplication Result
;; (read, long, h-blank, v-blank)
;;
;; Two's compliment product of 16 bit `M7A` and the
;; most recent byte written to `M7B`.
;; 
;; Should not be read during Mode 7 rendering.
MPY  = $2134
MPYL = $2134
MPYM = $2135
MPYH = $2136



;; Software Latch for H/V Counter
;; (read, any)
;;
;; See `OPHCT` and `OPVCT`
SLHV = $2137



;; Data for OAM Read
;; (read, read twice, h-blank, v-blank)
OAMDATAREAD = $2138



;; VRAM Data Read
;; (read, word, v-blank)
VMDATAREAD  = $2139
VMDATALREAD = $2139
VMDATAHREAD = $213A



;; CGRAM Data Read
;; (read, v-blank)
CGDATAREAD = $213B



;; Horizontal Scanline Location
;; (read, read twice, any)
;;
;; --------x xxxxxxxx
;;
;; Notes:
;;   * Latched on `SLHV` read or Pin 6 of Port 2 (bit 6 `WRIO`) falling trigger.
;;   * Range 0 to 339
;;   * Visible range: 22 - 277
;;   * Bits 1-7 of high byte are open bus. Should mask bits 0-8.
;;   * High/Low order reset on `STAT78` read.
OPHCT = $213C

;; Vertical Scanline Location
;; (read, read twice, any)
;;
;; --------y yyyyyyyy
;;
;; Notes:
;;   * Latched on `SLHV` read or Pin 6 of Port 2 (bit 6 `WRIO`) falling trigger.
;;   * Range 0 to 261 NTSC (+1 when interlacing)
;;   * Range 0 to 312 PAL (+1 when interlacing)
;;   * Visible range 1 - 224, 1 - 239? on overscan.
;;   * Bits 1-7 of high byte are open bus. Should mask bits 0-8.
;;   * High/Low order reset on `STAT78` read.
OPVCT = $213D



;; PPU 5c77 Status Flag and Version
;; (read, any)
;;
;; trm-vvvv
;;
;; t    = time over flag (more than 34 sprite tiles on a scanline)
;; r    = Range overflow (more than 32 sprites on a scanline)
;; m    = master/slave select. Unknown function
;; vvvv = 5c77 chip number.
STAT77 = $213E

;;; Masks
.define STAT77_VERSION_MASK %00001111

;;; Flags
.define STAT77_TIME_FLAG    %10000000
.define STAT77_RANGE_FLAG   %01000000



;; PPU 5c88 Status Flag and Version
;; (read, any)
;;
;; fl-pvvvv
;;
;; f    = Interlace flag (toggled every v-blank)
;; l    = External Latch Flag (set when PPU counters latched)
;; p    = NTSC/PAL mode (1 = PAL, 0 = NTSC)
;; vvvv = 5c78 chip number.
STAT78 = $213F

;;; Masks
.define STAT78_VERSION_MASK   %00001111
.define STAT78_PAL_MASK       %00010000

.define STAT78_PAL            %00010000
.define STAT78_NTSC           %00000000

;;; Flags
.define STAT78_INTERLACE_FLAG %10000000
.define STAT78_EXT_LATCH_FLAG %01000000



;; APU I/O Register
;; (read/write, any)
APUIO0 = $2140
APUIO1 = $2141
APUIO2 = $2142
APUIO3 = $2143



;; WRAM Data Read/Write
;; (read/write, any)
;;
;; Will not work with a DMA transfer from WRAM to WRAM.
WMDATA = $2180

;; WRAM Address
;; (write, long, any)
;;
;; -------x xxxxxxxx xxxxxxxx
WMADD  = $2181
WMADDL = $2181
WMADDM = $2182
WMADDH = $2183



;; NES-style Joypad Access Port 1
;; (read/write, any)
;;
;; Read:  ------ca
;; Write: -------l
;;
;; l = latch line of both ports
;; a = data 1 line
;; b = data 2 Line
JOYSER0 = $4016

;;; Write
.define JOYSER0_LATCH %00000001

;;; Read
.define JOYSER0_DATA1 %00000001
.define JOUSER0_DATA2 %00000010


 
;; NES-style Joypad Access Port 2
;; (read, any)
;;
;; ---111db
;;
;; b = data 1 line
;; d = data 2 line
;; 1 = Always set
JOYSER1 = $4017
 
;;; Read
.define JOYSER1_DATA1 %00000001
.define JOUSER1_DATA2 %00000010



;; Interrupt Enable Flags
;; (write, any)
;;
;; n-yx---a
;;
;; n  = Enable V-Blank (NMI)
;; yx = Enable IRQ
;;       00 - Disabled
;;       01 - On V-Counter (`VTIME`) 
;;       10 - On H-Counter (`HTIME`)
;;       11 - On V-Counter and H-Counter (`VTIME` and `HTIME`)
;; a  = Auto read joypad
NMITIMEN = $4200

;;; Flags
.define NMITIMEN_VBLANK_FLAG  %10000000
.define NMITIMEN_AUTOJOY_FLAG %00000001
.define NMITIMEN_IRQ_H_FLAG   %00010000
.define NMITIMEN_IRQ_V_FLAG   %00100000



;; Programmable I/O Port
;; (write, any)
;;
;; ab------
;;
;; a = Pin 6 of Port 1
;; b = Pin 6 of Port 2
;;
;; H/V Counters (`OPHCT` and `OPVCT` ) will be latched on lowering
;; trigger of Port 2 (Transition from 1 to 0).
WRIO = $4201

;;; Masks
.define WRIO_PORT1 %10000000
.define WRIO_PORT2 %01000000



;; Multiplicand A
;; (write, any)
WRMPYA = $4202

;; Multiplicand B
;; (write, any)
;;
;; After 8 CPU cycles `RDMPY` will contain the unsigned product of `WRMPYA` and `WRMPYB`.
WRMPYB = $4203 



;; Dividend C
;; (write, word, any)
WRDIV  = $4204
WRDIVL = $4204
WRDIVH = $4205

;; Divisor B
;; (write, any)
;;
;; After 16 CPU cycles `RDDIV` will contain the unsigned quotient of `WRDIV` and `WRDIVB`.
;; The remainder of the division is stored in `RDMPY`.
WRDIVB = $4206



;; H Timer
;; (write, word, any)
;;
;; ------h hhhhhhhh
;;
;; hhhhhhhhh = horizontal counter target (Range 0 - 339)
;; 
;; See `MNITIMEN` for details.
HTIME  = $4207
HTIMEL = $4207
HTIMEH = $4208

;; V Timer
;; (write, word, any)
;;
;; ------v vvvvvvvv
;;
;; vvvvvvvvvv = vertical counter target (Range 0 - 261 NTSC and 0 - 311 PAL)
;; 
;; See `MNITIMEN` for details.
VTIME  = $4209
VTIMEL = $4209
VTIMEH = $420A



;; DMA Enable
;; (write, any)
;;
;; 76543210
;;
;; 7/6/5/4/3/2/1 = Enable DMA channel.
;;
;; When a DMA is activated the CPU will be paused until all DMAs are completed.
MDMAEN = $420B

;;; Flags
.define MDMAEN_DMA0 %00000001
.define MDMAEN_DMA1 %00000010
.define MDMAEN_DMA2 %00000100
.define MDMAEN_DMA3 %00001000
.define MDMAEN_DMA4 %00010000
.define MDMAEN_DMA5 %00100000
.define MDMAEN_DMA6 %01000000
.define MDMAEN_DMA7 %10000000



;; HDMA Enable
;; (write, any)
;;
;; 76543210
;;
;; 7/6/5/4/3/2/1 = Enable HDMA channel.
HDMAEN = $420C

;;; Flags
.define HDMAEN_DMA0 %00000001
.define HDMAEN_DMA1 %00000010
.define HDMAEN_DMA2 %00000100
.define HDMAEN_DMA3 %00001000
.define HDMAEN_DMA4 %00010000
.define HDMAEN_DMA5 %00100000
.define HDMAEN_DMA6 %01000000
.define HDMAEN_DMA7 %10000000



;; ROM Access Speed
;; (write, any)
;;
;; -------r
;;
;; r = FastROM select (1 = FastROM, 0 = SlowROM)
MEMSEL = $420D

;;; Flags
.define MEMSEL_FASTROM %00000001
.define MEMSEL_SLOWROM %00000000



;; NMI Flag and 5A22 Version
;; (read, any)
;;
;; n---vvvv
;;
;; n    = MNI Flag. Set on start of V-Blank, cleaered on read
;;        or at end of V-Blank.
;; vvvv = 5A22 Chip version number
;;
;; This register should be read during the MNI Handler.
RDNMI = $4210

;;; Flags
.define RDNMI_NMI_FLAG          %10000000
.define RDNMI_5A22_VERSION_MASK %00001111



;; IRQ Flag
;; (read, any)
;;
;; i-------
;;
;; i = IRQ Flag. Set just after IRQ fires, cleared on read.
;;
;; This register should be read during the IRQ Handler.
TIMEUP = $4211



;; PPU Status
;; (read, any)
;;
;; vh-----a
;;
;; v = V-Blank Flag. Set during V-Blank
;; h = H-Blank Flag. Set during H-Blank
;; a = Auto Joypad read status. Set while reading, cleared when completed.
HVJOY = $4212

;;; Flags
.define HVJOY_VBLANK  %10000000
.define HVJOY_HBLANK  %01000000
.define HVJOY_AUTOJOY %00000001



;; Programmable I/O Port (in-port)
;;
;; ab------
;;
;; a = Pin 6 of Port 1
;; b = Pin 6 of Port 2
RDIO = $4201

;;; Masks
.define RDIO_PORT1 %10000000
.define RDIO_PORT2 %01000000



;; Quotient of Divide Result
;; (read, word, any)
;;
;; Set 16 CPU cycles after `WRDIVB` write
RDDIV  = $4214
RDDIVL = $4214
RDDIVH = $4215



;; Multiplication Product of Divide Remainder
;; (read, word, any)
;;
;; Set 16 CPU cycles after `WRDIVB` write or 8 CPU cycles after `WRMPYB` write. 
RDMPY  = $4216
RDMPYL = $4216
RDMPYH = $4217



;; Autojoy Controller Ports
;; (read, word, any)
;;
;; Should not be read when bit 0 of `HVJOY` set.
JOY1  = $4218
JOY2  = $421A
JOY3  = $421C
JOY4  = $421E

JOY1L = $4218
JOY1H = $4219
JOY2L = $421A
JOY2H = $421B
JOY3L = $421C
JOY3H = $421D
JOY4L = $421E
JOY4H = $421F

;;; Flags
.define JOY_B          %1000000000000000
.define JOY_Y          %0100000000000000
.define JOY_SELECT     %0010000000000000
.define JOY_START      %0001000000000000
.define JOY_UP         %0000100000000000
.define JOY_DOWN       %0000010000000000
.define JOY_LEFT       %0000001000000000
.define JOY_RIGHT      %0000000100000000
.define JOY_A          %0000000010000000
.define JOY_X          %0000000001000000
.define JOY_L          %0000000000100000
.define JOY_R          %0000000000010000

.define JOY_BUTTONS    %1100000011000000
.define JOY_DPAD       %0000111100000000
.define JOY_SHOLDER    %0000000000110000
.define JOY_TYPE_MASK  %0000000000001111

.define JOYH_B          %10000000
.define JOYH_Y          %01000000
.define JOYH_SELECT     %00100000
.define JOYH_START      %00010000
.define JOYH_UP         %00001000
.define JOYH_DOWN       %00000100
.define JOYH_LEFT       %00000010
.define JOYH_RIGHT      %00000001
.define JOYL_A          %10000000
.define JOYL_X          %01000000
.define JOYL_L          %00100000
.define JOYL_R          %00010000
.define JOYL_TYPE_MASK	%00001111

.define JOYH_DPAD       %00001111

.define JOY_TYPE_CONTROLLER      %0000
.define JOY_TYPE_MOUSE           %0001
.define JOY_TYPE_KONAMI_JUSTIFER %1110

;; DMA Control For Channel 0
;; (read, write, any)
;;
;; da-ifttt
;;
;; d    = Transfer direction (0 = CPU -> PPU, 1 = PPU -> CPU)
;; a    = HDMA Addressing Mode (0 = absolute, 1 = indirect). HDMA Only
;; i    = DMA Address Increment (0 = Increment, 1 = Decrement). DMA Only
;; f    = DMA Fixed Transfer (1 = Address will not change). DMA Only.
;; ttt  = Transfer Mode
;;          000 - 1 register,  write once         ( r                )
;;          001 - 2 registers, write once         ( r, r+1           )
;;          010 - 1 register,  write twice        ( r, r             )
;;          011 - 2 registers, write twice each   ( r, r,   r+1, r+1 )
;;          100 - 4 registers, write once         ( r, r+1, r+2, r+3 )
;;          101 - 2 register,  write twice alt  - undocumented
;;          110 - 1 register,  write twice      - undocumented
;;          111 - 2 registers, write twice each - undocumented
;;
DMAP0 = $4300
DMAP1 = $4310
DMAP2 = $4320
DMAP3 = $4330
DMAP4 = $4340
DMAP5 = $4350
DMAP6 = $4360
DMAP7 = $4370

;;; Masks
.define DMAP_TRANSFER_MASK %00000111

;;; Transfer Direction
.define DMAP_DIRECTION_TO_PPU %00000000
.define DMAP_DIRECTION_TO_CPU %10000000

;;; HDMA Addressing
.define DMAP_ADDRESSING_ABSOLUTE %00000000
.define DMAP_ADDRESSING_INDIRECT %01000000

;;; DMA Address Increment
.define DMAP_ADDRESS_INCREMENT %00000000
.define DMAP_ADDRESS_DECREMENT %00010000
.define DMAP_ADDRESS_FIXED     %00001000

;;; Transfer Mode
.define DMAP_TRANSFER_1REG              %000
.define DMAP_TRANSFER_2REGS             %001
.define DMAP_TRANSFER_WRITE_TWICE       %010
.define DMAP_TRANSFER_READ_TWICE        %010
.define DMAP_TRANSFER_2REGS_WRITE_TWICE %011
.define DMAP_TRANSFER_4REGS             %100



;; DMA Destination Register
;; (read, write, any)
;;
;; Bus B Address (PPU Memory Address, maps to $00:21xx)
BBAD0 = $4301
BBAD1 = $4311
BBAD2 = $4321
BBAD3 = $4331
BBAD4 = $4341
BBAD5 = $4351
BBAD6 = $4361
BBAD7 = $4371



;; DMA Source Address
;; (read, write, long, any)
;;
;; Bus A Address (CPU Memory Address)
;;
;; DMA:
;;  * Starting address for DMA Transfer.
;;  * Address will be incremented/decremented
;;  * Bank will not change during DMA Transfer.
;;
;; HDMA:
;;  * Table address for HDMA Transfer.
A1T0  = $4302
A1T1  = $4312
A1T2  = $4322
A1T3  = $4332
A1T4  = $4342
A1T5  = $4352
A1T6  = $4362
A1T7  = $4372

A1T0L = $4302
A1T0H = $4303
A1B0  = $4304
A1T1L = $4312
A1T1H = $4313
A1B1  = $4314
A1T2L = $4322
A1T2H = $4323
A1B2  = $4324
A1T3L = $4332
A1T3H = $4333
A1B3  = $4334
A1T4L = $4342
A1T4H = $4343
A1B4  = $4344
A1T5L = $4352
A1T5H = $4353
A1B5  = $4354
A1T6L = $4362
A1T6H = $4363
A1B6  = $4364
A1T7L = $4372
A1T7H = $4373
A1B7  = $4374



;; DMA Size / HDMA Indirect Address
;; (read, write, word, any)
;;
;; DMA:
;;  * 16 Bits
;;  * Number of bytes to transfer
;;  * Decremented during DMA.
;;  * If 0 then 0x10000 bytes will be transfered.
;;
;; HDMA:
;;  * Holding space for indirect addressing mode
;;  * These registers should not be set by the CPU
DAS0  = $4305
DAS1  = $4315
DAS2  = $4325
DAS3  = $4335
DAS4  = $4345
DAS5  = $4355
DAS6  = $4365
DAS7  = $4375


DAS0L = $4305
DAS0H = $4306
DAS1L = $4315
DAS1H = $4316
DAS2L = $4325
DAS2H = $4326
DAS3L = $4335
DAS3H = $4336
DAS4L = $4345
DAS4H = $4346
DAS5L = $4355
DAS5H = $4356
DAS6L = $4365
DAS6H = $4366
DAS7L = $4375
DAS7H = $4376



;; HDMA Indirect Address Bank
;; (read, write, any) 
;;
;; Used with HDMA only, combined with `DAS0` to form the effective address.
DAS0B = $4307
DAS1B = $4317
DAS2B = $4327
DAS3B = $4337
DAS4B = $4347
DAS5B = $4357
DAS6B = $4367
DAS7B = $4377



;; HDMA Table Address
;; (read, write, word, any)
;;
;; Used with HDMA only.
;;
;; Only modify this register if starting HDMA mid frame.
A2A0  = $4308
A2A1  = $4318
A2A2  = $4328
A2A3  = $4338
A2A4  = $4348
A2A5  = $4358
A2A6  = $4368
A2A7  = $4378

A2A0L = $4308
A2A0H = $4309
A2A1L = $4318
A2A1H = $4319
A2A2L = $4328
A2A2H = $4329
A2A3L = $4338
A2A3H = $4339
A2A4L = $4348
A2A4H = $4349
A2A5L = $4358
A2A5H = $4359
A2A6L = $4368
A2A6H = $4369
A2A7L = $4378
A2A7H = $4379



;; HDMA Line Counter
;; (read, write, any)
;;
;; rccccccc
;;
;; r       = Repeat Select (1 = HDMA every line, 0 
;; ccccccc = Line Count
;;
;; Used with HDMA only.
;;
;; Only modify this register if starting HDMA mid frame.
NLTR0 = $430A
NLTR1 = $431A
NLTR2 = $432A
NLTR3 = $433A
NLTR4 = $434A
NLTR5 = $435A
NLTR6 = $436A
NLTR7 = $437A

;;; Flags
.define NTLR_REPEAT_FLAG %10000000

;;; Masks
.define NTLR_COUNT_MASK  %01111111

.endif ; _REGISTERS_INC_

; vim: ft=asm:

