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

#define I2C_TOUT_MS 200
#define I2C_TOUT MS_TO_TICKS(I2C_TOUT_MS)

static int i2c_xfer(int fd, struct i2c_xfer_t *xfer)
{
	u8 minor = dev_minor(devs(fd)->id);
	unsigned int b;
	u32 tstart;
	int cnt;
	int timeout;
	u32 isr;

	dbg("xfer len %d\n", xfer->len);

	switch (minor) {
		case MINOR_I2C1: b = I2C1; break;
		case MINOR_I2C2: b = I2C2; break;
		case MINOR_I2C3: b = I2C3; break;
		case MINOR_I2C4: b = I2C4; break;
		default: return -ERRINVAL;
	}

	and32(R_I2C_CR2(b), ~(0xff << 16));
	or32(R_I2C_CR2(b), xfer->len << 16);

	if (xfer->dir == DIR_IN) {
		or32(R_I2C_CR2(b), BIT10);
	}
	else {
		and32(R_I2C_CR2(b), ~BIT10);
	}

	and32(R_I2C_CR2(b), ~0x3ff);
	or32(R_I2C_CR2(b), ((u32)xfer->addr) << 1);

	/* Start */
	or32(R_I2C_CR2(b), BIT13);
	cnt = 0;

	while (cnt < xfer->len) {

		tstart = k_ticks();

		if (xfer->dir == DIR_OUT)
			wr32(R_I2C_TXDR(b), xfer->buf[cnt]);


		tstart = k_ticks();
		while(rd32(R_I2C_CR2(b)) & BIT13 && k_elapsed(tstart) < I2C_TOUT);

		if ((rd32(R_I2C_CR2(b)) & BIT13) != 0) {
			or32(R_I2C_CR2(b), BIT14);
			err("%s %d\n", __func__, __LINE__);
			goto done;
		}

		timeout = 0;
		tstart = k_ticks();

		if (xfer->dir == DIR_IN) {

			while (((rd32(R_I2C_ISR(b)) & BIT2) == 0) &&
			    k_elapsed(tstart) < I2C_TOUT);

			if (rd32(R_I2C_ISR(b)) & BIT2)
				xfer->buf[cnt] = rd32(R_I2C_RXDR(b));
			else
			    timeout = 1;

		}
		else {
			while (((rd32(R_I2C_ISR(b)) & BIT0) == 0) &&
			    k_elapsed(tstart) < I2C_TOUT);

			isr = rd32(R_I2C_ISR(b));

			if (isr & BIT4) {
				err("%s %d\n", __func__, __LINE__);
				goto done;
			}
			else if ((isr & BIT0) == 0)
			    timeout = 1;
		}

		if (timeout) {
			err("%s %d\n", __func__, __LINE__);
			goto done;
		}
		cnt++;
	}

	tstart = k_ticks();

	while (((rd32(R_I2C_ISR(b)) & BIT6) == 0) &&
	    k_elapsed(tstart) < I2C_TOUT);

	/* Stop */
	if (!xfer->nostop)
		or32(R_I2C_CR2(b), BIT14);

	return cnt;

done:
	return -ERRIO;
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
