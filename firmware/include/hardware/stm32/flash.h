#ifndef STM32_FLASH_H
#define STM32_FLASH_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef union
{
	struct
	{
		uint32_t latency  :  4;
		uint32_t res7_4   :  4;
		uint32_t prften   :  1;
		uint32_t icen     :  1;
		uint32_t dcen     :  1;
		uint32_t icrst    :  1;
		uint32_t dcrst    :  1;
		uint32_t run_pd   :  1;
		uint32_t sleep_pd :  1;
		uint32_t res17_15 :  3;
		uint32_t dbg_swen :  1;
		uint32_t res31_19 : 13;
	};

	uint32_t mask;
} stm32_flash_ac_t;

typedef union
{
	struct
	{
		uint32_t       eop      :  1;
		uint32_t       operr    :  1;
		uint32_t       res2     :  1;
		uint32_t       progerr  :  1;
		uint32_t       wrperr   :  1;
		uint32_t       pgaerr   :  1;
		uint32_t       sizerr   :  1;
		uint32_t       pgserr   :  1;
		uint32_t       misserr  :  1;
		uint32_t       fasterr  :  1;
		uint32_t       res13_10 :  4;
		uint32_t       rderr    :  1;
		uint32_t       optverr  :  1;
		const uint32_t bsy      :  1;
		uint32_t       res31_17 : 15;
	};

	uint32_t mask;
} stm32_flash_s_t;

typedef union
{
	struct
	{
		uint32_t pg         :  1;
		uint32_t per        :  1;
		uint32_t mer1       :  1;
		uint32_t pnb        :  7;
		uint32_t res10      :  1;
		uint32_t bker       :  1;
		uint32_t res14_12   :  3;
		uint32_t mer2       :  1;
		uint32_t start      :  1;
		uint32_t optstrt    :  1;
		uint32_t fastpg     :  1;
		uint32_t res23_19   :  5;
		uint32_t eopie      :  1;
		uint32_t errie      :  1;
		uint32_t rderrie    :  1;
		uint32_t obl_launch :  1;
		uint32_t sec_prot1  :  1;
		uint32_t sec_prot2  :  1;
		uint32_t optlock    :  1;
		uint32_t lock       :  1;
	};

	uint32_t mask;
} stm32_flash_c_t;

typedef union
{
	struct
	{
		const uint32_t addr_ecc : 19;
		uint32_t       res20_19 :  2;
		const uint32_t bk_ecc   :  1;
		const uint32_t sysf_ecc :  1;
		uint32_t       res23    :  1;
		uint32_t       ecccie   :  1;
		uint32_t       res27_25 :  3;
		uint32_t       eccc2    :  1;
		uint32_t       eccd2    :  1;
		uint32_t       eccc     :  1;
		uint32_t       eccd     :  1;

	};

	uint32_t mask;
} stm32_flash_ecc_t;

typedef union
{
	struct
	{
		uint32_t rdp              : 8;
		uint32_t bor_lev          : 3;
		uint32_t res11            : 1;
		uint32_t nrst_stop        : 1;
		uint32_t nrst_stdby       : 1;
		uint32_t nrst_shdw        : 1;
		uint32_t res15            : 1;
		uint32_t iwdg_sw          : 1;
		uint32_t iwdg_stop        : 1;
		uint32_t iwdg_stdby       : 1;
		uint32_t wwdg_sw          : 1;
		uint32_t bfb2             : 1;
		uint32_t res21            : 1;
		uint32_t dbank            : 1;
		uint32_t nboot1           : 1;
		uint32_t sram_pe          : 1;
		uint32_t ccmsram_rst      : 1;
		uint32_t nswboot0         : 1;
		uint32_t nboot0           : 1;
		uint32_t nrst_mode        : 2;
		uint32_t irhen            : 1;
		uint32_t res31            : 1;
	};

	uint32_t mask;
} stm32_flash_opt_t;

typedef union
{
	struct
	{
		uint32_t pcrop_start : 15;
		uint32_t res31_15    : 17;
	};

	uint32_t mask;
} stm32_flash_pcrop_start_t;

typedef union
{
	struct
	{
		uint32_t pcrop_end : 15;
		uint32_t res30_15  : 16;
		uint32_t pcrop_rdp :  1;
	};

	uint32_t mask;
} stm32_flash_pcrop_end_t;

typedef union
{
	struct
	{
		uint32_t wrp_strt : 7;
		uint32_t res15_7  : 9;
		uint32_t wrp_end  : 7;
		uint32_t res31_23 : 9;
	};

	uint32_t mask;
} stm32_flash_wrp_t;

typedef union
{
	struct
	{
		uint32_t sec_size  :  8;
		uint32_t res15_8   :  8;
		uint32_t boot_lock :  1;
		uint32_t res31_17  : 15;
	};

	uint32_t mask;
} stm32_flash_sec_t;


typedef struct
{
	stm32_flash_ac_t          ac;
	uint32_t                  pdkey;
	uint32_t                  key;
	uint32_t                  optkey;
	stm32_flash_s_t           s;
	stm32_flash_c_t           c;
	stm32_flash_ecc_t         ecc;
	uint32_t                  res7;
	stm32_flash_opt_t         opt;
	stm32_flash_pcrop_start_t pcrop1s;
	stm32_flash_pcrop_end_t   pcrop1e;
	stm32_flash_wrp_t         wrp1a;
	stm32_flash_wrp_t         wrp1b;
	uint32_t                  res16_13[4];
	stm32_flash_pcrop_start_t pcrop2s;
	stm32_flash_pcrop_end_t   pcrop2e;
	stm32_flash_wrp_t         wrp2a;
	stm32_flash_wrp_t         wrp2b;
	uint32_t                  res27_21[7];
	stm32_flash_sec_t         sec1;
	stm32_flash_sec_t         sec2;
	uint32_t                  res255_30[226];
} stm32_flash_t;


#define STM32_FLASH_PDKEY1 0x04152637
#define STM32_FLASH_PDKEY2 0xFAFBFCFD

#define STM32_FLASH_KEY1 0x45670123
#define STM32_FLASH_KEY2 0xCDEF89AB

#define STM32_FLASH_OPTKEY1 0x08192A3B
#define STM32_FLASH_OPTKEY2 0x4C5D6E7F

#define STM32_FLASH_LINE_SIZE_WORDS       2
#define STM32_FLASH_LINE_SIZE_BYTES       (STM32_FLASH_LINE_SIZE_WORDS * 4)
#define STM32_FLASH_PAGE_SIZE_SINGLE_BANK 4096
#define STM32_FLASH_PAGE_SIZE_DUAL_BANK   2048

static const uint32_t * const STM32_FLASH_SIZE = (uint32_t *)(STM32_MMAP_ENGINEERING_BYTES + 0xE0);

static inline uint32_t stm32_flash_size_bytes(void)
{
	return (*STM32_FLASH_SIZE & 0xFFFF) << 10U;
}


static volatile stm32_flash_t * const stm32_flash  = (volatile stm32_flash_t *)STM32_MMAP_FLASH;

static inline uint8_t stm32_flash_page_from_addr_single_bank(const void *addr)
{
	return ((uint32_t)addr - STM32_MMAP_MAIN_FLASH) / STM32_FLASH_PAGE_SIZE_SINGLE_BANK;
}

static inline uint8_t stm32_flash_page_from_addr_dual_bank(const void *addr)
{
	return ((uint32_t)addr - STM32_MMAP_MAIN_FLASH) / STM32_FLASH_PAGE_SIZE_DUAL_BANK;
}

#endif
