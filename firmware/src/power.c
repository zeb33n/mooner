#include "power.h"
#include "irq.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/gpio.h>
#include <hardware/stm32/syscfg.h>
#include <hardware/stm32/irq.h>
#include <hardware/stm32/exti.h>
#include <hardware/stm32/rtc.h>
#include <hardware/stm32/tamp.h>
#include <hardware/cm4/scb.h>
#include <hardware/cm4/nvic.h>
#include <hardware/cm4/intrinsics.h>


#define GBFW_POWER_SWITCH_PORT       STM32_GPIO_PORT_A
#define GBFW_POWER_SWITCH_PIN        10
#define GBFW_POWER_SWITCH_IRQ_VECTOR stm32_vector_irq_exti_10

#if (REVISION <= 6)
#define GBFW_POWER_HOLD_PORT         STM32_GPIO_PORT_B
#define GBFW_POWER_HOLD_PIN          9
#else
#define GBFW_POWER_HOLD_PORT         STM32_GPIO_PORT_C
#define GBFW_POWER_HOLD_PIN          13
#define GBFW_POWER_HOLD_RTC_OUT
#endif

#if (REVISION >= 8)
#define GBFW_USB_DETECT_TAMP 2
#endif


typedef union
{
	struct
	{
		uint8_t hold     : 7;
		uint8_t shutdown : 1;
	};

	uint8_t mask;
} gbfw_power_state_t;

static gbfw_power_state_t gbfw_power__state = { .hold = 0, .shutdown = false };


void gbfw_power_init(void)
{
#ifdef GBFW_POWER_HOLD_RTC_OUT
#ifdef GBFW_USB_DETECT_TAMP
	stm32_tamp->fltc.tampudis = true;
	stm32_tamp->c2.tamp2noer  = true;
	stm32_tamp->c1.tamp2      = true;
#endif

	stm32_rtc->c.tampoe = true;
	stm32_gpio_pin_set(GBFW_POWER_HOLD_PORT, GBFW_POWER_HOLD_PIN);
#else
	stm32_gpio_pin_clear(GBFW_POWER_HOLD_PORT, GBFW_POWER_HOLD_PIN);
	stm32_gpio_pin_mode_set(GBFW_POWER_HOLD_PORT, GBFW_POWER_HOLD_PIN, STM32_GPIO_MODE_OUTPUT);
#endif

	stm32_gpio_pin_mode_set(GBFW_POWER_SWITCH_PORT, GBFW_POWER_SWITCH_PIN, STM32_GPIO_MODE_INPUT);
	stm32_gpio_pin_lock(GBFW_POWER_SWITCH_PORT, GBFW_POWER_SWITCH_PIN);

	stm32_syscfg_exti_port_set(GBFW_POWER_SWITCH_PIN, GBFW_POWER_SWITCH_PORT);
	gbfw_irq_enable(GBFW_POWER_SWITCH_PIN, true, true, GBFW_IRQ_PRIORITY_POWER_SWITCH);
}


static void gbfw_power__shutdown_atomic(void)
{
	cpsid(if);
	if (gbfw_power__state.hold == 0)
		cm4_system_reset();
	gbfw_power__state.shutdown = true;
	cpsie(if);
}

void __attribute__((interrupt)) GBFW_POWER_SWITCH_IRQ_VECTOR(void)
{
	stm32_exti_p_clear(GBFW_POWER_SWITCH_PIN);

	if (!gbfw_power_is_on())
		gbfw_power__shutdown_atomic();

	// STM32 Errata 2.1.3
	dsb();
}

_Noreturn void gbfw_power_shutdown(void)
{
	gbfw_power__shutdown_atomic();
	while (true);
}


bool gbfw_power_is_on(void)
{
	return stm32_gpio_pin_read(GBFW_POWER_SWITCH_PORT, GBFW_POWER_SWITCH_PIN);
}


bool gbfw_power_hold(void)
{
	if (gbfw_power__state.hold == 0)
	{
	#ifdef GBFW_POWER_HOLD_RTC_OUT
		stm32_gpio_pin_mode_set(GBFW_POWER_HOLD_PORT, GBFW_POWER_HOLD_PIN, STM32_GPIO_MODE_OUTPUT);
		stm32_rtc->c.tampoe = false;
	#else
		stm32_gpio_pin_set(GBFW_POWER_HOLD_PORT, GBFW_POWER_HOLD_PIN);
	#endif
	}
	else if (gbfw_power__state.hold == 0x7F)
	{
		return false;
	}
	gbfw_power__state.hold++;
	return true;
}

void gbfw_power_release(void)
{
	if (gbfw_power__state.hold == 0)
		return;

	gbfw_power__state.hold--;
	if (gbfw_power__state.hold == 0)
	{
	#ifdef GBFW_POWER_HOLD_RTC_OUT
		stm32_rtc->c.tampoe = true;
		stm32_gpio_pin_mode_set(GBFW_POWER_HOLD_PORT, GBFW_POWER_HOLD_PIN, STM32_GPIO_MODE_ANALOG);
	#else
		stm32_gpio_pin_clear(GBFW_POWER_HOLD_PORT, GBFW_POWER_HOLD_PIN);
	#endif

		if (gbfw_power__state.shutdown || !gbfw_power_is_on())
			cm4_system_reset();
	}
}
