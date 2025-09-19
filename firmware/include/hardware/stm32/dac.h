#ifndef STM32_DAC_H
#define STM32_DAC_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


#define STM32_DAC_TTRIM_US 50

typedef enum
{
	STM32_DAC1_TRG_CK_LS            =  0,
	STM32_DAC1_TRG_TIM8_TRGO        =  1,
	STM32_DAC1_TRG_TIM7_TRGO        =  2,
	STM32_DAC1_TRG_TIM15_TRGO       =  3,
	STM32_DAC1_TRG_TIM2_TRGO        =  4,
	STM32_DAC1_TRG_TIM4_TRGO        =  5,
	STM32_DAC1_TRG_EXTI9            =  6,
	STM32_DAC1_TRG_TIM6_TRGO        =  7,
	STM32_DAC1_TRG_TIM3_TRGO        =  8,
	STM32_DAC1_TRG_HRTIM_RESET_TRG1 =  9,
	STM32_DAC1_TRG_HRTIM_RESET_TRG2 = 10,
	STM32_DAC1_TRG_HRTIM_RESET_TRG3 = 11,
	STM32_DAC1_TRG_HRTIM_RESET_TRG4 = 12,
	STM32_DAC1_TRG_HRTIM_RESET_TRG5 = 13,
	STM32_DAC1_TRG_HRTIM_RESET_TRG6 = 14,
	STM32_DAC1_TRG_HRTIM_TRG2       = 15,
} stm32_dac1_trg_e;

typedef enum
{
	STM32_DAC2_TRG_CK_LS            =  0,
	STM32_DAC2_TRG_TIM8_TRGO        =  1,
	STM32_DAC2_TRG_TIM7_TRGO        =  2,
	STM32_DAC2_TRG_TIM15_TRGO       =  3,
	STM32_DAC2_TRG_TIM2_TRGO        =  4,
	STM32_DAC2_TRG_TIM4_TRGO        =  5,
	STM32_DAC2_TRG_EXTI9            =  6,
	STM32_DAC2_TRG_TIM6_TRGO        =  7,
	STM32_DAC2_TRG_TIM3_TRGO        =  8,
	STM32_DAC2_TRG_HRTIM_RESET_TRG1 =  9,
	STM32_DAC2_TRG_HRTIM_RESET_TRG2 = 10,
	STM32_DAC2_TRG_HRTIM_RESET_TRG3 = 11,
	STM32_DAC2_TRG_HRTIM_RESET_TRG4 = 12,
	STM32_DAC2_TRG_HRTIM_RESET_TRG5 = 13,
	STM32_DAC2_TRG_HRTIM_RESET_TRG6 = 14,
	STM32_DAC2_TRG_HRTIM_TRG2       = 15,
} stm32_dac2_trg_e;

typedef enum
{
	STM32_DAC3_TRG_CK_LS            =  0,
	STM32_DAC3_TRG_TIM1_TRGO        =  1,
	STM32_DAC3_TRG_TIM7_TRGO        =  2,
	STM32_DAC3_TRG_TIM15_TRGO       =  3,
	STM32_DAC3_TRG_TIM2_TRGO        =  4,
	STM32_DAC3_TRG_TIM4_TRGO        =  5,
	STM32_DAC3_TRG_EXTI9            =  6,
	STM32_DAC3_TRG_TIM6_TRGO        =  7,
	STM32_DAC3_TRG_TIM3_TRGO        =  8,
	STM32_DAC3_TRG_HRTIM_RESET_TRG1 =  9,
	STM32_DAC3_TRG_HRTIM_RESET_TRG2 = 10,
	STM32_DAC3_TRG_HRTIM_RESET_TRG3 = 11,
	STM32_DAC3_TRG_HRTIM_RESET_TRG4 = 12,
	STM32_DAC3_TRG_HRTIM_RESET_TRG5 = 13,
	STM32_DAC3_TRG_HRTIM_RESET_TRG6 = 14,
	STM32_DAC3_TRG_HRTIM_TRG3       = 15,
} stm32_dac3_trg_e;

typedef enum
{
	STM32_DAC4_TRG_CK_LS      =  0,
	STM32_DAC4_TRG_TIM8_TRGO  =  1,
	STM32_DAC4_TRG_TIM7_TRGO  =  2,
	STM32_DAC4_TRG_TIM15_TRGO =  3,
	STM32_DAC4_TRG_TIM2_TRGO  =  4,
	STM32_DAC4_TRG_TIM4_TRGO  =  5,
	STM32_DAC4_TRG_EXTI9      =  6,
	STM32_DAC4_TRG_TIM6_TRGO  =  7,
} stm32_dac4_trg_e;

typedef union
{
	struct
	{
		uint32_t en1       :  1;
		uint32_t ten1      :  1;
		uint32_t tsel1     :  4;
		uint32_t wave1     :  2;
		uint32_t mamp1     :  4;
		uint32_t dmaen1    :  1;
		uint32_t dmaudrie1 :  1;
		uint32_t cen1      :  1;
		uint32_t res15     :  1;
		uint32_t en2       :  1;
		uint32_t ten2      :  1;
		uint32_t tsel2     :  4;
		uint32_t wave2     :  2;
		uint32_t mamp2     :  4;
		uint32_t dmaen2    :  1;
		uint32_t dmaudrie2 :  1;
		uint32_t cen2      :  1;
		uint32_t res31     :  1;
	};

	uint32_t mask;
} stm32_dac_c_t;

typedef union
{
	struct
	{
		uint32_t swtrig1  :  1;
		uint32_t swtrig2  :  1;
		uint32_t res15_2  : 14;
		uint32_t swtrigb1 :  1;
		uint32_t swtrigb2 :  1;
		uint32_t res31_18 : 14;
	};

	uint32_t mask;
} stm32_dac_swtrg_t;

typedef union
{
	struct
	{
		uint32_t a        : 12;
		uint32_t res15_12 :  4;
		uint32_t b        : 12;
		uint32_t res31_28 :  4;
	};

	uint32_t mask;
} stm32_dac_dh12r_t;

typedef union
{
	struct
	{
		uint32_t res3_0   :  4;
		uint32_t a        : 12;
		uint32_t res19_16 :  4;
		uint32_t b        : 12;
	};

	uint32_t mask;
} stm32_dac_dh12l_t;

typedef union
{
	struct
	{
		uint32_t a        :  8;
		uint32_t b        :  8;
		uint32_t res31_16 : 16;
	};

	uint32_t mask;
} stm32_dac_dh8r_t;

typedef union
{
	struct
	{
		uint32_t res10_0   : 11;
		uint32_t dac1rdy   :  1;
		uint32_t dorstat1  :  1;
		uint32_t dmaudr1   :  1;
		uint32_t cal_flag1 :  1;
		uint32_t bwst1     :  1;
		uint32_t res26_16  : 11;
		uint32_t dac2rdy   :  1;
		uint32_t dorstat2  :  1;
		uint32_t dmaudr2   :  1;
		uint32_t cal_flag2 :  1;
		uint32_t bwst2     :  1;
	};

	uint32_t mask;
} stm32_dac_s_t;

typedef union
{
	struct
	{
		uint32_t otrim1   :  5;
		uint32_t res15_5  : 11;
		uint32_t otrim2   :  5;
		uint32_t res31_21 : 11;
	};

	uint32_t mask;
} stm32_dac_cc_t;

typedef union
{
	struct
	{
		uint32_t mode1      : 3;
		uint32_t res7_3     : 5;
		uint32_t dmadouble1 : 1;
		uint32_t sinformat1 : 1;
		uint32_t res13_10   : 4;
		uint32_t hfsel      : 2;
		uint32_t mode2      : 3;
		uint32_t res23_19   : 5;
		uint32_t dmadouble2 : 1;
		uint32_t sinformat2 : 1;
		uint32_t res31_26   : 6;
	};

	uint32_t mask;
} stm32_dac_mc_t;

typedef union
{
	struct
	{
		uint32_t thold1   : 10;
		uint32_t res15_10 :  6;
		uint32_t thold2   : 10;
		uint32_t res31_26 :  6;
	};

	uint32_t mask;
} stm32_dac_shh_t;

typedef union
{
	struct
	{
		uint32_t trefresh1 : 8;
		uint32_t res15_8   : 8;
		uint32_t trefresh2 : 8;
		uint32_t res31_24  : 8;
	};

	uint32_t mask;
} stm32_dac_shr_t;

typedef union
{
	struct
	{
		uint32_t strstdata : 12;
		uint32_t stdir     :  1;
		uint32_t res15_13  :  3;
		uint32_t stincdata : 16;
	};

	uint32_t mask;
} stm32_dac_st_t;

typedef union
{
	struct
	{
		uint32_t strsttrigsel1 : 4;
		uint32_t res7_4        : 4;
		uint32_t stinctrigsel1 : 4;
		uint32_t res15_12      : 4;
		uint32_t strsttrigsel2 : 4;
		uint32_t res23_20      : 4;
		uint32_t stinctrigsel2 : 4;
		uint32_t res31_28      : 4;
	};

	uint32_t mask;
} stm32_dac_stmod_t;


typedef struct
{
	stm32_dac_c_t     c;
	stm32_dac_swtrg_t swtrg;
	stm32_dac_dh12r_t dh12r1;
	stm32_dac_dh12l_t dh12l1;
	stm32_dac_dh8r_t  dh8r1;
	stm32_dac_dh12r_t dh12r2;
	stm32_dac_dh12l_t dh12l2;
	stm32_dac_dh8r_t  dh8r2;
	stm32_dac_dh12r_t dh12rd;
	stm32_dac_dh12l_t dh12ld;
	stm32_dac_dh8r_t  dh8rd;
	stm32_dac_dh12r_t dor1;
	stm32_dac_dh12r_t dor2;
	stm32_dac_s_t     s;
	stm32_dac_cc_t    cc;
	stm32_dac_mc_t    mc;
	uint32_t          shs1;
	uint32_t          shs2;
	stm32_dac_shh_t   shh;
	stm32_dac_shr_t   shr;
	uint32_t          res21_20[2];
	stm32_dac_st_t    st1;
	stm32_dac_st_t    st2;
	stm32_dac_stmod_t stmod;
	uint32_t          res255_25[230];
} stm32_dac_t;

static volatile stm32_dac_t * const stm32_dac1 = (volatile stm32_dac_t *)STM32_MMAP_DAC1;
static volatile stm32_dac_t * const stm32_dac2 = (volatile stm32_dac_t *)STM32_MMAP_DAC2;
static volatile stm32_dac_t * const stm32_dac3 = (volatile stm32_dac_t *)STM32_MMAP_DAC3;
static volatile stm32_dac_t * const stm32_dac4 = (volatile stm32_dac_t *)STM32_MMAP_DAC4;

#endif
