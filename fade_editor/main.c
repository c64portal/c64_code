#include <c64/memmap.h>
#include <c64/vic.h>
#include <string.h>

#include "fade.h"

#pragma stacksize(512)
#pragma heapsize(0)

#pragma region(main, 0x0a00, 0x4000, , , {code, data, bss, heap, stack})

#pragma section(screen_bss, 0, , , bss)
#pragma region(screen_region, 0x4000, 0x4400, , , {screen_bss})

#ifndef USE_FADE_TABLE
#define USE_FADE_TABLE 0
#endif
#define KOALA_CELLS 1000
#define FADE_LAST_STEP 80

#if USE_FADE_TABLE
#pragma section(fade_bss, 0, , , bss)
#pragma region(fade_region, 0x4400, 0x4c00, , , {fade_bss})
#endif

#pragma section(bitmap_data, 0)
#pragma region(bitmap_region, 0x6000, 0x7f40, , , {bitmap_data})

#pragma bss(screen_bss)
__export unsigned char ScreenRamMemory[1000];

#pragma data(data)
unsigned char * const Screen = ScreenRamMemory;
volatile unsigned char * const Color = (volatile unsigned char *)0xd800;

#if USE_FADE_TABLE
#pragma bss(fade_bss)
unsigned char fadeTable[FADE_TABLE_SIZE];
#endif

#pragma data(bitmap_data)
__export const unsigned char BitmapData[8000] = {
	#embed 8000 2 "pic.prg"
};

#pragma data(data)
const unsigned char KoalaScreen[1000] = {
	#embed 1000 8002 "pic.prg"
};

const unsigned char KoalaColor[1000] = {
	#embed 1000 9002 "pic.prg"
};

const unsigned char KoalaBackground[1] = {
	#embed 1 10002 "pic.prg"
};

const unsigned char FadeAnim[1000] = {
	#embed "women.fade"
};

const unsigned char fadeBase[8][16] = {
	{0x0,0x6,0x0,0x0,0x0,0x0,0x0,0x9,0x0,0x0,0x0,0x0,0x0,0x9,0x0,0x0},
	{0x0,0xb,0x0,0x9,0x0,0x9,0x0,0xb,0x0,0x0,0x9,0x0,0x9,0xb,0x9,0x6},
	{0x0,0x4,0x0,0xb,0x9,0xb,0x0,0x8,0x9,0x0,0xb,0x0,0x9,0x8,0x9,0xb},
	{0x0,0xc,0x9,0x8,0x9,0xb,0x0,0xc,0x9,0x0,0xb,0x9,0xb,0xc,0xb,0x4},
	{0x0,0xe,0x9,0x8,0xb,0x8,0x0,0xc,0xb,0x0,0x8,0x9,0xb,0xc,0xb,0x4},
	{0x0,0x3,0x9,0xc,0xb,0xc,0x0,0x5,0xb,0x0,0xc,0x9,0x8,0x5,0x8,0xc},
	{0x0,0xd,0x9,0x5,0xb,0xc,0x0,0x3,0xb,0x0,0xc,0x9,0x8,0x3,0x8,0xc},
	{0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf}
};

const unsigned char tablePage[10] = {
	7, 6, 5, 4, 3, 2, 1, 0, 0, 0
};

#pragma bss(bss)

int main(void)
{
	unsigned char threshold;

	mmap_trampoline();
	mmap_set(MMAP_NO_ROM);

	memset(Screen, 0, KOALA_CELLS);
	memset((void *)Color, 0, KOALA_CELLS);

	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLACK;
	vic_setmode(VICM_HIRES_MC, Screen, BitmapData);

#if USE_FADE_TABLE
	fade_build_table(fadeTable, fadeBase);
#endif

	for (threshold = 1; threshold < FADE_LAST_STEP; ++threshold) {
		// vic_waitFrame(); // vic_waitFrame(); vic_waitFrame();
		vic.color_border = 0x00;

#if USE_FADE_TABLE
		fade_step_table(
			KoalaScreen,
			KoalaColor,
			FadeAnim,
			fadeTable,
			fadeBase,
			tablePage,
			KOALA_CELLS,
			threshold
		);
#else
		fade_step_realtime(
			KoalaScreen,
			KoalaColor,
			FadeAnim,
			fadeBase,
			tablePage,
			KOALA_CELLS,
			threshold
		);
#endif

		vic.color_border = VCOL_BLACK;
	}

	memcpy(Screen, KoalaScreen, KOALA_CELLS);
	memcpy((void *)Color, KoalaColor, KOALA_CELLS);
	vic.color_back = KoalaBackground[0] & 0x0f;

	for (;;) {
	}

	return 0;
}
