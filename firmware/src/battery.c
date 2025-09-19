#include "battery.h"
#include "analog.h"
#include "usb.h"
#include <hardware/stm32/gpio.h>


#if (REVISION == 5)
#define GBFW_FAST_CHARGE_PORT STM32_GPIO_PORT_B
#define GBFW_FAST_CHARGE_PIN  10
#else
#define GBFW_FAST_CHARGE_PORT STM32_GPIO_PORT_A
#define GBFW_FAST_CHARGE_PIN  9
#endif


void gbfw_battery_init(void)
{
	stm32_gpio_pin_clear(GBFW_FAST_CHARGE_PORT, GBFW_FAST_CHARGE_PIN);
}

bool gbfw_battery_is_charging(void)
{
	return (gbfw_usb_voltage() >= 4500);
}

uint8_t gbfw_battery_level(void)
{
	uint16_t pv = gbfw_analog_battery_voltage();

	if (pv <= 3300) return 0;
	pv -= 3300;

	pv = (pv * 0xFF) / (4200 - 3300);
	return (pv > 0xFF ? 0xFF : pv);
}

void gbfw_battery_fast_charge(bool enable)
{
	stm32_gpio_pin_mode_set(GBFW_FAST_CHARGE_PORT, GBFW_FAST_CHARGE_PIN,
		(enable ? STM32_GPIO_MODE_OUTPUT : STM32_GPIO_MODE_ANALOG));
}
