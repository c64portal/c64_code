
.var charcolour = $d800
.var screencolour = $6000
.var sc1b = $4000
.var pic1 = $6800
.var anim = $6400 //size $0400
.var fadetable = $2800 //size $0800
.var screen = $ff

.pc = pic1
.import c64 "2a.prg" //loading picture in koala format
.var ScreenRam= pic1+$1f40
.var ColorRam = pic1+$1f40+$3e8
.var bitm1 = pic1



.pc=anim
.import binary "women.fade"

.pc = $0801 "Basic Program"
:BasicUpstart($0900)

.pc =$0900 "Program"

start:
	sei
	lda #$00
	sta $d020
	sta $d021
	lda #$0b
	sta $d011

{ //     generate fade table color    //
	lda #<fadetable
	sta $20
	lda #>fadetable
	sta $21
	clc
	adc #8
	sta doilr+1
		
	ldy #$00
pet:	ldx #$00
!:	
dfg:	lda tab,x
	clc	
dodaj:	adc #$00
	sta ($20),y
	inx
	iny
	cpx #$10
	bne !-
	inc dfl+1	
dfl:	ldx #$00
dfl2:	lda tab,x
	asl
	asl
	asl
	asl
	sta dodaj+1
	cpy #$00
	bne pet
	lda #$00
	sta dfl+1
	sta dodaj+1
	lda dfl2+1
	clc
	adc #16
	sta dfl2+1
	sta dfg+1
	lda dfl2+2
	adc #$00
	sta dfl2+2
	sta dfg+2

	inc $21
	lda $21
doilr:	cmp #$30 
	bne pet
/////////////////////////////////////
}

	
	lda #$34
	sta $01        // disable all ROMs


{	
	ldy #$00
	ldx #$00
gl1:	lda bitm1,y
gl2:	sta sc1b,y
	iny
	bne gl1
	inc gl1+2
	inc gl2+2
	inx
	cpx #$20
	bne gl1
}

	lda #$35
	sta $01        // disable all ROMs
	

	
	ldy #$00
	lda #$00
!:	
	sta screencolour,y
	sta screencolour+$100,y
	sta screencolour+$200,y
	sta screencolour+$300,y
	sta $d800,y
	sta $d800+$100,y
	sta $d800+$200,y
	sta $d800+$300,y
	iny
	bne !-
	
	
	lda #$7f
	sta $dc0d      // no timer IRQs
	lda $dc0d     // clear timer IRQ flags
	lda #%00001011 //blank screen
	sta $d011
	
	lda #$01
	sta $d01a      // enable raster IRQs
	dec $d019      // clear raster IRQ flag

	
	lda #$00
	sta $d020
	sta $d021
	lda #$00
	cmp $d012
	bne *-3
	
	lda #$ff
	cmp $d012
	bne *-3

		
	lda #$dc
	sta $d012
	lda #<irq
	sta $fffe
	lda #>irq
	sta $ffff
	
	
	
	
	ldx #$20
per:	lda #$ff
	cmp $d012
	bne *-3	
	lda #$fe
	cmp $d012
	bne *-3
	dex
	bne per
	
	
	cli


	lda #$38
	sta $d011
	
	lda #$d8
	sta $d016

	lda #$38
	sta $d018
	
	lda $d018
	and #%10000111
	ora #%10000000
	sta $d018
	lda #%00000010 
	sta $dd00
	
	jmp fadeo

waitraster:
	lda #$50
	cmp $d012
	bne *-3
	lda #$4f
	cmp $d012
	bne *-3
	rts

fadeo:
{ 
	ldx #$00
	stx $20

stu:	
!:	
ade:	lda anim,x
ade2:	cmp #$01
	bpl lll
yt:	inx
	bne !-
	inc szlok+1
	inc ad1+2
	inc ad2+2
	inc ad3+2
	inc ad4+2
	inc ade+2
szlok:	lda #$00
	cmp #$04
	bne !-
	lda #$00
	sta szlok+1
	lda #>anim
	sta ade+2
	lda #>ScreenRam
	sta ad1+2
	lda #>screencolour 
	sta ad2+2
	lda #>ColorRam
	sta ad3+2
	lda #$d8
	sta ad4+2
	inc ade2+1
	inc ade3+1
	lda ade3+1
	cmp #80 //43
	beq endzik1
	
	//dec $d020
	jsr waitraster
	
	jmp stu

endzik1:
	jmp *
lll:
ade3:	cmp #$09
	bpl yt
	sec
	sbc ade2+1
	tay
	lda tabp,y
	sta $21 
ad1:	ldy ScreenRam,x
	lda ($20),y
ad2:	sta screencolour,x
ad3:	ldy ColorRam,x
	lda ($20),y
ad4:	sta $d800,x
	jmp yt

/////////////////////////////////////////
}	
irq:
	sta as+1
	stx xs+1
	sty ys+1
	lda $01
	sta s01+1
	
	dec $d019
s01:	lda #$00
	sta $01
as: 	lda #$00
xs: 	ldx #$00
ys: 	ldy #$00
 	rti
tabp:	.byte $2f,$2e,$2d,$2c,$2b,$2a,$29,$28,$28,$28



.pc =$3f80
tab: 
	.byte $0,$9,$0,$0,$0,$0,$0,$9,$0,$0,$0,$0,$0,$9,$0,$0
	.byte $0,$b,$0,$9,$0,$9,$0,$b,$0,$0,$9,$0,$9,$b,$9,$9
	.byte $0,$8,$0,$b,$9,$b,$0,$8,$9,$0,$b,$0,$9,$8,$9,$b
	.byte $0,$c,$9,$8,$9,$b,$0,$c,$9,$0,$b,$9,$b,$c,$b,$8
	.byte $0,$5,$9,$8,$b,$8,$0,$c,$b,$0,$8,$9,$b,$c,$b,$8
	.byte $0,$3,$9,$c,$b,$c,$0,$5,$b,$0,$c,$9,$8,$5,$8,$c
	.byte $0,$7,$9,$5,$b,$c,$0,$3,$b,$0,$c,$9,$8,$3,$8,$5
	.byte $0,$1,$2,$3,$4,$5,$6,$7,$8,$9,$a,$b,$c,$d,$e,$f
