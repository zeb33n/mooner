#ifndef GBFW_PRIVATE_TASK_H
#define GBFW_PRIVATE_TASK_H

#include <stdint.h>
#include <stdbool.h>

void gbfw_task_init(void);
bool gbfw_task_schedule(void (*func)(void), uint32_t ticks);

#endif
