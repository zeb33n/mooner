#ifndef GBFW_PRIVATE_POWER_H
#define GBFW_PRIVATE_POWER_H

#include <gbfw/power.h>
#include <stdbool.h>

void gbfw_power_init(void);

bool gbfw_power_is_on(void);

bool gbfw_power_hold(void);
void gbfw_power_release(void);

#endif
