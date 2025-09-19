#ifndef GBFW_LOAD_H
#define GBFW_LOAD_H

#include <stdnoreturn.h>
#include <gbfw/fs.h>

typedef enum
{
	GBFW_SIGNAL_BEGIN = 0,
	GBFW_SIGNAL_EXIT  = 1,
	GBFW_SIGNAL_HOME  = 2,
} gbfw_signal_e;

_Noreturn void gbfw_exec_file(const gbfw_fs_file_t *entry);

#endif
