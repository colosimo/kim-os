/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <kim.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <log.h>
#include <errcode.h>

#include "db.h"
#include "pwm.h"
#include "eeprom.h"
#include "lcd.h"
#include "osm.h"

#define EEPROM_I2C_ADDR_7BIT 0b1010110

#define EEPROM_SIGN     "ELOS"
#define EEPROM_FMT_VER  3

static u8 pwm_def_freq[3] = {200, 100, 80};
static u8 pwm_def_duty[3] = {5, 5, 5};

static struct osm_cfg_t osm_def_cfg =
    {.enable = 1, .volt_perc = 80, .freq = 100, .duty = 50};

int i2c_fd;

void eeprom_reset(void)
{
	u32 tmp = 0xdeadbeaf;
	u8 tmp8;
	struct pwm_cfg_t p;
	int m;
	u8 daily_avg;

	log("%s\n", __func__);

	lcd_set_backlight(1);
	lcd_write_line("EEPROM RESET...", 0, 1);

	/* Invalidate signature (in order to recover in case it reboots during reset) */
	eeprom_write(EEPROM_SIGN_ADDR, &tmp, 4);

	/* Reset hours */
	tmp = 0;
	eeprom_write(EEPROM_HOURS_ADDR, &tmp, sizeof(tmp));

	/* Reset daily average */
	daily_avg = 1;
	eeprom_write(EEPROM_ENABLE_DAILY_AVG, &daily_avg, 1);

	/* Reset Bluetooth id */
	tmp = 0;
	eeprom_write(EEPROM_BLUETOOTH_ID, &tmp, sizeof(tmp));

	/* Reset PWM */
	m = 0;
	eeprom_write(EEPROM_PWM_CURRENT_MODE_ADDR, &m, 1);

	for (m = 0; m < 3; m++) {
		p.freq = pwm_def_freq[m];
		p.duty = pwm_def_duty[m];
		eeprom_write(EEPROM_PWM_MODE0_ADDR + m * sizeof(p), (u8*)&p, sizeof(p));
	}

	tmp = 1;
	eeprom_write(EEPROM_PWM_ROL_DAYS_SETTING_ADDR, &tmp, sizeof(tmp));

	/* Reset OSM */
	eeprom_write(EEPROM_OSM_CH1_CFG, &osm_def_cfg, sizeof(osm_def_cfg));
	eeprom_write(EEPROM_OSM_CH2_CFG, &osm_def_cfg, sizeof(osm_def_cfg));

	/* Write Format Version */
	tmp = EEPROM_FMT_VER;
	eeprom_write(EEPROM_FMT_VER_ADDR, &tmp, sizeof(tmp));

	/* Reset function mode flags */
	tmp8 = 1;
	eeprom_write(EEPROM_ENABLE_DEF_OUT, &tmp8, 1);
	tmp8 = 1;
	eeprom_write(EEPROM_ENABLE_OSM, &tmp8, 1);
	tmp8 = 70;
	eeprom_write(EEPROM_T_MAX, &tmp8, 1);

	/* Reset alarms */
	db_alarm_reset();

	/* Reset avvii */
	db_avvii_reset();

	/* Reset data */
	db_data_reset(1);

	/* Everything is ok, now write signature */
	eeprom_write(EEPROM_SIGN_ADDR, EEPROM_SIGN, 4);

	/* Reset Last seen on */
	tmp = ~0;
	eeprom_write(EEPROM_LAST_SEEN_ON_RTC, &tmp, sizeof(tmp));
	eeprom_write(EEPROM_LAST_SEEN_ON_RTC2, &tmp, sizeof(tmp));

#if 0 /* FIXME useful? */
	lcd_set_backlight(0);
#endif
}

void eeprom_init(void)
{
	u32 tmp;
	i2c_fd = k_fd_byname("i2c1");

	eeprom_read(EEPROM_SIGN_ADDR, &tmp, 4);
	if (tmp != *((u32*)EEPROM_SIGN))
		eeprom_reset();
	eeprom_read(EEPROM_FMT_VER_ADDR, &tmp, 4);
	if (tmp != EEPROM_FMT_VER)
		eeprom_reset();
}

int eeprom_read(u16 rndm_addr, void *val, int _cnt)
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

int eeprom_write(u16 rndm_addr, void *val, int _cnt)
{
	u8 data[2 + _cnt];
	int ret;

	if (rndm_addr / 64 != (rndm_addr + _cnt - 1) / 64)
		err("%s page overflow %04x %d\n", __func__, rndm_addr, _cnt);

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
	ret = k_ioctl(i2c_fd, IOCTL_DEV_XFER, &xfer, sizeof(xfer));
	k_delay_us(10000); /* FIXME: substitute with acknowledge polling (datasheet §7.0, page  12) */
	if (ret <= 0)
		return ret;
	return ret - 2;
}

static int e2p_r_cmd_cb(int argc, char *argv[], int fdout)
{
	u8 buf[128];
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

static int e2p_w_cmd_cb(int argc, char *argv[], int fdout)
{
	u8 buf[128];
	int i, ret;
	u32 count;
	u32 start;

	start = atoi_hex(argv[1]);
	count = min(argc - 2, sizeof(buf));

	for (i = 0; i < count; i++)
		buf[i] = atoi_hex(argv[i + 2]);

	ret = eeprom_write(start, buf, count);
	if (ret != count)
		return -ERRIO;
	return 0;

}

const struct cli_cmd_t attr_cli cli_e2p_w = {
	.narg = 2,
	.cmd = e2p_w_cmd_cb,
	.name = "e2p_w",
	.descr = "Write EEPROM: e2p_w <addr> <d1> <d2> ...",
};
