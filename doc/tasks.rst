Tasks definition and scheduling
===============================

A *task* inside KIM system is basically defined as a set of callbacks to be
executed at start, stop and at a periodic interval (called *step* inside KIM
sources. Basically, this allows to implement a Finit State Machine (FSM).
This is a different approach compared to commonly used threads, with some
advantages and a drawback:

* all machine interrupts are directly handled by the lower layer drivers
  (in arch/)

* when looking at the application code there always is one and only function
  running, leading to:

  - no concurrency on variables to be handled (code more deterministic, easier
    debug and test);

  - no context switching (better performance on a single-core CPU, no need to
    continuously save thread status from registers to RAM).

* the main drawback of FSM approach is the lack of a proper sleep system, since
  the task function must always immediately return; if a time wait is needed,
  this must be manually handled, by comparing current system time with the
  starting time + desired timeout.

The ``struct task_t`` is defined in ``include/kim.h``, together with the
functions to start and stop tasks:

.. code-block:: c

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

As stated before, the most important fields are the first three function
pointers, which must be filled with the desired functions for each task.
Since more tasks of the same kind can cohexist (i.e. more tasks can share
the same callbacks), ``priv`` is a generic pointer where the programmer can hold
some private information dedicated to each *instance* of the task.

``last_run``, ``tstart``, ``running``, ``async_start`` and ``hits`` are used
runtime to correctly schedule the task execution; ``max_duration`` can
optionally be declared with a positive value in case the task has a fixed
execution total time; ``no_autorun`` can be set to ``1`` if the task must not
automatically start at reset (which is the default behaviour); finally,
each task has a ``name``, which can be used to look for the task and
start it.

An example of task is provided in the file ``tasks/task-clock.c``. It counts
on the first UART found, counting up to 60s before issuing a newline. In the
meantime, each character received on the same UART is echoed, in order to
show both sides of UART communication.

To understand how tasks are started, a practical example is given by
``cli/cli-tasks.c``, where the CLI command set to handle tasks is implemented
(see :any:`cli` for more information).

For instance, this is a portion of code showing how a task is run, assuming
the task name is known:

.. code-block:: c

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
		int i;
		for (i = 1; i < argc; i++) {

			struct task_t *t = task_get_by_arg(argv[i], fdout);

			if (!t)
				continue;

			if (t->running)
				k_fprintf(fdout, "task already running: %s\n", argv[i]);
			else
				task_start(t);
		}
		return 0;
	}


Tasks can be started *directly* (function ``task_start``) or *asynchronously*
(function ``task_start_async``); on direct start, the task ``start`` callback
is immediately called; on asynchronous start, the main loop calls the ``start``.

If you want to have a look at how tasks are handled, just watch the ``k_main``
and ``task_stepall`` functions, both defined in ``kernel/kim.c``:

.. code-block:: c

	void task_stepall(void)
	{
		struct task_t *t = tasks(0);
		for (;t != &__stop_tsks; t++) {
			if (!t->running) {
				if (t->async_start) {
					task_start(t);
					t->async_start = 0;
				}
				continue;
			}

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

In the first part of ``k_main`` function, the init callback for each device
is called; see :any:`io-system` for more information.

The subsequent ``while(1)`` in ``k_main`` is the core of what is continuously
running in the system; the whole main is composed of a few lines, according to
the minimalistic principle KIM is adopting.
