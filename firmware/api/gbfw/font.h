#ifndef GBFW_FONT_H
#define GBFW_FONT_H

#include <stdint.h>
#include <standard/pixel/rgb565.h>


typedef struct
{
	struct
	{
		uint16_t hspace : 1;
		uint16_t vspace : 3;
		uint16_t width  : 6;
		uint16_t height : 6;
	};

	uint8_t range[2];

	const uint8_t *bitmap;
} gbfw_font_t;


extern const gbfw_font_t *gbfw_font_default;
extern const gbfw_font_t *gbfw_font_small;


#define GBFW_FONT_ELLIPSIS    27
#define GBFW_FONT_ARROW_UP    28
#define GBFW_FONT_ARROW_DOWN  29
#define GBFW_FONT_ARROW_LEFT  30
#define GBFW_FONT_ARROW_RIGHT 31


void gbfw_font_draw(
	const gbfw_font_t *font,
	uint8_t x, uint8_t y,
	uint8_t cols,
	const char *text,
	rgb565_t fore, rgb565_t back);

void gbfw_font_draw_border(
	const gbfw_font_t *font,
	uint8_t x, uint8_t y,
	uint8_t cols,
	const char *text,
	rgb565_t fore, rgb565_t back,
	uint8_t w, uint8_t h);

#endif
