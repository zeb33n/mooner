#include "task.h"
#include "clocks.h"
#include "irq.h"
#include <gbfw/tick.h>
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/tim.h>
#include <hardware/stm32/irq.h>
#include <hardware/cm4/nvic.h>
#include <hardware/cm4/intrinsics.h>
#include <stddef.h>
#include <stdbool.h>


#define GBFW_TASK_TIMER            stm32_tim17
#define GBFW_TASK_TIMER_RCC        stm32_rcc->apb2en.tim17
#define GBFW_TASK_TIMER_IRQ        STM32_IRQ_TIM1_TRG_COM_DIR_IDX_TIM17
#define GBFW_TASK_TIMER_IRQ_VECTOR stm32_vector_irq_tim1_trg_com_dir_idx_tim17


typedef struct
{
	void   (*func);
	uint32_t ticks;
} gbfw_task_t;

#define GBFW_TASK_COUNT 8

static volatile gbfw_task_t gbfw_task[GBFW_TASK_COUNT]       = {0};
static volatile uint8_t     gbfw_task_order[GBFW_TASK_COUNT] = {0};
static volatile uint8_t     gbfw_task_next                   =  0 ;
static volatile uint32_t    gbfw_task_last_tick              =  0 ;


static void gbfw_task_reschedule(uint32_t ticks)
{
	GBFW_TASK_TIMER->c1.cen  = false;
	GBFW_TASK_TIMER->c1.udis = true;

	GBFW_TASK_TIMER->ar = (ticks > 0xFFFF ? 0xFFFF : ticks);

	GBFW_TASK_TIMER->eg.ug  = true;
	GBFW_TASK_TIMER->s.mask = 0;

	GBFW_TASK_TIMER->c1.udis = false;
	GBFW_TASK_TIMER->c1.cen  = true;
}


void __attribute__((interrupt)) GBFW_TASK_TIMER_IRQ_VECTOR(void)
{
	GBFW_TASK_TIMER->s.uif = false;

	void (*func_list[GBFW_TASK_COUNT])(void) = {0};
	uint8_t func_count = 0;

	// Execute scheduled functions in order.
	bool exec = true;
	while (gbfw_task[gbfw_task_next].func && exec)
	{
		// Make atomic to block reschedules.
		cpsid(i);

		uint32_t delta = gbfw_tick_get() - gbfw_task_last_tick;

		exec = (gbfw_task[gbfw_task_next].ticks <= delta);
		if (exec)
		{
			delta = gbfw_task[gbfw_task_next].ticks;
			func_list[func_count++] = gbfw_task[gbfw_task_next].func;
			gbfw_task[gbfw_task_next].func = NULL;
			gbfw_task_next = gbfw_task_order[gbfw_task_next];
		}

		uint8_t t = gbfw_task_next;
		while (gbfw_task[t].func)
		{
			gbfw_task[t].ticks -= delta;
			if (gbfw_task_order[t] == t) break;
			t = gbfw_task_order[t];
		}

		gbfw_task_last_tick += delta;

		cpsie(i);
	}

	// Only reschedule if we have a task and nothing is scheduled.
	cpsid(i);
	if (gbfw_task[gbfw_task_next].func
		&& !GBFW_TASK_TIMER->c1.cen)
		gbfw_task_reschedule(gbfw_task[gbfw_task_next].ticks);
	cpsie(i);

	for (uint8_t i = 0; i < func_count; i++)
		func_list[i]();

	// STM32 Errata 2.1.3
	dsb();
}


void gbfw_task_init(void)
{
	GBFW_TASK_TIMER_RCC      = true;
	GBFW_TASK_TIMER->c1.opm  = true;
	GBFW_TASK_TIMER->die.uie = true;
	GBFW_TASK_TIMER->psc     = (GBFW_CLOCKS_DEFAULT_FREQ / GBFW_TICK_RESOLUTION) - 1;

	gbfw_task_last_tick = gbfw_tick_get();

	cm4_nvic_interrupt_priority_set(GBFW_TASK_TIMER_IRQ, GBFW_IRQ_PRIORITY_TASK);
	cm4_nvic_interrupt_enable(GBFW_TASK_TIMER_IRQ);
}

bool gbfw_task_schedule(void (*func)(void), uint32_t ticks)
{
	uint32_t mticks;
	// Handle modified ticks overflow.
	do
	{
		mticks = ticks + (gbfw_tick_get() - gbfw_task_last_tick);
	} while (mticks < ticks);

	// Make atomic to avoid conflicts.
	cpsid(i);

	uint8_t i;
	for (i = 0; (i < GBFW_TASK_COUNT) && gbfw_task[i].func; i++);
	if (i >= GBFW_TASK_COUNT)
	{
		cpsie(i);
		return false;
	}

	// Re-read ticks while atomic.
	mticks = ticks + (gbfw_tick_get() - gbfw_task_last_tick);

	if (!gbfw_task[gbfw_task_next].func)
	{
		gbfw_task_order[i] = i;
		gbfw_task_next = i;
		gbfw_task_reschedule(ticks);
	}
	else if (mticks < gbfw_task[gbfw_task_next].ticks)
	{
		gbfw_task_order[i] = gbfw_task_next;
		gbfw_task_next = i;
		gbfw_task_reschedule(ticks);
	}
	else
	{
		uint8_t prev = gbfw_task_next;
		while (true)
		{
			uint8_t curr = gbfw_task_order[prev];
			if (mticks < gbfw_task[curr].ticks)
			{
				gbfw_task_order[i]    = curr;
				gbfw_task_order[prev] = i;
				break;
			}
			else if (gbfw_task_order[curr] == curr)
			{
				gbfw_task_order[curr] = i;
				gbfw_task_order[i]    = i;
				break;
			}

			prev = curr;
		}
	}

	gbfw_task[i].ticks = mticks;
	gbfw_task[i].func  = func;

	cpsie(i);

	return true;
}
