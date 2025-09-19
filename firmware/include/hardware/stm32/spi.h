#ifndef STM32_SPI_H
#define STM32_SPI_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef union
{
	struct
	{
		uint32_t cpha     :  1;
		uint32_t cpol     :  1;
		uint32_t mstr     :  1;
		uint32_t br       :  3;
		uint32_t spe      :  1;
		uint32_t lsbfirst :  1;
		uint32_t ssi      :  1;
		uint32_t ssm      :  1;
		uint32_t rxonly   :  1;
		uint32_t crcl     :  1;
		uint32_t crcnext  :  1;
		uint32_t crcen    :  1;
		uint32_t bidioe   :  1;
		uint32_t bidimode :  1;
		uint32_t res31_16 : 16;
	};

	uint32_t mask;
} stm32_spi_c1_t;

typedef union
{
	struct
	{
		uint32_t rxdmaen  :  1;
		uint32_t txdmaen  :  1;
		uint32_t ssoe     :  1;
		uint32_t nssp     :  1;
		uint32_t frf      :  1;
		uint32_t errie    :  1;
		uint32_t rxneie   :  1;
		uint32_t txeie    :  1;
		uint32_t ds       :  4;
		uint32_t frxth    :  1;
		uint32_t ldma_rx  :  1;
		uint32_t ldma_tx  :  1;
		uint32_t res31_15 : 17;
	};

	uint32_t mask;
} stm32_spi_c2_t;

typedef enum
{
	STM32_SPI_FLVL_EMPTY   = 0,
	STM32_SPI_FLVL_QUARTER = 1,
	STM32_SPI_FLVL_HALF    = 2,
	STM32_SPI_FLVL_FULL    = 3,
} stm32_spi_flvl_e;

typedef union
{
	struct
	{
		const uint32_t rxne     :  1;
		const uint32_t txe      :  1;
		const uint32_t chside   :  1;
		const uint32_t udr      :  1;
		uint32_t       crcerr   :  1;
		const uint32_t modf     :  1;
		const uint32_t ovr      :  1;
		const uint32_t bsy      :  1;
		const uint32_t fre      :  1;
		const uint32_t frlvl    :  2;
		const uint32_t ftlvl    :  2;
		uint32_t       res31_13 : 19;
	};

	uint32_t mask;
} stm32_spi_s_t;

typedef enum
{
	STM32_SPI_I2S_CFG_SLAVE_TRANSMIT  = 0,
	STM32_SPI_I2S_CFG_SLAVE_RECEIVE   = 1,
	STM32_SPI_I2S_CFG_MASTER_TRANSMIT = 2,
	STM32_SPI_I2S_CFG_MASTER_RECEIVE  = 3,
} stm32_spi_i2s_cfg_e;

typedef enum
{
	STM32_SPI_I2S_STD_PHILIPS = 0,
	STM32_SPI_I2S_STD_MSB     = 1,
	STM32_SPI_I2S_STD_LSB     = 2,
	STM32_SPI_I2S_STD_PCM     = 3,
} stm32_spi_i2s_std_e;

typedef union
{
	struct
	{
		uint32_t chlen    :  1;
		uint32_t datlen   :  2;
		uint32_t ckpol    :  1;
		uint32_t i2sstd   :  2;
		uint32_t res6     :  1;
		uint32_t pcmsync  :  1;
		uint32_t i2scfg   :  2;
		uint32_t i2se     :  1;
		uint32_t i2smod   :  1;
		uint32_t astrten  :  1;
		uint32_t res31_15 : 17;
	};

	uint32_t mask;
} stm32_spi_i2scfg_t;

typedef union
{
	struct
	{
		uint32_t i2sdiv   :  8;
		uint32_t odd      :  1;
		uint32_t mckoe    :  1;
		uint32_t res31_10 : 22;
	};

	uint32_t mask;
} stm32_spi_i2sp_t;


typedef struct
{
	stm32_spi_c1_t     c1;
	stm32_spi_c2_t     c2;
	stm32_spi_s_t      s;
	uint32_t           d;
	uint32_t           crcp;
	uint32_t           rxcrc;
	uint32_t           txcrc;
	stm32_spi_i2scfg_t i2scfg;
	stm32_spi_i2sp_t   i2sp;
	uint32_t           res255_8[248];
} stm32_spi_t;

static volatile stm32_spi_t * const stm32_spi1 = (volatile stm32_spi_t *)STM32_MMAP_SPI1;
static volatile stm32_spi_t * const stm32_spi2 = (volatile stm32_spi_t *)STM32_MMAP_SPI2;
static volatile stm32_spi_t * const stm32_spi3 = (volatile stm32_spi_t *)STM32_MMAP_SPI3;
static volatile stm32_spi_t * const stm32_spi4 = (volatile stm32_spi_t *)STM32_MMAP_SPI4;

#endif
