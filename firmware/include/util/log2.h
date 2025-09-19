#ifndef UTIL_LOG2_H
#define UTIL_LOG2_H

#include <stdbool.h>
#include <stdint.h>


static inline bool is_pow2(uint32_t a)
{
	return ((a & (a - 1)) == 0);
}

static inline uint8_t log2(uint32_t a)
{
	return (31 - __builtin_clz(a));
}

#endif
