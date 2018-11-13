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

//$210B
extern volatile unsigned char BG12NBA;
typedef struct tagBG12NBABITS {
  unsigned BASEADDBG1:4;
  unsigned BASEADDBG2:4;
} BG12NBABITS;
extern volatile BG12NBABITS BG12NBAbits;

//$210C
extern volatile unsigned char BG34NBA;
typedef struct tagBG34NBABITS {
  unsigned BASEADDBG3:4;
  unsigned BASEADDBG4:4;
} BG34NBABITS;
extern volatile BG34NBABITS BG34NBAbits;

//$210D
extern volatile unsigned char BG1HOFS;
typedef struct tagBG1HOFSBITS {
  unsigned OFFSET:8;
} BG1HOFSBITS;
extern volatile BG1HOFSBITS BG1HOFSbits;

//$210E
extern volatile unsigned char BG1VOFS;
typedef struct tagBG1VOFSBITS {
  unsigned OFFSET:8;
} BG1VOFSBITS;
extern volatile BG1VOFSBITS BG1VOFSbits;

//$210F
extern volatile unsigned char BG2HOFS;
typedef struct tagBG2HOFSBITS {
  unsigned OFFSET:8;
} BG2HOFSBITS;
extern volatile BG2HOFSBITS BG2HOFSbits;

//$2110
extern volatile unsigned char BG2VOFS;
typedef struct tagBG2VOFSBITS {
  unsigned OFFSET:8;
} BG2VOFSBITS;
extern volatile BG2VOFSBITS BG2VOFSbits;

//$2111
extern volatile unsigned char BG3HOFS;
typedef struct tagBG3HOFSBITS {
  unsigned OFFSET:8;
} BG3HOFSBITS;
extern volatile BG3HOFSBITS BG3HOFSbits;

//$2112
extern volatile unsigned char BG3VOFS;
typedef struct tagBG3VOFSBITS {
  unsigned OFFSET:8;
} BG3VOFSBITS;
extern volatile BG3VOFSBITS BG3VOFSbits;

//$2113
extern volatile unsigned char BG4HOFS;
typedef struct tagBG4HOFSBITS {
  unsigned OFFSET:8;
} BG4HOFSBITS;
extern volatile BG4HOFSBITS BG4HOFSbits;

//$2114
extern volatile unsigned char BG4VOFS;
typedef struct tagBG4VOFSBITS {
  unsigned OFFSET:8;
} BG4VOFSBITS;
extern volatile BG4VOFSBITS BG4VOFSbits;

//$2115
extern volatile unsigned char VMAIN;
typedef struct tagVMAINBITS {
  unsigned SCINC:2;
  unsigned VRAMADDINC:2;
  unsigned :3;
  unsigned HLADDINC:1;
} VMAINBITS;
extern volatile VMAINBITS VMAINbits;

//2116-2117
extern volatile unsigned int VMADD;
typedef struct tagVMADDBITS {
  unsigned ADD:16;
} VMADDBITS;
extern volatile VMADDBITS VMADDbits;

//2118-2119
extern volatile unsigned int VMDATA;
typedef struct tagVMDATABITS {
  unsigned DATA:16;
} VMDATABITS;
extern volatile VMDATABITS VMDATAbits;

//211A
extern volatile unsigned char M7SEL;
typedef struct tagM7SELBITS {
  unsigned XFLIP:1;
  unsigned YFLIP:1;
  unsigned :4;
  unsigned OUTSCREEN:2;
} M7SELBITS;
extern volatile M7SELBITS M7SELbits;

//211B
extern volatile unsigned char M7A;
typedef struct tagM7ABITS {
  unsigned DATA:8;
} M7ABITS;
extern volatile M7ABITS M7Abits;

//211C
extern volatile unsigned char M7B;
typedef struct tagM7BBITS {
  unsigned DATA:8;
} M7BBITS;
extern volatile M7BBITS M7Bbits;

//211D
extern volatile unsigned char M7C;
typedef struct tagM7CBITS {
  unsigned DATA:8;
} M7CBITS;
extern volatile M7CBITS M7Cbits;

//211E
extern volatile unsigned char M7D;
typedef struct tagM7DBITS {
  unsigned DATA:8;
} M7DBITS;
extern volatile M7DBITS M7Dbits;

//211F
extern volatile unsigned char M7X;
typedef struct tagM7XBITS {
  unsigned DATA:8;
} M7XBITS;
extern volatile M7XBITS M7Xbits;

//2120
extern volatile unsigned char M7Y;
typedef struct tagM7YBITS {
  unsigned DATA:8;
} M7YBITS;
extern volatile M7YBITS M7Ybits;

//$2121
extern volatile unsigned char CGADD;
typedef struct tagCGADDBITS {
  unsigned DATA:8;
} CGADDBITS;
extern volatile CGADDBITS CGADDbits;

//$2122
extern volatile unsigned char CGDATA;
typedef struct tagCGDATABITS {
  unsigned DATA:8;
} CGDATABITS;
extern volatile CGDATABITS CGDATAbits;

//$2123-2124
extern volatile unsigned int W1234SEL;
typedef struct tagW1234SELBITS {
  unsigned BG1:4;
  unsigned BG2:4;
  unsigned BG3:4;
  unsigned BG4:4;
} W1234SELBITS;
extern volatile W1234SELBITS W1234SELbits;

//$2125
extern volatile unsigned char WOBJSEL;
typedef struct tagWOBJSELBITS {
  unsigned OBJWIN:4;
  unsigned COLWIN:4;
} WOBJSELBITS;
extern volatile WOBJSELBITS WOBJSELbits;

//$2126
extern volatile unsigned char WH0;
typedef struct tagWH0BITS {
  unsigned POSITION:8;
} WH0BITS;
extern volatile WH0BITS WH0bits;

//$2127
extern volatile unsigned char WH1;
typedef struct tagWH1BITS {
  unsigned POSITION:8;
} WH1BITS;
extern volatile WH1BITS WH1bits;

//$2128
extern volatile unsigned char WH2;
typedef struct tagWH2BITS {
  unsigned POSITION:8;
} WH2BITS;
extern volatile WH2BITS WH2bits;

//$2129
extern volatile unsigned char WH3;
typedef struct tagWH3BITS {
  unsigned POSITION:8;
} WH3BITS;
extern volatile WH3BITS WH3bits;

//$212A
extern volatile unsigned char WBGLOG;
typedef struct tagWBGLOGBITS {
  unsigned BG1:2;
  unsigned BG2:2;
  unsigned BG3:2;
  unsigned BG4:2;
} WBGLOGBITS;
extern volatile WBGLOGBITS WBGLOGbits;

//$212B
extern volatile unsigned char WOBJLOG;
typedef struct tagWOBJLOGBITS {
  unsigned CLRPARAM:2;
  unsigned OBJPARAM:2;
} WOBJLOGBITS;
extern volatile WOBJLOGBITS WOBJLOGbits;

//$212C
extern volatile unsigned char TM;
typedef struct tagTMBITS {
  unsigned BG1EN:1;
  unsigned BG2EN:1;
  unsigned BG3EN:1;
  unsigned BG4EN:1;
  unsigned OBJOAMEN:1;
  unsigned :3;
} TMBITS;
extern volatile TMBITS TMbits;

//$212D
extern volatile unsigned char TD;
typedef struct tagTDBITS {
  unsigned BG1EN:1;
  unsigned BG2EN:1;
  unsigned BG3EN:1;
  unsigned BG4EN:1;
  unsigned OBJOAMEN:1;
  unsigned :3;
} TDBITS;
extern volatile TDBITS TDbits;

//$212E
extern volatile unsigned char TMW;
typedef struct tagTMWBITS {
  unsigned BG1EN:1;
  unsigned BG2EN:1;
  unsigned BG3EN:1;
  unsigned BG4EN:1;
  unsigned OBJOAMEN:1;
  unsigned :3;
} TMWBITS;
extern volatile TMWBITS TMWbits;

//$212F
extern volatile unsigned char TSW;
typedef struct tagTSWBITS {
  unsigned BG1EN:1;
  unsigned BG2EN:1;
  unsigned BG3EN:1;
  unsigned BG4EN:1;
  unsigned OBJOAMEN:1;
  unsigned :3;
} TSWBITS;
extern volatile TSWBITS TSWbits;

//$2130
extern volatile unsigned char CGWSEL;
typedef struct tagCGWSELBITS {
  unsigned DIRCOL:1;
  unsigned EN:1;
  unsigned :2;
  unsigned SUB:2;
  unsigned MAIN:2;

} CGWSELBITS;
extern volatile CGWSELBITS CGWSELbits;

//$2131
extern volatile unsigned char CGADSUB;
typedef struct tagCGADSUBBITS {
  unsigned BG1:1;
  unsigned BG2:1;
  unsigned BG3:1;
  unsigned BG4:1;
  unsigned OBJ:1;
  unsigned BACK:1;
  unsigned HALF:1;
  unsigned SUBEN:1;
} CGADSUBBITS;
extern volatile CGADSUBBITS CGADSUBbits;

//$2132
extern volatile unsigned char COLDATA;
typedef struct tagCOLDATABITS {
  unsigned CONSTCOL:5;
  unsigned RED:1;
  unsigned GREEN:1;
  unsigned BLUE:1;
} COLDATABITS;
extern volatile COLDATABITS COLDATAbits;

//$2133
extern volatile unsigned char SETINI;
typedef struct tagSETINIBITS {
  unsigned IEN:1;
  unsigned ONJVSEL:1;
  unsigned H512E:1;
  unsigned :2;
  unsigned EXTBGEN:1;
  unsigned EXTSYN:1;

} SETINIBITS;
extern volatile SETINIBITS SETINIbits;

//PPU Picture Processing Unit Ports (Read-Only)



