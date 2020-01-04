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
#include <cbuf.h>
#include <linker.h>
#include <errcode.h>
#include <gpio.h>
#include <log.h>

#include <kim.h>

#define SPI_BUF_SIZE 32

#define spi_priv(fd) ((struct spi_data_t*)(devs(fd)->priv))

static struct cbuf_t spi_cbuf[3]; /* FIXME: ugly, better move buffers to
    spi_data_t (see spi.h)*/
static u8 buf1[SPI_BUF_SIZE];
static u8 buf2[SPI_BUF_SIZE];
static u8 buf3[SPI_BUF_SIZE];

int spi_dev_init(int fd)
{
	/* FIXME: very ugly */
	switch (dev_minor(devs(fd)->id)) {
		case 0: cbuf_init(&spi_cbuf[0], buf1, sizeof(buf1)); break;
		case 1: cbuf_init(&spi_cbuf[1], buf2, sizeof(buf2)); break;
		case 2: cbuf_init(&spi_cbuf[2], buf3, sizeof(buf3)); break;
	}
	return 0;
}

static int spi_dev_write(int fd, const void *buf, size_t len)
{
	unsigned i;
	volatile u32 *srr, *cr1r;
	volatile u8 *drr;
	volatile u32 sr;
	int minor = dev_minor(devs(fd)->id);
	u8 c;

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

	if (spi_priv(fd)->cs_io != IO_NULL)
		gpio_wr(spi_priv(fd)->cs_io, 0); /* manually handle CS (set to 0) */
	or32(cr1r, BIT6);

	for (i = 0; i < len; i++) {

		wr8(drr, ((u8*)buf)[i]); /* write */

		while (1) {
			sr = rd32(srr);
			if (!(sr & BIT7) && (sr & BIT1))
				break;
		}

		c = rd8(drr) & 0xff;
		while((rd32(srr) & BIT0)); /* read */
		cbuf_write(&spi_cbuf[minor], &c, 1);
	}

	if (spi_priv(fd)->cs_io != IO_NULL)
		gpio_wr(spi_priv(fd)->cs_io, 1); /* manually handle CS (set to 1) */

	while (rd32(srr) & (0b11 << 11));
	while (rd32(srr) & BIT7);
	and32(cr1r, ~BIT6); /* Disable peripheral */
	while (rd32(srr) & (0b11 << 9));

	return len;
}

static int spi_dev_avail(int fd)
{
	int minor = dev_minor(devs(fd)->id);

	if (minor > array_size(spi_cbuf))
		return -ERRINVAL;
	return cbuf_avail(&spi_cbuf[minor]);
}

static int spi_dev_read(int fd, void *buf, size_t count)
{
	int minor = dev_minor(devs(fd)->id);

	if (minor > array_size(spi_cbuf))
		return -ERRINVAL;

	if (!buf || count < 0)
		return -ERRINVAL;

	return cbuf_read(&spi_cbuf[minor], buf, count);
}

static const k_drv_t attr_drvs spi_dev_drv = {
	.maj = MAJ_SOC_SPI,
	.init = spi_dev_init,
	.read = spi_dev_read,
	.write = spi_dev_write,
	.avail = spi_dev_avail,
};
