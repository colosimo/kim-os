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
#include "ant.h"

static u32 last_duty, last_freq;

void ant_init(void)
{
	struct ant_cfg_t p;
	u8 m, status_rolling_mode;
	int i;

#ifdef BOARD_elo_new
	/* TIM4_CH3 on PB8 */
	gpio_func(IO(PORTB, 8), 2);
	and32(R_TIM4_CCMR2, ~0x7f);
	or32(R_TIM4_CCMR2, 0b1111000);
	or32(R_TIM4_CCER, BIT8);
	wr32(R_TIM4_PSC, 40); /* Prescaler is 40 */
#else
	/* TIM3_CH1 on PA6 */
	gpio_func(IO(PORTA, 6), 2);
	and32(R_TIM3_CCMR1, ~0x7f);
	or32(R_TIM3_CCMR1, 0b1111000);
	or32(R_TIM3_CCER, BIT0);
	wr32(R_TIM3_PSC, 40); /* Prescaler is 40 */
#endif

	eeprom_read(EEPROM_ANT_CURRENT_MODE_ADDR, &m, 1);
	if (m == 3) {
		eeprom_read(EEPROM_ANT_STATUS_MODE_ADDR, &status_rolling_mode, 1);
		rolling_start(status_rolling_mode);
	}
	else if (m == 4)
		log("Logger mode, no ant out\n");
	else if (m > 4) { /* Invalid mode, reset */
		m = 0;
		eeprom_write(EEPROM_ANT_CURRENT_MODE_ADDR, &m, 1);
	}

	for (i = 0; i < 3; i++) {

		eeprom_read(EEPROM_ANT_MODE0_ADDR + i * sizeof(p), (u8*)&p, sizeof(p));

		if (!ant_check(&p.freq, &p.duty))
			eeprom_write(EEPROM_ANT_MODE0_ADDR + i * sizeof(p), (u8*)&p, sizeof(p)); /* FIXME mode */

		if (i == m) {
			ant_set(p.freq, p.duty);
			eeprom_write(EEPROM_ANT_STATUS_MODE_ADDR, &m, 1);
		}
	}

	if (m == 4)
		ant_set(MIN_FREQ, 0);
}

int ant_check(u32 *freq, u32 *duty)
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

	if (*duty < MIN_DUTY && *duty != 0) {
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

void ant_set(u32 freq, u32 duty)
{
	u32 arr;

	log("ant set %d %d\n", (uint)freq, (uint)duty);

	ant_check(&freq, &duty);

#ifdef BOARD_elo_new
	or32(R_TIM4_CCER, BIT8);
#else
	or32(R_TIM3_CCER, BIT0);
#endif
	ant_enable();

	arr = 400000 / freq; /* 400000 is 16MHz / Prescaler */
#ifdef BOARD_elo_new
	wr32(R_TIM4_ARR, arr - 1);
	wr32(R_TIM4_CCR3, (arr * (100 - duty)) / 100);
#else
	wr32(R_TIM3_ARR, arr - 1);
	wr32(R_TIM3_CCR1, (arr * (100 - duty)) / 100);
#endif

	last_duty = duty;
	last_freq = freq;
}

void ant_disable(void)
{
#ifdef BOARD_elo_new
	and32(R_TIM4_CR1, ~BIT0);
#else
	and32(R_TIM3_CR1, ~BIT0);
#endif
}

void ant_enable(void)
{
#ifdef BOARD_elo_new
	or32(R_TIM4_CR1, BIT0);
#else
	or32(R_TIM3_CR1, BIT0);
#endif
}

void ant_get(u32 *_freq, u32 *_duty)
{
	*_freq = last_freq;
	*_duty = last_duty;
}
