/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <basic.h>
#include <kim.h>
#include <kim-io.h>
#include <linker.h>
#include <log.h>
#include <gpio.h>
#include <errcode.h>

#include "lcd.h"
#include "def.h"

void lcd_init()
{
	lcd_set_backlight(0);
	lcd_write(0x38, 0);
	lcd_cursor(0, 0, 0);
	lcd_write(0x06, 0);
	lcd_write(0x01, 0);
}

void lcd_write(u8 ch, int isdata)
{
	int i;
	char buf[8];
	int fd_data[8];

	dbg("%s %02x %d\n", __func__, ch, isdata);
	k_write(k_fd_byname("e"), &zero, 1);
	k_write(k_fd_byname("rs"), isdata ? &one : &zero, 1);
	k_write(k_fd_byname("rw"), &zero, 1);

	for (i = 0; i < 8; i++) {
		k_sprintf(buf, "db%d", i);
		fd_data[i] = k_fd_byname(buf);
		if (fd_data[i] < 0) {
			err("Could not open %s\n", buf);
			continue;
		}
	}

	for (i = 0; i < 8; i++) {
		k_write(fd_data[i], (ch & (1 << i)) ? &one : &zero, 1);
	}

	k_write(k_fd_byname("e"), &one, 1);
	k_delay_us(1000);
	k_write(k_fd_byname("e"), &zero, 1);
}

void lcd_write_string(const char *str, int line)
{
	int i;
	lcd_cursor(line, 0, 0);
	for (i = 0; i < strlen(str); i++)
		lcd_write(str[i], 1);
}

void lcd_cursor(int line, int pos, int show)
{
	lcd_write(0x80 + line * 0x40 + pos, 0);
	lcd_write(0x0c | (show ? BIT1 : 0), 0);
}

void lcd_set_backlight(int en)
{
	k_write(k_fd_byname("lcd_backlight"), en ? &one : &zero, 1);
}

int lcd_get_backlight(void)
{
	char b;
	k_read(k_fd_byname("lcd_backlight"), &b, 1);
	return b;
}

static int hc_cmd_cb(int argc, char *argv[], int fdout)
{
	lcd_write(atoi_hex(argv[1]), 0);
	return 0;
}

const struct cli_cmd_t attr_cli cli_hc = {
	.narg = 1,
	.cmd = hc_cmd_cb,
	.name = "hc",
	.descr = "Hitachi Control",
};

static int hd_cmd_cb(int argc, char *argv[], int fdout)
{
	lcd_write(*argv[1], 1);
	return 0;
}

const struct cli_cmd_t attr_cli cli_hd = {
	.narg = 1,
	.cmd = hd_cmd_cb,
	.name = "hd",
	.descr = "Hitachi Data",
};
