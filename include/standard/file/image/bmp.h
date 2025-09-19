#ifndef BMP_H
#define BMP_H

#include <stdint.h>

#define BMP_HEADER_TYPE_BITMAP        "BM"
#define BMP_HEADER_TYPE_BITMAP_ARRAY  "BA"
#define BMP_HEADER_TYPE_COLOR_ICON    "CI"
#define BMP_HEADER_TYPE_COLOR_POINTER "CP"
#define BMP_HEADER_TYPE_STRUCT_ICON   "IC"
#define BMP_HEADER_TYPE_POINTER       "PT"

typedef struct
__attribute__((__packed__))
{
	char     id[2];
	uint32_t size;
	uint16_t reserved[2];
	uint32_t offset;
} bmp_header_t;

typedef struct
__attribute__((__packed__))
{
	uint32_t size;
	int16_t  width;
	int16_t  height;
	int16_t  planes;
	int16_t  bpp;
} bmp_core_header_t;

typedef struct
__attribute__((__packed__))
{
	uint32_t size;
	uint16_t width;
	uint16_t height;
	uint16_t planes;
	uint16_t bpp;
} bmp_os2_core_header_v1_t;

typedef enum
{
	BMP_COMPRESSION_NONE           =  0,
	BMP_COMPRESSION_RLE8           =  1,
	BMP_COMPRESSION_RLE4           =  2,
	BMP_COMPRESSION_BITFIELDS      =  3,
	BMP_COMPRESSION_OS2_HUFFMAN_1D =  3,
	BMP_COMPRESSION_JPEG           =  4,
	BMP_COMPRESSION_OS2_RLE24      =  4,
	BMP_COMPRESSION_PNG            =  5,
	BMP_COMPRESSION_ALPHABITFIELDS =  6,
	BMP_COMPRESSION_CMYK           = 11,
	BMP_COMPRESSION_CMYK_RLE8      = 12,
	BMP_COMPRESSION_CMYK_RLE4      = 13,
} bmp_compression_e;

typedef struct
__attribute__((__packed__))
{
	uint32_t size;
	int32_t  width;
	int32_t  height;
	int16_t  planes;
	int16_t  bpp;
	uint32_t compression;
	uint32_t bitmap_size;
	int32_t  hres_ppm;
	int32_t  vres_ppm;
	uint32_t clut_count;
	uint32_t clut_important;
} bmp_info_header_t;

typedef enum
{
	BMP_RES_UNIT_PPM = 0,
} bmp_res_unit_e;

typedef enum
{
	BMP_HALFTONE_NONE            = 0,
	BMP_HALFTONE_ERROR_DIFFUSION = 1,
	BMP_HALFTONE_PANDA           = 2,
	BMP_HALFTONE_SUPER_CIRCLE    = 3,
} bmp_halftone_e;

typedef enum
{
	BMP_CLUT_FORMAT_RGB = 0,
} bmp_clut_format_e;

typedef struct
__attribute__((__packed__))
{
	uint32_t size;
	int32_t  width;
	int32_t  height;
	int16_t  planes;
	int16_t  bpp;
	uint32_t compression;
	uint32_t bitmap_size;
	int32_t  hres;
	int32_t  vres;
	uint32_t clut_count;
	uint32_t clut_important;
	uint16_t res_unit;
	uint16_t halftone;
	uint32_t halftone_param1;
	uint32_t halftone_param2;
	uint32_t clut_format;
	uint32_t application_identifier;
} bmp_os2_core_header_v2_t;

typedef struct
__attribute__((__packed__))
{
	uint32_t size;
	int32_t  width;
	int32_t  height;
	int16_t  planes;
	int16_t  bpp;
	uint32_t compression;
	uint32_t bitmap_size;
	int32_t  hres_ppm;
	int32_t  vres_ppm;
	uint32_t clut_count;
	uint32_t clut_important;
	uint32_t red_mask;
	uint32_t green_mask;
	uint32_t blue_mask;
} bmp_info_header_v2_t;

typedef struct
__attribute__((__packed__))
{
	uint32_t          size;
	int32_t           width;
	int32_t           height;
	int16_t           planes;
	int16_t           bpp;
	uint32_t          compression;
	uint32_t          bitmap_size;
	int32_t           hres_ppm;
	int32_t           vres_ppm;
	uint32_t          clut_count;
	uint32_t          clut_important;
	uint32_t          red_mask;
	uint32_t          green_mask;
	uint32_t          blue_mask;
	uint32_t          alpha_mask;
} bmp_info_header_v3_t;

typedef enum
{
	BMP_COLORSPACE_CALIBRATED_RGB = 0x00000000,
	BMP_COLORSPACE_SRGB           = 0x73524742,
	BMP_COLORSPACE_OS             = 0x57696E20,
} bmp_colorspace_e;

typedef struct
{
	uint32_t x, y, z;
} bmp_cie_xyz_t;

typedef struct
{
	bmp_cie_xyz_t r, g, b;
} bmp_cie_xyz_rgb_t;

typedef struct
__attribute__((__packed__))
{
	uint32_t          size;
	int32_t           width;
	int32_t           height;
	int16_t           planes;
	int16_t           bpp;
	uint32_t          compression;
	uint32_t          bitmap_size;
	int32_t           hres_ppm;
	int32_t           vres_ppm;
	uint32_t          clut_count;
	uint32_t          clut_important;
	uint32_t          red_mask;
	uint32_t          green_mask;
	uint32_t          blue_mask;
	uint32_t          alpha_mask;
	uint32_t          colorspace;
	bmp_cie_xyz_rgb_t endpoints;
	uint32_t          gamma_red;
	uint32_t          gamma_green;
	uint32_t          gamma_blue;
} bmp_info_header_v4_t;

typedef enum
{
	BMP_INTENT_BUSINESS         = 1,
	BMP_INTENT_GRAPHICS         = 2,
	BMP_INTENT_IMAGES           = 4,
	BMP_INTENT_ABS_COLORIMETRIC = 8,
} bmp_intent_e;

typedef struct
__attribute__((__packed__))
{
	uint32_t          size;
	int32_t           width;
	int32_t           height;
	int16_t           planes;
	int16_t           bpp;
	uint32_t          compression;
	uint32_t          bitmap_size;
	int32_t           hres_ppm;
	int32_t           vres_ppm;
	uint32_t          clut_count;
	uint32_t          clut_important;
	uint32_t          red_mask;
	uint32_t          green_mask;
	uint32_t          blue_mask;
	uint32_t          alpha_mask;
	uint32_t          colorspace;
	bmp_cie_xyz_rgb_t endpoints;
	uint32_t          gamma_red;
	uint32_t          gamma_green;
	uint32_t          gamma_blue;
	uint32_t          intent;
	uint32_t          profile_data;
	uint32_t          profile_size;
	uint32_t          reserved;
} bmp_info_header_v5_t;

typedef enum
{
	bmp_core_header_t    core;
	bmp_info_header_t    info_v1;
	bmp_info_header_v2_t info_v2;
	bmp_info_header_v3_t info_v3;
	bmp_info_header_v4_t info_v4;
	bmp_info_header_v5_t info_v5;

	bmp_os2_core_header_v1_t os2_core_v1;
	bmp_os2_core_header_v2_t os2_core_v2;
} bmp_dib_header_t;

#endif
