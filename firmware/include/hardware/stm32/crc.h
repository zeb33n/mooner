#ifndef STM32_CRC_H
#define STM32_CRC_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef enum
{
	STM32_CRC_POLYSIZE_32 = 0,
	STM32_CRC_POLYSIZE_16 = 1,
	STM32_CRC_POLYSIZE_8  = 2,
	STM32_CRC_POLYSIZE_7  = 3,	
} stm32_crc_polysize_e;

typedef union
{
	struct
	{
		uint32_t reset    :  1;
		uint32_t res2_1   :  2;
		uint32_t polysize :  2;
		uint32_t rev_in   :  2;
		uint32_t rev_out  :  1;
		uint32_t res31_8  : 24;
	};

	uint32_t mask;
} stm32_crc_c_t;


typedef struct
{
	uint32_t      d;
	uint32_t      id;
	stm32_crc_c_t c;
	uint32_t      res3;
	uint32_t      init;
	uint32_t      pol;
	uint32_t      res255_6[250];
} stm32_crc_t;

static volatile stm32_crc_t * const stm32_crc = (volatile stm32_crc_t *)STM32_MMAP_CRC;

#endif
