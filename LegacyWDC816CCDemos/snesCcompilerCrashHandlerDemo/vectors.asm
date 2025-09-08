; SNES ROM startup code 

;******************************************************************************
;*** Define a special section in case most of the code is not in bank 0.    ***
;******************************************************************************

STACK   EQU   $1fff     ;CHANGE THIS FOR YOUR SYSTEM

;Software, rom start offset
STARTUP SECTION OFFSET $000000,$008000

    .xref    _BEG_DATA
    .xref    _END_DATA
    .xref    _ROM_BEG_DATA
    .xref    _BEG_UDATA
    .xref    _END_UDATA
	;External C function names
    .xref    ~~NmiHandler
    .xref    ~~IrqHandler
	.xref	 ~~CrashHandler
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
		ldx #STACK;
		txs				; Stack pointer set correctly
		lda #$0000
		tcd				; Page 0 direct page
		;sep #$20;
		;LONGA	OFF;
		
		;Start of arm9 stuff
		; Clear WRAM (Stack+Low+High WRAM)
		rep #$20;
		LONGA	ON;
		ldy #$0000;		
		DMA_WRAM_START:
		;DMA 1 Byte		
		lda #$0000;
		;LoROM offset for my const Zero
		ldx #<CONST_ZERO+$8000;
		mvn #$00,#$7E
		tya
		bne DMA_WRAM_START
		
		
		; Clear Extended WRAM
		DMA_EXTENDED_WRAM_START:
		;DMA 1 Byte		
		lda #$0000;
		;LoROM offset for my const Zero
		ldx #<CONST_ZERO+$8000;
		mvn #$00,#$7F
		tya
		bne DMA_EXTENDED_WRAM_START
		
		rep     #$30
		longa	on
		longi	on
		;Next, we want to copy the initialized data section from ROM to RAM. Data must been stored in ROM at address _ROM_BEG_DATA
		lda     #_END_DATA-_BEG_DATA        ; Number of bytes to copy
		beq     SKIP                       ; If none, just skip

		dec     a                           ; Decrement size for mvn
		
		;BYTE $42, 00
		ldx     #<_ROM_BEG_DATA             ; Load source into X
		ldy     #<_BEG_DATA                 ; Load dest into Y
		;HardCoded to bank 3 for data's initial values
		mvn     #$03,#^_BEG_DATA ; Copy bytes		
		
		SKIP:
		;BYTE $42, 00
		
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
   
   BYTE	$99, $88
   BYTE	$99, $88
   BYTE	$99, $88
   BYTE	$99, $88
   
 
CRASHHOOK	SECTION OFFSET $000500

CRASH:
	;BYTE	$42, $00
	php
	LONGA	ON
	LONGI	ON
	rep     #$30
	pha
	
	;Push ASCII End
	lda #$2164
	pha
	lda #$6e45
	pha
	
	;Push the stack pointer up 0x500 bytes so that I can don't break the old stuff
	tsc
	pha
	sbc #$0500
	tcs
		
	
	jmp	>~~CrashHandler	
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
	XDEF ~~ftod
~~ftod




	
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

N_COP   DW   ($8000+CRASH)
N_BRK   DW   ($8000+CRASH)
N_ABORT DW   ($8000+CRASH)
N_NMI   DW   DIRQ
N_RSRVD DW   ($8000+CRASH)
N_IRQ   DW   IRQ
        DS   4
E_COP   DW   ($8000+CRASH)
E_RSRVD DW   ($8000+CRASH)
E_ABORT DW   ($8000+CRASH)
E_NMI   DW   DIRQ
E_RESET DW   $8000
E_IRQ   DW   DIRQ

;This is to inflate the file size to the nearest bank because snes9x hates
;Non standard roms. The actual ROM chip would have extra empty space so the binary
;Should as well.
FILLER	SECTION OFFSET $03FFFF
FILLERBYTE byte $00

END

