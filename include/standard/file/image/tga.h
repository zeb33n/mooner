#ifndef TGA_H
#define TGA_H

#include <stdint.h>

typedef enum
{
	TGA_CLUT_TYPE_NONE    = 0,
	TGA_CLUT_TYPE_DEFAULT = 1,
} tga_clut_type_e;

typedef enum
{
	TGA_IMAGE_TYPE_NONE          =  0,
	TGA_IMAGE_TYPE_CLUT          =  1,
	TGA_IMAGE_TYPE_COLOR         =  2,
	TGA_IMAGE_TYPE_GRAYSCALE     =  3,
	TGA_IMAGE_TYPE_CLUT_RLE      =  9,
	TGA_IMAGE_TYPE_COLOR_RLE     = 10,
	TGA_IMAGE_TYPE_GRAYSCALE_RLE = 11,
} tga_image_type_e;

typedef enum
{
	TGA_INTERLEAVE_NONE     = 0,
	TGA_INTERLEAVE_TWO_WAY  = 1,
	TGA_INTERLEAVE_FOUR_WAY = 2,
} tga_interleave_e;

typedef struct
__attribute__((__packed__))
{
	uint8_t  id_length;
	uint8_t  clut_type;
	uint8_t  image_type;
	uint16_t clut_first;
	uint16_t clut_count;
	uint8_t  clut_bits;
	uint16_t origin_x;
	uint16_t origin_y;
	uint16_t width;
	uint16_t height;
	uint8_t  bpp;

	union
	{
		struct
		{
			uint8_t alpha_bits : 4;
			uint8_t hflip      : 1;
			uint8_t vflip      : 1;
			uint8_t interleave : 2;
		};

		uint8_t mask;
	} image_descriptor;
} tga_header_t;

#define TGA_FOOTER_SIGNATURE "TRUEVISION-XFILE.\0"

typedef struct
__attribute__((__packed__))
{
	uint32_t extension_offset;
	uint32_t developer_offset;
	const char signature[18];
} tga_footer_t;

typedef struct
__attribute__((__packed__))
{
	uint16_t hour, minute, second;
} tga_time_t;

typedef struct
__attribute__((__packed__))
{
	uint16_t month, day, year;
} tga_date_t;

typedef union
{
	struct
	{
		uint8_t alpha;
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	};

	uint32_t mask;
} tga_color_t;

typedef struct
__attribute__((__packed__))
{
	uint16_t numerator, denominator;
} tga_fract_t;

typedef enum
{
	TGA_ALPHA_NONE          = 0,
	TGA_ALPHA_UNDEF_IGNORE  = 1,
	TGA_ALPHA_UNDEF_RETAIN  = 2,
	TGA_ALPHA_DEFAULT       = 3,
	TGA_ALPHA_PREMULTIPLIED = 4,
} tga_alpha_e;

typedef struct
__attribute__((__packed__))
{
	uint16_t size;

	char author[41];
	char comment[324];

	struct
	__attribute__((__packed__))
	{
		tga_date_t date;
		tga_time_t time;
	} timestamp;

	struct
	__attribute__((__packed__))
	{
		char       id[41];
		tga_time_t time[6];
	} job;

	struct
	__attribute__((__packed__))
	{
		char     id[41];
		uint16_t version_number;
		char     version_letter;
	} software;

	tga_color_t key_color;
	tga_fract_t aspect_ratio;
	tga_fract_t gamma;
	uint32_t    color_correction_offset;
	uint32_t    thumbnail_offset;
	uint32_t    scanline_offset;
	uint8_t     alpha;
} tga_extension_t;

typedef struct
__attribute__((__packed__))
{
	uint16_t alpha, red, green, blue;
} tga_cct_entry_t;

typedef struct
__attribute__((__packed__))
{
	uint16_t id;
	uint32_t offset;
	uint32_t size;
} tga_tag_t;

#endif
