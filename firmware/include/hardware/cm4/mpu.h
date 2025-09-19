#ifndef CM4_MPU_H
#define CM4_MPU_H

#include <stdint.h>


typedef union
{
	struct
	{
		uint32_t separate : 1;
		uint32_t res7_1   : 7;
		uint32_t dregion  : 8;
		uint32_t iregion  : 8;
		uint32_t res31_24 : 8;
	};

	uint32_t mask;
} cm4_mpu_type_t;

typedef union
{
	struct
	{
		uint32_t enable     :  1;
		uint32_t hfnmiena   :  1;
		uint32_t privdefena :  1;
		uint32_t res31_3    : 29;
	};

	uint32_t mask;
} cm4_mpu_ctrl_t;

typedef union
{
	struct
	{
		uint32_t region :  4;
		uint32_t valid  :  1;
		uint32_t addr   : 27;
	};

	uint32_t mask;
} cm4_mpu_rba_t;

typedef union
{
	struct
	{
		uint32_t enable   : 1;
		uint32_t size     : 5;
		uint32_t res7_6   : 2;
		uint32_t srd      : 8;
		uint32_t b        : 1;
		uint32_t c        : 1;
		uint32_t s        : 1;
		uint32_t tex      : 3;
		uint32_t res23_22 : 2;
		uint32_t ap       : 3;
		uint32_t res27    : 1;
		uint32_t xn       : 1;
		uint32_t res31_29 : 3;
	};

	uint32_t mask;
} cm4_mpu_ras_t;


typedef struct
{
	const cm4_mpu_type_t type;
	cm4_mpu_ctrl_t       ctrl;
	uint32_t             rn;
	cm4_mpu_rba_t        rba;
	cm4_mpu_ras_t        ras;
	cm4_mpu_rba_t        rba_a1;
	cm4_mpu_ras_t        ras_a1;
	cm4_mpu_rba_t        rba_a2;
	cm4_mpu_ras_t        ras_a2;
	cm4_mpu_rba_t        rba_a3;
	cm4_mpu_ras_t        ras_a3;
} cm4_mpu_t;


static volatile cm4_mpu_t * const cm4_mpu
	= (volatile cm4_mpu_t *)0xE000ED90;

#endif
