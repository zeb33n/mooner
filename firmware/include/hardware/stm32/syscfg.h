#ifndef STM32_SYSCFG_H
#define STM32_SYSCFG_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef enum
{
	STM32_SYSCFG_MEM_MODE_MAIN_FLASH   = 0,
	STM32_SYSCFG_MEM_MODE_SYSTEM_FLASH = 1,
	STM32_SYSCFG_MEM_MODE_FSMC         = 2,
	STM32_SYSCFG_MEM_MODE_SRAM1        = 3,
	STM32_SYSCFG_MEM_MODE_QUADSPI      = 4,
} stm32_syscfg_mem_mode_e;

typedef union
{
	struct
	{
		uint32_t mem_mode   :  3;
		uint32_t res7_3     :  5;
		uint32_t fb_mode    :  1;
		uint32_t res31_9    : 23;
	};

	uint32_t mask;
} stm32_syscfg_memrmp_t;

typedef union
{
	struct
	{
		uint32_t res7_0      : 8;
		uint32_t boosten     : 1;
		uint32_t anaswvdd    : 1;
		uint32_t res15_10    : 6;
		uint32_t i2c_pb6_fmp : 1;
		uint32_t i2c_pb7_fmp : 1;
		uint32_t i2c_pb8_fmp : 1;
		uint32_t i2c_pb9_fmp : 1;
		uint32_t i2c1_fmp    : 1;
		uint32_t i2c2_fmp    : 1;
		uint32_t i2c3_fmp    : 1;
		uint32_t i2c4_fmp    : 1;
		uint32_t res25_24    : 2;
		uint32_t fpu_ie      : 6;
	};

	uint32_t mask;
} stm32_syscfg_cfg_t;

typedef union
{
	struct
	{
		uint32_t cll     :  1;
		uint32_t spl     :  1;
		uint32_t pvdl    :  1;
		uint32_t eccl    :  1;
		uint32_t res7_4  :  4;
		uint32_t spf     :  1;
		uint32_t res31_9 : 23;
	};

	uint32_t mask;
} stm32_syscfg_cfg2_t;


typedef struct
{
	stm32_syscfg_memrmp_t memrmp;
	stm32_syscfg_cfg_t    cfg;
	uint32_t              extic1;
	uint32_t              extic2;
	uint32_t              extic3;
	uint32_t              extic4;
	stm32_syscfg_cfg2_t   cfg2;
	uint32_t              swp;
	uint32_t              sk;
	uint32_t              res255_9[247];
} stm32_syscfg_t;

static volatile stm32_syscfg_t * const stm32_syscfg = (volatile stm32_syscfg_t *)STM32_MMAP_SYSCFG;


static inline void stm32_syscfg_exti_port_set(uint8_t pin, uint8_t port)
{
	volatile uint32_t * const base = &stm32_syscfg->extic1;
	unsigned r = pin / 4;
	unsigned s = (pin % 4) * 4;

	uint32_t mask = base[r];
	mask &= ~(0xF << s);
	mask |= port << s;
	base[r] = mask;
}

#endif
