/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <reg.h>
#include <gpio.h>

#define FREQ 20

void pwm_init(void)
{
	u32 arr;

	/* TIM3_CH1 on PA6 */
	gpio_func(IO(PORTA, 6), 2);
	and32(R_TIM3_CCMR1, ~0x7f);
	or32(R_TIM3_CCMR1, 0b1111000);
	or32(R_TIM3_CCER, BIT0);
	wr32(R_TIM3_PSC, 2000);
	arr = 32000 / FREQ;
	wr32(R_TIM3_ARR, arr - 1);
	wr32(R_TIM3_CCR1, (arr * 95) / 100);
	or32(R_TIM3_CR1, BIT0);
	or32(R_PWR_CR, BIT8);
}

void pwm_set(u32 freq, u32 duty)
{
}

void pwm_get(u32 *freq, u32 *duty)
{
}
