#ifndef GBFW_CORDIC_H
#define GBFW_CORDIC_H

#include <stdint.h>

typedef enum
{
	GBFW_CORDIC_FUNC_SIN32      = 0,
	GBFW_CORDIC_FUNC_SIN32_FAST = 1,
} gbfw_cordic_func_e;

void gbfw_cordic_begin(gbfw_cordic_func_e func);
void gbfw_cordic_end(void);

uint32_t gbfw_cordic_process_simple(uint32_t arg);

#endif
