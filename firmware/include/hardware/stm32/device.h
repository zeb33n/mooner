#ifndef STM32_DEVICE_H
#define STM32_DEVICE_H

#include <stdint.h>
#include <hardware/stm32/mmap.h>


static const uint32_t * const STM32_PACKAGE    = (uint32_t *)(STM32_MMAP_ENGINEERING_BYTES + 0x00);
static const uint32_t * const STM32_UID        = (uint32_t *)(STM32_MMAP_ENGINEERING_BYTES + 0x90);

#endif
