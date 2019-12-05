/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <kim.h>
#include <linker.h>
#include <log.h>
#include <kim-io.h>
#include <kim-io-defs.h>

typedef struct tclock_priv_t {
	int last;
	int uart_fd;
} tclock_priv_t;

static tclock_priv_t tclock_priv = {0, 0};

void tclock_start(struct task_t *t)
{
	tclock_priv_t *p = (tclock_priv_t*)t->priv;
	int i;
	p->last = 0;
	p->uart_fd = -1;

	for (i = 0; i < 8; i++) {
		p->uart_fd = k_fd(MAJ_SOC_UART, i);
		if (p->uart_fd >= 0) {
			log("%s on <%s> (id %02x)\n", __func__,
			    devs(p->uart_fd)->name, devs(p->uart_fd)->id);
			break;
		}
	}

	if (p->uart_fd < 0)
		err("%s could not find any UART\n", __func__);
}

void tclock_stop(struct task_t *t)
{
}

void tclock_step(struct task_t *t)
{
	char c[2];
	tclock_priv_t *p = (tclock_priv_t*)t->priv;

	if (p->uart_fd < 0) {
		task_done(t);
		return;
	}

	/* Reset when a char is received (a key is pressed on minicom) */
	if (k_avail(p->uart_fd)) {
		k_read(p->uart_fd, c, 1);
		k_write(p->uart_fd, c, 1);
		p->last = 0;
	}

	/* it is counting on 100ms interval, so consider 1 out of 10 steps */
	if (p->last % 10)
		goto done;

	if (p->last % 600 == 0) {
		c[0] = '\r';
		c[1] = '\n';
		k_write(p->uart_fd, c, 2);
	}

	c[0] = ((p->last / 10) % 10) + '0';

	k_write(p->uart_fd, c, 1);

done:
	p->last++;
}

task_t attr_tasks tclock_uart = {
	.start = tclock_start,
	.stop = tclock_stop,
	.step = tclock_step,
	.priv = &tclock_priv,
	.intvl_ms = 100,
	.name = "clock",
};
