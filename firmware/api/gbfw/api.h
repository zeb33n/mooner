#ifndef GBFW_API_H
#define GBFW_API_H

#include <stddef.h>
#include <stdint.h>
#include <gbfw/tick.h>
#include <gbfw/delay.h>
#include <gbfw/random.h>
#include <gbfw/status.h>
#include <gbfw/power.h>
#include <gbfw/settings.h>
#include <gbfw/load.h>
#include <gbfw/display.h>
#include <gbfw/audio.h>
#include <gbfw/control.h>
#include <gbfw/home.h>
#include <gbfw/battery.h>
#include <gbfw/analog.h>
#include <gbfw/usb.h>
#include <gbfw/usb/mass_storage.h>
#include <gbfw/cordic.h>
#include <gbfw/draw.h>
#include <gbfw/font.h>
#include <gbfw/fs.h>


#define GBFW_API_VERSION 1
#define GBFW_API_COUNT  ((sizeof(gbfw_api_t) / sizeof(void (*)(void))) - 1)

#define GBFW_API_FUNCTION(x) __typeof__(&gbfw_##x) x
#define GBFW_API_VARIABLE(x) __typeof__(gbfw_##x) x

typedef struct
__attribute__((__packed__))
{
	uint16_t version;
	uint16_t count;

	// These entries are special as their symbols can't be renamed.
	void (*memcpy)(void * restrict dst, const void * restrict src, size_t size);
	void (*memset)(void *dst, int value, size_t size);

	GBFW_API_FUNCTION(tick_get);

	GBFW_API_FUNCTION(delay_us);

	GBFW_API_FUNCTION(random_u32);

	GBFW_API_FUNCTION(status_set);

	GBFW_API_FUNCTION(power_shutdown);

	GBFW_API_FUNCTION(settings_pointer);

	GBFW_API_FUNCTION(exec_file);

	GBFW_API_FUNCTION(display_backlight_set);
	GBFW_API_FUNCTION(display_render_region);
	GBFW_API_FUNCTION(display_sync);

	GBFW_API_FUNCTION(audio_volume_set);
	GBFW_API_FUNCTION(audio_volume_mute);
	GBFW_API_FUNCTION(audio_rate_set);
	GBFW_API_FUNCTION(audio_start);
	GBFW_API_FUNCTION(audio_stop);
	GBFW_API_FUNCTION(audio_continue);

	GBFW_API_FUNCTION(control_poll);

	GBFW_API_FUNCTION(home);

	GBFW_API_FUNCTION(battery_is_charging);
	GBFW_API_FUNCTION(battery_level);

	GBFW_API_FUNCTION(analog_temperature);

	GBFW_API_FUNCTION(usb_device_register);
	GBFW_API_FUNCTION(usb_endpoint_stall);
	GBFW_API_FUNCTION(usb_endpoint_set_packet);

	GBFW_API_FUNCTION(usb_mass_storage_enable);
	GBFW_API_FUNCTION(usb_mass_storage_disable);

	GBFW_API_FUNCTION(cordic_begin);
	GBFW_API_FUNCTION(cordic_end);
	GBFW_API_FUNCTION(cordic_process_simple);

	GBFW_API_FUNCTION(fs_start);
	GBFW_API_FUNCTION(fs_stop);
	GBFW_API_FUNCTION(fs_dir_root);
	GBFW_API_FUNCTION(fs_dir_find_by_name);
	GBFW_API_FUNCTION(fs_dir_entry_from_path);
	GBFW_API_FUNCTION(fs_dir_entry_size);
	GBFW_API_FUNCTION(fs_dir_entry_name);
	GBFW_API_FUNCTION(fs_dir_entry_match_name);
	GBFW_API_FUNCTION(fs_dir_entry_is_read_only);
	GBFW_API_FUNCTION(fs_dir_entry_is_directory);
	GBFW_API_FUNCTION(fs_dir_entry_file);
	GBFW_API_FUNCTION(fs_dir_entry_first);
	GBFW_API_FUNCTION(fs_dir_entry_next);
	GBFW_API_FUNCTION(fs_file_is_end);
	GBFW_API_FUNCTION(fs_file_read);
	//GBFW_API_FUNCTION(fs_file_write);

	GBFW_API_FUNCTION(draw_rect);
	GBFW_API_FUNCTION(draw_box);

	GBFW_API_VARIABLE(font_default);
	GBFW_API_VARIABLE(font_small);
	GBFW_API_FUNCTION(font_draw);
	GBFW_API_FUNCTION(font_draw_border);
} gbfw_api_t;

extern const gbfw_api_t *gbfw_api;

#ifndef FIRMWARE
static inline void memcpy(void * restrict dst, const void * restrict src, size_t size)
{
	return gbfw_api->memcpy(dst, src, size);
}

static inline void memset(void *dst, int value, size_t size)
{
	return gbfw_api->memset(dst, value, size);
}

#define gbfw_tick_get                    gbfw_api->tick_get
#define gbfw_delay_us                    gbfw_api->delay_us
#define gbfw_random_u32                  gbfw_api->random_u32
#define gbfw_status_set                  gbfw_api->status_set
#define gbfw_power_shutdown              gbfw_api->power_shutdown
#define gbfw_settings_pointer            gbfw_api->settings_pointer
#define gbfw_exec_file                   gbfw_api->exec_file
#define gbfw_display_backlight_set       gbfw_api->display_backlight_set
#define gbfw_display_render_region       gbfw_api->display_render_region
#define gbfw_display_sync                gbfw_api->display_sync
#define gbfw_audio_volume_set            gbfw_api->audio_volume_set
#define gbfw_audio_volume_mute           gbfw_api->audio_volume_mute
#define gbfw_audio_rate_set              gbfw_api->audio_rate_set
#define gbfw_audio_start                 gbfw_api->audio_start
#define gbfw_audio_stop                  gbfw_api->audio_stop
#define gbfw_audio_continue              gbfw_api->audio_continue
#define gbfw_control_poll                gbfw_api->control_poll
#define gbfw_home                        gbfw_api->home
#define gbfw_battery_is_charging         gbfw_api->battery_is_charging
#define gbfw_battery_level               gbfw_api->battery_level
#define gbfw_analog_temperature          gbfw_api->analog_temperature
#define gbfw_usb_device_register         gbfw_api->usb_device_register
#define gbfw_usb_endpoint_stall          gbfw_api->usb_endpoint_stall
#define gbfw_usb_endpoint_set_packet     gbfw_api->usb_endpoint_set_packet
#define gbfw_usb_mass_storage_enable     gbfw_api->usb_mass_storage_enable
#define gbfw_usb_mass_storage_disable    gbfw_api->usb_mass_storage_disable
#define gbfw_cordic_begin                gbfw_api->cordic_begin
#define gbfw_cordic_end                  gbfw_api->cordic_end
#define gbfw_cordic_process_simple       gbfw_api->cordic_process_simple
#define gbfw_usb_device_register         gbfw_api->usb_device_register
#define gbfw_usb_endpoint_stall          gbfw_api->usb_endpoint_stall
#define gbfw_usb_endpoint_set_packet     gbfw_api->usb_endpoint_set_packet
#define gbfw_draw_rect                   gbfw_api->draw_rect
#define gbfw_font_default                gbfw_api->font_default
#define gbfw_font_small                  gbfw_api->font_small
#define gbfw_font_draw                   gbfw_api->font_draw
#define gbfw_fs_start                    gbfw_api->fs_start
#define gbfw_fs_stop                     gbfw_api->fs_stop
#define gbfw_fs_dir_root                 gbfw_api->fs_dir_root
#define gbfw_fs_dir_find_by_name         gbfw_api->fs_dir_find_by_name
#define gbfw_fs_dir_entry_from_path      gbfw_api->fs_dir_entry_from_path
#define gbfw_fs_dir_entry_size           gbfw_api->fs_dir_entry_size
#define gbfw_fs_dir_entry_name           gbfw_api->fs_dir_entry_name
#define gbfw_fs_dir_entry_match_name     gbfw_api->fs_dir_entry_match_name
#define gbfw_fs_dir_entry_is_read_only   gbfw_api->fs_dir_entry_is_read_only
#define gbfw_fs_dir_entry_is_directory   gbfw_api->fs_dir_entry_is_directory
#define gbfw_fs_dir_entry_file           gbfw_api->fs_dir_entry_file
#define gbfw_fs_dir_entry_first          gbfw_api->fs_dir_entry_first
#define gbfw_fs_dir_entry_next           gbfw_api->fs_dir_entry_next
#define gbfw_fs_file_is_end              gbfw_api->fs_file_is_end
#define gbfw_fs_file_read                gbfw_api->fs_file_read
//#define gbfw_fs_file_write               gbfw_api->fs_file_write
#endif

#define gbfw_api_init(x) \
	do { \
	if ((api->version != GBFW_API_VERSION) \
		|| (api->count < GBFW_API_COUNT)) \
		return false; \
	gbfw_api = (x); \
	} while (0)

#endif
