#ifndef GBFW_FS_H
#define GBFW_FS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


typedef struct
{
	uint32_t sector;
	uint32_t sectors_contiguous;
	uint32_t sectors_chained;

	union
	{
		struct
		{
			uint32_t byte_remain :  9;
			uint32_t byte_offset :  9;
			uint32_t res29_18    : 12;
			uint32_t read_only   :  1;
			uint32_t error       :  1;
		};

		uint32_t mask;
	} flags;
} gbfw_fs_file_t;


bool gbfw_fs_start(void);
void gbfw_fs_stop(void);

bool gbfw_fs_dir_root(gbfw_fs_file_t *root);
bool gbfw_fs_dir_find_by_name(const gbfw_fs_file_t *dir, const uint16_t *name, gbfw_fs_file_t *entry);

bool     gbfw_fs_dir_entry_from_path(const uint16_t *path, gbfw_fs_file_t *entry);
uint64_t gbfw_fs_dir_entry_size(const gbfw_fs_file_t *entry);
uint32_t gbfw_fs_dir_entry_name(const gbfw_fs_file_t *entry, uint8_t max_length, uint16_t *name);
bool     gbfw_fs_dir_entry_match_name(const gbfw_fs_file_t *entry, const uint16_t *name);
bool     gbfw_fs_dir_entry_is_read_only(const gbfw_fs_file_t *entry);
bool     gbfw_fs_dir_entry_is_directory(const gbfw_fs_file_t *entry);
bool     gbfw_fs_dir_entry_file(const gbfw_fs_file_t *entry, gbfw_fs_file_t *file);
bool     gbfw_fs_dir_entry_first(gbfw_fs_file_t *entry);
bool     gbfw_fs_dir_entry_next(gbfw_fs_file_t *entry);

bool gbfw_fs_file_is_end(const gbfw_fs_file_t *file);
bool gbfw_fs_file_read(gbfw_fs_file_t *file, uint32_t size, void *data);
//bool gbfw_fs_file_write(gbfw_fs_file_t *position, uint32_t size, const void *data);

#define gbfw_fs_file_skip(file, size) gbfw_fs_file_read(file, size, NULL)

#endif
