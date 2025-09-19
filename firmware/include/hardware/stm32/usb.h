#ifndef STM32_USB_H
#define STM32_USB_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


#define STM32_USB_ENDPOINTS          8
#define STM32_USB_SRAM_SIZE_BYTES 1024
#define STM32_USB_MAX_PACKET_SIZE  512

#if STM32_USB_ENDPOINTS != 8
#error "Header not designed to support more or less than 8 endpoints."
#endif


typedef enum
{
	STM32_USB_EP_STAT_DISABLED = 0,
	STM32_USB_EP_STAT_STALL    = 1,
	STM32_USB_EP_STAT_NAK      = 2,
	STM32_USB_EP_STAT_VALID    = 3,
} stm32_usb_ep_stat_e;

typedef enum
{
	STM32_USB_EP_TYPE_BULK      = 0,
	STM32_USB_EP_TYPE_CONTROL   = 1,
	STM32_USB_EP_TYPE_ISO       = 2,
	STM32_USB_EP_TYPE_INTERRUPT = 3,
} stm32_usb_ep_type_e;

typedef enum
{
	STM32_USB_EP_KIND_DEFAULT    = 0,
	STM32_USB_EP_KIND_DBL_BUF    = 1,
	STM32_USB_EP_KIND_STATUS_OUT = 1,
} stm32_usb_ep_kind_e;

typedef union
{
	struct
	{
		uint32_t ea       :  4;
		uint32_t stat_tx  :  2;
		uint32_t dtog_tx  :  1;
		uint32_t ctr_tx   :  1;
		uint32_t ep_kind  :  1;
		uint32_t ep_type  :  2;
		uint32_t setup    :  1;
		uint32_t stat_rx  :  2;
		uint32_t dtog_rx  :  1;
		uint32_t ctr_rx   :  1;
		uint32_t res31_16 : 16;
	};

	uint32_t mask;
} stm32_usb_ep_t;

typedef union
{
	struct
	{
		uint32_t fres     :  1;
		uint32_t pdwn     :  1;
		uint32_t lp_mode  :  1;
		uint32_t fsusp    :  1;
		uint32_t resume   :  1;
		uint32_t l1resume :  1;
		uint32_t res6     :  1;
		uint32_t l1reqm   :  1;
		uint32_t esofm    :  1;
		uint32_t sofm     :  1;
		uint32_t resetm   :  1;
		uint32_t suspm    :  1;
		uint32_t wkupm    :  1;
		uint32_t errm     :  1;
		uint32_t pmaovrm  :  1;
		uint32_t ctrm     :  1;
		uint32_t res31_16 : 16;
	};

	uint32_t mask;
} stm32_usb_cnt_t;

typedef union
{
	struct
	{
		const uint32_t ep_id    :  4;
		const uint32_t dir      :  1;
		uint32_t       res6_5   :  2;
		uint32_t       l1req    :  1;
		uint32_t       esof     :  1;
		uint32_t       sof      :  1;
		uint32_t       reset    :  1;
		uint32_t       susp     :  1;
		uint32_t       wkup     :  1;
		uint32_t       err      :  1;
		uint32_t       pmaovr   :  1;
		const uint32_t ctr      :  1;
		uint32_t       res31_16 : 16;
	};

	uint32_t mask;
} stm32_usb_ist_t;

typedef union
{
	struct
	{
		uint32_t fn       : 11;
		uint32_t lsof     :  2;
		uint32_t lck      :  1;
		uint32_t rxdm     :  1;
		uint32_t rxdp     :  1;
		uint32_t res31_16 : 16;
	};

	uint32_t mask;
} stm32_usb_fn_t;

typedef union
{
	struct
	{
		uint32_t add     :  7;
		uint32_t ef      :  1;
		uint32_t res31_8 : 24;
	};

	uint32_t mask;
} stm32_usb_dadd_t;

typedef union
{
	struct
	{
		uint32_t lpmen   :  1;
		uint32_t lpmack  :  1;
		uint32_t res2    :  1;
		uint32_t remwake :  1;
		uint32_t besl    :  4;
		uint32_t res31_8 : 24;
	};

	uint32_t mask;
} stm32_usb_lpmcs_t;

typedef union
{
	struct
	{
		uint32_t       bcden    :  1;
		uint32_t       dcden    :  1;
		uint32_t       pden     :  1;
		uint32_t       sden     :  1;
		const uint32_t dcdet    :  1;
		const uint32_t pdet     :  1;
		const uint32_t sdet     :  1;
		const uint32_t ps2det   :  1;
		uint32_t       res14_8  :  7;
		uint32_t       dppu     :  1;
		uint32_t       res31_16 : 16;
	};

	uint32_t mask;
} stm32_usb_bcd_t;


typedef struct
{
	stm32_usb_ep_t       ep[STM32_USB_ENDPOINTS];
	uint32_t             res15_8[8];
	stm32_usb_cnt_t      cnt;
	stm32_usb_ist_t      ist;
	const stm32_usb_fn_t fn;
	stm32_usb_dadd_t     dadd;
	uint32_t             btable;
	stm32_usb_lpmcs_t    lpmcs;
	stm32_usb_bcd_t      bcd;
	uint32_t             res255_23[233];
} stm32_usb_t;

static volatile stm32_usb_t * const stm32_usb      = (volatile stm32_usb_t *)STM32_MMAP_USB_FS;
static volatile uint16_t    * const stm32_usb_sram = (volatile uint16_t    *)STM32_MMAP_USB_SRAM;

#define STM32_USB_SRAM_PTR(x) (volatile void *)((uintptr_t)stm32_usb_sram + (x))
#define STM32_USB_SRAM_SIZE_ROUND16(x) ((x) + ((x) & 1))

static inline void stm32_usb_sram_copy_in(
	uint16_t dst, const void * restrict src, uint16_t size)
{
	volatile uint16_t * restrict dst16
		= (volatile uint16_t *)STM32_USB_SRAM_PTR(dst);
	volatile const uint16_t * restrict src16 = src;

	for (; size > 1; size -=2)
		*dst16++ = *src16++;
	if (size > 0)
		*dst16 = *((volatile const uint8_t *)src16);
}

static inline void stm32_usb_sram_write8(
	volatile void *ptr, uint8_t value)
{
	if ((uintptr_t)ptr & 1)
	{
		volatile uint16_t *ptr16 = (volatile uint16_t *)((uintptr_t)ptr - 1);
		*ptr16 = ((*ptr16 & 0x00FF) | (value << 8));
	}
	else
	{
		volatile uint16_t *ptr16 = (volatile uint16_t *)ptr;
		*ptr16 = ((*ptr16 & 0xFF00) | value);
	}
}

static inline void stm32_usb_sram_write16(
	volatile void *ptr, uint16_t value)
{
	*((volatile uint16_t *)ptr) = value;
}

static inline void stm32_usb_sram_write32(
	volatile void *ptr, uint32_t value)
{
	volatile uint16_t *ptr16 = ptr;
	ptr16[0] = value & 0xFFFF;
	ptr16[1] = value >> 16;
}

static inline void stm32_usb_sram_write64(
	volatile void *ptr, uint64_t value)
{
	volatile uint16_t *ptr16 = ptr;
	ptr16[0] = value & 0xFFFF;
	ptr16[1] = (value >> 16) & 0xFFFF;
	ptr16[2] = (value >> 32) & 0xFFFF;
	ptr16[3] = value >> 48;
}

static inline void stm32_usb_sram_write16_unaligned(
	volatile void *ptr, uint16_t value)
{
	if ((uintptr_t)ptr & 1)
	{
		stm32_usb_sram_write8(ptr, (value & 0xFF));
		ptr = (volatile void *)((uintptr_t)ptr + 1);
		stm32_usb_sram_write8(ptr, (value >> 8));
	}
	else
	{
		stm32_usb_sram_write16(ptr, value);
	}
}

static inline void stm32_usb_sram_write32_unaligned(
	volatile void *ptr, uint32_t value)
{
	if ((uintptr_t)ptr & 1)
	{
		stm32_usb_sram_write8(ptr, (value & 0xFF));
		ptr = (volatile void *)((uintptr_t)ptr + 1);
		stm32_usb_sram_write16(ptr, ((value >> 8) & 0xFFFF));
		ptr = (volatile void *)((uintptr_t)ptr + 2);
		stm32_usb_sram_write8(ptr, (value  >> 24));
	}
	else
	{
		stm32_usb_sram_write32(ptr, value);
	}
}

static inline void stm32_usb_sram_write64_unaligned(
	volatile void *ptr, uint64_t value)
{
	if ((uintptr_t)ptr & 1)
	{
		stm32_usb_sram_write8(ptr, (value & 0xFF));
		ptr = (volatile void *)((uintptr_t)ptr + 1);
		stm32_usb_sram_write16(ptr, ((value >> 8) & 0xFFFF));
		ptr = (volatile void *)((uintptr_t)ptr + 2);
		stm32_usb_sram_write16(ptr, ((value >> 24) & 0xFFFF));
		ptr = (volatile void *)((uintptr_t)ptr + 2);
		stm32_usb_sram_write16(ptr, ((value >> 40) & 0xFFFF));
		ptr = (volatile void *)((uintptr_t)ptr + 2);
		stm32_usb_sram_write8(ptr, (value  >> 56));
	}
	else
	{
		stm32_usb_sram_write64(ptr, value);
	}
}

static inline uint8_t stm32_usb_sram_read8(volatile void *ptr)
{
	return *((volatile uint8_t *)ptr);
}

static inline uint16_t stm32_usb_sram_read16(volatile void *ptr)
{
	return *((volatile uint16_t *)ptr);
}

static inline uint32_t stm32_usb_sram_read32(volatile void *ptr)
{
	volatile uint16_t *ptr16 = ptr;
	return (ptr16[1] << 16) | ptr16[0];
}

static inline uint64_t stm32_usb_sram_read64(volatile void *ptr)
{
	volatile uint16_t *ptr16 = ptr;
	return ((uint64_t)ptr16[3] << 48)
		| ((uint64_t)ptr16[2] << 32)
		| ((uint64_t)ptr16[1] << 16)
		| (uint64_t)ptr16[0];
}

static inline uint16_t stm32_usb_sram_read16_unaligned(volatile void *ptr)
{
	if ((uintptr_t)ptr & 1)
	{
		volatile uint8_t *ptr8 = (volatile uint8_t *)ptr;
		return (ptr8[1] << 8) | ptr8[0];
	}
	else
	{
		return stm32_usb_sram_read16(ptr);
	}
}

static inline uint32_t stm32_usb_sram_read32_unaligned(volatile void *ptr)
{
	if ((uintptr_t)ptr & 1)
	{
		volatile uint8_t *ptr8 = (volatile uint8_t *)ptr;
		volatile uint16_t *ptr_a16 = (volatile uint16_t *)((uintptr_t)ptr + 1);
		return (ptr8[3] << 24) | (*ptr_a16 << 8) | ptr8[0];
	}
	else
	{
		return stm32_usb_sram_read32(ptr);
	}
}

static inline uint64_t stm32_usb_sram_read64_unaligned(volatile void *ptr)
{
	if ((uintptr_t)ptr & 1)
	{
		volatile uint8_t *ptr8 = (volatile uint8_t *)ptr;
		volatile uint16_t *ptr_a16 = (volatile uint16_t *)((uintptr_t)ptr + 1);
		return ((uint64_t)ptr8[7] << 56)
			| ((uint64_t)ptr_a16[2] << 40)
			| ((uint64_t)ptr_a16[1] << 24)
			| ((uint64_t)ptr_a16[0] << 8)
			| (uint64_t)ptr8[0];
	}
	else
	{
		return stm32_usb_sram_read64(ptr);
	}
}


typedef union
{
	struct
	{
		uint16_t count     : 10;
		uint16_t num_block :  5;
		uint16_t blsize    :  1;
	};

	uint16_t mask;
} stm32_usb_bdt_count_t;

typedef struct
{
	union
	{
		uint16_t addr_tx;
		uint16_t addr_tx_0;
		uint16_t addr_rx_0;
	};

	union
	{
		uint16_t              count_tx;
		uint16_t              count_tx_0;
		stm32_usb_bdt_count_t count_rx_0;
	};

	union
	{
		uint16_t addr_rx;
		uint16_t addr_tx_1;
		uint16_t addr_rx_1;
	};

	union
	{
		stm32_usb_bdt_count_t count_rx;
		uint16_t              count_tx_1;
		stm32_usb_bdt_count_t count_rx_1;
	};
} stm32_usb_bdt_t;

static inline uint16_t stm32_usb_bdt_size_next(uint16_t size)
{
	if (size % 2) size++;

	if (size >= 64)
	{
		size += 32;
		size &= ~0x001F;
	}

	return size;
}

static inline stm32_usb_bdt_count_t stm32_usb_bdt_count_encode(uint16_t size)
{
	stm32_usb_bdt_count_t count =
	{
		.blsize    = (size >= 64),
		.num_block = size >> (size < 64 ? 1 : 5),
		.count     = 0,
	};
	return count;
}

static inline stm32_usb_ep_t stm32_usb_ep_invariant(uint8_t index)
{
	stm32_usb_ep_t ep = stm32_usb->ep[index];
	ep.stat_tx = 0;
	ep.dtog_tx = 0;
	ep.ctr_tx  = 1;
	ep.stat_rx = 0;
	ep.dtog_rx = 0;
	ep.ctr_rx  = 1;

	return ep;
}

#endif
