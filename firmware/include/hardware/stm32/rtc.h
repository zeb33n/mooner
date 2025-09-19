#ifndef STM32_RTC_H
#define STM32_RTC_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef enum
{
	STM32_RTC_OUTPUT_DISABLED = 0,
	STM32_RTC_OUTPUT_ALARM_A  = 1,
	STM32_RTC_OUTPUT_ALARM_B  = 2,
	STM32_RTC_OUTPUT_WAKEUP   = 3,
} stm32_rtc_output_e;

typedef union
{
	struct
	{
		uint32_t su       : 4;
		uint32_t st       : 3;
		uint32_t res7     : 1;
		uint32_t mnu      : 4;
		uint32_t mnt      : 3;
		uint32_t res15    : 1;
		uint32_t hu       : 4;
		uint32_t ht       : 2;
		uint32_t pm       : 1;
		uint32_t res31_23 : 9;
	};

	uint32_t mask;
} stm32_rtc_t_t;

typedef union
{
	struct
	{
		uint32_t du       : 4;
		uint32_t dt       : 2;
		uint32_t res7_6   : 2;
		uint32_t mu       : 4;
		uint32_t mt       : 1;
		uint32_t wdu      : 3;
		uint32_t yu       : 4;
		uint32_t yt       : 4;
		uint32_t res31_24 : 8;
	};

	uint32_t mask;
} stm32_rtc_d_t;

typedef union
{
	struct
	{
		uint32_t alrawf   :  1;
		uint32_t alrbwf   :  1;
		uint32_t wutwf    :  1;
		uint32_t shpf     :  1;
		uint32_t inits    :  1;
		uint32_t rsf      :  1;
		uint32_t initf    :  1;
		uint32_t init     :  1;
		uint32_t res15_8  :  8;
		uint32_t recalpf  :  1;
		uint32_t res31_17 : 15;
	};

	uint32_t mask;
} stm32_rtc_ics_t;

typedef union
{
	struct
	{
		uint32_t prediv_s : 15;
		uint32_t res15    :  1;
		uint32_t prediv_a :  7;
		uint32_t res31_23 :  9;
	};

	uint32_t mask;
} stm32_rtc_pre_t;

typedef union
{
	struct
	{
		uint32_t wucksel       : 3;
		uint32_t tsedge        : 1;
		uint32_t refckon       : 1;
		uint32_t bypshad       : 1;
		uint32_t fmt           : 1;
		uint32_t res7          : 1;
		uint32_t alrae         : 1;
		uint32_t alrbe         : 1;
		uint32_t wute          : 1;
		uint32_t tse           : 1;
		uint32_t alraie        : 1;
		uint32_t alrbie        : 1;
		uint32_t wutie         : 1;
		uint32_t tsie          : 1;
		uint32_t add1h         : 1;
		uint32_t sub1h         : 1;
		uint32_t bkp           : 1;
		uint32_t cosel         : 1;
		uint32_t pol           : 1;
		uint32_t osel          : 2;
		uint32_t coe           : 1;
		uint32_t itse          : 1;
		uint32_t tampts        : 1;
		uint32_t tampoe        : 1;
		uint32_t res28_27      : 2;
		uint32_t tampalrm_pu   : 1;
		uint32_t tampalrm_type : 1;
		uint32_t out2en        : 1;
	};

	uint32_t mask;
} stm32_rtc_c_t;

typedef union
{
	struct
	{
		uint32_t calm     :  9;
		uint32_t res12_9  :  4;
		uint32_t calw16   :  1;
		uint32_t calw8    :  1;
		uint32_t calwp    :  1;
		uint32_t res31_16 : 16;
	};

	uint32_t mask;
} stm32_rtc_cal_t;

typedef union
{
	struct
	{
		uint32_t subfs    : 15;
		uint32_t res30_15 : 16;
		uint32_t add1s    :  1;
	};

	uint32_t mask;
} stm32_rtc_shift_t;

typedef union
{
	struct
	{
		uint32_t su       : 4;
		uint32_t st       : 3;
		uint32_t msk1     : 1;
		uint32_t mnu      : 4;
		uint32_t mnt      : 3;
		uint32_t msk2     : 1;
		uint32_t hu       : 4;
		uint32_t ht       : 2;
		uint32_t pm       : 1;
		uint32_t msk3     : 1;
		uint32_t du       : 4;
		uint32_t dt       : 2;
		uint32_t wdsel    : 1;
		uint32_t msk4     : 1;
	};

	uint32_t mask;
} stm32_rtc_alrm_t;

typedef union
{
	struct
	{
		uint32_t ss       : 15;
		uint32_t res23_15 :  9;
		uint32_t maskss   :  4;
		uint32_t res31_28 :  4;
	};

	uint32_t mask;
} stm32_rtc_alrmss_t;

typedef union
{
	struct
	{
		uint32_t alra     :  1;
		uint32_t alrb     :  1;
		uint32_t wut      :  1;
		uint32_t ts       :  1;
		uint32_t tsov     :  1;
		uint32_t its      :  1;
		uint32_t res31_6  : 26;
	};

	uint32_t mask;
} stm32_rtc_s_t;


typedef struct
{
	stm32_rtc_t_t      t;
	stm32_rtc_d_t      d;
	uint32_t           ss;
	stm32_rtc_ics_t    ics;
	stm32_rtc_pre_t    pre;
	uint32_t           wut;
	stm32_rtc_c_t      c;
	uint32_t           res8_7[2];
	uint32_t           wp;
	stm32_rtc_cal_t    cal;
	stm32_rtc_shift_t  shift;
	stm32_rtc_t_t      tst;
	stm32_rtc_d_t      dst;
	uint32_t           tsss;
	uint32_t           res15;
	stm32_rtc_alrm_t   alrma;
	stm32_rtc_alrmss_t alrmass;
	stm32_rtc_alrm_t   alrmb;
	stm32_rtc_alrmss_t alrmbss;
	stm32_rtc_s_t      s;
	stm32_rtc_s_t      mis;
	stm32_rtc_s_t      scr;

	uint32_t      res255_23[233];
} stm32_rtc_t;

#define STM32_RTC_WP_KEY1 0xCA
#define STM32_RTC_WP_KEY2 0x53

static volatile stm32_rtc_t * const stm32_rtc = (volatile stm32_rtc_t *)STM32_MMAP_RTC;

#endif
