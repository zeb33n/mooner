#ifndef GBFW_AUDIO_H
#define GBFW_AUDIO_H

#include <stdint.h>
#include <stdbool.h>

void gbfw_audio_volume_set(uint8_t num, uint8_t den);
void gbfw_audio_volume_mute(bool mute);
void gbfw_audio_rate_set(uint16_t rate);

void gbfw_audio_start(
	int16_t *buff, uint16_t size,
	void (*callback)(int16_t *buff, uint16_t size));
void gbfw_audio_stop(void);
void gbfw_audio_continue(void);

#endif
