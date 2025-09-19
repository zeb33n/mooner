#ifndef CM4_COPROC_H
#define CM4_COPROC_H

#include <stdint.h>


typedef enum
{
	CM4_COPROC_ACCESS_DENIED     = 0,
	CM4_COPROC_ACCESS_PRIVILEGED = 1,
	CM4_COPROC_ACCESS_FULL       = 3,
} cm4_coproc_access_e;


static volatile uint32_t * const cm4_coproc_access
	= (volatile cm4_coproc_ac_t *)0xE000ED88;


static inline void cm4_coproc_fpu_access_set(cm4_coproc_access_e access)
{
	uint32_t mask = *cm4_coproc_access;
	mask &= ~(0x7 << (10 * 2));
	mask |= (access << (10 * 2));
	mask &= ~(0x3 << (11 * 2));
	mask |= (access << (11 * 2));
	*cm4_coproc_access = mask;
}

#endif
