#ifndef ELF_H
#define ELF_H

#include <stdint.h>

#define ELF_MAGIC "\x7f""ELF"
#define ELF_VERSION 1

typedef enum
{
	ELF_CLASS_32BIT = 1,
	ELF_CLASS_64BIT = 2,
} elf_class_e;

typedef enum
{
	ELF_DATA_LSB = 1,
	ELF_DATA_MSB = 2,
} elf_data_e;

typedef enum
{
	ELF_TYPE_REL  = 1,
	ELF_TYPE_EXEC = 2,
	ELF_TYPE_DYN  = 3,
	ELF_TYPE_CORE = 4,
} elf_type_e;

typedef enum
{
	ELF_MACHINE_M32     = 0x0001,
	ELF_MACHINE_SPARC   = 0x0002,
	ELF_MACHINE_X86     = 0x0003,
	ELF_MACHINE_68K     = 0x0004,
	ELF_MACHINE_88K     = 0x0005,
	ELF_MACHINE_860     = 0x0007,
	ELF_MACHINE_MIPS    = 0x0008,
	ELF_MACHINE_ARM     = 0x0028,
	ELF_MACHINE_X86_64  = 0x003E,
	ELF_MACHINE_AARCH64 = 0x00B7,
	ELF_MACHINE_Z80     = 0x00DC,
	ELF_MACHINE_RISCV   = 0x00F3,
} elf_machine_e;

typedef struct
{
	struct
	{
		uint8_t  magic[4];
		uint8_t  class;
		uint8_t  data;
		uint8_t  version;
		uint8_t  osabi;
		uint8_t  pad[8];
	} ident;

	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint32_t entry;
	uint32_t phoff;
	uint32_t shoff;
	uint32_t flags;
	uint16_t ehsize;
	uint16_t phentsize;
	uint16_t phnum;
	uint16_t shentsize;
	uint16_t shnum;
	uint16_t shstrndx;
} elf_header_t;


typedef enum
{
	ELF_SECTION_UNDEF  = 0x0000,
	ELF_SECTION_ABS    = 0xFFF1,
	ELF_SECTION_COMMON = 0xFFF2,
} elf_section_e;

typedef enum
{
	ELF_SECTION_TYPE_PROGBITS =  1,
	ELF_SECTION_TYPE_SYMTAB   =  2,
	ELF_SECTION_TYPE_STRTAB   =  3,
	ELF_SECTION_TYPE_RELA     =  4,
	ELF_SECTION_TYPE_HASH     =  5,
	ELF_SECTION_TYPE_DYNAMIC  =  6,
	ELF_SECTION_TYPE_NOTE     =  7,
	ELF_SECTION_TYPE_NOBITS   =  8,
	ELF_SECTION_TYPE_REL      =  9,
	ELF_SECTION_TYPE_SHLIB    = 10,
	ELF_SECTION_TYPE_DYNSYM   = 11,
} elf_section_type_e;

typedef struct
{
	uint32_t name;
	uint32_t type;

	union
	{
		struct
		{
			uint32_t write     :  1;
			uint32_t alloc     :  1;
			uint32_t execinstr :  1;
			uint32_t res31_3   : 29;
		};

		uint32_t mask;
	} flags;

	uint32_t addr;
	uint32_t offset;
	uint32_t size;
	uint32_t link;
	uint32_t info;
	uint32_t addralign;
	uint32_t entsize;
} elf_section_header_t;


typedef enum
{
	ELF_SYMBOL_BIND_LOCAL  = 0,
	ELF_SYMBOL_BIND_GLOBAL = 1,
	ELF_SYMBOL_BIND_WEAK   = 2,
} elf_symbol_bind_e;

typedef enum
{
	ELF_SYMBOL_TYPE_NOTYPE  = 0,
	ELF_SYMBOL_TYPE_OBJECT  = 1,
	ELF_SYMBOL_TYPE_FUNC    = 2,
	ELF_SYMBOL_TYPE_SECTION = 3,
	ELF_SYMBOL_TYPE_FILE    = 4,
} elf_symbol_type_e;

typedef struct
{
	uint32_t name;
	uint32_t value;
	uint32_t size;

	union
	{
		struct
		{
			uint8_t type : 4;
			uint8_t bind : 4;
		};

		uint8_t mask;
	} info;

	uint8_t  other;
	uint16_t shndx;
} elf_symbol_t;


typedef enum
{
	ELF_REL_TYPE_ARM_NONE       =  0,
	ELF_REL_TYPE_ARM_PC24       =  1,
	ELF_REL_TYPE_ARM_ABS32      =  2,
	ELF_REL_TYPE_ARM_REL32      =  3,
	ELF_REL_TYPE_ARM_PC13       =  4,
	ELF_REL_TYPE_ARM_ABS16      =  5,
	ELF_REL_TYPE_ARM_ABS12      =  6,
	ELF_REL_TYPE_ARM_THM_ABS5   =  7,
	ELF_REL_TYPE_ARM_ABS8       =  8,
	ELF_REL_TYPE_ARM_SBREL32    =  9,
	ELF_REL_TYPE_ARM_THM_PC22   = 10,
	ELF_REL_TYPE_ARM_THM_PC8    = 11,
	ELF_REL_TYPE_ARM_AMP_VCALL9 = 12,
	ELF_REL_TYPE_ARM_SWI24      = 13,
	ELF_REL_TYPE_ARM_THM_SWI8   = 14,
	ELF_REL_TYPE_ARM_XPC25      = 15,
	ELF_REL_TYPE_ARM_THM_XPC22  = 16,
} elf_rel_type_e;

typedef struct
{
	uint32_t offset;

	union
	{
		struct
		{
			uint32_t type :  8;
			uint32_t sym  : 24;
		};

		uint32_t mask;
	} info;
} elf_rel_t;

typedef struct
{
	uint32_t offset;

	union
	{
		struct
		{
			uint32_t type :  8;
			uint32_t sym  : 24;
		};

		uint32_t mask;
	} info;

	int32_t addend;
} elf_rela_t;


typedef enum
{
	ELF_PROGRAM_TYPE_NULL    = 0,
	ELF_PROGRAM_TYPE_LOAD    = 1,
	ELF_PROGRAM_TYPE_DYNAMIC = 2,
	ELF_PROGRAM_TYPE_INTERP  = 3,
	ELF_PROGRAM_TYPE_NOTE    = 4,
	ELF_PROGRAM_TYPE_SHLIB   = 5,
	ELF_PROGRAM_TYPE_PHDR    = 6,
} elf_program_type_e;

typedef struct
{
	uint32_t type;
	uint32_t offset;
	uint32_t vaddr;
	uint32_t paddr;
	uint32_t filesz;
	uint32_t memsz;

	union
	{
		struct
		{
			uint32_t x       :  1;
			uint32_t w       :  1;
			uint32_t r       :  1;
			uint32_t res27_3 : 25;
			uint32_t proc    :  4;
		};

		struct
		{
			uint32_t res27_0 : 28;
			uint32_t sb      :  1;
			uint32_t pi      :  1;
			uint32_t abs     :  1;
			uint32_t res31   :  1;
		} arm;

		uint32_t mask;
	} flags;

	uint32_t align;
} elf_program_header_t;


typedef enum
{
	ELF_DYN_TAG_NULL     =  0,
	ELF_DYN_TAG_NEEDED   =  1,
	ELF_DYN_TAG_PLTRELSZ =  2,
	ELF_DYN_TAG_PLTGOT   =  3,
	ELF_DYN_TAG_HASH     =  4,
	ELF_DYN_TAG_STRTAB   =  5,
	ELF_DYN_TAG_SYMTAB   =  6,
	ELF_DYN_TAG_RELA     =  7,
	ELF_DYN_TAG_RELASZ   =  8,
	ELF_DYN_TAG_RELAENT  =  9,
	ELF_DYN_TAG_STRSZ    = 10,
	ELF_DYN_TAG_SYMENT   = 11,
	ELF_DYN_TAG_INIT     = 12,
	ELF_DYN_TAG_FINI     = 13,
	ELF_DYN_TAG_SONAME   = 14,
	ELF_DYN_TAG_RPATH    = 15,
	ELF_DYN_TAG_SYMBOLIC = 16,
	ELF_DYN_TAG_REL      = 17,
	ELF_DYN_TAG_RELSZ    = 18,
	ELF_DYN_TAG_RELENT   = 19,
	ELF_DYN_TAG_PLTREL   = 20,
	ELF_DYN_TAG_DEBUG    = 21,
	ELF_DYN_TAG_TEXTREL  = 22,
	ELF_DYN_TAG_JMPREL   = 23,
} elf_dyn_tag_e;

typedef struct
{
	int32_t tag;

	union
	{
		uint32_t val;
		uint32_t ptr;
	};
} elf_dyn_t;

#endif
