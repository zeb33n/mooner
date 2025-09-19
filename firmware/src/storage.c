#include "storage.h"
#include "task.h"
#include "power.h"
#include "dma.h"
#include "irq.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/gpio.h>
#include <hardware/stm32/spi.h>
#include <hardware/stm32/dma.h>
#include <hardware/stm32/dmamux.h>
#include <hardware/stm32/irq.h>
#include <hardware/stm32/crc.h>
#include <hardware/cm4/nvic.h>
#include <hardware/cm4/intrinsics.h>
#include <standard/mmc.h>
#include <util/endian.h>
#include <stddef.h>


#define GBFW_STORAGE_SPI               stm32_spi3
#define GBFW_STORAGE_SPI_SPEED         2

#define GBFW_STORAGE_RX_DMA            stm32_dma1
#define GBFW_STORAGE_RX_DMA_CHANNEL    2
#define GBFW_STORAGE_RX_DMA_IRQ        STM32_IRQ_DMA1_CH3
#define GBFW_STORAGE_RX_DMA_IRQ_VECTOR stm32_vector_irq_dma1_ch3

#define GBFW_STORAGE_TX_DMA            stm32_dma1
#define GBFW_STORAGE_TX_DMA_CHANNEL    3
#define GBFW_STORAGE_TX_DMA_IRQ        STM32_IRQ_DMA1_CH4
#define GBFW_STORAGE_TX_DMA_IRQ_VECTOR stm32_vector_irq_dma1_ch4

#define GBFW_STORAGE_NSS_PORT  STM32_GPIO_PORT_A
#define GBFW_STORAGE_NSS_PIN   15

#define GBFW_STORAGE_SCK_PORT  STM32_GPIO_PORT_B
#define GBFW_STORAGE_SCK_PIN   3
#define GBFW_STORAGE_SCK_AF    6

#define GBFW_STORAGE_MISO_PORT STM32_GPIO_PORT_B
#define GBFW_STORAGE_MISO_PIN  4
#define GBFW_STORAGE_MISO_AF   6

#define GBFW_STORAGE_MOSI_PORT STM32_GPIO_PORT_B
#define GBFW_STORAGE_MOSI_PIN  5
#define GBFW_STORAGE_MOSI_AF   6


typedef enum
{
	GBFW_STORAGE_TYPE_DEFAULT = 0,
	GBFW_STORAGE_TYPE_MMC     = 1,
	GBFW_STORAGE_TYPE_V2      = 2,
} gbfw_storage_type_e;

typedef union
{
	struct
	{
		uint8_t state      : 5;
		uint8_t type       : 2;
		uint8_t block_addr : 1;
	};

	uint8_t mask;
} gbfw_storage__flags_t;

static gbfw_storage__flags_t gbfw_storage__flags  = { .mask = 0x00 };
static uint16_t              gbfw_storage__multiple_count = 0;
static uint32_t              gbfw_storage__blocks = 0;



static inline void gbfw_storage_spi_init(void)
{
	stm32_rcc->apb1en1.spi3 = true;

	stm32_gpio_pin_af_set(GBFW_STORAGE_SCK_PORT , GBFW_STORAGE_SCK_PIN , GBFW_STORAGE_SCK_AF );
	stm32_gpio_pin_af_set(GBFW_STORAGE_MISO_PORT, GBFW_STORAGE_MISO_PIN, GBFW_STORAGE_MISO_AF);
	stm32_gpio_pin_af_set(GBFW_STORAGE_MOSI_PORT, GBFW_STORAGE_MOSI_PIN, GBFW_STORAGE_MOSI_AF);

	stm32_gpio_pin_set(GBFW_STORAGE_NSS_PORT , GBFW_STORAGE_NSS_PIN );
	stm32_gpio_pin_set(GBFW_STORAGE_MOSI_PORT, GBFW_STORAGE_MOSI_PIN);
	stm32_gpio_pin_pull_set(GBFW_STORAGE_MISO_PORT, GBFW_STORAGE_MISO_PIN, STM32_GPIO_PULL_UP);

	stm32_gpio_pin_mode_set(GBFW_STORAGE_NSS_PORT , GBFW_STORAGE_NSS_PIN , STM32_GPIO_MODE_OUTPUT);
	stm32_gpio_pin_mode_set(GBFW_STORAGE_SCK_PORT , GBFW_STORAGE_SCK_PIN , STM32_GPIO_MODE_ALTERNATE);
	stm32_gpio_pin_mode_set(GBFW_STORAGE_MISO_PORT, GBFW_STORAGE_MISO_PIN, STM32_GPIO_MODE_ALTERNATE);
	stm32_gpio_pin_mode_set(GBFW_STORAGE_MOSI_PORT, GBFW_STORAGE_MOSI_PIN, STM32_GPIO_MODE_ALTERNATE);

	stm32_gpio_pin_lock(GBFW_STORAGE_MISO_PORT, GBFW_STORAGE_MISO_PIN);

	stm32_spi_c1_t c1 = { .mask = GBFW_STORAGE_SPI->c1.mask };
	c1.mstr     = true;
	c1.ssm      = true;
	c1.ssi      = true;
	c1.crcl     = 1;
	c1.br       = GBFW_STORAGE_SPI_SPEED;
	GBFW_STORAGE_SPI->c1.mask = c1.mask;

	stm32_spi_c2_t c2 = { .mask = GBFW_STORAGE_SPI->c2.mask };
	c2.txeie  = true;
	c2.rxneie = true;
	c2.frxth  = true;
	GBFW_STORAGE_SPI->c2.mask = c2.mask;

	GBFW_STORAGE_SPI->crcp = 0x1021;

	stm32_dmamux->cc[GBFW_STORAGE_RX_DMA_CHANNEL].dmareq_id = STM32_DMAMUX_REQ_SPI3_RX;
	stm32_dmamux->cc[GBFW_STORAGE_TX_DMA_CHANNEL].dmareq_id = STM32_DMAMUX_REQ_SPI3_TX;

	stm32_dma_cc_t cc;

	cc.mask = 0;
	cc.tcie    = true;
	cc.htie    = false;
	cc.dir     = STM32_DMA_DIR_READ_PERIPH;
	cc.circ    = false;
	cc.pinc    = false;
	cc.pl      = 2;
	cc.mem2mem = false;
	GBFW_STORAGE_RX_DMA->c[GBFW_STORAGE_RX_DMA_CHANNEL].cc.mask = cc.mask;

	cc.mask = 0;
	cc.tcie    = true;
	cc.htie    = false;
	cc.dir     = STM32_DMA_DIR_READ_MEMORY;
	cc.circ    = false;
	cc.pinc    = false;
	cc.pl      = 2;
	cc.mem2mem = false;
	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cc.mask = cc.mask;

	GBFW_STORAGE_RX_DMA->c[GBFW_STORAGE_RX_DMA_CHANNEL].cpa = (uint32_t)&GBFW_STORAGE_SPI->d;
	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cpa = (uint32_t)&GBFW_STORAGE_SPI->d;

	cm4_nvic_interrupt_priority_set(GBFW_STORAGE_RX_DMA_IRQ, GBFW_IRQ_PRIORITY_STORAGE);
	cm4_nvic_interrupt_priority_set(GBFW_STORAGE_TX_DMA_IRQ, GBFW_IRQ_PRIORITY_STORAGE);
	cm4_nvic_interrupt_enable(GBFW_STORAGE_RX_DMA_IRQ);
	cm4_nvic_interrupt_enable(GBFW_STORAGE_TX_DMA_IRQ);

	GBFW_STORAGE_SPI->c1.spe = true;
}

static void gbfw_storage_spi_wait_dma_idle(void)
{
	wfi_while(GBFW_STORAGE_SPI->c2.txdmaen);
}

static void gbfw_storage_spi_wait_idle(void)
{
	gbfw_storage_spi_wait_dma_idle();

	while (GBFW_STORAGE_SPI->s.bsy);
}

static void gbfw_storage_spi_select(bool select)
{
	gbfw_storage_spi_wait_idle();
	stm32_gpio_pin_write(GBFW_STORAGE_NSS_PORT, GBFW_STORAGE_NSS_PIN, !select);
}

static void gbfw_storage_spi_xact(size_t size, void *data)
{
	gbfw_storage_spi_wait_dma_idle();

	uint8_t       *ptr = (uint8_t *)data;
	const uint8_t *end = &ptr[size];
	while ((uintptr_t)ptr < (uintptr_t)end)
	{
		while (!GBFW_STORAGE_SPI->s.txe);
		*((volatile uint8_t *)&GBFW_STORAGE_SPI->d) = *ptr;

		while (!GBFW_STORAGE_SPI->s.rxne);
		*ptr = *((volatile uint8_t *)&GBFW_STORAGE_SPI->d);

		ptr++;
	}
}

void __attribute__((interrupt)) GBFW_STORAGE_RX_DMA_IRQ_VECTOR(void)
{
	GBFW_STORAGE_RX_DMA->c[GBFW_STORAGE_RX_DMA_CHANNEL].cc.en = false;
	stm32_dma_interrupt_clear(
		GBFW_STORAGE_RX_DMA, GBFW_STORAGE_RX_DMA_CHANNEL,
		STM32_DMA_INTERRUPT_TRANSFER_COMPLETE);

	// STM32 Errata 2.1.3
	dsb();
}

void __attribute__((interrupt)) GBFW_STORAGE_TX_DMA_IRQ_VECTOR(void)
{
	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cc.en = false;
	stm32_dma_interrupt_clear(
		GBFW_STORAGE_TX_DMA, GBFW_STORAGE_TX_DMA_CHANNEL,
		STM32_DMA_INTERRUPT_TRANSFER_COMPLETE);

	// STM32 Errata 2.1.3
	dsb();
}

static inline void gbfw_storage_spi_block_crc_write_begin(uint16_t size, const void *data)
{
	gbfw_storage_spi_wait_idle();

	GBFW_STORAGE_SPI->c1.spe = false;

	stm32_spi_c1_t c1;
	c1.mask = GBFW_STORAGE_SPI->c1.mask;
	c1.bidimode = true;
	c1.bidioe   = true;
	c1.crcen    = true;
	GBFW_STORAGE_SPI->c1.mask = c1.mask;

	stm32_dma_size_e dma_size = STM32_DMA_SIZE_BYTE;
	if ((((uintptr_t)data | size) & 1) == 0)
	{
		dma_size = STM32_DMA_SIZE_HALF;
		size /= 2;

		GBFW_STORAGE_SPI->c2.frxth = false;
	}

	GBFW_STORAGE_SPI->c2.txdmaen = true;

	stm32_dma_cc_t cc = { .mask = GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cc.mask };
	cc.psize = dma_size;
	cc.msize = dma_size;
	cc.minc  = (data != NULL);
	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cc.mask = cc.mask;

	uint16_t dummy = 0xFFFF;
	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cma  = gbfw_dma_pointer(data ? data : &dummy);
	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cndt = size;

	GBFW_STORAGE_SPI->c1.spe = true;

	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cc.en = true;
}

static inline void gbfw_storage_spi_block_crc_write_end(void)
{
	wfi_while(GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cc.en);

	while (GBFW_STORAGE_SPI->s.bsy);

	GBFW_STORAGE_SPI->c1.spe = false;

	GBFW_STORAGE_SPI->c2.txdmaen = false;
	GBFW_STORAGE_SPI->c2.frxth   = true;

	stm32_spi_c1_t c1;
	c1.mask = GBFW_STORAGE_SPI->c1.mask;
	c1.bidimode = false;
	c1.bidioe   = false;
	c1.crcen    = false;
	GBFW_STORAGE_SPI->c1.mask = c1.mask;

	GBFW_STORAGE_SPI->c1.spe = true;
}

static void gbfw_storage_spi_block_crc_read_begin(uint16_t size, void *data)
{
	gbfw_storage_spi_wait_idle();

	GBFW_STORAGE_SPI->c1.spe = false;

	GBFW_STORAGE_SPI->c1.crcen = true;

	stm32_dma_size_e dma_size = STM32_DMA_SIZE_BYTE;
	if ((((uintptr_t)data | size) & 1) == 0)
	{
		dma_size = STM32_DMA_SIZE_HALF;
		size /= 2;

		GBFW_STORAGE_SPI->c2.frxth = false;
	}

	GBFW_STORAGE_SPI->c2.rxdmaen = true;
	GBFW_STORAGE_SPI->c2.txdmaen = true;

	stm32_dma_cc_t txcc = { .mask = GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cc.mask };
	txcc.psize = dma_size;
	txcc.msize = dma_size;
	txcc.minc = false;
	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cc.mask = txcc.mask;

	uint16_t tx_dummy = 0xFFFF;
	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cma  = gbfw_dma_pointer(&tx_dummy);
	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cndt = size;

	stm32_dma_cc_t rxcc = { .mask = GBFW_STORAGE_RX_DMA->c[GBFW_STORAGE_RX_DMA_CHANNEL].cc.mask };
	rxcc.psize = dma_size;
	rxcc.msize = dma_size;
	rxcc.minc = (data != NULL);
	GBFW_STORAGE_RX_DMA->c[GBFW_STORAGE_RX_DMA_CHANNEL].cc.mask = rxcc.mask;

	uint16_t rx_dummy = 0;
	GBFW_STORAGE_RX_DMA->c[GBFW_STORAGE_RX_DMA_CHANNEL].cma  = gbfw_dma_pointer(data ? data : &rx_dummy);
	GBFW_STORAGE_RX_DMA->c[GBFW_STORAGE_RX_DMA_CHANNEL].cndt = size;
	GBFW_STORAGE_RX_DMA->c[GBFW_STORAGE_RX_DMA_CHANNEL].cc.en = true;

	GBFW_STORAGE_SPI->c1.spe = true;

	GBFW_STORAGE_TX_DMA->c[GBFW_STORAGE_TX_DMA_CHANNEL].cc.en = true;
}

static bool gbfw_storage_spi_block_crc_read_end(void)
{
	wfi_while(GBFW_STORAGE_RX_DMA->c[GBFW_STORAGE_RX_DMA_CHANNEL].cc.en);

	while (GBFW_STORAGE_SPI->s.bsy);

	bool error = GBFW_STORAGE_SPI->s.crcerr;
	GBFW_STORAGE_SPI->s.crcerr = 0;

	while (GBFW_STORAGE_SPI->s.frlvl > 0)
		*((volatile uint8_t *)&GBFW_STORAGE_SPI->d);

	GBFW_STORAGE_SPI->c1.spe = false;

	GBFW_STORAGE_SPI->c1.crcen = false;

	GBFW_STORAGE_SPI->c2.rxdmaen = false;
	GBFW_STORAGE_SPI->c2.txdmaen = false;
	GBFW_STORAGE_SPI->c2.frxth   = true;

	GBFW_STORAGE_SPI->c1.spe = true;

	return !error;
}



static uint8_t crc7_calculate(const void *ptr, size_t size)
{
	stm32_crc->c.reset = true;

	volatile uint8_t * const crcd = (volatile uint8_t *)&stm32_crc->d;

	const uint8_t *byte = (const uint8_t *)ptr;
	for (; size > 0; byte++, size--)
		*crcd = *byte;
	return *crcd;
}

static void mmc_cmd_frame_create(
	mmc_cmd_e cmd, uint32_t arg,
	mmc_cmd_frame_t *frame)
{
	frame->index    = 0x40 | cmd;
	frame->argument = ENDIAN_SWAP32(arg);
	frame->crc      = 0x01 | (crc7_calculate(frame, (sizeof(*frame) - 1)) << 1);
}

static void mmc_wait_idle(void)
{
	uint8_t r;
	do
	{
		r = 0xFF;
		gbfw_storage_spi_xact(sizeof(r), &r);
	} while (r != 0xFF);
}

static mmc_resp_r1_t mmc_cmd_send(uint8_t cmd, uint32_t arg, uint32_t *eresp)
{
	gbfw_storage_spi_select(true);

	mmc_resp_r1_t resp;
	do
	{
		mmc_cmd_frame_t frame;
		mmc_cmd_frame_create(cmd, arg, &frame);

		gbfw_storage_spi_xact(sizeof(frame), &frame);

		if (cmd == MMC_CMD_STOP_TRANSMISSION)
		{
			uint8_t dummy = 0xFF;
			gbfw_storage_spi_xact(sizeof(dummy), &dummy);
		}

		for (uint8_t i = 0; i <= 8; i++)
		{
			resp.mask = 0xFF;
			gbfw_storage_spi_xact(sizeof(resp), &resp);
			if (resp.start_bit == 0) break;
		}
	} while ((resp.start_bit == 0) && resp.crc_error);

	if (eresp)
	{
		uint32_t er = 0xFFFFFFFF;
		gbfw_storage_spi_xact(sizeof(er), &er);
		*eresp = ENDIAN_SWAP32(er);
	}

	return resp;
}

static bool mmc_cmd_read_begin(uint8_t cmd, uint32_t arg, void *data, size_t size)
{
	mmc_resp_r1_t resp = mmc_cmd_send(cmd, arg, NULL);
	if (resp.mask != 0x00) return false;

	uint8_t token;
	do
	{
		token = 0xFF;
		gbfw_storage_spi_xact(sizeof(token), &token);
	} while (token == MMC_DATA_TOKEN_NONE);

	if (token != MMC_DATA_TOKEN)
		return false;

	// Need to block the un-needed TX CRC as this could corrupt state.
	stm32_gpio_pin_mode_set(GBFW_STORAGE_MOSI_PORT, GBFW_STORAGE_MOSI_PIN, STM32_GPIO_MODE_OUTPUT);

	gbfw_storage_spi_block_crc_read_begin(size, data);
	return true;
}

static bool mmc_cmd_read_end(void)
{
	bool success = gbfw_storage_spi_block_crc_read_end();
	mmc_wait_idle();
	gbfw_storage_spi_select(false);
	stm32_gpio_pin_mode_set(GBFW_STORAGE_MOSI_PORT, GBFW_STORAGE_MOSI_PIN, STM32_GPIO_MODE_ALTERNATE);
	return success;
}

static mmc_resp_r1_t mmc_cmd_send_nodata(uint8_t cmd, uint32_t arg, uint32_t *eresp)
{
	mmc_resp_r1_t resp = mmc_cmd_send(cmd, arg, eresp);
	mmc_wait_idle();
	gbfw_storage_spi_select(false);
	return resp;
}

static mmc_resp_r1_t mmc_cmd_app_send(mmc_cmd_app_e cmd, uint32_t arg, uint32_t *eresp)
{
	mmc_resp_r1_t resp = mmc_cmd_send_nodata(MMC_CMD_APP_CMD, 0, NULL);
	if (resp.mask & 0x7C) return resp;
	return mmc_cmd_send_nodata(cmd, arg, eresp);
}

static void mmc_send_clockburst(void)
{
	uint8_t data[10] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	gbfw_storage_spi_xact(sizeof(data), data);
}

static bool mmc_cmd_read_ocr(mmc_ocr_t *ocr)
{
	mmc_resp_r1_t resp = mmc_cmd_send(MMC_CMD_READ_OCR, 0, NULL);
	if (resp.mask != 0x00) return false;

	mmc_ocr_t r3 = { .mask = 0xFFFFFFFF };
	gbfw_storage_spi_xact(sizeof(r3), &r3);
	ocr->mask = ENDIAN_SWAP32(r3.mask);

	mmc_wait_idle();
	gbfw_storage_spi_select(false);
	return true;
}


static bool gbfw_storage__init(void)
{
	mmc_resp_r1_t resp = { .mask = 0x00 };
	if (gbfw_storage__flags.type != GBFW_STORAGE_TYPE_MMC)
	{
		mmc_send_op_cond_t arg = { .mask = 0 };
		arg.hcs = (gbfw_storage__flags.type == GBFW_STORAGE_TYPE_V2);
		resp = mmc_cmd_app_send(MMC_CMD_APP_SEND_OP_COND, arg.mask, NULL);

		if ((resp.start_bit == 0) && resp.illegal_command)
		{
			gbfw_storage__flags.type = GBFW_STORAGE_TYPE_MMC;
		}
		else if ((gbfw_storage__flags.type == GBFW_STORAGE_TYPE_V2) && (resp.mask == 0x00))
		{
			mmc_ocr_t ocr;
			if (!mmc_cmd_read_ocr(&ocr))
			{
				gbfw_storage__flags.state = GBFW_STORAGE_STATE_ERROR;
			}
			else
			{
				gbfw_storage__flags.block_addr = ocr.ccs;
			}
		}
	}

	if (gbfw_storage__flags.type == GBFW_STORAGE_TYPE_MMC)
	{
		resp = mmc_cmd_send_nodata(MMC_CMD_SEND_OP_COND, 0, NULL);
	}

	return (resp.mask == 0x00);
}


static void gbfw_storage_init__finalize(void)
{
	if (!gbfw_storage__init())
	{
		if (gbfw_storage__flags.state == GBFW_STORAGE_STATE_ERROR)
			return;
		if (gbfw_storage__multiple_count++ >= 50)
		{
			gbfw_storage__flags.state = GBFW_STORAGE_STATE_ERROR;
			return;
		}

		gbfw_task_schedule(gbfw_storage_init__finalize, 10000);
		return;
	}

	mmc_csd_t csd;
	if (!mmc_cmd_read_begin(MMC_CMD_SEND_CSD, 0, &csd, sizeof(csd))
		|| !mmc_cmd_read_end())
	{
		gbfw_storage__flags.state = GBFW_STORAGE_STATE_ERROR;
		return;
	}

	uint64_t block_count;
	if ((gbfw_storage__flags.type != GBFW_STORAGE_TYPE_MMC)
		&& (csd.csd_structure == 1))
	{
		uint32_t c_size = csd.v2.c_size_7_0
			| (csd.v2.c_size_15_8 << 8)
			| (csd.v2.c_size_21_16 << 16);

		block_count = (c_size + 1U) << (19 - csd.read_bl_len);
	}
	else
	{
		uint32_t c_size = csd.c_size_1_0
			| (csd.c_size_9_2 << 2)
			| (csd.c_size_11_10 << 10);
		uint8_t c_size_mult = csd.c_size_mult_0
			| (csd.c_size_mult_2_1 << 1);

		block_count = (c_size + 1U) << (c_size_mult + 2);
	}

	if (csd.read_bl_len != GBFW_STORAGE_BLOCK_SIZE_LOG2)
	{
		mmc_resp_r1_t resp = mmc_cmd_send_nodata(
			MMC_CMD_SET_BLOCKLEN,
			GBFW_STORAGE_BLOCK_SIZE, NULL);
		if (resp.mask != 0x00)
		{
			gbfw_storage__flags.state = GBFW_STORAGE_STATE_ERROR;
			return;
		}

		block_count <<= (csd.read_bl_len - GBFW_STORAGE_BLOCK_SIZE_LOG2);
	}

	if ((block_count >> 32) != 0)
	{
		gbfw_storage__flags.state = GBFW_STORAGE_STATE_ERROR;
		return;
	}

	gbfw_storage__blocks = block_count;

	// Turn CRC checking on since we support in hardware anyway.
	if (gbfw_storage__flags.type != GBFW_STORAGE_TYPE_MMC)
		mmc_cmd_send_nodata(MMC_CMD_CRC_ON_OFF, 1, NULL);

	gbfw_storage__flags.state = GBFW_STORAGE_STATE_IDLE;
}

void gbfw_storage_init(void)
{
	stm32_rcc->ahb1en.crc = true;

	stm32_crc->c.polysize = STM32_CRC_POLYSIZE_7;
	stm32_crc->pol        = MMC_CRC_POLY;
	stm32_crc->init       = 0x00000000;

	gbfw_storage_spi_init();

	mmc_send_clockburst();

	mmc_resp_r1_t resp;

	resp = mmc_cmd_send_nodata(MMC_CMD_GO_IDLE_STATE, 0, NULL);
	if (resp.mask != 0x01)
	{
		gbfw_storage__flags.state = GBFW_STORAGE_STATE_NOT_PRESENT;
		return;
	}
	gbfw_storage__flags.state = GBFW_STORAGE_STATE_INITIALIZING;

	mmc_send_if_cond_t arg_send_if_cond =
	{
		.check_pattern   = 0xAA,
		.voltage         = 1,
		.reserved        = 0x0000,
		.command_version = 0x0,
	};
	resp = mmc_cmd_send_nodata(
		MMC_CMD_SEND_IF_COND,
		arg_send_if_cond.mask,
		&arg_send_if_cond.mask);
	if (!resp.illegal_command)
	{
		gbfw_storage__flags.type = GBFW_STORAGE_TYPE_V2;

		if ((arg_send_if_cond.check_pattern != 0xAA)
			|| (arg_send_if_cond.voltage != 1))
		{
			gbfw_storage__flags.state = GBFW_STORAGE_STATE_ERROR;
			return;
		}
	}

	gbfw_storage_init__finalize();
}


bool gbfw_storage_lock(void)
{
	wfi_while(gbfw_storage__flags.state == GBFW_STORAGE_STATE_INITIALIZING);

	bool success = false;
	cpsid(i);
	if (gbfw_storage__flags.state == GBFW_STORAGE_STATE_IDLE)
	{
		gbfw_storage__flags.state = GBFW_STORAGE_STATE_READY;
		success = true;
	}
	cpsie(i);

	return success;
}

void gbfw_storage_release(void)
{
	gbfw_storage_read_multiple_end();
	gbfw_storage_write_multiple_end();

	if (gbfw_storage__flags.state < GBFW_STORAGE_STATE_READY)
		return;

	gbfw_storage__flags.state = GBFW_STORAGE_STATE_IDLE;
}


gbfw_storage_state_e gbfw_storage_state(void)
{
	return gbfw_storage__flags.state;
}

uint32_t gbfw_storage_size(void)
{
	return gbfw_storage__blocks;
}


bool gbfw_storage_read_multiple_begin(uint32_t addr, uint16_t count)
{
	if (gbfw_storage__flags.state != GBFW_STORAGE_STATE_READY)
		return false;

	if ((gbfw_storage__flags.type == GBFW_STORAGE_TYPE_MMC)
		&& (count != 1))
	{
		mmc_resp_r1_t resp = mmc_cmd_send(
			MMC_CMD_SET_BLOCK_COUNT, count, NULL);
		if (resp.mask != 0x00) return false;
	}

	if (!gbfw_storage__flags.block_addr)
		addr <<= GBFW_STORAGE_BLOCK_SIZE_LOG2;

	mmc_resp_r1_t resp = mmc_cmd_send((count == 1
		? MMC_CMD_READ_SINGLE_BLOCK
		: MMC_CMD_READ_MULTIPLE_BLOCK), addr, NULL);
	if (resp.mask != 0x00) return false;

	// Need to block the un-needed TX CRC as this could corrupt state.
	stm32_gpio_pin_mode_set(GBFW_STORAGE_MOSI_PORT, GBFW_STORAGE_MOSI_PIN, STM32_GPIO_MODE_OUTPUT);

	gbfw_storage__flags.state = (count == 1
		? GBFW_STORAGE_STATE_READ_SINGLE
		: GBFW_STORAGE_STATE_READ_MULTIPLE);
	gbfw_storage__multiple_count = count;
	return true;
}

bool gbfw_storage_read_multiple_block_begin(uint8_t *block)
{
	if ((gbfw_storage__flags.state != GBFW_STORAGE_STATE_READ_SINGLE)
		&& (gbfw_storage__flags.state != GBFW_STORAGE_STATE_READ_MULTIPLE))
		return false;
	if (gbfw_storage__multiple_count == 0)
		return false;

	uint8_t token;
	do
	{
		token = 0xFF;
		gbfw_storage_spi_xact(sizeof(token), &token);
	} while (token == MMC_DATA_TOKEN_NONE);

	if (token != MMC_DATA_TOKEN)
		return false;

	gbfw_storage__flags.state++;
	gbfw_storage_spi_block_crc_read_begin(GBFW_STORAGE_BLOCK_SIZE, block);
	return true;
}

bool gbfw_storage_read_multiple_block_end(void)
{
	if ((gbfw_storage__flags.state != GBFW_STORAGE_STATE_READ_SINGLE_ACTIVE)
		&& (gbfw_storage__flags.state != GBFW_STORAGE_STATE_READ_MULTIPLE_ACTIVE))
		return false;

	bool success = gbfw_storage_spi_block_crc_read_end();
	gbfw_storage__multiple_count--;
	gbfw_storage__flags.state--;
	return success;
}

bool gbfw_storage_read_multiple_end(void)
{
	if ((gbfw_storage__flags.state == GBFW_STORAGE_STATE_READ_SINGLE_ACTIVE)
		|| (gbfw_storage__flags.state == GBFW_STORAGE_STATE_READ_MULTIPLE_ACTIVE))
		gbfw_storage_read_multiple_block_end();

	if (gbfw_storage__flags.state == GBFW_STORAGE_STATE_READY)
		return true;

	if ((gbfw_storage__flags.state != GBFW_STORAGE_STATE_READ_SINGLE)
		&& (gbfw_storage__flags.state != GBFW_STORAGE_STATE_READ_MULTIPLE))
		return false;

	bool success = true;
	if ((gbfw_storage__flags.type == GBFW_STORAGE_TYPE_MMC)
		|| (gbfw_storage__flags.state == GBFW_STORAGE_STATE_READ_SINGLE))
	{
		// MMC requires you to read all blocks, can't cancel.
		while (gbfw_storage__multiple_count > 0)
		{
			if (!gbfw_storage_read_multiple_block_begin(NULL)
				|| !gbfw_storage_read_multiple_block_end())
				break;
		}
		success = (gbfw_storage__multiple_count == 0);
		stm32_gpio_pin_mode_set(GBFW_STORAGE_MOSI_PORT, GBFW_STORAGE_MOSI_PIN, STM32_GPIO_MODE_ALTERNATE);
	}
	else
	{
		stm32_gpio_pin_mode_set(GBFW_STORAGE_MOSI_PORT, GBFW_STORAGE_MOSI_PIN, STM32_GPIO_MODE_ALTERNATE);
		mmc_resp_r1_t resp = mmc_cmd_send(
			MMC_CMD_STOP_TRANSMISSION, 0, NULL);
		success = (resp.mask == 0x00);
	}
	mmc_wait_idle();
	gbfw_storage_spi_select(false);

	gbfw_storage__multiple_count = 0;
	gbfw_storage__flags.state = GBFW_STORAGE_STATE_READY;
	return success;
}


bool gbfw_storage_write_multiple_begin(uint32_t addr, uint16_t count)
{
	if (gbfw_storage__flags.state != GBFW_STORAGE_STATE_READY)
		return false;

	if ((gbfw_storage__flags.type == GBFW_STORAGE_TYPE_MMC)
		&& (count != 1))
	{
		mmc_resp_r1_t resp = mmc_cmd_send(
			MMC_CMD_SET_BLOCK_COUNT, count, NULL);
		if (resp.mask != 0x00) return false;
	}
	else if (gbfw_storage__flags.type != GBFW_STORAGE_TYPE_MMC)
	{
		// We don't care about the response as we'll overwrite anyway.
		mmc_cmd_app_send(MMC_CMD_APP_SET_WR_BLK_ERASE_COUNT, count, NULL);
	}

	if (!gbfw_storage__flags.block_addr)
		addr <<= GBFW_STORAGE_BLOCK_SIZE_LOG2;

	// We hold power on during writes to prevent corruption.
	if (!gbfw_power_hold())
		return false;

	mmc_resp_r1_t resp = mmc_cmd_send((count == 1
		? MMC_CMD_WRITE_BLOCK
		: MMC_CMD_WRITE_MULTIPLE_BLOCK), addr, NULL);
	if (resp.mask != 0x00)
	{
		gbfw_power_release();
		return false;
	}

	uint8_t dummy = 0xFF;
	gbfw_storage_spi_xact(sizeof(dummy), &dummy);

	gbfw_storage__flags.state = (count == 1
		? GBFW_STORAGE_STATE_WRITE_SINGLE
		: GBFW_STORAGE_STATE_WRITE_MULTIPLE);
	gbfw_storage__multiple_count = count;
	return true;
}

bool gbfw_storage_write_multiple_block_begin(const uint8_t *block)
{
	if ((gbfw_storage__flags.state != GBFW_STORAGE_STATE_WRITE_SINGLE)
		&& (gbfw_storage__flags.state != GBFW_STORAGE_STATE_WRITE_MULTIPLE))
		return false;
	if (gbfw_storage__multiple_count == 0)
		return false;

	bool is_mmc_or_single = (gbfw_storage__flags.type == GBFW_STORAGE_TYPE_MMC)
		|| (gbfw_storage__flags.state == GBFW_STORAGE_STATE_WRITE_SINGLE);

	gbfw_storage__flags.state++;

	uint8_t token = (is_mmc_or_single ? MMC_DATA_TOKEN : MMC_DATA_TOKEN_WRITE_MULTIPLE);
	gbfw_storage_spi_xact(sizeof(token), &token);
	gbfw_storage_spi_block_crc_write_begin(GBFW_STORAGE_BLOCK_SIZE, block);
	return true;
}

bool gbfw_storage_write_multiple_block_end(void)
{
	if ((gbfw_storage__flags.state != GBFW_STORAGE_STATE_WRITE_SINGLE_ACTIVE)
		&& (gbfw_storage__flags.state != GBFW_STORAGE_STATE_WRITE_MULTIPLE_ACTIVE))
		return false;

	gbfw_storage_spi_block_crc_write_end();

	mmc_data_response_t data_response;
	gbfw_storage_spi_xact(sizeof(data_response), &data_response.mask);

	mmc_wait_idle();

	gbfw_storage__multiple_count--;
	gbfw_storage__flags.state--;
	return (data_response.response == MMC_DATA_RESPONSE_ACCEPTED);
}

bool gbfw_storage_write_multiple_end(void)
{
	if ((gbfw_storage__flags.state == GBFW_STORAGE_STATE_WRITE_SINGLE_ACTIVE)
		|| (gbfw_storage__flags.state == GBFW_STORAGE_STATE_WRITE_MULTIPLE_ACTIVE))
		gbfw_storage_write_multiple_block_end();

	if (gbfw_storage__flags.state == GBFW_STORAGE_STATE_READY)
		return true;

	if ((gbfw_storage__flags.state != GBFW_STORAGE_STATE_WRITE_SINGLE)
		&& (gbfw_storage__flags.state != GBFW_STORAGE_STATE_WRITE_MULTIPLE))
		return false;

	bool success = true;
	if ((gbfw_storage__flags.type == GBFW_STORAGE_TYPE_MMC)
		|| (gbfw_storage__flags.state == GBFW_STORAGE_STATE_WRITE_SINGLE))
	{
		// MMC requires you to write all blocks, can't cancel.
		while (gbfw_storage__multiple_count > 0)
		{
			if (!gbfw_storage_write_multiple_block_begin(NULL)
				|| !gbfw_storage_write_multiple_block_end())
				break;
		}
		success = (gbfw_storage__multiple_count == 0);
	}
	else
	{
		uint8_t token = MMC_DATA_TOKEN_STOP_TRANSMISSION;
		gbfw_storage_spi_xact(sizeof(token), &token);

		uint8_t dummy = 0xFF;
		gbfw_storage_spi_xact(sizeof(dummy), &dummy);

		mmc_wait_idle();
	}
	gbfw_storage_spi_select(false);

	gbfw_power_release();

	gbfw_storage__multiple_count = 0;
	gbfw_storage__flags.state = GBFW_STORAGE_STATE_READY;
	return success;
}