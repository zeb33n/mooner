#include "audio.h"
#include "clocks.h"
#include "dma.h"
#include "irq.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/gpio.h>
#include <hardware/stm32/tim.h>
#include <hardware/stm32/dac.h>
#include <hardware/stm32/dbg.h>
#include <hardware/stm32/dma.h>
#include <hardware/stm32/dmamux.h>
#include <hardware/stm32/irq.h>
#include <hardware/cm4/nvic.h>
#include <hardware/cm4/intrinsics.h>
#include <stddef.h>
#include <stdbool.h>


#define GBFW_AUDIO_DAC_PORT STM32_GPIO_PORT_A
#define GBFW_AUDIO_DAC_PIN  4

#define GBFW_AUDIO_DAC_TIMER stm32_tim6

#define GBFW_AUDIO_SWITCH_MAX_FREQ 40000000

#define GBFW_AUDIO_DAC         stm32_dac1
#define GBFW_AUDIO_DAC_CHANNEL 0

#define GBFW_AUDIO_DAC_DMA             stm32_dma1
#define GBFW_AUDIO_DAC_DMA_CHANNEL     0
#define GBFW_AUDIO_DAC_DMA_IRQ         STM32_IRQ_DMA1_CH1
#define GBFW_AUDIO_DAC_DMA_IRQ_VECTOR  stm32_vector_irq_dma1_ch1

#define GBFW_AUDIO_VOL_PWM_PORT STM32_GPIO_PORT_A
#define GBFW_AUDIO_VOL_PWM_PIN  5
#define GBFW_AUDIO_VOL_PWM_AF   1

#define GBFW_AUDIO_VOL_PWM_TIMER stm32_tim2



static void    (*gbfw_audio_callback)(int16_t *buff, uint16_t size);
static int16_t  *gbfw_audio_callback_addr;
static uint16_t  gbfw_audio_callback_size;

void __attribute__((interrupt)) GBFW_AUDIO_DAC_DMA_IRQ_VECTOR(void)
{
	if (GBFW_AUDIO_DAC_DMA->is.htif1)
	{
		gbfw_audio_callback(
			gbfw_audio_callback_addr,
			gbfw_audio_callback_size);

		GBFW_AUDIO_DAC_DMA->ifc.htif1 = true;
	}
	else if (GBFW_AUDIO_DAC_DMA->is.tcif1)
	{
		gbfw_audio_callback(
			&gbfw_audio_callback_addr[gbfw_audio_callback_size],
			gbfw_audio_callback_size);

		GBFW_AUDIO_DAC_DMA->ifc.tcif1 = true;
	}

	// STM32 Errata 2.1.3
	dsb();
}


void gbfw_audio_init(void)
{
	stm32_rcc->apb1en1.tim2 = true;
	stm32_rcc->apb1en1.tim6 = true;
	stm32_rcc->ahb2en.dac1  = true;

	stm32_gpio_pin_set      (GBFW_AUDIO_VOL_PWM_PORT, GBFW_AUDIO_VOL_PWM_PIN);
	stm32_gpio_pin_speed_set(GBFW_AUDIO_VOL_PWM_PORT, GBFW_AUDIO_VOL_PWM_PIN, STM32_GPIO_SPEED_VERY_HIGH);
	stm32_gpio_pin_af_set   (GBFW_AUDIO_VOL_PWM_PORT, GBFW_AUDIO_VOL_PWM_PIN, GBFW_AUDIO_VOL_PWM_AF);

#if GBFW_CLOCKS_DEFAULT_FREQ > GBFW_AUDIO_SWITCH_MAX_FREQ
	GBFW_AUDIO_VOL_PWM_TIMER->psc = ((GBFW_CLOCKS_DEFAULT_FREQ + (GBFW_AUDIO_SWITCH_MAX_FREQ - 1)) / GBFW_AUDIO_SWITCH_MAX_FREQ) - 1;
#endif

	GBFW_AUDIO_VOL_PWM_TIMER->ccm1.oc.oc1m = STM32_TIM_CCM_OCM_PWM_INVERTED;
	GBFW_AUDIO_VOL_PWM_TIMER->cce.cc1e     = true;

	GBFW_AUDIO_DAC_TIMER->c1.dithen = true;
	GBFW_AUDIO_DAC_TIMER->c2.mms    = STM32_TIM_MMS_UPDATE;

	stm32_dac_mc_t mc = { .mask = GBFW_AUDIO_DAC->mc.mask };
	mc.dmadouble1 = true;
	mc.sinformat1 = true;
#if GBFW_CLOCKS_DEFAULT_FREQ > 160000000
	mc.hfsel = 2;
#elif GBFW_CLOCKS_DEFAULT_FREQ > 80000000
	mc.hfsel = 1;
#endif
	GBFW_AUDIO_DAC->mc.mask = mc.mask;

	// TODO: Calibrate DAC.

	stm32_dac_c_t c = { .mask = GBFW_AUDIO_DAC->c.mask };
	c.ten1   = true;
	c.tsel1  = STM32_DAC1_TRG_TIM6_TRGO;
	c.dmaen1 = true;
	GBFW_AUDIO_DAC->c.mask = c.mask;

#ifndef NDEBUG
	// Pause audio during debug halt.
	stm32_dbg->apb1_fz1.dbg_tim6_stop = true;
#endif

	stm32_dmamux->cc[0].dmareq_id = STM32_DMAMUX_REQ_DAC1_CH1;

	stm32_dma_cc_t cc = { .mask = GBFW_AUDIO_DAC_DMA->c[GBFW_AUDIO_DAC_DMA_CHANNEL].cc.mask };
	cc.tcie    = true;
	cc.htie    = true;
	cc.dir     = STM32_DMA_DIR_READ_MEMORY;
	cc.circ    = true;
	cc.pinc    = false;
	cc.minc    = true;
	cc.psize   = STM32_DMA_SIZE_WORD;
	cc.msize   = STM32_DMA_SIZE_WORD;
	cc.pl      = 3;
	cc.mem2mem = false;
	GBFW_AUDIO_DAC_DMA->c[GBFW_AUDIO_DAC_DMA_CHANNEL].cc.mask = cc.mask;

	GBFW_AUDIO_DAC_DMA->c[GBFW_AUDIO_DAC_DMA_CHANNEL].cpa = (uint32_t)&GBFW_AUDIO_DAC->dh12l1;

	cm4_nvic_interrupt_priority_set(GBFW_AUDIO_DAC_DMA_IRQ, GBFW_IRQ_PRIORITY_AUDIO);
	cm4_nvic_interrupt_enable(GBFW_AUDIO_DAC_DMA_IRQ);
}


void gbfw_audio_volume_set(uint8_t num, uint8_t den)
{
	GBFW_AUDIO_VOL_PWM_TIMER->cc1 = num;
	GBFW_AUDIO_VOL_PWM_TIMER->ar  = den;

	GBFW_AUDIO_VOL_PWM_TIMER->eg.ug  = true;
	GBFW_AUDIO_VOL_PWM_TIMER->s.mask = 0;
}

void gbfw_audio_volume_mute(bool mute)
{
	stm32_gpio_pin_mode_set(GBFW_AUDIO_VOL_PWM_PORT, GBFW_AUDIO_VOL_PWM_PIN,
		(mute ? STM32_GPIO_MODE_OUTPUT : STM32_GPIO_MODE_ALTERNATE));
	GBFW_AUDIO_VOL_PWM_TIMER->c1.cen = !mute;
	GBFW_AUDIO_DAC->c.en1 = !mute;
}

void gbfw_audio_rate_set(uint16_t rate)
{
	GBFW_AUDIO_DAC_TIMER->ar = (GBFW_CLOCKS_DEFAULT_FREQ  * 16UL) / rate;
}


void gbfw_audio_start(
	int16_t *buff, uint16_t size,
	void (*callback)(int16_t *buff, uint16_t size))
{
	gbfw_audio_callback = callback;

	gbfw_audio_callback_addr = (int16_t *)gbfw_dma_pointer(buff);
	gbfw_audio_callback_size = size / 2;

	GBFW_AUDIO_DAC_DMA->c[GBFW_AUDIO_DAC_DMA_CHANNEL].cma  = (uint32_t)gbfw_audio_callback_addr;
	GBFW_AUDIO_DAC_DMA->c[GBFW_AUDIO_DAC_DMA_CHANNEL].cndt = size / 2;

	callback( buff                          , gbfw_audio_callback_size);
	callback(&buff[gbfw_audio_callback_size], gbfw_audio_callback_size);
	gbfw_audio_continue();
}

void gbfw_audio_stop(void)
{
	GBFW_AUDIO_DAC_TIMER->c1.cen = false;
	GBFW_AUDIO_DAC_DMA->c[GBFW_AUDIO_DAC_DMA_CHANNEL].cc.en = false;
}

void gbfw_audio_continue(void)
{
	GBFW_AUDIO_DAC_TIMER->c1.cen = true;
	GBFW_AUDIO_DAC_DMA->c[GBFW_AUDIO_DAC_DMA_CHANNEL].cc.en = true;
}
