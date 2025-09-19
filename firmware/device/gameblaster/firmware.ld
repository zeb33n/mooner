OUTPUT_FORMAT(elf32-littlearm)
OUTPUT_ARCH(arm)

MEMORY
{
	FLASH (rx ) : ORIGIN = 0x08000000, LENGTH = 32K
	SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH =  8K
}

SECTIONS
{
	.arm_vector_table :
	{
		. = ALIGN(128);
		KEEP (*(.arm_vector_table))
		. = ALIGN(8);
	} > FLASH

	.text :
	{
		. = ALIGN(8);
		*(.text.arm_vector_*)
		*(.text*)
		*(.text)
		*(.glue_7)
		*(.glue_7t)
		*(.eh_frame)

		KEEP (*(.init))
		KEEP (*(.fini))
		. = ALIGN(8);
	} > FLASH

	.rodata :
	{
		. = ALIGN(8);
		*(.rodata)
		*(.rodata*)
		. = ALIGN(8);
	} > FLASH

	.data :
	{
		. = ALIGN(8);
		_linker_script__data_begin = .;
		*(.data)
		*(.data*)
		. = ALIGN(8);
		_linker_script__data_end = .;
	} > SRAM AT > FLASH

	.bss (NOLOAD) :
	{
		. = ALIGN(8);
		_linker_script__bss_begin = .;
		*(.bss)
		*(.bss*)
		*(COMMON)
		. = ALIGN(8);
		_linker_script__bss_end = .;
	} > SRAM
}

_linker_script__data_flash = LOADADDR(.data);

_linker_script__stack_end = ORIGIN(SRAM) + LENGTH(SRAM);

_linker_script__reserved_flash_base = ORIGIN(FLASH);
_linker_script__reserved_flash_size = LENGTH(FLASH);

_linker_script__reserved_sram_base = ORIGIN(SRAM);
_linker_script__reserved_sram_size = LENGTH(SRAM);

ENTRY(arm_vector_reset)
