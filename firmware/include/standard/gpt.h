#ifndef GPT_H
#define GPT_H

#include <stdint.h>

#define GPT_SIGNATURE "EFI PART"
#define GPT_PROTECTIVE_MBR_TYPE 0xEE

typedef struct
__attribute__((__packed__))
{
	char     signature[8];
	uint32_t revision;
	uint32_t header_size;
	uint32_t crc;
	uint32_t reserved;
	uint64_t current;
	uint64_t backup;
	uint64_t first_usable;
	uint64_t last_usable;
	uint8_t  disk_guid[16];
	uint64_t partition_addr;
	uint32_t partition_count;
	uint32_t partition_entry_size;
	uint32_t partition_crc;
} gpt_t;

typedef union
{
	struct
	{
		uint64_t required     :  1;
		uint64_t ignore       :  1;
		uint64_t legacy       :  1;
		uint64_t reserved47_3 : 45;
		uint64_t impl_flags   : 16;
	};

	uint64_t mask;
} gpt_part_flags_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t          type_guid[16];
	uint8_t          part_guid[16];
	uint64_t         first;
	uint64_t         last;
	gpt_part_flags_t flags;
	uint16_t         name[36];
} gpt_part_t;

static const uint8_t GPT_PART_TYPE_BASIC_DATA[16] =
{
	0xEB, 0xD0, 0xA0, 0xA2,
	0xB9, 0xE5,
	0x44, 0x33,
	0x87, 0xC0,
	0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7,
};

#endif
