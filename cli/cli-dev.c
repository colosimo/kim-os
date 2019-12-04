/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* dev-and I/O related CLI commands */

#include <stdint.h>
#include <log.h>
#include <basic.h>
#include <kim.h>
#include <linker.h>
#include <version.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <errcode.h>

static int lsdev_cmd_cb(int argc, char *argv[], int fdout)
{
	k_fprintf(fdout, "maj\tmin\tname\n");
	dev_enum(dev_dump, fdout);
	return 0;
}

const struct cli_cmd_t attr_cli cli_lsdev = {
	.narg = 0,
	.cmd = lsdev_cmd_cb,
	.name = "lsdev",
	.descr = "List devices"
};
