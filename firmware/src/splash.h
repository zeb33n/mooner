#ifndef GBFW_PRIVATE_SPLASH_H
#define GBFW_PRIVATE_SPLASH_H

#include <stdbool.h>

void gbfw_splash(void);
bool gbfw_splash_is_complete(void);

typedef enum
{
	GBFW_SPLASH_ERROR_FILESYSTEM = 0,
	GBFW_SPLASH_ERROR_BOOT       = 1,
	GBFW_SPLASH_ERROR_RECOVERY   = 2,
} gbfw_splash_error_e;

void gbfw_splash_error(gbfw_splash_error_e error);

#endif
