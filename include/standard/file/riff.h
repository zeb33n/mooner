#ifndef RIFF_H
#define RIFF_H

#include <stdint.h>

typedef struct
{
	char     name[4];
	uint32_t size;
} riff_t;

#endif
