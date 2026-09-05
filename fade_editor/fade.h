#ifndef FADE_H
#define FADE_H

#define FADE_COLOR_STEPS 8
#define FADE_COLORS 16
#define FADE_TABLE_SIZE (FADE_COLOR_STEPS * 256)

extern unsigned char * const Screen;
extern volatile unsigned char * const Color;

void fade_build_table(
	unsigned char * fadeTable,
	const unsigned char fadeBase[FADE_COLOR_STEPS][FADE_COLORS]
);

void fade_step_table(
	const unsigned char * sourceScreen,
	const unsigned char * sourceColor,
	const unsigned char * fadeAnim,
	const unsigned char * fadeTable,
	const unsigned char fadeBase[FADE_COLOR_STEPS][FADE_COLORS],
	const unsigned char * tablePage,
	unsigned int cellCount,
	unsigned char threshold
);

void fade_step_realtime(
	const unsigned char * sourceScreen,
	const unsigned char * sourceColor,
	const unsigned char * fadeAnim,
	const unsigned char fadeBase[FADE_COLOR_STEPS][FADE_COLORS],
	const unsigned char * tablePage,
	unsigned int cellCount,
	unsigned char threshold
);

#pragma compile("fade.c")

#endif
