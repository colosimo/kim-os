/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* task related CLI commands */

#include <stdint.h>
#include <log.h>
#include <basic.h>
#include <kim.h>
#include <linker.h>
#include <version.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <errcode.h>

static int ps_cmd_cb(int argc, char *argv[], int fdout)
{
	struct task_t *t = tasks(0);

	k_fprintf(fdout, "id    stat name\n");
	for (;t != &__stop_tsks; t++) {
		k_fprintf(fdout, "%5d %4s %s\n", task_id(t), t->running ? "R" : "S",
		    t->name);
	}
	return 0;
}

const struct cli_cmd_t attr_cli cli_ps = {
	.narg = 0,
	.cmd = ps_cmd_cb,
	.name = "ps",
	.descr = "List tasks"
};

static task_t *task_get_by_arg(const char *argv, int fdout)
{
	struct task_t *t = tasks(0);

	if (isdigit(argv[0]))
		t = task_get(atoi(argv));
	else
		t = task_find(argv);

	if (!t)
		k_fprintf(fdout, "task not found: %s\n", argv);

	return t;
}

static int start_cmd_cb(int argc, char *argv[], int fdout)
{
	struct task_t *t = task_get_by_arg(argv[1], fdout);

	if (!t)
		return 0;

	if (t->running)
		k_fprintf(fdout, "task already running: %s\n", argv[1]);
	else
		task_start(t);

	return 0;
}

const struct cli_cmd_t attr_cli cli_ = {
	.narg = 1,
	.cmd = start_cmd_cb,
	.name = "start",
	.descr = "Start task. Usage: start <id> or start <name>"
};

static int stop_cmd_cb(int argc, char *argv[], int fdout)
{
	struct task_t *t = task_get_by_arg(argv[1], fdout);

	if (!t)
		return 0;

	if (!t->running)
		k_fprintf(fdout, "task not running: %s\n", argv[1]);
	else
		task_stop(t);

	return 0;
}

const struct cli_cmd_t attr_cli cli_stop = {
	.narg = 1,
	.cmd = stop_cmd_cb,
	.name = "stop",
	.descr = "Stop task. Usage: stop <id> or stop <name>"
};
