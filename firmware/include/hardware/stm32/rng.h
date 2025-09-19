#ifndef STM32_RNG_H
#define STM32_RNG_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef union
{
	struct
	{
		uint32_t res1_0  :  2;
		uint32_t rngen   :  1;
		uint32_t ie      :  1;
		uint32_t res4    :  1;
		uint32_t ced     :  1;
		uint32_t res31_6 : 26;
	};

	uint32_t mask;
} stm32_rng_c_t;

typedef union
{
	struct
	{
		uint32_t drdy    :  1;
		uint32_t cecs    :  1;
		uint32_t secs    :  1;
		uint32_t res4_3  :  2;
		uint32_t ceis    :  1;
		uint32_t seis    :  1;
		uint32_t res31_7 : 25;
	};

	uint32_t mask;
} stm32_rng_s_t;


typedef struct
{
	stm32_rng_c_t c;
	stm32_rng_s_t s;
	uint32_t      d;
	uint32_t      res255_3[253];
} stm32_rng_t;

static volatile stm32_rng_t * const stm32_rng = (volatile stm32_rng_t *)STM32_MMAP_RNG;

#endif
