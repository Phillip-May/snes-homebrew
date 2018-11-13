//I try and adhere to the names listed in yoshi's/Qwertie's doc and those listed
//in peter lemons snes examples where possible. Bit fields are in the style of
//microchip xc header files with names from the english snes programming manuals

//PPU Picture Processing Unit Ports (Write-Only)
//$2100
extern volatile unsigned char INIDISP;
typedef struct tagINIDISPBITS {
  unsigned BRIGHTNESS:4;
  unsigned :3;
  unsigned SCREENDISABLE:1;
} INIDISPBITS;
extern volatile INIDISPBITS INIDISPbits;

//$2101
extern volatile unsigned char OBSEL;
typedef struct tagOBSELBITS {
  unsigned BASESEL:3;
  unsigned NAMESEL:2;
  unsigned OBJSIZE:3;
} OBSELBITS;
extern volatile OBSELBITS OBSELbits;

//$2102
extern volatile unsigned char OAMADDL;
typedef struct tagOAMADDLBITS {
  unsigned ADDRESSL:8;
} OAMADDLBITS;
extern volatile OAMADDLBITS OAMADDLbits;

//$2103
extern volatile unsigned char OAMADDH;
typedef struct tagOAMADDHBITS {
  unsigned ADDRESSMSB:1;
  unsigned :6;
  unsigned OAMROTATE:1;
} OAMADDHBITS;
extern volatile OAMADDHBITS OAMADDHbits;

//$2104
extern volatile unsigned int OAMDATA;
typedef struct tagOAMDATABITS {
  unsigned OAMDATA:8;
} OAMDATABITS;
extern volatile OAMDATABITS OAMDATAbits;

//$2105
extern volatile unsigned char BGMODE;
typedef struct tagBGMODEBITS {
  unsigned MODE:3;
  unsigned BG3PRIORITY:1;
  unsigned BG1SIZE:1;
  unsigned BG2SIZE:1;
  unsigned BG3SIZE:1;
  unsigned BG4SIZE:1;
} BGMODEBITS;
extern volatile BGMODEBITS BGMODEbits;

//$2106
extern volatile unsigned char MOSAIC;
typedef struct tagMOSAICBITS {
  unsigned MOSAICENBG1:1;
  unsigned MOSAICENBG2:1;
  unsigned MOSAICENBG3:1;
  unsigned MOSAICENBG4:1;
  unsigned MOSAICSIZE:4;
} MOSAICBITS;
extern volatile MOSAICBITS MOSAICbits;

//$2107
extern volatile unsigned char BG1SC;
typedef struct tagBG1SCBITS {
  unsigned SC:2;
  unsigned TILEMAPADD:6;
} BG1SCBITS;
extern volatile BG1SCBITS BG1SCbits;

//$2108
extern volatile unsigned char BG2SC;
typedef struct tagBG2SCBITS {
  unsigned SC:2;
  unsigned TILEMAPADD:6;
} BG2SCBITS;
extern volatile BG2SCBITS BG2SCbits;

//$2109
extern volatile unsigned char BG3SC;
typedef struct tagBG3SCBITS {
  unsigned SC:2;
  unsigned TILEMAPADD:6;
} BG3SCBITS;
extern volatile BG3SCBITS BG3SCbits;

//$210A
extern volatile unsigned char BG4SC;
typedef struct tagBG4SCBITS {
  unsigned SC:2;
  unsigned TILEMAPADD:6;
} BG4SCBITS;
extern volatile BG4SCBITS BG4SCbits;

//$2121
extern volatile unsigned char  CGADD;
typedef struct tagCGADDBITS {
  unsigned CGADD0:1;
  unsigned CGADD1:1;
  unsigned CGADD2:1;
  unsigned CGADD3:1;
  unsigned CGADD4:1;
  unsigned CGADD5:1;
  unsigned CGADD6:1;
  unsigned CGADD7:1;
} CGADDBITS;
extern volatile CGADDBITS CGADDbits;


//$2122
extern volatile unsigned char  CGDATA;
typedef struct tagCGDATABITS {
  unsigned CGDATA0:1;
  unsigned CGDATA1:1;
  unsigned CGDATA2:1;
  unsigned CGDATA3:1;
  unsigned CGDATA4:1;
  unsigned CGDATA5:1;
  unsigned CGDATA6:1;
  unsigned CGDATA7:1;
} CGDATABITS;
extern volatile CGDATABITS CGDATAbits;
