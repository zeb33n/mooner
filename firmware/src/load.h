#ifndef GBFW_PRIVATE_LOAD_H
#define GBFW_PRIVATE_LOAD_H

#include <gbfw/load.h>

#include <stdbool.h>
#include <stdint.h>
#include <gbfw/fs.h>

void *gbfw_load(const gbfw_fs_file_t *entry);
bool  gbfw_exec(void *entry);
bool  gbfw_signal(uint32_t signal);

#endif
