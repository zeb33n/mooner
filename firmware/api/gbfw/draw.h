#ifndef GBFW_DRAW_H
#define GBFW_DRAW_H

#include <standard/pixel/rgb565.h>
#include <stdint.h>


void gbfw_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, rgb565_t color);

void gbfw_draw_box(
	uint8_t x, uint8_t y, uint8_t w, uint8_t h, rgb565_t color,
	uint8_t t, uint8_t b, uint8_t l, uint8_t r);

#define gbfw_draw_vline(x, y0, y1, color) \
	gbfw_draw_rect((x), (y0), 1, 1 + ((y1) - (y0)), (color))

#define gbfw_draw_hline(x0, x1, y, color) \
	gbfw_draw_rect((x0), (y), 1 + ((x1) - (x0)), 1, (color))

#define gbfw_draw_box_simple(x, y, w, h, color, border) \
	gbfw_draw_box((x), (y), (w), (h), (color), \
		(border), (border), (border), (border))

#endif
