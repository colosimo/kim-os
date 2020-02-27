/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* show version information commands */

#include <stdint.h>
#include <log.h>
#include <basic.h>
#include <kim.h>
#include <linker.h>
#include <version.h>

static int version_cmd_cb(int argc, char *argv[], int fdout)
{
	k_fprintf(fdout, "git fw version: %s\n", GIT_VERSION);
	k_fprintf(fdout, "compiled on: %s\n", COMPILE_DATE);
	return 0;
}

const struct cli_cmd_t attr_cli cli_version = {
	.narg = 0,
	.cmd = version_cmd_cb,
	.name = "version",
	.descr = "Show version",
};
