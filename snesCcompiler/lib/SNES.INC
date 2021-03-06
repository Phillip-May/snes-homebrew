	XDEF ~~INIDISP
	XDEF ~~INIDISPbits
~~INIDISP EQU #$002100;
~~INIDISPbits EQU #$002100;

	XDEF ~~OBSEL
	XDEF ~~OBSELbits
~~OBSEL EQU #$002101;
~~OBSELbits EQU #$002101;

	XDEF ~~OAMADDL
	XDEF ~~OAMADDLbits
~~OAMADDL EQU #$002102;
~~OAMADDLbits EQU #$002102;

	XDEF ~~OAMADDH
	XDEF ~~OAMADDHbits
~~OAMADDH EQU #$002103;
~~OAMADDHbits EQU #$002103;

	XDEF ~~OAMDATA
	XDEF ~~OAMDATAbits
~~OAMDATA EQU #$002104;
~~OAMDATAbits EQU #$002104;

	XDEF ~~BGMODE
	XDEF ~~BGMODEbits
~~BGMODE EQU #$002105;
~~BGMODEbits EQU #$002105;

	XDEF ~~MOSAIC
	XDEF ~~MOSAICbits
~~MOSAIC EQU #$002106;
~~MOSAICbits EQU #$002106;

	XDEF ~~BG1SC
	XDEF ~~BG1SCbits
~~BG1SC EQU #$002107;
~~BG1SCbits EQU #$002107;

	XDEF ~~BG2SC
	XDEF ~~BG2SCbits
~~BG2SC EQU #$002108;
~~BG2SCbits EQU #$002108;

	XDEF ~~BG3SC
	XDEF ~~BG3SCbits
~~BG3SC EQU #$002109;
~~BG3SCbits EQU #$002109;

	XDEF ~~BG4SC
	XDEF ~~BG4SCbits
~~BG4SC EQU #$00210A;
~~BG4SCbits EQU #$00210A;

	XDEF ~~BG12NBA
	XDEF ~~BG12NBAbits
~~BG12NBA EQU #$00210B;
~~BG12NBAbits EQU #$00210B;

	XDEF ~~BG34NBA
	XDEF ~~BG34NBAbits
~~BG34NBA EQU #$00210C;
~~BG34NBAbits EQU #$00210C;

	XDEF ~~BG1HOFS
	XDEF ~~BG1HOFSbits
~~BG1HOFS EQU #$00210D;
~~BG1HOFSbits EQU #$00210D;

	XDEF ~~BG1VOFS
	XDEF ~~BG1VOFSbits
~~BG1VOFS EQU #$00210E;
~~BG1VOFSbits EQU #$00210E;

	XDEF ~~BG2HOFS
	XDEF ~~BG2HOFSbits
~~BG2HOFS EQU #$00210D;
~~BG2HOFSbits EQU #$00210F;

	XDEF ~~BG2VOFS
	XDEF ~~BG2VOFSbits
~~BG2VOFS EQU #$00210E;
~~BG2VOFSbits EQU #$002110;

	XDEF ~~BG3HOFS
	XDEF ~~BG3HOFSbits
~~BG3HOFS EQU #$002111;
~~BG3HOFSbits EQU #$002111;

	XDEF ~~BG3VOFS
	XDEF ~~BG3VOFSbits
~~BG3VOFS EQU #$002112;
~~BG3VOFSbits EQU #$002112;

	XDEF ~~BG4HOFS
	XDEF ~~BG4HOFSbits
~~BG4HOFS EQU #$002113;
~~BG4HOFSbits EQU #$002113;

	XDEF ~~BG4VOFS
	XDEF ~~BG4VOFSbits
~~BG4VOFS EQU #$002114;
~~BG4VOFSbits EQU #$002114;

	XDEF ~~VMAIN
	XDEF ~~VMAINbits
~~VMAIN EQU #$002115;
~~VMAINbits EQU #$002115;

	XDEF ~~VMADD
	XDEF ~~VMADDbits
~~VMADD EQU #$002116;
~~VMADDbits EQU #$002116;

	XDEF ~~VMDATA
	XDEF ~~VMDATAbits
~~VMDATA EQU #$002118;
~~VMDATAbits EQU #$002118;

	XDEF ~~M7SEL
	XDEF ~~M7SELbits
~~M7SEL EQU #$00211A;
~~M7SELbits EQU #$00211A;

	XDEF ~~M7A
	XDEF ~~M7Abits
~~M7A EQU #$00211B;
~~M7Abits EQU #$00211B;

	XDEF ~~M7B
	XDEF ~~M7Bbits
~~M7B EQU #$00211C;
~~M7Bbits EQU #$00211C;

	XDEF ~~M7C
	XDEF ~~M7Cbits
~~M7C EQU #$00211D;
~~M7Cbits EQU #$00211D;

	XDEF ~~M7D
	XDEF ~~M7Dbits
~~M7D EQU #$00211D;
~~M7Dbits EQU #$00211E;

	XDEF ~~M7X
	XDEF ~~M7Xbits
~~M7X EQU #$00211D;
~~M7Xbits EQU #$00211F;

	XDEF ~~M7Y
	XDEF ~~M7Ybits
~~M7Y EQU #$00211D;
~~M7Ybits EQU #$002120;

	XDEF ~~CGADD
	XDEF ~~CGADDbits
~~CGADD EQU #$002121;
~~CGADDbits EQU #$002121;

	XDEF ~~CGDATA
	XDEF ~~CGDATAbits
~~CGDATA EQU #$002122;
~~CGDATAbits EQU #$002122;

	XDEF ~~W1234SEL
	XDEF ~~W1234SELbits
~~W1234SEL EQU #$002123;
~~W1234SELbits EQU #$002123;

	XDEF ~~WOBJSEL
	XDEF ~~WOBJSELbits
~~WOBJSEL EQU #$002125;
~~WOBJSELbits EQU #$002125;

	XDEF ~~WH0
	XDEF ~~WH0bits
~~WH0 EQU #$002126;
~~WH0bits EQU #$002126;

	XDEF ~~WH1
	XDEF ~~WH1bits
~~WH1 EQU #$002127;
~~WH1bits EQU #$002127;

	XDEF ~~WH2
	XDEF ~~WH2bits
~~WH2 EQU #$002128;
~~WH2bits EQU #$002128;

	XDEF ~~WH3
	XDEF ~~WH3bits
~~WH3 EQU #$002129;
~~WH3bits EQU #$002129;

	XDEF ~~WBGLOG
	XDEF ~~WBGLOGbits
~~WBGLOG EQU #$00212A;
~~WBGLOGbits EQU #$00212A;

	XDEF ~~WOBJLOG
	XDEF ~~WOBJLOGbits
~~WOBJLOG EQU #$00212B;
~~WOBJLOGbits EQU #$00212B;

	XDEF ~~TM
	XDEF ~~TMbits
~~TM EQU #$00212C;
~~TMbits EQU #$00212C;

	XDEF ~~TD
	XDEF ~~TDbits
~~TD EQU #$00212D;
~~TDbits EQU #$00212D;

	XDEF ~~TMW
	XDEF ~~TMWbits
~~TMW EQU #$00212E;
~~TMWbits EQU #$00212E;

	XDEF ~~TSW
	XDEF ~~TSWbits
~~TSW EQU #$00212F;
~~TSWbits EQU #$00212F;

	XDEF ~~CGWSEL
	XDEF ~~CGWSELbits
~~CGWSEL EQU #$002130;
~~CGWSELbits EQU #$002130;

	XDEF ~~CGADSUB
	XDEF ~~CGADSUBbits
~~CGADSUB EQU #$002131;
~~CGADSUBbits EQU #$002131;

	XDEF ~~COLDATA
	XDEF ~~COLDATAbits
~~COLDATA EQU #$002132;
~~COLDATAbits EQU #$002132;

	XDEF ~~SETINI
	XDEF ~~SETINIbits
~~SETINI EQU #$002133;
~~SETINIbits EQU #$002133;














