/*
 * Author: Aurelio Colosimo, 2020
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <basic.h>

#define CLI_BUF_MAXLEN 80

struct cli_bridge_t {
	const char *fname;
	int fd;
	const char *enter_cmd;
	const char *exit_cmd;
};

struct cli_info_t {
	const char *fname;
	int fd;
	int pos;
	char buf[CLI_BUF_MAXLEN];
	int escaping: 1;
	int no_echo: 1;
	uint16_t escbuf;
	struct cli_bridge_t *b;
};

void cli_set_bridge(struct cli_bridge_t *b);

void cli_step(struct task_t *t);

void cli_start(struct task_t *t);
