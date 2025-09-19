#ifndef GBFW_PRIVATE_BATTERY_H
#define GBFW_PRIVATE_BATTERY_H

#include <gbfw/battery.h>
#include <stdbool.h>

void gbfw_battery_init(void);

void gbfw_battery_fast_charge(bool enable);

#endif
