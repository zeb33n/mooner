#ifndef STM32_VREFBUF_H
#define STM32_VREFBUF_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef enum
{
	STM32_VREFBUF_VRS_2V048 = 0,
	STM32_VREFBUF_VRS_2V5   = 1,
	STM32_VREFBUF_VRS_2V9   = 2,
} stm32_vrefbuf_vrs_e;

typedef union
{
	struct
	{
		uint32_t envr    :  1;
		uint32_t hiz     :  1;
		uint32_t res2    :  1;
		uint32_t vrr     :  1;
		uint32_t vrs     :  2;
		uint32_t res31_6 : 26;
	};

	uint32_t mask;
} stm32_vrefbuf_cs_t;


typedef struct
{
	stm32_vrefbuf_cs_t cs;
	uint32_t           cc;
	uint32_t           res255_2[254];
} stm32_vrefbuf_t;

static volatile stm32_vrefbuf_t * const stm32_vrefbuf = (volatile stm32_vrefbuf_t *)STM32_MMAP_VREFBUF;

#endif
