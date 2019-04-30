/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <kim.h>
#include <cpu.h>
#include <reg.h>
#include <log.h>

#define SYSTICKS_FREQ 100

extern int putchar(int c);
extern void board_init(u32 *cpu_freq);

extern void isr_none(void);

extern void uart_init(void);

/* dummy USART behavior: echo char */
void attr_weak isr_uart1(void)
{
	wr32(R_USART1_TDR, rd32(R_USART1_RDR));
}

void attr_weak isr_uart2(void)
{
	wr32(R_USART2_TDR, rd32(R_USART2_RDR));
}

static const void *attr_isrv_irq _isrv_irq[] = {

	/* STM32F030 peripheral IRQs */
	isr_none,	/* WWDG */
	0,			/* Reserved */
	isr_none,	/* RTC */
	isr_none,	/* FLASH */
	isr_none,	/* RCC */
	isr_none,	/* EXTI0_1 */
	isr_none,	/* EXTI2_3 */
	isr_none,	/* EXTI4_15 */
	0,			/* Reserved */
	isr_none,	/* DMA_CH1 */
	isr_none,	/* DMA_CH2_3 */
	isr_none,	/* DMA_CH4_5 */
	isr_none,	/* ADC */
	isr_none,	/* TIM1_BRK_UP_TRG_COM */
	isr_none,	/* TIM1_CC */
	0,			/* Reserved */
	isr_none,	/* TIM3 */
	isr_none,	/* TIM6 */
	0,			/* Reserved */
	isr_none,	/* TIM14 */
	isr_none,	/* TIM15 */
	isr_none,	/* TIM16 */
	isr_none,	/* TIM17 */
	isr_none,	/* I2C1 */
	isr_none,	/* I2C2 */
	isr_none,	/* SPI1 */
	isr_none,	/* SPI2 */
	isr_uart1,	/* USART1 */
	isr_uart2,	/* USART2 */
	isr_none,	/* USART3_4_5_6 */
	0,			/* Reserved */
	isr_none,	/* USB */
};

u32 attr_weak k_ticks_freq(void)
{
	return SYSTICKS_FREQ;
}

void attr_used init(void)
{
	u32 cpu_freq;

	/* Init board */
	board_init(&cpu_freq);

	/* Init system ticks */
	wr32(R_SYST_RVR, cpu_freq / SYSTICKS_FREQ);
	wr32(R_SYST_CVR, 0);
	wr32(R_SYST_CSR, BIT0 | BIT1 | BIT2);

	uart_init();

	/* Skip to main */
	k_main();
}
