#ifndef GBFW_PRIVATE_ANALOG_H
#define GBFW_PRIVATE_ANALOG_H

#include <stdbool.h>
#include <stdint.h>
#include <gbfw/analog.h>

void     gbfw_analog_init(void);
uint16_t gbfw_analog_usb_voltage(void);
uint16_t gbfw_analog_battery_voltage(void);

#endif
