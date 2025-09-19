#ifndef CM4_FPU_H
#define CM4_FPU_H

#include <stdint.h>


typedef union
{
	struct
	{
		uint32_t lspact  :  1;
		uint32_t user    :  1;
		uint32_t res2    :  1;
		uint32_t thread  :  1;
		uint32_t hfrdy   :  1;
		uint32_t mmrdy   :  1;
		uint32_t bfrdy   :  1;
		uint32_t res7    :  7;
		uint32_t monrdy  :  1;
		uint32_t res29_9 : 21;
		uint32_t lspen   :  1;
		uint32_t aspen   :  1;
	};

	uint32_t mask;
} cm4_fpu_cc_t;

typedef enum
{
	CM4_FPU_RMODE_NEAREST        = 0,
	CM4_FPU_RMODE_PLUS_INFINITY  = 1,
	CM4_FPU_RMODE_MINUS_INFINITY = 2,
	CM4_FPU_RMODE_ZERO           = 3,
} cm4_fpu_rmode_e;

typedef union
{
	struct
	{
		uint32_t ioc     :  1;
		uint32_t dzc     :  1;
		uint32_t ofc     :  1;
		uint32_t ufc     :  1;
		uint32_t ixc     :  1;
		uint32_t res6_5  :  2;
		uint32_t idc     :  1;
		uint32_t res21_8 : 14;
		uint32_t rmode   :  2;
		uint32_t fz      :  1;
		uint32_t dn      :  1;
		uint32_t ahp     :  1;
		uint32_t res27   :  1;
		uint32_t v       :  1;
		uint32_t c       :  1;
		uint32_t z       :  1;
		uint32_t n       :  1;
	};

	uint32_t mask;
} cm4_fpu_sc_t;


typedef struct
{
	cm4_fpu_cc_t cc;
	uint32_t     ca;
	cm4_fpu_sc_t sc;
	cm4_fpu_sc_t dsc;
} cm4_fpu_t;


static volatile cm4_fpu_t * const cm4_fpu
	= (volatile cm4_fpu_t *)0xE000EF34;

#endif
