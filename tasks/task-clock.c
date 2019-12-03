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

typedef struct tclock_priv_t {
	int last;
	int uart_fd;
} tclock_priv_t;

static tclock_priv_t tclock_priv[2] = {
	{0, 0},
	{0, 1},
};

void tclock_start(struct task_t *t)
{
	tclock_priv_t *p = (tclock_priv_t*)t->priv;
	p->last = 0;
	p->uart_fd = k_fd_byname("uart1");
	log("%s on uart id=%d\n", __func__, p->uart_fd);
}

void tclock_stop(struct task_t *t)
{
}

void tclock_step(struct task_t *t)
{
	char c[2];
	tclock_priv_t *p = (tclock_priv_t*)t->priv;

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

task_t attr_tasks tclock_uart1 = {
	.start = tclock_start,
	.stop = tclock_stop,
	.step = tclock_step,
	.priv = &tclock_priv[0],
	.intvl_ms = 100,
};

task_t attr_tasks tclock_uart2 = {
	.start = tclock_start,
	.stop = tclock_stop,
	.step = tclock_step,
	.priv = &tclock_priv[1],
	.intvl_ms = 100,
};
