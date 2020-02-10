/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <basic.h>
#include <kim.h>
#include <kim-io.h>
#include <linker.h>
#include <log.h>

extern void sleep(void);

int task_id(struct task_t *t)
{
	return t - tasks(0);
}

void task_start(task_t *t) {
	if (!t)
		return;
	t->last_run = 0;
	t->running = 1;
	t->tstart = k_ticks();
	if (t->start)
		t->start(t);
	t->hits = 0;
}

void task_stop(task_t *t) {
	if (!t)
		return;

	if (t->stop)
		t->stop(t);

	task_done(t);
}

void task_done(task_t *t)
{
	if (!t)
		return;

	log("%s done, %d hits in %dms\n", t->name, (uint)t->hits,
		(uint)k_elapsed(t->tstart));

	t->running = 0;
}

task_t *task_find(const char *name)
{
	struct task_t *t = tasks(0);
	for (;t != &__stop_tsks; t++) {
		if (!strcmp(t->name, name))
			return t;
	}
	return NULL;
}

task_t *task_get(int id)
{
	if (tasks(id) >= &__stop_tsks)
		return NULL;
	return tasks(id);
}

void task_stepall(void)
{
	struct task_t *t = tasks(0);
	for (;t != &__stop_tsks; t++) {
		if (!t->running)
			continue;

		if (t->max_duration && k_elapsed(t->tstart) > t->max_duration)
			task_done(t);

		if (k_elapsed(t->last_run) < MS_TO_TICKS(t->intvl_ms))
			continue;
		t->last_run = k_ticks();
		t->step(t);
		t->hits++;
	}
}

void attr_weak k_main(void)
{
	struct task_t *t = tasks(0);
	struct k_dev_t *d = devs(0);
	int fd;

	for (; d != &__stop_devs; d++) {
		fd = k_fd(dev_major(d->id), dev_minor(d->id));
		if (fd < 0 || !d->drv) {
			err("Could not open %s (%04x)\n", d->name, d->id);
			continue;
		}
		if (d->drv->init)
			d->drv->init(fd);
	}

	for (;t != &__stop_tsks; t++) {
		if (!t->no_autorun)
			task_start(t);
	}

	while(1) {
		sleep();
		task_stepall();
	}
}
