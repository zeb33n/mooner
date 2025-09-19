#ifndef GBFW_MEMCPY_H
#define GBFW_MEMCPY_H

#include <stddef.h>

void memcpy(void * restrict dst, const void * restrict src, size_t size);
void memset(void *dst, int value, size_t size);

#endif
