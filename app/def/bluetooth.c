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

#include "bluetooth.h"
#include "eeprom.h"
#include "def.h"

#define DEFAULT_BT_PIN 41111

static int fd_at_cmd, fd_bt_reset, fd_uart2;

static char buf[512];
static int bt_detect = 0;

static void bt_reset(u8 at_cmd, u16 id)
{
	u8 val;
	char *cmd;
	val = 0;
	char _name[64], _pin[64];

	k_delay(1000);
	k_write(fd_bt_reset, &val, 1);
	k_write(fd_at_cmd, &at_cmd, 1);
	val = 1;
	k_write(fd_bt_reset, &val, 1);
	k_delay(500);

	if (at_cmd) {
		k_sprintf(_pin, "AT+PSWD=%d\r\n", DEFAULT_BT_PIN);
		cmd = _pin;
		k_write(fd_uart2, cmd, strlen(cmd));
		k_delay(100);

		k_sprintf(_name, "AT+NAME=\"ELO %05d\"\r\n", id);
		cmd = _name;
		k_write(fd_uart2, cmd, strlen(cmd));
		k_delay(100);

		k_read(fd_uart2, buf, sizeof(buf));
	}
	else
		k_delay(500);
}

void bt_init()
{
	u32 id;
	eeprom_read(EEPROM_BLUETOOTH_ID, &id, 4);
	if (id > 0xffff) {
		id = 0;
		eeprom_write(EEPROM_BLUETOOTH_ID, &id, 4);
	}

	memset(buf, 0, sizeof(buf));
	fd_at_cmd = k_fd_byname("at_cmd");
	fd_bt_reset = k_fd_byname("bt_reset");
	fd_uart2 = k_fd_byname("uart2");

	if (fd_at_cmd < 0 || fd_bt_reset < 0 || fd_uart2 < 0)
		return;

	bt_reset(1, id);
	bt_reset(0, id);

	if (!strcmp(buf, "OK\r\nOK\r\n"))
		bt_detect = 1;
	else {
		log("BT NOT DETECTED, READ <%s>\n", buf);
		bt_detect = 0;
	}
}

int bt_present()
{
	return bt_detect;
}
