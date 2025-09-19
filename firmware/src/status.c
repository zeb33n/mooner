#include "status.h"
#include "settings.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/gpio.h>
#include <hardware/stm32/tim.h>
#include <standard/pixel/rgb888.h>

#define GBFW_STATUS_RED_PORT   STM32_GPIO_PORT_B
#define GBFW_STATUS_RED_PIN    6
#define GBFW_STATUS_RED_AF     2

#define GBFW_STATUS_GREEN_PORT STM32_GPIO_PORT_B
#define GBFW_STATUS_GREEN_PIN  7
#define GBFW_STATUS_GREEN_AF   2

#define GBFW_STATUS_BLUE_PORT  STM32_GPIO_PORT_B
#define GBFW_STATUS_BLUE_PIN   8
#define GBFW_STATUS_BLUE_AF    2

#if (REVISION <= 7)
#define GBFW_STATUS_COLOR_CORRECT_RED   256
#define GBFW_STATUS_COLOR_CORRECT_GREEN  38
#define GBFW_STATUS_COLOR_CORRECT_BLUE   98
#else
#define GBFW_STATUS_COLOR_CORRECT_RED   239
#define GBFW_STATUS_COLOR_CORRECT_GREEN  99
#define GBFW_STATUS_COLOR_CORRECT_BLUE  256
#endif



void gbfw_status_init(void)
{
	stm32_rcc->apb1en1.tim4 = true;

	stm32_gpio_pin_af_set(GBFW_STATUS_RED_PORT  , GBFW_STATUS_RED_PIN  , GBFW_STATUS_RED_AF  );
	stm32_gpio_pin_af_set(GBFW_STATUS_GREEN_PORT, GBFW_STATUS_GREEN_PIN, GBFW_STATUS_GREEN_AF);
	stm32_gpio_pin_af_set(GBFW_STATUS_BLUE_PORT , GBFW_STATUS_BLUE_PIN , GBFW_STATUS_BLUE_AF );

	stm32_gpio_pin_mode_set(GBFW_STATUS_RED_PORT  , GBFW_STATUS_RED_PIN  , STM32_GPIO_MODE_ALTERNATE);
	stm32_gpio_pin_mode_set(GBFW_STATUS_GREEN_PORT, GBFW_STATUS_GREEN_PIN, STM32_GPIO_MODE_ALTERNATE);
	stm32_gpio_pin_mode_set(GBFW_STATUS_BLUE_PORT , GBFW_STATUS_BLUE_PIN , STM32_GPIO_MODE_ALTERNATE);

	stm32_tim_ccm1_t ccm1 = { .mask = stm32_tim4->ccm1.mask };
	ccm1.oc.oc1m = STM32_TIM_CCM_OCM_PWM_INVERTED;
	ccm1.oc.oc2m = STM32_TIM_CCM_OCM_PWM_INVERTED;
	stm32_tim4->ccm1.mask = ccm1.mask;

	stm32_tim4->ccm2.oc.oc3m = STM32_TIM_CCM_OCM_PWM_INVERTED;

	stm32_tim_cce_t cce = { .mask = stm32_tim4->cce.mask };
	cce.cc1e = true;
	cce.cc2e = true;
	cce.cc3e = true;
	stm32_tim4->cce.mask = cce.mask;

	stm32_tim4->ar        = 4096;
	stm32_tim4->c1.dithen = true;
	stm32_tim4->c1.cen    = true;
}

void gbfw_status_set(rgb888_t color, uint8_t brightness)
{
	uint16_t sb = gbfw_settings->primary.led_level * 17;
	sb += sb >> 7;

	stm32_tim4->cc1 = (color.r * GBFW_STATUS_COLOR_CORRECT_RED   * (brightness + 1) * sb) / 65536;
	stm32_tim4->cc2 = (color.g * GBFW_STATUS_COLOR_CORRECT_GREEN * (brightness + 1) * sb) / 65536;
	stm32_tim4->cc3 = (color.b * GBFW_STATUS_COLOR_CORRECT_BLUE  * (brightness + 1) * sb) / 65536;
}
