#include "sandbox.h"
#include "flash.h"
#include <hardware/stm32/gpio.h>
#include <hardware/stm32/flash.h>

extern volatile uint8_t arm_vector_hard_fault_ignore;

void gbfw_sandbox(void)
{
	// Finalize GPIO locking.
	for (stm32_gpio_port_e port = STM32_GPIO_PORT_A; port <= STM32_GPIO_PORT_F; port++)
	{
		stm32_gpio_lck_t lck = { .mask = stm32_gpio[port].lck.mask };
		if (lck.mask == 0)
			continue;

		lck.lckk = 1;
		stm32_gpio[port].lck.mask = lck.mask;
		lck.lckk = 0;
		stm32_gpio[port].lck.mask = lck.mask;
		lck.lckk = 1;
		stm32_gpio[port].lck.mask = lck.mask;
	}

	gbfw_flash_lock(true);
}
