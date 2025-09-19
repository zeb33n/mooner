#include "fs.h"
#include "storage.h"
#include <gbfw/memcpy.h>
#include <standard/mbr.h>
#include <standard/gpt.h>
#include <standard/fat.h>
#include <util/log2.h>
#include <util/clamp.h>


typedef struct
{
	uint32_t offset;
	uint32_t sectors;
} gbfw_fs_partition_t;

typedef enum
{
	GBFW_FS_FILESYSTEM_FAT12 = 0,
	GBFW_FS_FILESYSTEM_FAT16 = 1,
	GBFW_FS_FILESYSTEM_FAT32 = 2,
	GBFW_FS_FILESYSTEM_EXFAT = 3,
} gbfw_fs_filesystem_e;

typedef enum
{
	GBFW_FS_FAT_0_ACTIVE = 0,
	GBFW_FS_FAT_1_ACTIVE = 1,
	GBFW_FS_FAT_MIRROR   = 3,
} gbfw_fs_fat_mirror_e;

typedef struct
{
	uint32_t data_offset;
	uint32_t clusters;

	union
	{
		struct
		{
			uint16_t root_directory_sector;
			uint16_t root_directory_entries;
		};

		uint32_t root_directory_cluster;
	};

	uint32_t fat_offset;
	uint32_t fat_size;

	union
	{
		struct
		{
			uint32_t fs_info_offset;
		} fat;

		struct
		{
			uint32_t bitmap_cluster[2];
		} exfat;
	};

	uint8_t cluster_size_log2;

	struct
	{
		uint8_t type       : 2;
		uint8_t fat_mirror : 2;
		uint8_t fat_count  : 1;
		uint8_t res7_5     : 3;
	} flags;
} gbfw_fs_filesystem_t;

typedef union
{
	struct
	{
		uint8_t enable      : 1;
		uint8_t read_only   : 1;
		uint8_t res6_2      : 5;
		uint8_t block_valid : 1;
	};

	uint8_t mask;
} gbfw_fs_flags_t;



static volatile gbfw_fs_flags_t gbfw_fs__flags      = { .mask = 0x00 };
static gbfw_fs_partition_t      gbfw_fs__partition  = {0};
static gbfw_fs_filesystem_t     gbfw_fs__filesystem = {0};



static uint8_t  gbfw_fs__block[GBFW_STORAGE_BLOCK_SIZE];
static uint32_t gbfw_fs__block_sector;

static inline bool gbfw_fs__block_read(uint32_t sector)
{
	if (gbfw_fs__flags.block_valid && (gbfw_fs__block_sector == sector))
		return true;

	gbfw_fs__flags.block_valid = false;
	if (!gbfw_storage_read_block(sector, gbfw_fs__block))
		return false;

	gbfw_fs__block_sector      = sector;
	gbfw_fs__flags.block_valid = true;
	return true;
}

static inline bool gbfw_fs__block_read_multi(uint32_t sector)
{
	gbfw_fs__flags.block_valid = false;
	if (!gbfw_storage_read_multiple_block_begin(gbfw_fs__block)
		|| !gbfw_storage_read_multiple_block_end())
		return false;

	gbfw_fs__block_sector      = sector;
	gbfw_fs__flags.block_valid = true;
	return true;
}



static inline uint32_t gbfw_fs__sector(uint32_t sector)
{
	return (gbfw_fs__partition.offset + sector);
}

static inline uint32_t gbfw_fs__cluster_to_sector(uint32_t cluster)
{
	return gbfw_fs__sector(gbfw_fs__filesystem.data_offset
		+ ((cluster - 2) << gbfw_fs__filesystem.cluster_size_log2));
}

static inline uint32_t gbfw_fs__cluster_from_sector(uint32_t sector)
{
	sector -= gbfw_fs__partition.offset;
	sector -= gbfw_fs__filesystem.data_offset;
	return (sector >> gbfw_fs__filesystem.cluster_size_log2) + 2;
}



static bool gbfw_fs__read_gpt(void)
{
	if (!gbfw_fs__block_read(1))
		return false;

	const gpt_t *gpt = (const gpt_t *)gbfw_fs__block;

	const char signature[] = GPT_SIGNATURE;
	for (uint8_t i = 0; i < sizeof(gpt->signature); i++)
	{
		if (gpt->signature[i] != signature[i])
			return false;
	}

	if (gpt->header_size != sizeof(gpt))
		return false;

	if (gpt->partition_entry_size != sizeof(gpt_part_t))
		return false;

	if ((gpt->partition_addr >> 32) != 0)
		return false;

	uint32_t partition_count  = gpt->partition_count;
	uint32_t partition_offset = gpt->partition_addr & 0xFFFFFFFFUL;

	const gpt_part_t *parts = (const gpt_part_t *)gbfw_fs__block;
	for (uint32_t p = 0; p < partition_count; )
	{
		if (!gbfw_fs__block_read(partition_offset))
			return false;

		for (uint8_t i = 0; i < 4; i++, p++)
		{
			const gpt_part_t *part = &parts[i];

			if (part->flags.ignore) continue;

			// We don't support 64-bit LBA.
			if (((part->first >> 32) != 0)
				|| ((part->last >> 32) != 0))
				continue;

			// We only mount basic data partitions.
			uint8_t j;
			for (j = 0; j < sizeof(part->type_guid); j++)
			{
				if (part->type_guid[j] != GPT_PART_TYPE_BASIC_DATA[j])
					break;
			}
			if (j >= sizeof(part->type_guid)) continue;

			uint8_t basic_data_flags = (part->flags.mask >> 60);
			// Skip shadow copy, hidden and non-automount partitions.
			if ((basic_data_flags >> 1) != 0x00) continue;

			gbfw_fs__flags.read_only   = (basic_data_flags & 1);
			gbfw_fs__partition.offset  = part->first & 0xFFFFFFFFUL;
			gbfw_fs__partition.sectors = 1 + (part->first - part->last);
			return true;
		}

		partition_offset++;
		if (partition_offset == 0)
			return false;
	}

	return false;
}

static bool gbfw_fs__read_mbr(void)
{
	if (!gbfw_fs__block_read(0))
		return false;

	const mbr_t *mbr = (const mbr_t *)gbfw_fs__block;

	// Check valid MBR signature.
	if (mbr->boot_signature != MBR_BOOT_SIGNATURE)
		return false;

	// Return false as a GPT partition table is present.
	if (mbr->part[0].type == GPT_PROTECTIVE_MBR_TYPE)
		return false;

	uint8_t p;
	for (p = 0; p < 4; p++)
	{
		if (mbr_part_is_basic_data(mbr->part[p].type))
			break;
	}

	// No valid partition found.
	if (p >= 4) return false;

	gbfw_fs__flags.read_only   = false;
	gbfw_fs__partition.offset  = mbr->part[p].lba_first;
	gbfw_fs__partition.sectors = mbr->part[p].sectors;
	return true;
}

static void gbfw_fs__read_partition_table(void)
{
	if (gbfw_fs__read_mbr())
		return;

	if (gbfw_fs__read_gpt())
		return;

	// If no partition table is identified, assume raw partition.
	gbfw_fs__partition.offset  = 0;
	gbfw_fs__partition.sectors = gbfw_storage_size();
}

static bool gbfw_fs__read_filesystem(void)
{
	if (!gbfw_fs__block_read(gbfw_fs__partition.offset))
		return false;

	const fat_vbr_t *vbr = (const fat_vbr_t *)gbfw_fs__block;

	if (vbr->signature != FAT_VBR_BOOT_SIGNATURE)
		return false;

	bool is_exfat = true;
	const char *exfat_signature = EXFAT_SIGNATURE;
	for (uint8_t i = 0; is_exfat && (i < sizeof(vbr->label)); i++)
		is_exfat = (vbr->label[i] == exfat_signature[i]);

	if (is_exfat)
	{
		if ((vbr->bpb.exfat.volume_length >> 32) != 0)
			return false;

		if ((vbr->bpb.exfat.number_of_fats == 0)
			|| (vbr->bpb.exfat.number_of_fats > 2))
			return false;

		if (vbr->bpb.exfat.sectors_per_cluster_shift > 31)
			return false;

		gbfw_fs__filesystem.flags.type = GBFW_FS_FILESYSTEM_EXFAT;

		gbfw_fs__filesystem.flags.fat_count = (vbr->bpb.exfat.number_of_fats - 1);

		gbfw_fs__filesystem.cluster_size_log2
			= vbr->bpb.exfat.sectors_per_cluster_shift;

		gbfw_fs__filesystem.clusters = vbr->bpb.exfat.cluster_count;

		gbfw_fs__filesystem.data_offset = vbr->bpb.exfat.cluster_heap_offset;

		gbfw_fs__filesystem.root_directory_cluster
			= vbr->bpb.exfat.first_cluster_of_root_directory;

		gbfw_fs__filesystem.fat_offset = vbr->bpb.exfat.fat_offset;
		gbfw_fs__filesystem.fat_size   = vbr->bpb.exfat.fat_length;

		if (gbfw_fs__filesystem.flags.fat_count > 0)
			gbfw_fs__filesystem.flags.fat_mirror = vbr->bpb.exfat.volume_flags.active_fat;
	}
	else
	{
		if (vbr->bpb.fat1x.bytes_per_logical_sector != GBFW_STORAGE_BLOCK_SIZE)
			return false;

		if ((vbr->bpb.fat1x.logical_sectors_per_cluster == 0)
			|| !is_pow2(vbr->bpb.fat1x.logical_sectors_per_cluster))
			return false;

		if ((vbr->bpb.fat1x.number_of_fats == 0)
			|| (vbr->bpb.fat1x.number_of_fats > 2))
			return false;

		uint32_t sectors = (vbr->bpb.fat1x.total_logical_sectors == 0
			? vbr->bpb.fat1x.total_large_logical_sectors
			: vbr->bpb.fat1x.total_logical_sectors);

		gbfw_fs__filesystem.fat_size = (vbr->bpb.fat1x.logical_sectors_per_fat == 0
			? vbr->bpb.fat32.large_logical_sectors_per_fat
			: vbr->bpb.fat1x.logical_sectors_per_fat);

		gbfw_fs__filesystem.fat_offset = vbr->bpb.fat1x.reserved_logical_sectors;
		uint32_t root_dir_offset = gbfw_fs__filesystem.fat_offset
			+ (gbfw_fs__filesystem.fat_size * vbr->bpb.fat1x.number_of_fats);
		uint32_t root_dir_size_bytes = vbr->bpb.fat1x.root_directory_entries * sizeof(fat_dir_entry_t);
		uint32_t root_dir_size_sectors = (root_dir_size_bytes + (GBFW_STORAGE_BLOCK_SIZE - 1)) / GBFW_STORAGE_BLOCK_SIZE;

		gbfw_fs__filesystem.flags.fat_count = (vbr->bpb.fat1x.number_of_fats - 1);

		gbfw_fs__filesystem.cluster_size_log2
			= log2(vbr->bpb.fat1x.logical_sectors_per_cluster);

		gbfw_fs__filesystem.data_offset = root_dir_offset + root_dir_size_sectors;

		gbfw_fs__filesystem.clusters = (sectors - gbfw_fs__filesystem.data_offset)
			>> gbfw_fs__filesystem.cluster_size_log2;

		if (gbfw_fs__filesystem.clusters <= FAT12_MAX_CLUSTERS)
			gbfw_fs__filesystem.flags.type = GBFW_FS_FILESYSTEM_FAT12;
		else if (gbfw_fs__filesystem.clusters <= FAT16_MAX_CLUSTERS)
			gbfw_fs__filesystem.flags.type = GBFW_FS_FILESYSTEM_FAT16;
		else
			gbfw_fs__filesystem.flags.type = GBFW_FS_FILESYSTEM_FAT32;

		if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_FAT32)
		{
			gbfw_fs__filesystem.root_directory_cluster
				= vbr->bpb.fat32.root_directory_cluster;

			if (gbfw_fs__filesystem.flags.fat_count > 0)
			{
				if (vbr->bpb.fat32.mirroring_flags.fat_mirror)
				{
					if (vbr->bpb.fat32.mirroring_flags.active_fat <= 1)
						gbfw_fs__filesystem.flags.fat_mirror = vbr->bpb.fat32.mirroring_flags.active_fat;
				}
				else
				{
					gbfw_fs__filesystem.flags.fat_mirror = GBFW_FS_FAT_MIRROR;
				}
			}

			// TODO: Validate FSINFO.
		}
		else
		{
			gbfw_fs__filesystem.root_directory_sector
				= root_dir_offset;
			gbfw_fs__filesystem.root_directory_entries
				= vbr->bpb.fat1x.root_directory_entries;
		}
	}

	return true;
}

bool gbfw_fs__fat_read(uint32_t cluster, uint32_t *entry)
{
	uint32_t sector = gbfw_fs__partition.offset
		+ gbfw_fs__filesystem.fat_offset;

	if (gbfw_fs__filesystem.flags.fat_mirror == GBFW_FS_FAT_1_ACTIVE)
		sector += gbfw_fs__filesystem.fat_size;

	uint32_t e;
	if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_FAT12)
	{
		uint32_t byte = (cluster * 3) / 2;
		sector += byte >> GBFW_STORAGE_BLOCK_SIZE_LOG2;
		byte %= GBFW_STORAGE_BLOCK_SIZE_LOG2;

		if (!gbfw_fs__block_read(sector))
			return false;

		e = gbfw_fs__block[byte];
		if (byte >= (GBFW_STORAGE_BLOCK_SIZE - 1))
		{
			if (!gbfw_fs__block_read(sector + 1))
				return false;

			e |= gbfw_fs__block[0] << 8;
		}
		else
		{
			e |= gbfw_fs__block[byte + 1] << 8;
		}

		if (cluster & 1)
			e >>= 4;
		else
			e &= 0xFFF;

		if (e == FAT12_BAD_SECTOR)
			e = EXFAT_BAD_SECTOR;
	}
	else if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_FAT16)
	{
		sector += cluster >> (GBFW_STORAGE_BLOCK_SIZE_LOG2 - 1);
		uint32_t byte = (cluster * 2) % GBFW_STORAGE_BLOCK_SIZE;

		if (!gbfw_fs__block_read(sector))
			return false;

		e = *((uint16_t *)&gbfw_fs__block[byte]);
		if (e == FAT16_BAD_SECTOR)
			e = EXFAT_BAD_SECTOR;
	}
	else
	{
		sector += cluster >> (GBFW_STORAGE_BLOCK_SIZE_LOG2 - 2);
		uint32_t byte = (cluster * 4) % GBFW_STORAGE_BLOCK_SIZE;

		if (!gbfw_fs__block_read(sector))
			return false;

		e = *((uint32_t *)&gbfw_fs__block[byte]);

		if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_FAT32)
		{
			if (e == FAT32_BAD_SECTOR)
				e = EXFAT_BAD_SECTOR;
		}
	}

	if ((e != EXFAT_BAD_SECTOR) && (e >= gbfw_fs__filesystem.clusters))
		e = EXFAT_END_OF_CHAIN;

	*entry = e;
	return true;
}



bool gbfw_fs_start(void)
{
	if (gbfw_fs__flags.enable)
		return false;

	if (!gbfw_storage_lock())
		return false;

	gbfw_fs__read_partition_table();

	if (!gbfw_fs__read_filesystem())
		return false;

	gbfw_fs__flags.enable = true;
	return true;
}

void gbfw_fs_stop(void)
{
	if (gbfw_fs__flags.enable)
		gbfw_storage_release();
	gbfw_fs__flags.enable = false;
}


static bool gbfw_fs_file__advance_cluster(gbfw_fs_file_t *file)
{
	if (file->sectors_chained == 0)
		return false;

	uint32_t cluster = gbfw_fs__cluster_from_sector(file->sector) - 1;

	uint32_t fe;
	if (!gbfw_fs__fat_read(cluster, &fe))
		return false;

	if (fe == EXFAT_BAD_SECTOR)
	{
		file->flags.error = true;
		return false;
	}

	if (exfat_is_end_of_chain(fe))
	{
		file->sectors_chained = 0;
		return false;
	}

	file->sector = gbfw_fs__cluster_to_sector(fe);

	uint32_t sectors_per_cluster = 1U << gbfw_fs__filesystem.cluster_size_log2;
	if (file->sectors_chained >= sectors_per_cluster)
	{
		file->sectors_contiguous += sectors_per_cluster;
		file->sectors_chained    -= sectors_per_cluster;
	}
	else
	{
		file->sectors_contiguous = file->sectors_chained;
		file->sectors_chained = 0;
	}

	return true;
}



bool gbfw_fs_dir_root(gbfw_fs_file_t *root)
{
	if (!gbfw_fs__flags.enable)
		return false;

	if ((gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_FAT12)
		|| (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_FAT16))
	{
		root->sector = gbfw_fs__sector(gbfw_fs__filesystem.root_directory_sector);

		uint32_t bytes = gbfw_fs__filesystem.root_directory_entries * sizeof(fat_entry_t);
		root->sectors_contiguous = (bytes + (GBFW_STORAGE_BLOCK_SIZE - 1)) / GBFW_STORAGE_BLOCK_SIZE;
		root->sectors_chained    = 0;

		root->flags.mask = 0;
		root->flags.byte_remain = GBFW_STORAGE_BLOCK_SIZE - (bytes % GBFW_STORAGE_BLOCK_SIZE);
		root->flags.byte_offset = 0;
	}
	else
	{
		root->sector = gbfw_fs__cluster_to_sector(
			gbfw_fs__filesystem.root_directory_cluster);
		root->sectors_contiguous = 1U << gbfw_fs__filesystem.cluster_size_log2;
		root->sectors_chained    = 0xFFFFFFFF;

		root->flags.mask = 0;
		root->flags.byte_remain = 0;
		root->flags.byte_offset = 0;
	}

	root->flags.read_only = gbfw_fs__flags.read_only;
	return true;
}

void gbfw_fs_dir_entry_from_sector_offset(uint32_t sector, uint8_t offset, gbfw_fs_file_t *entry)
{
	entry->sector             = sector;
	entry->sectors_contiguous = 1U << gbfw_fs__filesystem.cluster_size_log2;
	entry->sectors_chained    = 0xFFFFFFFF;

	entry->flags.mask        = 0;
	entry->flags.byte_remain = 0;
	entry->flags.byte_offset = offset * sizeof(fat_entry_t);
}

void gbfw_fs_dir_entry_to_sector_offset(const gbfw_fs_file_t *entry, uint32_t *sector, uint8_t *offset)
{
	*sector = entry->sector;
	*offset = entry->flags.byte_offset / sizeof(fat_entry_t);
}


static bool gbfw_fs_file__has_bytes_remaining(const gbfw_fs_file_t *file, uint32_t size)
{
	uint64_t sectors = file->sectors_contiguous
		+ file->sectors_chained;
	uint64_t bytes = (sectors * GBFW_STORAGE_BLOCK_SIZE)
		- (file->flags.byte_offset + file->flags.byte_remain);
	return (size <= bytes);
}

bool gbfw_fs_file_read(gbfw_fs_file_t *file, uint32_t size, void *data)
{
	if (!gbfw_fs__flags.enable || file->flags.error)
		return false;

	if (size == 0) return true;

	bool skip = (data == NULL);

	if (!gbfw_fs_file__has_bytes_remaining(file, size))
		return false;

	uint16_t first_bytes;
	if (file->flags.byte_offset > 0)
	{
		first_bytes = GBFW_STORAGE_BLOCK_SIZE
			- file->flags.byte_offset;
		if (first_bytes > size) first_bytes = size;
	}
	else
	{
		first_bytes = 0;
	}

	uint32_t middle_sectors = (size - first_bytes)
		/ GBFW_STORAGE_BLOCK_SIZE;
	uint16_t  last_bytes = (size - first_bytes) % GBFW_STORAGE_BLOCK_SIZE;

	uint32_t sectors = (first_bytes > 0 ? 1 : 0)
		+ middle_sectors + (last_bytes > 0 ? 1 : 0);

	if (file->sectors_contiguous > 0)
	{
		uint32_t sectors_cont = MIN(sectors, file->sectors_contiguous);
		if (!skip && !gbfw_storage_read_multiple_begin(file->sector, sectors_cont))
			return false;
	}

	for (uint32_t i = 0; i < sectors; i++)
	{
		if (file->sectors_contiguous == 0)
		{
			if (!skip && (i > 0) && !gbfw_storage_read_multiple_end())
				return false;

			if (!gbfw_fs_file__advance_cluster(file))
				return false;

			uint32_t sectors_cont = MIN((sectors - i), file->sectors_contiguous);
			if (!skip && !gbfw_storage_read_multiple_begin(file->sector, sectors_cont))
				return false;
		}

		bool first = (i == 0) && (file->flags.byte_offset != 0);
		bool last  = (size < GBFW_STORAGE_BLOCK_SIZE);

		if (!skip)
		{
			bool success;
			if (first || last)
			{
				success = gbfw_fs__block_read_multi(file->sector);
			}
			else
			{
				// Direct block reads bypass the block cache.
				success = gbfw_storage_read_multiple_block_begin(data)
					&& gbfw_storage_read_multiple_block_end();
			}
			if (!success)
			{
				gbfw_storage_read_multiple_end();
				return false;
			}
		}

		if (first)
		{
			if (!skip)
			{
				memcpy(data, &gbfw_fs__block[file->flags.byte_offset], first_bytes);
				data = (void *)((uintptr_t)data + first_bytes);
			}
			size -= first_bytes;

			file->flags.byte_offset += first_bytes;
			if (file->flags.byte_offset == 0)
			{
				file->sector++;
				file->sectors_contiguous--;
			}
		}
		else if (last)
		{
			if (!skip)
				memcpy(data, gbfw_fs__block, size);
			file->flags.byte_offset += size;
		}
		else
		{
			size -= GBFW_STORAGE_BLOCK_SIZE;

			if (!skip)
			{
				data = (void *)((uintptr_t)data
					+ GBFW_STORAGE_BLOCK_SIZE);
			}

			file->sector++;
			file->sectors_contiguous--;
		}
	}

	return (skip || gbfw_storage_read_multiple_end());
}


bool gbfw_fs_file_is_end(const gbfw_fs_file_t *file)
{
	if (file->sectors_chained > 0)
		return false;
	if (file->sectors_contiguous > 1)
		return false;
	if (file->sectors_contiguous == 0)
		return true;

	uint32_t bytes = file->flags.byte_offset + file->flags.byte_remain;
	return (bytes >= GBFW_STORAGE_BLOCK_SIZE);
}

static void gbfw_fs_file__set_eof(gbfw_fs_file_t *file)
{
	file->sectors_chained = 0;

	if (file->flags.byte_offset == 0)
	{
		file->sectors_contiguous = 0;
		file->flags.byte_remain  = 0;
	}
	else
	{
		file->sectors_contiguous = 1;
		file->flags.byte_remain = (GBFW_STORAGE_BLOCK_SIZE - file->flags.byte_offset);
	}
}


static bool gbfw_fs__fat_entry_is_lfn(const fat_entry_t *entry)
{
	if ((entry->dir.name[0] == FAT_ENTRY_FINAL)
		|| (entry->dir.name[0] == FAT_ENTRY_AVAILABLE))
		return false;

	if ((gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_FAT12)
		|| (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_FAT16))
	{
		if ((entry->dir.first_cluster != 0)
			|| (entry->dir.file_size == 0))
			return false;
	}

	return fat_attributes_is_lfn(entry->lfn.attributes);
}

uint32_t gbfw_fs_dir_entry_name(const gbfw_fs_file_t *entry, uint8_t max_length, uint16_t *name)
{
	uint32_t name_length;

	gbfw_fs_file_t fp = *entry;
	if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_EXFAT)
	{
		exfat_entry_t e;
		if (!gbfw_fs_file_read(&fp, sizeof(e), &e)
			|| (e.entry_type != EXFAT_ENTRY_FILE_DIRECTORY)
			|| (e.primary.secondary_count < 2))
			return 0;

		if (!gbfw_fs_file_read(&fp, sizeof(e), &e)
			|| (e.entry_type != EXFAT_ENTRY_STREAM_EXTENSION))
			return 0;

		name_length = e.stream_extension.name_length;
		if (name_length < max_length) max_length = name_length;

		while (max_length > 0)
		{
			if (!gbfw_fs_file_read(&fp, sizeof(e), &e)
				|| (e.entry_type != EXFAT_ENTRY_FILE_NAME))
				return 0;

			size_t count = sizeof(e.file_name.file_name) / sizeof(e.file_name.file_name[0]);
			for (size_t i = 0; (i < count) && (max_length > 0); i++, max_length--, name++)
				*name = e.file_name.file_name[i];
		}
	}
	else
	{
		fat_entry_t e;
		if (!gbfw_fs_file_read(&fp, sizeof(e), &e))
			return 0;

		if (e.dir.name[0] == FAT_ENTRY_AVAILABLE)
			return 0;

		bool is_lfn = gbfw_fs__fat_entry_is_lfn(&e);
		if (!is_lfn)
		{
			if (e.dir.name[0] == FAT_ENTRY_IS_E5)
				e.dir.name[0] = 0xE5;

			name_length = 0;
			for (uint8_t i = 0; (i < sizeof(e.dir.name)) && (e.dir.name[i] != ' ') && (name_length < max_length); i++, name_length++)
					name[name_length] = e.dir.name[i];

			if (!e.dir.attributes.subdirectory
				&& (e.dir.extension[0] != ' '))
			{
				if (name_length < max_length)
					name[name_length] = '.';
				name_length++;
			}

			for (uint8_t i = 0; (i < sizeof(e.dir.extension)) && (e.dir.extension[i] != ' ') && (name_length < max_length); i++, name_length++)
					name[name_length] = e.dir.extension[i];
		}
		else
		{
			if ((e.lfn.sequence & FAT_ENTRY_LFN_SEQ_FLAG_FINAL) == 0)
				return 0;

			uint32_t offset = ((e.lfn.sequence & 0x1F) - 1) * FAT_LFN_CHARACTERS_PER_ENTRY;
			name_length = offset;
			for (uint8_t i = 0; (i < FAT_LFN_CHARACTERS_PER_ENTRY)
				&& (fat_lfn_name(&e.lfn, i) != '\0'); i++, name_length++);

			while (true)
			{
				for (uint8_t i = 0; (i < FAT_LFN_CHARACTERS_PER_ENTRY)
					&& (fat_lfn_name(&e.lfn, i) != '\0')
					&& (offset < max_length); i++, offset++)
				{
					name[offset] = fat_lfn_name(&e.lfn, i);
				}

				if ((e.lfn.sequence & 0x1F) == 1)
					break;

				if (!gbfw_fs_file_read(&fp, sizeof(e), &e)
					|| !gbfw_fs__fat_entry_is_lfn(&e))
					return 0;

				offset = ((e.lfn.sequence & 0x1F) - 1) * FAT_LFN_CHARACTERS_PER_ENTRY;
			}
		}
	}

	return name_length;
}

bool gbfw_fs_dir_entry__match_name(gbfw_fs_file_t *entry, const uint16_t *name)
{
	if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_EXFAT)
	{
		exfat_entry_t e;
		if (!gbfw_fs_file_read(entry, sizeof(e), &e))
			return false;

		if (e.entry_type == EXFAT_ENTRY_FINAL)
		{
			gbfw_fs_file__set_eof(entry);
			return false;
		}

		if (!exfat_entry_type_is_active(e.entry_type)
			|| !exfat_entry_type_is_primary(e.entry_type))
			return false;

		bool is_match = true;
		uint8_t secondary_count = e.primary.secondary_count;
		uint8_t match_count = 0;
		for (uint8_t j = 0; (j < secondary_count) && is_match; j++)
		{
			gbfw_fs_file_t ep = *entry;
			if (!gbfw_fs_file_read(&ep, sizeof(e), &e))
			{
				*entry = ep;
				return false;
			}

			if (e.entry_type == EXFAT_ENTRY_FINAL)
			{
				gbfw_fs_file__set_eof(entry);
				return false;
			}

			if (exfat_entry_type_is_primary(e.entry_type))
				return false;
			*entry = ep;

			if (!is_match) continue;

			if (e.entry_type != EXFAT_ENTRY_FILE_NAME)
				continue;

			const uint8_t fnl = sizeof(e.file_name.file_name);
			const uint16_t *fn = e.file_name.file_name;
			for (uint8_t i = 0; (i < fnl) && (fn[i] != '\0') && is_match; i++)
				is_match = (fn[i] == name[match_count++]);
		}

		if (!is_match || (name[match_count] != '\0'))
			return false;
	}
	else
	{
		fat_entry_t e;
		if (!gbfw_fs_file_read(entry, sizeof(e), &e))
			return false;

		if (e.dir.name[0] == FAT_ENTRY_FINAL)
		{
			gbfw_fs_file__set_eof(entry);
			return false;
		}

		if (e.dir.name[0] == FAT_ENTRY_AVAILABLE)
			return false;

		if (gbfw_fs__fat_entry_is_lfn(&e))
		{
			if ((e.lfn.sequence & FAT_ENTRY_LFN_SEQ_FLAG_FINAL) == 0)
				return false;

			bool is_match = true;

			uint8_t nlen;
			for (nlen = 0; (nlen < 255) && (name[nlen] != '\0'); nlen++);
			if (name[nlen] != '\0') is_match = false;

			uint16_t max_len = (e.lfn.sequence & 0x1F) * FAT_LFN_CHARACTERS_PER_ENTRY;
			if (max_len < nlen) is_match = false;

			if (e.lfn.name4_0[0] == '\0')
				is_match = false;

			uint16_t elen = 1;
			if (is_match)
			{
				for (; (elen < FAT_LFN_CHARACTERS_PER_ENTRY)
					&& (fat_lfn_name(&e.lfn, elen) != 0); elen++);

				uint16_t flen = (max_len - FAT_LFN_CHARACTERS_PER_ENTRY) + elen;
				if (flen != nlen) is_match = false;
			}

			const uint16_t *nptr = &name[max_len - FAT_LFN_CHARACTERS_PER_ENTRY];
			uint8_t s = e.lfn.sequence & 0x1F;
			while (true)
			{
				for (uint8_t i = 0; is_match && (i < elen); i++)
				{
					if (fat_lfn_name(&e.lfn, i) != nptr[i])
						is_match = false;
				}

				gbfw_fs_file_t ep = *entry;
				if (!gbfw_fs_file_read(&ep, sizeof(e), &e))
				{
					*entry = ep;
					return false;
				}

				if (--s == 0)
				{
					*entry = ep;
					break;
				}

				if ((e.lfn.sequence & 0x1F) != s)
					return false;
				*entry = ep;

				elen  = FAT_LFN_CHARACTERS_PER_ENTRY;
				nptr -= FAT_LFN_CHARACTERS_PER_ENTRY;
			}

			if (!is_match) return false;
		}
		else
		{
			if (e.dir.name[0] == FAT_ENTRY_IS_E5)
				e.dir.name[0] = 0xE5;

			uint8_t eo = 0;
			for (uint8_t i = 0; (i < sizeof(e.dir.name)) && (e.dir.name[i] != ' '); i++)
			{
				uint16_t c = name[eo++];
				if (c > 0xFF) return false;

				if (c == ' ') return false;

				if ((c >= 'a') && (c <= 'z'))
					c -= ('a' - 'A');

				if (c != e.dir.name[i]) return false;
			}

			if (!e.dir.attributes.subdirectory)
			{
				if (name[eo] == '.')
					eo++;
				else if (e.dir.extension[0] != ' ')
					return false;
			}

			for (uint8_t i = 0; (i < sizeof(e.dir.extension)) && (e.dir.extension[i] != ' '); i++)
			{
				uint16_t c = name[eo++];
				if (c > 0xFF) return false;

				if (c == ' ') return false;

				if ((c >= 'a') && (c <= 'z'))
					c -= ('a' - 'A');

				if (c != e.dir.extension[i])
					return false;
			}

			if (name[eo] != ' ') return false;
		}
	}

	return true;
}

bool gbfw_fs_dir_entry_match_name(const gbfw_fs_file_t *entry, const uint16_t *name)
{
	gbfw_fs_file_t e = *entry;
	return gbfw_fs_dir_entry__match_name(&e, name);
}

bool gbfw_fs_dir_find_by_name(const gbfw_fs_file_t *dir, const uint16_t *name, gbfw_fs_file_t *entry)
{
	gbfw_fs_file_t dp = *dir;
	while (!dp.flags.error && !gbfw_fs_file_is_end(&dp))
	{
		gbfw_fs_file_t ep = dp;
		if (gbfw_fs_dir_entry__match_name(&dp, name))
		{
			*entry = ep;
			return true;
		}
	}

	return false;
}

bool gbfw_fs_dir_entry_first(gbfw_fs_file_t *entry)
{
	gbfw_fs_file_t fp = *entry;

	if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_EXFAT)
	{
		exfat_entry_t e;
		while (true)
		{
			gbfw_fs_file_t peek = fp;
			if (!gbfw_fs_file_read(&peek, sizeof(e), &e))
				return false;

			if (e.entry_type == EXFAT_ENTRY_FINAL)
				return false;

			if (exfat_entry_type_is_active(e.entry_type)
				&& exfat_entry_type_is_primary(e.entry_type))
				break;

			fp = peek;
		}
	}
	else
	{
		fat_entry_t e;
		while (true)
		{
			gbfw_fs_file_t peek = fp;
			if (!gbfw_fs_file_read(&peek, sizeof(e), &e))
				return false;

			if (e.dir.name[0] == FAT_ENTRY_FINAL)
				return false;

			if (e.dir.name[0] != FAT_ENTRY_AVAILABLE)
				break;

			fp = peek;
		}
	}

	*entry = fp;
	return true;
}

bool gbfw_fs_dir_entry_next(gbfw_fs_file_t *entry)
{
	gbfw_fs_file_t fp = *entry;

	if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_EXFAT)
	{
		exfat_entry_t e;
		if (!gbfw_fs_file_read(&fp, sizeof(e), &e))
			return false;

		if (exfat_entry_type_is_primary(e.entry_type))
		{
			if (!gbfw_fs_file_skip(&fp, (e.primary.secondary_count * sizeof(e))))
				return false;
		}
	}
	else
	{
		fat_entry_t e;
		if (!gbfw_fs_file_read(&fp, sizeof(e), &e))
			return false;

		if (gbfw_fs__fat_entry_is_lfn(&e))
		{
			uint8_t sequence = (e.lfn.sequence & 0x1F);
			if (!gbfw_fs_file_skip(&fp, (sequence * sizeof(e))))
				return false;
		}
	}

	if (!gbfw_fs_dir_entry_first(&fp))
		return false;

	*entry = fp;
	return true;
}

static inline bool gbfw_fs__fat_entry_skip_lfn(
	gbfw_fs_file_t *file, fat_entry_t *entry)
{
	if (!gbfw_fs_file_read(file, sizeof(*entry), entry))
		return false;

	bool is_lfn = gbfw_fs__fat_entry_is_lfn(entry);
	if (is_lfn)
	{
		uint8_t sequence = (entry->lfn.sequence & 0x1F);
		if (!gbfw_fs_file_skip(file, ((sequence - 1) * sizeof(*entry))))
			return false;

		if (!gbfw_fs_file_read(file, sizeof(*entry), entry))
			return false;
	}

	return ((entry->dir.name[0] != FAT_ENTRY_FINAL)
		&& (entry->dir.name[0] != FAT_ENTRY_AVAILABLE));
}

bool gbfw_fs_dir_entry_file(const gbfw_fs_file_t *entry, gbfw_fs_file_t *file)
{
	gbfw_fs_file_t ep = *entry;
	if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_EXFAT)
	{
		exfat_entry_t e;
		if (!gbfw_fs_file_read(&ep, sizeof(e), &e))
			return false;

		if (!exfat_entry_type_is_primary(e.entry_type))
			return false;

		bool read_only = false;
		if (e.entry_type == EXFAT_ENTRY_FILE_DIRECTORY)
			read_only = e.file_directory.attributes.read_only;

		bool has_secondary = ((e.entry_type != EXFAT_ENTRY_ALLOCATION_BITMAP)
			&& (e.entry_type != EXFAT_ENTRY_UP_CASE_TABLE));
		if (has_secondary)
		{
			uint8_t secondary_count = e.primary.secondary_count;
			uint8_t j;
			for (j = 0; j < secondary_count; j++)
			{
				
				if (!gbfw_fs_file_read(&ep, sizeof(e), &e))
					return false;

				if (exfat_entry_type_is_primary(e.entry_type))
					return false;

				if ((e.entry_type == EXFAT_ENTRY_STREAM_EXTENSION)
					|| (e.entry_type == EXFAT_ENTRY_VENDOR_ALLOCATION))
					break;
			}
			if (j >= secondary_count) return false;
		}

		if (has_secondary && !e.secondary.flags.allocation_possible)
			return false;

		uint64_t bytes = e.stream_extension.data_length;
		if (bytes == 0) return false;
	
		uint64_t sectors = (bytes + (GBFW_STORAGE_BLOCK_SIZE - 1)) / GBFW_STORAGE_BLOCK_SIZE;
		if ((sectors >> 32) != 0) return false;

		file->sector = gbfw_fs__cluster_to_sector(e.stream_extension.first_cluster);

		if (has_secondary && e.secondary.flags.no_fat_chain)
		{
			file->sectors_contiguous = sectors;
			file->sectors_chained    = 0;
		}
		else
		{
			uint32_t sectors_per_cluster = 1U << gbfw_fs__filesystem.cluster_size_log2;
			file->sectors_contiguous = MIN(sectors, sectors_per_cluster);
			file->sectors_chained    = sectors - file->sectors_contiguous;
		}

		file->flags.mask = 0;
		file->flags.byte_remain = GBFW_STORAGE_BLOCK_SIZE - (bytes % GBFW_STORAGE_BLOCK_SIZE);
		file->flags.read_only   = read_only;
	}
	else
	{
		fat_entry_t e;
		if (!gbfw_fs__fat_entry_skip_lfn(&ep, &e))
			return false;

		if (!e.dir.attributes.subdirectory
			&& (e.dir.file_size == 0))
			return false;

		file->sector          = gbfw_fs__cluster_to_sector(e.dir.first_cluster);
		file->flags.mask      = 0;
		file->flags.read_only = e.dir.attributes.read_only;

		uint32_t sectors_per_cluster = 1U << gbfw_fs__filesystem.cluster_size_log2;
		if (e.dir.file_size == 0)
		{
			file->sectors_contiguous = sectors_per_cluster;
			file->sectors_chained    = 0xFFFFFFFF;
		}
		else
		{
			uint32_t sectors = (e.dir.file_size + (GBFW_STORAGE_BLOCK_SIZE - 1)) / GBFW_STORAGE_BLOCK_SIZE;
			file->sectors_contiguous = MIN(sectors, sectors_per_cluster);
			file->sectors_chained    = sectors - file->sectors_contiguous;
			file->flags.byte_remain  = GBFW_STORAGE_BLOCK_SIZE - (e.dir.file_size % GBFW_STORAGE_BLOCK_SIZE);
		}
	}

	return true;
}

static bool gbfw_fs_dir_entry__attributes(
	const gbfw_fs_file_t *entry, exfat_attributes_t *attributes)
{
	gbfw_fs_file_t ep = *entry;
	if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_EXFAT)
	{
		exfat_entry_t e;
		if (!gbfw_fs_file_read(&ep, sizeof(e), &e))
			return false;

		if (e.entry_type != EXFAT_ENTRY_FILE_DIRECTORY)
			return false;

		attributes->mask = e.file_directory.attributes.mask;
	}
	else
	{
		fat_entry_t e;
		if (!gbfw_fs__fat_entry_skip_lfn(&ep, &e))
			return false;

		if ((e.dir.name[0] == FAT_ENTRY_FINAL)
			|| (e.dir.name[0] == FAT_ENTRY_AVAILABLE))
			return false;

		attributes->mask = e.dir.attributes.mask;
	}

	return true;
}

bool gbfw_fs_dir_entry_is_directory(const gbfw_fs_file_t *entry)
{
	exfat_attributes_t attributes;
	if (!gbfw_fs_dir_entry__attributes(entry, &attributes))
		return false;

	return attributes.subdirectory;
}

bool gbfw_fs_dir_entry_is_read_only(const gbfw_fs_file_t *entry)
{
	exfat_attributes_t attributes;
	if (!gbfw_fs_dir_entry__attributes(entry, &attributes))
		return false;

	return attributes.read_only;
}

bool gbfw_fs_dir_entry_from_path(const uint16_t *path, gbfw_fs_file_t *entry)
{
	gbfw_fs_file_t dir;
	if (!gbfw_fs_dir_root(&dir))
		return false;

	if (path[0] == '/')
		path++;

	while (true)
	{
		uintptr_t len;
		for (len = 0; (path[len] != '\0') && (path[len] != '/'); len++);
		if (path[len] == '\0') break;

		uint16_t dir_name[len + 1];
		memcpy(dir_name, path, (len * sizeof(dir_name[0])));
		dir_name[len] = '\0';

		if (!gbfw_fs_dir_find_by_name(&dir, dir_name, &dir))
			return false;
		path += (len + 1);

		if (!gbfw_fs_dir_entry_is_directory(&dir))
			return false;
	}

	return gbfw_fs_dir_find_by_name(&dir, path, entry);
}

uint64_t gbfw_fs_dir_entry_size(const gbfw_fs_file_t *entry)
{
	gbfw_fs_file_t ep = *entry;
	if (gbfw_fs__filesystem.flags.type == GBFW_FS_FILESYSTEM_EXFAT)
	{
		exfat_entry_t e;
		if (!gbfw_fs_file_read(&ep, sizeof(e), &e))
			return false;

		if (!exfat_entry_type_is_primary(e.entry_type))
			return false;

		bool has_secondary = ((e.entry_type != EXFAT_ENTRY_ALLOCATION_BITMAP)
			&& (e.entry_type != EXFAT_ENTRY_UP_CASE_TABLE));
		if (has_secondary)
		{
			uint8_t secondary_count = e.primary.secondary_count;
			uint8_t j;
			for (j = 0; j < secondary_count; j++)
			{
				if (!gbfw_fs_file_read(&ep, sizeof(e), &e))
					return false;

				if (exfat_entry_type_is_primary(e.entry_type))
					return false;

				if ((e.entry_type == EXFAT_ENTRY_STREAM_EXTENSION)
					|| (e.entry_type == EXFAT_ENTRY_VENDOR_ALLOCATION))
					break;
			}
			if (j >= secondary_count) return false;
		}

		return e.stream_extension.data_length;
	}
	else
	{
		fat_entry_t e;
		if (!gbfw_fs__fat_entry_skip_lfn(&ep, &e))
			return false;

		return e.dir.file_size;
	}

	return true;
}
