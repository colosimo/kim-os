/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <basic.h>
#include <kim.h>
#include <linker.h>

struct task_t attr_tasks dummy;

u32 attr_weak k_ticks(void) /* FIXME dummy */
{
	return 0;
}

u32 attr_weak k_ticks_freq(void) /* FIXME dummy */
{
	return 1000;
}

void task_start(struct task_t *t) {
	t->last_run = 0;
	t->running = 1;
	if (t->start)
		t->start(t);
}

void task_stop(struct task_t *t) {
	if (t->stop)
		t->stop(t);
	t->running = 0;
}

void task_done(struct task_t *t)
{
	t->running = 0;
}

struct task_t *task_find(int id)
{
	struct task_t *t = tasks(0);
	for (;t != &__stop_tasks; t++) {
		if (id == t->id)
			return t;
	}
	return NULL;
}

void task_stepall()
{
	struct task_t *t = tasks(0);
	for (;t != &__stop_tasks; t++) {
		if (!t->running)
			continue;
		if (k_elapsed(t->last_run) < MS_TO_TICKS(t->intvl_ms))
			continue;
		t->last_run = k_ticks();
		t->step(t);
	}
}
