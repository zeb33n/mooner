#include "load.h"
#include "flash.h"
#include "memcpy.h"
#include "fs.h"
#include "power.h"
#include "settings.h"
#include <gbfw/api.h>
#include <hardware/stm32/mmap.h>
#include <hardware/stm32/flash.h>
#include <hardware/stm32/dbg.h>
#include <standard/file/elf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


extern uint32_t _linker_script__reserved_sram_base;
extern uint32_t _linker_script__reserved_sram_size;

extern uint32_t _linker_script__reserved_flash_base;
extern uint32_t _linker_script__reserved_flash_size;


static inline bool gbfw_load__is_loadable_sram(
	uintptr_t start, uintptr_t end, bool executable)
{
	const uintptr_t reserved_start = (uintptr_t)&_linker_script__reserved_sram_base;
	const uintptr_t reserved_size  = (uintptr_t)&_linker_script__reserved_sram_size;
	const uintptr_t reserved_end   = reserved_start + reserved_size;

	if ((start < reserved_end) && (end > reserved_start))
		return false;

	uintptr_t region[][2] =
	{
		{ (uintptr_t)stm32_sram_base()        , stm32_sram_size()     },
		{ (uintptr_t)stm32_sram1_base(true)   , stm32_sram1_size()    },
		{ (uintptr_t)stm32_ccm_sram_base(true), stm32_ccm_sram_size() },
	};

	for (uint8_t i = (executable ? 1 : 0); i < (sizeof(region) / sizeof(region[0])); i++)
	{
		uintptr_t region_start = region[i][0];
		uintptr_t region_end   = region_start + region[i][1];

		if ((start >= region_start) && (end <= region_end))
			return true;
	}

	return false;
}

static inline bool gbfw_load__is_loadable_flash(uintptr_t start, uintptr_t end)
{
	// Unaligned flash sections are not supported.
	if ((start % STM32_FLASH_LINE_SIZE_BYTES)
		|| (end % STM32_FLASH_LINE_SIZE_BYTES))
		return false;

	const uintptr_t reserved_start = (uintptr_t)&_linker_script__reserved_flash_base;
	const uintptr_t reserved_size  = (uintptr_t)&_linker_script__reserved_flash_size;
	const uintptr_t reserved_end   = reserved_start + reserved_size;

	if ((start < reserved_end) && (end > reserved_start))
		return false;

	const uintptr_t flash_start = STM32_MMAP_MAIN_FLASH;
	const uintptr_t flash_size  = stm32_flash_size_bytes();
	const uintptr_t flash_end   = flash_start + flash_size;

	return ((start >= flash_start) && (end <= flash_end));
}


void *gbfw_load(const gbfw_fs_file_t *entry)
{
	gbfw_fs_file_t file_entry = *entry;

	gbfw_fs_file_t fp;
	if (!gbfw_fs_dir_entry_file(&file_entry, &fp))
		return NULL;
	const gbfw_fs_file_t fp_origin = fp;

	elf_header_t header;
	if (!gbfw_fs_file_read(&fp, sizeof(header), &header))
		return NULL;

	for (uint8_t i = 0; i < sizeof(header.ident.magic); i++)
	{
		if (header.ident.magic[i] != ELF_MAGIC[i])
			return NULL;
	}

	if ((header.ident.class != ELF_CLASS_32BIT)
		|| (header.ident.data != ELF_DATA_LSB)
		|| (header.ident.version != ELF_VERSION)
		|| (header.type != ELF_TYPE_EXEC)
		|| (header.machine != ELF_MACHINE_ARM)
		|| (header.version != ELF_VERSION)
		|| (header.phoff == 0)
		|| (header.phentsize < sizeof(elf_program_header_t)))
		return NULL;

	fp = fp_origin;
	if (!gbfw_fs_file_skip(&fp, header.phoff))
		return NULL;
	gbfw_fs_file_t fp_phoff = fp;

	bool flash_used = false;
	bool success = true;

	// Iterate and erase pages first as pages may overlap.
	const uint32_t phskip = header.phentsize - sizeof(elf_program_header_t);
	for (uint32_t i = 0; success && (i < header.phnum); i++)
	{
		if ((i > 0) && !gbfw_fs_file_skip(&fp, phskip))
		{
			success = false;
			break;
		}

		elf_program_header_t program_header;
		if (!gbfw_fs_file_read(&fp, sizeof(program_header), &program_header))
		{
			success = false;
			break;
		}

		if ((program_header.type != ELF_PROGRAM_TYPE_LOAD)
			|| (program_header.memsz == 0))
			continue;

		uintptr_t start = program_header.paddr;
		uintptr_t end   = start + program_header.memsz;

		if (gbfw_load__is_loadable_flash(start, end))
		{
			if (!flash_used)
			{
				success = gbfw_flash_unlock();
				if (!success) break;
				flash_used = true;
			}

			gbfw_flash_erase(
				(void *)program_header.paddr,
				program_header.memsz);
		}
	}

	fp = fp_phoff;
	for (uint32_t i = 0; success && (i < header.phnum); i++)
	{
		if ((i > 0) && !gbfw_fs_file_skip(&fp, phskip))
		{
			success = false;
			break;
		}

		elf_program_header_t program_header;
		if (!gbfw_fs_file_read(&fp, sizeof(program_header), &program_header))
		{
			success = false;
			break;
		}

		if ((program_header.type != ELF_PROGRAM_TYPE_LOAD)
			|| (program_header.memsz == 0))
			continue;

		gbfw_fs_file_t pfp = fp_origin;
		if (!gbfw_fs_file_skip(&pfp, program_header.offset))
		{
			success = false;
			break;
		}

		uintptr_t start = program_header.paddr;
		uintptr_t end   = start + program_header.memsz;

		if (gbfw_load__is_loadable_flash(start, end))
		{
			success = gbfw_flash_program_begin();

			while (success && (start < end))
			{
				uint32_t line[STM32_FLASH_LINE_SIZE_WORDS];
				success = gbfw_fs_file_read(&pfp, sizeof(line), line);
				if (!success) break;

				success = gbfw_flash_program_line((void *)start, &line);
				start += STM32_FLASH_LINE_SIZE_BYTES;
			}

			gbfw_flash_program_end();
		}
		else if (gbfw_load__is_loadable_sram(start, end, program_header.flags.x))
		{
			uint8_t *pptr = (uint8_t *)program_header.paddr;

			success = gbfw_fs_file_read(&pfp, program_header.filesz, pptr);

			if (program_header.filesz < program_header.memsz)
			{
				memset(&pptr[program_header.filesz], 0x00,
					(program_header.memsz - program_header.filesz));
			}
		}
		else
		{
			success = false;
		}
	}

	gbfw_flash_lock(false);

	if (!success) return NULL;

	return (void *)header.entry;
}


static bool (*gbfw_entry_point)(uint32_t, const gbfw_api_t *) = NULL;

bool gbfw_signal(uint32_t signal)
{
	if (!gbfw_entry_point) return false;
	return gbfw_entry_point(signal, gbfw_api);
}

bool gbfw_exec(void *entry)
{
	gbfw_entry_point = entry;
	return gbfw_signal(GBFW_SIGNAL_BEGIN);
}

_Noreturn void gbfw_exec_file(const gbfw_fs_file_t *entry)
{
	uint32_t sector;
	uint8_t offset;
	gbfw_fs_dir_entry_to_sector_offset(entry, &sector, &offset);

	gbfw_settings->exec_sector         = sector;
	gbfw_settings->primary.exec_entry  = offset;
	gbfw_settings->primary.exec_valid  = true;
	gbfw_settings->primary.exec_fail   = false;
	gbfw_settings->primary.skip_splash = true;

	gbfw_power_shutdown();
}
