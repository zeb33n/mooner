#ifndef RGB888_H
#define RGB888_H

#include <stdint.h>

typedef struct
__attribute__((__packed__))
{
	uint8_t r, g, b;
} rgb888_t;

#define RGB888(red, green, blue) (rgb888_t){ .r = (red), .g = (green), .b = (blue) }
#define RGB888_GRAY(x) RGB888((x), (x), (x))

#define RGB888_BLACK       RGB888_GRAY(  0)
#define RGB888_GRAY_DARK   RGB888_GRAY( 63)
#define RGB888_GRAY_MEDIUM RGB888_GRAY(127)
#define RGB888_GRAY_LIGHT  RGB888_GRAY(191)
#define RGB888_WHITE       RGB888_GRAY(255)

#define RGB888_RED     RGB888(255,   0,   0)
#define RGB888_GREEN   RGB888(  0, 255,   0)
#define RGB888_BLUE    RGB888(  0,   0, 255)
#define RGB888_CYAN    RGB888(  0, 255, 255)
#define RGB888_MAGENTA RGB888(255,   0, 255)
#define RGB888_YELLOW  RGB888(255, 255,   0)


static inline rgb888_t rgb888_invert(rgb888_t a)
{
	return (rgb888_t){ .r = ~a.r, .g = ~a.g, .b = ~a.b };
}

static inline rgb888_t rgb888_blend8(rgb888_t a, rgb888_t b, uint8_t v)
{
	return (rgb888_t){
		.r = (((uint16_t)a.r * (256 - v)) + ((uint16_t)b.r * v)) / 256,
		.g = (((uint16_t)a.g * (256 - v)) + ((uint16_t)b.g * v)) / 256,
		.b = (((uint16_t)a.b * (256 - v)) + ((uint16_t)b.b * v)) / 256 };
}

static inline rgb888_t rgb888_max(rgb888_t a, rgb888_t b)
{
	return (rgb888_t){
		.r = (a.r > b.r ? a.r : b.r),
		.g = (a.g > b.g ? a.g : b.g),
		.b = (a.b > b.b ? a.b : b.b) };
}

static inline rgb888_t rgb888_adds(rgb888_t a, rgb888_t b)
{
	return (rgb888_t){
		.r = (((uint16_t)a.r + (uint16_t)b.r) >= 255 ? 255 : (a.r + b.r)),
		.g = (((uint16_t)a.g + (uint16_t)b.g) >= 255 ? 255 : (a.g + b.g)),
		.b = (((uint16_t)a.b + (uint16_t)b.b) >= 255 ? 255 : (a.b + b.b)) };
}

#endif
