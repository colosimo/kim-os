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
#include <linker.h>

/* Main Function */
void k_main(void);

/* System ticks */

#define MS_TO_TICKS(ms) ((k_ticks_freq() * (ms)) / 1000)
#define TICKS_TO_MS(tk) ((1000 * (tk)) / k_ticks_freq())

u32 k_ticks(void);
u32 k_ticks_freq(void);
void k_delay(u32 msec);
void k_delay_us(u32 usec);
static inline u32 k_elapsed(u32 tprev) {return k_ticks() - tprev;}

struct task_t {
	void (*start)(struct task_t *t);
	void (*step)(struct task_t *t);
	void (*stop)(struct task_t *t);
	void *priv;
	u32 last_run;
	u32 intvl_ms;
	u32 tstart;
	u32 max_duration;
	u32 hits;
	const char *name;
	int running: 1;
	int no_autorun: 1;
	int async_start: 1;
};

typedef struct task_t task_t;

/* tasks_step: to be called in the main loop */
void task_stepall();

/* tasks_id: returns the id, being the index of task inside tsks section*/
int task_id(struct task_t *t);

/* tasks_find: find a task from a known name */
task_t *task_find(const char *name);

/* tasks_get: find a task from a known id */
task_t *task_get(int id);

/* task_start: start the desired task */
void task_start(task_t *t);

/* task_start_async: start the desired task within the event loop (no
 * direct call to task start callback */
void task_start_async(task_t *t);

/* task_stop: stop the desired task */
void task_stop(task_t *t);

/* task_done: task ended spontaneously */
void task_done(task_t *t);

struct __attribute__((packed)) cli_cmd_t {
	int narg;
	int (*cmd)(int argc, char *argv[], int fdout);
	const char *name;
	const char *descr;
};

typedef struct cli_cmd_t cli_cmd_t;

#endif /* _KIM_H_ */
