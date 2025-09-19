#ifndef STM32_TAMP_H
#define STM32_TAMP_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef union
{
	struct
	{
		uint32_t tamp1    :  1;
		uint32_t tamp2    :  1;
		uint32_t tamp3    :  1;
		uint32_t res18_3  : 15;
		uint32_t itamp3   :  1;
		uint32_t itamp4   :  1;
		uint32_t itamp5   :  1;
		uint32_t itamp6   :  1;
		uint32_t res31_22 : 10;
	};

	uint32_t mask;
} stm32_tamp_c1_ie_s_mis_sc_t;

typedef union
{
	struct
	{
		uint32_t tamp1noer :  1;
		uint32_t tamp2noer :  1;
		uint32_t tamp3noer :  1;
		uint32_t res16_3   : 13;
		uint32_t tamp1msk  :  1;
		uint32_t tamp2msk  :  1;
		uint32_t tamp3msk  :  1;
		uint32_t res22_19  :  5;
		uint32_t tamp1trg  :  1;
		uint32_t tamp2trg  :  1;
		uint32_t tamp3trg  :  1;
		uint32_t res31_27  :  5;
	};

	uint32_t mask;
} stm32_tamp_c2_t;

typedef union
{
	struct
	{
		uint32_t tampfreq :  3;
		uint32_t tampflt  :  2;
		uint32_t tampprch :  2;
		uint32_t tampudis :  1;
		uint32_t res31_24 : 24;
	};

	uint32_t mask;
} stm32_tamp_fltc_t;


typedef struct
{
	stm32_tamp_c1_ie_s_mis_sc_t c1;
	stm32_tamp_c2_t             c2;
	uint32_t                    res2;
	stm32_tamp_fltc_t           fltc;
	uint32_t                    res10_4[7];
	stm32_tamp_c1_ie_s_mis_sc_t ie;
	stm32_tamp_c1_ie_s_mis_sc_t s;
	stm32_tamp_c1_ie_s_mis_sc_t mis;
	uint32_t                    res14;
	stm32_tamp_c1_ie_s_mis_sc_t sc;
	uint32_t                    res63_16[48];
	uint32_t                    bkp[32];
	uint32_t                    res255_96[160];
} stm32_tamp_t;

static volatile stm32_tamp_t * const stm32_tamp = (volatile stm32_tamp_t *)STM32_MMAP_TAMP;

#endif
