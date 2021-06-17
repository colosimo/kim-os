/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <kim.h>
#include <reg.h>
#include <gpio.h>
#include <log.h>

#include "pwm.h"

#define MIN_FREQ 20
#define MAX_FREQ 200
#define MIN_DUTY 2
#define MAX_DUTY 10

static u32 freq, duty;

void pwm_init(void)
{
	/* TIM3_CH1 on PA6 */
	gpio_func(IO(PORTA, 6), 2);
	and32(R_TIM3_CCMR1, ~0x7f);
	or32(R_TIM3_CCMR1, 0b1111000);
	or32(R_TIM3_CCER, BIT0);
	wr32(R_TIM3_PSC, 200);

	pwm_set(MIN_FREQ, MIN_DUTY);
#if 0
	arr = 320000 / MIN_FREQ;
	wr32(R_TIM3_ARR, arr - 1);
	wr32(R_TIM3_CCR1, (arr * (100 - MIN_DUTY)) / 100);
#endif
	or32(R_TIM3_CR1, BIT0);
	or32(R_PWR_CR, BIT8);
}

void pwm_set(u32 freq, u32 duty)
{
	u32 arr;
	if (freq < MIN_FREQ) {
		wrn("Min freq %d\n", MIN_FREQ);
		freq = MIN_FREQ;
	}
	else if (freq > MAX_FREQ) {
		wrn("Max freq %d\n", MAX_FREQ);
		freq = MAX_FREQ;
	}

	if (duty < MIN_DUTY) {
		wrn("Min duty %d\n", MIN_DUTY);
		duty = MIN_DUTY;
	}
	else if (duty > MAX_DUTY) {
		wrn("Max duty %d\n", MAX_DUTY);
		duty = MAX_DUTY;
	}

	arr = 320000 / freq;
	wr32(R_TIM3_ARR, arr - 1);
	wr32(R_TIM3_CCR1, (arr * (100 - duty)) / 100);
}

void pwm_get(u32 *_freq, u32 *_duty)
{
	*_freq = freq;
	*_duty = duty;
}
