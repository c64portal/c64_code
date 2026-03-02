/*
* "Censor Hires Bubbles Intro" 
*
* Created in Jan/Feb 2026 for for CSDB ICC2026 compo
* By Carrion / Censor Design
* Music by Magnar / Censor Design
----------------------------------------------------------------------*/

// #define _DEBUG_

#include <c64/memmap.h>
#include <c64/cia.h>
#include <c64/vic.h>
#include <c64/sprites.h>
#include <c64/joystick.h>
#include <c64/keyboard.h>
#include <c64/rasterirq.h>

#include <string.h>
#include <oscar.h>

#include "mem.h"

//--------------------------------

#define _screen0 0x0400
#define SCREEN0 ((char *)_screen0)
#define COLS    ((char *)0xd800)
#define CHARS0  ((char *)0x2000)
#define FONTS0  ((char *)0x0800)
#define _sprites 0x2200
#define SPRITES ((char *)_sprites)
#define _anim 0x2800
#define ANIM ((char *)_anim)

#define SEI __asm { sei } 
#define CLI __asm { cli } 
#define IRQ_VECTOR *(void **)0xfffe

//--------------------------------

#define RASTERLINE_0 0x0
#define RASTERLINE_1 0x52
#define RASTERLINE_2 0x6f
#define RASTERLINE_3 0x8a
#define RASTERLINE_4 0xf2
#define SPR_POS_Y 61

#define SCROLL_LINE 19
#define TEXT_LINE1 13
// #define TEXT_LINE2 14

#define COLOR_0 0x00
#define COLOR_1 0x06
#define COLOR_4 0x0b
#define COLOR_2 0x05
#define COLOR_6 0x03
#define COLOR_3 0x0d
#define COLOR_5 0x01

__hwinterrupt void irq0(void);
__hwinterrupt void irq1(void);
__hwinterrupt void irq2(void);
__hwinterrupt void irq3(void);
__hwinterrupt void irq4(void);

//--------------------------------

#pragma code ( code )
#pragma data ( data )

int posx1[8];
int posx2[8];
int posx3[8];
int posx4[8]={100,124,148,172,100,124,148,172};
char posy4[8]={0,0,0,0,21,21,21,21};

char tick1=0;
char tick2=0;
__zeropage char animspr1=0 ;
__zeropage char animspr2=0 ;

const char text[] = s"3.. 2.. 1.. 0!    csdb icc 2026!   greetings to all compo participants."
s"   this intro was coded in c oscar64!            ";

__zeropage char txtPos=0;
__zeropage char scroll=7;

const char text1[] = s"censor hires bubbles intro     "
s"   by carrion and magnar of censor design";

__zeropage char sxi=0;
__zeropage char syi=0;

char colorPalette[]={
0x00, 0x06, 0x0b, 0x05, 0x03, 0x0d, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01,
0x01, 0x0d, 0x03, 0x05, 0x0b, 0x06, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};

__zeropage char cfade1=0;
__zeropage char cfade2=0;

__zeropage char fnta1 = 0;

//------------------------------------------------------------------------

void musicInit(){
	__asm{
		lda #$00
		jsr $1000
	}
}

void musicPlay (){
	__asm {
		jsr $1003
	}
}

//------------------------------------------------------------------------

void spriteAnimation1 (){

	char a = animspr1>>2;
	for (char i=0; i<64;i++){
		SPRITES[i] = sprites[(64*a)+i];
	}
}

void spriteAnimation2 (){

	char a = animspr2>>3;
	for (char i=0; i<64;i++){
		SPRITES[i+64] = sprites[64*8+(64*a)+i];
	}
}

void spritesMove (int *pos, char col, char y){

	char spr=1;

	#pragma unroll(full)
	for (char i=0 ; i<8; i++){
		spr_color(i,col);
		spr_move (i, pos[i], SPR_POS_Y+y);
		spr=1-spr;
		spr_image (i,0x88+spr);
	}
}

void spritesMove2 (int *xpos, char col, char *ypos){

	char spr=1;

	char sx = sin_table[sxi++];
	if (sxi==128) sxi=0;
	char sy = sin_table[syi++];
	if (syi==128) syi=0;

	#pragma unroll(full)
	for (char i=0 ; i<8; i++){
		spr_color(i,col);
		spr_move (i, xpos[i]+sx, 116+ SPR_POS_Y - 8 + ypos[i]+(sy>>1));
		// spr=1-spr;
		spr_image (i,0xa0+i);

	}
}

//------------------------------------------------------------------------

// not the fastest scroll routine ;) 
// but it works
void scrollText (){
	if (scroll<=0){
		scroll=7;
		char c=0;

		for (char i=0; i<39; i++){
			c = text[txtPos];

			SCREEN0[(40*SCROLL_LINE)+i] = SCREEN0[(40*SCROLL_LINE)+i+1];

			if (c==0) txtPos=0;

		}
		SCREEN0[(40*SCROLL_LINE)+39] = c+0x80;
		txtPos++;
	}
	else{
		scroll --;
	}
}

void colorFader1 (){

	for (char i=0; i<40; i++){
		COLS[TEXT_LINE1*40+i]   = colorPalette[(cfade1)];
		COLS[TEXT_LINE1*40+i+40]= colorPalette[(cfade2)];
	}
}

void fontAnimation1 (char fr){

	for (char i=0; i<8; i++){
		FONTS0[0x1f0+i] = charanim1[(fr*8)+i];
		FONTS0[0x1e8+i] = charanim2[(fr*8)+i];
	}
}
//------------------------------------------------------------------------


void waitForSpace (){
	__asm {
		lda $dc00
		and #%00010000
		cmp #%00010000
		bne pressed

		lda #$ff
		sta $dc02
		lda $dc01
		cmp #$ef
		beq pressed
		rts

pressed:
		lda #$37
		sta $01
		jsr $ff81
		sei
		lda #$31
		sta $0e14
		lda #$ea
		sta $0315
		cli
		lda #$00
		sta $d418

		jmp $fce2 // reset
	}
}

//------------------------------------------------------------------------

__hwinterrupt void irq0(void){

	vic.color_back = 0;
	vic.color_back1 = COLOR_1;
	vic.color_back2 = COLOR_2;
	vic.color_back3 = COLOR_3;

#ifdef _DEBUG_
	vic.color_border = 0x06;
#endif

	vic_setmode(VICM_TEXT_ECM, SCREEN0, CHARS0);

	vic.spr_priority=0xff;

	spritesMove (posx1, COLOR_2, 0);
	
	for (char i=0; i<8; i++){
		
		if (tick1==1) posx1[i]--;
		if (posx1[i]<0) posx1[i]=336+48;

		posx2[i]--;
		posx3[i]-=2;
		if (posx2[i]<0) posx2[i]=336+48;
		if (posx3[i]<0) posx3[i]=336+48;
	}	

#ifdef _DEBUG_
	vic.color_border = 0x0b;
#endif

	animspr1 ++;
	spriteAnimation1 ();
	if (animspr1==31) animspr1=0;

#ifdef _DEBUG_
	vic.color_border = 0x02;
#endif

	animspr2 ++;
	spriteAnimation2 ();
	if (animspr2==63) animspr2=0;

#ifdef _DEBUG_
	vic.color_border = 0x00;
#endif

	vic.raster = RASTERLINE_1;
	IRQ_VECTOR = irq1; 
	vic.intr_ctrl <<= 1;	

}

//------------------------------------------------------------------------

__hwinterrupt void irq1(void){

	vic.color_back =0;
	vic.color_back1 = COLOR_4;

#ifdef _DEBUG_
	vic.color_border = 0x02;
#endif
	vic.spr_priority=0x00;
	
	spritesMove (posx2, COLOR_3, 28);

	tick1 = 1 - tick1;
	tick2 ++;
	if (tick2==5) tick2=0;

#ifdef _DEBUG_
	vic.color_border = 0x00;
#endif

	vic.raster = RASTERLINE_2 ;
	IRQ_VECTOR = irq2; 
	vic.intr_ctrl <<= 1;
}

//------------------------------------------------------------------------

__hwinterrupt void irq2(void){

#ifdef _DEBUG_
	vic.color_border = 0x05;
#endif

	spritesMove (posx3, 0x00, 56);

#ifdef _DEBUG_
	vic.color_border = 0x00;
#endif

	vic.raster = RASTERLINE_3 ;
	IRQ_VECTOR = irq3;
	vic.intr_ctrl <<= 1;
}

//------------------------------------------------------------------------

__hwinterrupt void irq3(void){

	vic_setmode(VICM_TEXT_ECM, SCREEN0, FONTS0);
	vic.color_back1 = COLOR_1;
	vic.color_back2 = 0x0e;

#ifdef _DEBUG_
	vic.color_border = 0x02;
#endif
	scrollText();

#ifdef _DEBUG_
	vic.color_border = 0x04;
#endif

	spritesMove2 (posx4, COLOR_5, posy4);

	// this is a ugly way to wait for a specific line and....
	__asm{		
		lda #$b0
		cmp $d012
		bne *-3
	}

	//  .... and then do scrolling in $D016
	vic.ctrl2 = (scroll ) & 0b00000111; 

#ifdef _DEBUG_
	vic.color_border = 0x00;
#endif

	waitForSpace ();

	vic.raster = RASTERLINE_4 ;
	IRQ_VECTOR = irq4;
	vic.intr_ctrl <<= 1;
}

//------------------------------------------------------------------------

__hwinterrupt void irq4(void){

#ifdef _DEBUG_
	vic.color_border = 0x09;
#endif

	musicPlay ();

	colorFader1();
	cfade1 ++;
	cfade1 = cfade1 & 0b00011111;
	cfade2 ++;
	cfade2 = cfade2 & 0b00011111;

	fontAnimation1 ( (fnta1++) & 0b00000111 );

#ifdef _DEBUG_
	vic.color_border = 0x00;
#endif

	vic.raster = RASTERLINE_0 ;
	IRQ_VECTOR = irq0;
	vic.intr_ctrl <<= 1;
}

/*
----------------------------------------------------------------------
Main Start of the program
----------------------------------------------------------------------
*/

int main(void){

	mmap_set(MMAP_ROM);
	cia_init();

	// background and border colors to black
	vic.color_border = 0x00;
	vic.color_back = 0x00;

	// some vars inits
	scroll=7;
	sxi=0;
	sxi=32;
	cfade1=0;
	cfade2=6;
	fnta1=0;

	memclr(SCREEN0, 0x0400);
	memset(COLS, COLOR_5, 0x400);

	memset(COLS+(40*1),  COLOR_1, 40*6);
	memset(COLS+(40*2),  COLOR_5, 40*6);
	memset(COLS+(40*6),  COLOR_2, 40*4);
	memset(COLS+(40*10), COLOR_4, 40*1);

	memset(COLS+(40*16), 0x00, 40*1);
	memset(COLS+(40*17), 0x06, 40*1);
	memset(COLS+(40*18), 0x0f, 40*1);

	memset(COLS+(40*20), 0x01, 40*1);
	memset(COLS+(40*21), 0x06, 40*1);
	memset(COLS+(40*22), 0x00, 40*1);

	COLS[40+20]=COLOR_5;
	COLS[40+21]=COLOR_5;

	oscar_expand_lzo( SCREEN0, map); 
	oscar_expand_lzo( FONTS0, fonts); 

	strcpy(SCREEN0+(TEXT_LINE1*40+7), text1);

	spr_init (SCREEN0);

	vic.spr_priority=0xff;

	char spr=1;

	for (char i=0; i<8; i++){
		spr=1-spr;

		// clear x positions for 3 sprites parallax layers
		int sx=i*48; // multiplying here is not critical for the performance
		posx1[i]=sx;
		posx2[i]=sx;
		posx3[i]=sx;
		
		// init and setup the sprites 
		spr_set (i, true, sx, 61, 0x88+spr, 0x05,false,false,false);	
	}
	
	musicInit ();

	SEI;
		mmap_set(MMAP_NO_ROM);	

		IRQ_VECTOR = irq0;     
		vic.intr_enable = VIC_INTR_RST;	

		vic.ctrl1 &= ~VIC_CTRL1_RST8;
		vic.raster = RASTERLINE_0;
	CLI;

	//-------------------------------------------

	char goForward =1;
	char animFrame=0;

	while (true){

		vic_waitFrame ();

		oscar_expand_lzo( ANIM, anim_frames[animFrame]); 

		if (goForward==1){
			animFrame++;
			if (animFrame==25) goForward=0;
		}
		if (goForward==0){
			animFrame--;
			if (animFrame==0) goForward=1;
		}
		
	}

	return 0;

}
