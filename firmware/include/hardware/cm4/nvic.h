#ifndef CM4_NVIC_H
#define CM4_NVIC_H

#include <stdint.h>

static volatile uint32_t * const CM4_NVIC_ISER = (volatile uint32_t *)0xE000E100;
static volatile uint32_t * const CM4_NVIC_ICER = (volatile uint32_t *)0xE000E180;
static volatile uint32_t * const CM4_NVIC_ISPR = (volatile uint32_t *)0xE000E200;
static volatile uint32_t * const CM4_NVIC_ICPR = (volatile uint32_t *)0xE000E280;
static volatile uint32_t * const CM4_NVIC_IABR = (volatile uint32_t *)0xE000E300;
static volatile uint32_t * const CM4_NVIC_IPR  = (volatile uint32_t *)0xE000E400;
static volatile uint32_t * const CM4_NVIC_STIR = (volatile uint32_t *)0xE000F000;

#define CM4_NVIC_PRIORITY_HIGHEST   0
#define CM4_NVIC_PRIORITY_LOWEST  255

static inline uint8_t cm4_nvic_interrupt_priority_get(
	uint8_t interrupt)
{
	uint8_t word = interrupt / 4;
	uint8_t byte = interrupt % 4;

	uint32_t mask = CM4_NVIC_IPR[word];
	return ((mask >> (byte * 8)) & 0xFF);
}

static inline void cm4_nvic_interrupt_priority_set(
	uint8_t interrupt, uint8_t priority)
{
	uint8_t word = interrupt / 4;
	uint8_t byte = interrupt % 4;

	uint32_t mask = CM4_NVIC_IPR[word];
	mask &= ~(0xFF << (byte * 8));
	mask |= priority << (byte * 8);
	CM4_NVIC_IPR[word] = mask;
}

static inline void cm4_nvic_interrupt_enable(uint8_t interrupt)
{
	CM4_NVIC_ISER[interrupt / 32] |= 1U << (interrupt % 32);
}

static inline void cm4_nvic_interrupt_disable(uint8_t interrupt)
{
	CM4_NVIC_ICER[interrupt / 32] |= 1U << (interrupt % 32);
}

static inline void cm4_nvic_interrupt_trigger(uint8_t interrupt)
{
	*CM4_NVIC_STIR = interrupt;
}

#endif
