#include "control.h"
#include "irq.h"
#include "home.h"
#include "load.h"
#include "settings.h"
#include <hardware/stm32/gpio.h>
#include <hardware/stm32/syscfg.h>
#include <hardware/stm32/irq.h>
#include <hardware/stm32/exti.h>
#include <hardware/cm4/nvic.h>
#include <hardware/cm4/intrinsics.h>


#define GBFW_CONTROL_SELECT_IRQ stm32_vector_irq_exti_3



typedef struct
{
	unsigned pin  : 4;
	unsigned port : 4;
} gbfw_control__port_pin_t;

static const gbfw_control__port_pin_t gbfw_control__port_pin[GBFW_CONTROL_COUNT] =
{
	[GBFW_CONTROL_UP    ] = { .port = STM32_GPIO_PORT_B, .pin =  1 },
	[GBFW_CONTROL_DOWN  ] = { .port = STM32_GPIO_PORT_A, .pin =  6 },
	[GBFW_CONTROL_LEFT  ] = { .port = STM32_GPIO_PORT_B, .pin =  0 },
	[GBFW_CONTROL_RIGHT ] = { .port = STM32_GPIO_PORT_A, .pin =  7 },
#if (REVISION == 5)
	[GBFW_CONTROL_Y     ] = { .port = STM32_GPIO_PORT_F, .pin =  0 },
#else
	[GBFW_CONTROL_Y     ] = { .port = STM32_GPIO_PORT_B, .pin = 11 },
#endif
#if (REVISION <= 7)
	[GBFW_CONTROL_A     ] = { .port = STM32_GPIO_PORT_A, .pin =  0 },
#else
	[GBFW_CONTROL_A     ] = { .port = STM32_GPIO_PORT_F, .pin =  0 },
#endif
#if (REVISION <= 6)
	[GBFW_CONTROL_X     ] = { .port = STM32_GPIO_PORT_C, .pin = 13 },
#else
	[GBFW_CONTROL_X     ] = { .port = STM32_GPIO_PORT_B, .pin =  9 },
#endif
	[GBFW_CONTROL_B     ] = { .port = STM32_GPIO_PORT_F, .pin =  1 },
	[GBFW_CONTROL_START ] = { .port = STM32_GPIO_PORT_A, .pin =  1 },
#if (REVISION == 5)
	[GBFW_CONTROL_SELECT] = { .port = STM32_GPIO_PORT_A, .pin =  2 },
#else
	[GBFW_CONTROL_SELECT] = { .port = STM32_GPIO_PORT_A, .pin =  3 },
#endif
	[GBFW_CONTROL_L1    ] = { .port = STM32_GPIO_PORT_B, .pin =  2 },
#if (REVISION == 5)
	[GBFW_CONTROL_R1    ] = { .port = STM32_GPIO_PORT_A, .pin =  9 },
#else
	[GBFW_CONTROL_R1    ] = { .port = STM32_GPIO_PORT_B, .pin = 10 },
#endif
};


void __attribute__((interrupt)) GBFW_CONTROL_SELECT_IRQ(void)
{
	stm32_exti_p_clear(gbfw_control__port_pin[GBFW_CONTROL_SELECT].pin);

	if (gbfw_settings->primary.home != GBFW_SETTINGS_HOME_DISABLED)
	{
		if (!stm32_gpio_pin_read(
			gbfw_control__port_pin[GBFW_CONTROL_START].port,
			gbfw_control__port_pin[GBFW_CONTROL_START].pin))
		{
			gbfw_signal(GBFW_SIGNAL_HOME);
		}
	}

	// STM32 Errata 2.1.3
	dsb();
}

void gbfw_control_init(void)
{
	for (uint8_t i = 0; i < GBFW_CONTROL_COUNT; i++)
	{
		stm32_gpio_pin_pull_set(
			gbfw_control__port_pin[i].port,
			gbfw_control__port_pin[i].pin,
			STM32_GPIO_PULL_UP);
		stm32_gpio_pin_mode_set(
			gbfw_control__port_pin[i].port,
			gbfw_control__port_pin[i].pin,
			STM32_GPIO_MODE_INPUT);
		stm32_gpio_pin_lock(
			gbfw_control__port_pin[i].port,
			gbfw_control__port_pin[i].pin);
	}

	stm32_syscfg_exti_port_set(
		gbfw_control__port_pin[GBFW_CONTROL_START].pin,
		gbfw_control__port_pin[GBFW_CONTROL_START].port);
	stm32_syscfg_exti_port_set(
		gbfw_control__port_pin[GBFW_CONTROL_SELECT].pin,
		gbfw_control__port_pin[GBFW_CONTROL_SELECT].port);

	gbfw_irq_enable(gbfw_control__port_pin[GBFW_CONTROL_SELECT].pin,
		true, false, GBFW_IRQ_PRIORITY_HOME);
}


gbfw_control_t gbfw_control_poll(void)
{
	uint16_t port_data[STM32_GPIO_PORT_F + 1] =
	{
		[STM32_GPIO_PORT_A] = gpio_port_read(STM32_GPIO_PORT_A),
		[STM32_GPIO_PORT_B] = gpio_port_read(STM32_GPIO_PORT_B),
#if (REVISION <= 6)
		[STM32_GPIO_PORT_C] = gpio_port_read(STM32_GPIO_PORT_C),
#endif
		[STM32_GPIO_PORT_F] = gpio_port_read(STM32_GPIO_PORT_F),
	};

	uint16_t mask = 0x0000;
	for (uint8_t i = 0; i < GBFW_CONTROL_COUNT; i++)
	{
		unsigned port = gbfw_control__port_pin[i].port;
		unsigned pin  = gbfw_control__port_pin[i].pin;

		if (((port_data[port] >> pin) & 1) == 0)
			mask |= 1U << i;
	}

	return (gbfw_control_t){ .mask = mask };
}
