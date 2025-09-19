#ifndef CM4_AUX_H
#define CM4_AUX_H

#include <stdint.h>


typedef union
{
	struct
	{
		uint32_t disfold    :  1;
		uint32_t disdefwbuf :  1;
		uint32_t dismcycint :  1;
		uint32_t res7_3     :  5;
		uint32_t disoofp    :  1;
		uint32_t disfpca    :  1;
		uint32_t res31_10   : 22;
	};

	uint32_t mask;
} cm4_aux_ctl_t;


static volatile cm4_aux_ctl_t * const cm4_aux_ctl
	= (volatile cm4_aux_ctl_t *)0xE000E008;

#endif
