#include <gbfw/api.h>
#include <gbfw/tick.h>
#include <gbfw/delay.h>
#include <gbfw/random.h>
#include <gbfw/memcpy.h>
#include <gbfw/status.h>
#include <gbfw/power.h>
#include <gbfw/settings.h>
#include <gbfw/load.h>
#include <gbfw/display.h>
#include <gbfw/audio.h>
#include <gbfw/control.h>
#include <gbfw/battery.h>
#include <gbfw/analog.h>
#include <gbfw/usb.h>
#include <gbfw/usb/mass_storage.h>
#include <gbfw/cordic.h>
#include <gbfw/draw.h>
#include <gbfw/font.h>


extern const gbfw_font_t font_default;
extern const gbfw_font_t font_small;


#define GBFW_API_SET_PREFIX(name, prefix) .name = prefix##name
#define GBFW_API_LINK(name) GBFW_API_SET_PREFIX(name, gbfw_)


static const gbfw_api_t gbfw_api__table =
{
	.version = GBFW_API_VERSION,
	.count   = GBFW_API_COUNT,

	GBFW_API_LINK(tick_get),

	GBFW_API_LINK(delay_us),

	GBFW_API_LINK(random_u32),

	.memcpy = memcpy,
	.memset = memset,

	GBFW_API_LINK(status_set),

	GBFW_API_LINK(power_shutdown),

	GBFW_API_LINK(settings_pointer),

	GBFW_API_LINK(exec_file),

	GBFW_API_LINK(display_backlight_set),
	GBFW_API_LINK(display_render_region),
	GBFW_API_LINK(display_sync),

	GBFW_API_LINK(audio_volume_set),
	GBFW_API_LINK(audio_volume_mute),
	GBFW_API_LINK(audio_rate_set),
	GBFW_API_LINK(audio_start),
	GBFW_API_LINK(audio_stop),
	GBFW_API_LINK(audio_continue),

	GBFW_API_LINK(control_poll),

	GBFW_API_LINK(home),

	GBFW_API_LINK(battery_is_charging),
	GBFW_API_LINK(battery_level),

	GBFW_API_LINK(analog_temperature),

	GBFW_API_LINK(usb_device_register),
	GBFW_API_LINK(usb_endpoint_stall),
	GBFW_API_LINK(usb_endpoint_set_packet),

	GBFW_API_LINK(usb_mass_storage_enable),
	GBFW_API_LINK(usb_mass_storage_disable),

	GBFW_API_LINK(cordic_begin),
	GBFW_API_LINK(cordic_end),
	GBFW_API_LINK(cordic_process_simple),

	GBFW_API_LINK(fs_start),
	GBFW_API_LINK(fs_stop),
	GBFW_API_LINK(fs_dir_root),
	GBFW_API_LINK(fs_dir_find_by_name),
	GBFW_API_LINK(fs_dir_entry_from_path),
	GBFW_API_LINK(fs_dir_entry_size),
	GBFW_API_LINK(fs_dir_entry_name),
	GBFW_API_LINK(fs_dir_entry_match_name),
	GBFW_API_LINK(fs_dir_entry_is_read_only),
	GBFW_API_LINK(fs_dir_entry_is_directory),
	GBFW_API_LINK(fs_dir_entry_file),
	GBFW_API_LINK(fs_dir_entry_first),
	GBFW_API_LINK(fs_dir_entry_next),
	GBFW_API_LINK(fs_file_is_end),
	GBFW_API_LINK(fs_file_read),
	//GBFW_API_LINK(fs_file_write),

	GBFW_API_LINK(draw_rect),
	GBFW_API_LINK(draw_box),

	.font_default = &font_default,
	.font_small   = &font_small,
	GBFW_API_LINK(font_draw),
	GBFW_API_LINK(font_draw_border),
};

const gbfw_api_t *gbfw_api = &gbfw_api__table;
