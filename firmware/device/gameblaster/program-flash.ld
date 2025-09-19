OUTPUT_FORMAT(elf32-littlearm)
OUTPUT_ARCH(arm)

MEMORY
{
	FLASH    (rx ) : ORIGIN = 0x08008000, LENGTH = 96K
	SRAM1    (rwx) : ORIGIN = 0x00002000, LENGTH = 72K
	SRAM2    (rw ) : ORIGIN = 0x20014000, LENGTH = 16K
	CCM_SRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 32K
}

SECTIONS
{
	.text :
	{
		. = ALIGN(8);
		*(.text*)
		*(.text)
		*(.glue_7)
		*(.glue_7t)
		*(.eh_frame)

		KEEP (*(.init))
		KEEP (*(.fini))
		. = ALIGN(8);
	}

	.rodata :
	{
		. = ALIGN(8);
		*(.rodata)
		*(.rodata*)
		. = ALIGN(8);
	}

	.data :
	{
		*(.data)
		*(.data*)
	}

	.bss (NOLOAD) :
	{
		*(.bss)
		*(.bss*)
		*(COMMON)
	}
}

_linker_script__stack_end = ORIGIN(CCM_SRAM) + LENGTH(CCM_SRAM);

ENTRY(main)
