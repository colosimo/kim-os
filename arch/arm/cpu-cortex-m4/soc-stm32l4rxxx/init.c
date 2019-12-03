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
	wr32(R_USART1_TDR, rd32(R_USART1_RDR));
}

void attr_weak isr_usart2(void)
{
	wr32(R_USART2_TDR, rd32(R_USART2_RDR));
}

void attr_weak isr_usart3(void)
{
	wr32(R_USART3_TDR, rd32(R_USART3_RDR));
}

static const void *attr_isrv_irq _isrv_irq[] = {
	/* STM32F407 peripheral IRQs */
	isr_none, /* WWDG */
	isr_none, /* PVD_PVM */
	isr_none, /* RTC_TAMP_STAMP */
	isr_none, /* RTC_WKUP */
	isr_none, /* FLASH */
	isr_none, /* RCC */
	isr_none, /* EXTI0 */
	isr_none, /* EXTI1 */
	isr_none, /* EXTI2 */
	isr_none, /* EXTI3 */
	isr_none, /* EXTI4 */
	isr_none, /* DMA1_CH1 */
	isr_none, /* DMA1_CH2 */
	isr_none, /* DMA1_CH3 */
	isr_none, /* DMA1_CH4 */
	isr_none, /* DMA1_CH5 */
	isr_none, /* DMA1_CH6 */
	isr_none, /* DMA1_CH7 */
	isr_none, /* ADC1 */
	isr_none, /* CAN1_TX */
	isr_none, /* CAN1_RX0 */
	isr_none, /* CAN1_RX1 */
	isr_none, /* CAN1_SCE */
	isr_none, /* EXTI9_5 */
	isr_none, /* TIM1_BRK_TIM15 */
	isr_none, /* TIM1_UP_TIM16 */
	isr_none, /* TIM1_TRG_COM_TIM17 */
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
	isr_none, /* RTC_ALARM */
	isr_none, /* DFSDM1_FLT3 */
	isr_none, /* TIM8_BRK */
	isr_none, /* TIM8_UP */
	isr_none, /* TIM8_TRG_COM */
	isr_none, /* TIM8_CC */
	NULL,     /* Reserved */
	isr_none, /* FMC */
	isr_none, /* SDMMC1 */
	isr_none, /* TIM5 */
	isr_none, /* SPI3 */
	isr_none, /* UART4 */
	isr_none, /* UART5 */
	isr_none, /* TIM6_DACUNDER */
	isr_none, /* TIM7 */
	isr_none, /* DMA2_CH1 */
	isr_none, /* DMA2_CH2 */
	isr_none, /* DMA2_CH3 */
	isr_none, /* DMA2_CH4 */
	isr_none, /* DMA2_CH5 */
	isr_none, /* DFSDM1_FLT0 */
	isr_none, /* DFSDM1_FLT1 */
	isr_none, /* DFSDM1_FLT2 */
	isr_none, /* COMP */
	isr_none, /* LPTIM1 */
	isr_none, /* LPTIM2 */
	isr_none, /* OTG_FS */
	isr_none, /* DMA2_CH6 */
	isr_none, /* DMA2_CH7 */
	isr_none, /* LPUART1 */
	isr_none, /* OCTOSPI1 */
	isr_none, /* I2C3_EV */
	isr_none, /* I2C3_ER */
	isr_none, /* SAI1 */
	isr_none, /* SAI2 */
	isr_none, /* OCTOSPI2 */
	isr_none, /* TSC */
	isr_none, /* SIHSOT */
	isr_none, /* AES */
	isr_none, /* RNG */
	isr_none, /* FPU */
	isr_none, /* HASH_CRS */
	isr_none, /* I2C4_EV */
	isr_none, /* I2C4_ER */
	isr_none, /* DCMI */
	NULL,     /* Reserved */
	NULL,     /* Reserved */
	NULL,     /* Reserved */
	NULL,     /* Reserved */
	isr_none, /* DMA2D */
	isr_none, /* LCD_TFT */
	isr_none, /* LCD_TFT_ER */
	isr_none, /* GFXMMU */
	isr_none, /* DMAMUX1_OVR */
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
	/* NOTE: Cortex system timer freq is HCLK / 8 */
	wr32(R_SYST_RVR, cpu_freq / SYSTICKS_FREQ);
	wr32(R_SYST_CVR, 0);
	wr32(R_SYST_CSR, BIT0 | BIT1 | BIT2);

	log("ahb freq is %d\n", (uint)ahb_freq);

	uart_init();

	/* Skip to main */
	k_main();
}
