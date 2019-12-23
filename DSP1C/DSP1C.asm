architecture necdsp
output "DSP1C.rom", create

//Pad the file to the standard size
origin 0x17F0-3
cmd_nop

//P select constants
constant RAM =  %00
constant IDB =  %01
constant REGM = %10
constant REGN = %11

//ALU field constants
constant NOP  =  %0000
constant OR   =  %0001
constant AND  =  %0010
constant XOR  =  %0011
constant SUB  =  %0100
constant ADD  =  %0101
constant SBB  =  %0110
constant ADC  =  %0111
constant DEC  =  %1000
constant INC  =  %1001
constant CMP  =  %1010
constant SHR1 =  %1011
constant SHL1 =  %1100
constant SHL2 =  %1101
constant SHL4 =  %1110
constant XCHG =  %1111

//ASL Field constants
constant ACCA   =  %0
constant ACCB   =  %1

//DPL Field constants
constant DPNOP = %00
constant DPINC = %01
constant DPDEC = %10
constant DPCLR = %11

//DPH Field is just a 4 bit mask XOR

//RPDCR Field
constant RPNOP   =  %0
constant RPDEC   =  %1

//SRC Field
constant src_NON  = %0000
constant src_TRB  = %0000
constant src_A    = %0001
constant src_B    = %0010
constant src_TR   = %0011
constant src_DP   = %0100
constant src_RP   = %0101
constant src_RO   = %0110
constant src_SGN  = %0111
constant src_DR   = %1000
constant src_DRNF = %1001
constant src_SR   = %1010
constant src_SIM  = %1011
constant src_SIL  = %1100
constant src_K    = %1101
constant src_L    = %1110
constant src_MEM  = %1111

//DST Field
constant dst_NON  = %0000
constant dst_A    = %0001
constant dst_B    = %0010
constant dst_TR   = %0011
constant dst_DP   = %0100
constant dst_RP   = %0101
constant dst_DR   = %0110
constant dst_SR   = %0111
constant dst_SOL  = %1000
constant dst_SOM  = %1001
constant dst_K    = %1010
constant dst_KLR  = %1011
constant dst_KLM  = %1100
constant dst_L    = %1101
constant dst_TRB  = %1110
constant dst_MEM  = %1111

//Branch Fields
constant JMP    = %100000000
constant CALL   = %101000000
constant JNCA   = %010000000
constant JCA    = %010000010
constant JNCB   = %010000100
constant JCB    = %010000110
constant JNZA   = %010001000
constant JZA    = %010001010
constant JNZB   = %010001100
constant JZB    = %010001110
constant JNOVA0 = %010010000
constant JOVA0  = %010010010
constant JNOVB0 = %010010100
constant JOVB0  = %010010110
constant JNOVA1 = %010011000
constant JOVA1  = %010011010
constant JNOVB1 = %010011100
constant JOVB1  = %010011110
constant JNSA0  = %010100000
constant JSA0   = %010100010
constant JNSB0  = %010100100
constant JSB0   = %010100110
constant JNSA1  = %010101000
constant JSA1   = %010101010
constant JNSB1  = %010101100
constant JSB1   = %010101110
constant JDPL0  = %010110000
constant JDPLN0 = %010110001
constant JDPLF  = %010110010
constant JDPLNF = %010110011
constant JNSIAK = %010110100
constant JSIAK  = %010110110
constant JNSOAK = %010111000
constant JSOAK  = %010111010
constant JNRQM  = %010111100
constant JRQM   = %010111110

//I tried to use the table driven assembler and failed so I used macros
macro writeJP(branchCmd,address) {
    variable finalOpcode = 0
	variable realAddress
	//Bits 23 and 22 for jump
	finalOpcode = finalOpcode + (%10 << 22)
	//Jump condition bits 21 to 13
	finalOpcode = finalOpcode + ({branchCmd} << 13)
	//Add address, check for overflow
	realAddress = {address} / 3
	if realAddress > 0x1FF {
		error "Address overflow"
	}
	finalOpcode = finalOpcode + (realAddress << 2)
	op24BitConst #finalOpcode
}

//I tried to use the table driven assembler and failed so I used macros
macro writeOPRT(opcode,pSelect,ALU,ASL,DPL,DPH,RPDCR,DST,SRC) {
    variable finalOpcode = 0
	//Bits 23 and 22 for OP/RT
	finalOpcode = finalOpcode + (%00 << 22)
	//p select bits 21-20
	finalOpcode = finalOpcode + ({pSelect} << 20)
	//ALU operation bits 16-19
	finalOpcode = finalOpcode + ({ALU} << 16)
	//ASL select bit 15
	finalOpcode = finalOpcode + ({ASL} << 15)
	//DPL select bits 14-13
	finalOpcode = finalOpcode + ({DPL} << 13)
	//DPH select bits 12-9
	finalOpcode = finalOpcode + ({DPH} << 9)
	//DPH select bits 12-9
	finalOpcode = finalOpcode + ({DPH} << 9)
	//RPDCR select bit 8
	finalOpcode = finalOpcode + ({RPDCR} << 8)
	//SRC select bits 7-4
	finalOpcode = finalOpcode + ({SRC} << 4)
	//DST select bits 3-0
	finalOpcode = finalOpcode + ({DST} << 0)
	op24BitConst #finalOpcode
}

//LD takes a 16 bit value and puts it somewhere (lda+sta)
//DST field is the same as in the OP/RT instructions
macro writeLD(ld_DST,ld_Data) {
    variable finalOpcode = 0
	//Bits 23 and 22 for ld
	finalOpcode = finalOpcode + (%11 << 22)
	//16 bit data used in the ldi instruction, bits 21-6
	finalOpcode = finalOpcode + ({ld_Data} << 6)
	//ld_DST in bits 3-0
	finalOpcode = finalOpcode + ({ld_DST} << 0)
	op24BitConst #finalOpcode
}

macro writeOP(pSelect,ALU,ASL,DPL,DPH,RPDCR,DST,SRC) {
    writeOPRT(%00,{pSelect},{ALU},{ASL},{DPL},{DPH},{RPDCR},{DST},{SRC})
}

macro writeRT(pSelect,ALU,ASL,DPL,DPH,RPDCR,DST,SRC) {
    writeOPRT(%01,{pSelect},{ALU},{ASL},{DPL},{DPH},{RPDCR},{DST},{SRC})
}

//Notes
//Page 13 of datasheet has table of usefull status register flags
//RQM = 0 on external write, reset by internal write
//Page 37 for the start of the serial interface explenation


origin 0x0000

initStall:
writeJP(JRQM,initStall) //Wait for previous transfer to end
writeLD(dst_SR,0x0400) //Set serial input data to 1, 8 bit transfers
writeLD(dst_DR,0x0080) //Write 8 bit data to the data register for transfer
writeLD(dst_DR,0x0080) //Write 8 bit data to the data register for transfer
Stall2:
writeJP(JRQM,Stall2) //Wait for previous transfer to end

writeLD(dst_SR,0x0000) //Set serial input data to 0, 16 bit transfers
writeOP(RAM,NOP,ACCA,DPNOP,0x0,RPNOP,dst_A,src_DR)
writeOP(RAM,NOP,ACCA,DPNOP,0x0,RPNOP,dst_DR,src_A)
Stall:
writeOP(IDB,INC,ACCA,DPNOP,0x0,RPNOP,dst_NON,src_A)
writeOP(RAM,NOP,ACCA,DPNOP,0x0,RPNOP,dst_DR,src_A)
writeJP(JMP,Stall)
//Test commands
//writeJP(JRQM,0x1AC) //Wait for previous transfer to end
writeOP(RAM,NOP,ACCA,DPNOP,0x0,RPNOP,dst_K,src_DR)
//writeOP(RAM,INC,ACCA,DPNOP,0x0,RPNOP,dst_DR,src_A)




















