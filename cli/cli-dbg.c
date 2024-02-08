/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* dbg related CLI commands: raw write and read to/from registers */

#include <stdint.h>
#include <kprint.h>
#include <basic.h>
#include <kim.h>
#include <kim-io.h>

#ifndef ARCH_unix

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

static int rr_cmd_cb(int argc, char *argv[], int fdout)
{
	volatile u32 *addr;
	int nreg = 1;
	int i;

	addr = (u32*)atoi_hex(argv[1]);

	if (argc > 2)
		nreg = atoi(argv[2]);

	for (i = 0; i < nreg; i++) {
		if (i % 4 == 0)
			k_fprintf(fdout, "%p: ", addr + i);

		k_fprintf(fdout, "%08x ", (uint)rd32(addr + i));

		if (i % 4 == 3 || i == nreg - 1)
			k_fprintf(fdout, "\n");
	}

	return 0;
}

const struct cli_cmd_t attr_cli cli_rr = {
	.narg = 1,
	.cmd = rr_cmd_cb,
	.name = "rr",
	.descr = "Register Read: read one or N CPU registers and dump them.\r\n"
	    "\tUsage: rr <addr> [N]",
};

#endif
