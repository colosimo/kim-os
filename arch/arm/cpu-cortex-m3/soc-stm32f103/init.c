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
#include <uart.h>

#define SYSTICKS_FREQ 1000

extern int putchar(int c);
extern void board_init(u32 *cpu_freq);

extern void isr_none(void);

/* dummy USART behavior: echo char */
void attr_weak isr_uart1(void)
{
	wr32(R_USART1_DR, rd32(R_USART1_DR));
}

void attr_weak isr_uart2(void)
{
	wr32(R_USART2_DR, rd32(R_USART2_DR));
}

void attr_weak isr_uart3(void)
{
	wr32(R_USART3_DR, rd32(R_USART3_DR));
}

static const void *attr_isrv_irq _isrv_irq[] = {
	/* STM32F103 peripheral IRQs */
	isr_none, /* WWDG */
	isr_none, /* PVD */
	isr_none, /* Tamper */
	isr_none, /* RTC */
	isr_none, /* FLASH */
	isr_none, /* RCC */
	isr_none, /* EXTI0 */
	isr_none, /* EXTI1 */
	isr_none, /* EXTI2 */
	isr_none, /* EXTI3 */
	isr_none, /* EXTI4 */
	isr_none, /* DMA1_Channel1 */
	isr_none, /* DMA1_Channel2 */
	isr_none, /* DMA1_Channel3 */
	isr_none, /* DMA1_Channel4 */
	isr_none, /* DMA1_Channel5 */
	isr_none, /* DMA1_Channel6 */
	isr_none, /* DMA1_Channel7 */
	isr_none, /* ADC1_2 */
	isr_none, /* CAN1_TX */
	isr_none, /* CAN1_RX0 */
	isr_none, /* CAN1_RX1 */
	isr_none, /* CAN1_SCE */
	isr_none, /* EXTI9_5 */
	isr_none, /* TIM1_BRK */
	isr_none, /* TIM1_UP */
	isr_none, /* TIM1_TRG_COM */
	isr_none, /* TIM1_CC */
	isr_none, /* TIM2 */
	isr_none, /* TIM3 */
	isr_none, /* TIM4 */
	isr_none, /* I2C1_EV */
	isr_none, /* I2C1_ER */
	isr_none, /* I2C2_EV */
	isr_none, /* I2C2_ER */
	isr_none, /* SPI1 */
	isr_none, /* SPI2 */
	isr_uart1, /* USART1 */
	isr_uart2, /* USART2 */
	isr_uart3, /* USART3 */
	isr_none, /* EXTI15_10 */
	isr_none, /* RTC_Alarm */
	isr_none, /* OTG_FS WKUP */
	0,		  /* Reserved */
	0,		  /* Reserved */
	0,		  /* Reserved */
	0,		  /* Reserved */
	0,		  /* Reserved */
	0,		  /* Reserved */
	0,		  /* Reserved */
	isr_none, /* TIM5 */
	isr_none, /* SPI3 */
	isr_none, /* UART4 */
	isr_none, /* UART5 */
	isr_none, /* TIM6 */
	isr_none, /* TIM7 */
	isr_none, /* DMA2_Channel1 */
	isr_none, /* DMA2_Channel2 */
	isr_none, /* DMA2_Channel3 */
	isr_none, /* DMA2_Channel4 */
	isr_none, /* DMA2_Channel5 */
	isr_none, /* ETH */
	isr_none, /* ETH_WKUP */
	isr_none, /* CAN2_TX */
	isr_none, /* CAN2_RX0 */
	isr_none, /* CAN2_RX1 */
	isr_none, /* CAN2_SCE */
	isr_none, /* OTG_FS */
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

	uart_init(cpu_freq);

	/* Skip to main */
	k_main();
}
