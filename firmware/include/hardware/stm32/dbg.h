#ifndef STM32_DBG_H
#define STM32_DBG_H

#include <stdbool.h>
#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef enum
{
	STM32_DEV_ID_CAT2 = 0x468,
	STM32_DEV_ID_CAT3 = 0x469,
	STM32_DEV_ID_CAT4 = 0x479,
} stm32_dev_id_e;

typedef enum
{
	STM32_REVISION_A      = 0x1000,
	STM32_REVISION_CAT4_Z = 0x1001,
	STM32_REVISION_B      = 0x2000,
	STM32_REVISION_Z      = 0x2001,
	STM32_REVISION_Y      = 0x2002,
	STM32_REVISION_X      = 0x2003,
} stm32_revision_e;

typedef union
{
	struct
	{
		uint32_t devid    : 12;
		uint32_t res15_12 :  4;
		uint32_t revid    : 16;
	};

	uint32_t mask;
} stm32_dbg_idcode_t;

typedef enum
{
	STM32_TRACE_MODE_ASYNC       = 0,
	STM32_TRACE_MODE_SYNC_SIZE_1 = 1,
	STM32_TRACE_MODE_SYNC_SIZE_2 = 2,
	STM32_TRACE_MODE_SYNC_SIZE_4 = 3,
} stm32_trace_mode_e;

typedef union
{
	struct
	{
		uint32_t dbh_sleep   :  1;
		uint32_t dbg_stop    :  1;
		uint32_t dbg_standby :  1;
		uint32_t res4_3      :  2;
		uint32_t trace_ioen  :  1;
		uint32_t trace_mode  :  2;
		uint32_t res31_8     : 24;
	};

	uint32_t mask;
} stm32_dbg_c_t;

typedef union
{
	struct
	{
		uint32_t dbg_tim2_stop   : 1;
		uint32_t dbg_tim3_stop   : 1;
		uint32_t dbg_tim4_stop   : 1;
		uint32_t dbg_tim5_stop   : 1;
		uint32_t dbg_tim6_stop   : 1;
		uint32_t dbg_tim7_stop   : 1;
		uint32_t res9_6          : 4;
		uint32_t dbg_rtc_stop    : 1;
		uint32_t dbg_wwdg_stop   : 1;
		uint32_t dbg_iwdg_stop   : 1;
		uint32_t res20_13        : 8;
		uint32_t dbg_i2c1_stop   : 1;
		uint32_t dbg_i2c2_stop   : 1;
		uint32_t res29_23        : 7;
		uint32_t dbg_i2c3_stop   : 1;
		uint32_t dbg_lptim1_stop : 1;
	};

	uint32_t mask;
} stm32_dbg_apb1_fz1_t;

typedef union
{
	struct
	{
		uint32_t res0          :  1;
		uint32_t dbg_i2c4_stop :  1;
		uint32_t res31_2       : 30;
	};

	uint32_t mask;
} stm32_dbg_apb1_fz2_t;

typedef union
{
	struct
	{
		uint32_t res10_0        : 11;
		uint32_t dbg_tim1_stop  :  1;
		uint32_t res12          :  1;
		uint32_t dbg_tim8_stop  :  1;
		uint32_t res15_14       :  2;
		uint32_t dbg_tim15_stop :  1;
		uint32_t dbg_tim16_stop :  1;
		uint32_t dbg_tim17_stop :  1;
		uint32_t res19          :  1;
		uint32_t dbg_tim20_stop :  1;
		uint32_t res25_21       :  5;
		uint32_t dbg_hrtim_stop :  1;
		uint32_t res31_27       :  5;
	};

	uint32_t mask;
} stm32_dbg_apb2_fz_t;



typedef struct
{
	const stm32_dbg_idcode_t idcode;
	stm32_dbg_c_t            c;
	stm32_dbg_apb1_fz1_t     apb1_fz1;
	stm32_dbg_apb1_fz2_t     apb1_fz2;
	stm32_dbg_apb2_fz_t      apb2_fz;
	uint32_t                 res255_5[251];
} stm32_dbg_t;

static volatile stm32_dbg_t * const stm32_dbg = (volatile stm32_dbg_t *)STM32_MMAP_DBG;


static inline void *stm32_sram1_base(bool code)
{
	return (void *)(code ? 0 : STM32_MMAP_SRAM);
}

static inline uintptr_t stm32_sram1_size(void)
{
	uintptr_t size = 0;
	switch (stm32_dbg->idcode.devid)
	{
		case STM32_DEV_ID_CAT2:
			size = 16;
			break;

		case STM32_DEV_ID_CAT3:
		case STM32_DEV_ID_CAT4:
			size = 80;
			break;

		default:
			break;
	}

	return (size << 10);
}


static inline void *stm32_sram2_base(void)
{
	return (void *)((uintptr_t)stm32_sram1_base(false) + stm32_sram1_size);
}

static inline uintptr_t stm32_sram2_size(void)
{
	return (16 << 10);
}


static inline void *stm32_ccm_sram_base(bool code)
{
	if (code) return (void *)STM32_MMAP_CCM_SRAM_CODE;
	return (void *)((uintptr_t)stm32_sram2_base() + stm32_sram2_size);
}

static inline uintptr_t stm32_ccm_sram_size(void)
{
	uintptr_t size = 0;
	switch (stm32_dbg->idcode.devid)
	{
		case STM32_DEV_ID_CAT2:
			size = 10;
			break;

		case STM32_DEV_ID_CAT3:
			size = 32;
			break;

		case STM32_DEV_ID_CAT4:
			size = 16;
			break;

		default:
			break;
	}

	return (size << 10);
}


static inline void *stm32_sram_base(void)
{
	return (void *)STM32_MMAP_SRAM;
}

static inline uintptr_t stm32_sram_size(void)
{
	return (stm32_sram1_size() + stm32_sram2_size()
		+ stm32_ccm_sram_size());
}

#endif
