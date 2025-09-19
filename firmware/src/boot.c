#include "init.h"
#include "memcpy.h"

#include <hardware/stm32/rcc.h>
#include <hardware/stm32/syscfg.h>
#include <hardware/cm4/scb.h>
#include <hardware/cm4/intrinsics.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdnoreturn.h>


void __attribute__((interrupt)) arm_vector_default(void)
{
	cm4_system_reset();
}

void __attribute__((interrupt)) stm32_vector_irq_default(void)
{
	// STM32 Errata 2.1.1
	nop();

	// STM32 Errata 2.1.3
	dsb();
}


#define ARM_VECTOR(x) void __attribute__((weak, interrupt, alias("arm_vector_default"))) arm_vector_##x(void)

ARM_VECTOR(nmi);
ARM_VECTOR(hard_fault);
ARM_VECTOR(mem_manage);
ARM_VECTOR(bus_fault);
ARM_VECTOR(usage_fault);
ARM_VECTOR(svc);
ARM_VECTOR(pend_sv);
ARM_VECTOR(system_tick);

#define STM32_VECTOR_IRQ(x) void __attribute__((weak, interrupt, alias("stm32_vector_irq_default"))) stm32_vector_irq_##x(void)

STM32_VECTOR_IRQ(wwdg);
STM32_VECTOR_IRQ(pvd_pvm);
STM32_VECTOR_IRQ(rtc_tamp_css_lse);
STM32_VECTOR_IRQ(rtc_wkup);
STM32_VECTOR_IRQ(flash);
STM32_VECTOR_IRQ(rcc);
STM32_VECTOR_IRQ(exti_0);
STM32_VECTOR_IRQ(exti_1);
STM32_VECTOR_IRQ(exti_2);
STM32_VECTOR_IRQ(exti_3);
STM32_VECTOR_IRQ(exti_4);
STM32_VECTOR_IRQ(dma1_ch1);
STM32_VECTOR_IRQ(dma1_ch2);
STM32_VECTOR_IRQ(dma1_ch3);
STM32_VECTOR_IRQ(dma1_ch4);
STM32_VECTOR_IRQ(dma1_ch5);
STM32_VECTOR_IRQ(dma1_ch6);
STM32_VECTOR_IRQ(dma1_ch7);
STM32_VECTOR_IRQ(adc1_2);
STM32_VECTOR_IRQ(usb_hp);
STM32_VECTOR_IRQ(usb_lp);
STM32_VECTOR_IRQ(fdcan1_it0);
STM32_VECTOR_IRQ(fdcan1_it1);
STM32_VECTOR_IRQ(exti9_5);
STM32_VECTOR_IRQ(tim1_brk_tim15);
STM32_VECTOR_IRQ(tim1_up_tim16);
STM32_VECTOR_IRQ(tim1_trg_com_dir_idx_tim17);
STM32_VECTOR_IRQ(tim1_cc);
STM32_VECTOR_IRQ(tim2);
STM32_VECTOR_IRQ(tim3);
STM32_VECTOR_IRQ(tim4);
STM32_VECTOR_IRQ(i2c1_ev);
STM32_VECTOR_IRQ(i2c1_er);
STM32_VECTOR_IRQ(i2c2_ev);
STM32_VECTOR_IRQ(i2c2_er);
STM32_VECTOR_IRQ(spi1);
STM32_VECTOR_IRQ(spi2);
STM32_VECTOR_IRQ(usart1);
STM32_VECTOR_IRQ(usart2);
STM32_VECTOR_IRQ(usart3);
STM32_VECTOR_IRQ(exti15_10);
STM32_VECTOR_IRQ(rtc_alarm);
STM32_VECTOR_IRQ(usb_wakeup);
STM32_VECTOR_IRQ(tim8_brk_ter_ierr);
STM32_VECTOR_IRQ(tim8_up);
STM32_VECTOR_IRQ(tim8_trg_com_dir_idx);
STM32_VECTOR_IRQ(tim8_cc);
STM32_VECTOR_IRQ(adc3);
STM32_VECTOR_IRQ(fsmc);
STM32_VECTOR_IRQ(lptim1);
STM32_VECTOR_IRQ(tim5);
STM32_VECTOR_IRQ(spi3);
STM32_VECTOR_IRQ(uart4);
STM32_VECTOR_IRQ(uart5);
STM32_VECTOR_IRQ(tim6_dacunder);
STM32_VECTOR_IRQ(tim7_dacunder);
STM32_VECTOR_IRQ(dma2_ch1);
STM32_VECTOR_IRQ(dma2_ch2);
STM32_VECTOR_IRQ(dma2_ch3);
STM32_VECTOR_IRQ(dma2_ch4);
STM32_VECTOR_IRQ(dma2_ch5);
STM32_VECTOR_IRQ(adc4);
STM32_VECTOR_IRQ(adc5);
STM32_VECTOR_IRQ(ucpd1);
STM32_VECTOR_IRQ(comp1_2_3);
STM32_VECTOR_IRQ(comp4_5_6);
STM32_VECTOR_IRQ(comp7);
STM32_VECTOR_IRQ(hrtim_master);
STM32_VECTOR_IRQ(hrtim_tima);
STM32_VECTOR_IRQ(hrtim_timb);
STM32_VECTOR_IRQ(hrtim_timc);
STM32_VECTOR_IRQ(hrtim_timd);
STM32_VECTOR_IRQ(hrtim_time);
STM32_VECTOR_IRQ(hrtim_tim_flt);
STM32_VECTOR_IRQ(hrtim_timf);
STM32_VECTOR_IRQ(crs);
STM32_VECTOR_IRQ(sai);
STM32_VECTOR_IRQ(tim20_brk_terr_ierr);
STM32_VECTOR_IRQ(tim20_up);
STM32_VECTOR_IRQ(tim20_trg_com_dir_idx);
STM32_VECTOR_IRQ(tim20_cc);
STM32_VECTOR_IRQ(fpu);
STM32_VECTOR_IRQ(i2c4_ev);
STM32_VECTOR_IRQ(i2c4_er);
STM32_VECTOR_IRQ(spi4);
STM32_VECTOR_IRQ(aes);
STM32_VECTOR_IRQ(fdcan2_it0);
STM32_VECTOR_IRQ(fdcan2_it1);
STM32_VECTOR_IRQ(fdcan3_it0);
STM32_VECTOR_IRQ(fdcan3_it1);
STM32_VECTOR_IRQ(rng);
STM32_VECTOR_IRQ(lpuart);
STM32_VECTOR_IRQ(i2c3_ev);
STM32_VECTOR_IRQ(i2c3_er);
STM32_VECTOR_IRQ(dmamux_ovr);
STM32_VECTOR_IRQ(quadspi);
STM32_VECTOR_IRQ(dma1_ch8);
STM32_VECTOR_IRQ(dma2_ch6);
STM32_VECTOR_IRQ(dma2_ch7);
STM32_VECTOR_IRQ(dma2_ch8);
STM32_VECTOR_IRQ(cordic);
STM32_VECTOR_IRQ(fmac);


extern uint32_t _linker_script__stack_end;
extern uint32_t _linker_script__data_begin;
extern uint32_t _linker_script__data_end;
extern uint32_t _linker_script__data_flash;
extern uint32_t _linker_script__bss_begin;
extern uint32_t _linker_script__bss_end;

_Noreturn void arm_vector_reset(void)
	__attribute__((section(".text.arm_vector_reset")));

void (* const arm_vector_table[])(void)
	__attribute__((section(".arm_vector_table")))
	__attribute__((used)) =
{
	(void*)&_linker_script__stack_end,
	arm_vector_reset,

	arm_vector_nmi,
	arm_vector_hard_fault,
	arm_vector_mem_manage,
	arm_vector_bus_fault,
	arm_vector_usage_fault,
	NULL,
	NULL,
	NULL,
	NULL,
	arm_vector_svc,
	NULL,
	NULL,
	arm_vector_pend_sv,
	arm_vector_system_tick,

	stm32_vector_irq_wwdg,
	stm32_vector_irq_pvd_pvm,
	stm32_vector_irq_rtc_tamp_css_lse,
	stm32_vector_irq_rtc_wkup,
	stm32_vector_irq_flash,
	stm32_vector_irq_rcc,
	stm32_vector_irq_exti_0,
	stm32_vector_irq_exti_1,
	stm32_vector_irq_exti_2,
	stm32_vector_irq_exti_3,
	stm32_vector_irq_exti_4,
	stm32_vector_irq_dma1_ch1,
	stm32_vector_irq_dma1_ch2,
	stm32_vector_irq_dma1_ch3,
	stm32_vector_irq_dma1_ch4,
	stm32_vector_irq_dma1_ch5,
	stm32_vector_irq_dma1_ch6,
	stm32_vector_irq_dma1_ch7,
	stm32_vector_irq_adc1_2,
	stm32_vector_irq_usb_hp,
	stm32_vector_irq_usb_lp,
	stm32_vector_irq_fdcan1_it0,
	stm32_vector_irq_fdcan1_it1,
	stm32_vector_irq_exti9_5,
	stm32_vector_irq_tim1_brk_tim15,
	stm32_vector_irq_tim1_up_tim16,
	stm32_vector_irq_tim1_trg_com_dir_idx_tim17,
	stm32_vector_irq_tim1_cc,
	stm32_vector_irq_tim2,
	stm32_vector_irq_tim3,
	stm32_vector_irq_tim4,
	stm32_vector_irq_i2c1_ev,
	stm32_vector_irq_i2c1_er,
	stm32_vector_irq_i2c2_ev,
	stm32_vector_irq_i2c2_er,
	stm32_vector_irq_spi1,
	stm32_vector_irq_spi2,
	stm32_vector_irq_usart1,
	stm32_vector_irq_usart2,
	stm32_vector_irq_usart3,
	stm32_vector_irq_exti15_10,
	stm32_vector_irq_rtc_alarm,
	stm32_vector_irq_usb_wakeup,
	stm32_vector_irq_tim8_brk_ter_ierr,
	stm32_vector_irq_tim8_up,
	stm32_vector_irq_tim8_trg_com_dir_idx,
	stm32_vector_irq_tim8_cc,
	stm32_vector_irq_adc3,
	stm32_vector_irq_fsmc,
	stm32_vector_irq_lptim1,
	stm32_vector_irq_tim5,
	stm32_vector_irq_spi3,
	stm32_vector_irq_uart4,
	stm32_vector_irq_uart5,
	stm32_vector_irq_tim6_dacunder,
	stm32_vector_irq_tim7_dacunder,
	stm32_vector_irq_dma2_ch1,
	stm32_vector_irq_dma2_ch2,
	stm32_vector_irq_dma2_ch3,
	stm32_vector_irq_dma2_ch4,
	stm32_vector_irq_dma2_ch5,
	stm32_vector_irq_adc4,
	stm32_vector_irq_adc5,
	stm32_vector_irq_ucpd1,
	stm32_vector_irq_comp1_2_3,
	stm32_vector_irq_comp4_5_6,
	stm32_vector_irq_comp7,
	stm32_vector_irq_hrtim_master,
	stm32_vector_irq_hrtim_tima,
	stm32_vector_irq_hrtim_timb,
	stm32_vector_irq_hrtim_timc,
	stm32_vector_irq_hrtim_timd,
	stm32_vector_irq_hrtim_time,
	stm32_vector_irq_hrtim_tim_flt,
	stm32_vector_irq_hrtim_timf,
	stm32_vector_irq_crs,
	stm32_vector_irq_sai,
	stm32_vector_irq_tim20_brk_terr_ierr,
	stm32_vector_irq_tim20_up,
	stm32_vector_irq_tim20_trg_com_dir_idx,
	stm32_vector_irq_tim20_cc,
	stm32_vector_irq_fpu,
	stm32_vector_irq_i2c4_ev,
	stm32_vector_irq_i2c4_er,
	stm32_vector_irq_spi4,
	stm32_vector_irq_aes,
	stm32_vector_irq_fdcan2_it0,
	stm32_vector_irq_fdcan2_it1,
	stm32_vector_irq_fdcan3_it0,
	stm32_vector_irq_fdcan3_it1,
	stm32_vector_irq_rng,
	stm32_vector_irq_lpuart,
	stm32_vector_irq_i2c3_ev,
	stm32_vector_irq_i2c3_er,
	stm32_vector_irq_dmamux_ovr,
	stm32_vector_irq_quadspi,
	stm32_vector_irq_dma1_ch8,
	stm32_vector_irq_dma2_ch6,
	stm32_vector_irq_dma2_ch7,
	stm32_vector_irq_dma2_ch8,
	stm32_vector_irq_cordic,
	stm32_vector_irq_fmac,
};

_Noreturn void arm_vector_reset(void)
{
	// STM32G47x Errata 2.2.7
	__asm__ volatile (
		"mov r0, #0x20000000\n"
		"ldr r1, [r0]\n"
		"add r0, #0x00008000\n"
		"ldr r1, [r0]\n"
		"add r0, #0x00008000\n"
		"ldr r1, [r0]\n"
		"add r0, #0x00004000\n"
		"ldr r1, [r0]\n"
		"mov r0, #0x10000000\n"
		"ldr r1, [r0]\n" ::: "r0", "r1");

#ifndef NDEBUG
	while (true)
#endif
	{
		cm4_scb->vtor.mask = (uint32_t)arm_vector_table;
		stm32_syscfg->memrmp.mem_mode = STM32_SYSCFG_MEM_MODE_SRAM1;

		// Enable DMA
		stm32_rcc_ahb1_t ahb1en = { .mask = stm32_rcc->ahb1en.mask };
		ahb1en.dma1    = true;
		ahb1en.dmamux1 = true;
		stm32_rcc->ahb1en.mask = ahb1en.mask;

		gbfw_memcpy_init();

		memcpy(
			&_linker_script__data_begin,
			&_linker_script__data_flash,
			((uintptr_t)&_linker_script__data_end
				- (uintptr_t)&_linker_script__data_begin));

		memset(&_linker_script__bss_begin, 0x00,
			((uintptr_t)&_linker_script__bss_end
				- (uintptr_t)&_linker_script__bss_begin));

		gbfw_init();
	}
}
