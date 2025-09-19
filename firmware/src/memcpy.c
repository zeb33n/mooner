#include "memcpy.h"
#include "dma.h"
#include "irq.h"
#include <hardware/stm32/dma.h>
#include <hardware/stm32/irq.h>
#include <hardware/cm4/scb.h>
#include <hardware/cm4/nvic.h>
#include <hardware/cm4/intrinsics.h>


#define GBFW_MEMCPY_DMA            stm32_dma1
#define GBFW_MEMCPY_DMA_CHANNEL    4
#define GBFW_MEMCPY_DMA_IRQ        STM32_IRQ_DMA1_CH5
#define GBFW_MEMCPY_DMA_IRQ_VECTOR stm32_vector_irq_dma1_ch5
#define GBFW_MEMCPY_DMA_THRESHOLD  32


void __attribute__((interrupt)) GBFW_MEMCPY_DMA_IRQ_VECTOR(void)
{
	stm32_dma_interrupt_clear(GBFW_MEMCPY_DMA, GBFW_MEMCPY_DMA_CHANNEL, STM32_DMA_INTERRUPT_GLOBAL);

	GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.en = false;

	// STM32 Errata 2.1.3
	dsb();
}


void gbfw_memcpy_init(void)
{
	stm32_dma_cc_t cc = {0};
	cc.en      = false;
	cc.tcie    = true;
	cc.htie    = false;
	cc.teie    = true;
	cc.dir     = STM32_DMA_DIR_READ_MEMORY;
	cc.circ    = false;
	cc.pinc    = true;
	cc.minc    = true;
	cc.psize   = STM32_DMA_SIZE_BYTE;
	cc.msize   = STM32_DMA_SIZE_BYTE;
	cc.pl      = 3;
	cc.mem2mem = true;
	GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.mask = cc.mask;

	cm4_nvic_interrupt_priority_set(GBFW_MEMCPY_DMA_IRQ, GBFW_IRQ_PRIORITY_MEMCPY);
	cm4_nvic_interrupt_enable(GBFW_MEMCPY_DMA_IRQ);
}

// Note: This stops gcc from optimizing to call to itself.
__attribute__((optimize("-fno-tree-loop-distribute-patterns")))
void memcpy(void * restrict dst, const void * restrict src, size_t size)
{
	uint8_t       *dst8 = dst;
	const uint8_t *src8 = src;

	if ((size >= GBFW_MEMCPY_DMA_THRESHOLD)
		&& (cm4_scb->ics.vectactive == 0))
	{
		uintptr_t align = (uintptr_t)dst | (uintptr_t)src;

		uint8_t dma_size;
		if (align & 1)
		{
			align = 0;
			dma_size = STM32_DMA_SIZE_BYTE;
		}
		else if (align & 2)
		{
			align = 1;
			dma_size = STM32_DMA_SIZE_HALF;
		}
		else
		{
			align = 2;
			dma_size = STM32_DMA_SIZE_WORD;
		}

		size_t count = size >> align;

		stm32_dma_cc_t cc = { .mask = GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.mask };
		cc.psize = dma_size;
		cc.msize = dma_size;
		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.mask = cc.mask;

		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cpa  = gbfw_dma_pointer(dst);
		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cma  = gbfw_dma_pointer(src);
		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cndt = count;
		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.en = true;

		wfi_while(GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.en);

		size_t bytes = count << align;
		size -= bytes;
		dst8 += bytes;
		src8 += bytes;
	}

	for (; size; size--)
		*dst8++ = *src8++;
}

// Note: This stops gcc from optimizing to call to itself.
__attribute__((optimize("-fno-tree-loop-distribute-patterns")))
void memset(void *dst, int value, size_t size)
{
	uint8_t *dst8 = dst;

	if ((size >= GBFW_MEMCPY_DMA_THRESHOLD)
		&& (cm4_scb->ics.vectactive == 0))
	{
		uintptr_t align = (uintptr_t)dst;

		uint32_t v = (value & 0xFF);

		uint8_t dma_size;
		if (align & 1)
		{
			align = 0;
			dma_size = STM32_DMA_SIZE_BYTE;
		}
		else if (align & 2)
		{
			align = 1;
			dma_size = STM32_DMA_SIZE_HALF;
			v |= (v << 8);
		}
		else
		{
			align = 2;
			dma_size = STM32_DMA_SIZE_WORD;
			v |= (v <<  8);
			v |= (v << 16);
		}

		size_t count = size >> align;

		stm32_dma_cc_t cc = { .mask = GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.mask };
		cc.psize = dma_size;
		cc.msize = dma_size;
		cc.minc  = false;
		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.mask = cc.mask;

		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cpa  = gbfw_dma_pointer(dst);
		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cma  = gbfw_dma_pointer(&v);
		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cndt = count;
		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.en = true;

		wfi_while(GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.en);

		GBFW_MEMCPY_DMA->c[GBFW_MEMCPY_DMA_CHANNEL].cc.minc = true;

		size_t bytes = count << align;
		size -= bytes;
		dst8 += bytes;
	}

	for (; size; size--)
		*dst8++ = (value & 0xFF);
}
