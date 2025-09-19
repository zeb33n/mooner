#ifndef GBFW_PRIVATE_DISPLAY_H
#define GBFW_PRIVATE_DISPLAY_H

#include <gbfw/display.h>

typedef enum
{
	GBFW_DISPLAY_STATE_UNINITIALIZED = 0,
	GBFW_DISPLAY_STATE_INITIALIZING  = 1,
	GBFW_DISPLAY_STATE_ERROR         = 2,
	GBFW_DISPLAY_STATE_READY         = 3,
} gbfw_display_state_e;

#define GBFW_DISPLAY_SCANBUFF_COUNT (GBFW_DISPLAY_WIDTH * 2)
extern rgb565_t gbfw_display_scanbuff[GBFW_DISPLAY_SCANBUFF_COUNT];

void                 gbfw_display_init(void);
gbfw_display_state_e gbfw_display_state(void);

#endif
