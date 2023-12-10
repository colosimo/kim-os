/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <stdint.h>
#include <log.h>
#include <kim.h>
#include <linker.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <errcode.h>

#include "def.h"
#include "eeprom.h"

char buzzer_enabled = BUZ_OFF;

void buzzer_enable(int en)
{
	if (buzzer_enabled == en)
		return;

	buzzer_enabled = en;
	if (en) {
		if (get_alarm_bitfield())
			k_write(k_fd_byname("buzzer"), &buzzer_on, 1);
	}
	else
		k_write(k_fd_byname("buzzer"), &buzzer_off, 1);
}

#ifdef BOARD_elo_new
void buzzer_step(struct task_t *t)
{
	char cur;
	u32 alarm_bits;

	alarm_bits = get_alarm_bitfield();
	k_read(k_fd_byname("buzzer"), &cur, 1);

	if (!alarm_bits || buzzer_enabled == BUZ_OFF) {
		if (cur != buzzer_off)
			k_write(k_fd_byname("buzzer"), &buzzer_off, 1);
	}
	else if (alarm_bits) {
		if (buzzer_enabled == BUZ_CONT && cur != buzzer_on)
			k_write(k_fd_byname("buzzer"), &buzzer_on, 1);
		else {
			cur = !cur;
			if (buzzer_enabled == BUZ_INT)
				k_write(k_fd_byname("buzzer"), &cur, 1);
		}
	}
}

struct task_t attr_tasks task_buzzer = {
	.step = buzzer_step,
	.intvl_ms = 500,
	.name = "buzzer",
};
#endif

void ledblink_step(struct task_t *t)
{
	char cur;
	u32 alarm_bits;

	k_read(k_fd_byname("user_led_1"), &cur, 1);
	cur = !cur;
	k_write(k_fd_byname("user_led_1"), &cur, 1);

	alarm_bits = get_alarm_bitfield();
	if ((alarm_bits & ALRM_BITFIELD_ANT))
		return;

	if (!alarm_bits) {
		cur = led_off;
		k_write(k_fd_byname("user_led_2"), &cur, 1);
	}
	else {
		/* red led blink on alarms different than Antenna */
		k_write(k_fd_byname("user_led_2"), &cur, 1);
	}
}

struct task_t attr_tasks task_ledblink = {
	.step = ledblink_step,
	.intvl_ms = 500,
	.name = "ledg",
};

static int first_step = 1;

void ledr_and_relay_step(struct task_t *t)
{
#ifdef BOARD_elo_new
	char tmp;
	char alarm;
	char relay_out;

	k_read(k_fd_byname("user_led_2"), &tmp, 1);

	alarm = get_alarm(ALRM_BITFIELD_ANY);

	if (first_step || (alarm && tmp == led_off) || (!alarm && tmp == led_on)) {
		tmp = alarm ? led_on : led_off;

		if (get_alarm(ALRM_BITFIELD_ANT))
			k_write(k_fd_byname("user_led_2"), &tmp, 1);

		first_step = 0;
	}

	eeprom_read(EEPROM_ALRM_OUT_POL, &tmp, 1);
	switch (tmp) {
		case 0: relay_out = alarm ? 0 : 1; break;
		case 1: relay_out = alarm; break;
		case 2:
		default: relay_out = 0; break;
	}
	k_read(k_fd_byname("alarm_out"), &tmp, 1);
	if (tmp != relay_out)
		k_write(k_fd_byname("alarm_out"), &relay_out, 1);


#else
	char tmp;
	char alarm;

	k_read(k_fd_byname("alarm_out"), &tmp, 1);

	alarm = get_alarm(ALRM_BITFIELD_ANY);

	if (tmp != alarm) {
		k_write(k_fd_byname("alarm_out"), &alarm, 1);
		tmp = alarm ? led_on : led_off;
		k_write(k_fd_byname("user_led_2"), &tmp, 1);
	}
#endif
}

struct task_t attr_tasks task_ledr_and_relay = {
	.step = ledr_and_relay_step,
	.intvl_ms = 100,
	.name = "ledr_and_relay",
};
