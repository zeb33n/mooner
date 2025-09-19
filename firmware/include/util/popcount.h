#ifndef UTIL_POPCOUNT_H
#define UTIL_POPCOUNT_H

#include <stdint.h>


static inline uint8_t popcount8(uint8_t a)
{
	a = (a & 0b01010101) + ((a >> 1) & 0b01010101);
	a = (a & 0b00110011) + ((a >> 2) & 0b00110011);
	a = (a & 0b00001111) +  (a >> 4)              ;
	return a;
}

static inline uint8_t popcount16(uint16_t a)
{
	a = (a & 0b0101010101010101) + ((a >> 1) & 0b0101010101010101);
	a = (a & 0b0011001100110011) + ((a >> 2) & 0b0011001100110011);
	a = (a & 0b0000111100001111) + ((a >> 4) & 0b0000111100001111);
	a = (a & 0b0000000011111111) +  (a >> 8)                      ;
	return a;
}

static inline uint8_t popcount32(uint32_t a)
{
	a = (a & 0x55555555) + ((a >>  1) & 0x55555555);
	a = (a & 0x33333333) + ((a >>  2) & 0x33333333);
	a = (a & 0x0F0F0F0F) + ((a >>  4) & 0x0F0F0F0F);
	a = (a & 0x00FF00FF) + ((a >>  8) & 0x00FF00FF);
	a = (a & 0x0000FFFF) +  (a >> 16)              ;
	return a;
}

static inline uint8_t popcount64(uint64_t a)
{
	a = (a & 0x5555555555555555ULL) + ((a >>  1) & 0x5555555555555555ULL);
	a = (a & 0x3333333333333333ULL) + ((a >>  2) & 0x3333333333333333ULL);
	a = (a & 0x0F0F0F0F0F0F0F0FULL) + ((a >>  4) & 0x0F0F0F0F0F0F0F0FULL);
	a = (a & 0x00FF00FF00FF00FFULL) + ((a >>  8) & 0x00FF00FF00FF00FFULL);
	a = (a & 0x0000FFFF0000FFFFULL) + ((a >> 16) & 0x0000FFFF0000FFFFULL);
	a = (a & 0x00000000FFFFFFFFULL) +  (a >> 32)                         ;
	return a;
}

#endif
