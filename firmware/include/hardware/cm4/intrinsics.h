#ifndef CM4_INTRINSICS_H
#define CM4_INTRINSICS_H

static inline void nop(void) { __asm__ volatile ("nop"); }
static inline void wfi(void) { __asm__ volatile ("wfi"); }
static inline void wfe(void) { __asm__ volatile ("wfe"); }

#define cpsid(flags) __asm__ volatile ("cpsid " #flags)
#define cpsie(flags) __asm__ volatile ("cpsie " #flags)

static inline void dsb(void) { __asm__ volatile ("dsb 0xf":::"memory"); }

#define wfi_while(cond) while (1) { cpsid(i); if (cond) wfi(); else break; cpsie(i); } cpsie(i)

static inline void dummy_read(volatile void *ptr)
{
	volatile uint32_t dummy;
	__asm__ volatile ("ldr %0, [%1]" : "=r"(dummy) : "r"(ptr));
	(void)dummy;
}

static inline uint32_t ror(uint32_t a, uint32_t b)
{
	uint32_t r;
	__asm__ ("ror %0, %1, %2" : "=r"(r) : "r"(a), "r"(b));
	return r;
}

static inline uint32_t rbit(uint32_t a)
{
	uint32_t r;
	__asm__ ("rbit %0, %1" : "=r"(r) : "r"(a));
	return r;
}

static inline uint32_t clz(uint32_t a)
{
	uint32_t r;
	__asm__ ("clz %0, %1" : "=r"(r) : "r"(a));
	return r;
}

static inline uint32_t ctz(uint32_t a)
{
	return clz(rbit(a));
}

static inline uint32_t clo(uint32_t a)
{
	return clz(~a);
}

static inline uint32_t cto(uint32_t a)
{
	return ctz(~a);
}

#endif
