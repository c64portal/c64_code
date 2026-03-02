/*
* "Censor Hires Bubbles Intro" 
* for CSDB ICC2026 compo
* By Carrion / Censor Design
* Music by Magnar / Censor Design
*
* memory segments definiotions & data assets  
--------------------------------------------------------------------------*/

#pragma stacksize(0)
#pragma heapsize(0)	

#pragma region( code0, 0x0a00, 0x0fff, , ,     { code ,data, bss, stack} )
#pragma region( code1, 0x1700, 0x1fff, , ,    {code, data, bss, heap, stack} )
#pragma region( code2, 0x3000, 0x3bff, , ,    {code, bss, heap, stack} )
#pragma region( onlydata, 0x3c00, 0x3fff, , ,    {data} )

// ------------------------------------------------------------------------

#pragma section( musicsection, 0) 
#pragma region( data2, 0x1000, 0x16ff, , , { musicsection } )
#pragma data ( musicsection )
__export char music[] ={
    #embed 2048 2 "data/music.prg"
};

// ------------------------------------------------------------------------

#pragma data ( data )
__export char map[] ={
    #embed ctm_map8 lzo  "data/censor.ctm"
};

__export char fonts[] ={
    #embed ctm_chars lzo  "data/fonts.ctm"
};

__export char charanim1[] ={
    #embed ctm_chars "data/charanim1.ctm"
};

__export char charanim2[] ={
    #embed ctm_chars "data/charanim2.ctm"
};

__export unsigned char sin_table[128] = {
  32, 34, 35, 37, 38, 40, 41, 43, 44, 46, 47, 48, 50, 51, 52, 53,
  55, 56, 57, 58, 59, 59, 60, 61, 62, 62, 63, 63, 63, 64, 64, 64,
  64, 64, 64, 64, 63, 63, 63, 62, 62, 61, 60, 59, 59, 58, 57, 56,
  55, 53, 52, 51, 50, 48, 47, 46, 44, 43, 41, 40, 38, 37, 35, 34,
  32, 30, 29, 27, 26, 24, 23, 21, 20, 18, 17, 16, 14, 13, 12, 11,
   9,  8,  7,  6,  5,  5,  4,  3,  2,  2,  1,  1,  1,  0,  0,  0,
   0,  0,  0,  0,  1,  1,  1,  2,  2,  3,  4,  5,  5,  6,  7,  8,
   9, 11, 12, 13, 14, 16, 17, 18, 20, 21, 23, 24, 26, 27, 29, 30
};

// ------------------------------------------------------------------------

#pragma section( charsection, 0) 
#pragma region( data3, 0x2000, 0x2200, , , { charsection } )
#pragma data ( charsection )
__export char chars[] ={
    #embed 512 "data/censor - Chars.bin"
};

#pragma section( spritesection2, 0) 
#pragma region( data4, 0x2400, 0x2800, , , { spritesection2 } )
#pragma data ( spritesection2 )
__export char sprites[] = {
	#embed spd_sprites  "data/sprites.spd"
};

// ------------------------------------------------------------------------

// meta balls animation frames each one is 512 bytes compressed with oscar internal lzo compresor
#pragma section( spritesection, 0) 
#pragma region( data5, 0x2a00, 0x3fff, , , { spritesection } )
#pragma data ( spritesection )

__export char s_0000[] = {
    #embed lzo "data/anim/0000.bin"
};

__export char s_0001[] = {
    #embed lzo "data/anim/0001.bin"
};

__export char s_0002[] = {
    #embed lzo "data/anim/0002.bin"
};

__export char s_0003[] = {
    #embed lzo "data/anim/0003.bin"
};

__export char s_0004[] = {
    #embed lzo "data/anim/0004.bin"
};

__export char s_0005[] = {
    #embed lzo "data/anim/0005.bin"
};

__export char s_0006[] = {
    #embed lzo "data/anim/0006.bin"
};

__export char s_0007[] = {
    #embed lzo "data/anim/0007.bin"
};

__export char s_0008[] = {
    #embed lzo "data/anim/0008.bin"
};

__export char s_0009[] = {
    #embed lzo "data/anim/0009.bin"
};

__export char s_0010[] = {
    #embed lzo "data/anim/0010.bin"
};

__export char s_0011[] = {
    #embed lzo "data/anim/0011.bin"
};

__export char s_0012[] = {
    #embed lzo "data/anim/0012.bin"
};

__export char s_0013[] = {
    #embed lzo "data/anim/0013.bin"
};

__export char s_0014[] = {
    #embed lzo "data/anim/0014.bin"
};

__export char s_0015[] = {
    #embed lzo "data/anim/0015.bin"
};

__export char s_0016[] = {
    #embed lzo "data/anim/0016.bin"
};

__export char s_0017[] = {
    #embed lzo "data/anim/0017.bin"
};

__export char s_0018[] = {
    #embed lzo "data/anim/0018.bin"
};

__export char s_0019[] = {
    #embed lzo "data/anim/0019.bin"
};

__export char s_0020[] = {
    #embed lzo "data/anim/0020.bin"
};

__export char s_0021[] = {
    #embed lzo "data/anim/0021.bin"
};

__export char s_0022[] = {
    #embed lzo "data/anim/0022.bin"
};

__export char s_0023[] = {
    #embed lzo "data/anim/0023.bin"
};

__export char s_0024[] = {
    #embed lzo "data/anim/0024.bin"
};

// table of pointers to each of 25 frames
__export char* anim_frames[] = {
    s_0000,
    s_0001,
    s_0002,
    s_0003,
    s_0004,
    s_0005,
    s_0006,
    s_0007,
    s_0008,
    s_0009,
    s_0010,
    s_0011,
    s_0012,
    s_0013,
    s_0014,
    s_0015,
    s_0016,
    s_0017,
    s_0018,
    s_0019,
    s_0020,
    s_0021,
    s_0022,
    s_0023,
    s_0024
};
