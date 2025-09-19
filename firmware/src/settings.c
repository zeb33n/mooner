#include "settings.h"
#include <hardware/stm32/tamp.h>
#include <stdbool.h>


static const gbfw_settings_t gbfw_settings_default =
{
	.primary =
	{
		.brightness = 15,
		.volume     =  8,
		.led_level  =  2,
		.splash     = GBFW_SETTINGS_SPLASH_FULL,
		.home       = GBFW_SETTINGS_HOME_DEFAULT,
		.exec_valid = false,
		.boot_fail  = false,
		.valid      = true,
	},

	.exec_sector = 0xFFFFFFFF,
};

volatile gbfw_settings_t * const gbfw_settings = (volatile gbfw_settings_t * const)&stm32_tamp->bkp[0];


volatile gbfw_settings_t *gbfw_settings_pointer(void)
{
	return gbfw_settings;
}


void gbfw_settings_init(void)
{
	if (!gbfw_settings->primary.valid)
		*gbfw_settings = gbfw_settings_default;
}


uint8_t gbfw_settings_brightness_get(void)
{
	return (((gbfw_settings->primary.brightness + 1) * 16) - 1);
}
