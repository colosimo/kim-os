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

#define IO_BUF_LEN 32

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

static int w_cmd_cb(int argc, char *argv[], int fdout)
{
	int fd;
	int i;
	u8 buf[IO_BUF_LEN];

	fd = k_fd_byname(argv[1]);

	if (fd < 0) {
		k_fprintf(fdout, "dev not found: <%s>\n", argv[1]);
		return -ERRINVAL;
	}

	if (argc - 2 > sizeof(buf)) {
		k_fprintf(fdout, "%s: max " str(IO_BUF_LEN) " bytes allowed\n",
			argv[0]);
		return -ERRINVAL;
	}

	for (i = 0; i < argc - 2; i++)
		buf[i] = atoi_hex(argv[i + 2]);

	k_write(fd, buf, i);

	return 0;
}

const struct cli_cmd_t attr_cli cli_w = {
	.narg = 2,
	.cmd = w_cmd_cb,
	.name = "w",
	.descr = "Write a hex sequence to a device (max " str(IO_BUF_LEN) " bytes).\r\n"
	    "\tUsage: w <devname> <byte1> <byte2> ...",
};

static int r_cmd_cb(int argc, char *argv[], int fdout)
{
	int fd;
	int i, n, rd;
	u8 buf[IO_BUF_LEN];

	fd = k_fd_byname(argv[1]);

	if (fd < 0) {
		k_fprintf(fdout, "dev not found: <%s>\n", argv[1]);
		return -ERRINVAL;
	}

	n = atoi(argv[2]);

	while (n) {
		rd = k_read(fd, buf, min(IO_BUF_LEN, n));
		if (rd <= 0)
			break;
		for (i = 0; i < rd; i++)
			k_fprintf(fdout, "%02x ", buf[i]);
		n-= rd;
	}

	return 0;
}

const struct cli_cmd_t attr_cli cli_r = {
	.narg = 2,
	.cmd = r_cmd_cb,
	.name = "r",
	.descr = "Read data from a device and dump it.\r\n"
	    "\tUsage: r <devname> <maxbytes> (maxbytes < 0 to flush all)",
};
