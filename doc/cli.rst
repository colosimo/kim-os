Command Line Interface (CLI)
============================

.. index:: CLI

The Command Line Interface provides the capability to interact with KIM-OS
on any device proving read and write callback functions.

Typically, this is achieved on a UART connection, but it is fully portable
on any custom device providing read and write (e.g. a TCP socket for a
telnet-like connection): more CLI can co-exhist in the same firmware.

The code implementing the core of this feature is all contained in
``tasks/task-cli.c``. The embedded commands, provided with the basic realease
of KIM, are in *cli* subfolder, and grouped by type:

- ``cli-dbg.c``: commands helping in debug (register read/write);

- ``cli-dev.c``: commands to read and write over devices;

- ``cli-tasks.c``: commands to start and stop a task;

- ``cli-version.c``: the version command, retreving information about firmware
  version;

One special command, ``help``, is also available (and implemented in
``tasks/task-cli.c`` itself).

Going deeper into CLI implementation and usage, here is how the struct cli
is defined (in  ``include/kim.h``):

.. code-block:: c

	struct __attribute__((packed)) cli_cmd_t {
		int narg;
		int (*cmd)(int argc, char *argv[], int fdout);
		const char *name;
		const char *descr;
	};

``name`` is the command name itself, ``descr`` is the description shown by
the ``help`` command, ``narg`` is the minimum number of arguments required by
the command, and ``cmd`` is the callback executing the command.

For an example about how to add some command to the system, please just
browse into *cli* subfolder. For instance, the ``rw`` command, which performs
a direct write into a 32-bit register, is implemented in a these few lines:

.. code-block:: c

	static int rw_cmd_cb(int argc, char *argv[], int fdout)
	{
			wr32((u32*)atoi_hex(argv[1]), atoi_hex(argv[2]));
			return 0;
	}

	const struct cli_cmd_t attr_cli cli_rw = {
			.narg = 2,
			.cmd = rw_cmd_cb,
			.name = "rw",
			.descr = "Register Write: set a CPU register (in hex)\r\n"
				"\tUsage: rw <addr> <val>",
	};

.. note::

  When executing the callback, the CLI core calls ``cmd`` with the ``argc`` and
  ``argv`` filled in like standard C *main function* convention: ``argv[0]``
  is the command name itself, while ``argc`` is 1 + the number of command line
  parameters.
