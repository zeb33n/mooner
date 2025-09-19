#ifndef CM4_BITBAND_H
#define CM4_BITBAND_H

#include <stdint.h>

#define CM4_BITBAND_REGION_BIT(addr, bit) ((volatile uint32_t *)((((uintptr_t)(addr) & 0xFFF00000) + 0x02000000) + (((uintptr_t)(addr) & 0x000FFFFF) * 32) + ((bit) * 4)))

#endif
