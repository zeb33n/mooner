#include "clocks.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/pwr.h>
#include <hardware/stm32/crs.h>
#include <hardware/stm32/flash.h>
#include <stdbool.h>


static void gbfw_clocks__pllon(bool enable)
{
	stm32_rcc->c.pllon = enable;
	while (stm32_rcc->c.pllrdy != enable);
}

#if GBFW_FLASH_WS > 0
static void gbfw_flash_latency_set(uint8_t latency)
{
	stm32_flash->ac.latency = latency;
	while (stm32_flash->ac.latency != latency);
}
#endif

void gbfw_clocks_default(void)
{
	stm32_rcc->cfg.sw = STM32_RCC_SW_HSI16;
	while (stm32_rcc->cfg.sws != STM32_RCC_SW_HSI16);

	gbfw_clocks__pllon(false);

	stm32_rcc_pllcfg_t cfg;
	cfg.mask = stm32_rcc->pllcfg.mask;

	cfg.pllsrc = STM32_RCC_PLLSRC_HSI16;
	cfg.pllren = true;

#if GBFW_CLOCKS_PLL_M != 0
	cfg.pllm = GBFW_CLOCKS_PLL_M;
#endif

#if GBFW_CLOCKS_PLL_N != 8
	cfg.plln = GBFW_CLOCKS_PLL_N;
#endif

#if GBFW_CLOCKS_PLL_R != 0
	cfg.pllr = GBFW_CLOCKS_PLL_R;
#endif

	stm32_rcc->pllcfg.mask = cfg.mask;

	gbfw_clocks__pllon(true);

#if GBFW_CLOCKS_R1_MODE != 1
	stm32_pwr->c5.r1mode = 0;
#endif

#if GBFW_FLASH_WS > 0
	gbfw_flash_latency_set(GBFW_FLASH_WS);
	stm32_flash->ac.prften = true;
#endif

	stm32_rcc->cfg.sw = STM32_RCC_SW_PLLRCLK;

	stm32_rcc->apb1en1.crs  = true;
	stm32_crs->c.autotrimen = true;
	stm32_rcc->crrc.hsi48on = true;
	while (!stm32_rcc->crrc.hsi48rdy);
}
