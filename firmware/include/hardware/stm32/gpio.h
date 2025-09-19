#ifndef STM32_GPIO_H
#define STM32_GPIO_H

#include <stdbool.h>
#include <stdint.h>
#include <hardware/stm32/mmap.h>


typedef enum
{
	STM32_GPIO_PORT_A,
	STM32_GPIO_PORT_B,
	STM32_GPIO_PORT_C,
	STM32_GPIO_PORT_D,
	STM32_GPIO_PORT_E,
	STM32_GPIO_PORT_F,
} stm32_gpio_port_e;

typedef enum
{
	STM32_GPIO_MODE_INPUT     = 0,
	STM32_GPIO_MODE_OUTPUT    = 1,
	STM32_GPIO_MODE_ALTERNATE = 2,
	STM32_GPIO_MODE_ANALOG    = 3,
} stm32_gpio_mode_e;

typedef enum
{
	STM32_GPIO_OT_PUSH_PULL  = 0,
	STM32_GPIO_OT_OPEN_DRAIN = 1,
} stm32_gpio_ot_e;

typedef enum
{
	STM32_GPIO_SPEED_LOW       = 0,
	STM32_GPIO_SPEED_MEDIUM    = 1,
	STM32_GPIO_SPEED_HIGH      = 2,
	STM32_GPIO_SPEED_VERY_HIGH = 3,
} stm32_gpio_speed_e;

typedef enum
{
	STM32_GPIO_PULL_NONE = 0,
	STM32_GPIO_PULL_UP   = 1,
	STM32_GPIO_PULL_DOWN = 2,
} stm32_gpio_pull_e;


typedef union
{
	struct
	{
		uint32_t lck      : 16;
		uint32_t lckk     :  1;
		uint32_t res31_17 : 15;
	};

	uint32_t mask;
} stm32_gpio_lck_t;

typedef struct
{
	uint32_t         mode;
	uint32_t         otype;
	uint32_t         ospeed;
	uint32_t         pull;
	uint32_t         id;
	uint32_t         od;
	uint32_t         bsr;
	stm32_gpio_lck_t lck;
	uint32_t         afl;
	uint32_t         afh;
	uint32_t         br;
	const uint32_t   res255_11[245];
} stm32_gpio_t;

static volatile stm32_gpio_t * const stm32_gpio = (volatile stm32_gpio_t *)STM32_MMAP_IOPORT;


static inline void stm32_gpio_port_write(stm32_gpio_port_e port, uint16_t data)
{
	stm32_gpio[port].od = data;
}

static inline uint16_t gpio_port_read(stm32_gpio_port_e port)
{
	return stm32_gpio[port].id;
}


static inline void stm32_gpio_pin_set(stm32_gpio_port_e port, uint8_t pin)
{
	stm32_gpio[port].bsr = 1U << pin;
}

static inline void stm32_gpio_pin_clear(stm32_gpio_port_e port, uint8_t pin)
{
	stm32_gpio[port].br = 1U << pin;
}

static inline void stm32_gpio_pin_write(stm32_gpio_port_e port, uint8_t pin, bool bit)
{
	if (bit)
		stm32_gpio_pin_set(port, pin);
	else
		stm32_gpio_pin_clear(port, pin);
}

static inline bool stm32_gpio_pin_read(stm32_gpio_port_e port, uint8_t pin)
{
	return (stm32_gpio[port].id >> pin) & 1;
}


static inline void reg32_write_elem(volatile uint32_t *reg, uint8_t size, uint8_t elem, uint32_t data)
{
	*reg = (*reg & ~(((1U << size) - 1) << (elem * size))) | (data << (elem * size));
}

static inline void stm32_gpio_pin_mode_set(stm32_gpio_port_e port, uint8_t pin, stm32_gpio_mode_e mode)
{
	reg32_write_elem(&stm32_gpio[port].mode, 2, pin, mode);
}

static inline void stm32_gpio_pin_ot_set(stm32_gpio_port_e port, uint8_t pin, stm32_gpio_ot_e ot)
{
	reg32_write_elem(&stm32_gpio[port].otype, 1, pin, ot);
}

static inline void stm32_gpio_pin_speed_set(stm32_gpio_port_e port, uint8_t pin, stm32_gpio_speed_e speed)
{
	reg32_write_elem(&stm32_gpio[port].ospeed, 2, pin, speed);
}

static inline void stm32_gpio_pin_pull_set(stm32_gpio_port_e port, uint8_t pin, stm32_gpio_pull_e pull)
{
	reg32_write_elem(&stm32_gpio[port].pull, 2, pin, pull);
}

static inline void stm32_gpio_pin_af_set(stm32_gpio_port_e port, uint8_t pin, uint8_t af)
{
	if (pin < 8)
		reg32_write_elem(&stm32_gpio[port].afl, 4, pin, af);
	else
		reg32_write_elem(&stm32_gpio[port].afh, 4, (pin - 8), af);
}

static inline void stm32_gpio_pin_lock(stm32_gpio_port_e port, uint8_t pin)
{
	stm32_gpio[port].lck.mask |= 1U << pin;
}

#endif
