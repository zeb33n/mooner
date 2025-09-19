#include "delay.h"
#include "clocks.h"
#include "irq.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/tim.h>
#include <hardware/stm32/irq.h>
#include <hardware/cm4/nvic.h>
#include <hardware/cm4/intrinsics.h>
#include <stdbool.h>


#define GBFW_DELAY_PSC_MS (GBFW_CLOCKS_DEFAULT_FREQ / 1000)
#define GBFW_DELAY_PSC_US (GBFW_DELAY_PSC_MS        / 1000)

#define GBFW_DELAY_TIMER            stm32_tim16
#define GBFW_DELAY_TIMER_RCC        stm32_rcc->apb2en.tim16
#define GBFW_DELAY_TIMER_IRQ        STM32_IRQ_TIM1_UP_TIM16
#define GBFW_DELAY_TIMER_IRQ_VECTOR stm32_vector_irq_tim1_up_tim16


void __attribute__((interrupt)) GBFW_DELAY_TIMER_IRQ_VECTOR(void)
{
	stm32_tim16->s.uif = false;

	// STM32 Errata 2.1.3
	dsb();
}


void gbfw_delay_init(void)
{
	GBFW_DELAY_TIMER_RCC      = true;
	GBFW_DELAY_TIMER->c1.opm  = true;
	GBFW_DELAY_TIMER->die.uie = true;

	cm4_nvic_interrupt_priority_set(GBFW_DELAY_TIMER_IRQ, GBFW_IRQ_PRIORITY_DELAY);
	cm4_nvic_interrupt_enable(GBFW_DELAY_TIMER_IRQ);
}

static void gbfw_delay_psc(uint16_t psc, uint16_t time)
{
	GBFW_DELAY_TIMER->psc = psc;
	GBFW_DELAY_TIMER->ar  = time;

	GBFW_DELAY_TIMER->eg.ug  = true;
	GBFW_DELAY_TIMER->s.mask = 0;

	GBFW_DELAY_TIMER->c1.cen = true;

	wfi_while(GBFW_DELAY_TIMER->c1.cen);
}

void gbfw_delay_us(uint16_t time)
{
	gbfw_delay_psc(GBFW_DELAY_PSC_US - 1, time);
}
