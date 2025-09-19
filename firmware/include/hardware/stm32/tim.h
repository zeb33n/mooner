#ifndef STM32_TIM_H
#define STM32_TIM_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef union
{
	struct
	{
		uint32_t cen      :  1;
		uint32_t udis     :  1;
		uint32_t urs      :  1;
		uint32_t opm      :  1;
		uint32_t dir      :  1;
		uint32_t cms      :  2;
		uint32_t arpe     :  1;
		uint32_t ckd      :  2;
		uint32_t res10    :  1;
		uint32_t uifremap :  1;
		uint32_t dithen   :  1;
		uint32_t res31_13 : 19;
	};

	uint32_t mask;
} stm32_tim_c1_t;

typedef enum
{
	STM32_TIM_MMS_RESET  = 0,
	STM32_TIM_MMS_ENABLE = 1,
	STM32_TIM_MMS_UPDATE = 2,
} stm32_tim_mms_e;

typedef union
{
	struct
	{
		uint32_t ccpc     : 1;
		uint32_t res1     : 1;
		uint32_t ccus     : 1;
		uint32_t ccds     : 1;
		uint32_t mms      : 3;
		uint32_t t1s      : 1;
		uint32_t ois1     : 1;
		uint32_t ois1n    : 1;
		uint32_t ois2     : 1;
		uint32_t ois2n    : 1;
		uint32_t ois3     : 1;
		uint32_t ois3n    : 1;
		uint32_t ois4     : 1;
		uint32_t res15    : 1;
		uint32_t ois5     : 1;
		uint32_t res17    : 1;
		uint32_t ois6     : 1;
		uint32_t res19    : 1;
		uint32_t mms2     : 4;
		uint32_t res31_24 : 8;
	};

	uint32_t mask;
} stm32_tim_c2_t;

typedef union
{
	struct
	{
		uint32_t sms      :  3;
		uint32_t occs     :  1;
		uint32_t ts       :  3;
		uint32_t msm      :  1;
		uint32_t etf      :  4;
		uint32_t etps     :  2;
		uint32_t ece      :  1;
		uint32_t etp      :  1;
		uint32_t sms3     :  1;
		uint32_t res19_17 :  3;
		uint32_t ts4_3    :  2;
		uint32_t res31_22 : 10;
	};

	uint32_t mask;
} stm32_tim_smc_t;

typedef union
{
	struct
	{
		uint32_t uie      :  1;
		uint32_t cc1ie    :  1;
		uint32_t cc2ie    :  1;
		uint32_t cc3ie    :  1;
		uint32_t cc4ie    :  1;
		uint32_t comie    :  1;
		uint32_t tie      :  1;
		uint32_t bie      :  1;
		uint32_t ude      :  1;
		uint32_t cc1de    :  1;
		uint32_t cc2de    :  1;
		uint32_t cc3de    :  1;
		uint32_t cc4de    :  1;
		uint32_t comde    :  1;
		uint32_t tde      :  1;
		uint32_t res31_15 : 17;
	};

	uint32_t mask;
} stm32_tim_die_t;

typedef union
{
	struct
	{
		uint32_t uif      :  1;
		uint32_t cc1if    :  1;
		uint32_t cc2if    :  1;
		uint32_t cc3if    :  1;
		uint32_t cc4if    :  1;
		uint32_t comif    :  1;
		uint32_t tif      :  1;
		uint32_t bif      :  1;
		uint32_t b2if     :  1;
		uint32_t cc1of    :  1;
		uint32_t cc2of    :  1;
		uint32_t cc3of    :  1;
		uint32_t cc4of    :  1;
		uint32_t sbif     :  1;
		uint32_t res15_14 :  2;
		uint32_t cc5if    :  1;
		uint32_t cc6if    :  1;
		uint32_t res31_18 : 14;
	};

	uint32_t mask;
} stm32_tim_s_t;

typedef union
{
	struct
	{
		uint32_t ug       :  1;
		uint32_t cc1g     :  1;
		uint32_t cc2g     :  1;
		uint32_t cc3g     :  1;
		uint32_t cc4g     :  1;
		uint32_t comg     :  1;
		uint32_t tg       :  1;
		uint32_t bg       :  1;
		uint32_t b2g      :  1;
		uint32_t res31_9  : 23;
	};

	uint32_t mask;
} stm32_tim_eg_t;

typedef enum
{
	STM32_TIM_CCM_OCM_FROZEN                     =  0,
	STM32_TIM_CCM_OCM_MATCH                      =  1,
	STM32_TIM_CCM_OCM_MATCH_INVERTED             =  2,
	STM32_TIM_CCM_OCM_TOGGLE                     =  3,
	STM32_TIM_CCM_OCM_FORCE_ACTIVE               =  4,
	STM32_TIM_CCM_OCM_FORCE_INACTIVE             =  5,
	STM32_TIM_CCM_OCM_PWM                        =  6,
	STM32_TIM_CCM_OCM_PWM_INVERTED               =  7,
	STM32_TIM_CCM_OCM_RETRIGGERABLE_OPM          =  8,
	STM32_TIM_CCM_OCM_RETRIGGERABLE_OPM_INVERTED =  9,
	STM32_TIM_CCM_OCM_PWM_COMBINED               = 12,
	STM32_TIM_CCM_OCM_PWM_COMBINED_INVERTED      = 13,
	STM32_TIM_CCM_OCM_PWM_ASYMMETRIC             = 14,
	STM32_TIM_CCM_OCM_PWM_ASYMMETRIC_INVERTED    = 15,
} stm32_tim_ccm_ocm_e;

typedef union
{
	struct
	{
		uint32_t cc1s     :  2;
		uint32_t ic1psc   :  2;
		uint32_t ic1f     :  4;
		uint32_t cc2s     :  2;
		uint32_t ic2psc   :  2;
		uint32_t ic2f     :  4;
		uint32_t res31_16 : 16;
	} ic;

	struct
	{
		uint32_t cc1s     : 2;
		uint32_t oc1fe    : 1;
		uint32_t oc1pe    : 1;
		uint32_t oc1m     : 3;
		uint32_t oc1ce    : 1;
		uint32_t cc2s     : 2;
		uint32_t oc2fe    : 1;
		uint32_t oc2pe    : 1;
		uint32_t oc2m     : 3;
		uint32_t oc2ce    : 1;
		uint32_t oc1m3    : 1;
		uint32_t res23_17 : 7;
		uint32_t oc2m3    : 1;
		uint32_t res31_25 : 7;
	} oc;

	uint32_t mask;
} stm32_tim_ccm1_t;

typedef union
{
	struct
	{
		uint32_t cc3s     :  2;
		uint32_t ic3psc   :  2;
		uint32_t ic3f     :  4;
		uint32_t cc4s     :  2;
		uint32_t ic4psc   :  2;
		uint32_t ic4f     :  4;
		uint32_t res31_16 : 16;
	} ic;

	struct
	{
		uint32_t cc3s     : 2;
		uint32_t oc3fe    : 1;
		uint32_t oc3pe    : 1;
		uint32_t oc3m     : 3;
		uint32_t oc3ce    : 1;
		uint32_t cc4s     : 2;
		uint32_t oc4fe    : 1;
		uint32_t oc4pe    : 1;
		uint32_t oc4m     : 3;
		uint32_t oc4ce    : 1;
		uint32_t oc3m3    : 1;
		uint32_t res23_17 : 7;
		uint32_t oc4m3    : 1;
		uint32_t res31_25 : 7;
	} oc;

	uint32_t mask;
} stm32_tim_ccm2_t;

typedef union
{
	struct
	{
		uint32_t cc1e     :  1;
		uint32_t cc1p     :  1;
		uint32_t cc1ne    :  1;
		uint32_t cc1np    :  1;
		uint32_t cc2e     :  1;
		uint32_t cc2p     :  1;
		uint32_t cc2ne    :  1;
		uint32_t cc2np    :  1;
		uint32_t cc3e     :  1;
		uint32_t cc3p     :  1;
		uint32_t cc3ne    :  1;
		uint32_t cc3np    :  1;
		uint32_t cc4e     :  1;
		uint32_t cc4p     :  1;
		uint32_t res14    :  1;
		uint32_t cc4np    :  1;
		uint32_t cc5e     :  1;
		uint32_t cc5p     :  1;
		uint32_t res19_18 :  2;
		uint32_t cc6e     :  1;
		uint32_t cc6p     :  1;
		uint32_t res31_22 : 10;
	};

	uint32_t mask;
} stm32_tim_cce_t;

typedef union
{
	struct
	{
		uint32_t cnt    : 31;
		uint32_t uifcpy :  1;
	};

	uint32_t mask;
} stm32_tim_cnt_t;

typedef union
{
	struct
	{
		uint32_t dtg      : 8;
		uint32_t lock     : 2;
		uint32_t ossi     : 1;
		uint32_t ossr     : 1;
		uint32_t bke      : 1;
		uint32_t bkp      : 1;
		uint32_t aoe      : 1;
		uint32_t moe      : 1;
		uint32_t bkf      : 4;
		uint32_t bk2f     : 4;
		uint32_t bk2e     : 1;
		uint32_t bk2p     : 1;
		uint32_t bkdsrm   : 1;
		uint32_t bk2dsrm  : 1;
		uint32_t bkbid    : 1;
		uint32_t bk2bid   : 1;
		uint32_t res31_30 : 2;
	};

	uint32_t mask;
} stm32_tim_bdt_t;

typedef union
{
	struct
	{
		uint32_t dba      :  5;
		uint32_t res7_5   :  3;
		uint32_t dbl      :  5;
		uint32_t res31_13 : 19;
	};

	uint32_t mask;
} stm32_tim_dc_t;

typedef union
{
	struct
	{
		uint32_t res1_0   : 2;
		uint32_t oc5fe    : 1;
		uint32_t oc5pe    : 1;
		uint32_t oc5m     : 3;
		uint32_t oc5ce    : 1;
		uint32_t res9_8   : 2;
		uint32_t oc6fe    : 1;
		uint32_t oc6pe    : 1;
		uint32_t oc6m     : 3;
		uint32_t oc6ce    : 1;
		uint32_t oc5m3    : 1;
		uint32_t res23_17 : 7;
		uint32_t oc6m3    : 1;
		uint32_t res31_25 : 7;
	} oc;

	uint32_t mask;
} stm32_tim_ccm3_t;

typedef union
{
	struct
	{
		uint32_t ccr5     : 16;
		uint32_t res28_16 : 13;
		uint32_t gc5c1    :  1;
		uint32_t gc5c2    :  1;
		uint32_t gc5c3    :  1;
	};

	uint32_t mask;
} stm32_tim_cc5_t;

typedef union
{
	struct
	{
		uint32_t bkine    :  1;
		uint32_t res8_1   :  8;
		uint32_t bkinp    :  1;
		uint32_t res13_10 :  4;
		uint32_t etrsel   :  4;
		uint32_t res31_18 : 14;
	};

	uint32_t mask;
} stm32_tim_af1_t;

typedef union
{
	struct
	{
		uint32_t bk2ine   :  1;
		uint32_t res8_1   :  8;
		uint32_t bk2inp   :  1;
		uint32_t res31_10 : 22;
	};

	uint32_t mask;
} stm32_tim_af2_t;

typedef union
{
	struct
	{
		uint32_t ti1sel   : 4;
		uint32_t res7_4   : 4;
		uint32_t ti2sel   : 4;
		uint32_t res15_12 : 4;
		uint32_t ti3sel   : 4;
		uint32_t res23_20 : 4;
		uint32_t ti4sel   : 4;
		uint32_t res31_28 : 4;
	};

	uint32_t mask;
} stm32_tim_tisel_t;


typedef struct
{
	stm32_tim_c1_t     c1;
	stm32_tim_c2_t     c2;
	stm32_tim_smc_t    smc;
	stm32_tim_die_t    die;
	stm32_tim_s_t      s;
	stm32_tim_eg_t     eg;
	stm32_tim_ccm1_t   ccm1;
	stm32_tim_ccm2_t   ccm2;
	stm32_tim_cce_t    cce;
	stm32_tim_cnt_t    cnt;
	uint32_t           psc;
	uint32_t           ar;
	uint32_t           rc;
	uint32_t           cc1;
	uint32_t           cc2;
	uint32_t           cc3;
	uint32_t           cc4;
	stm32_tim_bdt_t    bdt;
	stm32_tim_dc_t     dc;
	uint32_t           dma;
	uint32_t           res20;
	stm32_tim_ccm3_t   ccm3;
	stm32_tim_cc5_t    cc5;
	uint32_t           cc6;
	stm32_tim_af1_t    af1;
	stm32_tim_af2_t    af2;
	stm32_tim_tisel_t  tisel;
	uint32_t           res255_27[229];
} stm32_tim_t;


static volatile stm32_tim_t * const stm32_tim1  = (volatile stm32_tim_t *)STM32_MMAP_TIM1;
static volatile stm32_tim_t * const stm32_tim2  = (volatile stm32_tim_t *)STM32_MMAP_TIM2;
static volatile stm32_tim_t * const stm32_tim3  = (volatile stm32_tim_t *)STM32_MMAP_TIM3;
static volatile stm32_tim_t * const stm32_tim4  = (volatile stm32_tim_t *)STM32_MMAP_TIM4;
static volatile stm32_tim_t * const stm32_tim5  = (volatile stm32_tim_t *)STM32_MMAP_TIM5;
static volatile stm32_tim_t * const stm32_tim6  = (volatile stm32_tim_t *)STM32_MMAP_TIM6;
static volatile stm32_tim_t * const stm32_tim7  = (volatile stm32_tim_t *)STM32_MMAP_TIM7;
static volatile stm32_tim_t * const stm32_tim8  = (volatile stm32_tim_t *)STM32_MMAP_TIM8;
static volatile stm32_tim_t * const stm32_tim15 = (volatile stm32_tim_t *)STM32_MMAP_TIM15;
static volatile stm32_tim_t * const stm32_tim16 = (volatile stm32_tim_t *)STM32_MMAP_TIM16;
static volatile stm32_tim_t * const stm32_tim17 = (volatile stm32_tim_t *)STM32_MMAP_TIM17;
static volatile stm32_tim_t * const stm32_tim20 = (volatile stm32_tim_t *)STM32_MMAP_TIM20;

#endif
