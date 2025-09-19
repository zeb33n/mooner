#include "rtc.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/rtc.h>
#include <stdbool.h>


void gbfw_rtc_init(void)
{
	if (stm32_rcc->bdc.rtcen)
		return;

	stm32_rcc->bdc.lseon = false;
	while (stm32_rcc->bdc.lserdy);

#if (REVISION < 7)
	stm32_rcc->bdc.lsedrv = STM32_RCC_LSEDRV_HIGH;
#else
	stm32_rcc->bdc.lsebyp = true;
#endif

	stm32_rcc->bdc.lseon = true;

	while (!stm32_rcc->bdc.lserdy);

	stm32_rcc->bdc.rtcsel = STM32_RCC_RTCSEL_LSE;
	stm32_rcc->bdc.rtcen = true;

	stm32_rtc->wp = STM32_RTC_WP_KEY1;
	stm32_rtc->wp = STM32_RTC_WP_KEY2;
}
