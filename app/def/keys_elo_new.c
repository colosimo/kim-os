/*
 * Author: Aurelio Colosimo, 2023
 * Copyright: Elo System srl
 */

#include <stdint.h>
#include <log.h>
#include <kim.h>
#include <linker.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <errcode.h>

#include <gpio.h>

#include "keys.h"
#include "def.h"

static int fd_cap1296;
static const u8 keys_cap1296_idx[4] = {0, 4, 1, 3};
static int last_key_off[4];
static u8 keys_stat[4];
static u32 keys_evts;
static int long_evt[4];

#define CAP1296_I2C_ADDR_7BIT 0b0101000

static int cap1296_read(int i2c_fd, u8 reg, void *val, int _cnt)
{
	int ret;

	struct i2c_xfer_t xfer = {
			.addr = CAP1296_I2C_ADDR_7BIT, /* 7bit address */
			.buf = &reg,
			.len = 1,
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

int cap1296_write(int i2c_fd, u8 reg, void *val, int _cnt)
{
	u8 data[1 + _cnt];
	int ret;

	data[0] = reg;

	memcpy(&data[1], val, _cnt);

	struct i2c_xfer_t xfer = {
			.addr = CAP1296_I2C_ADDR_7BIT, /* 7bit address */
			.buf = data,
			.len = 1 + _cnt,
			.dir = I2C_DIR_OUT,
			.nostop = 0,
	};

	/* Write addr + data*/
	ret = k_ioctl(i2c_fd, IOCTL_DEV_XFER, &xfer, sizeof(xfer));

	if (ret <= 0)
		return ret;

	return ret - 1;
}

void keys_start(struct task_t *t)
{
	u8 v[256];

	fd_cap1296 = k_fd_byname("i2c3");
	if (fd_cap1296 < 0) {
		err("Could not open i2c3\n");
		task_done(t);
		return;
	}

	v[0] = 0b01000100;
	cap1296_write(fd_cap1296, 0x00, &v, 1);

	v[0] = 0;
	cap1296_write(fd_cap1296, 0x2A, &v, 1); /* allow multiple touches */

	v[0] = 0x2F;
	cap1296_write(fd_cap1296, 0x1F, &v, 1); /* sensitivity 0x2f (=32x) */

	v[0] = 0;
	cap1296_write(fd_cap1296, 0x28, &v, 1); /* disable repeat interrupts on all pins */

	v[0] = 0x3b;
	cap1296_write(fd_cap1296, 0x29, &v, 1); /* enable signal guard */

	v[0] = 0xFF;
	cap1296_write(fd_cap1296, 0x26, &v, 1); /* force calibration */
}

void keys_step(struct task_t *t)
{
	u8 v, intr;
	int i;
	u8 tmp[4];
	u32 ticks;
	int fast_key;
	int rd;

	ticks = k_ticks();

	if (gpio_rd(IO(PORTA, 12)) == 0) {
		intr = 0b01000100;
		cap1296_write(fd_cap1296, 0x00, &intr, 1);
	}

	rd = cap1296_read(fd_cap1296, 0x3, &v, 1);
	if (rd < 1) {
		err("cap1296 read failed, force to 0\n");
		v = 0;
	}

	for (i = KEY_UP; i <= KEY_ENTER; i++) {

		tmp[i] = (v & (1 << keys_cap1296_idx[i])) ? 1 : 0;

		if (!tmp[i])
			last_key_off[i] = ticks;

		if (k_elapsed(last_key_off[i]) > MS_TO_TICKS(2000))
			long_evt[i] = 1;
		else
			long_evt[i] = 0;

		if (i <= KEY_DOWN && tmp[i] &&
		    k_elapsed(last_key_off[i]) > MS_TO_TICKS(1000))
			fast_key = 1;
		else
			fast_key = 0;

		if (tmp[i] && (fast_key || !keys_stat[i])) {
			rearm_standby();
			if (get_standby()) {
				if (i != KEY_ESC)
					set_standby(0);
			}
			else
				keys_evts |= (1 << i);
		}

		keys_stat[i] = tmp[i];
	}
}

int keys_is_long_evt(int key)
{
	int ret;
	if (key > KEY_ENTER)
		return 0;

	ret = keys_stat[key] && long_evt[key];
	if (ret) {
		last_key_off[key] = 0;
		long_evt[key] = 0;
	}
	return ret;
}

struct task_t attr_tasks task_keys = {
	.start = keys_start,
	.step = keys_step,
	.intvl_ms = 150,
	.name = "keys",
};

u32 keys_get_evts(void)
{
	return keys_evts;
}

void keys_clear_evts(u32 evts)
{
	keys_evts &= ~evts;
}

u8 keys_get_stat(int id)
{
	if (id <= KEY_ENTER)
		return keys_stat[id];
	return 0;
}
