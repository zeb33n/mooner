#ifndef STM32_DMA_H
#define STM32_DMA_H

#include <stdbool.h>
#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef enum
{
	STM32_DMA_INTERRUPT_GLOBAL            = 0,
	STM32_DMA_INTERRUPT_TRANSFER_COMPLETE = 1,
	STM32_DMA_INTERRUPT_HALF_TRANSFER     = 2,
	STM32_DMA_INTERRUPT_TRANSFER_ERROR    = 3,
} stm32_dma_interrupt_e;

typedef union
{
	struct
	{
		uint32_t gif1  : 1;
		uint32_t tcif1 : 1;
		uint32_t htif1 : 1;
		uint32_t teif1 : 1;
		uint32_t gif2  : 1;
		uint32_t tcif2 : 1;
		uint32_t htif2 : 1;
		uint32_t teif2 : 1;
		uint32_t gif3  : 1;
		uint32_t tcif3 : 1;
		uint32_t htif3 : 1;
		uint32_t teif3 : 1;
		uint32_t gif4  : 1;
		uint32_t tcif4 : 1;
		uint32_t htif4 : 1;
		uint32_t teif4 : 1;
		uint32_t gif5  : 1;
		uint32_t tcif5 : 1;
		uint32_t htif5 : 1;
		uint32_t teif5 : 1;
		uint32_t gif6  : 1;
		uint32_t tcif6 : 1;
		uint32_t htif6 : 1;
		uint32_t teif6 : 1;
		uint32_t gif7  : 1;
		uint32_t tcif7 : 1;
		uint32_t htif7 : 1;
		uint32_t teif7 : 1;
		uint32_t gif8  : 1;
		uint32_t tcif8 : 1;
		uint32_t htif8 : 1;
		uint32_t teif8 : 1;
	};

	uint32_t mask;
} stm32_dma_i_t;

typedef enum
{
	STM32_DMA_SIZE_BYTE = 0,
	STM32_DMA_SIZE_HALF = 1,
	STM32_DMA_SIZE_WORD = 2,
} stm32_dma_size_e;

typedef enum
{
	STM32_DMA_DIR_READ_PERIPH = 0,
	STM32_DMA_DIR_READ_MEMORY = 1,
} stm32_dma_dir_e;

typedef union
{
	struct
	{
		uint32_t en       :  1;
		uint32_t tcie     :  1;
		uint32_t htie     :  1;
		uint32_t teie     :  1;
		uint32_t dir      :  1;
		uint32_t circ     :  1;
		uint32_t pinc     :  1;
		uint32_t minc     :  1;
		uint32_t psize    :  2;
		uint32_t msize    :  2;
		uint32_t pl       :  2;
		uint32_t mem2mem  :  1;
		uint32_t res31_15 : 17;
	};

	uint32_t mask;
} stm32_dma_cc_t;

typedef struct
{
	stm32_dma_cc_t cc;
	uint32_t       cndt;
	uint32_t       cpa;
	uint32_t       cma;
	uint32_t       reserved;
} stm32_dma_c_t;


typedef struct
{
	const stm32_dma_i_t is;
	stm32_dma_i_t       ifc;
	stm32_dma_c_t       c[8];
	uint32_t            res255_42[214];
} stm32_dma_t;

static volatile stm32_dma_t * const stm32_dma1 = (volatile stm32_dma_t *)STM32_MMAP_DMA1;
static volatile stm32_dma_t * const stm32_dma2 = (volatile stm32_dma_t *)STM32_MMAP_DMA2;


static inline bool stm32_dma_interrupt_check(
	volatile stm32_dma_t * const dma, uint8_t channel,
	stm32_dma_interrupt_e interrupt)
{
	return ((dma->is.mask >> (channel * 4)) >> interrupt) & 1;
}

static inline void stm32_dma_interrupt_clear(
	volatile stm32_dma_t * const dma, uint8_t channel,
	stm32_dma_interrupt_e interrupt)
{
	dma->ifc.mask = 1U << ((channel * 4) + interrupt);
}

#endif
