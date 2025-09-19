#ifndef UTIL_RAND_H
#define UTIL_RAND_H

#include <stdint.h>

static inline uint32_t rand32(uint32_t seed)
{
	return (seed * 1664525U) + 1013904223U;
}

#endif
