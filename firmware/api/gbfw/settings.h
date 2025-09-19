#ifndef GBFW_SETTINGS_H
#define GBFW_SETTINGS_H

#include <stdint.h>

typedef enum
{
	GBFW_SETTINGS_SPLASH_DISABLED = 0,
	GBFW_SETTINGS_SPLASH_STATIC   = 1,
	GBFW_SETTINGS_SPLASH_MUTED    = 2,
	GBFW_SETTINGS_SPLASH_FULL     = 3,
} gbfw_settings_splash_e;

typedef enum
{
	GBFW_SETTINGS_HOME_DISABLED = 0,
	GBFW_SETTINGS_HOME_DEFAULT  = 1,
	GBFW_SETTINGS_HOME_QUIT     = 2,
} gbfw_settings_home_e;

typedef enum
{
	GBFW_BOOT_DEFAULT = 0,
	GBFW_BOOT_FILE    = 1,
	GBFW_BOOT_FAIL    = 2,
	GBFW_BOOT_MEMORY  = 3,
} gbfw_boot_e;

typedef union
{
	struct
	{
		uint32_t brightness  : 4;
		uint32_t volume      : 4;
		uint32_t led_level   : 4;
		uint32_t splash      : 2;
		uint32_t home        : 2;
		uint32_t exec_entry  : 4;
		uint32_t res28_22    : 7;
		uint32_t skip_splash : 1;
		uint32_t exec_valid  : 1;
		uint32_t exec_fail   : 1;
		uint32_t boot_fail   : 1;
		uint32_t valid       : 1;
	};

	uint32_t mask;
} gbfw_settings_primary_t;

typedef struct
{
	gbfw_settings_primary_t primary;
	uint32_t                exec_sector;
} gbfw_settings_t;


volatile gbfw_settings_t *gbfw_settings_pointer(void);

#endif
