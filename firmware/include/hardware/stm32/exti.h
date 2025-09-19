#ifndef STM32_EXTI_H
#define STM32_EXTI_H

#include <stdint.h>
#include <stdbool.h>
#include <hardware/stm32/mmap.h>


typedef struct
{
	uint32_t im1;
	uint32_t em1;
	uint32_t rts1;
	uint32_t fts1;
	uint32_t swie1;
	uint32_t p1;
	uint32_t res7_6[2];
	uint32_t im2;
	uint32_t em2;
	uint32_t rts2;
	uint32_t fts2;
	uint32_t swie2;
	uint32_t p2;
	uint32_t res255_14[242];
} stm32_exti_t;

static volatile stm32_exti_t * const stm32_exti = (volatile stm32_exti_t *)STM32_MMAP_EXTI;


static inline volatile uint32_t *stm32_exti_reg(volatile uint32_t *r, uint8_t index)
{
	return (index < 32 ? r : &r[8]);
}

static inline bool stm32_exti_get(volatile uint32_t *r, uint8_t index)
{
	return ((*stm32_exti_reg(r, index) >> (index % 32)) & 1) != 0;
}

static inline void stm32_exti_set(volatile uint32_t *r, uint8_t index)
{
	*stm32_exti_reg(r, index) |= 1U << (index % 32);
}

static inline void stm32_exti_clear(volatile uint32_t *r, uint8_t index)
{
	*stm32_exti_reg(r, index) &= ~(1U << (index % 32));
}


static inline bool stm32_exti_im_get(uint8_t index)
{
	return stm32_exti_get(&stm32_exti->im1, index);
}

static inline bool stm32_exti_em_get(uint8_t index)
{
	return stm32_exti_get(&stm32_exti->em1, index);
}

static inline bool stm32_exti_rts_get(uint8_t index)
{
	return stm32_exti_get(&stm32_exti->rts1, index);
}

static inline bool stm32_exti_fts_get(uint8_t index)
{
	return stm32_exti_get(&stm32_exti->fts1, index);
}

static inline bool stm32_exti_swie_get(uint8_t index)
{
	return stm32_exti_get(&stm32_exti->swie1, index);
}


static inline void stm32_exti_im_set(uint8_t index)
{
	stm32_exti_set(&stm32_exti->im1, index);
}

static inline void stm32_exti_em_set(uint8_t index)
{
	stm32_exti_set(&stm32_exti->em1, index);
}

static inline void stm32_exti_rts_set(uint8_t index)
{
	stm32_exti_set(&stm32_exti->rts1, index);
}

static inline void stm32_exti_fts_set(uint8_t index)
{
	stm32_exti_set(&stm32_exti->fts1, index);
}

static inline void stm32_exti_swie_set(uint8_t index)
{
	stm32_exti_set(&stm32_exti->swie1, index);
}


static inline void stm32_exti_im_clear(uint8_t index)
{
	stm32_exti_clear(&stm32_exti->im1, index);
}

static inline void stm32_exti_em_clear(uint8_t index)
{
	stm32_exti_clear(&stm32_exti->em1, index);
}

static inline void stm32_exti_rts_clear(uint8_t index)
{
	stm32_exti_clear(&stm32_exti->rts1, index);
}

static inline void stm32_exti_fts_clear(uint8_t index)
{
	stm32_exti_clear(&stm32_exti->fts1, index);
}

static inline void stm32_exti_p_clear(uint8_t index)
{
	stm32_exti_set(&stm32_exti->p1, index);
}

#endif
