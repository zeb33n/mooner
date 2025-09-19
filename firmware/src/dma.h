#ifndef GBFW_DMA_H
#define GBFW_DMA_H

#include <hardware/stm32/mmap.h>
#include <hardware/stm32/syscfg.h>
#include <hardware/stm32/dbg.h>

static inline uintptr_t gbfw_dma_pointer(const void *ptr)
{
	uintptr_t base = (uintptr_t)ptr;

	uint8_t region = base >> 24;
	if (region == 0x00)
	{
		switch (stm32_syscfg->memrmp.mem_mode)
		{
			case STM32_SYSCFG_MEM_MODE_MAIN_FLASH:
				base += STM32_MMAP_MAIN_FLASH;
				break;

			case STM32_SYSCFG_MEM_MODE_SYSTEM_FLASH:
				base += STM32_MMAP_SYSTEM_MEMORY;
				break;

			case STM32_SYSCFG_MEM_MODE_FSMC:
				base += STM32_MMAP_FSMC;
				break;

			case STM32_SYSCFG_MEM_MODE_SRAM1:
				base += STM32_MMAP_SRAM;
				break;

			case STM32_SYSCFG_MEM_MODE_QUADSPI:
				base += STM32_MMAP_QUADSPI;
				break;

			default:
				break;
		}
	}
	else if (region == (STM32_MMAP_CCM_SRAM_CODE >> 24))
	{
		base -= STM32_MMAP_CCM_SRAM_CODE;
		base += (uintptr_t)stm32_ccm_sram_base(false);
	}

	return base;
}

#endif
