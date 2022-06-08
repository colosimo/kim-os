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

static void bt_reset(u16 id)
{
	u8 rst;
	u8 at_cmd;
	char *cmd;
	char _name[64], _pin[64];


	rst = 0;
	at_cmd = 1;
	k_write(fd_bt_reset, &rst, 1);
	k_delay(50);

	rst = 1;
	k_write(fd_bt_reset, &rst, 1);
	k_delay(1000);

	k_write(fd_at_cmd, &at_cmd, 1);
	k_sprintf(_pin, "AT+PSWD=%d\r\n", DEFAULT_BT_PIN);
	cmd = _pin;
	k_write(fd_uart2, cmd, strlen(cmd));
	k_delay(100);

	k_sprintf(_name, "AT+NAME=\"ELO %05d\"\r\n", id);
	cmd = _name;
	k_write(fd_uart2, cmd, strlen(cmd));
	k_delay(100);
	k_read(fd_uart2, buf, sizeof(buf));

	rst = 0;
	at_cmd = 0;
	k_write(fd_at_cmd, &at_cmd, 1);
	k_write(fd_bt_reset, &rst, 1);
	k_delay(50);

	rst = 1;
	k_write(fd_bt_reset, &rst, 1);
}

void bt_shutdown()
{
	u8 rst;
	if (fd_bt_reset >= 0) {
		rst = 0;
		k_write(fd_bt_reset, &rst, 1);
	}
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

	ant_check_enable(0);
	bt_reset(id);
	ant_check_enable(1);
}
