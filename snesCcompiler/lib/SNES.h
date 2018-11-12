//I try and adhere to the names listed in yoshies doc and those listed in
//peter lemons snes examples where possible. Bit fields are in the style of
//microchip xc header files

//$2100
extern volatile unsigned char  INIDISP;
typedef struct tagINIDISPBITS {
  unsigned BRIGHTNESS:4;
  unsigned :3;
  unsigned SCREENDISABLE:1;
} INIDISPBITS;
extern volatile INIDISPBITS INIDISPbits;


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
