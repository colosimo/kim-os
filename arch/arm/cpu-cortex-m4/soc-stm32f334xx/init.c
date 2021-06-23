/*
 * Author: Aurelio Colosimo, 2021
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <kim.h>
#include <cpu.h>
#include <reg.h>
#include <log.h>
#include <gpio.h>

#define SYSTICKS_FREQ 1000

static u32 cpu_freq, ahb_freq, apb_freq;

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

void attr_weak isr_exti15_10(void)
{
}

static const void *attr_isrv_irq _isrv_irq[] = {
	/* STM32F334 peripheral IRQs */
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
	isr_none, /* DMA1_Channel1 */
	isr_none, /* DMA1_Channel2 */
	isr_none, /* DMA1_Channel3 */
	isr_none, /* DMA1_Channel4 */
	isr_none, /* DMA1_Channel5 */
	isr_none, /* DMA1_Channel6 */
	isr_none, /* DMA1_Channel7 */
	isr_none, /* ADC1_2 */
	isr_none, /* CAN_TX */
	isr_none, /* CAN_RX0 */
	isr_none, /* CAN_RX1 */
	isr_none, /* CAN_SCE */
	isr_none, /* EXTI9_5 */
	isr_none, /* TIM1_BRK_TIM15 */
	isr_none, /* TIM1_UP_TIM16 */
	isr_none, /* TIM1_TRG_COM_TIM17 */
	isr_none, /* TIM1_CC */
	isr_none, /* TIM2 */
	isr_none, /* TIM3 */
	isr_none, /* Reserved */
	isr_none, /* I2C1_EV */
	isr_none, /* I2C1_ER */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* SPI1 */
	isr_none, /* Reserved */
	isr_usart1, /* USART1 */
	isr_usart2, /* USART2 */
	isr_usart3, /* USART3 */
	isr_exti15_10, /* EXTI15_10 */
	isr_none, /* RTC_Alarm */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* TIM6_DAC1 */
	isr_none, /* TIM7_DAC2 */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* COMP2 */
	isr_none, /* COMP4_6 */
	isr_none, /* Reserved */
	isr_none, /* HRTIM_Master_IRQn */
	isr_none, /* HRTIMA_IRQn */
	isr_none, /* HRTIMB_IRQn */
	isr_none, /* HRTIMC_IRQn */
	isr_none, /* HRTIMD_IRQn */
	isr_none, /* HRTIME_IRQn */
	isr_none, /* HRTIM_FLT_IRQn */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* Reserved */
	isr_none, /* FPU */
};

u32 attr_weak k_ticks_freq(void)
{
	return SYSTICKS_FREQ;
}

void attr_weak k_delay_us(u32 usec)
{
	i32 tstart = rd32(R_SYST_CVR);
	i32 tnow;
	i32 intvl = usec * (cpu_freq / MHZ);
	int st = 0;;
	while (1) {
		tnow = rd32(R_SYST_CVR);
		if (tnow < tstart && !st) {
			intvl -= cpu_freq / SYSTICKS_FREQ;
			st = 1;
		}
		else if (tnow > tstart)
			st = 0;
		if (intvl < 0 || tnow > tstart + intvl)
			break;
	}
}

extern int putchar(int c);

void attr_used init(void)
{
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
