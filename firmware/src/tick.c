#include "tick.h"
#include "clocks.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/tim.h>
#include <hardware/stm32/dbg.h>
#include <stdbool.h>


#define GBFW_TICK_TIMER          stm32_tim5
#define GBFW_TICK_TIMER_RCC      stm32_rcc->apb1en1.tim5
#define GBFW_TICK_TIMER_DEBUG_FZ stm32_dbg->apb1_fz1.dbg_tim5_stop


void gbfw_tick_init(void)
{
	GBFW_TICK_TIMER_RCC  = true;
	GBFW_TICK_TIMER->psc = (GBFW_CLOCKS_DEFAULT_FREQ / GBFW_TICK_RESOLUTION) - 1;
	GBFW_TICK_TIMER->ar  = 0xFFFFFFFFUL;

	GBFW_TICK_TIMER->eg.ug  = true;
	GBFW_TICK_TIMER->s.mask = 0;

#ifndef NDEBUG
	// Make tick timer stop during breakpoints.
	GBFW_TICK_TIMER_DEBUG_FZ = true;
#endif

	GBFW_TICK_TIMER->c1.cen = true;
}

uint32_t gbfw_tick_get(void)
{
	return GBFW_TICK_TIMER->cnt.mask;
}
