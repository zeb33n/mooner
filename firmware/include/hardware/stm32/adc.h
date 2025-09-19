#ifndef STM32_ADC_H
#define STM32_ADC_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef enum
{
	STM32_ADC1_CHANNEL_OPAMP1  = 13,
	STM32_ADC1_CHANNEL_TS      = 16,
	STM32_ADC1_CHANNEL_VBAT    = 17,
	STM32_ADC1_CHANNEL_VREFINT = 18,

	STM32_ADC2_CHANNEL_OPAMP2  = 16,
	STM32_ADC2_CHANNEL_OPAMP3  = 18,

	STM32_ADC3_CHANNEL_OPAMP3  = 13,
	STM32_ADC3_CHANNEL_VBAT    = 17,
	STM32_ADC3_CHANNEL_VREFINT = 18,

	STM32_ADC4_CHANNEL_OPAMP6  = 17,
	STM32_ADC4_CHANNEL_VREFINT = 18,

	STM32_ADC5_CHANNEL_OPAMP5  =  3,
	STM32_ADC5_CHANNEL_TS      =  4,
	STM32_ADC5_CHANNEL_OPAMP4  =  5,
	STM32_ADC5_CHANNEL_VBAT    = 17,
	STM32_ADC5_CHANNEL_VREFINT = 18,
} stm32_adc_channel_e;

typedef union
{
	struct
	{
		uint32_t adrdy    :  1;
		uint32_t eosmp    :  1;
		uint32_t eoc      :  1;
		uint32_t eos      :  1;
		uint32_t ovr      :  1;
		uint32_t jeoc     :  1;
		uint32_t jeos     :  1;
		uint32_t awd1     :  1;
		uint32_t awd2     :  1;
		uint32_t awd3     :  1;
		uint32_t jqovf    :  1;
		uint32_t res31_11 : 21;
	};

	uint32_t mask;
} stm32_adc_i_t;

typedef union
{
	struct
	{
		uint32_t aden     :  1;
		uint32_t addis    :  1;
		uint32_t adstart  :  1;
		uint32_t jadstart :  1;
		uint32_t adstp    :  1;
		uint32_t jadstp   :  1;
		uint32_t res27_6  : 22;
		uint32_t advregen :  1;
		uint32_t deeppwd  :  1;
		uint32_t adcaldif :  1;
		uint32_t adcal    :  1;
	};

	uint32_t mask;
} stm32_adc_c_t;

typedef union
{
	struct
	{
		uint32_t dmaen     : 1;
		uint32_t dmacfg    : 1;
		uint32_t res2      : 1;
		uint32_t res       : 2;
		uint32_t extsel    : 5;
		uint32_t exten     : 2;
		uint32_t ovrmod    : 1;
		uint32_t cont      : 1;
		uint32_t autdly    : 1;
		uint32_t align     : 1;
		uint32_t discen    : 1;
		uint32_t discnum   : 3;
		uint32_t jdiscen   : 1;
		uint32_t jqm       : 1;
		uint32_t awd1sgl   : 1;
		uint32_t awd1en    : 1;
		uint32_t jawd1en   : 1;
		uint32_t jauto     : 1;
		uint32_t awd1ch    : 5;
		uint32_t jqdis     : 1;
	};

	uint32_t mask;
} stm32_adc_cfg_t;

typedef union
{
	struct
	{
		uint32_t rovse     : 1;
		uint32_t jovse     : 1;
		uint32_t ovsr      : 3;
		uint32_t ovss      : 4;
		uint32_t trovs     : 1;
		uint32_t rovsm     : 1;
		uint32_t res15_11  : 5;
		uint32_t gcomp     : 1;
		uint32_t res24_17  : 8;
		uint32_t swtrig    : 1;
		uint32_t bulb      : 1;
		uint32_t smptrig   : 1;
		uint32_t res31_28  : 4;
	};

	uint32_t mask;
} stm32_adc_cfg2_t;

typedef union
{
	struct
	{
		uint32_t smp0    : 3;
		uint32_t smp1    : 3;
		uint32_t smp2    : 3;
		uint32_t smp3    : 3;
		uint32_t smp4    : 3;
		uint32_t smp5    : 3;
		uint32_t smp6    : 3;
		uint32_t smp7    : 3;
		uint32_t smp8    : 3;
		uint32_t smp9    : 3;
		uint32_t res30   : 1;
		uint32_t smpplus : 1;
	};

	uint32_t mask;
} stm32_adc_smp1_t;

typedef union
{
	struct
	{
		uint32_t smp10    : 3;
		uint32_t smp11    : 3;
		uint32_t smp12    : 3;
		uint32_t smp13    : 3;
		uint32_t smp14    : 3;
		uint32_t smp15    : 3;
		uint32_t smp16    : 3;
		uint32_t smp17    : 3;
		uint32_t smp18    : 3;
		uint32_t res31_27 : 5;
	};

	uint32_t mask;
} stm32_adc_smp2_t;

typedef union
{
	struct
	{
		uint32_t lt       : 12;
		uint32_t awdfilt  :  1;
		uint32_t res15    :  1;
		uint32_t ht       : 12;
		uint32_t res31_28 :  4;
	};

	uint32_t mask;
} stm32_adc_t1_t;

typedef union
{
	struct
	{
		uint32_t lt       : 8;
		uint32_t res15_8  : 8;
		uint32_t ht       : 8;
		uint32_t res31_24 : 8;
	};

	uint32_t mask;
} stm32_adc_t_t;

typedef union
{
	struct
	{
		uint32_t l        : 4;
		uint32_t res5_4   : 2;
		uint32_t sq1      : 5;
		uint32_t res11    : 1;
		uint32_t sq2      : 5;
		uint32_t res17    : 1;
		uint32_t sq3      : 5;
		uint32_t res23    : 1;
		uint32_t sq4      : 5;
		uint32_t res31_29 : 3;
	};

	uint32_t mask;
} stm32_adc_sq1_t;

typedef union
{
	struct
	{
		uint32_t sq5      : 5;
		uint32_t res5     : 1;
		uint32_t sq6      : 5;
		uint32_t res11    : 1;
		uint32_t sq7      : 5;
		uint32_t res17    : 1;
		uint32_t sq8      : 5;
		uint32_t res23    : 1;
		uint32_t sq9      : 5;
		uint32_t res31_29 : 3;
	};

	uint32_t mask;
} stm32_adc_sq2_t;

typedef union
{
	struct
	{
		uint32_t sq10     : 5;
		uint32_t res5     : 1;
		uint32_t sq11     : 5;
		uint32_t res11    : 1;
		uint32_t sq12     : 5;
		uint32_t res17    : 1;
		uint32_t sq13     : 5;
		uint32_t res23    : 1;
		uint32_t sq14     : 5;
		uint32_t res31_29 : 3;
	};

	uint32_t mask;
} stm32_adc_sq3_t;

typedef union
{
	struct
	{
		uint32_t sq15     :  5;
		uint32_t res5     :  1;
		uint32_t sq16     :  5;
		uint32_t res31_11 : 21;
	};

	uint32_t mask;
} stm32_adc_sq4_t;

typedef union
{
	struct
	{
		uint32_t jl       : 2;
		uint32_t jextsel  : 5;
		uint32_t jexten   : 2;
		uint32_t jsq1     : 5;
		uint32_t res14    : 1;
		uint32_t jsq2     : 5;
		uint32_t res20    : 1;
		uint32_t jsq3     : 5;
		uint32_t res26    : 1;
		uint32_t jsq4     : 5;
	};

	uint32_t mask;
} stm32_adc_jsq_t;

typedef union
{
	struct
	{
		uint32_t s        : 7;
		uint32_t res15_7  : 9;
		uint32_t d        : 7;
		uint32_t res31_23 : 9;
	};

	uint32_t mask;
} stm32_adc_calfact_t;

typedef union
{
	struct
	{
		uint32_t offset    : 12;
		uint32_t res23_12  : 12;
		uint32_t offsetpos :  1;
		uint32_t saten     :  1;
		uint32_t offset_ch :  5;
		uint32_t offset_en :  1;
	};

	uint32_t mask;
} stm32_adc_of_t;

typedef union
{
	struct
	{
		uint32_t adrdy_mst  : 1;
		uint32_t eosmp_mst  : 1;
		uint32_t eoc_mst    : 1;
		uint32_t eos_mst    : 1;
		uint32_t ovr_mst    : 1;
		uint32_t jeoc_mst   : 1;
		uint32_t jeos_mst   : 1;
		uint32_t awd1_mst   : 1;
		uint32_t awd2_mst   : 1;
		uint32_t awd3_mst   : 1;
		uint32_t jqovf_mst  : 1;
		uint32_t res15_11   : 5;
		uint32_t adrdy_slv  : 1;
		uint32_t eosmp_slv  : 1;
		uint32_t eoc_slv    : 1;
		uint32_t eos_slv    : 1;
		uint32_t ovr_slv    : 1;
		uint32_t jeoc_slv   : 1;
		uint32_t jeos_slv   : 1;
		uint32_t awd1_slv   : 1;
		uint32_t awd2_slv   : 1;
		uint32_t awd3_slv   : 1;
		uint32_t jqovf_slv  : 1;
		uint32_t res31_27   : 5;
	};

	uint32_t mask;
} stm32_adc_cs_t;

typedef union
{
	struct
	{
		uint32_t dual      : 5;
		uint32_t res7_5    : 3;
		uint32_t delay     : 4;
		uint32_t res12     : 1;
		uint32_t dmacfg    : 1;
		uint32_t mdma      : 2;
		uint32_t ckmode    : 2;
		uint32_t presc     : 4;
		uint32_t vrefen    : 1;
		uint32_t vsensesel : 1;
		uint32_t vbatsel   : 1;
		uint32_t res31_25  : 7;
	};

	uint32_t mask;
} stm32_adc_cc_t;

typedef union
{
	struct
	{
		uint32_t master : 16;
		uint32_t slave  : 16;
	};

	uint32_t mask;
} stm32_adc_cd_t;


typedef struct
{
	stm32_adc_i_t       is;
	stm32_adc_i_t       ie;
	stm32_adc_c_t       c;
	stm32_adc_cfg_t     cfg;
	stm32_adc_cfg2_t    cfg2;
	stm32_adc_smp1_t    smp1;
	stm32_adc_smp2_t    smp2;
	uint32_t            res7;
	stm32_adc_t1_t      t1;
	stm32_adc_t_t       t2;
	stm32_adc_t_t       t3;
	uint32_t            res11;
	stm32_adc_sq1_t     sq1;
	stm32_adc_sq2_t     sq2;
	stm32_adc_sq3_t     sq3;
	stm32_adc_sq4_t     sq4;
	uint32_t            d;
	uint32_t            res18_17[2];
	stm32_adc_jsq_t     jsq;
	uint32_t            res23_20[4];
	stm32_adc_of_t      of1;
	stm32_adc_of_t      of2;
	stm32_adc_of_t      of3;
	stm32_adc_of_t      of4;
	uint32_t            res31_28[4];
	uint32_t            jd1;
	uint32_t            jd2;
	uint32_t            jd3;
	uint32_t            jd4;
	uint32_t            res39_36[4];
	uint32_t            awd2c;
	uint32_t            awd3c;
	uint32_t            res43_42[2];
	uint32_t            difsel;
	stm32_adc_calfact_t calfact;
	uint32_t            res47_46[2];
	uint32_t            gcomp;
	uint32_t            res63_49[15];
} stm32_adc_t;

typedef struct
{
	stm32_adc_t master;
	stm32_adc_t slave;
	stm32_adc_t single;

	stm32_adc_cs_t cs;
	uint32_t       res65;
	stm32_adc_cc_t cc;
	stm32_adc_cd_t cd;
	uint32_t       res255_196[60];
} stm32_adc_group_t;


static volatile stm32_adc_group_t * const stm32_adc_group12  = (volatile stm32_adc_group_t *)STM32_MMAP_ADC12;
static volatile stm32_adc_group_t * const stm32_adc_group345 = (volatile stm32_adc_group_t *)STM32_MMAP_ADC345;

static volatile stm32_adc_t * const stm32_adc1 = (volatile stm32_adc_t *)&stm32_adc_group12->master;
static volatile stm32_adc_t * const stm32_adc2 = (volatile stm32_adc_t *)&stm32_adc_group12->slave;
static volatile stm32_adc_t * const stm32_adc3 = (volatile stm32_adc_t *)&stm32_adc_group345->master;
static volatile stm32_adc_t * const stm32_adc4 = (volatile stm32_adc_t *)&stm32_adc_group345->slave;
static volatile stm32_adc_t * const stm32_adc5 = (volatile stm32_adc_t *)&stm32_adc_group345->single;

#define STM32_TS_CAL1_TEMP_MC  30000
#define STM32_TS_CAL2_TEMP_MC 110000
#define STM32_VREFINT_MV        3000

const uint16_t * const STM32_TS_CAL1     = (uint16_t *)(STM32_MMAP_ENGINEERING_BYTES + 0xA8);
const uint16_t * const STM32_TS_CAL2     = (uint16_t *)(STM32_MMAP_ENGINEERING_BYTES + 0xCA);
const uint16_t * const STM32_VREFINT_CAL = (uint16_t *)(STM32_MMAP_ENGINEERING_BYTES + 0xAA);


static inline void stm32_adc_smp_set(
	volatile stm32_adc_t * const adc,
	uint8_t channel, uint8_t smp)
{
	volatile uint32_t * const r = &(&adc->smp1.mask)[channel < 10 ? 0 : 1];
	if (channel >= 10) channel -= 10;

	uint32_t mask = *r;
	mask &= ~(7UL << (channel * 3));
	mask |=  (smp << (channel * 3));
	*r = mask;

}

static inline void stm32_adc_sqr_set(
	volatile stm32_adc_t * const adc,
	uint8_t sq, uint8_t channel)
{
	sq += 1;

	volatile uint32_t * const r = &(&adc->sq1.mask)[sq / 5];
	sq %= 5;

	uint32_t mask = *r;
	mask &= ~( 0x1FUL << (sq * 6));
	mask |=  (channel << (sq * 6));
	*r = mask;
}

#endif
