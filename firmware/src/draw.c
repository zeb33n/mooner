#include <gbfw/draw.h>
#include "display.h"
#include <stddef.h>



void gbfw_draw_rect(
	uint8_t x, uint8_t y, uint8_t w, uint8_t h,
	rgb565_t color)
{
	// Ensure we're not currently drawing before updating buffer.
	gbfw_display_sync();

	for (size_t i = 0; i < (w * 2); i++)
		gbfw_display_scanbuff[i] = color;

	gbfw_display_render_region(x, y, w, h, NULL);
}

void gbfw_draw_box(
	uint8_t x, uint8_t y, uint8_t w, uint8_t h, rgb565_t color,
	uint8_t t, uint8_t b, uint8_t l, uint8_t r)
{
	if (t > 0) gbfw_draw_rect(x, y, w, t, color);
	y += t;
	h -= t;

	uint8_t m = h - b;
	if (m > 0)
	{
		if (l > 0) gbfw_draw_rect(x, y, l, m, color);
		if (r > 0) gbfw_draw_rect(x + (w - r), y, r, m, color);
	}
	y += m;

	if (b > 0) gbfw_draw_rect(x, y, w, b, color);
}
