#ifndef CM4_SYSTICK_H
#define CM4_SYSTICK_H

#include <stdint.h>


#define CM4_SYSTICK_CLKSOURCE_REFERENCE 0
#define CM4_SYSTICK_CLKSOURCE_PROCESSOR 1

typedef union
{
	struct
	{
		uint32_t enable    :  1;
		uint32_t tickint   :  1;
		uint32_t clksource :  1;
		uint32_t res15_3   : 13;
		uint32_t countflag :  1;
		uint32_t res31_17  : 15;
	};

	uint32_t mask;
} cm4_systick_cs_t;

typedef union
{
	struct
	{
		uint32_t tenms    : 24;
		uint32_t res29_24 :  6;
		uint32_t skew     :  1;
		uint32_t noref    :  1;
	};

	uint32_t mask;
} cm4_systick_calib_t;

typedef struct
{
	cm4_systick_cs_t          cs;
	uint32_t                  rv;
	uint32_t                  cv;
	const cm4_systick_calib_t calib;
} cm4_systick_t;


static volatile cm4_systick_t * const cm4_systick
	= (volatile cm4_systick_t *)0xE000E010;

#endif
