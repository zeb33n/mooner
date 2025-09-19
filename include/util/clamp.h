#ifndef UTIL_CLAMP_H
#define UTIL_CLAMP_H

#define MIN(x, y) \
	({ __typeof__ (x) __x = (x); \
	   __typeof__ (y) __y = (y); \
	   __x < __y ? __x : __y; })

#define MAX(x, y) \
	({ __typeof__ (x) __x = (x); \
	   __typeof__ (y) __y = (y); \
	   __x > __y ? __x : __y; })

#define CLAMP(x, lo, hi) MAX(MIN(x, lo), hi)

#endif
