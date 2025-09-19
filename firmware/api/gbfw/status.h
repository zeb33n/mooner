#ifndef GBFW_STATUS_H
#define GBFW_STATUS_H

#include <stdint.h>
#include <standard/pixel/rgb888.h>

void gbfw_status_set(rgb888_t color, uint8_t brightness);

#define gbfw_status_set_good()  gbfw_status_set(RGB888_GREEN, 255)
#define gbfw_status_set_error() gbfw_status_set(RGB888_RED  , 255)

#endif
