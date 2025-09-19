#ifndef GBFW_CONTROL_H
#define GBFW_CONTROL_H

#include <stdint.h>

typedef enum
{
	GBFW_CONTROL_UP     =  0,
	GBFW_CONTROL_DOWN   =  1,
	GBFW_CONTROL_LEFT   =  2,
	GBFW_CONTROL_RIGHT  =  3,
	GBFW_CONTROL_Y      =  4,
	GBFW_CONTROL_A      =  5,
	GBFW_CONTROL_X      =  6,
	GBFW_CONTROL_B      =  7,
	GBFW_CONTROL_START  =  8,
	GBFW_CONTROL_SELECT =  9,
	GBFW_CONTROL_L1     = 10,
	GBFW_CONTROL_R1     = 11,
	GBFW_CONTROL_COUNT
} gbfw_control_e;

typedef union
{
	struct
	{
		uint16_t up     : 1;
		uint16_t down   : 1;
		uint16_t left   : 1;
		uint16_t right  : 1;
		uint16_t y      : 1;
		uint16_t a      : 1;
		uint16_t x      : 1;
		uint16_t b      : 1;
		uint16_t start  : 1;
		uint16_t select : 1;
		uint16_t l1     : 1;
		uint16_t r1     : 1;
		uint16_t res12  : 1;
		uint16_t res13  : 1;
		uint16_t res14  : 1;
		uint16_t res15  : 1;
	};

	uint16_t mask;
} gbfw_control_t;


gbfw_control_t gbfw_control_poll(void);

#endif
