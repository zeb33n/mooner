#include <gbfw/cordic.h>
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/cordic.h>
#include <stdbool.h>


static const stm32_cordic_cs_t gbfw_cordic_func__cs[] =
{
	[GBFW_CORDIC_FUNC_SIN32] =
	{
		.func      = STM32_CORDIC_FUNC_SINE,
		.precision = 6,
		.scale     = 0,
		.nres      = 0,
		.nargs     = 0,
		.ressize   = 0,
		.argsize   = 0,
	},

	[GBFW_CORDIC_FUNC_SIN32_FAST] =
	{
		.func      = STM32_CORDIC_FUNC_SINE,
		.precision = 4,
		.scale     = 0,
		.nres      = 0,
		.nargs     = 0,
		.ressize   = 0,
		.argsize   = 0,
	},
};

void gbfw_cordic_begin(gbfw_cordic_func_e func)
{
	stm32_rcc->ahb1en.cordic = true;
	stm32_cordic->cs.mask = gbfw_cordic_func__cs[func].mask;
}

void gbfw_cordic_end(void)
{
	stm32_rcc->ahb1en.cordic = false;
}

uint32_t gbfw_cordic_process_simple(uint32_t arg)
{
	stm32_cordic->wdata = arg;
	return stm32_cordic->rdata;
}
