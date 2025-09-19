#ifndef GBFW_PRIVATE_SETTINGS_H
#define GBFW_PRIVATE_SETTINGS_H

#include <gbfw/settings.h>

extern volatile gbfw_settings_t * const gbfw_settings;

void gbfw_settings_init(void);

uint8_t gbfw_settings_brightness_get(void);

#endif
