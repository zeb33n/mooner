#include <gbfw/font.h>
#include <gbfw/draw.h>
#include <gbfw/display.h>
#include <stdbool.h>

#include "gen/font_default.h"
const gbfw_font_t *gbfw_font_default = &font_default;

#include "gen/font_small.h"
const gbfw_font_t *gbfw_font_small = &font_small;


static const gbfw_font_t *gbfw_font_render_font;
static rgb565_t           gbfw_font_render_fore;
static rgb565_t           gbfw_font_render_back;
static uint8_t            gbfw_font_render_cols;
static const char        *gbfw_font_render_text;


static void gbfw_font_scan_callback(rgb565_t *color, uint8_t size, uint8_t row)
{
	const gbfw_font_t *font = gbfw_font_render_font;

	uint8_t x = 0;
	if (row < font->height)
	{
		uint8_t  bpg = font->width * font->height;
		uint32_t gyo = font->width * row;

		for (uint8_t col = 0; col < gbfw_font_render_cols; col++)
		{
			char c = gbfw_font_render_text[col];
			if (c == '\0') break;

			if ((c < font->range[0]) || (c > font->range[1]))
			{
				for (uint8_t gx = 0; (gx < font->width) && (x < size); gx++, x++)
					color[x] = gbfw_font_render_fore;
			}
			else
			{
				uint32_t co = ((c - font->range[0]) * bpg) + gyo;
				for (uint8_t gx = 0; (gx < font->width) && (x < size); gx++, x++, co++)
				{
					bool pixel = (font->bitmap[co / 8] >> (co % 8)) & 1;
					color[x] = (pixel ? gbfw_font_render_fore : gbfw_font_render_back);
				}
			}

			if (x >= size) break;

			if (font->hspace)
				color[x++] = gbfw_font_render_back;
		}
	}

	for (; x < size; x++)
		color[x] = gbfw_font_render_back;
}


void gbfw_font_draw(
	const gbfw_font_t *font,
	uint8_t x, uint8_t y,
	uint8_t cols,
	const char *text,
	rgb565_t fore, rgb565_t back)
{
	uint8_t w = ((font->width + font->hspace) * cols) - font->hspace;
	uint8_t h = font->height;

	// Ensure we're not corrupting font state from previous call.
	gbfw_display_sync();

	gbfw_font_render_font    = font;
	gbfw_font_render_fore    = fore;
	gbfw_font_render_back    = back;
	gbfw_font_render_cols    = cols;
	gbfw_font_render_text    = text;

	gbfw_display_render_region(
		x, y, w, h,
		gbfw_font_scan_callback);
}

void gbfw_font_draw_border(
	const gbfw_font_t *font,
	uint8_t x, uint8_t y,
	uint8_t cols,
	const char *text,
	rgb565_t fore, rgb565_t back,
	uint8_t w, uint8_t h)
{
	uint8_t bw = w - ((cols * (font->width + font->hspace)) - font->hspace);
	uint8_t bh = h - font->height;

	uint8_t bl = bw / 2;
	uint8_t br = (bw + 1) / 2;
	uint8_t bt = bh / 2;
	uint8_t bb = (bh + 1) / 2;

	gbfw_font_draw(font, (x + bl), (y + bt), cols, text, fore, back);
	gbfw_draw_box(x, y, w, h, back, bt, bb, bl, br);
}
