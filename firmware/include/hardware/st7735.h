#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>

typedef enum
{
	ST7735_CMD_NOP       = 0x00,
	ST7735_CMD_SWRESET   = 0x01,
	ST7735_CMD_RDDID     = 0x04,
	ST7735_CMD_RDDST     = 0x09,
	ST7735_CMD_RDDPM     = 0x0A,
	ST7735_CMD_RDDMADCTL = 0x0B,
	ST7735_CMD_RDDCOLMOD = 0x0C,
	ST7735_CMD_RDDIM     = 0x0D,
	ST7735_CMD_RDDSM     = 0x0E,
	ST7735_CMD_RDDSDR    = 0x0F,
	ST7735_CMD_SLPIN     = 0x10,
	ST7735_CMD_SLPOUT    = 0x11,
	ST7735_CMD_PTLON     = 0x12,
	ST7735_CMD_NORON     = 0x13,
	ST7735_CMD_INVOFF    = 0x20,
	ST7735_CMD_INVON     = 0x21,
	ST7735_CMD_GAMSET    = 0x26,
	ST7735_CMD_DISPOFF   = 0x28,
	ST7735_CMD_DISPON    = 0x29,
	ST7735_CMD_CASET     = 0x2A,
	ST7735_CMD_RASET     = 0x2B,
	ST7735_CMD_RAMWR     = 0x2C,
	ST7735_CMD_RGBSET    = 0x2D,
	ST7735_CMD_RAMRD     = 0x2E,
	ST7735_CMD_PTLAR     = 0x30,
	ST7735_CMD_SCRLAR    = 0x33,
	ST7735_CMD_TEOFF     = 0x34,
	ST7735_CMD_TEON      = 0x35,
	ST7735_CMD_MADCTL    = 0x36,
	ST7735_CMD_VSCSAD    = 0x37,
	ST7735_CMD_IDMOFF    = 0x38,
	ST7735_CMD_IDMON     = 0x39,
	ST7735_CMD_COLMOD    = 0x3A,
	ST7735_CMD_FRMCTR1   = 0xB1,
	ST7735_CMD_FRMCTR2   = 0xB2,
	ST7735_CMD_FRMCTR3   = 0xB3,
	ST7735_CMD_INVCTR    = 0xB4,
	ST7735_CMD_PWRCTR1   = 0xC0,
	ST7735_CMD_PWRCTR2   = 0xC1,
	ST7735_CMD_PWRCTR3   = 0xC2,
	ST7735_CMD_PWRCTR4   = 0xC3,
	ST7735_CMD_PWRCTR5   = 0xC4,
	ST7735_CMD_VMCTR1    = 0xC5,
	ST7735_CMD_VMOFCTR   = 0xC7,
	ST7735_CMD_WRID2     = 0xD1,
	ST7735_CMD_WRID3     = 0xD2,
	ST7735_CMD_NVCTR1    = 0xD9,
	ST7735_CMD_RDID1     = 0xDA,
	ST7735_CMD_RDID2     = 0xDB,
	ST7735_CMD_RDID3     = 0xDC,
	ST7735_CMD_NVCTR2    = 0xDE,
	ST7735_CMD_NVCTR3    = 0xDF,
	ST7735_CMD_GAMCTRP1  = 0xE0,
	ST7735_CMD_GAMCTRN1  = 0xE1,
	ST7735_CMD_GCV       = 0xFC,
} st7735_cmd_e;

typedef union
{
	struct
	{
		uint8_t d     : 2;
		uint8_t dison : 1;
		uint8_t noron : 1;
		uint8_t slpon : 1;
		uint8_t ptlon : 1;
		uint8_t idmon : 1;
		uint8_t bston : 1;
	};

	uint8_t mask;
} st7735_pm_t;


typedef enum
{
	ST7735_MADCTL_LEFT_TO_RIGHT = 0,
	ST7735_MADCTL_RIGHT_TO_LEFT = 1,
} st7735_madctl_mxh_e;

typedef enum
{
	ST7735_MADCTL_TOP_TO_BOTTOM = 0,
	ST7735_MADCTL_BOTTOM_TO_TOP = 1,
} st7735_madctl_myl_e;

typedef enum
{
	ST7735_MADCTL_NORMAL = 0,
	ST7735_MADCTL_ROTATE = 1,
} st7735_madctl_rotate_e;

typedef enum
{
	ST7735_MADCTL_RGB = 0,
	ST7735_MADCTL_BGR = 1,
} st7735_madctl_order_e;

typedef union
{
	struct
	{
		uint8_t d   : 2;
		uint8_t mh  : 1;
		uint8_t rgb : 1;
		uint8_t ml  : 1;
		uint8_t mv  : 1;
		uint8_t mx  : 1;
		uint8_t my  : 1;
	};

	uint8_t mask;
} st7735_madctl_t;


typedef enum
{
	ST7735_COLMOD_RGB444 = 3,
	ST7735_COLMOD_RGB565 = 5,
	ST7735_COLMOD_RGB666 = 6,
} st7735_colmod_e;

typedef union
{
	struct
	{
		uint8_t ifpf : 3;
		uint8_t dc   : 1;
		uint8_t zero : 4;
	};

	uint8_t mask;
} st7735_colmod_t;


typedef union
{
	struct
	{
		uint8_t gcs   : 3;
		uint8_t d4_3  : 2;
		uint8_t invon : 1;
		uint8_t d6    : 1;
		uint8_t vsson : 1;
	};

	uint8_t mask;
} st7735_im_t;


typedef union
{
	struct
	{
		uint8_t d5_0 : 6;
		uint8_t teon : 1;
		uint8_t tem  : 1;
	};

	uint8_t mask;
} st7735_sm_t;


typedef union
{
	struct
	{
		uint8_t d    : 4;
		uint8_t brd  : 1;
		uint8_t attd : 1;
		uint8_t fund : 1;
		uint8_t reld : 1;
	};

	uint8_t mask;
} st7735_sdr_t;

#endif
