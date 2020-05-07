/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <stdint.h>
#include <log.h>
#include <kim.h>
#include <linker.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <errcode.h>
#include <task-cli.h>

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

#define info(t) ((struct cli_info_t*)t->priv)

static struct cli_info_t cli_priv = {
	.fname = UART_CLI_FNAME
};

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
				if (*cmdline == '"') {
					argv[argc - 1]++;
					status = 2;
					break;
				}

				if (isspace(*cmdline)) {
					*cmdline = '\0';
					status = 0;
				}

			case 2: /* Inside double quotes " " */
				if (*cmdline == '"') {
					*cmdline = '\0';
					status = 1;
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
	struct cli_info_t *cli = info(t);
	cli->pos = 0;
	memset(cli->buf, 0, sizeof(cli->buf));
	k_fprintf(cli->fd, CLI_PROMPT);
}

void cli_start(struct task_t *t)
{
	struct cli_info_t *cli = info(t);
	int i;

	if (strlen(cli->fname)) {
		cli->fd = k_fd_byname(cli->fname);
		if (cli->fd < 0) {
			err("Could not open %s", cli->fname);
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

	if (cli->fd >= 0)
		priv_reset(t);
}

void cli_step(struct task_t *t)
{
	char c;
	struct cli_info_t *cli = info(t);
	char buf[BUF_MAXLEN];
	int n;
	int i;

	if (cli->fd < 0) {
		task_done(t);
		return;
	}

	n = k_read(cli->fd, buf, sizeof(buf));

	if (n < 0)
		task_done(t);

	for (i = 0; i < n; i++) {
		c = buf[i];
		if (!cli->no_echo && !cli->escaping && c != ESC && (c != BS || cli->pos))
			k_fprintf(cli->fd, "%c", c);

		if (c == ESC && !cli->escaping) {
			cli->escaping = 1;
			cli->escbuf = 0;
			continue;
		}

		if (c == BS) {
			if (cli->pos == 0)
				continue;
			cli->pos--;
			cli->buf[cli->pos] = '\0';
			k_fprintf(cli->fd, "%c[J", ESC);
			continue;
		}

		if (cli->escaping) {
			cli->escbuf = (cli->escbuf << 8) | c;
			cli->escaping++;
			if (cli->escaping == 3) {
				cli->escaping = 0;

				switch (cli->escbuf & 0xff) {

				case 0x44:
					if (cli->pos > 0)
						cli->pos--;
					else
						continue;
					break;
				case 0x43:
					if (cli->pos < BUF_MAXLEN &&
						cli->buf[cli->pos] != '\0')
						cli->pos++;
					else
						continue;
					break;

				default:
					continue;
				}

				k_fprintf(cli->fd, "%c%c%c", ESC, cli->escbuf >> 8, cli->escbuf & 0xff);
			}
			continue;
		}

		if (c >= 0x20) {
			cli->buf[cli->pos++] = c;
			if (cli->pos < BUF_MAXLEN - 1)
				continue;
		}

		if (c != '\r' && c != '\n')
			continue;

		k_fprintf(cli->fd, "\n");

		if (cli_exec(cli->buf, cli->fd))
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
