/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <kim.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <log.h>
#include <errcode.h>

#include "eeprom.h"

#define EEPROM_I2C_ADDR_7BIT 0b1010110

int i2c_fd;

void eeprom_init(void)
{
	/* FIXME Check eeprom integrity with checksum, especially in key values */
	i2c_fd = k_fd_byname("i2c1");
}

int eeprom_read(u16 rndm_addr, u8 *val, int _cnt)
{
	int ret;
	u8 addr[2];

	addr[0] = (rndm_addr >> 8) & 0xff;
	addr[1] = rndm_addr & 0xff;

	struct i2c_xfer_t xfer = {
			.addr = EEPROM_I2C_ADDR_7BIT, /* 7bit address */
			.buf = addr,
			.len = 2,
			.dir = I2C_DIR_OUT,
			.nostop = 1,
	};

	/* Write 16-bit address */
	ret = k_ioctl(i2c_fd, IOCTL_DEV_XFER, &xfer, sizeof(xfer));

	if (ret < 0)
			return ret;

	/* Read data */
	xfer.buf = val;
	xfer.len = _cnt;
	xfer.dir = I2C_DIR_IN;
	xfer.nostop = 0;

	return k_ioctl(i2c_fd, IOCTL_DEV_XFER, &xfer, sizeof(xfer));
}

int eeprom_write(u16 rndm_addr, u8 *val, int _cnt)
{
	u8 data[2 + _cnt];

	data[0] = (rndm_addr >> 8) & 0xff;
	data[1] = rndm_addr & 0xff;
	memcpy(&data[2], val, _cnt);

	struct i2c_xfer_t xfer = {
			.addr = EEPROM_I2C_ADDR_7BIT, /* 7bit address */
			.buf = data,
			.len = 2 + _cnt,
			.dir = I2C_DIR_OUT,
			.nostop = 0,
	};

	/* Write addr + data*/
	return k_ioctl(i2c_fd, IOCTL_DEV_XFER, &xfer, sizeof(xfer));
}

static int e2p_r_cmd_cb(int argc, char *argv[], int fdout)
{
	u8 buf[64];
	char str[32];
	int ret;
	u32 count;
	u32 start;

	start = atoi_hex(argv[1]);
	count = atoi(argv[2]);
	ret = eeprom_read(start, buf, min(sizeof(buf), count));
	if (ret != count)
		return -ERRIO;

	k_sprintf(str, "EEPROM content from %04x\n", (uint)start);

	k_dumphex(str, buf, count);

	return 0;
}

const struct cli_cmd_t attr_cli cli_e2p_r = {
	.narg = 2,
	.cmd = e2p_r_cmd_cb,
	.name = "e2p_r",
	.descr = "Read EEPROM: e2p_r <addr> <count>",
};
