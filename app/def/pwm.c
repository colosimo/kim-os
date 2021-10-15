/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <kim.h>
#include <reg.h>
#include <gpio.h>
#include <log.h>

#include "def.h"
#include "eeprom.h"
#include "pwm.h"

static u32 last_duty, last_freq;

void pwm_init(void)
{
	struct pwm_cfg_t p;
	u8 m, status_rolling_mode;
	int i;

	/* TIM3_CH1 on PA6 */
	gpio_func(IO(PORTA, 6), 2);
	and32(R_TIM3_CCMR1, ~0x7f);
	or32(R_TIM3_CCMR1, 0b1111000);
	or32(R_TIM3_CCER, BIT0);
	wr32(R_TIM3_PSC, 40); /* Prescaler is 40 */

	eeprom_read(EEPROM_PWM_CURRENT_MODE_ADDR, &m, 1);
	if (m == 3) {
		eeprom_read(EEPROM_PWM_STATUS_MODE_ADDR, &status_rolling_mode, sizeof(status_rolling_mode));
		rolling_start(status_rolling_mode);
	}
	else if (m > 3) { /* Invalid mode, reset */
		m = 0;
		eeprom_write(EEPROM_PWM_CURRENT_MODE_ADDR, &m, 1);
	}

	for (i = 0; i < 3; i++) {

		eeprom_read(EEPROM_PWM_MODE0_ADDR + i * sizeof(p), (u8*)&p, sizeof(p));

		if (!pwm_check(&p.freq, &p.duty))
			eeprom_write(EEPROM_PWM_MODE0_ADDR + i * sizeof(p), (u8*)&p, sizeof(p)); /* FIXME mode */

		if (i == m && m != 3) {
			pwm_set(p.freq, p.duty);
			eeprom_write(EEPROM_PWM_STATUS_MODE_ADDR, &m, 1);
		}
	}
}

int pwm_check(u32 *freq, u32 *duty)
{
	int ret = 1;
	if (*freq < MIN_FREQ) {
		wrn("Min freq %d\n", MIN_FREQ);
		*freq = MIN_FREQ;
		ret = 0;
	}
	else if (*freq > MAX_FREQ) {
		wrn("Max freq %d\n", MAX_FREQ);
		*freq = MAX_FREQ;
		ret = 0;
	}

	if (*duty < MIN_DUTY) {
		wrn("Min duty %d\n", MIN_DUTY);
		*duty = MIN_DUTY;
		ret = 0;
	}
	else if (*duty > MAX_DUTY) {
		wrn("Max duty %d\n", MAX_DUTY);
		*duty = MAX_DUTY;
		ret = 0;
	}
	return ret;
}

void pwm_set(u32 freq, u32 duty)
{
	u32 arr;

	log("pwm set %d %d\n", (uint)freq, (uint)duty);

	or32(R_TIM3_CR1, BIT0);
	pwm_check(&freq, &duty);

	arr = 400000 / freq; /* 400000 is 16MHz / Prescaler */
	wr32(R_TIM3_ARR, arr - 1);
	wr32(R_TIM3_CCR1, (arr * (100 - duty)) / 100);

	last_duty = duty;
	last_freq = freq;
}

void pwm_get(u32 *_freq, u32 *_duty)
{
	*_freq = last_freq;
	*_duty = last_duty;
}
