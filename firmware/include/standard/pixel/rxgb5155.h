#ifndef RXGB5155_H
#define RXGB5155_H

#include <stdint.h>

typedef union
{
	struct
	{
		uint16_t r : 5;
		uint16_t x : 1;
		uint16_t g : 5;
		uint16_t b : 5;
	};

	uint16_t mask;
} rxgb5155_t;

#define RXGB5155(red, green, blue) (rxgb5155_t){ .r = ((red) >> 3), .x = (green & 1), .g = ((green) >> 3), .b = ((blue) >> 3) }
#define RXGB5155_GRAY(x) RXGB5155((x), (x), (x))

#define RXGB5155_BLACK       RXGB5155_GRAY(  0)
#define RXGB5155_GRAY_DARK   RXGB5155_GRAY( 63)
#define RXGB5155_GRAY_MEDIUM RXGB5155_GRAY(127)
#define RXGB5155_GRAY_LIGHT  RXGB5155_GRAY(191)
#define RXGB5155_WHITE       RXGB5155_GRAY(255)

#define RXGB5155_RED     RXGB5155(255,   0,   0)
#define RXGB5155_GREEN   RXGB5155(  0, 255,   0)
#define RXGB5155_BLUE    RXGB5155(  0,   0, 255)
#define RXGB5155_CYAN    RXGB5155(  0, 255, 255)
#define RXGB5155_MAGENTA RXGB5155(255,   0, 255)
#define RXGB5155_YELLOW  RXGB5155(255, 255,   0)


static inline rxgb5155_t rxgb5155_invert(rxgb5155_t a)
{
	return (rxgb5155_t){ .mask = ~a.mask };
}

static inline rxgb5155_t rxgb5155_blend8(rxgb5155_t a, rxgb5155_t b, uint8_t v)
{
	uint8_t g = (((uint16_t)a.g * (256 - v)) + ((uint16_t)b.g * v)) / 256;
	return (rxgb5155_t){
		.r = (((uint16_t)a.r * (256 - v)) + ((uint16_t)b.r * v)) / 256,
		.x = (g & 1),
		.g = g,
		.b = (((uint16_t)a.b * (256 - v)) + ((uint16_t)b.b * v)) / 256 };
}

static inline rxgb5155_t rxgb5155_max(rxgb5155_t a, rxgb5155_t b)
{
	uint8_t g = (a.g > b.g ? a.g : b.g);
	return (rxgb5155_t){
		.r = (a.r > b.r ? a.r : b.r),
		.x = (g & 1),
		.g = g,
		.b = (a.b > b.b ? a.b : b.b) };
}

static inline rxgb5155_t rxgb5155_adds(rxgb5155_t a, rxgb5155_t b)
{
	uint8_t g = ((a.g + b.g) >= 31 ? 31 : (a.g + b.g));
	return (rxgb5155_t){
		.r = ((a.r + b.r) >= 31 ? 31 : (a.r + b.r)),
		.x = (g & 1),
		.g = g,
		.b = ((a.b + b.b) >= 31 ? 31 : (a.b + b.b)) };
}

#endif
