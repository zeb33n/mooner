#include "random.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/rng.h>
#include <stdbool.h>


void gbfw_random_init(void)
{
	stm32_rcc->ahb2en.rng = true;
	stm32_rng->c.rngen    = true;
}

uint32_t gbfw_random_u32(void)
{
	// A rare error can cause the value zero to appear too often.
	uint32_t v = 0;
	while (v == 0)
	{
		while (!stm32_rng->s.drdy);
		v = stm32_rng->d;
	}
	return v;	
}
