/*
 * Author: Aurelio Colosimo, 2022
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <kim.h>
#include <cpu.h>
#include <reg.h>
#include <log.h>

#define SYSTICKS_FREQ 1000

extern void board_init(u32 *cpu_freq, u32 *ahb_freq, u32 *apb_freq);

extern void isr_none(void);

/* dummy USART behavior: echo char */
void attr_weak isr_usart1(void)
{
	wr32(R_USART1_DR, rd32(R_USART1_DR));
}

void attr_weak isr_usart2(void)
{
	wr32(R_USART2_DR, rd32(R_USART2_DR));
}

void attr_weak isr_usart6(void)
{
	wr32(R_USART6_DR, rd32(R_USART6_DR));
}

static const void *attr_isrv_irq _isrv_irq[] = {
	/* STM32F407 peripheral IRQs */
	isr_none, /* WWDG */
	isr_none, /* PVD */
	isr_none, /* TAMP_STAMP */
	isr_none, /* RTC_WKUP */
	isr_none, /* FLASH */
	isr_none, /* RCC */
	isr_none, /* EXTI0 */
	isr_none, /* EXTI1 */
	isr_none, /* EXTI2 */
	isr_none, /* EXTI3 */
	isr_none, /* EXTI4 */
	isr_none, /* DMA1_Stream0 */
	isr_none, /* DMA1_Stream1 */
	isr_none, /* DMA1_Stream2 */
	isr_none, /* DMA1_Stream3 */
	isr_none, /* DMA1_Stream4 */
	isr_none, /* DMA1_Stream5 */
	isr_none, /* DMA1_Stream6 */
	isr_none, /* ADC */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* EXTI9_5 */
	isr_none, /* TIM1_BRK_TIM9 */
	isr_none, /* TIM1_UP */
	isr_none, /* TIM1_TRG_COM_TIM11 */
	isr_none, /* TIM1_CC */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* I2C1_EV */
	isr_none, /* I2C1_ER */
	isr_none, /* I2C2_EV */
	isr_none, /* I2C2_ER */
	isr_none, /* SPI1 */
	isr_none, /* SPI2 */
	isr_usart1, /* USART1 */
	isr_usart2, /* USART2 */
	isr_none, /* Reserved */
	isr_none, /* EXTI15_10 */
	isr_none, /* EXTI17_RTC_Alarm */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* DMA1_Stream7 */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* TIM5 */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* TIM6_DAC */
	isr_none, /* Reserved */
	isr_none, /* DMA2_Stream0 */
	isr_none, /* DMA2_Stream1 */
	isr_none, /* DMA2_Stream2 */
	isr_none, /* DMA2_Stream3 */
	isr_none, /* DMA2_Stream4 */
	isr_none, /* Reserved */
	isr_none, /* EXTI19 */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* DMA2_Stream5 */
	isr_none, /* DMA2_Stream6 */
	isr_none, /* DMA2_Stream7 */
	isr_usart6, /* UART6 */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* EXTI20 */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* HASH_RNG */
	isr_none, /* FPU */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* SPI5 */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* I2C4_EV */
	isr_none, /* I2C4_ER */
	isr_none, /* LPTIM1_EXTI23 */
};

u32 attr_weak k_ticks_freq(void)
{
	return SYSTICKS_FREQ;
}

void attr_used init(void)
{
	u32 cpu_freq, ahb_freq, apb_freq;

	/* Init board */
	board_init(&cpu_freq, &ahb_freq, &apb_freq);

	/* Init system ticks */
	wr32(R_SYST_RVR, cpu_freq / SYSTICKS_FREQ);
	wr32(R_SYST_CVR, 0);
	wr32(R_SYST_CSR, BIT0 | BIT1 | BIT2);

	log("ahb freq is %d\n", (uint)ahb_freq);

	/* Skip to main */
	k_main();
}
