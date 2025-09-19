#ifndef WAV_H
#define WAV_H

#include <stdint.h>
#include <standard/file/riff.h>

#define WAV_HEADER_IDENT "WAVE"

#define WAV_CHUNK_FORMAT "fmt "
#define WAV_CHUNK_DATA   "data"
#define WAV_CHUNK_FACT   "fact"

typedef struct
__attribute__((__packed__))
{
	riff_t chunk;
	char   ident[4];
} wav_header_t;

typedef enum
{
	WAV_FORMAT_PCM        = 0x0001,
	WAV_FORMAT_FLOAT      = 0x0003,
	WAV_FORMAT_ALAW       = 0x0006,
	WAV_FORMAT_MULAW      = 0x0007,
	WAV_FORMAT_EXTENSIBLE = 0xFFFE,
} wav_format_e;

typedef struct
__attribute__((__packed__))
{
	riff_t   chunk;

	uint16_t format;
	uint16_t channels;
	uint32_t rate;
	uint32_t bytes_per_second;
	uint16_t block_align;
	uint16_t bits_per_sample;

	uint16_t extension_size;
	uint16_t valid_bits_per_sample;
	uint32_t speaker_position_mask;
	uint8_t  subformat[16];
} wav_chunk_format_t;

typedef struct
__attribute__((__packed__))
{
	riff_t   chunk;
	uint32_t samples;
} wav_chunk_fact_t;

#endif
