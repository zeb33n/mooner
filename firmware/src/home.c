#include "home.h"
#include "control.h"
#include "display.h"
#include "load.h"
#include "settings.h"
#include <gbfw/draw.h>
#include <gbfw/font.h>


#define GBFW_HOME_COLOR_FORE RGB565_WHITE
#define GBFW_HOME_COLOR_BACK RGB565_GRAY(31)

#define GBFW_HOME_LABEL_MAX 6

#define GBFW_HOME_BUTTON_SIZE 13


static void gbfw_home__draw_button(const gbfw_font_t *font, uint8_t x, uint8_t y, uint8_t size, char button, rgb565_t color)
{
	rgb565_t back = rgb565_blend8(color, RGB565_BLACK, 192);

	gbfw_font_draw_border(
		font, (x + 1), (y + 1), 1, &button, color, back,
		(size - 2), (size - 2));
	gbfw_draw_box_simple(x, y, size, size, color, 1);
}

static void gbfw_home__draw_button_instruction(
	char button, rgb565_t button_color, const char *label, uint8_t label_size, uint8_t y, uint8_t height)
{
	const gbfw_font_t *font = (height < 13 ? gbfw_font_small : gbfw_font_default);

	uint8_t button_size       = height;
	uint8_t space_width       = 4;
	uint8_t max_caption_width = (GBFW_HOME_LABEL_MAX * (font->width + font->hspace)) - font->hspace;
	uint8_t caption_width     = (label_size * (font->width + font->hspace)) - font->hspace;

	uint8_t border_width = GBFW_DISPLAY_WIDTH
		- (button_size + space_width + max_caption_width);

	uint8_t left_border  = border_width / 2;
	uint8_t right_border = ((border_width + 1) / 2) + (max_caption_width - caption_width);

	uint8_t x = 0;

	gbfw_draw_rect(x, y, left_border, height, GBFW_HOME_COLOR_BACK);
	x += left_border;

	gbfw_home__draw_button(font, x, y, button_size, button, button_color);
	x += button_size;

	gbfw_draw_rect(x, y, space_width, height, GBFW_HOME_COLOR_BACK);
	x += space_width;

	gbfw_font_draw_border(
		font, x, y, label_size, label,
		GBFW_HOME_COLOR_FORE, GBFW_HOME_COLOR_BACK,
		caption_width, height);
	x += caption_width;

	gbfw_draw_rect(x, y, right_border, height, GBFW_HOME_COLOR_BACK);
}

void gbfw_home(void)
{
	if (gbfw_settings->primary.home == GBFW_SETTINGS_HOME_QUIT)
	{
		gbfw_signal(GBFW_SIGNAL_EXIT);
		return;
	}

	gbfw_display_sync();

	uint8_t menu_space      = 4;
	uint8_t vertical_border = GBFW_DISPLAY_HEIGHT - ((GBFW_HOME_BUTTON_SIZE * 2) + menu_space);
	uint8_t top_height      = vertical_border / 2;
	uint8_t bottom_height   = (vertical_border + 1) / 2;

	uint8_t y = 0;
	gbfw_draw_rect(
		0, y,
		GBFW_DISPLAY_WIDTH, top_height,
		GBFW_HOME_COLOR_BACK);
	y += top_height;

	gbfw_home__draw_button_instruction('A', RGB565_GREEN, "Resume", 6, y, GBFW_HOME_BUTTON_SIZE);
	y += GBFW_HOME_BUTTON_SIZE;

	gbfw_draw_rect(
		0, y,
		GBFW_DISPLAY_WIDTH, menu_space,
		GBFW_HOME_COLOR_BACK);
	y += menu_space;

	gbfw_home__draw_button_instruction('B', RGB565_RED, "Exit", 4, y, GBFW_HOME_BUTTON_SIZE);
	y += GBFW_HOME_BUTTON_SIZE;

	gbfw_draw_rect(
		0, y,
		GBFW_DISPLAY_WIDTH, bottom_height,
		GBFW_HOME_COLOR_BACK);

	// TODO: Render Battery Charge Level & Status

	// TODO: Render Time and Date

	// TODO: Render Volume and Brightness

	gbfw_control_t control_last = { .start = 1, .select = 1 };
	bool open = true;
	while (open)
	{
		gbfw_control_t control  = gbfw_control_poll();
		gbfw_control_t released = { .mask = ~control.mask & control_last.mask };
		control_last.mask = control.mask;

		if (released.a)
		{
			open = false;
		}
		else if (released.b)
		{
			gbfw_signal(GBFW_SIGNAL_EXIT);
			open = false;
		}
	}
}
