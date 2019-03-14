;:ts=8
R0	equ	1
R1	equ	5
R2	equ	9
R3	equ	13
BANK0	section
	ends
	BANK0
	xdef	~~testString
~~testString:
	db	$49,$6E,$69,$74,$69,$61,$6C,$20,$73,$74
	db	$72,$69,$6E,$67,$20,$69,$6E,$20,$52,$4F
	db	$4D,$0
	ds	18
	ends
	BANK0
	xdef	~~testString2
~~testString2:
	db	$A,$49,$6E,$69,$74,$69,$61,$6C,$20,$73
	db	$74,$72,$69,$6E,$67,$20,$69,$6E,$20,$52
	db	$4F,$4D,$20,$32,$0
	ds	15
	ends
	data
	xdef	~~testStringRam
~~testStringRam:
	db	$45,$72,$72,$6F,$72,$0
	ds	34
	ends
	BANK0
	xdef	~~main
	func
~~main:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L2
	tcs
	phd
	tcd
	data
L4:
	db	$20,$0
	ends
	data
L5:
	dw	$5
	ends
lastInputLo_1	set	0
lastInputHi_1	set	1
spriteX_1	set	2
spriteY_1	set	4
BGTileLocation_1	set	5
BGTile_1	set	7
BGTileValue_1	set	9
i_1	set	11
regRead1_1	set	13
regRead2_1	set	14
test_heap2_1	set	15
counter_1	set	19
pOAMCopy_1	set	23
stringa1_1	set	27
multiply_1	set	31
multiply2_1	set	33
multiply3_1	set	35
	stz	<L3+spriteX_1
	sep	#$20
	longa	off
	stz	<L3+spriteY_1
	rep	#$20
	longa	on
	lda	#$f800
	sta	<L3+BGTileLocation_1
	pea	#^L4
	pea	#<L4
	clc
	tdc
	adc	#<L3+BGTile_1
	sta	<R0
	lda	#$0
	sta	<R0+2
	pei	<R0+2
	pei	<R0
	lda	#$2
	xref	~~~fmov
	jsl	~~~fmov
	lda	#$20
	sta	<L3+BGTileValue_1
	stz	<L3+i_1
	lda	#$9c40
	sta	<L3+counter_1
	lda	#$0
	sta	<L3+counter_1+2
	pea	#<$0
	jsl	~~initSNES
	asmstart
	BYTE $42, $00
	asmend
	lda	#$6
	sta	<L3+multiply_1
	lda	#$6
	sta	<L3+multiply2_1
	lda	#$a
	sta	<L3+multiply3_1
	inc	<L3+multiply_1
	lda	<L3+multiply2_1
	ldx	<L3+multiply_1
	xref	~~~hwmul
	jsl	~~~hwmul
	sta	<L3+multiply3_1
	inc	<L3+multiply_1
	pea	#<$7
	lda	>~~school_bin_size
	pha
	pea	#<$2000
	pea	#^~~school_bin
	pea	#<~~school_bin
	jsl	~~LoadVram
	lda	|L5
	pha
	pea	#<$20
	pea	#<$0
	pea	#^~~school_pal
	pea	#<~~school_pal
	jsl	~~LoadCGRam
	pea	#<$0
	pea	#<$20
	pea	#<$80
	pea	#^~~biker_clr
	pea	#<~~biker_clr
	jsl	~~LoadCGRam
	pea	#<$7
	pea	#<$800
	pea	#<$0
	pea	#^~~biker_pic
	pea	#<~~biker_pic
	jsl	~~LoadVram
	pea	#<$7
	pea	#<$740
	pei	<L3+BGTileLocation_1
	pea	#^~~school_tilemap
	pea	#<~~school_tilemap
	jsl	~~LoadVram
	pea	#^$220
	pea	#<$220
	pea	#^$1
	pea	#<$1
	jsl	~~farcalloc
	sta	<L3+pOAMCopy_1
	stx	<L3+pOAMCopy_1+2
	pei	<L3+pOAMCopy_1+2
	pei	<L3+pOAMCopy_1
	jsl	~~initOAMCopy
	sep	#$20
	longa	off
	lda	#$70
	sta	[<L3+pOAMCopy_1]
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$60
	ldy	#$1
	sta	[<L3+pOAMCopy_1],Y
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$0
	ldy	#$2
	sta	[<L3+pOAMCopy_1],Y
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$70
	ldy	#$3
	sta	[<L3+pOAMCopy_1],Y
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$54
	ldy	#$200
	sta	[<L3+pOAMCopy_1],Y
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$a0
	sta	>8449	; volatile
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$11
	sta	>8492	; volatile
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$9
	sta	>8453	; volatile
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$fc
	sta	>8455	; volatile
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$1
	sta	>8459	; volatile
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$0
	sta	>8461	; volatile
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$0
	sta	>8461	; volatile
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$0
	sta	>8462	; volatile
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$0
	sta	>8462	; volatile
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	#$1
	sta	>16896	; volatile
	rep	#$20
	longa	on
	lda	#$0
	sta	>16406	; volatile
	sep	#$20
	longa	off
	lda	#$f
	sta	>8448	; volatile
	rep	#$20
	longa	on
L10001:
L10005:
	sep	#$20
	longa	off
	lda	>16912	; volatile
	sta	<L3+regRead1_1
	rep	#$20
	longa	on
L10003:
	sep	#$20
	longa	off
	sec
	lda	#$0
	sbc	<L3+regRead1_1
	rep	#$20
	longa	on
	bvs	L6
	sep	#$20
	longa	off
	eor	#$80
	rep	#$20
	longa	on
L6:
	bmi	L7
	brl	L10005
L7:
L10004:
L10008:
	sep	#$20
	longa	off
	lda	>16914	; volatile
	sta	<L3+regRead1_1
	rep	#$20
	longa	on
L10006:
	sep	#$20
	longa	off
	lda	<L3+regRead1_1
	and	#<$1
	rep	#$20
	longa	on
	beq	L8
	brl	L10008
L8:
L10007:
	sep	#$20
	longa	off
	lda	>16920	; volatile
	sta	<L3+lastInputLo_1
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	>16921	; volatile
	sta	<L3+lastInputHi_1
	rep	#$20
	longa	on
	pea	#<$0
	pea	#<$220
	pea	#<$0
	pei	<L3+pOAMCopy_1+2
	pei	<L3+pOAMCopy_1
	jsl	~~LoadOAMCopy
	sep	#$20
	longa	off
	dec	<L3+spriteY_1
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	<L3+lastInputHi_1
	and	#<$8
	rep	#$20
	longa	on
	bne	L9
	brl	L10009
L9:
	sep	#$20
	longa	off
	dec	<L3+spriteY_1
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	dec	<L3+spriteY_1
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	dec	<L3+spriteY_1
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	dec	<L3+spriteY_1
	rep	#$20
	longa	on
	brl	L10010
L10009:
	sep	#$20
	longa	off
	lda	<L3+lastInputHi_1
	and	#<$4
	rep	#$20
	longa	on
	bne	L10
	brl	L10011
L10:
	sep	#$20
	longa	off
	inc	<L3+spriteY_1
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	inc	<L3+spriteY_1
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	inc	<L3+spriteY_1
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	inc	<L3+spriteY_1
	rep	#$20
	longa	on
L10011:
L10010:
	sep	#$20
	longa	off
	lda	<L3+lastInputHi_1
	and	#<$2
	rep	#$20
	longa	on
	bne	L11
	brl	L10012
L11:
	dec	<L3+spriteX_1
	dec	<L3+spriteX_1
	dec	<L3+spriteX_1
	dec	<L3+spriteX_1
	brl	L10013
L10012:
	sep	#$20
	longa	off
	lda	<L3+lastInputHi_1
	and	#<$1
	rep	#$20
	longa	on
	bne	L12
	brl	L10014
L12:
	asmstart
	BYTE $00, $00
	asmend
	inc	<L3+spriteX_1
	inc	<L3+spriteX_1
	inc	<L3+spriteX_1
	inc	<L3+spriteX_1
L10014:
L10013:
	sep	#$20
	longa	off
	lda	<L3+spriteX_1
	sta	[<L3+pOAMCopy_1]
	rep	#$20
	longa	on
	lda	<L3+spriteX_1
	and	#<$100
	sta	<R0
	lda	<R0
	xba
	and	#$00ff
	sep	#$20
	longa	off
	ldy	#$200
	sta	[<L3+pOAMCopy_1],Y
	rep	#$20
	longa	on
	sep	#$20
	longa	off
	lda	<L3+spriteY_1
	ldy	#$1
	sta	[<L3+pOAMCopy_1],Y
	rep	#$20
	longa	on
	brl	L10001
L2	equ	41
L3	equ	5
	ends
	efunc
	BANK0
	xdef	~~IRQHandler
	func
~~IRQHandler:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L13
	tcs
	phd
	tcd
L15:
	pld
	tsc
	clc
	adc	#L13
	tcs
	rtl
L13	equ	0
L14	equ	1
	ends
	efunc
	BANK0
	xdef	~~termM0PrintString
	func
~~termM0PrintString:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L16
	tcs
	phd
	tcd
szInput_0	set	4
	data
L18:
	dw	$F800
	ends
test_heap_1	set	0
curOffset_1	set	4
i_1	set	5
	sep	#$20
	longa	off
	stz	<L17+curOffset_1
	rep	#$20
	longa	on
	pei	<L16+szInput_0+2
	pei	<L16+szInput_0
	jsl	~~strlen
	sta	<R0
	lda	<R0
	ina
	sta	<R1
	lda	<R1
	sta	<R0
	stz	<R0+2
	pei	<R0+2
	pei	<R0
	jsl	~~farmalloc
	sta	<L17+test_heap_1
	stx	<L17+test_heap_1+2
	stz	<L17+i_1
	brl	L10016
L10015:
	inc	<L17+i_1
L10016:
	pei	<L16+szInput_0+2
	pei	<L16+szInput_0
	jsl	~~strlen
	sta	<R0
	lda	<R0
	ina
	sta	<R1
	lda	<L17+i_1
	cmp	<R1
	bcc	L19
	brl	L10017
L19:
	sep	#$20
	longa	off
	ldy	<L17+i_1
	lda	[<L16+szInput_0],Y
	cmp	#<$a
	rep	#$20
	longa	on
	beq	L20
	brl	L10018
L20:
	sep	#$20
	longa	off
	lda	<L17+i_1
	sta	<L17+curOffset_1
	rep	#$20
	longa	on
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~strlen
	tax
	bne	L21
	brl	L10019
L21:
	pea	#<$0
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~strlen
	pha
	lda	|L18
	pha
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~LoadLoVram
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~strlen
	sta	<R1
	lda	<R1
	asl	A
	sta	<R0
	clc
	lda	<R0
	adc	|L18
	sta	|L18
L10019:
	lda	<L17+i_1
	ina
	sta	<R0
	sep	#$20
	longa	off
	lda	<R0
	sta	<L17+curOffset_1
	rep	#$20
	longa	on
	lda	|L18
	and	#<$ffc0
	sta	<R0
	clc
	lda	#$40
	adc	<R0
	sta	|L18
	brl	L10020
L10018:
	sep	#$20
	longa	off
	ldy	<L17+i_1
	lda	[<L16+szInput_0],Y
	cmp	#<$d
	rep	#$20
	longa	on
	beq	L22
	brl	L10021
L22:
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~strlen
	tax
	bne	L23
	brl	L10022
L23:
	pea	#<$0
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~strlen
	pha
	lda	|L18
	pha
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~LoadLoVram
L10022:
	lda	<L17+i_1
	ina
	sta	<R0
	sep	#$20
	longa	off
	lda	<R0
	sta	<L17+curOffset_1
	rep	#$20
	longa	on
	lda	#$3f
	trb	|L18
	brl	L10023
L10021:
	ldy	<L17+i_1
	lda	[<L16+szInput_0],Y
	and	#$ff
	beq	L24
	brl	L10024
L24:
	lda	<L17+curOffset_1
	and	#$ff
	sta	<R0
	sec
	lda	<L17+i_1
	sbc	<R0
	sta	<R1
	sep	#$20
	longa	off
	lda	#$0
	ldy	<R1
	sta	[<L17+test_heap_1],Y
	rep	#$20
	longa	on
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~strlen
	tax
	bne	L25
	brl	L10025
L25:
	pea	#<$0
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~strlen
	pha
	lda	|L18
	pha
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~LoadLoVram
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~strlen
	sta	<R1
	lda	<R1
	asl	A
	sta	<R0
	clc
	lda	<R0
	adc	|L18
	sta	|L18
L10025:
	lda	<L17+i_1
	ina
	sta	<R0
	sep	#$20
	longa	off
	lda	<R0
	sta	<L17+curOffset_1
	rep	#$20
	longa	on
L10024:
L10023:
L10020:
	lda	<L17+curOffset_1
	and	#$ff
	sta	<R0
	sec
	lda	<L17+i_1
	sbc	<R0
	sta	<R1
	sep	#$20
	longa	off
	ldy	<L17+i_1
	lda	[<L16+szInput_0],Y
	ldy	<R1
	sta	[<L17+test_heap_1],Y
	rep	#$20
	longa	on
	lda	<L17+curOffset_1
	and	#$ff
	sta	<R0
	sec
	lda	<L17+i_1
	sbc	<R0
	sta	<R1
	lda	<R1
	ina
	sta	<R0
	sep	#$20
	longa	off
	lda	#$0
	ldy	<R0
	sta	[<L17+test_heap_1],Y
	rep	#$20
	longa	on
	brl	L10015
L10017:
	pei	<L17+test_heap_1+2
	pei	<L17+test_heap_1
	jsl	~~farfree
	lda	#$0
L26:
	tay
	lda	<L16+2
	sta	<L16+2+4
	lda	<L16+1
	sta	<L16+1+4
	pld
	tsc
	clc
	adc	#L16+4
	tcs
	tya
	rtl
L16	equ	15
L17	equ	9
	ends
	efunc
	xref	~~initOAMCopy
	xref	~~LoadOAMCopy
	xref	~~LoadLoVram
	xref	~~LoadVram
	xref	~~LoadCGRam
	xref	~~initSNES
	xref	~~strlen
	xref	~~farmalloc
	xref	~~farfree
	xref	~~farcalloc
	xref	~~biker_clr
	xref	~~biker_pic
	xref	~~school_tilemap
	xref	~~school_pal
	xref	~~school_bin
	xref	~~school_bin_size
	end
