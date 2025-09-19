#include "splash.h"
#include "display.h"
#include "tick.h"
#include "delay.h"
#include "task.h"
#include "settings.h"
#include <gbfw/api.h>
#include <hardware/cm4/intrinsics.h>
#include <standard/pixel/rgb888.h>
#include <standard/pixel/rxgb5155.h>
#include <stddef.h>
#include <stdbool.h>


#define LOGO_SCALE_X 3
#define LOGO_SCALE_Y 3
#define LOGO_BORDER  1

#if (((LOGO_BORDER * 2) > LOGO_SCALE_X) || ((LOGO_BORDER * 2) > LOGO_SCALE_Y))
#error "LOGO_BORDER can't be greater than half of LOGO_SCALE."
#endif

#include "gen/logo_top.h"
#include "gen/logo_bottom.h"

#define BITMAP_LOGO_WIDTH(x)  ((x) == 0 ? BITMAP_LOGO_TOP_WIDTH  : BITMAP_LOGO_BOTTOM_WIDTH )
#define BITMAP_LOGO_HEIGHT(x) ((x) == 0 ? BITMAP_LOGO_TOP_HEIGHT : BITMAP_LOGO_BOTTOM_HEIGHT)
#define BITMAP_LOGO_STRIDE(x) ((x) == 0 ? BITMAP_LOGO_TOP_STRIDE : BITMAP_LOGO_BOTTOM_STRIDE)
#define LOGO_SCALE_WIDTH(x)   ((BITMAP_LOGO_WIDTH(x)  + 2) * LOGO_SCALE_X)
#define LOGO_SCALE_HEIGHT(x)  ((BITMAP_LOGO_HEIGHT(x) + 2) * LOGO_SCALE_Y)

#define LOGO_BOTH_SCALE_HEIGHT ((BITMAP_LOGO_HEIGHT(0) + BITMAP_LOGO_HEIGHT(1) + 2) * LOGO_SCALE_Y)
#define LOGO_MAX_SCALE_WIDTH (LOGO_SCALE_WIDTH(0) > LOGO_SCALE_WIDTH(1) ? LOGO_SCALE_WIDTH(0) : LOGO_SCALE_WIDTH(1))

#define LOGO_TOP_OFFSET_Y    0
#define LOGO_BOTTOM_OFFSET_Y (BITMAP_LOGO_TOP_HEIGHT * LOGO_SCALE_Y)

#define LOGO_OFFSET_X(x) ((LOGO_MAX_SCALE_WIDTH - LOGO_SCALE_WIDTH(x)) / 2)
#define LOGO_OFFSET_Y(x) (x == 0 ? LOGO_TOP_OFFSET_Y : LOGO_BOTTOM_OFFSET_Y)

#define LOGO_BOTH_OFFSET_X ((GBFW_DISPLAY_WIDTH  - LOGO_MAX_SCALE_WIDTH  ) / 2)
#define LOGO_BOTH_OFFSET_Y ((GBFW_DISPLAY_HEIGHT - LOGO_BOTH_SCALE_HEIGHT) / 2)


#if (LOGO_MAX_SCALE_WIDTH > GBFW_DISPLAY_WIDTH)
#error "Logo exceeds display width."
#endif

#if (LOGO_BOTH_SCALE_HEIGHT > GBFW_DISPLAY_HEIGHT)
#error "Logo exceeds display height."
#endif


// Posible Values: 1, 3, 5, 15, 17, 51, 85, 255
#define FADE_RATE 15

#if ((255 % FADE_RATE) != 0)
#error "FADE_RATE must be a factor of 255."
#endif



// Neat approximation of iron color at each temperature in kelvin.
static inline rgb888_t color_from_temperature(int16_t temperature)
{
	rgb888_t c = RGB888_WHITE;
	if (temperature < 1280)
		c.b = (temperature >= 1024 ? (temperature - 1024) : 0);
	if (temperature < 1024)
		c.g = (temperature >=  768 ? (temperature -  768) : 0);
	if (temperature < 768)
		c.r = (temperature >=  512 ? (temperature -  512) : 0);

	return c;
}

static rxgb5155_t metal_color_from_temperature(int16_t temperature, rxgb5155_t back)
{
	rgb888_t c = color_from_temperature(temperature);
	return rxgb5155_max(back, RXGB5155(c.r, c.g, c.b));
}


#define GBFW_SPLASH_COLOR RXGB5155_GRAY(31)
#define GBFW_SPLASH_FPS 60
#define GBFW_SPLASH_TICKS (GBFW_TICK_RESOLUTION / GBFW_SPLASH_FPS)

static uint32_t gbfw_splash__tick_offset    = 0;
static uint8_t  gbfw_splash__fade           = 0;
static int16_t  gbfw_splash__temperature[2] = {0};
static uint16_t gbfw_splash__y[2] = {0};
static bool     gbfw_splash__complete = false;

static uint8_t noise(uint8_t x, uint8_t y)
{
	uint8_t h = (x * 251) + (y * 113);
	h = (h ^ ror(h, 3)) * 239;
	return h;
}

static void gbfw_splash__draw_logo(
	uint8_t piece, rgb565_t *buff, uint8_t size, uint8_t row, bool transparent)
{
	const uint8_t *bitmap_logo = (piece == 0 ? bitmap_logo_top : bitmap_logo_bottom);

	int16_t temperature = gbfw_splash__temperature[piece];

	const int16_t y  = row;
	const int16_t ys = (y / LOGO_SCALE_Y);
	const uint8_t ym = (y % LOGO_SCALE_Y);

	static const uint8_t mask_left   = (1 << 0);
	static const uint8_t mask_center = (1 << 1);
	static const uint8_t mask_right  = (1 << 2);

	const bool clip_above   = (ys <= 1);
	const bool clip_current = (ys == 0) || (ys > BITMAP_LOGO_HEIGHT(piece));
	const bool clip_below   = (ys >= BITMAP_LOGO_HEIGHT(piece));

	rxgb5155_t ignore = RXGB5155_MAGENTA;
	rxgb5155_t back   = RXGB5155_BLACK;
	rxgb5155_t fill   = GBFW_SPLASH_COLOR;
	rxgb5155_t dark   = rxgb5155_blend8(fill, back, 128);
	rxgb5155_t light  = rxgb5155_blend8(fill, RXGB5155_WHITE, 32);

	uint8_t exposure = (temperature + 136) / 6;
	fill  = rxgb5155_blend8(fill , back, exposure);
	dark  = rxgb5155_blend8(dark , back, exposure);
	light = rxgb5155_blend8(light, back, exposure);

	uint16_t mask_above   = 0;
	uint16_t mask_current = 0;
	uint16_t mask_below   = 0;

	if (!transparent)
	{
		rxgb5155_t *xbuff = (rxgb5155_t *)buff;
		for (uint8_t x = 0; x < LOGO_OFFSET_X(piece); x++)
			xbuff[x] = back;
	}
	buff += LOGO_OFFSET_X(piece);
	size -= LOGO_OFFSET_X(piece);

	for (uint8_t x = 0, bits = 2, byte = 0; x < size; bits--)
	{
		if ((bits < 3) && (byte < BITMAP_LOGO_STRIDE(piece)))
		{
			if (!clip_above  ) mask_above   |= bitmap_logo[((ys - 2) * BITMAP_LOGO_STRIDE(piece)) + byte] << bits;
			if (!clip_current) mask_current |= bitmap_logo[((ys - 1) * BITMAP_LOGO_STRIDE(piece)) + byte] << bits;
			if (!clip_below  ) mask_below   |= bitmap_logo[((ys + 0) * BITMAP_LOGO_STRIDE(piece)) + byte] << bits;
			byte++;
			bits += 8;
		}

		rxgb5155_t cl, cm, cr;
		uint8_t    tl, tm, tr;
		if (mask_current & mask_center)
		{
			cl = fill;
			cm = fill;
			cr = fill;

			if (ym == 0)
			{
				tl = 54;
				tm = 88;
				tr = 54;

				if (mask_current & mask_left)
					tl += 34;
				if (mask_current & mask_right)
					tr += 34;

				if (mask_above & mask_left)
					tl += 14;
				if (mask_above & mask_right)
					tr += 14;

				if (mask_above & mask_center)
				{
					tl += 34;
					tm += 48;
					tr += 34;
				}
			}
			else if (ym >= (LOGO_SCALE_Y - 1))
			{
				tl = 54;
				tm = 88;
				tr = 54;

				if (mask_current & mask_left)
					tl += 34;
				if (mask_current & mask_right)
					tr += 34;

				if (mask_below & mask_left)
					tl += 14;
				if (mask_below & mask_right)
					tr += 14;

				if (mask_below & mask_center)
				{
					tl += 34;
					tm += 48;
					tr += 34;
				}
			}
			else
			{
				tl = 88;
				tm = 136;
				tr = 88;

				if (mask_current & mask_left)
					tl += 48;

				if (mask_current & mask_right)
					tr += 48;
			}
		}
		else
		{
			cl = ignore;
			cm = ignore;
			cr = ignore;

			tl = 0;
			tm = 0;
			tr = 0;

			if (ym < LOGO_BORDER)
			{
				// Bottom border.
				if (mask_above & mask_center)
				{
					cl = dark;
					cm = dark;

					if (mask_current & mask_right)
						cr = fill;
					else
						cr = dark;

					tl += 17;
					tm += 24;
					tr += 17;
				}
				else
				{
					if ((mask_above & mask_left)
						|| (mask_current & mask_left))
						cl = dark;

					if (mask_current & mask_right)
						cr = light;
					else if (mask_above & mask_right)
						cr = fill;
				}

				if (mask_above & mask_left)
					tl += 7;
				if (mask_above & mask_right)
					tr += 7;

				if (mask_current & mask_left)
					tl += 17;
				if (mask_current & mask_right)
					tr += 17;
			}
			else if (ym >= (LOGO_SCALE_Y - LOGO_BORDER))
			{
				// Top border.
				if (mask_below & mask_center)
				{
					if (mask_current & mask_left)
						cl = fill;
					else
						cl = light;

					cm = light;
					cr = light;

					tl += 17;
					tm += 24;
					tr += 17;
				}
				else
				{
					if (mask_current & mask_left)
						cl = dark;
					else if (mask_below & mask_left)
						cl = fill;

					if ((mask_below & mask_right)
						|| (mask_current & mask_right))
						cr = light;
				}

				if (mask_below & mask_left)
					tl += 7;
				if (mask_below & mask_right)
					tr += 7;

				if (mask_current & mask_left)
					tl += 17;
				if (mask_current & mask_right)
					tr += 17;
			}
			else
			{
				// Left/right border.
				if (mask_current & mask_left)
					cl = dark;
				if (mask_current & mask_right)
					cr = light;

				if (mask_current & mask_left)
					tl += 24;
				if (mask_current & mask_right)
					tr += 24;
			}
		}

		int16_t atl = temperature + tl;
		int16_t atm = temperature + tm;
		int16_t atr = temperature + tr;

		rxgb5155_t *xbuff = (rxgb5155_t *)buff;
		for (uint8_t i = 0; i < LOGO_SCALE_X; i++, x++)
		{
			uint16_t   at;
			rxgb5155_t c;
			if (i < LOGO_BORDER)
			{
				at = atl;
				c  = cl;
			}
			else if (i < (LOGO_SCALE_X - LOGO_BORDER))
			{
				at = atm;
				c  = cm;
			}
			else
			{
				at = atr;
				c  = cr;
			}

			if (c.mask == ignore.mask)
			{
				if (!transparent)
					xbuff[x] = back;
				continue;
			}

			// Cooling noise effect.
			uint8_t lsw = LOGO_SCALE_WIDTH(piece);
			uint8_t lsh = LOGO_SCALE_HEIGHT(piece);
			bool nc[3] =
			{
				(noise((lsw + (lsw / 2)) + (x / 4), (lsh + (lsh / 2)) + (y / 4)) >> 7),
				(noise(lsw + (x / 2), lsh + (y / 2)) >> 7),
				(noise(x, y) >> 7),
			};

			uint8_t n = 0;
			for (uint8_t j = 0, a = 0; j < 3; j++)
			{
				n += nc[j] << a;
				a += nc[j];
			}
			at -= n * (at / 64);

			c = metal_color_from_temperature(at, c);
			xbuff[x] = c;
		}

		mask_above   >>= 1;
		mask_current >>= 1;
		mask_below   >>= 1;
	}
}

static void gbfw_splash__draw_callback(rgb565_t *buff, uint8_t size, uint8_t row)
{
	uint16_t bt = LOGO_BOTTOM_OFFSET_Y + (gbfw_splash__y[1] - gbfw_splash__y[0]);
	uint16_t bb = bt + LOGO_SCALE_HEIGHT(1);

	bool top    = (row < LOGO_SCALE_HEIGHT(0));
	bool bottom = (row >= bt) && (row < bb);

	if (top) gbfw_splash__draw_logo(0, buff, size, row, false);
	if (bottom) gbfw_splash__draw_logo(1, buff, size, (row - bt), top);

	if (!top && !bottom)
	{
		for (uint8_t x = 0; x < size; x++)
			buff[x] = RGB565_BLACK;
	}
}


static void gbfw_splash__tick(void)
{
	uint32_t tick = gbfw_tick_get() - gbfw_splash__tick_offset;

	static const uint32_t gbfw_splash__cooling = 1200000;
	static const uint32_t gbfw_splash__phase[3] =
	{
		 400000,
		 800000,
		2000000,
	};

	uint32_t atick = (gbfw_settings->primary.splash == GBFW_SETTINGS_SPLASH_STATIC
		? gbfw_splash__phase[2] : tick);

	if (atick < gbfw_splash__phase[0])
		gbfw_splash__y[0] = (gbfw_splash__phase[0] - atick) / (gbfw_splash__phase[0] / 100);
	else
		gbfw_splash__y[0] = 0;

	if (atick > gbfw_splash__cooling)
	{
		gbfw_splash__temperature[0] = 0;
	}
	else
	{
		uint32_t fract = ((gbfw_splash__cooling - atick) * 256U) / gbfw_splash__cooling;
		fract *= fract;
		gbfw_splash__temperature[0] = 512 + (fract / 85);
	}

	if (atick < gbfw_splash__phase[0])
	{
		gbfw_splash__y[1] = 100;
		gbfw_splash__temperature[1] = 1280;
	}
	else
	{
		if (atick < gbfw_splash__phase[1])
			gbfw_splash__y[1] = (gbfw_splash__phase[1] - atick) / ((gbfw_splash__phase[1] - gbfw_splash__phase[0]) / 100);
		else
			gbfw_splash__y[1] = 0;

		atick -= gbfw_splash__phase[0];
		if (atick > gbfw_splash__cooling)
		{
			gbfw_splash__temperature[1] = 0;
		}
		else
		{
			uint32_t fract = ((gbfw_splash__cooling - atick) * 256U) / gbfw_splash__cooling;
			if (fract > 255) fract = 255;
			fract *= fract;
			gbfw_splash__temperature[1] = 512 + (fract / 85);
		}
	}

	uint16_t top = LOGO_BOTH_OFFSET_Y + gbfw_splash__y[0];
	if (top < GBFW_DISPLAY_HEIGHT)
	{
		gbfw_display_render_region(
			LOGO_BOTH_OFFSET_X, top,
			LOGO_MAX_SCALE_WIDTH, (GBFW_DISPLAY_HEIGHT - top),
			gbfw_splash__draw_callback);
	}

	uint8_t fade_target = gbfw_settings_brightness_get();
	if (gbfw_splash__fade < fade_target)
	{
		gbfw_splash__fade += FADE_RATE;
		if (gbfw_splash__fade > fade_target)
			gbfw_splash__fade = fade_target;
		gbfw_display_backlight_set(gbfw_splash__fade);
	}

	if (tick < gbfw_splash__phase[2])
		gbfw_task_schedule(gbfw_splash__tick, GBFW_SPLASH_TICKS);
	else
		gbfw_splash__complete = true;
}

void gbfw_splash(void)
{
	gbfw_draw_rect(0, 0, GBFW_DISPLAY_WIDTH, GBFW_DISPLAY_HEIGHT, RGB565_BLACK);
	gbfw_splash__tick_offset = gbfw_tick_get();

	if ((gbfw_settings->primary.splash == GBFW_SETTINGS_SPLASH_DISABLED)
		|| gbfw_settings->primary.skip_splash)
	{
		// Deferring backlight fade in to second frame time to avoid flash.
		gbfw_delay_us(GBFW_SPLASH_TICKS);
		gbfw_display_backlight_set(gbfw_settings_brightness_get());
		gbfw_settings->primary.skip_splash = false;
		gbfw_splash__complete = true;
	}
	else
	{
		// Deferring backlight fade in to second frame time to avoid flash.
		gbfw_task_schedule(gbfw_splash__tick, GBFW_SPLASH_TICKS);
	}
}

bool gbfw_splash_is_complete(void)
{
	return gbfw_splash__complete;
}


typedef struct
{
	const char *str;
	uint8_t     len;
} gbfw_splash__string_t;

#define GBFW_SPLASH__STRING(x) (gbfw_splash__string_t){ (x), (sizeof(x) - 1) }

static const gbfw_splash__string_t gbfw_splash__string[] =
{
	GBFW_SPLASH__STRING("Connect Data Cable"),
	GBFW_SPLASH__STRING("PRESS [START] TO RETRY"),
	GBFW_SPLASH__STRING("FAILED TO READ FILE SYSTEM"),
	GBFW_SPLASH__STRING("FAILED TO LOAD BOOT.ELF"),
	GBFW_SPLASH__STRING("PRESS [START] TO EXIT"),
	GBFW_SPLASH__STRING("ENTERED RECOVERY MODE"),
};

typedef struct
{
	uint8_t string[3];
} gbfw_splash__text_t;

static const gbfw_splash__text_t gbfw_splash__text[] =
{
	{ .string = { 0, 2, 1 } },
	{ .string = { 0, 3, 1 } },
	{ .string = { 0, 5, 4 } },
};

#define GBFW_SPLASH__ERROR_Y 120

void gbfw_splash_error(gbfw_splash_error_e error)
{
	gbfw_status_set_error();
	wfi_while(!gbfw_splash_is_complete());

	// Redraw logo in-case we get an error when splash is disabled.
	gbfw_display_render_region(
		LOGO_BOTH_OFFSET_X, LOGO_BOTH_OFFSET_Y,
		LOGO_MAX_SCALE_WIDTH, LOGO_BOTH_SCALE_HEIGHT,
		gbfw_splash__draw_callback);

	uint8_t x = 0;
	uint8_t y = GBFW_SPLASH__ERROR_Y;
	uint8_t w = GBFW_DISPLAY_WIDTH;
	uint8_t h = (GBFW_DISPLAY_HEIGHT - y);

	rgb565_t back = RGB565_BLACK;
	rgb565_t fore = RGB565_WHITE;

	gbfw_draw_rect(x, y, w, h, back);

	for (unsigned i = 0; i < 3; i++)
	{
		gbfw_splash__string_t string	
			= gbfw_splash__string[gbfw_splash__text[error].string[i]];
		const gbfw_font_t *f = (i == 0
			? gbfw_font_default : gbfw_font_small);

		w = (f->width + f->hspace) * string.len;
		x = (GBFW_DISPLAY_WIDTH - w) / 2;

		if (string.str) gbfw_font_draw(f, x, y, string.len, string.str, fore, back);

		h = (f->height + f->vspace);
		y += h;
	}

	// Enable mass storage so boot.elf can be provided.
	gbfw_fs_stop();
	gbfw_usb_mass_storage_enable();

	while (true)
	{
		if (gbfw_control_poll().start)
		{
			while (gbfw_control_poll().start);
			break;
		}
	}

	gbfw_usb_mass_storage_disable();
}
