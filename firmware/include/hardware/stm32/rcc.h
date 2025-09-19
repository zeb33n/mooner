#ifndef STM32_RCC_H
#define STM32_RCC_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef union
{
	struct
	{
		uint32_t       res7_0   : 8;
		uint32_t       hsion    : 1;
		uint32_t       hsikeron : 1;
		const uint32_t hsirdy   : 1;
		uint32_t       res15_11 : 5;
		uint32_t       hseon    : 1;
		const uint32_t hserdy   : 1;
		uint32_t       hsebyp   : 1;
		uint32_t       csson    : 1;
		uint32_t       res23_20 : 4;
		uint32_t       pllon    : 1;
		const uint32_t pllrdy   : 1;
		uint32_t       res31_26 : 6;
	};

	uint32_t mask;
} stm32_rcc_c_t;

typedef union
{
	struct
	{
		uint32_t       res15_0  : 16;
		const uint32_t hsical   :  8;
		uint32_t       hsitrim  :  7;
		uint32_t       res31    :  1;
	};

	uint32_t mask;
} stm32_rcc_icsc_t;

typedef enum
{
	STM32_RCC_MCOSEL_NONE       =  0,
	STM32_RCC_MCOSEL_SYSCLK     =  1,
	STM32_RCC_MCOSEL_HSI16      =  3,
	STM32_RCC_MCOSEL_HSE        =  4,
	STM32_RCC_MCOSEL_PLL        =  5,
	STM32_RCC_MCOSEL_LSI        =  6,
	STM32_RCC_MCOSEL_LSE        =  7,
	STM32_RCC_MCOSEL_HSI48      =  8,
} stm32_rcc_mcosel_e;

typedef enum
{
	STM32_RCC_SW_HSI16   =  1,
	STM32_RCC_SW_HSE     =  2,
	STM32_RCC_SW_PLLRCLK =  3,
} stm32_rcc_sw_e;

typedef union
{
	struct
	{
		uint32_t       sw       :  2;
		const uint32_t sws      :  2;
		uint32_t       hpre     :  4;
		uint32_t       ppre1    :  3;
		uint32_t       ppre2    :  3;
		uint32_t       res23_14 : 10;
		uint32_t       mcosel   :  4;
		uint32_t       mcopre   :  3;
		uint32_t       res31    :  1;
	};

	uint32_t mask;
} stm32_rcc_cfg_t;

typedef enum
{
	STM32_RCC_PLLSRC_NONE       =  0,
	STM32_RCC_PLLSRC_HSI16      =  2,
	STM32_RCC_PLLSRC_HSE        =  3,
} stm32_rcc_pllsrc_e;

typedef union
{
	struct
	{
		uint32_t pllsrc   : 2;
		uint32_t res3_2   : 2;
		uint32_t pllm     : 4;
		uint32_t plln     : 7;
		uint32_t res15    : 1;
		uint32_t pllpen   : 1;
		uint32_t pllp     : 1;
		uint32_t res19_18 : 2;
		uint32_t pllqen   : 1;
		uint32_t pllq     : 2;
		uint32_t res23    : 1;
		uint32_t pllren   : 1;
		uint32_t pllr     : 2;
		uint32_t pllpdiv  : 5;
	};

	uint32_t mask;
} stm32_rcc_pllcfg_t;

typedef union
{
	struct
	{
		uint32_t lsirdy   :  1;
		uint32_t lserdy   :  1;
		uint32_t res2     :  1;
		uint32_t hsirdy   :  1;
		uint32_t hserdy   :  1;
		uint32_t pllrdy   :  1;
		uint32_t res7_6   :  2;
		uint32_t css      :  1;
		uint32_t lsecss   :  1;
		uint32_t hsi48rdy :  1;
		uint32_t res31_11 : 21;
	};

	uint32_t mask;
} stm32_rcc_ci_t;

typedef union
{
	struct
	{
		uint32_t dma1     :  1;
		uint32_t dma2     :  1;
		uint32_t dmamux1  :  1;
		uint32_t cordic   :  1;
		uint32_t fmac     :  1;
		uint32_t res7_5   :  3;
		uint32_t flash    :  1;
		uint32_t sram     :  1;
		uint32_t res11_10 :  2;
		uint32_t crc      :  1;
		uint32_t res31_13 : 19;
	};

	uint32_t mask;
} stm32_rcc_ahb1_t;

typedef union
{
	struct
	{
		uint32_t gpioa    : 1;
		uint32_t gpiob    : 1;
		uint32_t gpioc    : 1;
		uint32_t gpiod    : 1;
		uint32_t gpioe    : 1;
		uint32_t gpiof    : 1;
		uint32_t gpiog    : 1;
		uint32_t res8_7   : 2;
		uint32_t ccmsram  : 1;
		uint32_t sram2    : 1;
		uint32_t res12_11 : 2;
		uint32_t adc12    : 1;
		uint32_t adc345   : 1;
		uint32_t res15    : 1;
		uint32_t dac1     : 1;
		uint32_t dac2     : 1;
		uint32_t dac3     : 1;
		uint32_t dac4     : 1;
		uint32_t res23_20 : 4;
		uint32_t aes      : 1;
		uint32_t res26    : 1;
		uint32_t rng      : 1;
		uint32_t res31_27 : 5;
	};

	uint32_t mask;
} stm32_rcc_ahb2_t;

typedef union
{
	struct
	{
		uint32_t fsmc    :  1;
		uint32_t res7_1  :  7;
		uint32_t qspi    :  1;
		uint32_t res31_9 : 23;
	};

	uint32_t mask;
} stm32_rcc_ahb3_t;

typedef union
{
	struct
	{
		uint32_t tim2     : 1;
		uint32_t tim3     : 1;
		uint32_t tim4     : 1;
		uint32_t tim5     : 1;
		uint32_t tim6     : 1;
		uint32_t tim7     : 1;
		uint32_t res7_6   : 2;
		uint32_t crs      : 1;
		uint32_t res9     : 1;
		uint32_t rtcapb   : 1;
		uint32_t wwdg     : 1;
		uint32_t res13_12 : 2;
		uint32_t spi2     : 1;
		uint32_t spi3     : 1;
		uint32_t res16    : 1;
		uint32_t usart2   : 1;
		uint32_t usart3   : 1;
		uint32_t uart4    : 1;
		uint32_t uart5    : 1;
		uint32_t i2c1     : 1;
		uint32_t i2c2     : 1;
		uint32_t usb      : 1;
		uint32_t res24    : 1;
		uint32_t fdcan    : 1;
		uint32_t res27_26 : 2;
		uint32_t pwr      : 1;
		uint32_t res29    : 1;
		uint32_t i2c3     : 1;
		uint32_t lptim1   : 1;
	};

	uint32_t mask;
} stm32_rcc_apb1_1_t;

typedef union
{
	struct
	{
		uint32_t lpuart1 :  1;
		uint32_t i2c4    :  1;
		uint32_t res7_2  :  6;
		uint32_t ucpd1   :  1;
		uint32_t res31_9 : 23;
	};

	uint32_t mask;
} stm32_rcc_apb1_2_t;

typedef union
{
	struct
	{
		uint32_t syscfg   :  1;
		uint32_t res10_1  : 10;
		uint32_t tim1     :  1;
		uint32_t spi1     :  1;
		uint32_t tim8     :  1;
		uint32_t usart1   :  1;
		uint32_t spi4     :  1;
		uint32_t tim15    :  1;
		uint32_t tim16    :  1;
		uint32_t tim17    :  1;
		uint32_t res19    :  1;
		uint32_t tim20    :  1;
		uint32_t sai1     :  1;
		uint32_t res25_22 :  4;
		uint32_t hrtim    :  1;
		uint32_t res31_27 :  5;
	};

	uint32_t mask;
} stm32_rcc_apb2_t;

typedef enum
{
	STM32_RCC_USART_CLKSEL_PCLK   = 0,
	STM32_RCC_USART_CLKSEL_SYSCLK = 1,
	STM32_RCC_USART_CLKSEL_HSI16  = 2,
	STM32_RCC_USART_CLKSEL_LSE    = 3,
} stm32_rcc_usart_clksel_e;

typedef enum
{
	STM32_RCC_I2C_CLKSEL_PCLK   = 0,
	STM32_RCC_I2C_CLKSEL_SYSCLK = 1,
	STM32_RCC_I2C_CLKSEL_HSI16  = 2,
} stm32_rcc_i2c_clksel_e;

typedef enum
{
	STM32_RCC_LPTIM_CLKSEL_PCLK   = 0,
	STM32_RCC_LPTIM_CLKSEL_SYSCLK = 1,
	STM32_RCC_LPTIM_CLKSEL_HSI16  = 2,
	STM32_RCC_LPTIM_CLKSEL_LSE    = 3,
} stm32_rcc_lptim_clksel_e;

typedef enum
{
	STM32_RCC_SAI_CLKSEL_SYSCLK   = 0,
	STM32_RCC_SAI_CLKSEL_PLLQCLK  = 1,
	STM32_RCC_SAI_CLKSEL_I2S_CKIN = 2,
	STM32_RCC_SAI_CLKSEL_HSI16    = 3,
} stm32_rcc_sai_clksel_e;

typedef enum
{
	STM32_RCC_I2S_CLKSEL_SYSCLK   = 0,
	STM32_RCC_I2S_CLKSEL_PLLQCLK  = 1,
	STM32_RCC_I2S_CLKSEL_I2S_CKIN = 2,
	STM32_RCC_I2S_CLKSEL_HSI16    = 3,
} stm32_rcc_i2s_clksel_e;

typedef enum
{
	STM32_RCC_FDCAN_CLKSEL_HSE     = 0,
	STM32_RCC_FDCAN_CLKSEL_PLLQCLK = 1,
	STM32_RCC_FDCAN_CLKSEL_PCLK    = 2,
} stm32_rcc_fdcan_clksel_e;

typedef enum
{
	STM32_RCC_CLK48_CLKSEL_HSI48   = 0,
	STM32_RCC_CLK48_CLKSEL_PLLQCLK = 2,
} stm32_rcc_clk48_clksel_e;

typedef enum
{
	STM32_RCC_ADC_CLKSEL_NONE    = 0,
	STM32_RCC_ADC_CLKSEL_PLLPCLK = 1,
	STM32_RCC_ADC_CLKSEL_SYSCLK  = 2,
} stm32_rcc_adc_clksel_e;

typedef enum
{
	STM32_RCC_RTC_CLKSEL_NONE      = 0,
	STM32_RCC_RTC_CLKSEL_LSE       = 1,
	STM32_RCC_RTC_CLKSEL_LSI       = 2,
	STM32_RCC_RTC_CLKSEL_HSE_DIV32 = 3,
} stm32_rcc_rtc_clksel_e;

typedef enum
{
	STM32_RCC_LSCO_CLKSEL_LSI = 0,
	STM32_RCC_LSCO_CLKSEL_LSE = 1,
} stm32_rcc_lsco_clksel_e;

typedef enum
{
	STM32_RCC_QSPI_CLKSEL_SYSCLK  = 0,
	STM32_RCC_QSPI_CLKSEL_HSI16   = 1,
	STM32_RCC_QSPI_CLKSEL_PLLQCLK = 2,
} stm32_rcc_qspi_clksel_e;

typedef union
{
	struct
	{
		uint32_t usart1sel  : 2;
		uint32_t usart2sel  : 2;
		uint32_t usart3sel  : 2;
		uint32_t uart4sel   : 2;
		uint32_t uart5sel   : 2;
		uint32_t lpuart2sel : 2;
		uint32_t i2c1sel    : 2;
		uint32_t i2c2sel    : 2;
		uint32_t i2c3sel    : 2;
		uint32_t lptim1sel  : 2;
		uint32_t sai1sel    : 2;
		uint32_t i2s23sel   : 2;
		uint32_t fdcansel   : 2;
		uint32_t clk48sel   : 2;
		uint32_t adc12sel   : 2;
		uint32_t adc345sel  : 2;
	};

	uint32_t mask;
} stm32_rcc_ccip_t;

typedef union
{
	struct
	{
		uint32_t i2c4sel  :  2;
		uint32_t res19_2  : 18;
		uint32_t qspisel  :  2;
		uint32_t res31_22 : 10;
	};

	uint32_t mask;
} stm32_rcc_ccip2_t;

typedef enum
{
	STM32_RCC_LSEDRV_LOW      = 0,
	STM32_RCC_LSEDRV_MED_LOW  = 1,
	STM32_RCC_LSEDRV_MED_HIGH = 2,
	STM32_RCC_LSEDRV_HIGH     = 3,
} stm32_rcc_lsedrv_e;

typedef enum
{
	STM32_RCC_RTCSEL_NONE = 0,
	STM32_RCC_RTCSEL_LSE  = 1,
	STM32_RCC_RTCSEL_LSI  = 2,
	STM32_RCC_RTCSEL_HSE  = 3,
} stm32_rcc_rtcsel_e;

typedef union
{
	struct
	{
		uint32_t       lseon    : 1;
		uint32_t       lserdy   : 1;
		uint32_t       lsebyp   : 1;
		uint32_t       lsedrv   : 2;
		uint32_t       lsecsson : 1;
		const uint32_t lsecssd  : 1;
		uint32_t       res7     : 1;
		uint32_t       rtcsel   : 2;
		uint32_t       res14_10 : 5;
		uint32_t       rtcen    : 1;
		uint32_t       bdrst    : 1;
		uint32_t       res23_17 : 7;
		uint32_t       lscoen   : 1;
		uint32_t       lscosel  : 1;
		uint32_t       res31_26 : 6;
	};

	uint32_t mask;
} stm32_rcc_bdc_t;

typedef union
{
	struct
	{
		uint32_t       lsion    :  1;
		const uint32_t lsirdy   :  1;
		uint32_t       res22_2  : 21;
		uint32_t       rmvf     :  1;
		uint32_t       res24    :  1;
		const uint32_t oblrstf  :  1;
		const uint32_t pinrstf  :  1;
		const uint32_t borrstf  :  1;
		const uint32_t sftrstf  :  1;
		const uint32_t iwwgrstf :  1;
		const uint32_t wwdgrstf :  1;
		const uint32_t lpwrrstf :  1;
	};

	uint32_t mask;
} stm32_rcc_cs_t;

typedef union
{
	struct
	{
		uint32_t       hsi48on  :  1;
		const uint32_t hsi48rdy :  1;
		uint32_t       res6_2   :  5;
		const uint32_t hsi48cal :  9;
		uint32_t       res31_16 : 16;
	};

	uint32_t mask;
} stm32_rcc_crrc_t;


typedef struct
{
	stm32_rcc_c_t        c;
	stm32_rcc_icsc_t     icsc;
	stm32_rcc_cfg_t      cfg;
	stm32_rcc_pllcfg_t   pllcfg;
	uint32_t             res5_4[2];
	stm32_rcc_ci_t       cie;
	const stm32_rcc_ci_t cif;
	stm32_rcc_ci_t       cic;
	uint32_t             res9;
	stm32_rcc_ahb1_t     ahb1rst;
	stm32_rcc_ahb2_t     ahb2rst;
	stm32_rcc_ahb3_t     ahb3rst;
	uint32_t             res13;
	stm32_rcc_apb1_1_t   apb1rst1;
	stm32_rcc_apb1_2_t   apb1rst2;
	stm32_rcc_apb2_t     apb2rst;
	uint32_t             res17;
	stm32_rcc_ahb1_t     ahb1en;
	stm32_rcc_ahb2_t     ahb2en;
	stm32_rcc_ahb3_t     ahb3en;
	uint32_t             res21;
	stm32_rcc_apb1_1_t   apb1en1;
	stm32_rcc_apb1_2_t   apb1en2;
	stm32_rcc_apb2_t     apb2en;
	uint32_t             res25;
	stm32_rcc_ahb1_t     ahb1smen;
	stm32_rcc_ahb2_t     ahb2smen;
	stm32_rcc_ahb3_t     ahb3smen;
	uint32_t             res29;
	stm32_rcc_apb1_1_t   apb1smen1;
	stm32_rcc_apb1_2_t   apb1smen2;
	stm32_rcc_apb2_t     apb2smen;
	uint32_t             res33;
	stm32_rcc_ccip_t     ccip;
	uint32_t             res35;
	stm32_rcc_bdc_t      bdc;
	stm32_rcc_cs_t       cs;
	stm32_rcc_crrc_t     crrc;
	stm32_rcc_ccip2_t    ccip2;

	uint32_t             res255_40[216];
} stm32_rcc_t;

static volatile stm32_rcc_t * const stm32_rcc = (volatile stm32_rcc_t *)STM32_MMAP_RCC;

#endif
