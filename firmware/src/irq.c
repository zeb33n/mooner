#include "irq.h"

#include <hardware/stm32/syscfg.h>
#include <hardware/stm32/exti.h>
#include <hardware/cm4/nvic.h>
#include <hardware/cm4/intrinsics.h>


void __attribute__((weak)) stm32_vector_irq_exti_5(void);
void __attribute__((weak)) stm32_vector_irq_exti_6(void);
void __attribute__((weak)) stm32_vector_irq_exti_7(void);
void __attribute__((weak)) stm32_vector_irq_exti_8(void);
void __attribute__((weak)) stm32_vector_irq_exti_9(void);

void __attribute__((weak)) stm32_vector_irq_exti_10(void);
void __attribute__((weak)) stm32_vector_irq_exti_11(void);
void __attribute__((weak)) stm32_vector_irq_exti_12(void);
void __attribute__((weak)) stm32_vector_irq_exti_13(void);
void __attribute__((weak)) stm32_vector_irq_exti_14(void);
void __attribute__((weak)) stm32_vector_irq_exti_15(void);


#define IRQ_CHECK_TRIGGER(x) \
	do { \
		if ((stm32_exti->p1 & (1U << x)) \
			&& (stm32_exti->im1 & (1U << x))) \
			stm32_vector_irq_exti_##x(); \
	} while (false)

void __attribute__((interrupt)) stm32_vector_irq_exti9_5(void)
{
	IRQ_CHECK_TRIGGER(5);
	IRQ_CHECK_TRIGGER(6);
	IRQ_CHECK_TRIGGER(7);
	IRQ_CHECK_TRIGGER(8);
	IRQ_CHECK_TRIGGER(9);

	// STM32 Errata 2.1.3
	dsb();
}

void __attribute__((interrupt)) stm32_vector_irq_exti15_10(void)
{
	IRQ_CHECK_TRIGGER(10);
	IRQ_CHECK_TRIGGER(11);
	IRQ_CHECK_TRIGGER(12);
	IRQ_CHECK_TRIGGER(13);
	IRQ_CHECK_TRIGGER(14);
	IRQ_CHECK_TRIGGER(15);

	// STM32 Errata 2.1.3
	dsb();
}


void gbfw_irq_enable(uint8_t irq, bool rising, bool falling, uint8_t priority)
{
	if (rising)
		stm32_exti_rts_set(irq);
	else
		stm32_exti_rts_clear(irq);

	if (falling)
		stm32_exti_fts_set(irq);
	else
		stm32_exti_fts_clear(irq);

	if (rising || falling)
	{
		stm32_exti_im_set(irq);

		if (irq > 15) return;

		uint8_t nvic_irq;
		bool unique = true;
		if (irq >= 10)
		{
			nvic_irq = STM32_IRQ_EXTI15_10;
			uint16_t mask = (stm32_exti->im1 & 0xFC00);
			unique = ((mask & (mask - 1)) == 0);
		}
		else if (irq >= 5)
		{
			nvic_irq = STM32_IRQ_EXTI9_5;
			uint16_t mask = (stm32_exti->im1 & 0x03E0);
			unique = ((mask & (mask - 1)) == 0);
		}
		else
		{
			nvic_irq = STM32_IRQ_EXTI_0 + irq;
		}

		uint8_t nvic_priority = priority;
		if (!unique)
		{
			uint8_t p = cm4_nvic_interrupt_priority_get(nvic_irq);
			if (p < nvic_priority)
				nvic_priority = p;
		}

		cm4_nvic_interrupt_enable(nvic_irq);
		cm4_nvic_interrupt_priority_set(nvic_irq, nvic_priority);
	}
	else
	{
		stm32_exti_im_clear(irq);

		if (irq > 15) return;

		uint8_t nvic_irq;
		bool disable = true;
		if (irq >= 10)
		{
			disable = ((stm32_exti->im1 & 0xFC00) == 0);
			nvic_irq = STM32_IRQ_EXTI15_10;
		}
		else if (irq >= 5)
		{
			disable = ((stm32_exti->im1 & 0x03E0) == 0);
			nvic_irq = STM32_IRQ_EXTI9_5;
		}
		else
		{
			nvic_irq = STM32_IRQ_EXTI_0 + irq;
		}

		if (disable) cm4_nvic_interrupt_disable(nvic_irq);
	}
}
