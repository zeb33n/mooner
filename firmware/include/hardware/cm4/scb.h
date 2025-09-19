#ifndef CM4_SCB_H
#define CM4_SCB_H

#include <stdint.h>
#include <stdnoreturn.h>


typedef enum
{
	CM4_SCB_CPUID_IMPLEMENTER_ARM       = 0x41,
	CM4_SCB_CPUID_IMPLEMENTER_BROADCOM  = 0x42,
	CM4_SCB_CPUID_IMPLEMENTER_CAVIUM    = 0x43,
	CM4_SCB_CPUID_IMPLEMENTER_DEC       = 0x44,
	CM4_SCB_CPUID_IMPLEMENTER_FUJITSU   = 0x46,
	CM4_SCB_CPUID_IMPLEMENTER_HISILICON = 0x48,
	CM4_SCB_CPUID_IMPLEMENTER_INFINEON  = 0x49,
	CM4_SCB_CPUID_IMPLEMENTER_MOTOROLA  = 0x4D,
	CM4_SCB_CPUID_IMPLEMENTER_NVIDIA    = 0x4E,
	CM4_SCB_CPUID_IMPLEMENTER_APM       = 0x50,
	CM4_SCB_CPUID_IMPLEMENTER_QUALCOMM  = 0x51,
	CM4_SCB_CPUID_IMPLEMENTER_SAMSUNG   = 0x53,
	CM4_SCB_CPUID_IMPLEMENTER_MARVELL   = 0x56,
	CM4_SCB_CPUID_IMPLEMENTER_APPLE     = 0x61,
	CM4_SCB_CPUID_IMPLEMENTER_FARADAY   = 0x66,
	CM4_SCB_CPUID_IMPLEMENTER_INTEL     = 0x69,
	CM4_SCB_CPUID_IMPLEMENTER_PHYTIUM   = 0x70,
	CM4_SCB_CPUID_IMPLEMENTER_AMPERE    = 0xC0,
} cm4_scb_cpuid_implementer_e;

typedef enum
{
	CM4_SCB_CPUID_ARCHITECTURE_ARMV6M   = 0xC,
	CM4_SCB_CPUID_ARCHITECTURE_CONSTANT = 0xF,
} cm4_scb_cpuid_architecture_e;

typedef enum
{
	CM4_SCB_CPUID_PARTNO_CORTEX_M0PLUS = 0xC60,
	CM4_SCB_CPUID_PARTNO_CORTEX_M0     = 0xC20,
	CM4_SCB_CPUID_PARTNO_CORTEX_M3     = 0xC23,
	CM4_SCB_CPUID_PARTNO_CORTEX_M4     = 0xC24,
	CM4_SCB_CPUID_PARTNO_CORTEX_M7     = 0xC27,
	CM4_SCB_CPUID_PARTNO_CORTEX_M23    = 0xD20,
	CM4_SCB_CPUID_PARTNO_CORTEX_M33    = 0xD21,
	CM4_SCB_CPUID_PARTNO_CORTEX_M55    = 0xD22,
	CM4_SCB_CPUID_PARTNO_CORTEX_M85    = 0xD23,
	CM4_SCB_CPUID_PARTNO_CORTEX_M52    = 0xD24,
} cm4_scb_cpuid_partno_e;


typedef union
{
	struct
	{
		uint32_t revision     :  4;
		uint32_t partno       : 12;
		uint32_t architecture :  4;
		uint32_t variant      :  4;
		uint32_t implementer  :  8;
	};

	uint32_t mask;
} cm4_scb_cpuid_t;

typedef union
{
	struct
	{
		const uint32_t vectactive  : 9;
		uint32_t       res10_9     : 2;
		const uint32_t rettobase   : 1;
		const uint32_t vectpending : 6;
		uint32_t       res21_18    : 4;
		const uint32_t isrpending  : 1;
		uint32_t       res24_23    : 2;
		uint32_t       pendstclr   : 1;
		uint32_t       pendset     : 1;
		uint32_t       pendsvclr   : 1;
		uint32_t       pendsvset   : 1;
		uint32_t       res30_29    : 2;
		uint32_t       nmipendset  : 1;
	};

	uint32_t mask;
} cm4_scb_ics_t;

typedef union
{
	struct
	{
		uint32_t res6_0 :  7;
		uint32_t tbloff : 25;
	};

	uint32_t mask;
} cm4_scb_vtor_t;

typedef union
{
	struct
	{
		uint32_t       vectreset     :  1;
		uint32_t       vectclractive :  1;
		uint32_t       sysresetreq   :  1;
		uint32_t       res7_3        :  5;
		uint32_t       prigroup      :  3;
		uint32_t       res14_11      :  4;
		const uint32_t endianess     :  1;
		uint32_t       vectkey       : 16;
	};

	uint32_t mask;
} cm4_scb_airc_t;

#define CM4_SCB_AIRC_VECTKEY     0xFA05
#define CM4_SCB_AIRC_VECTKEYSTAT 0x05FA

typedef union
{
	struct
	{
		uint32_t res0        :  1;
		uint32_t sleeponexit :  1;
		uint32_t sleepdeep   :  1;
		uint32_t res3        :  1;
		uint32_t sevonpend   :  1;
		uint32_t res31_5     : 27;
	};

	uint32_t mask;
} cm4_scb_sc_t;

typedef union
{
	struct
	{
		uint32_t nonbasethrdena :  1;
		uint32_t usersetmpend   :  1;
		uint32_t res2           :  1;
		uint32_t unalign_trp    :  1;
		uint32_t div_0_trp      :  1;
		uint32_t res7_5         :  3;
		uint32_t bfhfnmign      :  1;
		uint32_t stkalign       :  1;
		uint32_t res31_10       : 22;
	};

	uint32_t mask;
} cm4_scb_cc_t;

typedef union
{
	struct
	{
		uint8_t pri_4;
		uint8_t pri_5;
		uint8_t pri_6;
		uint8_t pri_7;
	};

	uint32_t mask;
} cm4_scb_shp1_t;

typedef union
{
	struct
	{
		uint8_t pri_8;
		uint8_t pri_9;
		uint8_t pri_10;
		uint8_t pri_11;
	};

	uint32_t mask;
} cm4_scb_shp2_t;

typedef union
{
	struct
	{
		uint8_t pri_12;
		uint8_t pri_13;
		uint8_t pri_14;
		uint8_t pri_15;
	};

	uint32_t mask;
} cm4_scb_shp3_t;

typedef union
{
	struct
	{
		uint32_t memfaultact    :  1;
		uint32_t busfaultact    :  1;
		uint32_t res2           :  1;
		uint32_t usgfaultact    :  1;
		uint32_t res6_4         :  3;
		uint32_t svcallact      :  1;
		uint32_t monitoract     :  1;
		uint32_t res9           :  1;
		uint32_t pendsvact      :  1;
		uint32_t systickact     :  1;
		uint32_t usgfaultpended :  1;
		uint32_t memfaultpended :  1;
		uint32_t busfaultpended :  1;
		uint32_t svcallpended   :  1;
		uint32_t memfaultena    :  1;
		uint32_t busfaultena    :  1;
		uint32_t usgfaultena    :  1;
		uint32_t res31_19       : 13;
	};

	uint32_t mask;
} cm4_scb_shcs_t;

typedef union
{
	struct
	{
		uint8_t iaccviol  : 1;
		uint8_t daccviol  : 1;
		uint8_t res2      : 1;
		uint8_t munstkerr : 1;
		uint8_t mstkerr   : 1;
		uint8_t mlsperr   : 1;
		uint8_t res6      : 1;
		uint8_t mmarvalid : 1;
	};

	uint8_t mask;
} cm4_scb_mmfs_t;

typedef union
{
	struct
	{
		uint8_t ibuserr     : 1;
		uint8_t preciserr   : 1;
		uint8_t impreciserr : 1;
		uint8_t unstkerr    : 1;
		uint8_t stkerr      : 1;
		uint8_t lsperr      : 1;
		uint8_t res6        : 1;
		uint8_t bfarvalid   : 1;
	};

	uint8_t mask;
} cm4_scb_bfs_t;

typedef union
{
	struct
	{
		uint16_t undefinstr : 1;
		uint16_t invstate   : 1;
		uint16_t invpc      : 1;
		uint16_t nocp       : 1;
		uint16_t res7_4     : 4;
		uint16_t divbyzero  : 1;
		uint16_t unaligned  : 1;
		uint16_t res15_10   : 6;
	};

	uint16_t mask;
} cm4_scb_ufs_t;

typedef union
{
	struct
	{
		cm4_scb_mmfs_t mmfs;
		cm4_scb_bfs_t  bfs;
		cm4_scb_ufs_t  ufs;
	};

	uint32_t mask;
} cm4_scb_cfs_t;

typedef union
{
	struct
	{
		uint32_t res0     :  1;
		uint32_t vecttbl  :  1;
		uint32_t res29_2  : 28;
		uint32_t forced   :  1;
		uint32_t debugevt :  1;
	};

	uint32_t mask;
} cm4_scb_hfs_t;



typedef struct
{
	const cm4_scb_cpuid_t cpuid;
	cm4_scb_ics_t         ics;
	cm4_scb_vtor_t        vtor;
	cm4_scb_airc_t        airc;
	cm4_scb_sc_t          sc;
	cm4_scb_cc_t          cc;
	cm4_scb_shp1_t        shp1;
	cm4_scb_shp2_t        shp2;
	cm4_scb_shp3_t        shp3;
	cm4_scb_shcs_t        shcs;
	cm4_scb_cfs_t         cfs;
	cm4_scb_hfs_t         hfs;
	uint32_t              res12;
	uint32_t              mma;
	uint32_t              bfa;
	uint32_t              afs;
} cm4_scb_t;


static volatile cm4_scb_t * const cm4_scb
	= (volatile cm4_scb_t *)0xE000ED00;

static inline _Noreturn void cm4_system_reset(void)
{
	cm4_scb_airc_t airc = { .mask = cm4_scb->airc.mask };
	airc.vectkey        = CM4_SCB_AIRC_VECTKEYSTAT;
	airc.sysresetreq    = 1;
	cm4_scb->airc.mask  = airc.mask;
	__builtin_unreachable();
}

#endif
