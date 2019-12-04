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

void attr_weak isr_usart3(void)
{
	wr32(R_USART3_DR, rd32(R_USART3_DR));
}

void attr_weak isr_otgfs(void)
{
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
	isr_none, /* CAN1_TX */
	isr_none, /* CAN1_RX0 */
	isr_none, /* CAN1_RX1 */
	isr_none, /* CAN1_SCE */
	isr_none, /* EXTI9_5 */
	isr_none, /* TIM1_BRK_TIM9 */
	isr_none, /* TIM1_UP_TIM10 */
	isr_none, /* TIM1_TRG_COM_TIM11 */
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
	isr_usart1, /* USART1 */
	isr_usart2, /* USART2 */
	isr_usart3, /* USART3 */
	isr_none, /* EXTI15_10 */
	isr_none, /* RTC_Alarm */
	isr_none, /* OTG_FS WKUP */
	isr_none, /* TIM8_BRK_TIM12 */
	isr_none, /* TIM8_UP_TIM13 */
	isr_none, /* TIM8_TRG_COM_TIM14 */
	isr_none, /* TIM8_CC */
	isr_none, /* DMA1_Stream7 */
	isr_none, /* FSMC */
	isr_none, /* SDIO */
	isr_none, /* TIM5 */
	isr_none, /* SPI3 */
	isr_none, /* UART4 */
	isr_none, /* UART5 */
	isr_none, /* TIM6_DAC */
	isr_none, /* TIM7 */
	isr_none, /* DMA2_Stream0 */
	isr_none, /* DMA2_Stream1 */
	isr_none, /* DMA2_Stream2 */
	isr_none, /* DMA2_Stream3 */
	isr_none, /* DMA2_Stream4 */
	isr_none, /* ETH */
	isr_none, /* ETH_WKUP */
	isr_none, /* CAN2_TX */
	isr_none, /* CAN2_RX0 */
	isr_none, /* CAN2_RX1 */
	isr_none, /* CAN2_SCE */
	isr_otgfs, /* OTG_FS */
	isr_none, /* DMA2_Stream5 */
	isr_none, /* DMA2_Stream6 */
	isr_none, /* DMA2_Stream7 */
	isr_none, /* UART6 */
	isr_none, /* I2C3_EV */
	isr_none, /* I2C3_ER */
	isr_none, /* OTG_HS_EP1_OUT */
	isr_none, /* OTG_HS_EP1_IN */
	isr_none, /* OTG_HS_WKUP */
	isr_none, /* OTG_HS */
	isr_none, /* DCMI */
	isr_none, /* CRYP */
	isr_none, /* HASH_RNG */
	isr_none, /* FPU */
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

	uart_init();

	/* Skip to main */
	k_main();
}
