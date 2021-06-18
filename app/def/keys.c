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

#include "keys.h"
#include "def.h"

char *fname_keys[4] = {"sw_up", "sw_down", "sw_esc", "sw_enter"};
int fd_keys[4];
u8 keys_stat[4];
u32 keys_evts;

void keys_start(struct task_t *t)
{
	int i;
	keys_evts = 0;
	for (i = KEY_UP; i <= KEY_ENTER; i++) {
		fd_keys[i] = k_fd_byname(fname_keys[i]);
		keys_stat[i] = 0;
	}
}

void keys_step(struct task_t *t)
{
	int i;
	u8 tmp[4];
	for (i = KEY_UP; i <= KEY_ENTER; i++) {

		k_read(fd_keys[i], &tmp[i], 1);

		if (tmp[i] && !keys_stat[i]) {
			rearm_standby();
			if (get_standby()) {
				if (i != KEY_ESC)
					set_standby(0);
			}
			else
				keys_evts |= (1 << i);
		}

		keys_stat[i] = tmp[i];
	}
}

struct task_t attr_tasks task_keys = {
	.start = keys_start,
	.step = keys_step,
	.intvl_ms = 5,
	.name = "keys",
};

u32 keys_get_evts(void)
{
	return keys_evts;
}

void keys_clear_evts(u32 evts)
{
	keys_evts &= ~evts;
}
