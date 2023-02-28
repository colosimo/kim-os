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

void ledg_step(struct task_t *t)
{
	char cur;
	k_read(k_fd_byname("user_led_1"), &cur, 1);
	cur = !cur;
	k_write(k_fd_byname("user_led_1"), &cur, 1);
}

struct task_t attr_tasks task_ledg = {
	.step = ledg_step,
	.intvl_ms = 500,
	.name = "ledg",
};

void ledr_step(struct task_t *t)
{
	char tmp;
	char alarm;

	k_read(k_fd_byname("alarm_out"), &tmp, 1);

	alarm = get_alarm(ALRM_BITFIELD_ANY);

	if (tmp != alarm) {
		k_write(k_fd_byname("alarm_out"), &alarm, 1);
		tmp = alarm ? led_on : led_off;
		k_write(k_fd_byname("user_led_2"), &tmp, 1);
	}
}

struct task_t attr_tasks task_ledr = {
	.step = ledr_step,
	.intvl_ms = 100,
	.name = "ledr",
};
