/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* SPI driver */

#include <kim-io.h>
#include <kim-io-defs.h>
#include <reg.h>
#include <spi.h>
#include <linker.h>
#include <errcode.h>
#include <gpio.h>
#include <log.h>

#include <kim.h>

#define SPI_BUF_SIZE 256

#define spi_priv(fd) ((struct spi_data_t*)(devs(fd)->priv))

static u8 buf[3][SPI_BUF_SIZE];

int spi_dev_init(int fd)
{
	u8 min = dev_minor(devs(fd)->id);
	cbuf_init(&spi_priv(fd)->cbuf, buf[min], sizeof(buf[min]));
	return 0;
}

static int spi_dev_write(int fd, const void *buf, size_t len)
{
	unsigned i;
	volatile u32 *srr, *cr1r;
	volatile u8 *drr;
	int minor = dev_minor(devs(fd)->id);
	u8 c[len];
	int rd;

	/* Performs a "write-then-read" operation on spi */
	switch (minor) {
	case MINOR_SPI1:
		srr = R_SPI1_SR;
		drr = R_SPI1_DR;
		cr1r = R_SPI1_CR1;
		break;

	case MINOR_SPI2:
		srr = R_SPI2_SR;
		drr = R_SPI2_DR;
		cr1r = R_SPI2_CR1;
		break;

	case MINOR_SPI3:
		srr = R_SPI3_SR;
		drr = R_SPI3_DR;
		cr1r = R_SPI3_CR1;
		break;

	default:
		return -ERRINVAL;
	}

	or32(cr1r, BIT6);

	if (spi_priv(fd)->cs_io != IO_NULL)
		gpio_wr(spi_priv(fd)->cs_io, 0); /* manually handle CS (set to 0) */

	rd = 0;

	for (i = 0; i < len || rd < len; i++) {

		if (i < len) {
			while (rd32(srr) & BIT7);
			wr8(drr, ((u8*)buf)[i]); /* write */
		}

		while (!(rd32(srr) & BIT0));
		c[rd++] = rd8(drr) & 0xff;
	}

	if (spi_priv(fd)->cs_io != IO_NULL)
		gpio_wr(spi_priv(fd)->cs_io, 1); /* manually handle CS (set to 1) */

	and32(cr1r, ~BIT6); /* Disable peripheral */

	cbuf_write(&spi_priv(fd)->cbuf, c, len);

	return len;
}

static int spi_dev_avail(int fd)
{
	return cbuf_avail(&spi_priv(fd)->cbuf);
}

static int spi_dev_read(int fd, void *buf, size_t count)
{
	if (!buf || count < 0)
		return -ERRINVAL;

	return cbuf_read(&spi_priv(fd)->cbuf, buf, count);
}

static int spi_dev_xfer(int fd, struct spi_dev_xfer_t *xfer)
{
	int addr_discard = 1;
	int len;
	volatile u32 *srr, *cr1r;
	volatile u8 *drr;
	int i, rd;
	int minor = dev_minor(devs(fd)->id);
	volatile u8 c;
	u8 tmp;

	if (xfer->dir != DIR_IN)
		return -ERRINVAL; /* Unsupported yet */

	/* Performs a repeated read of same register set */
	switch (minor) {
	case MINOR_SPI1:
		srr = R_SPI1_SR;
		drr = R_SPI1_DR;
		cr1r = R_SPI1_CR1;
		break;

	case MINOR_SPI2:
		srr = R_SPI2_SR;
		drr = R_SPI2_DR;
		cr1r = R_SPI2_CR1;
		break;

	case MINOR_SPI3:
		srr = R_SPI3_SR;
		drr = R_SPI3_DR;
		cr1r = R_SPI3_CR1;
		break;

	default:
		return -ERRINVAL;
	}

	or32(cr1r, BIT6); /* Enable peripheral */

	rd = 0;

	len = xfer->count * (xfer->bs + 1);

	for (i = 0; i < len || rd < xfer->count * xfer->bs; i++) {

		if (i < len) {

			while (rd32(srr) & BIT7);

			if (i % (xfer->bs + 1) == 0) {

				if (xfer->fd_cs > 0) {
					tmp = 0;
					k_write(xfer->fd_cs, &tmp, 1);
				}

				wr8(drr, xfer->addr); /* write */
			}
			else
				wr8(drr, 0xff); /* write */

			if (i % (xfer->bs + 1) == xfer->bs) {
				if (xfer->fd_cs > 0) {
					tmp = 1;
					k_write(xfer->fd_cs, &tmp, 1);
				}
			}
		}

		while (!(rd32(srr) & BIT0));
		c = rd8(drr) & 0xff;

		if (addr_discard && (rd % xfer->bs == 0))
			addr_discard = 0;
		else {
			addr_discard = 1;
			xfer->buf[rd++] = c;
		}
	}

	and32(cr1r, ~BIT6); /* Disable peripheral */

	if (xfer->fd_cs > 0) {
		tmp = 1;
		k_write(xfer->fd_cs, &tmp, 1);
	}

	return 0;
}

static int spi_dev_ioctl(int fd, int cmd, void *buf, size_t count)
{
	switch (cmd) {

	case IOCTL_DEV_XFER:
		if (!buf || count < sizeof(struct spi_dev_xfer_t))
			return -ERRINVAL;

		return spi_dev_xfer(fd, (struct spi_dev_xfer_t *)buf);

	default:
		return -ERRINVAL;
	}
}

static const k_drv_t attr_drvs spi_dev_drv = {
	.maj = MAJ_SOC_SPI,
	.init = spi_dev_init,
	.read = spi_dev_read,
	.write = spi_dev_write,
	.avail = spi_dev_avail,
	.ioctl = spi_dev_ioctl,
};
