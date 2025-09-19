#include "display.h"
#include "delay.h"
#include "task.h"
#include "dma.h"
#include "irq.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/gpio.h>
#include <hardware/stm32/tim.h>
#include <hardware/stm32/spi.h>
#include <hardware/stm32/dma.h>
#include <hardware/stm32/dmamux.h>
#include <hardware/stm32/irq.h>
#include <hardware/cm4/nvic.h>
#include <hardware/cm4/intrinsics.h>
#include <hardware/st7735.h>
#include <stddef.h>


#define GBFW_DISPLAY_SPI             stm32_spi2
#define GBFW_DISPLAY_SPI_SPEED       1

#define GBFW_DISPLAY_BACKLIGHT_TIMER stm32_tim1

#define GBFW_DISPLAY_DMA             stm32_dma1
#define GBFW_DISPLAY_DMA_CHANNEL     1
#define GBFW_DISPLAY_DMA_IRQ         STM32_IRQ_DMA1_CH2
#define GBFW_DISPLAY_DMA_IRQ_VECTOR  stm32_vector_irq_dma1_ch2

#define GBFW_DISPLAY_NSS_PORT       STM32_GPIO_PORT_B
#define GBFW_DISPLAY_NSS_PIN        12

#define GBFW_DISPLAY_SCK_PORT       STM32_GPIO_PORT_B
#define GBFW_DISPLAY_SCK_PIN        13
#define GBFW_DISPLAY_SCK_AF         5

#define GBFW_DISPLAY_DCX_PORT       STM32_GPIO_PORT_B
#define GBFW_DISPLAY_DCX_PIN        14

#define GBFW_DISPLAY_SDA_PORT       STM32_GPIO_PORT_B
#define GBFW_DISPLAY_SDA_PIN        15
#define GBFW_DISPLAY_SDA_AF         5

#define GBFW_DISPLAY_BACKLIGHT_PORT STM32_GPIO_PORT_A
#define GBFW_DISPLAY_BACKLIGHT_PIN  8
#define GBFW_DISPLAY_BACKLIGHT_AF   6



static inline void gbfw_display_backlight_init(void)
{
	stm32_rcc->apb2en.tim1 = true;

	// Set PWM mode.
	GBFW_DISPLAY_BACKLIGHT_TIMER->ccm1.oc.oc1m = STM32_TIM_CCM_OCM_PWM_INVERTED;

	// Enable output.
	GBFW_DISPLAY_BACKLIGHT_TIMER->cce.cc1e = true;
	GBFW_DISPLAY_BACKLIGHT_TIMER->bdt.moe  = true;

	// Count from 0 to 255.
	GBFW_DISPLAY_BACKLIGHT_TIMER->ar = 256;

	// Default to off.
	GBFW_DISPLAY_BACKLIGHT_TIMER->cc1 = 0;

	// Start timer.
	GBFW_DISPLAY_BACKLIGHT_TIMER->c1.cen = true;

	stm32_gpio_pin_af_set  (GBFW_DISPLAY_BACKLIGHT_PORT, GBFW_DISPLAY_BACKLIGHT_PIN, GBFW_DISPLAY_BACKLIGHT_AF);
	stm32_gpio_pin_mode_set(GBFW_DISPLAY_BACKLIGHT_PORT, GBFW_DISPLAY_BACKLIGHT_PIN, STM32_GPIO_MODE_ALTERNATE);
}

void gbfw_display_backlight_set(uint8_t level)
{
	GBFW_DISPLAY_BACKLIGHT_TIMER->cc1 = level;
}



static inline void gbfw_display_spi_init(void)
{
	stm32_rcc->apb1en1.spi2 = true;

	stm32_gpio_speed_e speed = STM32_GPIO_SPEED_MEDIUM;
	stm32_gpio_pin_speed_set(GBFW_DISPLAY_SCK_PORT, GBFW_DISPLAY_SCK_PIN, speed);
	stm32_gpio_pin_speed_set(GBFW_DISPLAY_SDA_PORT, GBFW_DISPLAY_SDA_PIN, speed);
	stm32_gpio_pin_speed_set(GBFW_DISPLAY_DCX_PORT, GBFW_DISPLAY_DCX_PIN, speed);
	stm32_gpio_pin_speed_set(GBFW_DISPLAY_NSS_PORT, GBFW_DISPLAY_NSS_PIN, speed);

	stm32_gpio_pin_af_set(GBFW_DISPLAY_SCK_PORT, GBFW_DISPLAY_SCK_PIN, GBFW_DISPLAY_SCK_AF);
	stm32_gpio_pin_af_set(GBFW_DISPLAY_SDA_PORT, GBFW_DISPLAY_SDA_PIN, GBFW_DISPLAY_SDA_AF);

	stm32_gpio_pin_set(GBFW_DISPLAY_NSS_PORT, GBFW_DISPLAY_NSS_PIN);

	stm32_gpio_pin_mode_set(GBFW_DISPLAY_SCK_PORT, GBFW_DISPLAY_SCK_PIN, STM32_GPIO_MODE_ALTERNATE);
	stm32_gpio_pin_mode_set(GBFW_DISPLAY_SDA_PORT, GBFW_DISPLAY_SDA_PIN, STM32_GPIO_MODE_ALTERNATE);
	stm32_gpio_pin_mode_set(GBFW_DISPLAY_DCX_PORT, GBFW_DISPLAY_DCX_PIN, STM32_GPIO_MODE_OUTPUT);
	stm32_gpio_pin_mode_set(GBFW_DISPLAY_NSS_PORT, GBFW_DISPLAY_NSS_PIN, STM32_GPIO_MODE_OUTPUT);

	stm32_spi_c1_t c1 = { .mask = GBFW_DISPLAY_SPI->c1.mask };
	c1.mstr     = true;
	c1.ssm      = true;
	c1.ssi      = true;
	c1.bidimode = true;
	c1.bidioe   = true;
	c1.br       = GBFW_DISPLAY_SPI_SPEED;
	GBFW_DISPLAY_SPI->c1.mask = c1.mask;

	stm32_spi_c2_t c2 = { .mask = GBFW_DISPLAY_SPI->c2.mask };
	c2.txeie  = true;
	c2.frxth  = true;
	GBFW_DISPLAY_SPI->c2.mask = c2.mask;

	stm32_dmamux->cc[GBFW_DISPLAY_DMA_CHANNEL].dmareq_id = STM32_DMAMUX_REQ_SPI2_TX;

	stm32_dma_cc_t cc = { .mask = GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cc.mask };
	cc.tcie    = true;
	cc.htie    = false;
	cc.dir     = STM32_DMA_DIR_READ_MEMORY;
	cc.circ    = false;
	cc.pinc    = false;
	cc.minc    = true;
	cc.psize   = STM32_DMA_SIZE_HALF;
	cc.msize   = STM32_DMA_SIZE_HALF;
	cc.pl      = 2;
	cc.mem2mem = false;
	GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cc.mask = cc.mask;

	GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cpa = (uint32_t)&GBFW_DISPLAY_SPI->d;

	cm4_nvic_interrupt_priority_set(GBFW_DISPLAY_DMA_IRQ, GBFW_IRQ_PRIORITY_DISPLAY);
	cm4_nvic_interrupt_enable(GBFW_DISPLAY_DMA_IRQ);

	GBFW_DISPLAY_SPI->c1.spe = true;
}

static void gbfw_display_spi_wait_dma_idle(void)
{
	wfi_while(GBFW_DISPLAY_SPI->c2.txdmaen);
}

static void gbfw_display_spi_wait_idle(void)
{
	gbfw_display_spi_wait_dma_idle();

	while (GBFW_DISPLAY_SPI->s.bsy);
}

static void gbfw_display_spi_select(bool select)
{
	gbfw_display_spi_wait_idle();
	stm32_gpio_pin_write(GBFW_DISPLAY_NSS_PORT, GBFW_DISPLAY_NSS_PIN, !select);
}

typedef enum
{
	GBFW_DISPLAY_SPI_MODE_COMMAND = 0,
	GBFW_DISPLAY_SPI_MODE_DATA    = 1,
} gbfw_display_spi_mode_e;

static void gbfw_display_spi_mode_set(gbfw_display_spi_mode_e mode)
{
	gbfw_display_spi_wait_idle();
	stm32_gpio_pin_write(GBFW_DISPLAY_DCX_PORT, GBFW_DISPLAY_DCX_PIN, mode);
}

static void gbfw_display_spi_write(size_t size, const void *data)
{
	gbfw_display_spi_wait_dma_idle();

	const uint8_t *ptr = (const uint8_t *)data;
	const uint8_t *end = &ptr[size];
	while ((uintptr_t)ptr < (uintptr_t)end)
	{
		while (!GBFW_DISPLAY_SPI->s.txe);

		*((volatile uint8_t *)&GBFW_DISPLAY_SPI->d) = *ptr++;
	}
}

static uint8_t gbfw_display_spi_read_byte(void)
{
	gbfw_display_spi_wait_dma_idle();

	GBFW_DISPLAY_SPI->c1.spe    = false;
	GBFW_DISPLAY_SPI->c1.bidioe = false;

	// Needed as interrupt could allow additional bytes to be read.
	cpsid(if);
	GBFW_DISPLAY_SPI->c1.spe = true;
	GBFW_DISPLAY_SPI->c1.spe = false;
	cpsie(if);

	// STM32 Errata 2.18.1 says not to use busy flag.
	while (!GBFW_DISPLAY_SPI->s.rxne);
	uint8_t byte = *((volatile uint8_t *)&GBFW_DISPLAY_SPI->d);

	GBFW_DISPLAY_SPI->c1.bidioe = true;
	GBFW_DISPLAY_SPI->c1.spe    = true;

	return byte;
}

static void    (*gbfw_display__dma_callback)(rgb565_t *buff, uint8_t size, uint8_t row) = NULL;
static uint8_t   gbfw_display__dma_count;
static uint8_t   gbfw_display__dma_offset;
static rgb565_t *gbfw_display__dma_addr;
static uint8_t   gbfw_display__dma_size;

void __attribute__((interrupt)) GBFW_DISPLAY_DMA_IRQ_VECTOR(void)
{
	GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cc.en = false;
	stm32_dma_interrupt_clear(
		GBFW_DISPLAY_DMA, GBFW_DISPLAY_DMA_CHANNEL,
		STM32_DMA_INTERRUPT_TRANSFER_COMPLETE);

	gbfw_display__dma_offset++;
	if (gbfw_display__dma_offset >= gbfw_display__dma_count)
	{
		GBFW_DISPLAY_SPI->c2.txdmaen = false;
	}
	else
	{
		rgb565_t *top = &gbfw_display__dma_addr[gbfw_display__dma_size];

		if ((gbfw_display__dma_offset & 1) != 0)
		{
			GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cma   = (uint32_t)top;
			GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cndt  = gbfw_display__dma_size;
			GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cc.en = true;

			if (gbfw_display__dma_offset < (gbfw_display__dma_count - 1))
				gbfw_display__dma_callback(gbfw_display__dma_addr, gbfw_display__dma_size, (gbfw_display__dma_offset + 1));
		}
		else
		{
			GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cma   = (uint32_t)gbfw_display__dma_addr;
			GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cndt  = gbfw_display__dma_size;
			GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cc.en = true;

			if (gbfw_display__dma_offset < (gbfw_display__dma_count - 1))
				gbfw_display__dma_callback(top, gbfw_display__dma_size, (gbfw_display__dma_offset + 1));
		}
	}

	// STM32 Errata 2.1.3
	dsb();
}

static void gbfw_display_spi_write_dma_dblbuf16__dummy_callback(
	rgb565_t *buff, uint8_t size, uint8_t row)
{
	(void)buff;
	(void)size;
	(void)row;
}

static void gbfw_display_spi_write_dma_dblbuf16(uint8_t size, rgb565_t *data, uint8_t count,
	void (*callback)(rgb565_t *buff, uint8_t size, uint8_t row))
{
	gbfw_display_spi_wait_idle();

	GBFW_DISPLAY_SPI->c2.txdmaen = true;

	// We have to do 16-bit data transfers or the color is byte reversed.
	GBFW_DISPLAY_SPI->c2.ds = 15;

	gbfw_display__dma_callback = (callback ? callback : gbfw_display_spi_write_dma_dblbuf16__dummy_callback);
	gbfw_display__dma_count    = count;
	gbfw_display__dma_offset   = 0;
	gbfw_display__dma_addr     = (rgb565_t *)gbfw_dma_pointer(data);
	gbfw_display__dma_size     = size;

	GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cma  = (uint32_t)gbfw_display__dma_addr;
	GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cndt = gbfw_display__dma_size;

	if (count > 0) gbfw_display__dma_callback( data      , size, 0);
	if (count > 1) gbfw_display__dma_callback(&data[size], size, 1);

	GBFW_DISPLAY_DMA->c[GBFW_DISPLAY_DMA_CHANNEL].cc.en = true;
}



static void gbfw_display_command_begin(uint8_t cmd)
{
	gbfw_display_spi_select(true);

	gbfw_display_spi_mode_set(GBFW_DISPLAY_SPI_MODE_COMMAND);

	// Ensure 8-bit transmission for commands.
	GBFW_DISPLAY_SPI->c2.ds = 7;

	gbfw_display_spi_write(sizeof(cmd), &cmd);
}

static void gbfw_display_write_command(uint8_t cmd, uint8_t size, const uint8_t *data)
{
	gbfw_display_command_begin(cmd);

	if (size > 0)
	{
		gbfw_display_spi_mode_set(GBFW_DISPLAY_SPI_MODE_DATA);
		gbfw_display_spi_write(size, data);
	}

	gbfw_display_spi_select(false);
}

static uint8_t gbfw_display_read_command(uint8_t cmd)
{
	gbfw_display_command_begin(cmd);

	gbfw_display_spi_mode_set(GBFW_DISPLAY_SPI_MODE_DATA);
	uint8_t byte = gbfw_display_spi_read_byte();
	gbfw_display_spi_select(false);
	return byte;
}

static void gbfw_display_framectr_set(uint8_t rtna, uint8_t fp, uint8_t bp)
{
	uint8_t c[3] = { rtna, fp, bp };
	gbfw_display_write_command(ST7735_CMD_FRMCTR1, sizeof(c), c);
}

static void gbfw_display_colmod_set(st7735_colmod_e colmod)
{
	st7735_colmod_t c = { .mask = 0 };
	c.ifpf = colmod;
	gbfw_display_write_command(ST7735_CMD_COLMOD, sizeof(c), &c.mask);
}

static void gbfw_display_madctl_set(
	st7735_madctl_mxh_e    col_addr,
	st7735_madctl_myl_e    row_addr,
	st7735_madctl_mxh_e    h_refresh,
	st7735_madctl_myl_e    v_refresh,
	st7735_madctl_order_e  rgb,
	st7735_madctl_rotate_e rotate)
{
	st7735_madctl_t madctl = { .mask = 0 };
	madctl.mh  = h_refresh;
	madctl.rgb = rgb;
	madctl.ml  = v_refresh;
	madctl.mv  = rotate;
	madctl.mx  = col_addr;
	madctl.my  = row_addr;

	gbfw_display_write_command(ST7735_CMD_MADCTL, sizeof(madctl), &madctl.mask);
}

static void gbfw_display_region_set(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	x += 2;
	y += 1;

	w += x - 1;
	h += y - 1;

	static uint8_t caset[4] = {0};
	caset[1] = x;
	caset[3] = w;
	gbfw_display_write_command(ST7735_CMD_CASET, sizeof(caset), caset);

	static uint8_t raset[4] = {0};
	raset[1] = y;
	raset[3] = h;
	gbfw_display_write_command(ST7735_CMD_RASET, sizeof(raset), raset);
}

static gbfw_display_state_e gbfw_display__state = GBFW_DISPLAY_STATE_UNINITIALIZED;

static void gbfw_display_init_deferred(void)
{
	st7735_sdr_t sdr = { .mask = gbfw_display_read_command(ST7735_CMD_RDDSDR) };
	if (!sdr.brd || !sdr.attd || !sdr.fund || !sdr.reld)
	{
		gbfw_display__state = GBFW_DISPLAY_STATE_ERROR;
		return;
	}

	gbfw_display_write_command(ST7735_CMD_IDMOFF, 0, NULL);

	gbfw_display_madctl_set(
		ST7735_MADCTL_RIGHT_TO_LEFT,
		ST7735_MADCTL_BOTTOM_TO_TOP,
		ST7735_MADCTL_LEFT_TO_RIGHT,
		ST7735_MADCTL_TOP_TO_BOTTOM,
		ST7735_MADCTL_BGR,
		ST7735_MADCTL_NORMAL);

	gbfw_display_colmod_set(ST7735_COLMOD_RGB565);

	// Since we don't have a tearing effect pin, set the framerate as high as possible.
	gbfw_display_framectr_set(0, 1, 1);

	gbfw_display_write_command(ST7735_CMD_DISPON, 0, NULL);

	gbfw_display__state = GBFW_DISPLAY_STATE_READY;
}

void gbfw_display_init(void)
{
	gbfw_display__state = GBFW_DISPLAY_STATE_INITIALIZING;

	gbfw_display_spi_init();

	gbfw_display_backlight_init();

	gbfw_display_backlight_set(0x00);

	gbfw_delay_us(1200);

	st7735_sdr_t sdr = { .mask = gbfw_display_read_command(ST7735_CMD_RDDSDR) };
	if (sdr.mask != 0x00)
	{
		gbfw_display__state = GBFW_DISPLAY_STATE_ERROR;
		return;
	}

	gbfw_display_write_command(ST7735_CMD_SLPOUT, 0, NULL);
	gbfw_task_schedule(gbfw_display_init_deferred, 120000);
}

gbfw_display_state_e gbfw_display_state(void)
{
	return gbfw_display__state;
}

rgb565_t gbfw_display_scanbuff[GBFW_DISPLAY_SCANBUFF_COUNT];

void gbfw_display_render_region(
	uint8_t x, uint8_t y,
	uint8_t w, uint8_t h,
	void (*callback)(rgb565_t *buff, uint8_t size, uint8_t row))
{
	gbfw_display_region_set(x, y, w, h);

	gbfw_display_command_begin(ST7735_CMD_RAMWR);

	gbfw_display_spi_mode_set(GBFW_DISPLAY_SPI_MODE_DATA);

	gbfw_display_spi_write_dma_dblbuf16(w, gbfw_display_scanbuff, h, callback);

	// This shouldn't work as we're de-selecting the display a very short
	// way through rendering it, however it does work and significantly
	// simplifies the code as we don't need to wait until the SPI has
	// finished writing after a display update to disable the chip select.
	gbfw_display_spi_select(false);
}

void gbfw_display_sync(void)
{
	gbfw_display_spi_wait_idle();
}
