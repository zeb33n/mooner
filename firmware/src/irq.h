#ifndef GBFW_IRQ_H
#define GBFW_IRQ_H

#include <stdbool.h>
#include <stdint.h>
#include <hardware/stm32/irq.h>

// Kept in one-place to ease understanding.
#define GBFW_IRQ_PRIORITY_POWER_SWITCH STM32_IRQ_PRIORITY( 0)
#define GBFW_IRQ_PRIORITY_AUDIO        STM32_IRQ_PRIORITY( 1)
#define GBFW_IRQ_PRIORITY_DISPLAY      STM32_IRQ_PRIORITY( 2)
#define GBFW_IRQ_PRIORITY_STORAGE      STM32_IRQ_PRIORITY( 3)
#define GBFW_IRQ_PRIORITY_DELAY        STM32_IRQ_PRIORITY( 4)
#define GBFW_IRQ_PRIORITY_HOME         STM32_IRQ_PRIORITY( 5)
#define GBFW_IRQ_PRIORITY_USB          STM32_IRQ_PRIORITY( 6)
#define GBFW_IRQ_PRIORITY_USB_DETECT   STM32_IRQ_PRIORITY(13)
#define GBFW_IRQ_PRIORITY_TASK         STM32_IRQ_PRIORITY(14)
#define GBFW_IRQ_PRIORITY_MEMCPY       STM32_IRQ_PRIORITY(15)

void gbfw_irq_enable(uint8_t irq, bool rising, bool falling, uint8_t priority);

#endif
