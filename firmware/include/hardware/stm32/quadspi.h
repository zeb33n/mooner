#ifndef STM32_QUADSPI_H
#define STM32_QUADSPI_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef union
{
	struct
	{
		uint32_t en        : 1;
		uint32_t abort     : 1;
		uint32_t dmaen     : 1;
		uint32_t tcen      : 1;
		uint32_t sshift    : 1;
		uint32_t res5      : 1;
		uint32_t dfm       : 1;
		uint32_t fsel      : 1;
		uint32_t fthres    : 4;
		uint32_t res15_12  : 4;
		uint32_t teie      : 1;
		uint32_t tcie      : 1;
		uint32_t ftie      : 1;
		uint32_t smie      : 1;
		uint32_t toie      : 1;
		uint32_t res21     : 1;
		uint32_t apms      : 1;
		uint32_t pmm       : 1;
		uint32_t prescaler : 8;
	};

	uint32_t mask;
} stm32_quadspi_c_t;

typedef union
{
	struct
	{
		uint32_t ckmode    :  1;
		uint32_t res7_1    :  7;
		uint32_t csht      :  3;
		uint32_t res15_11  :  5;
		uint32_t fsize     :  5;
		uint32_t res31_21  : 11;
	};

	uint32_t mask;
} stm32_quadspi_dc_t;

typedef union
{
	struct
	{
		uint32_t tef       :  1;
		uint32_t tcf       :  1;
		uint32_t ftf       :  1;
		uint32_t smf       :  1;
		uint32_t tof       :  1;
		uint32_t busy      :  1;
		uint32_t res7_6    :  2;
		uint32_t flevel    :  5;
		uint32_t res31_13  : 19;
	};

	uint32_t mask;
} stm32_quadspi_s_t;

typedef union
{
	struct
	{
		uint32_t instruction : 8;
		uint32_t imode       : 2;
		uint32_t admode      : 2;
		uint32_t adsize      : 2;
		uint32_t abmode      : 2;
		uint32_t absize      : 2;
		uint32_t dcyc        : 5;
		uint32_t res23       : 1;
		uint32_t dmode       : 2;
		uint32_t fmode       : 2;
		uint32_t sioo        : 1;
		uint32_t res29       : 1;
		uint32_t dhhc        : 1;
		uint32_t ddrm        : 1;
	};

	uint32_t mask;
} stm32_quadspi_cc_t;


typedef struct
{
	stm32_quadspi_c_t  c;
	stm32_quadspi_dc_t dc;
	stm32_quadspi_s_t  s;
	stm32_quadspi_s_t  fc;
	uint32_t           dl;
	stm32_quadspi_cc_t cc;
	uint32_t           ab;
	uint32_t           d;
	uint32_t           psmk;
	uint32_t           psma;
	uint32_t           pi;
	uint32_t           lpt;
	uint32_t           res255_13[243];
} stm32_quadspi_t;

static volatile stm32_quadspi_t * const stm32_quadspi = (volatile stm32_quadspi_t *)STM32_MMAP_QUADSPI;

#endif
