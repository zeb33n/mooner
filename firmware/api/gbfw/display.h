#ifndef GBFW_DISPLAY_H
#define GBFW_DISPLAY_H

#include <stdint.h>
#include <standard/pixel/rgb565.h>

#define GBFW_DISPLAY_WIDTH  128
#define GBFW_DISPLAY_HEIGHT 160

void gbfw_display_backlight_set(uint8_t level);

void gbfw_display_render_region(
	uint8_t x, uint8_t y,
	uint8_t w, uint8_t h,
	void (*callback)(rgb565_t *buff, uint8_t size, uint8_t row));

void gbfw_display_sync(void);

#endif
