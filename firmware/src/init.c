#include "init.h"
#include "clocks.h"
#include "tick.h"
#include "delay.h"
#include "task.h"
#include "rtc.h"
#include "settings.h"
#include "power.h"
#include "battery.h"
#include "analog.h"
#include "random.h"
#include "storage.h"
#include "display.h"
#include "control.h"
#include "status.h"
#include "audio.h"
#include "usb.h"
#include "splash.h"
#include "load.h"
#include "fs.h"
#include "sandbox.h"
#include <gbfw/api.h>
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/gpio.h>
#include <hardware/stm32/pwr.h>
#include <hardware/cm4/intrinsics.h>
#include <stdnoreturn.h>


_Noreturn void gbfw_init(void)
{
	// PWR is needed both for clocks and to disable UCPD.
	stm32_rcc->apb1en1.pwr = true;

	gbfw_clocks_default();

	// Disable UCPD otherwise pull-downs stop storage from working.
	stm32_pwr->c3.ucpd1_dbdis = true;

	// Enable access to RTC and backup domain registers.
	stm32_pwr->c1.dbp = true;

	// Initialize GPIO ports
	stm32_rcc_ahb2_t ahb2en = { .mask = stm32_rcc->ahb2en.mask };
	ahb2en.gpioa = true;
	ahb2en.gpiob = true;
	ahb2en.gpioc = true;
	ahb2en.gpiof = true;
	stm32_rcc->ahb2en.mask = ahb2en.mask;

	// Enable SysConfig
	stm32_rcc->apb2en.syscfg = true;

	gbfw_tick_init();
	gbfw_delay_init();
	gbfw_task_init();

	gbfw_rtc_init();
	gbfw_settings_init();

	gbfw_power_init();
	gbfw_battery_init();
	gbfw_analog_init();

	wfi_while(!gbfw_power_is_on());

	gbfw_status_init();
	gbfw_status_set_good();

	gbfw_display_init();
	gbfw_storage_init();
	gbfw_random_init();
	gbfw_control_init();
	gbfw_audio_init();
	gbfw_usb_init();

	wfi_while(gbfw_display_state() == GBFW_DISPLAY_STATE_INITIALIZING);
	if (gbfw_display_state() != GBFW_DISPLAY_STATE_READY)
	{
		// Enable mass storage so the user can recover files from a
		// device with a broken or disconnected screen.
		gbfw_usb_mass_storage_enable();

		gbfw_status_set_error();
		while(true);
	}

	gbfw_splash();

	gbfw_control_t control = gbfw_control_poll();
	if (control.l1)
		gbfw_splash_error(GBFW_SPLASH_ERROR_RECOVERY);

	while (true)
	{
		while (!gbfw_fs_start())
		{
			// Enable mass storage so the user can recover a corrupt card.
			gbfw_splash_error(GBFW_SPLASH_ERROR_FILESYSTEM);
		}

		if (!gbfw_settings->primary.boot_fail)
		{
			bool boot_exec = (gbfw_settings->primary.exec_valid
				&& (gbfw_settings->exec_sector != 0xFFFFFFFF));

			gbfw_fs_file_t exec_entry;
			if (boot_exec)
			{
				gbfw_settings->primary.exec_valid = false;
				gbfw_settings->primary.exec_fail  = false;
				gbfw_fs_dir_entry_from_sector_offset(
					gbfw_settings->exec_sector,
					gbfw_settings->primary.exec_entry,
					&exec_entry);
			}
			else if (!gbfw_fs_dir_entry_from_path(u"boot.elf", &exec_entry))
			{
				exec_entry.flags.error = true;
			}

			void *entry = gbfw_load(&exec_entry);
			if (entry)
			{
				// Don't execute the application/menu during splash.
				wfi_while(!gbfw_splash_is_complete());

				if (!boot_exec)
					gbfw_settings->primary.boot_fail = true;

				gbfw_status_set_good();
				gbfw_sandbox();
				if (!gbfw_exec(entry))
				{
					if (boot_exec)
						gbfw_settings->primary.exec_fail = true;
					else
						gbfw_settings->primary.boot_fail = true;
				}

				gbfw_power_shutdown();
			}
			else
			{
				if (boot_exec)
					gbfw_settings->primary.exec_fail = true;
				else
					gbfw_settings->primary.boot_fail = true;
			}
		}

		if (gbfw_settings->primary.boot_fail)
		{
			// Enable mass storage so boot.elf can be provided.
			gbfw_splash_error(GBFW_SPLASH_ERROR_BOOT);
			gbfw_settings->primary.boot_fail = false;
		}

		gbfw_fs_stop();
	}
}
