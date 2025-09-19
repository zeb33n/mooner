#ifndef STM32_FMAC_H
#define STM32_FMAC_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef union
{
	struct
	{
		uint32_t x1_base     : 8;
		uint32_t x1_buf_size : 8;
		uint32_t res23_16    : 8;
		uint32_t full_wm     : 2;
		uint32_t res31_26    : 6;
	};

	uint32_t mask;
} stm32_fmac_x1bufcfg_t;

typedef union
{
	struct
	{
		uint32_t x2_base     :  8;
		uint32_t x2_buf_size :  8;
		uint32_t res31_16    : 16;
	};

	uint32_t mask;
} stm32_fmac_x2bufcfg_t;

typedef union
{
	struct
	{
		uint32_t y_base     : 8;
		uint32_t y_buf_size : 8;
		uint32_t res23_16   : 8;
		uint32_t empty_wm   : 2;
		uint32_t res31_26   : 6;
	};

	uint32_t mask;
} stm32_fmac_ybufcfg_t;

typedef enum
{
	STM32_FMAC_FUNC_LOAD_X1_BUFFER = 1,
	STM32_FMAC_FUNC_LOAD_X2_BUFFER = 2,
	STM32_FMAC_FUNC_LOAD_Y_BUFFER  = 3,
	STM32_FMAC_FUNC_CONVOLUTION    = 8,
	STM32_FMAC_FUNC_IIR_FILTER     = 9,
} stm32_fmac_func_e;

typedef union
{
	struct
	{
		uint32_t p     : 8;
		uint32_t q     : 8;
		uint32_t r     : 8;
		uint32_t func  : 7;
		uint32_t start : 1;
	};

	uint32_t mask;
} stm32_fmac_param_t;

typedef union
{
	struct
	{
		uint32_t rien     :  1;
		uint32_t wien     :  1;
		uint32_t ovflien  :  1;
		uint32_t unflien  :  1;
		uint32_t satien   :  1;
		uint32_t res7_5   :  3;
		uint32_t dmaren   :  1;
		uint32_t dmawen   :  1;
		uint32_t res14_10 :  5;
		uint32_t clipen   :  1;
		uint32_t reset    :  1;
		uint32_t res31_17 : 15;
	};

	uint32_t mask;
} stm32_fmac_c_t;

typedef union
{
	struct
	{
		uint32_t y_empty  :  1;
		uint32_t x1_full  :  1;
		uint32_t res7_2   :  6;
		uint32_t ovfl     :  1;
		uint32_t unfl     :  1;
		uint32_t sat      :  1;
		uint32_t res31_11 : 21;
	};

	uint32_t mask;
} stm32_fmac_s_t;


typedef struct
{
	stm32_fmac_x1bufcfg_t x1bufcfg;
	stm32_fmac_x2bufcfg_t x2bufcfg;
	stm32_fmac_ybufcfg_t  ybufcfg;
	stm32_fmac_param_t    param;
	stm32_fmac_c_t        c;
	stm32_fmac_s_t        s;
	uint32_t              wdata;
	uint32_t              rdata;
	uint32_t      res255_8[248];
} stm32_fmac_t;

static volatile stm32_fmac_t * const stm32_fmac = (volatile stm32_fmac_t *)STM32_MMAP_FMAC;

#endif
