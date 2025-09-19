#ifndef GBFW_PRIVATE_FLASH_H
#define GBFW_PRIVATE_FLASH_H

#include <stddef.h>
#include <stdbool.h>

bool gbfw_flash_unlock(void);
void gbfw_flash_lock(bool hard);

bool gbfw_flash_erase(void *addr, size_t size);

bool gbfw_flash_program_begin(void);
bool gbfw_flash_program_line(void *addr, const void *data);
void gbfw_flash_program_end(void);

#endif
