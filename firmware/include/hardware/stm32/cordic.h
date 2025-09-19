#ifndef STM32_CORDIC_H
#define STM32_CORDIC_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef enum
{
	STM32_CORDIC_FUNC_COSINE            = 0,
	STM32_CORDIC_FUNC_SINE              = 1,
	STM32_CORDIC_FUNC_PHASE             = 2,
	STM32_CORDIC_FUNC_MODULUS           = 3,
	STM32_CORDIC_FUNC_ARCTANGENT        = 4,
	STM32_CORDIC_FUNC_HYPERBOLIC_COSINE = 5,
	STM32_CORDIC_FUNC_HYPERBOLIC_SINE   = 6,
	STM32_CORDIC_FUNC_ARCTANH           = 7,
	STM32_CORDIC_FUNC_NATURAL_LOGARITHM = 8,
	STM32_CORDIC_FUNC_SQUARE_ROOT       = 9,
} stm32_cordic_func_e;

typedef enum
{
	STM32_CORDIC_SIZE_32_BIT = 0,
	STM32_CORDIC_SIZE_16_BIT = 1,
} stm32_cordic_size_e;

typedef union
{
	struct
	{
		uint32_t       func      : 4;
		uint32_t       precision : 4;
		uint32_t       scale     : 3;
		uint32_t       res15_11  : 5;
		uint32_t       ien       : 1;
		uint32_t       dmaren    : 1;
		uint32_t       dmawen    : 1;
		uint32_t       nres      : 1;
		uint32_t       nargs     : 1;
		uint32_t       ressize   : 1;
		uint32_t       argsize   : 1;
		uint32_t       res30_23  : 8;
		const uint32_t rrdy      : 1;
	};

	uint32_t mask;
} stm32_cordic_cs_t;


typedef struct
{
	stm32_cordic_cs_t cs;
	uint32_t          wdata;
	uint32_t          rdata;
	uint32_t          res255_3[253];
} stm32_cordic_t;

static volatile stm32_cordic_t * const stm32_cordic = (volatile stm32_cordic_t *)STM32_MMAP_CORDIC;

#endif
