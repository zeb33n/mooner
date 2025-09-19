#ifndef STM32_CRS_H
#define STM32_CRS_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef union
{
	struct
	{
		uint32_t syncokie   :  1;
		uint32_t syncwarnie :  1;
		uint32_t errie      :  1;
		uint32_t esyncie    :  1;
		uint32_t res4       :  1;
		uint32_t cen        :  1;
		uint32_t autotrimen :  1;
		uint32_t swsync     :  1;
		uint32_t trim       :  7;
		uint32_t res31_15   : 17;
	};

	uint32_t mask;
} stm32_crs_c_t;

typedef enum
{
	STM32_CRS_SYNC_SRC_GPIO    = 0,
	STM32_CRS_SYNC_SRC_LSE     = 1,
	STM32_CRS_SYNC_SRC_USB_SOF = 2,
} stm32_crs_sync_src_e;

typedef union
{
	struct
	{
		uint32_t reload  : 16;
		uint32_t felim   :  8;
		uint32_t syncdiv :  3;
		uint32_t res27   :  1;
		uint32_t syncsrc :  2;
		uint32_t res30   :  1;
		uint32_t syncpol :  1;
	};

	uint32_t mask;
} stm32_crs_cfg_t;

typedef union
{
	struct
	{
		uint32_t syncokf   :  1;
		uint32_t syncwarnf :  1;
		uint32_t errf      :  1;
		uint32_t esyncf    :  1;
		uint32_t res7_4    :  4;
		uint32_t syncerr   :  1;
		uint32_t syncmiss  :  1;
		uint32_t trimovf   :  1;
		uint32_t res14_11  :  4;
		uint32_t fedir     :  1;
		uint32_t fecap     : 16;
	};

	uint32_t mask;
} stm32_crs_is_t;

typedef union
{
	struct
	{
		uint32_t syncokc   :  1;
		uint32_t syncwarnc :  1;
		uint32_t errc      :  1;
		uint32_t esyncc    :  1;
		uint32_t res31_4   : 28;
	};

	uint32_t mask;
} stm32_crs_ic_t;


typedef struct
{
	stm32_crs_c_t   c;
	stm32_crs_cfg_t cfg;
	stm32_crs_is_t  is;
	stm32_crs_ic_t  ic;
	uint32_t        res255_4[252];
} stm32_crs_t;

static volatile stm32_crs_t * const stm32_crs = (volatile stm32_crs_t *)STM32_MMAP_CRS;

#endif
