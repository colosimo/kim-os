/*
 * Author: Aurelio Colosimo, 2020
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* I2C driver */

#include <kim-io.h>
#include <kim-io-defs.h>
#include <reg.h>
#include <i2c.h>
#include <linker.h>
#include <errcode.h>
#include <gpio.h>
#include <log.h>

#include <kim.h>

#define I2C_W(addr)    ((addr << 1) & 0xFE)
#define I2C_R(addr)    ((addr << 1) | 0x01)

#define I2C_TOUT_MS 50
#define I2C_TOUT MS_TO_TICKS(I2C_TOUT_MS)

static int xfer_start(unsigned int b, const uint8_t addr, u8 minor)
{
	int ret = 0;
	u32 t;

	t = k_ticks();

	/* start condition */
	or32(R_I2C_CR1(b), BIT8);
	while ((rd32(R_I2C_SR1(b)) & BIT0) == 0 && k_elapsed(t) < I2C_TOUT);

	if ((rd32(R_I2C_SR1(b)) & BIT0) == 0) {
		ret = -ERRIO;
		err("%s %d\n", __func__, __LINE__);
		goto done;
	}

	/* address */
	wr32(R_I2C_DR(b), addr);
	while ((rd32(R_I2C_SR1(b)) & BIT1) == 0  && k_elapsed(t) < I2C_TOUT);

	if ((rd32(R_I2C_SR1(b)) & BIT1) == 0) {
		ret = -ERRIO;
		err("%s %d addr <%02x> not acked\n", __func__, __LINE__, addr);
		or32(R_I2C_CR1(b), BIT9);
		while (rd32(R_I2C_CR1(b)) & BIT9);
		goto done;
	}

	while ((rd32(R_I2C_SR2(b)) & BIT0) == 0  && k_elapsed(t) < I2C_TOUT);
	if ((rd32(R_I2C_SR2(b)) & BIT0) == 0) {
		ret = -ERRIO;
		err("%s %d dev not gone to master mode\n", __func__, __LINE__);
		goto done;
	}

done:
	return ret;
}

static int i2c_xfer(int fd, struct i2c_xfer_t *xfer)
{
	u8 minor = dev_minor(devs(fd)->id);
	unsigned int b;
	u32 tstart;
	int cnt = 0;
	int ret = 0;

	dbg("xfer len %d\n", xfer->len);

	switch (minor) {
		case MINOR_I2C1: b = I2C1; break;
		case MINOR_I2C2: b = I2C2; break;
		case MINOR_I2C3: b = I2C3; break;
		default: return -ERRINVAL;
	}

	or32(R_I2C_CR1(b), BIT0);

	if (xfer->dir == DIR_IN)
		ret = xfer_start(b, I2C_R(xfer->addr), minor);
	else
		ret = xfer_start(b, I2C_W(xfer->addr), minor);

	if (ret)
		goto done;

	tstart = k_ticks();

	if (xfer->dir == DIR_IN)
		or32(R_I2C_CR1(b), BIT10); /* FIXME needed? */

	while (cnt < xfer->len) {

		if (xfer->dir == DIR_IN) {
			if (cnt == xfer->len - 1)
				and32(R_I2C_CR1(b), ~BIT10); /* FIXME needed? */

			while (!(rd32(R_I2C_SR1(b)) & BIT6) &&
				k_elapsed(tstart) < I2C_TOUT);

			if (!(rd32(R_I2C_SR1(b)) & BIT6)) {
				err("%s %d i2c err sr1=%02x\n", __func__, cnt,
				    (uint)rd32(R_I2C_SR1(b)));
				ret = -ERRIO;
				goto done;
			}
			xfer->buf[cnt] = rd32(R_I2C_DR(b));
		}
		else {

			wr32(R_I2C_DR(b), xfer->buf[cnt]);

			while (!(rd32(R_I2C_SR1(b)) & BIT7) &&
				k_elapsed(tstart) < I2C_TOUT);

			if (!(rd32(R_I2C_SR1(b)) & BIT7)) {
				err("%s %d i2c err sr1=%02x\n", __func__, cnt,
				    (uint)rd32(R_I2C_SR1(b)));
				ret = -ERRIO;
				goto done;
			}
		}
		cnt++;
	}

	/* Generate a Stop */
	if (!xfer->nostop) {
		or32(R_I2C_CR1(b), BIT9);
		while (rd32(R_I2C_CR1(b)) & BIT9);
	}

done:
	and32(R_I2C_CR1(b), ~BIT0);
	return ret ? ret : cnt;
}

static int i2c_dev_ioctl(int fd, int cmd, void *buf, size_t count)
{
	switch (cmd) {
	case IOCTL_DEV_XFER:
		return i2c_xfer(fd, (struct i2c_xfer_t *)buf);
	default:
		break;
	}

	return -ERRINVAL;
}

static const k_drv_t attr_drvs i2c_dev_drv = {
	.maj = MAJ_SOC_I2C,
	.ioctl = i2c_dev_ioctl,
};
