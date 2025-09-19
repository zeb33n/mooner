#ifndef GBFW_BATTERY_H
#define GBFW_BATTERY_H

#include <stdbool.h>
#include <stdint.h>

bool    gbfw_battery_is_charging(void);
uint8_t gbfw_battery_level(void);

#endif
