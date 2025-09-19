#include "analog.h"
#include "delay.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/gpio.h>
#include <hardware/stm32/adc.h>
#include <hardware/stm32/vrefbuf.h>
#include <hardware/stm32/device.h>


#if (REVISION == 5)
#define GBFW_USB_DETECT_ADC_CHANNEL 14
#elif (REVISION <= 7)
#define GBFW_USB_DETECT_ADC_CHANNEL 10
#else
#define GBFW_USB_DETECT_ADC_CHANNEL 1
#endif

#if (REVISION <= 7)
#define GBFW_USB_DETECT_DIV_HI 255
#define GBFW_USB_DETECT_DIV_LO 510
#else
#define GBFW_USB_DETECT_DIV_HI 1
#define GBFW_USB_DETECT_DIV_LO 1
#endif

#if (REVISION == 5)
#define GBFW_BATTERY_VOLTAGE_PORT        STM32_GPIO_PORT_A
#define GBFW_BATTERY_VOLTAGE_PIN         3
#define GBFW_BATTERY_VOLTAGE_ADC_CHANNEL 4
#else
#define GBFW_BATTERY_VOLTAGE_PORT        STM32_GPIO_PORT_A
#define GBFW_BATTERY_VOLTAGE_PIN         2
#define GBFW_BATTERY_VOLTAGE_ADC_CHANNEL 3
#endif

#if (REVISION < 7)
#define GBFW_BATTERY_VOLTAGE_DIV_HI  51
#define GBFW_BATTERY_VOLTAGE_DIV_LO 115
#elif (REVISION == 7)
#define GBFW_BATTERY_VOLTAGE_DIV_HI 115
#define GBFW_BATTERY_VOLTAGE_DIV_LO 402
#else
#define GBFW_BATTERY_VOLTAGE_DIV_HI 1
#define GBFW_BATTERY_VOLTAGE_DIV_LO 1
#endif


void gbfw_analog_init(void)
{
#if (REVISION >= 8)
	// Enable Voltage Reference
	stm32_vrefbuf_cs_t vref_cs = { .mask = stm32_vrefbuf->cs.mask };
	vref_cs.vrs  = STM32_VREFBUF_VRS_2V9;
	vref_cs.hiz  = 0;
	vref_cs.envr = 1;
	stm32_vrefbuf->cs.mask = vref_cs.mask;
#endif

	stm32_rcc->ccip.adc12sel = STM32_RCC_ADC_CLKSEL_SYSCLK;
	stm32_rcc->ahb2en.adc12 = true;

	stm32_adc_cc_t cc = { .mask = stm32_adc_group12->cc.mask };
	cc.ckmode    = 3;
	cc.vbatsel   = true;
	cc.vsensesel = true;
	cc.vrefen    = true;
	stm32_adc_group12->cc.mask = cc.mask;

	stm32_adc1->c.deeppwd  = false;
	stm32_adc1->c.advregen = true;
	gbfw_delay_us(20);

	stm32_adc1->c.adcal = true;
	while (stm32_adc1->c.adcal);

	stm32_adc_cfg2_t cfg2 = { .mask = stm32_adc1->cfg2.mask };
	cfg2.rovse = true;
	cfg2.ovsr  = 7;
	cfg2.ovss  = 4;
	stm32_adc1->cfg2.mask = cfg2.mask;

	stm32_adc_smp_set(stm32_adc1, STM32_ADC1_CHANNEL_TS           , 7);
	stm32_adc_smp_set(stm32_adc1, STM32_ADC1_CHANNEL_VBAT         , 7);
	stm32_adc_smp_set(stm32_adc1, STM32_ADC1_CHANNEL_VREFINT      , 7);
	stm32_adc_smp_set(stm32_adc1, GBFW_USB_DETECT_ADC_CHANNEL     , 7);
	stm32_adc_smp_set(stm32_adc1, GBFW_BATTERY_VOLTAGE_ADC_CHANNEL, 7);

	stm32_adc1->c.aden = true;
	while (!stm32_adc1->is.adrdy);

	stm32_gpio_pin_lock(GBFW_BATTERY_VOLTAGE_PORT, GBFW_BATTERY_VOLTAGE_PIN);
}

static uint16_t gbfw_analog_adc_read_raw(uint8_t channel)
{
	stm32_adc_sqr_set(stm32_adc1, 0, channel);

	stm32_adc1->c.adstart = true;
	while (stm32_adc1->c.adstart);

	return stm32_adc1->d;
}

static uint32_t gbfw_analog_adc_read_vref(uint8_t channel)
{
	uint16_t raw  = gbfw_analog_adc_read_raw(channel);

#if (REVISION >= 8)
	return raw;
#else
	uint16_t vref = gbfw_analog_adc_read_raw(STM32_ADC1_CHANNEL_VREFINT);

	uint16_t vref_cal = *STM32_VREFINT_CAL;
	vref_cal = (vref_cal << 4) | (vref_cal >> 8);

	return (((uint32_t)raw * vref_cal) / vref);
#endif
}

static uint16_t gbfw_analog_adc_read_mv(uint8_t channel, uint32_t div_hi, uint32_t div_lo)
{
#if (REVISION >= 8)
	const uint32_t ref_mv = 2900;
#else
	const uint32_t ref_mv = STM32_VREFINT_MV;
#endif

	uint32_t mv = (gbfw_analog_adc_read_vref(channel) * ref_mv) >> 16;
	mv *= (div_hi + div_lo);
	mv /= div_lo;
	return mv;
}

uint16_t gbfw_analog_battery_voltage(void)
{
	return gbfw_analog_adc_read_mv(
		GBFW_BATTERY_VOLTAGE_ADC_CHANNEL,
		GBFW_BATTERY_VOLTAGE_DIV_HI,
		GBFW_BATTERY_VOLTAGE_DIV_LO);
}

uint16_t gbfw_analog_usb_voltage(void)
{
	return gbfw_analog_adc_read_mv(
		GBFW_USB_DETECT_ADC_CHANNEL,
		GBFW_USB_DETECT_DIV_HI,
		GBFW_USB_DETECT_DIV_LO);
}

int32_t gbfw_analog_temperature(void)
{
	uint32_t ts_data = gbfw_analog_adc_read_vref(STM32_ADC1_CHANNEL_TS);

	uint16_t ts_cal1 = *STM32_TS_CAL1;
	uint16_t ts_cal2 = *STM32_TS_CAL2;

	ts_cal1 = (ts_cal1 << 4) | (ts_cal1 >> 8);
	ts_cal2 = (ts_cal2 << 4) | (ts_cal2 >> 8);

	int32_t t = (ts_data - ts_cal1);
	t *= (STM32_TS_CAL2_TEMP_MC - STM32_TS_CAL1_TEMP_MC);
	t /= (ts_cal2 - ts_cal1);
	t += STM32_TS_CAL1_TEMP_MC;

	return t;
}
