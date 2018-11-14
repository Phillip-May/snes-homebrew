; SNES ROM startup code 

;******************************************************************************
;*** Define a special section in case most of the code is not in bank 0.    ***
;******************************************************************************

STACK   EQU   $01ff     ;CHANGE THIS FOR YOUR SYSTEM

;Software, rom start offset
STARTUP SECTION OFFSET $000000,$008000

    .xref    _BEG_DATA
    .xref    _END_DATA
    .xref    _ROM_BEG_DATA
    .xref    _BEG_UDATA
    .xref    _END_UDATA
    .xref    ~~NmiHandler
    .xref    ~~IrqHandler
    .xref    ~~main

	XDEF  	START
START:
        XREF  	~~main
		XREF	~~sbrk
  	
		sei             ; Disabled interrupts
        clc             ; clear carry to switch to native mode
        xce             ; Xchange carry & emulation bit. native mode
		phk
		plb
        rep     #$38    ; Binary mode (decimal mode off), X/Y 16 bit
		LONGI	ON;
		LONGA	ON;
		ldx #$1fff;
		txs
		lda #$0000
		tcd
		sep #$20;
		LONGA	OFF;
		lda #$00;
		sta $420D
		lda #$8F
		sta $2100
		stz $2101
		stz $2102
		stz $2103
		stz $2105
		stz $2106
		stz $2107
		stz $2108
		stz $2109
		stz $210A
		stz $210B
		stz $210C
		stz $210D
		stz $210D
		stz $210E
		stz $210E
		stz $210F
		stz $210F
		stz $2110
		stz $2110
		stz $2111
		stz $2111
		stz $2112
		stz $2112
		stz $2113
		stz $2113
		stz $2114
		stz $2114
		lda #$01
		stz $211B
		stz $211B
		stz $211C
		stz $211C
		stz $211D
		stz $211D
		stz $211E
		stz $211E
		stz $211F
		stz $211F
		stz $2120
		stz $2120
		stz $2123
		stz $2124
		stz $2125
		stz $2126
		stz $2127
		stz $2128
		stz $2129
		stz $212A
		stz $212B
		stz $212C
		stz $212D
		stz $212E
		stz $212F
		lda #$30
		sta $2130
		stz $2131
		lda #$E0
		sta $2132
		stz $2133
		stz $4016
		stz $4200
		lda #$FF
		sta $4201
		stz $4202
		stz $4203
		stz $4204
		stz $4205
		stz $4206
		stz $4207
		stz $4208
		stz $4209
		stz $420A
		stz $420B
		stz $420C
		ldx #$0080
		lda #$E0
		INITLOOP1:
		sta $2104
		sta $2104
		stz $2104
		stz $2104
		dex
		bne INITLOOP1
		ldx #$0020
		INITLOOP2:
		stz $2104
		dex
		bne INITLOOP2
		ldy #$0000
		sty $2181
		stz $2183
		ldx #$8008
		stx $4300
		ldx #$CONST_ZERO; This should not be hardcoded but I cant get it to work othwerise
		lda #$00; This should be CONST_ZERO
		stx $4302
		sta $4304
		sty $4305
		lda #$01
		sta $420B
		nop
		sta $420B
		lda #$80
		sta $2115
		ldy #$0000
		sty $2116
		sty $4305
		ldx #$1809
		stx $4300
		lda #$01
		sta $420B
		stz $2121
		ldx #$0200
		stx $4305
		ldx #$2208
		stx $4300
		sta $420B
		;Start of arm9 stuff
		; Clear WRAM
		ldy     #$0000
		sty     $2181           ; Transfer to $7E:0000
		stz     $2183           ; Select first WRAM bank ($7E)

		ldx     #$8008          ; Fixed source byte write to $2180
		stx     $4300
		ldx     #CONST_ZERO
		lda     #^CONST_ZERO

		stx     $4302           ; DMA destination address
		sta     $4304           ; Destination bank
		sty     $4305           ; Transfer 64KiB

		lda     #$01
		sta     $420B
		nop
		sta     $420B           ; $2181-$2183 and $4305 wrap appropriately

		ldx     #$0000
		ldy     #$0000          ; 64KiB		

		
		rep     #$30
		longa	on
		longi	on
		;Next, we want to copy the initialized data section from ROM to RAM. Data must been stored in ROM at address _ROM_BEG_DATA
		lda     #_END_DATA-_BEG_DATA        ; Number of bytes to copy
		beq     ?skip                       ; If none, just skip

		BYTE $42, $00
		dec     a                           ; Decrement size for mvn

		ldx     #<_ROM_BEG_DATA             ; Load source into X
		ldy     #<_BEG_DATA                 ; Load dest into Y
		;HardCoded to bank 3 for data
		mvn     #$03,#^_BEG_DATA ; Copy bytes		
		?skip:
	    ldx     #_END_UDATA-_BEG_UDATA  ; Get number of bytes to clear
	    beq     ?DONE                   ; Nothing to do
	 
	    lda     #0                      ; Get a zero for storing
	 
	    .longa  off                     ; Set 8 bit accumulator
	   sep     #$20
	 
	    ldy     #_BEG_UDATA             ; Get beginning of zeros
	
		?LOOP:
	    sta     |0,Y                    ; Clear memory
	    iny                             ; Bump pointer
	    dex                             ; Decrement count
	    bne     ?LOOP                   ; Continue till done

		?DONE:		
		
		
		.longi  on
		.longa  on
		rep     #$30

		lda     #$0000          ; Set direct page to $0000 for no apparent reason
		tcd
	
        jsl   	>~~main
        brk

		XDEF  	IRQ
		XREF	~~IRQHandler
IRQ:	
	LONGA	ON
	LONGI	ON
	rep	#$30
	pha
	phx
	phy
	jsl	~~IRQHandler
	ply
	plx
	pla
	rti

DIRQ:
	rti
	
	XREF CONST_ZERO
CONST_ZERO:
    BYTE $00, $00
;Dummy stuff for standard library to compile at all
;This may need to be improved later
;Basically if stuff breaks blame this
	XDEF ~~unlink
~~unlink:
	XDEF ~~close
~~close:
	XDEF ~~open
~~open:
	XDEF ~~isatty
~~isatty:
	XDEF ~~write
~~write:
	XDEF ~~lseek
~~lseek:



	
;******************************************************************************
;*** SNES ROM Registartion Data                                             ***
;******************************************************************************

REGISTRATION_DATA SECTION OFFSET $007FB0

MAKER_CODE		FCC	/FF/
GAME_CODE		FCC	/SMWJ/
FIXED_VALUE0		BYTE	$00, $00, $00, $00, $00, $00, $00
EXPANSION_RAM_SIZE	BYTE	$00
SPECIAL_VERSION		BYTE	$00
CARTRIDGE_TYPE_SUB	BYTE	$00
GAME_TITLE		FCC	/GAME TITLE          /
				;01234567890123456789;
MAP_MODE			BYTE	$20
CARTRIDGE_SIZE		BYTE	$00
ROM_SIZE			BYTE	$01
RAM_SIZE			BYTE	$00
VIDEO_REFRESH_RATE	BYTE	$00
FIXED_VALUE1		BYTE	$33
MASK_ROM_VERSION	BYTE	$00
COMPLEMENT_CHECK	BYTE	$00, $00
CHEKSUM				BYTE	$00, $00 

;******************************************************************************
;*** SNES Interrupts and Reset vector                                       ***
;******************************************************************************

VECTORS	SECTION OFFSET $007FE4

N_COP   DW   DIRQ
N_BRK   DW   DIRQ
N_ABORT DW   DIRQ        
N_NMI   DW   DIRQ
N_RSRVD DW   DIRQ
N_IRQ   DW   IRQ
        DS   4
E_COP   DW   DIRQ
E_RSRVD DW   DIRQ
E_ABORT DW   DIRQ
E_NMI   DW   DIRQ
E_RESET DW   $8000
E_IRQ   DW   DIRQ

;This is to inflate the file size to the nearest bank because snes9x hates
;Non standard roms. The actual ROM chip would have extra empty space so the binary
;Should as well.
FILLER	SECTION OFFSET $01FFFF
FILLERBYTE byte $00

END

