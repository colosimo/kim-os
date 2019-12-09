/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <stdint.h>
#include <log.h>
#include <basic.h>
#include <kim.h>
#include <linker.h>
#include <version.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <errcode.h>

#define BUF_MAXLEN 80
#define MAX_PARAMS 32

#define CLI_PROMPT "kim> "
#define CLI_BYE "\nBye!\n"

#define STX 0x02
#define EOT 0x04
#define BS  0x08
#define ESC 0x1b

#ifndef UART_CLI_FNAME
#define UART_CLI_FNAME ""
#endif

static int escaping;
uint16_t escbuf;

static struct cli_priv_t {
	int fd;
	int pos;
	char buf[BUF_MAXLEN];
} cli_priv;

#define priv(t) ((struct cli_priv_t*)t->priv)

static int help_cmd_cb(int argc, char *argv[], int fdout)
{
	const struct cli_cmd_t *c = cli(0);

	for (;c != &__stop_cli; c++) {
		k_fprintf(fdout, "%s", c->name);
		if (c->descr)
			k_fprintf(fdout, "\n\t%s\n", c->descr);
		else
			k_fprintf(fdout, "\n");
	}
	return 0;
}

const struct cli_cmd_t attr_cli cli_help = {
	.narg = 0,
	.cmd = help_cmd_cb,
	.name = "help",
	.descr = "List CLI commands"
};

static int cli_exec(char *cmdline, int fdout)
{
	const struct cli_cmd_t *c = cli(0);
	char *argv[MAX_PARAMS];
	int argc = 0;
	int status = 0;

	while (isspace(*cmdline))
		cmdline++;

	while (*cmdline != '\0') {
		switch (status) {
			case 0: /* Searching cmd */
				if (!isspace(*cmdline)) {
					if (argc == MAX_PARAMS)
						return -ERRINVAL;
					argv[argc] = cmdline;
					argc++;
					status = 1;
				}
			case 1: /* Searching space */
				if (isspace(*cmdline)) {
					*cmdline = '\0';
					status = 0;
				}
			default:
				break;
		}
		cmdline++;
	}

	if (argc == 0)
		return 0; /* Empty command line */

	for (;c != &__stop_cli; c++) {
		if (!strcmp(c->name, argv[0]))
			break;
	}

	if (c == &__stop_cli)
		return -ERRINVAL;

	if (c->narg > argc - 1)
		return -ERRINVAL;

	return c->cmd(argc, argv, fdout);
}

static void priv_reset(struct task_t *t)
{
	priv(t)->pos = 0;
	memset(priv(t)->buf, 0, sizeof(priv(t)->buf));
	k_fprintf(priv(t)->fd, CLI_PROMPT);
}

static void cli_start(struct task_t *t)
{
	struct cli_priv_t *cli = priv(t);
	int i;

	if (strlen(UART_CLI_FNAME)) {
		cli->fd = k_fd_byname(UART_CLI_FNAME);
		if (cli->fd < 0) {
			err("Could not open %s", UART_CLI_FNAME);
			return;
		}
	}
	else {
		/* Search for first available UART */
		for (i = 0; i < 256; i++) {
			cli->fd = k_fd(MAJ_SOC_UART, i);
			if (cli->fd >= 0) {
				log("%s on <%s> (id %02x)\n", __func__,
					devs(cli->fd)->name, devs(cli->fd)->id);
				break;
			}
		}
	}
	priv_reset(t);
}

static void cli_step(struct task_t *t)
{
	char c;
	struct cli_priv_t *cli = priv(t);

	if (cli->fd < 0) {
		task_done(t);
		return;
	}

	while (k_read(cli->fd, &c, 1) > 0) {
		if (!escaping && c != ESC && (c != BS || priv(t)->pos))
			k_fprintf(cli->fd, "%c", c);

		if (c == ESC && !escaping) {
			escaping = 1;
			escbuf = 0;
			continue;
		}

		if (c == BS) {
			if (priv(t)->pos == 0)
				continue;
			priv(t)->pos--;
			priv(t)->buf[priv(t)->pos] = '\0';
			k_fprintf(cli->fd, "%c[J", ESC);
			continue;
		}

		if (escaping) {
			escbuf = (escbuf << 8) | c;
			escaping++;
			if (escaping == 3) {
				escaping = 0;

				switch (escbuf & 0xff) {

				case 0x44:
					if (priv(t)->pos > 0)
						priv(t)->pos--;
					else
						continue;
					break;
				case 0x43:
					if (priv(t)->pos < BUF_MAXLEN &&
						priv(t)->buf[priv(t)->pos] != '\0')
						priv(t)->pos++;
					else
						continue;
					break;

				default:
					continue;
				}

				k_fprintf(cli->fd, "%c%c%c", ESC, escbuf >> 8, escbuf & 0xff);
			}
			continue;
		}

		if (c >= 0x20) {
			priv(t)->buf[priv(t)->pos++] = c;
			if (priv(t)->pos < BUF_MAXLEN - 1)
				continue;
		}

		if (c != '\r' && c != '\n')
			continue;

		k_fprintf(cli->fd, "\n");

		if (cli_exec(priv(t)->buf, priv(t)->fd))
			k_fprintf(cli->fd, "Invalid cmd\n");

		priv_reset(t);
	}
}

struct task_t attr_tasks task_cli = {
	.start = cli_start,
	.step = cli_step,
	.intvl_ms = 100,
	.priv = &cli_priv,
	.name = "cli",
};
