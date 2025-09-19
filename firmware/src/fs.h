#ifndef GBFW_PRIVATE_FS_H
#define GBFW_PRIVATE_FS_H

#include <gbfw/fs.h>

void gbfw_fs_dir_entry_from_sector_offset(uint32_t sector, uint8_t offset, gbfw_fs_file_t *entry);
void gbfw_fs_dir_entry_to_sector_offset(const gbfw_fs_file_t *entry, uint32_t *sector, uint8_t *offset);

#endif
