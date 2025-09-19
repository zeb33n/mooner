#ifndef STM32_PWR_H
#define STM32_PWR_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef enum
{
	STM32_PWR_LPMS_STOP_0   = 0,
	STM32_PWR_LPMS_STOP_1   = 1,
	STM32_PWR_LPMS_STANDBY  = 3,
	STM32_PWR_LPMS_SHUTDOWN = 4,
} stm32_pwr_lpms_e;

typedef enum
{
	STM32_PWR_VOS_RANGE_1 = 1,
	STM32_PWR_VOS_RANGE_2 = 2,
} stm32_pwr_vos_e;

typedef union
{
	struct
	{
		uint32_t lpms     :  3;
		uint32_t res7_3   :  5;
		uint32_t dbp      :  1;
		uint32_t vos      :  2;
		uint32_t res13_11 :  3;
		uint32_t lpr      :  1;
		uint32_t res31_15 : 17;
	};

	uint32_t mask;
} stm32_pwr_c1_t;

typedef union
{
	struct
	{
		uint32_t pvde    :  1;
		uint32_t pls     :  3;
		uint32_t res5_4  :  2;
		uint32_t pvme1   :  1;
		uint32_t pvme2   :  1;
		uint32_t res31_8 : 24;
	};

	uint32_t mask;
} stm32_pwr_c2_t;

typedef union
{
	struct
	{
		uint32_t ewup1       :  1;
		uint32_t ewup2       :  1;
		uint32_t ewup3       :  1;
		uint32_t ewup4       :  1;
		uint32_t ewup5       :  1;
		uint32_t res7_5      :  3;
		uint32_t rrs         :  1;
		uint32_t res9        :  1;
		uint32_t apc         :  1;
		uint32_t res12_11    :  2;
		uint32_t ucpd1_stdby :  1;
		uint32_t ucpd1_dbdis :  1;
		uint32_t eiwul       :  1;
		uint32_t res31_16    : 16;
	};

	uint32_t mask;
} stm32_pwr_c3_t;

typedef union
{
	struct
	{
		uint32_t wup1     :  1;
		uint32_t wup2     :  1;
		uint32_t wup3     :  1;
		uint32_t wup4     :  1;
		uint32_t wup5     :  1;
		uint32_t res7_5   :  3;
		uint32_t vbe      :  1;
		uint32_t vbrs     :  1;
		uint32_t res31_10 : 22;
	};

	uint32_t mask;
} stm32_pwr_c4_t;

typedef union
{
	struct
	{
		uint32_t wuf1     :  1;
		uint32_t wuf2     :  1;
		uint32_t wuf3     :  1;
		uint32_t wuf4     :  1;
		uint32_t wuf5     :  1;
		uint32_t res7_5   :  3;
		uint32_t sbf      :  1;
		uint32_t res14_9  :  6;
		uint32_t wufi     :  1;
		uint32_t res31_16 : 16;
	};

	uint32_t mask;
} stm32_pwr_s1_t;

typedef union
{
	struct
	{
		uint32_t res6_0    :  7;
		uint32_t flash_rdy :  1;
		uint32_t reglps    :  1;
		uint32_t reglpf    :  1;
		uint32_t vosf      :  1;
		uint32_t pvdo      :  1;
		uint32_t res13_12  :  2;
		uint32_t pvmo1     :  1;
		uint32_t pvmo2     :  1;
		uint32_t res31_16  : 16;
	};

	uint32_t mask;
} stm32_pwr_s2_t;

typedef enum
{
	STM32_PWR_R1MODE_BOOST  = 0,
	STM32_PWR_R1MODE_NORMAL = 1,
} stm32_pwr_r1mode_e;

typedef union
{
	struct
	{
		uint32_t res7_0  :  8;
		uint32_t r1mode  :  1;
		uint32_t res31_9 : 23;
	};

	uint32_t mask;
} stm32_pwr_c5_t;


typedef struct
{
	stm32_pwr_c1_t c1;
	stm32_pwr_c2_t c2;
	stm32_pwr_c3_t c3;
	stm32_pwr_c4_t c4;
	stm32_pwr_s1_t s1;
	stm32_pwr_s2_t s2;
	stm32_pwr_s1_t sc;
	uint32_t       puca;
	uint32_t       pdca;
	uint32_t       pucb;
	uint32_t       pdcb;
	uint32_t       pucc;
	uint32_t       pdcc;
	uint32_t       pucd;
	uint32_t       pdcd;
	uint32_t       puce;
	uint32_t       pdce;
	uint32_t       pucf;
	uint32_t       pdcf;
	uint32_t       pucg;
	uint32_t       pdcg;
	uint32_t       res31_22[10];
	stm32_pwr_c5_t c5;
	uint32_t       res255_33[223];
} stm32_pwr_t;

static volatile stm32_pwr_t * const stm32_pwr = (volatile stm32_pwr_t *)STM32_MMAP_PWR;

#endif
