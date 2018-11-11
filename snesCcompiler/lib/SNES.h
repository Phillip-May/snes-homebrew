#define PortA (*(unsigned char *)0xfff002)

extern volatile unsigned int  REG_CGDATA;
typedef struct tagREG_CGDATABITS {
  unsigned CGDATA0:1;
  unsigned CGDATA1:1;
  unsigned CGDATA2:1;
  unsigned CGDATA3:1;
  unsigned CGDATA4:1;
  unsigned CGDATA5:1;
  unsigned CGDATA6:1;
  unsigned CGDATA7:1;
} REG_CGDATABITS;
extern volatile REG_CGDATABITS REG_CGDATAbits;