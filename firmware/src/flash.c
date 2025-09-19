#include "flash.h"
#include "power.h"
#include <hardware/stm32/flash.h>
#include <hardware/cm4/scb.h>
#include <hardware/cm4/intrinsics.h>


bool gbfw_flash_unlock(void)
{
	stm32_flash->key = STM32_FLASH_KEY1;
	stm32_flash->key = STM32_FLASH_KEY2;

	return !stm32_flash->c.lock;
}

void gbfw_flash_lock(bool hard)
{
	stm32_flash->c.lock = true;

	if (hard)
	{
		cpsid(if);
		stm32_flash->key = 0;
		dummy_read(&stm32_flash->c);
		cm4_scb->shcs.busfaultpended = false;
		cpsie(if);
	}
}


static void gbfw_flash_erase_page(uint8_t page)
{
	stm32_flash_c_t c = {0};
	c.per = true;
	c.pnb = page;
	stm32_flash->c.mask = c.mask;

	stm32_flash->c.start = true;
	wfi_while(stm32_flash->s.bsy);
}

bool gbfw_flash_erase(void *addr, size_t size)
{
	uint8_t page      = stm32_flash_page_from_addr_single_bank(addr);
	size_t  page_size = STM32_FLASH_PAGE_SIZE_DUAL_BANK;
	uint8_t pages = (size + ((uintptr_t)addr % page_size) + (page_size - 1)) / page_size;

	if (!gbfw_power_hold())
		return false;

	for(uint8_t i = 0; i < pages; i++)
		gbfw_flash_erase_page(page++);

	gbfw_power_release();
	return true;
}

bool gbfw_flash_program_begin(void)
{
	if (!gbfw_power_hold())
		return false;

	stm32_flash->s.mask = 0x0000FFFF;

	stm32_flash_c_t c = {0};
	c.pg = true;
	stm32_flash->c.mask = c.mask;

	return true;
}

bool gbfw_flash_program_line(void *addr, const void *data)
{
	uint32_t *addr_word = (uint32_t *)addr;
	uint32_t *data_word = (uint32_t *)data;

	for (uint8_t i = 0; i < STM32_FLASH_LINE_SIZE_WORDS; i++)
		addr_word[i] = data_word[i];

	wfi_while(stm32_flash->s.bsy);

	return (stm32_flash->s.pgserr != 0);
}

void gbfw_flash_program_end(void)
{
	stm32_flash->c.pg = false;
	gbfw_power_release();
}
