#ifndef FAT_H
#define FAT_H

#include <stdbool.h>
#include <stdint.h>

#define FAT_VBR_BOOT_SIGNATURE      0xAA55
#define FAT_FSINFO_SIGNATURE_START  0x41615252
#define FAT_FSINFO_SIGNATURE_MIDDLE 0x61417272
#define FAT_FSINFO_SIGNATURE_END    0xAA550000
#define EXFAT_SIGNATURE             "EXFAT   "

#define FAT12_MAX_CLUSTERS  4085
#define FAT16_MAX_CLUSTERS 65532

#define FAT12_EMPTY_SECTOR 0x000
#define FAT12_RESERVED     0x001
#define FAT12_BAD_SECTOR   0xFF7
#define FAT12_END_OF_CHAIN 0xFFF

#define FAT16_EMPTY_SECTOR 0x0000
#define FAT16_RESERVED     0x0001
#define FAT16_BAD_SECTOR   0xFFF7
#define FAT16_END_OF_CHAIN 0xFFFF

#define FAT32_EMPTY_SECTOR 0x00000000
#define FAT32_RESERVED     0x00000001
#define FAT32_BAD_SECTOR   0x0FFFFFF7
#define FAT32_END_OF_CHAIN 0x0FFFFFFF

#define EXFAT_BAD_SECTOR   0xFFFFFFF7
#define EXFAT_END_OF_CHAIN 0xFFFFFFFF


static inline bool fat12_is_end_of_chain(uint16_t entry)
{
	return ((entry == FAT12_EMPTY_SECTOR)
		|| (entry == FAT12_RESERVED)
		|| (entry == FAT12_END_OF_CHAIN));
}

static inline bool fat16_is_end_of_chain(uint16_t entry)
{
	return ((entry == FAT16_EMPTY_SECTOR)
		|| (entry == FAT16_RESERVED)
		|| (entry == FAT16_END_OF_CHAIN));
}

static inline bool fat32_is_end_of_chain(uint32_t entry)
{
	return ((entry == FAT32_EMPTY_SECTOR)
		|| (entry == FAT32_RESERVED)
		|| (entry == FAT32_END_OF_CHAIN));
}

static inline bool exfat_is_end_of_chain(uint32_t entry)
{
	return ((entry == FAT32_EMPTY_SECTOR)
		|| (entry == FAT32_RESERVED)
		|| (entry == EXFAT_END_OF_CHAIN));
}


typedef struct
__attribute__((__packed__))
{
	uint16_t bytes_per_logical_sector;
	uint8_t  logical_sectors_per_cluster;
	uint16_t reserved_logical_sectors;
	uint8_t  number_of_fats;
	uint16_t root_directory_entries;
	uint16_t total_logical_sectors;
	uint8_t  media_descriptor;
	uint16_t logical_sectors_per_fat;
	uint16_t physical_sectors_per_track;
	uint16_t number_of_heads;
	uint32_t hidden_sectors;
	uint32_t total_large_logical_sectors;

	uint8_t  physical_drive_number;
	uint8_t  flags;
	uint8_t  extended_boot_signature;
	uint32_t volume_serial_number;
	char     volume_label[11];
	char     filesytem_type[8];
} fat1x_bpb_t;

typedef struct
__attribute__((__packed__))
{
	uint16_t bytes_per_logical_sector;
	uint8_t  logical_sectors_per_cluster;
	uint16_t reserved_logical_sectors;
	uint8_t  number_of_fats;
	uint16_t root_directory_entries;
	uint16_t total_logical_sectors;
	uint8_t  media_descriptor;
	uint16_t logical_sectors_per_fat;
	uint16_t physical_sectors_per_track;
	uint16_t number_of_heads;
	uint32_t hidden_sectors;
	uint32_t total_large_logical_sectors;

	uint32_t large_logical_sectors_per_fat;

	union
	{
		struct
		{
			uint16_t active_fat : 4;
			uint16_t res6_4     : 3;
			uint16_t fat_mirror : 1;
			uint16_t res15_8    : 8;
		};

		uint16_t mask;
	} mirroring_flags;


	uint16_t version;
	uint32_t root_directory_cluster;
	uint16_t location_of_fs_information_sector;
	uint16_t location_of_backup_sectors;
	char     boot_file_name[12];

	uint8_t  physical_drive_number;
	uint8_t  flags;
	uint8_t  extended_boot_signature;
	uint32_t volume_serial_number;
	char     volume_label[11];
	char     filesytem_type[8];
} fat32_bpb_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t  zero[53];
	uint64_t partition_offset;
	uint64_t volume_length;
	uint32_t fat_offset;
	uint32_t fat_length;
	uint32_t cluster_heap_offset;
	uint32_t cluster_count;
	uint32_t first_cluster_of_root_directory;
	uint32_t volume_serial_number;
	uint16_t file_system_revision;

	union
	{
		struct
		{
			uint16_t active_fat    :  1;
			uint16_t volume_dirty  :  1;
			uint16_t media_failure :  1;
			uint16_t res15_3       : 13;
		};

		uint16_t mask;
	} volume_flags;

	uint8_t  bytes_per_sector_shift;
	uint8_t  sectors_per_cluster_shift;
	uint8_t  number_of_fats;
	uint8_t  drive_select;
	uint8_t  percent_in_use;
} exfat_bpb_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t jump[3];
	char    label[8];

	union
	__attribute__((__packed__))
	{
		fat1x_bpb_t fat1x;
		fat32_bpb_t fat32;
		exfat_bpb_t exfat;

		uint8_t data[499];
	} bpb;

	uint16_t signature;
} fat_vbr_t;

typedef struct
{
	uint32_t signature_start;
	uint8_t  reserved_start[480];
	uint32_t signature_middle;
	uint32_t free_clusters;
	uint32_t last_allocated;
	uint8_t  reserved_end[12];
	uint32_t signature_end;
} fat_fsinfo_t;


typedef union
{
	struct
	{
		uint16_t seconds : 5;
		uint16_t minutes : 6;
		uint16_t hours   : 5;
	};

	uint16_t mask;
} fat_time_t;

typedef union
{
	struct
	{
		uint16_t day   : 5;
		uint16_t month : 4;
		uint16_t year  : 7;
	};

	uint16_t mask;
} fat_date_t;

typedef union
{
	struct
	{
		uint8_t read_only    : 1;
		uint8_t hidden       : 1;
		uint8_t system       : 1;
		uint8_t volume_label : 1;
		uint8_t subdirectory : 1;
		uint8_t archive      : 1;
		uint8_t device       : 1;
		uint8_t res7         : 1;
	};

	uint8_t mask;
} fat_attributes_t;

typedef struct
{
	char             name[8];
	char             extension[3];
	fat_attributes_t attributes;
	uint8_t          type;
	uint8_t          create_time_ms;
	fat_time_t       create_time;
	fat_date_t       create_date;
	fat_date_t       last_access_date;
	uint16_t         first_cluster_high;
	fat_time_t       last_modified_time;
	fat_date_t       last_modified_date;
	uint16_t         first_cluster;
	uint32_t         file_size;
} fat_dir_entry_t;

#define FAT_ENTRY_FINAL     0x00
#define FAT_ENTRY_AVAILABLE 0xE5
#define FAT_ENTRY_IS_E5     0x05

#define FAT_ENTRY_LFN_SEQ_MAX        0x14
#define FAT_ENTRY_LFN_SEQ_FLAG_FINAL 0x40

typedef struct
__attribute__((__packed__))
{
	uint8_t          sequence;
	uint16_t         name4_0[5];
	fat_attributes_t attributes;
	uint8_t          type;
	uint8_t          checksum;
	uint16_t         name10_5[6];
	uint16_t         first_cluster;
	uint16_t         name12_11[2];
} fat_lfn_entry_t;

#define FAT_LFN_CHARACTERS_PER_ENTRY 13

static inline uint16_t fat_lfn_name(const fat_lfn_entry_t *entry, uint8_t index)
{
	if (index <= 4)
		return entry->name4_0[index];
	else if (index <= 10)
		return entry->name10_5[index - 5];
	else
		return entry->name12_11[index - 11];
}

typedef union
{
	fat_dir_entry_t dir;
	fat_lfn_entry_t lfn;
} fat_entry_t;

static inline bool fat_attributes_is_lfn(fat_attributes_t attributes)
{
	return (attributes.read_only && attributes.hidden
		&& attributes.system && attributes.volume_label);
}


typedef enum
{
	EXFAT_ENTRY_FINAL                = 0x00,
	EXFAT_ENTRY_ALLOCATION_BITMAP    = 0x81,
	EXFAT_ENTRY_UP_CASE_TABLE        = 0x82,
	EXFAT_ENTRY_VOLUME_LABEL         = 0x83,
	EXFAT_ENTRY_FILE_DIRECTORY       = 0x85,
	EXFAT_ENTRY_VOLUME_GUID          = 0xA0,
	EXFAT_ENTRY_TEXFAT_PADDING       = 0xA1,
	EXFAT_ENTRY_ACCESS_CONTROL_TABLE = 0xA2,
	EXFAT_ENTRY_STREAM_EXTENSION     = 0xC0,
	EXFAT_ENTRY_FILE_NAME            = 0xC1,
	EXFAT_ENTRY_ACCESS_CONTROL_LIST  = 0xC2,
	EXFAT_ENTRY_VENDOR_EXTENSION     = 0xE0,
	EXFAT_ENTRY_VENDOR_ALLOCATION    = 0xE1,
} exfat_entry_e;

static inline bool exfat_entry_type_is_active(uint8_t entry_type)
{
	return (entry_type >> 7);
}

static inline bool exfat_entry_type_is_primary(uint8_t entry_type)
{
	return ((entry_type & 0x40) == 0);
}

typedef union
{
	struct
	{
		fat_time_t time;
		fat_date_t date;
	};

	uint32_t mask;
} exfat_timestamp_t;

typedef union
{
	struct
	{
		uint8_t offset : 7;
		uint8_t valid  : 1;
	};

	uint8_t mask;
} exfat_utc_offset_t;

typedef union
{
	struct
	{
		uint16_t read_only    :  1;
		uint16_t hidden       :  1;
		uint16_t system       :  1;
		uint16_t res3         :  1;
		uint16_t subdirectory :  1;
		uint16_t archive      :  1;
		uint16_t res15_6      : 10;
	};

	uint16_t mask;
} exfat_attributes_t;

typedef union
{
	struct
	{
		uint16_t allocation_possible :  1;
		uint16_t no_fat_chain        :  1;
		uint16_t custom_defined      : 14;
	};

	uint16_t mask;
} exfat_primary_flags_t;

typedef union
{
	struct
	{
		uint8_t allocation_possible : 1;
		uint8_t no_fat_chain        : 1;
		uint8_t custom_defined      : 6;
	};

	uint8_t mask;
} exfat_secondary_flags_t;

typedef struct
{
	uint8_t entry_type;
	uint8_t secondary_count;
	uint8_t res31_2[30];
} exfat_entry_primary_t;

typedef struct
{
	uint8_t                 entry_type;
	exfat_secondary_flags_t flags;
	uint8_t                 res31_2[30];
} exfat_entry_secondary_t;

typedef struct
{
	uint8_t  entry_type;
	uint8_t  identifier;
	uint8_t  res19_2[18];
	uint32_t first_cluster;
	uint64_t data_length;
} exfat_entry_allocation_bitmap_t;

typedef struct
{
	uint8_t  entry_type;
	uint8_t  res3_1[3];
	uint32_t table_checksum;
	uint8_t  reserved19_8[12];
	uint32_t first_cluster;
	uint64_t data_length;
} exfat_entry_up_case_table_t;

typedef struct
{
	uint8_t  entry_type;
	uint8_t  character_count;
	uint16_t volume_label[11];
	uint8_t  res31_24[8];
} exfat_entry_volume_label_t;

typedef struct
{
	uint8_t            entry_type;
	uint8_t            secondary_count;
	uint16_t           set_checksum;
	exfat_attributes_t attributes;
	uint8_t            res7_6[2];
	exfat_timestamp_t  create_timestamp;
	exfat_timestamp_t  last_modified_timestamp;
	exfat_timestamp_t  last_accessed_timestamp;
	uint8_t            create_10ms_increment;
	uint8_t            last_modified_10ms_increment;
	exfat_utc_offset_t create_utc_offset;
	exfat_utc_offset_t last_modified_utc_offset;
	exfat_utc_offset_t last_accessed_utc_offset;
	uint8_t            res31_25[7];
} exfat_entry_file_directory_t;

typedef struct
{
	uint8_t               entry_type;
	uint8_t               secondary_count;
	uint16_t              set_checksum;
	exfat_primary_flags_t flags;
	uint8_t               volume_guid[16];
	uint8_t               res31_22[10];
} exfat_entry_volume_guid_t;

typedef struct
{
	uint8_t                 entry_type;
	exfat_secondary_flags_t flags;
	uint8_t                 res2;
	uint8_t                 name_length;
	uint16_t                name_hash;
	uint8_t                 res7_6[2];
	uint64_t                valid_data_length;
	uint8_t                 res19_16[4];
	uint32_t                first_cluster;
	uint64_t                data_length;
} exfat_entry_stream_extension_t;

typedef struct
{
	uint8_t                 entry_type;
	exfat_secondary_flags_t flags;
	uint16_t                file_name[15];
} exfat_entry_file_name_t;

typedef struct
{
	uint8_t                 entry_type;
	exfat_secondary_flags_t flags;
	uint8_t                 vendor_guid[16];
	uint8_t                 vendor_defined[14];
} exfat_entry_vendor_extension_t;

typedef struct
{
	uint8_t                 entry_type;
	exfat_secondary_flags_t flags;
	uint8_t                 vendor_guid[16];
	uint8_t                 vendor_defined[2];
	uint32_t                first_cluster;
	uint64_t                data_length;
} exfat_entry_vendor_allocation_t;

typedef union
{
	uint8_t entry_type;

	exfat_entry_primary_t   primary;
	exfat_entry_secondary_t secondary;

	exfat_entry_allocation_bitmap_t allocation_bitmap;
	exfat_entry_up_case_table_t     up_case_table;
	exfat_entry_volume_label_t      volume_label;
	exfat_entry_file_directory_t    file_directory;
	exfat_entry_volume_guid_t       volume_guid;
	exfat_entry_stream_extension_t  stream_extension;
	exfat_entry_file_name_t         file_name;
	exfat_entry_vendor_extension_t  vendor_extension;
	exfat_entry_vendor_allocation_t vendor_allocation;
} exfat_entry_t;

#endif
