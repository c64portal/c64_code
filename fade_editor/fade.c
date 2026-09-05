#include "fade.h"

static unsigned char fade_map_screen_byte(unsigned char source, const unsigned char * colors)
{
	unsigned char lowColor = colors[source & 0x0f];
	unsigned char highColor = colors[source >> 4] << 4;

	return lowColor | highColor;
}

void fade_build_table(
	unsigned char * fadeTable,
	const unsigned char fadeBase[FADE_COLOR_STEPS][FADE_COLORS]
)
{
	unsigned char step;

	for (step = 0; step < FADE_COLOR_STEPS; ++step) {
		unsigned char hi;

		for (hi = 0; hi < FADE_COLORS; ++hi) {
			unsigned char lo;
			unsigned char highColor = fadeBase[step][hi] << 4;
			unsigned char value = hi << 4;
			const unsigned char * colors = fadeBase[step];

			for (lo = 0; lo < FADE_COLORS; ++lo) {
				fadeTable[(unsigned int)step * 256 + value + lo] = highColor | colors[lo];
			}
		}
	}
}

void fade_step_table(
	const unsigned char * sourceScreen,
	const unsigned char * sourceColor,
	const unsigned char * fadeAnim,
	const unsigned char * fadeTable,
	const unsigned char fadeBase[FADE_COLOR_STEPS][FADE_COLORS],
	const unsigned char * tablePage,
	unsigned int cellCount,
	unsigned char threshold
)
{
	unsigned int cell;

	for (cell = 0; cell < cellCount; ++cell) {
		unsigned char fadeValue = fadeAnim[cell];

		if (fadeValue >= threshold && fadeValue < threshold + FADE_COLOR_STEPS) {
			unsigned char step = tablePage[fadeValue - threshold];
			const unsigned char * colors = fadeBase[step];

			Screen[cell] = fadeTable[(unsigned int)step * 256 + sourceScreen[cell]];
			Color[cell] = colors[sourceColor[cell] & 0x0f];
		}
	}
}

void fade_step_realtime(
	const unsigned char * sourceScreen,
	const unsigned char * sourceColor,
	const unsigned char * fadeAnim,
	const unsigned char fadeBase[FADE_COLOR_STEPS][FADE_COLORS],
	const unsigned char * tablePage,
	unsigned int cellCount,
	unsigned char threshold
)
{
	unsigned int cell;

	for (cell = 0; cell < cellCount; ++cell) {
		unsigned char fadeValue = fadeAnim[cell];

		if (fadeValue >= threshold && fadeValue < threshold + FADE_COLOR_STEPS) {
			unsigned char step = tablePage[fadeValue - threshold];
			const unsigned char * colors = fadeBase[step];

			Screen[cell] = fade_map_screen_byte(sourceScreen[cell], colors);
			Color[cell] = colors[sourceColor[cell] & 0x0f];
		}
	}
}
