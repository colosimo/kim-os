/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _KIM_H_
#define _KIM_H_

#include <intdefs.h>

/* Main Function */
void k_main(void);

/* System ticks */

#define MS_TO_TICKS(ms) (k_ticks_freq() * ms / 1000)

u32 k_ticks(void);
u32 k_ticks_freq(void);
static inline u32 k_elapsed(u32 tprev) {return k_ticks() - tprev;}

struct task_t {
	int id;
	void (*start)(struct task_t *t);
	void (*step)(struct task_t *t);
	void (*stop)(struct task_t *t);
	void *priv;
	u32 last_run;
	u32 intvl_ms;
	int running: 1;
};

typedef struct task_t task_t;

/* tasks_step: to be called in the main loop */
void task_stepall();

/* tasks_find: find a task from a known id */
struct task_t *task_find(int id);

/* task_start: start the desired task */
void task_start(task_t *t);

/* task_stop: stop the desired task */
void task_stop(task_t *t);

/* task_done: task ended spontaneously */
void task_done(task_t *t);

#endif /* _KIM_H_ */
