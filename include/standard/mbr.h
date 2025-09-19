#ifndef MBR_H
#define MBR_H

#include <stdint.h>
#include <stdbool.h>

#define MBR_BOOT_SIGNATURE 0xAA55

typedef enum
{
	MBR_PART_TYPE_EMPTY      = 0x00,
	MBR_PART_TYPE_FAT12      = 0x01,
	MBR_PART_TYPE_FAT16      = 0x04,
	MBR_PART_TYPE_FAT16B_CHS = 0x06,
	MBR_PART_TYPE_EXFAT      = 0x07,
	MBR_PART_TYPE_FAT32_CHS  = 0x0B,
	MBR_PART_TYPE_FAT32_LBA  = 0x0C,
	MBR_PART_TYPE_FAT16B_LBA = 0x0E,
} mbr_part_type_e;

typedef struct
__attribute__((__packed__))
{
	uint8_t  status;
	uint8_t  chs_first[3];
	uint8_t  type;
	uint8_t  chs_last[3];
	uint32_t lba_first;
	uint32_t sectors;
} mbr_part_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t    bootstrap[446];
	mbr_part_t part[4];
	uint16_t   boot_signature;
} mbr_t;


static inline bool mbr_part_is_basic_data(mbr_part_type_e type)
{
	switch (type)
	{
		case MBR_PART_TYPE_FAT12:
		case MBR_PART_TYPE_FAT16:
		case MBR_PART_TYPE_FAT16B_CHS:
		case MBR_PART_TYPE_EXFAT:
		case MBR_PART_TYPE_FAT32_CHS:
		case MBR_PART_TYPE_FAT32_LBA:
		case MBR_PART_TYPE_FAT16B_LBA:
			return true;

		default:
			break;
	}

	return false;
}

#endif
